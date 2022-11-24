# include <iostream>
# include <netinet/in.h>
# include <stdio.h>
# include <stdlib.h>
# include <string>
# include <string.h>
# include <sys/socket.h>
# include <unistd.h>

# include "send_read.cpp"

# include "mysql_connection.h"

# include <cppconn/driver.h>
# include <cppconn/exception.h>
# include <cppconn/resultset.h>
# include <cppconn/statement.h>
# include <cppconn/prepared_statement.h>

# define PORT 8080

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

    // while(1)
    // {
        if (listen(server_fd, 3) < 0) 
        {
            perror("listen");
            exit(EXIT_FAILURE);
        }

        if ((new_socket = accept(server_fd, (struct sockaddr*)&address, (socklen_t*)&addrlen)) < 0) 
        {
            perror("accept");
            exit(EXIT_FAILURE);
        }

        int id;
        read(new_socket, &id, 2);
        std::cout << "read id : " << id << "\n";

        con->setSchema("testdb");
        sql::ResultSet  *res;
        sql::Statement *stmt;
        stmt = con->createStatement();

        std::string query = "SELECT id, name FROM test_t WHERE id = ";
        query += std::to_string(id);

        std::string result;
        res = stmt->executeQuery(query);
        while (res->next())
        {
            // cout << "id = " << res->getInt(1) << endl;
            result = res->getString("name");
            std::cout << "name = " << result << std::endl;
        }

        send_msg_socket(new_socket, result.c_str());
        std::cout << "send message : " << result << "\n";



    // }


    delete res;
    delete stmt;
    delete con;
    // closing the connected socket
    close(new_socket);
    // closing the listening socket
    shutdown(server_fd, SHUT_RDWR);
    return 0;
}