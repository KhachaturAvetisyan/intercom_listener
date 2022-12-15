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

// Device class
// # include "device.h"
// class Device;

// http API
# include <pistache/endpoint.h>
# include <pistache/http.h>
# include <pistache/router.h>

# include "rapidjson/document.h"

// # include "../utils/send_read.cpp"

// curl & json includes
# include <curl/curl.h>
# include <nlohmann/json.hpp>

using json = nlohmann::json;
using namespace Pistache;

// structs
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

// main servers thread functions
void http_serv(int port_);
void socket_serv(int port);

# endif