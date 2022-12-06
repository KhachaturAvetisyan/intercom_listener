# include <iostream>
# include <arpa/inet.h>
# include <stdio.h>
# include <string>
# include <string.h>
# include <sys/socket.h>
# include <unistd.h>
# include <stdlib.h>
# include <ctime>

# include "../utils/send_read.cpp"

# define PORT 8080


typedef struct 
{
  uint32_t startword;
  char imei[15];
  uint8_t checksum;
}init_struct;

int main()
{
    int sock = 0, valread, client_fd;
    struct sockaddr_in serv_addr;

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) 
    {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    // Convert IPv4 and IPv6 addresses from text to binary form
    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) 
    {
        perror("inet_pton");
        exit(EXIT_FAILURE);
    }

    if ((client_fd = connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr))) < 0) 
    {
        perror("connect");
        exit(EXIT_FAILURE);
    }

    init_struct* init_s;
    init_s = (init_struct*)malloc(sizeof(init_struct));
    init_s->startword = 0x11223344;
    
    const char* str = "862770041752523";
    // const char* str = "245189011584231";
    // const char* str = "771596412074913";
    // const char* str = "999999999999999";

    for (int i = 0; i < 15; ++i)
        init_s->imei[i] = str[i];

    init_s->checksum = 0;
    for (int i = 0; i < 19; ++i)
        init_s->checksum ^= ((uint8_t*)init_s)[i];
    
    // std::cout << "sleep 5 sec\n";
    // sleep(5);
    send(sock, init_s, 20, 0);

    bool imei_flag;
    read(sock, &imei_flag, 1);
    std::cout << imei_flag << "\n";

    if (!imei_flag)
    {
        close(client_fd);
        return 0;
    }

    // time_t upd_time = 414086872;
    // char* upd_f;
    // upd_f = (char*)malloc(1);
    
    // read(sock, upd_f, 1);
    // std::cout << "upd flag = " << upd_f << "\n";

    // closing the connected socket
    close(client_fd);
    return 0;
}