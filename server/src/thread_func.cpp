# include "../includes/serv_include.h"
# include "../includes/device.h"


void dev_thread(int client_socket)
{
    Device dev(client_socket);

    
    if(!dev.init_dev_struct())
    {
        dev.send_status(0x00);
        return;
    }

    dev.send_status(0x01);
    

    // while(1)
    // {
    //     if(!dev.check_imei())
    //         return;

    //     if(send(client_socket, &dev.upd_f, 1, 0) < 0)
    //     {
    //         perror("send client upd_f");
    //         return;
    //     }

    //     // if(dev.upd_f)
    //     // {
    //     //     if(!dev.send_rfid_list())
    //     //         return;
    //     // }

    //     std::this_thread::sleep_for(std::chrono::seconds(5));
    // }
}

void socket_serv(int port)
{
    // server init
    int server_fd, new_socket, valread;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);

    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) 
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // Forcefully attaching socket to the port 8080
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) 
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);

    // Forcefully attaching socket to the port 8080
    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) 
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    std::cout << "binding access!\n";
    std::vector<std::thread> threads;

    while(1)
    {
        std::cout << "listening\n";
        if (listen(server_fd, 1) < 0) 
        {
            perror("listen");
            exit(EXIT_FAILURE);
        }

        std::cout << "accept\n";
        if ((new_socket = accept(server_fd, (struct sockaddr*)&address, (socklen_t*)&addrlen)) < 0) 
        {
            perror("accept");
            exit(EXIT_FAILURE);
        }

        threads.push_back(std::thread(dev_thread, new_socket));

    }

    // closing the listening socket
    shutdown(server_fd, SHUT_RDWR);

}


void echo(const Rest::Request& req, Http::ResponseWriter resp) 
{
    rapidjson::Document doc;
    doc.Parse(req.body().c_str());
    // std::string responseString = doc.HasMember("text") ? doc["text"].GetString() : "No text parameter supplied in JSON:\n" + req.body();
    resp.send(Http::Code::Ok, "OK");
}

void http_serv(int port_)
{
    using namespace Rest;

    Router router;      // POST/GET/etc. route handler
    Port port(port_);    // port to listen on
    Address addr(Ipv4::any(), port);
    std::shared_ptr<Http::Endpoint> endpoint = std::make_shared<Http::Endpoint>(addr);
    auto opts = Http::Endpoint::options().threads(1);   // how many threads for the server

    std::cout << "run http server\n";

    endpoint->init(opts);

    Routes::Post(router, "/echo", Routes::bind(&echo));

    endpoint->setHandler(router.handler());
    endpoint->serve();

}