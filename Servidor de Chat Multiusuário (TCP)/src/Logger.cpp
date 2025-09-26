#include "Logger.hpp"
#include <iostream>
#include <chrono>
#include <ctime>
#include <thread>

Logger::Logger() {
    logFile.open("chat_log.txt", std::ios::app);
    if (!logFile.is_open()) {
        throw std::runtime_error("Não foi possível abrir chat_log.txt");
    }
}

Logger::~Logger() {
    if (logFile.is_open()) {
        logFile.close();
    }
}

Logger& Logger::getInstance() {
    static Logger instance;
    return instance;
}

void Logger::log(const std::string& msg) {
    std::lock_guard<std::mutex> lock(mtx);

    // Data e hora formatada
    auto now = std::chrono::system_clock::now();
    std::time_t t = std::chrono::system_clock::to_time_t(now);
    char buf[32];
    std::strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", std::localtime(&t));

    // ID da thread
    auto tid = std::this_thread::get_id();

    // Escreve no log
    logFile << "----------------------------------------\n";
    logFile << "[" << buf << "] "
            << "(Thread " << tid << ") "
            << msg << "\n";
    logFile.flush();
}
