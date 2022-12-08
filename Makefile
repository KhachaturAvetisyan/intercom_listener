CC = g++
CFLAGS = -lcurl

# LDLIBS = -lcurl -lpthread

CLIENT_DIR = client/
SERVER_DIR = server/

OBJ_DIR = build/
SERV_SRC_DIR = $(SERVER_DIR)src/
SERV_INCLUDE = -I $(SERVER_DIR)includes/

SERV_SRC = $(wildcard $(SERV_SRC_DIR)*.cpp)
SERV_OBJ = $(patsubst %.cpp, $(OBJ_DIR)%.o, $(notdir $(SERV_SRC)))


all: make_dir server.out client.out

make_dir : 
	mkdir -p $(OBJ_DIR)

server.out :  $(OBJ_DIR)server.o $(SERV_OBJ)
	$(CC) $(SERV_INCLUDE) $^ $(CFLAGS) -o $@
$(OBJ_DIR)%.o : $(SERV_SRC_DIR)%.cpp
	$(CC) -c $< -o $@
$(OBJ_DIR)server.o : $(SERVER_DIR)server.cpp
	$(CC) -c $^ -o $@

client.out : $(OBJ_DIR)client.o
	$(CC) $^ -o $@
$(OBJ_DIR)client.o : $(CLIENT_DIR)client.cpp
	$(CC) -c $^ -o $@

clean:
	rm -rf *.out $(OBJ_DIR)