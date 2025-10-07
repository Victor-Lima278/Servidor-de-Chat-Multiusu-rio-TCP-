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
    tslog::log("Limite de " + std::to_string(MAX_CLIENTS) + " clientes simultâneos.");

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
        client_slots.acquire();

        sockaddr_in client_addr{};
        socklen_t len = sizeof(client_addr);
        int client_socket = accept(server_fd, (sockaddr*)&client_addr, &len);

        if (client_socket >= 0) {
            // Pega um ID único para o novo cliente
            int client_id = next_client_id++;

            {
                std::lock_guard<std::mutex> lock(clients_mtx);
                clients.push_back(client_socket);
            }
            // Loga a conexão com o novo ID
            tslog::log("Cliente " + std::to_string(client_id) + " conectado.");
            
            // Passa o ID para a thread que vai cuidar do cliente
            std::thread(&Server::handleClient, this, client_socket, client_id).detach();
        } else {
            client_slots.release();
        }
    }
}

void Server::handleClient(int client_socket, int client_id) {
    // Envia o histórico de mensagens para o novo cliente
    {
        std::lock_guard<std::mutex> lock(history_mtx);
        if (!message_history.empty()) {
            std::string history_msg = "--- Histórico das últimas mensagens ---\n";
            for (const auto& msg : message_history) {
                history_msg += msg + "\n";
            }
            history_msg += "----------------------------------------\n";
            send(client_socket, history_msg.c_str(), history_msg.size(), 0);
        }
    }

    char buffer[1024];
    while (true) {
        int bytes = recv(client_socket, buffer, sizeof(buffer) - 1, 0);
        if (bytes <= 0) {
            // Loga a desconexão com o ID do cliente
            tslog::log("Cliente " + std::to_string(client_id) + " desconectado.");
            CLOSESOCKET(client_socket);
            std::lock_guard<std::mutex> lock(clients_mtx);
            clients.erase(std::remove(clients.begin(), clients.end(), client_socket), clients.end());
            
            client_slots.release();
            break;
        }

        buffer[bytes] = '\0';
        std::string msg(buffer);

        // Formata a mensagem para incluir o ID do cliente
        std::string formatted_msg = "Cliente " + std::to_string(client_id) + ": " + msg;
        
        // Loga a mensagem formatada
        tslog::log("Mensagem recebida do " + formatted_msg);
        
        // Envia a mensagem formatada para todos os outros clientes
        broadcast(formatted_msg, client_socket);
    }
}

void Server::broadcast(const std::string& msg, int sender_fd) {
    // Salva a mensagem já formatada no histórico
    {
        std::lock_guard<std::mutex> lock(history_mtx);
        message_history.push_back(msg);
        if (message_history.size() > HISTORY_LINES) {
            message_history.erase(message_history.begin());
        }
    }

    std::lock_guard<std::mutex> lock(clients_mtx);
    for (int client : clients) {
        // Envia a mensagem já com o "Cliente X:" na frente
        if (client != sender_fd) {
            send(client, msg.c_str(), msg.size(), 0);
        }
    }
}