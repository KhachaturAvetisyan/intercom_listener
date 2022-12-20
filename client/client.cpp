# include <iostream>
# include <arpa/inet.h>
# include <stdio.h>
# include <string>
# include <string.h>
# include <sys/socket.h>
# include <unistd.h>
# include <stdlib.h>
# include <ctime>
# include <poll.h>

// # include "../utils/send_read.cpp"

# define PORT 8080


typedef struct 
{
  uint8_t startword;
  char imei[15];
}init_struct;

typedef struct
{
  uint8_t work_mode;
  uint16_t firmware_version;
  uint8_t sim_info;
  uint8_t con_quality_sim_1;
  uint8_t con_quality_sim_2;
  uint16_t battery_voltage;
  uint32_t updtime_NFC;
  uint32_t updtime_PIN;
}ping_struct;

typedef struct
{
  uint32_t time;
  uint8_t type; // 0x00 - NFC & 0x01 - PIN
  uint64_t value;
  uint16_t crc;
}history_struct;

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
    init_s->startword = 0xFE;
    
    // const char* str = "767738917568351";
    const char* str = "859038861542972";

    for (int i = 0; i < 15; ++i)
        init_s->imei[i] = str[i];
    
    // std::cout << "sleep 5 sec\n";
    // sleep(5);
    if(send(sock, init_s, 16, 0) < 0)
    {
        perror("send error");
        exit(EXIT_FAILURE);
    }

    struct pollfd mypoll = { sock, POLLIN|POLLPRI };
    uint8_t status;

    if (poll(&mypoll, 1, 30000))
    {
        if(read(sock, &status, 1) < 0)
        {
            perror("read msg error");
            exit(EXIT_FAILURE);
        }
    }
    else
    {
        std::cout << "read timeout\n";
        return false;
    }

    if (status != 0x01)
    {
        std::cout << "status is : error\n";
        close(client_fd);
        return 0;
    }
    
    std::cout << "status is : OK\n";

    while(1)
    {
        
    }

    // closing the connected socket
    close(client_fd);
    return 0;
}