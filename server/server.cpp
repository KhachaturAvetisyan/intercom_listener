# include "includes/serv_include.hpp"

int main()
{
    std::thread http_serv_thread(http_serv, 9900);
    std::thread socket_serv_thread(socket_serv, 8080);

    http_serv_thread.join();
    socket_serv_thread.join();

    return 0;
}