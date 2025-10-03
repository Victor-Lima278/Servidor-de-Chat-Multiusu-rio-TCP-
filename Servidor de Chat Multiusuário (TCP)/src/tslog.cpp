#include "tslog.hpp"
#include <iostream>
#include <fstream>
#include <mutex>
#include <chrono>
#include <ctime>
#include <thread>
#include <stdexcept>

namespace { // Namespace anônimo para encapsulamento
    std::ofstream logFile;
    std::mutex mtx;
    bool initialized = false;
}

namespace tslog {

void init(const std::string& filename) {
    if (initialized) return;
    logFile.open(filename, std::ios::app);
    if (!logFile.is_open()) {
        throw std::runtime_error("Não foi possível abrir o arquivo de log: " + filename);
    }
    initialized = true;
}

void close() {
    if (initialized && logFile.is_open()) {
        logFile.close();
        initialized = false;
    }
}

void log(const std::string& msg) {
    if (!initialized) {
        // Auto-inicializa para garantir que o log funcione mesmo se init() não for chamado.
        init();
    }
    
    std::lock_guard<std::mutex> lock(mtx);

    auto now = std::chrono::system_clock::now();
    std::time_t t = std::chrono::system_clock::to_time_t(now);
    char buf[32];
    #ifdef _WIN32
        struct tm timeinfo;
        localtime_s(&timeinfo, &t);
        std::strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", &timeinfo);
    #else
        std::strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", std::localtime(&t));
    #endif


    auto tid = std::this_thread::get_id();

    logFile << "----------------------------------------\n";
    logFile << "[" << buf << "] "
            << "(Thread " << tid << ") "
            << msg << "\n";
    logFile.flush();
}

} // namespace tslog
