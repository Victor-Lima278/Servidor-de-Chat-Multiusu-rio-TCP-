# Compilador e flags
CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++20 -pthread
INCLUDES = -Iinclude

# Diretórios
SRC_DIR = src
OBJ_DIR = obj
BIN_DIR = bin

# Executáveis
TARGET_SERVER = $(BIN_DIR)/server
TARGET_CLIENT = $(BIN_DIR)/client

# Fontes
SRCS_TSLOG = $(SRC_DIR)/tslog.cpp
SRCS_SERVER = $(SRC_DIR)/main_server.cpp $(SRC_DIR)/Server.cpp $(SRCS_TSLOG)
SRCS_CLIENT = $(SRC_DIR)/main_client.cpp $(SRC_DIR)/Client.cpp $(SRCS_TSLOG)

# Objetos (transforma src/xxx.cpp em obj/xxx.o)
OBJS_SERVER = $(SRCS_SERVER:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)
OBJS_CLIENT = $(SRCS_CLIENT:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)

# Regras principais
all: dirs $(TARGET_SERVER) $(TARGET_CLIENT)

dirs:
	mkdir -p $(OBJ_DIR) $(BIN_DIR)

# Server
$(TARGET_SERVER): $(OBJS_SERVER)
	$(CXX) $(CXXFLAGS) -o $@ $^

# Client
$(TARGET_CLIENT): $(OBJS_CLIENT)
	$(CXX) $(CXXFLAGS) -o $@ $^

# Regra genérica para compilar qualquer .cpp em obj/
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | dirs
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# Limpeza
clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR) chat_log.txt

# Execuções rápidas
run-server: $(TARGET_SERVER)
	./$(TARGET_SERVER)

run-client: $(TARGET_CLIENT)
	./$(TARGET_CLIENT)
