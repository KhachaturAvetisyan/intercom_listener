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

# include "../utils/send_read.cpp"

// mysql includes
# include "mysql_connection.h"

# include <cppconn/driver.h>
# include <cppconn/exception.h>
# include <cppconn/resultset.h>
# include <cppconn/statement.h>
# include <cppconn/prepared_statement.h>

// curl & json includes
# include <curl/curl.h>
# include <nlohmann/json.hpp>

using json = nlohmann::json;

# define PORT 8080

std::string DEV_T = "Dev";
std::string USERS_T = "Users";
std::string RFID_T = "Rfid";

typedef struct 
{
  uint32_t startword;
  char imei[15];
  uint8_t checksum;
}init_struct;

json post_req(std::string url, json post);
json get_req(std::string url);

# endif