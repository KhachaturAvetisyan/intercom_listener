# include "../includes/serv_include.hpp"

// class constructor
Device::Device(int dev_sock, std::unordered_map<std::string, Device*> *dev_map) : device_socket(dev_sock), device_map(dev_map)
{}
// class destructor
Device::~Device()
{
    std::cout << "close client\n";
    close(device_socket);

    device_map->erase(imei);
}


// listener to server requests
bool Device::Get_device_status()
{
    std::string url = api + "device_status/" + imei;
    std::cout << "url is : " << url << "\n";
    json res = get_req(url);
    std::cout << std::setw(4) << res << std::endl;
    
    if (res.empty())
    {
        perror("Get_device_status response is empty");
        return false;
    }
    else if(!res.contains("updtime_NFS"))
    {
        perror("Get_device_status response not contains updtime_NFS");
        return false;
    }
    else if(!res.contains("updtime_PIN"))
    {
        perror("Get_device_status response not contains updtime_PIN");
        return false;
    }

    serv_updtime_NFC = res.at("updtime_NFS");
    serv_updtime_PIN = res.at("updtime_PIN");

    return true; 
}

bool Device::Get_NFC_list()
{
    json res = get_req(api + "NFC_list/" + imei);
    std::cout << std::setw(4) << res << std::endl;

    if(res.empty())
    {
        perror("Get_NFC_list response error");
        return false;
    }

    NFC_list = res;

    return true;
}

bool Device::Get_PIN_list()
{
    json res = get_req(api + "?imei=" + imei);
    std::cout << std::setw(4) << res << std::endl;

    if(res.empty())
    {
        perror("Get_PIN_list response error");
        return false;
    }

    PIN_list = res;

    return true;
}

bool Device::Post_device_event()
{
    json req = 
    {
        {"imei", imei},
        {"time", history_s->time},
        {"type", history_s->type},
        {"value", history_s->value}
    };

    std::string res = post_req(api + "", req);

    if (res != "OK")
    {
        perror("Post_device_event response error");
        return false;
    }

    return true;
}

bool Device::Post_device_updtime()
{
    json req = 
    {
        {"imei", imei},
        {"updtime_NFC", dev_updtime_NFC},
        {"updtime_PIN", dev_updtime_PIN}
    };

    std::string res = post_req(api + "", req);

    if (res != "OK")
    {
        perror("Post_device_updtime response error");
        return false;
    }

    return true;
}


// helper functions
template <typename T>  
bool Device::read_data(T data, size_t size)
{
    // init poll struct
    struct pollfd mypoll = { device_socket, POLLIN|POLLPRI };

    if (poll(&mypoll, 1, read_timelimit))
    {
        if(read(device_socket, data, size) < 0)
        {
            perror("read msg error");
        }
    }
    else
    {
        std::cout << "read timeout\n";
        return false;
    }

    return true;
}

template <typename T>  
bool Device::send_data(T data, size_t size)
{
    if(send(device_socket, &data, size, 0) < 0)
    {
        perror("send error");
        return false;
    }

    return true;
}


// status functions
void Device::send_status(uint8_t status)
{
    send_data(status, 1);
}

uint8_t Device::read_byte()
{
    uint8_t data;
    if(!read_data(&data, 1))
        return 0x00;
    // std::cout << (data == 0xFE) << "\n";
    return data;
}


// listener to device send & read functions
bool Device::hand_shake()
{
    uint8_t data[16];
    if(!read_data(&data, 16))
        return false;

    uint8_t startbyte = data[0];
    if (startbyte != 0xFE)
    {
        perror("startbyte error");
        return false;
    }

    for (int i = 0; i < 15; ++i)
        imei += data[1 + i];

    std::cout << imei << "\n";
    return true;
}

bool Device::read_ping()
{
    ping_struct *ping_s;
    uint8_t startbyte;
    ping_s = (ping_struct*)malloc(sizeof(ping_struct));

    if(!read_data(ping_s, sizeof(ping_struct)))
        return false;
    dev_updtime_NFC = ping_s->updtime_NFC;
    dev_updtime_PIN = ping_s->updtime_PIN;
    send_status(0X01);
    free(ping_s);
    return true;
}

bool Device::read_history()
{
    history_s = (history_struct*)malloc(sizeof(history_struct));

    if(!read_data(history_s, sizeof(history_struct)))
        return false;

    // free(history_s);
    return true;
}


// curl post & get request
size_t Device::WriteCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

std::string Device::post_req(std::string url, json post)
{
  std::string post_str = to_string(post);

  struct curl_slist *slist1;
  slist1 = NULL;
  slist1 = curl_slist_append(slist1, "Content-Type: application/json");

  CURL *curl;
  CURLcode res;
  std::string readBuffer;

  curl = curl_easy_init();
  if(curl) 
  {
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());

    curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 1L);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, post_str.c_str());
    curl_easy_setopt(curl, CURLOPT_USERAGENT, "curl/7.38.0");
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, slist1);
    curl_easy_setopt(curl, CURLOPT_MAXREDIRS, 50L);
    curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "POST");
    curl_easy_setopt(curl, CURLOPT_TCP_KEEPALIVE, 1L);

    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);

    // curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);

    res = curl_easy_perform(curl);
    /* Check for errors */
    if(res != CURLE_OK)
      fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
    
    long http_code = 0;
    curl_easy_getinfo (curl, CURLINFO_RESPONSE_CODE, &http_code);
    std::cout << "code status is : " << http_code << "\n";

    curl_easy_cleanup(curl);
  }

  // std::cout << readBuffer << "\n";

  return readBuffer;
}

json Device::get_req(std::string url)
{
  CURL *curl;
  CURLcode res;
  std::string readBuffer;
  
  curl = curl_easy_init();
  if(curl) 
  {
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);

    res = curl_easy_perform(curl);

    long http_code = 0;
    curl_easy_getinfo (curl, CURLINFO_RESPONSE_CODE, &http_code);
    std::cout << "code status is : " << http_code << "\n";
    curl_easy_cleanup(curl);
  }

  return json::parse(readBuffer);
}

bool Device::separate_data_by_pakets()
{
    if(NFC_list.size() > 0)
    {
        int size = NFC_list.size();
        nfcs = (long long *)malloc(sizeof(long long*) * size);
        std::string nfc;
        int i = -1;
        while(++i < size)
            nfcs[i] = NFC_list[i].get<long long>();
        
        return (true);
    }
    return (false);
}

bool Device::Request_for_update(uint8_t req_code)
{
    uint8_t packets;
    upd_request upd;

    if(!nfcs[0])
        return (false);
    if(req_code == 0X00)
    {
        upd.startbyte  = 0XB1;
        upd.datatype   = req_code;
        upd.data_time  = time(NULL);
        upd.data_count = 14;
        upd.checksum = checksum(upd);
        if(!send_data(upd, sizeof(upd_request)))
        {
            perror("request for update was dumped");
            return (false);
        }
    }
    else if(req_code == 0x01)
    {
        //request for update PIN list
        std::cout << "Update PIN List" << std::endl;
    }
    else
        return (false);
        
    return (true);
}

uint16_t Device::checksum(upd_request upd)
{
    uint16_t sum;

    sum = upd.startbyte  + \
          upd.datatype   + \
          upd.data_time  + \
          upd.data_count;
    return (sum);
}