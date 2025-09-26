#ifndef CLIENT_HPP
#define CLIENT_HPP

class Client {
public:
    void connectToServer(const char* ip, int port);
    void sendMessage(const char* msg);
    void receiveLoop();
};

#endif // CLIENT_HPP
