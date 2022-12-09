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

// http API
#include <pistache/endpoint.h>
#include <pistache/http.h>
#include <pistache/router.h>

#include "rapidjson/document.h"

// # include "../utils/send_read.cpp"

// curl & json includes
# include <curl/curl.h>
# include <nlohmann/json.hpp>

using json = nlohmann::json;
using namespace Pistache;

# define PORT 8080

typedef struct 
{
  uint32_t startword;
  char imei[15];
  uint8_t checksum;
}init_struct;

void http_serv();
void socket_serv();

# endif