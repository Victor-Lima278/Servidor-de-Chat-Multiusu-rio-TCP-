#ifndef SERVER_HPP
#define SERVER_HPP

#include <vector>
#include <thread>
#include <mutex>
#include <string>

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

    void acceptClients();
    void handleClient(int client_socket);
    void broadcast(const std::string& msg, int sender_fd);
};

#endif // SERVER_HPP
