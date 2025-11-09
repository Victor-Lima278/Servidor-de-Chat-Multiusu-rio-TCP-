#!/bin/bash

echo "Iniciando servidor..."
./bin/server &
SERVER_PID=$!

# espera o servidor subir
sleep 2

echo "Iniciando cliente 1..."
(
    echo "Olá, eu sou o cliente 1!"
    echo "Como vai cliente 2?"
    sleep 2
    echo "/quit"
) | ./bin/client &

echo "Iniciando cliente 2..."
(
    echo "Oi, aqui é o cliente 2!"
    echo "Estou recebendo mensagens do cliente 1!"
    sleep 2
    echo "E você cliente 3?"
    sleep 2
    echo "/quit"
) | ./bin/client &

echo "Iniciando cliente 3..."
(
    echo "Salve! Eu sou o cliente 3!"
    echo "Confirmando que recebi a mensagem do cliente 2!"
    sleep 2
    echo "/quit"
) | ./bin/client &

# espera todos os clientes terminarem
wait

# encerra o servidor
kill $SERVER_PID

echo "Teste concluído. Verifique o chat_log.txt"
