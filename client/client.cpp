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
    uint8_t  startbyte;
    uint8_t  working_mode;
    uint16_t firmware_version;
    uint8_t  SIM_info;
    uint8_t  SIM1_connection_quality;
    uint8_t  SIM2_connection_quality;
    uint16_t battery_voltage;
    uint32_t NFC_list_update_time;
    uint32_t PIN_list_update_time;
    uint16_t checksum;
} ping_struct;

typedef struct
{
  uint32_t time;
  uint8_t type; // 0x00 - NFC & 0x01 - PIN
  uint64_t value;
  uint16_t crc;
}history_struct;

// typedef struct
// {
//     uint8_t startbyte;
//     uint8_t packet_count;
//     uint8_t 
// }body;

typedef struct
{
  uint8_t  startbyte; //  0XB1
  uint8_t  datatype;  //  NFC[0X00] | PIN[0X01]
  uint32_t data_time; //  1672040924
  uint64_t data_count;//  15
  uint16_t checksum;  //  sizeof(struct X) - sizeof(uint16_t) | 
}upd_request; 

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

    // handshake
    uint8_t startword = 0xFE;
    
    // std::string imei = "767738917568351";
    std::string imei = "859038861542972";

    uint8_t data[16];

    data[0] = startword;
    for (int i = 0; i < 15; ++i)
        data[1 + i] = imei[i];
    
    // std::cout << "sleep 5 sec\n";
    // sleep(5);

    if(send(sock, data, 16, 0) < 0)
    {
        perror("send error");
        exit(EXIT_FAILURE);
    }

    // delete data;

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


    // ping
    ping_struct ping;
    // upd_request upd;
    // uint8_t startword;

    ping.startbyte = 0xA1;
    ping.working_mode = 0x01;
    ping.firmware_version = 0xA101;
    ping.SIM_info = 0x01;
    ping.SIM1_connection_quality = 120;
    ping.SIM2_connection_quality = 65;
    ping.battery_voltage = 1206;
    ping.NFC_list_update_time = 0xFA122112;
    ping.PIN_list_update_time = 0xFA122112;
    ping.checksum = 13256;

    std::cout << (int)ping.startbyte << "\n";
    std::cout << (int)ping.working_mode << "\n";
    std::cout << (int)ping.firmware_version << "\n";
    std::cout << (int)ping.SIM_info << "\n";
    std::cout << (int)ping.SIM1_connection_quality << "\n";
    std::cout << (int)ping.SIM2_connection_quality << "\n";
    std::cout << (int)ping.battery_voltage << "\n";
    std::cout << ping.NFC_list_update_time << "\n";
    std::cout << ping.PIN_list_update_time << "\n";
    std::cout << ping.checksum << "\n";

    uint8_t array[19];

    array[0] = ping.startbyte;
    array[1] = ping.working_mode;

    array[2] = ping.firmware_version >> 8;
    array[3] = ping.firmware_version;
    
    array[4] = ping.SIM_info;
    array[5] = ping.SIM1_connection_quality;
    array[6] = ping.SIM2_connection_quality;

    array[7] = ping.battery_voltage >> 8;
    array[8] = ping.battery_voltage;

    array[9]  = ping.NFC_list_update_time >> 24;
    array[10] = ping.NFC_list_update_time >> 16;
    array[11] = ping.NFC_list_update_time >>  8;
    array[12] = ping.NFC_list_update_time;

    array[13] = ping.PIN_list_update_time >> 24;
    array[14] = ping.PIN_list_update_time >> 16;
    array[15] = ping.PIN_list_update_time >>  8;
    array[16] = ping.PIN_list_update_time;

    array[17] = ping.checksum >> 8;
    array[18] = ping.checksum;

    if(send(sock, array, 16, 0) < 0)
    {
        perror("send error");
        exit(EXIT_FAILURE);
    }

    // delete array;

    // struct pollfd mypoll = { sock, POLLIN|POLLPRI };
    // uint8_t status;

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

    // while(1)
    // {
    //     startword = 0XA1;
    //     if(startword == 0XA1) // ping
    //     {
    //         if(send(sock, &startword, sizeof(uint8_t), 0) < 0)
    //         {
    //             perror("Ping Startword Did Not Send");
    //             return (0);
    //         }
    //         if(send(sock, &ping, sizeof(ping_struct), 0) < 0)
    //         {
    //             perror("Ping Structure Did Not Send");
    //             return (0);
    //         }
    //         if(read(sock, &startword, sizeof(uint8_t)) < 0)
    //         {
    //             perror("Response not found");
    //             return (0);
    //         }        
    //         if(startword == 0X01)
    //         {
    //             if(read(sock, &upd, sizeof(upd_request)) < 0)
    //             {
    //                 perror("requested update was dumped");
    //                 return (0);
    //             }
                
    //             startword = 0x01;
    //             if(send(sock, &startword, sizeof(uint8_t), 0) < 0)
    //             {
    //                 perror("send to update was dumped");
    //                 return (0);
    //             }   
    //         }
    //         else
    //         {
    //             perror("Rsponse Not returned");
    //             exit(0);
    //         } 
    //     }
    //     else if(startword == 0XA2) //history
    //     {
    //     }
    //     sleep(5);
    // }
    // closing the connected socket
    close(client_fd);
    return 0;
}