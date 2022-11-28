# include <iostream>
# include <netinet/in.h>
# include <stdio.h>
# include <stdlib.h>
# include <string>
# include <string.h>
# include <sys/socket.h>
# include <unistd.h>
# include <thread>
# include <vector>
# include <ctime>

# include "send_read.cpp"

# include "mysql_connection.h"

# include <cppconn/driver.h>
# include <cppconn/exception.h>
# include <cppconn/resultset.h>
# include <cppconn/statement.h>
# include <cppconn/prepared_statement.h>

# define PORT 8080

std::string DEV_T = "Dev";
std::string USERS_T = "Users";
std::string RFID_T = "Rfid";


typedef struct 
{
  uint32_t startword;
  char imei[15];
  uint8_t checksum;
}init_struct;


void update_time(sql::Connection* con, std::string devid)
{
        sql::Statement *declaration;
        time_t current_time;
        time(&current_time);

        declaration = con->createStatement();
        declaration->executeUpdate("UPDATE " + DEV_T + " SET time = '" + std::to_string(current_time) + "', upd_f = 0 WHERE id = '" + devid + "'");
        
        delete declaration;
}


void thread_func(int client_socket, sql::Connection* con)
{
    // struct init
    init_struct* init_s;
    init_s = (init_struct*)malloc(sizeof(init_struct));

    read(client_socket, init_s, 20);

    if (init_s->startword != 0x11223344)
    {
        std::cout << "start word error\n";
        // closing the connected socket
        std::cout << "close client\n";
        close(client_socket);
        return;
    }
    
    uint8_t loc_checksum = 0;
    for (int i = 0; i < 19; ++i)
        loc_checksum ^= ((uint8_t*)init_s)[i];

    if (loc_checksum != init_s->checksum)
    {
        std::cout << "checksum error\n";
        // closing the connected socket
        std::cout << "close client\n";
        close(client_socket);
        return;
    }
    
    std::string imei;
    for (int i = 0; i < 15; ++i)
        imei += init_s->imei[i];

    std::cout << imei << "\n";


    // check imei
    sql::ResultSet  *res;
    sql::Statement *stmt;

    con->setSchema("dom_test_db");
    stmt = con->createStatement();

    std::string result;
    res = stmt->executeQuery("SELECT id FROM " + DEV_T + " WHERE imei = " + imei);

    while (res->next())
    {
        result = res->getString("id");

        send_msg_socket(client_socket, result.c_str());
        std::cout << "send message : " << result << "\n";
    }

    if (result == "")
    {
        send_msg_socket(client_socket, "id dosent exist");
        std::cout << "send message : " << "id dosent exist" << "\n";
    }


    delete res;
    delete stmt;

    // closing the connected socket
    std::cout << "close client\n";
    close(client_socket);
}


int main()
{
    // mysql connection
    sql::Driver *driver;
    sql::Connection *con;

    try 
    {
        driver = get_driver_instance();
        con = driver->connect("tcp://127.0.0.1:3306", "root", "1111");
    } 
    catch (sql::SQLException &e) 
    {
        perror("mysql");
        exit(EXIT_FAILURE);
    }

    // server init
    int server_fd, new_socket, valread;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);

    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) 
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // Forcefully attaching socket to the port 8080
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) 
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Forcefully attaching socket to the port 8080
    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) 
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    std::cout << "binding access!\n";
    std::vector<std::thread> threads;

    while(1)
    {
        std::cout << "listening\n";
        if (listen(server_fd, 1) < 0) 
        {
            perror("listen");
            exit(EXIT_FAILURE);
        }

        std::cout << "accept\n";
        if ((new_socket = accept(server_fd, (struct sockaddr*)&address, (socklen_t*)&addrlen)) < 0) 
        {
            perror("accept");
            exit(EXIT_FAILURE);
        }

        threads.push_back(std::thread(thread_func, new_socket, con));

    }


    delete con;
    // closing the listening socket
    shutdown(server_fd, SHUT_RDWR);
    return 0;
}

