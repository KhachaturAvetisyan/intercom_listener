# include "../includes/serv_include.hpp"

/*

   -- 0XFE -> handshake

   ---0XA1 -> ping-----send-->0X01------
   |     0XB1 -> Request for update    |
   |         0X00 -> update for NFC    |
   |         0X01 -> update for PIN    |
   |     0XB2 -> Data Packet           |
   |___________________________________|

   -- 0XA2 -> history

*/


// create hashmap
std::unordered_map<std::string, Device*> device_map;

void dev_thread(int client_socket, int thread_num)
{
    //logs
    std::cout << "Thread " << thread_num << " : " << "create new thread\n";
    
    Device dev(client_socket, thread_num, &device_map);
    
    // Case #01 Device connection
    if(!dev.hand_shake())
    {
        perror("hand shake error");
        dev.send_status(0x00);
        return;
    }
    if(!dev.Get_device_status())
    {
        perror("Get device status error");
        dev.send_status(0x00);
        return;
    }
    dev.send_status(0x01);

    uint8_t startbyte = 0x01;
    uint8_t status;

    while(startbyte != 0x00)
    {
        startbyte = dev.read_byte();
        if(startbyte == dev.device_ping_data.startbyte)
        {
            // Case #02 Ping from device

            if(!dev.read_ping())
            {
                perror("Ping Was not reading");
                continue;
            }

            // check NFC update 
            if(dev.serv_updtime_NFC > dev.device_ping_data.NFC_list_update_time)
            {
                // logs
                std::cout << "Thread " << thread_num << " : " << "NFC list update\n";

                // Case #03 Time competition for NFC is false
                // if(!dev.Get_NFC_list())
                // {
                //     perror("Get_NFC_list error");
                //     continue;
                // }
                // if(!dev.separate_data_by_pakets())
                // {
                //     perror("separate_data_by_pakets error");
                //     continue;
                // }
                // if(!dev.Request_for_update(0X00))
                // {
                //     perror("Request_for_update error");
                //     continue;
                // }
                // status = dev.read_byte();
                // if(status == 0x00)
                // {
                //     std::cout << "Do not Update!" << std::endl;
                //     continue;
                // }
                // else if(status == 0x01)
                // {
                //     for(int i = 0; i < dev.paket_count; ++i)
                //     {
                //         for(int j = 0; j < 3; ++j)
                //         {
                //             // if(dev.Data_Body())
                //             //     break;
                //         }
                //     }
                //     if(!dev.Post_device_updtime())
                //     {
                //         perror("Post device updtime error");
                //         continue;
                //     }
                // }
            }
            // check PIN update
            if(dev.serv_updtime_PIN > dev.device_ping_data.PIN_list_update_time)
            {
                // logs
                std::cout << "Thread " << thread_num << " : " << "PIN list update\n";

                // Case #04 Time competition for PIN is false
                // if(!dev.Get_PIN_list())
                // {
                //     perror("Get_PIN_list error");
                //     continue;
                // }
                // if(!dev.separate_data_by_pakets())
                // {
                //     perror("separate_data_by_pakets error");
                //     continue;
                // }
                // if(!dev.Request_for_update())
                // {
                //     perror("Request_for_update error");
                //     continue;
                // }
                // status = dev.read_byte();
                // if(status == 0x00)
                // {
                //     sleep(30);
                //     continue;
                // }
                // else if(status == 0x01)
                // {
                //     for(int i = 0; i < dev.paket_count; ++i)
                //     {
                //         for(int j = 0; j < 3; ++j)
                //         {
                //             if(dev.Data_Body())
                //                 break;
                //         }
                //     }
                //     if(!dev.Post_device_updtime())
                //     {
                //         perror("Post device updtime error");
                //         continue;
                //     }
                // }

            }
        }
        else if(startbyte == dev.device_history_data.startbyte)
        {
            // Case #06 Post device event (history)
            if(!dev.read_history())
            {
                perror("read history error");
                continue;
            }
            if(dev.Post_device_event())
            {
                perror("Post device event error");
                continue;
            }
        }
    }
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

    // Forcefully attaching socket to the port
    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) 
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    std::vector<std::thread> threads;
    int thread_num = 1;

    while(1)
    {
        // logs
        std::cout << "Socket server: listening\n";

        if (listen(server_fd, 1) < 0) 
        {
            perror("listen");
            exit(EXIT_FAILURE);
        }

        // logs
        std::cout << "Socket server: accept device\n";
        
        if ((new_socket = accept(server_fd, (struct sockaddr*)&address, (socklen_t*)&addrlen)) < 0) 
        {
            perror("accept");
            exit(EXIT_FAILURE);
        }

        threads.push_back(std::thread(dev_thread, new_socket, thread_num++));

    }

    // closing the listening socket
    shutdown(server_fd, SHUT_RDWR);

}

// Case #05 post request from server about update
void device_updt(const Rest::Request& req, Http::ResponseWriter resp) 
{
    json doc = json::parse(req.body());

    std::string responseString;

    if(doc.contains("imei") && doc.contains("updtime_NFC") && doc.contains("updtime_PIN")) 
    {   
        //logs
        std::cout << "HTTP server: " << "imei is : " << doc["imei"] << "\n";
        std::cout << "HTTP server: " << "updtime_NFC is : " << doc["updtime_NFC"] << "\n";
        std::cout << "HTTP server: " << "updtime_PIN is : " << doc["updtime_PIN"] << "\n";

        std::string imei = doc["imei"];

        if(device_map.find(imei) != device_map.end())
        {
            device_map[imei]->serv_updtime_NFC = doc["updtime_NFC"];
            device_map[imei]->serv_updtime_PIN = doc["updtime_PIN"];

            responseString = "OK";
        }
        else
            responseString = "imei in map has not exists";

    }
    else
        responseString = "No text parameter supplied in JSON:\n" + req.body();
    //logs
    std::cout << "HTTP server: response msg -> " << responseString << "\n";
    
    resp.send(Http::Code::Ok, responseString);
}

void http_serv(int port_)
{
    using namespace Rest;

    Router router;      // POST/GET/etc. route handler
    Port port(port_);    // port to listen on
    Address addr(Ipv4::any(), port);
    std::shared_ptr<Http::Endpoint> endpoint = std::make_shared<Http::Endpoint>(addr);
    auto opts = Http::Endpoint::options().threads(1);   // how many threads for the server
    
    //logs
    std::cout << "HTTP server: server started\n";
    
    endpoint->init(opts);
    Routes::Post(router, "/device_updt", Routes::bind(&device_updt));
    endpoint->setHandler(router.handler());
    endpoint->serve();

}