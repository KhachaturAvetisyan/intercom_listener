# MYSQL_CONCPP_DIR = /usr
# CPPFLAGS = -I $(MYSQL_CONCPP_DIR)/include/cppconn -L $(MYSQL_CONCPP_DIR)/lib
LDLIBS = -lcurl -lpthread
# CXXFLAGS = -std=c++11
CLIENT_DIR = client/
SERVER_DIR = server/

all: server.out client.out

server.out : $(SERVER_DIR)server.cpp $(SERVER_DIR)device.cpp
	g++ $^ $(LDLIBS) -o $@

client.out : $(CLIENT_DIR)client.cpp
	g++ $^ -o $@

clean:
	rm *.out