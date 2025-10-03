#ifndef TSLOG_HPP
#define TSLOG_HPP

#include <string>

namespace tslog {
    // Inicializa o logger com o nome de arquivo especificado.
    void init(const std::string& filename = "chat_log.txt");

    // Registra uma mensagem no log.
    void log(const std::string& msg);

    // Fecha o arquivo de log.
    void close();
}

#endif // TSLOG_HPP
