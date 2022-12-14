# include "../includes/device.h"

// class constructor
Device::Device(int dev_sock) : device_socket(dev_sock)
{}
// class destructor
Device::~Device()
{
    std::cout << "close client\n";
    close(device_socket);
}


// listener to server requests
bool Device::Get_device_status()
{
    json j = get_req(api + "?imei=" + imei);
    // std::cout << std::setw(4) << j << std::endl;
    
    if (j.empty())
    {
        std::cout << "no data\n";
        return false;
    }

    return true; 
}

bool Device::Get_NFC_list()
{

}

bool Device::Get_PIN_list()
{

}

bool Device::Post_device_event()
{

}

bool Device::Post_device_updtime()
{

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

bool Device::read_status()
{
    uint8_t status;
    read_data(&status, 1);

    return status == 0x01 ? true : false;
}


// listener to device send & read functions
bool Device::init_dev_struct()
{
    // struct init
    init_struct* init_s;
    init_s = (init_struct*)malloc(sizeof(init_struct));

    if(!read_data(init_s, sizeof(init_struct)))
        return false;

    if (init_s->startword != 0xFE)
    {
        std::cout << "start word error\n";
        return false;
    }

    for (int i = 0; i < 15; ++i)
        imei += init_s->imei[i];

    std::cout << imei << "\n";

    free(init_s);
    return true;
}

bool Device::read_ping()
{
    ping_struct* ping_s;
    ping_s = (ping_struct*)malloc(sizeof(ping_struct));

    if(!read_data(ping_s, sizeof(ping_struct)))
        return false;
    
    dev_updtime_NFC = ping_s->updtime_NFC;
    dev_updtime_PIN = ping_s->updtime_PIN;

    free(ping_s);
    return true;
}

bool Device::read_history()
{
    history_struct* history_s;
    history_s - (history_struct*)malloc(sizeof(history_struct));

    if(!read_data(history_s, sizeof(history_struct)))
        return false;
    


    free(history_s);
    return true;
}


// curl post & get request
size_t Device::WriteCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

json Device::post_req(std::string url, json post)
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

  return json::parse(readBuffer);
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