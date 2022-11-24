MYSQL_CONCPP_DIR = /usr
CPPFLAGS = -I $(MYSQL_CONCPP_DIR)/include/cppconn -L $(MYSQL_CONCPP_DIR)/lib
LDLIBS = -lmysqlcppconn -lpthread
# CXXFLAGS = -std=c++11

all: server client

server : server.cpp

client : client.cpp
	g++ client.cpp -o client

clean:
	rm client server