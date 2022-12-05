# include "serv_include.h"


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
    // init poll struct
    struct pollfd mypoll = { client_socket, POLLIN|POLLPRI };

    // struct init
    init_struct* init_s;
    init_s = (init_struct*)malloc(sizeof(init_struct));

    if (poll(&mypoll, 1, 2000))
        read(client_socket, init_s, 20);
    else
    {
        std::cout << "read timeout\n";
        // closing the connected socket
        std::cout << "close client\n";
        close(client_socket);
        return;
    }

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

    std::string id;
    bool imei_flag = false;
    res = stmt->executeQuery("SELECT id FROM " + DEV_T + " WHERE imei = " + imei);

    while (res->next())
    {
        id = res->getString("id");

        imei_flag = true;
        std::cout << "send id : " << id << "\n";
    }

    send(client_socket, &imei_flag, 1, 0);

    if (!imei_flag)
    {
        // closing the connected socket
        std::cout << "close client\n";
        close(client_socket);
        return;
    }

    std::string upd_f;
    res = stmt->executeQuery("SELECT upd_f FROM " + DEV_T + " WHERE imei = " + imei);

    while (res->next())
    {
        upd_f = res->getString("upd_f");

        std::cout << "send upd_f : " << upd_f << "\n" << "size = " << upd_f.size() << "\n";
    }

    send(client_socket, &upd_f, 1, 0);

    delete res;
    delete stmt;

    // closing the connected socket
    std::cout << "close client\n";
    close(client_socket);
    return;
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

