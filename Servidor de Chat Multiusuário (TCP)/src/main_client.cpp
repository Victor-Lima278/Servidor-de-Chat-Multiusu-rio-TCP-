#include "Client.hpp"
#include "tslog.hpp"
#include <iostream>

int main() {
    try {
        tslog::init("chat_log.txt");
        Client client("127.0.0.1", 8080);
        client.start();
        tslog::close();
    } catch (const std::exception& e) {
        tslog::log("Erro no cliente: " + std::string(e.what()));
        std::cerr << "Erro: " << e.what() << std::endl;
        tslog::close();
    }
}