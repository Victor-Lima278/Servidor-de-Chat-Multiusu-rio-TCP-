Esta seção detalha como os requisitos gerais de programação concorrente, especificados no documento do projeto, foram implementados no código-fonte.

1. Threads: O projeto utiliza std::thread para a programação concorrente, conforme o requisito.

Servidor: No Server.cpp, uma nova thread é criada para cada cliente aceito, garantindo que o servidor possa lidar com múltiplas conexões simultaneamente sem bloquear.

Código: std::thread(&Server::handleClient, this, client_socket).detach(); em Server::acceptClients().

Cliente: No Client.cpp, uma thread separada (recv_thread) é iniciada para receber mensagens, permitindo que o usuário envie e receba mensagens de forma não bloqueante.

Código: std::thread recv_thread(&Client::receiveLoop, this); em Client::start().

2. Exclusão Mútua: std::mutex foi utilizado para proteger o acesso a dados compartilhados, evitando condições de corrida, como especificado.

Lista de Clientes: Em Server.hpp, o vetor clients é protegido por clients_mtx. O std::lock_guard é usado em Server.cpp para garantir acesso seguro ao adicionar/remover clientes e durante o broadcast.

Histórico de Mensagens: O vetor message_history é protegido pelo history_mtx para garantir que a adição e leitura do histórico sejam atômicas.

Arquivo de Log: Na biblioteca tslog.cpp, o acesso ao arquivo logFile é protegido por um std::mutex (mtx) para serializar as operações de escrita de múltiplas threads.

3. Monitores: O conceito de monitor, que encapsula dados e sua sincronização, foi aplicado na biblioteca tslog.

Código: O namespace tslog e suas funções init, log e close atuam como um monitor, abstraindo a complexidade da sincronização (std::mutex) do restante da aplicação.

4. Sockets: A comunicação em rede foi implementada utilizando a API de Sockets, conforme o requisito.

Servidor: Server.cpp utiliza socket, bind, listen e accept para conexões TCP.

Cliente: Client.cpp utiliza socket e connect para estabelecer a comunicação.

5. Tratamento de Erros: Blocos try...catch são usados nos arquivos main_server.cpp e main_client.cpp para capturar exceções std::runtime_error e registrar os erros com tslog.

6. Logging Concorrente: O uso da biblioteca libtslog é um requisito central, e chamadas a tslog::log() estão presentes em todos os pontos importantes da execução.pdf, Servidor de Chat Multiusuário (TCP)/src/main_client.cpp, Servidor de Chat Multiusuário (TCP)/src/main_server.cpp, Servidor de Chat Multiusuário (TCP)/src/Server.cpp, Servidor de Chat Multiusuário (TCP)/src/Client.cpp].

7. Build: Um Makefile funcional foi criado para compilar o servidor e o cliente, atendendo ao requisito.