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

// curl & json includes
# include <curl/curl.h>
# include <nlohmann/json.hpp>

using json = nlohmann::json;
using namespace Pistache;

# include "structs.hpp"

# include "device.hpp"

// main servers thread functions
void http_serv(int port_);
void socket_serv(int port);

# endif // SERV_HEADER