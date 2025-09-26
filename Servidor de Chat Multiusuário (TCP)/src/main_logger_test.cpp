#include "Logger.hpp"
#include <thread>
#include <vector>
#include <iostream>

void worker(int id) {
    for (int i = 0; i < 100; i++) {
        Logger::getInstance().log("Thread " + std::to_string(id) + " mensagem " + std::to_string(i));
    }
}

int main() {
    std::vector<std::thread> threads;

    for (int i = 0; i < 4; i++) {
        threads.emplace_back(worker, i);
    }

    for (auto& t : threads) {
        t.join();
    }

    std::cout << "Logs gerados em chat_log.txt" << std::endl;
    return 0;
}
