# include <iostream>
# include <arpa/inet.h>
# include <stdio.h>
# include <string>
# include <string.h>
# include <sys/socket.h>
# include <unistd.h>
# include <stdlib.h>
# include <ctime>

// # include "../utils/send_read.cpp"

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
    
    const char* str = "767738917568351";
    // const char* str = "859038861542972";

    for (int i = 0; i < 15; ++i)
        init_s->imei[i] = str[i];

    init_s->checksum = 0;
    for (int i = 0; i < 19; ++i)
        init_s->checksum ^= ((uint8_t*)init_s)[i];
    
    // std::cout << "sleep 5 sec\n";
    // sleep(5);
    if(send(sock, init_s, 20, 0) < 0)
    {
        perror("send error");
        exit(EXIT_FAILURE);
    }

    bool imei_flag;

    while(1)
    {
        if(read(sock, &imei_flag, 1) < 0)
        {
            perror("read error");
            exit(EXIT_FAILURE);
        }
        std::cout << imei_flag << "\n";
    }

    // if (!imei_flag)
    // {
    //     close(client_fd);
    //     return 0;
    // }

    // closing the connected socket
    close(client_fd);
    return 0;
}