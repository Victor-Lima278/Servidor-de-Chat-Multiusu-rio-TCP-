#include "Server.hpp"
#include "tslog.hpp"
#include <iostream>
#include <algorithm>
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

Server::Server(int port) : port(port), running(false) {}

Server::~Server() {
    stop();
}

void Server::start() {
#ifdef _WIN32
    WSADATA wsaData;
    int iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
    if (iResult != 0) {
        throw std::runtime_error("WSAStartup falhou: " + std::to_string(iResult));
    }
#endif

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1) {
        throw std::runtime_error("Erro ao criar socket do servidor");
    }

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(server_fd, (sockaddr*)&addr, sizeof(addr)) < 0) {
        throw std::runtime_error("Erro no bind");
    }

    if (listen(server_fd, 5) < 0) {
        throw std::runtime_error("Erro no listen");
    }

    running = true;
    tslog::log("Servidor iniciado na porta " + std::to_string(port));

    std::thread(&Server::acceptClients, this).detach();
}

void Server::stop() {
    running = false;
    CLOSESOCKET(server_fd);
#ifdef _WIN32
    WSACleanup();
#endif
    tslog::log("Servidor encerrado.");
}

void Server::acceptClients() {
    while (running) {
        sockaddr_in client_addr{};
        socklen_t len = sizeof(client_addr);
        int client_socket = accept(server_fd, (sockaddr*)&client_addr, &len);

        if (client_socket >= 0) {
            {
                std::lock_guard<std::mutex> lock(clients_mtx);
                clients.push_back(client_socket);
            }
            tslog::log("Novo cliente conectado.");
            std::thread(&Server::handleClient, this, client_socket).detach();
        }
    }
}

void Server::handleClient(int client_socket) {
    char buffer[1024];
    while (true) {
        int bytes = recv(client_socket, buffer, sizeof(buffer) - 1, 0);
        if (bytes <= 0) {
            tslog::log("Cliente desconectado.");
            CLOSESOCKET(client_socket);
            std::lock_guard<std::mutex> lock(clients_mtx);
            clients.erase(std::remove(clients.begin(), clients.end(), client_socket), clients.end());
            break;
        }

        buffer[bytes] = '\0';
        std::string msg(buffer);
        tslog::log("Mensagem recebida: " + msg);
        broadcast(msg, client_socket);
    }
}

void Server::broadcast(const std::string& msg, int sender_fd) {
    std::lock_guard<std::mutex> lock(clients_mtx);
    for (int client : clients) {
        if (client != sender_fd) {
            send(client, msg.c_str(), msg.size(), 0);
        }
    }
}