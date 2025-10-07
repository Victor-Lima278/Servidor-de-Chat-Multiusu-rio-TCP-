#ifndef SERVER_HPP
#define SERVER_HPP

#include <vector>
#include <thread>
#include <mutex>
#include <string>
#include <semaphore>
#include <atomic> // <<< ADICIONADO: Header para o contador atômico

class Server {
public:
    Server(int port);
    ~Server();

    void start();
    void stop();

private:
    int server_fd;
    int port;
    bool running;

    std::vector<int> clients;
    std::mutex clients_mtx;

    std::vector<std::string> message_history;
    std::mutex history_mtx;
    const int HISTORY_LINES = 10;

    static const int MAX_CLIENTS = 5;
    std::counting_semaphore<> client_slots{MAX_CLIENTS};

    // --- ADICIONADO PARA IDENTIFICAR CLIENTES ---
    std::atomic<int> next_client_id{1};
    // ------------------------------------------

    void acceptClients();
    // --- ALTERADO: para aceitar o ID do cliente ---
    void handleClient(int client_socket, int client_id);
    // --------------------------------------------
    void broadcast(const std::string& msg, int sender_fd);
};

#endif // SERVER_HPP
