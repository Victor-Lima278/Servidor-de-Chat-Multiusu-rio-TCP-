#ifndef LOGGER_HPP
#define LOGGER_HPP

#include <string>
#include <fstream>
#include <mutex>
#include <memory>

class Logger {
public:
    static Logger& getInstance();

    void log(const std::string& msg);

private:
    Logger();   // construtor privado (singleton)
    ~Logger();

    std::ofstream logFile;
    std::mutex mtx;
};

#endif // LOGGER_HPP
