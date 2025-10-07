#!/bin/bash

# Limpa o log antigo antes de começar
rm -f chat_log.txt

# Função para verificar e exibir a contagem de clientes conectados
check_client_count() {
    # Garante que o arquivo de log existe antes de tentar ler
    if [ -f "chat_log.txt" ]; then
        # Conta as linhas de conexão e desconexão no log
        connected=$(grep -c "Novo cliente conectado" chat_log.txt)
        disconnected=$(grep -c "Cliente desconectado" chat_log.txt)
        
        # Calcula o total atual
        current_clients=$((connected - disconnected))
        echo "--- STATUS: $current_clients cliente(s) online ---"
    fi
}

echo "Iniciando servidor em background..."
./bin/server &
SERVER_PID=$!

# Espera o servidor inicializar completamente
sleep 2
echo "--- Servidor iniciado ---"
check_client_count # Deve mostrar 0

# Loop para simular 6 clientes conectando e desconectando sequencialmente
for i in {1..6}
do
    echo ""
    echo "--> Cliente $i conectando..."
    (
        echo "Olá, sou o cliente $i."
        sleep 2
        echo "/quit"
    ) | ./bin/client &
    CLIENT_PID=$!

    # Espera um pouco para a conexão e o log serem escritos
    sleep 1

    # Verifica e exibe a contagem de clientes
    check_client_count

    # Espera o processo do cliente atual terminar (após o /quit)
    wait $CLIENT_PID

    echo "--> Cliente $i desconectou."
    
    # Pausa para o log de desconexão ser escrito e verifica a contagem novamente
    sleep 1
    check_client_count
done

# Encerra o servidor
echo ""
echo "Encerrando o servidor (PID: $SERVER_PID)..."
kill $SERVER_PID

sleep 1

echo ""
echo "Teste sequencial com 6 clientes concluído."
echo "Verifique o arquivo chat_log.txt para ver os logs completos."