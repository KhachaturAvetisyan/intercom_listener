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

# include "../utils/send_read.cpp"

# include "mysql_connection.h"

# include <cppconn/driver.h>
# include <cppconn/exception.h>
# include <cppconn/resultset.h>
# include <cppconn/statement.h>
# include <cppconn/prepared_statement.h>

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

# endif