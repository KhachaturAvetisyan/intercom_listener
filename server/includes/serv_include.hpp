# ifndef SERV_HEADER
# define SERV_HEADER

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
# include <poll.h>
# include <chrono>
# include <unordered_map>
# include <stdlib.h>

// http API
# include <pistache/endpoint.h>
# include <pistache/http.h>
# include <pistache/router.h>

// # include "rapidjson/document.h"

// # include "../utils/send_read.cpp"

// curl & json includes
# include <curl/curl.h>
# include <nlohmann/json.hpp>

using json = nlohmann::json;
using namespace Pistache;

// structs
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
}ping_struct;

typedef struct
{
  uint32_t time;
  uint8_t type; // 0x00 - NFC & 0x01 - PIN
  uint64_t value;
  uint16_t crc;
}history_struct;

typedef struct
{
  uint8_t  startbyte; //  0XB1
  uint8_t  datatype;  //  NFC[0X00] | PIN[0X01]
  uint32_t data_time; //  1672040924
  uint64_t data_count;//  15
  uint16_t checksum;  //  sizeof(struct X) - sizeof(uint16_t) | 
}upd_request; 

# include "device.hpp"

// main servers thread functions
void http_serv(int port_);
void socket_serv(int port);

# endif // SERV_HEADER