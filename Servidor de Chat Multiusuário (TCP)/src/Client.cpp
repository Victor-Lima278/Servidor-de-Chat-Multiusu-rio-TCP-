#include "Client.hpp"
#include "tslog.hpp"
#include <iostream>
#include <thread>
#include <stdexcept>

#ifdef _WIN32
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #pragma comment(lib, "Ws2_32.lib")
    #define CLOSESOCKET closesocket
#else
    #include <unistd.h>
    #include <arpa/inet.h>
    #include <sys/socket.h>
    #define CLOSESOCKET close
#endif

Client::Client(const std::string& ip, int port) : server_ip(ip), server_port(port) {}

Client::~Client() {
    CLOSESOCKET(sock);
#ifdef _WIN32
    WSACleanup();
#endif
}

void Client::start() {
#ifdef _WIN32
    WSADATA wsaData;
    int iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
    if (iResult != 0) {
        throw std::runtime_error("WSAStartup falhou: " + std::to_string(iResult));
    }
#endif

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1) {
        throw std::runtime_error("Erro ao criar socket do cliente");
    }

    sockaddr_in server_addr{};
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(server_port);
    inet_pton(AF_INET, server_ip.c_str(), &server_addr.sin_addr);

    if (connect(sock, (sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        throw std::runtime_error("Erro ao conectar no servidor");
    }

    tslog::log("Conectado ao servidor " + server_ip + ":" + std::to_string(server_port));

    std::thread recv_thread(&Client::receiveLoop, this);
    sendLoop();
    recv_thread.join();
}

void Client::sendLoop() {
    std::string msg;
    while (true) {
        std::getline(std::cin, msg);
        if (msg == "/quit") break;
        send(sock, msg.c_str(), msg.size(), 0);
    }
}

void Client::receiveLoop() {
    char buffer[1024];
    while (true) {
        int bytes = recv(sock, buffer, sizeof(buffer) - 1, 0);
        if (bytes <= 0) break;
        buffer[bytes] = '\0';
        std::cout << ">> " << buffer << std::endl;
    }
}
