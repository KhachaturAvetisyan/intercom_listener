# include "includes/serv_include.h"

int main()
{
    std::thread http_serv_thread(http_serv);
    std::thread socket_serv_thread(socket_serv);

    http_serv_thread.join();
    socket_serv_thread.join();

    return 0;
}

