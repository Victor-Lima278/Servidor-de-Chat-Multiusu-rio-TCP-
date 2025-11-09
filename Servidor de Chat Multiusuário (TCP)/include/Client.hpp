#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <string>

class Client {
public:
    Client(const std::string& ip, int port);
    ~Client();

    void start();

private:
    int sock;
    std::string server_ip;
    int server_port;

    void sendLoop();
    void receiveLoop();
};

#endif // CLIENT_HPP
