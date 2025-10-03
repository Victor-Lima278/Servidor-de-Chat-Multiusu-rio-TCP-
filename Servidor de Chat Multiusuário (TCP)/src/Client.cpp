#include "Server.hpp"
#include "tslog.hpp"
#include <iostream>
#include <csignal>
#include <atomic>

std::atomic<bool> running(true);

void signalHandler(int signum) {
    running = false;
}

int main() {
    // captura Ctrl+C (SIGINT) para encerrar servidor corretamente
    std::signal(SIGINT, signalHandler);

    try {
        tslog::init("chat_log.txt");

        Server server(8080);
        server.start();

        std::cout << "Servidor rodando na porta 8080. Pressione Ctrl+C para encerrar.\n";

        while (running) {
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }

        server.stop();
        tslog::close();
    } catch (const std::exception& e) {
        tslog::log("Erro no servidor: " + std::string(e.what()));
        std::cerr << "Erro no servidor: " << e.what() << std::endl;
        tslog::close();
    }

    return 0;
}
