#ifndef CLIENT_INCLUDE
#define CLIENT_INCLUDE

# include <iostream>
# include <arpa/inet.h>
# include <cstdio>
# include <string>
# include <sys/socket.h>
# include <unistd.h>
# include <cstdlib>
# include <poll.h>

# define PORT 8080


// structures

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
  uint8_t startbyte;
  uint32_t event_time;
  uint8_t event_type;
  uint64_t value;
  uint16_t checksum;
}history_struct;

typedef struct
{
  uint8_t  startbyte; //  0XB1
  uint8_t  datatype;  //  NFC[0X00] | PIN[0X01]
  uint32_t data_time; //  1672040924
  uint64_t data_count;//  15
  uint16_t checksum;  //  sizeof(struct X) - sizeof(uint16_t) | 
}upd_request; 

#endif