# include "../includes/device.hpp"

// class constructor
Device::Device(int dev_sock, int thr_num,  std::unordered_map<std::string, Device*> *dev_map) : device_socket(dev_sock), thread_num(thr_num), device_map(dev_map)
{
    device_ping_data.startbyte = 0xA1;
    device_history_data.startbyte = 0xA2;
}
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
    //logs
    std::cout << "Thread " << thread_num << " : " << "Get device status requests\n";

    std::string url = api + "device_status/" + imei;
    // std::cout << "url is : " << url << "\n";

    json res = get_req(url);
    
    // logs //
    // std::cout << std::setw(4) << res << std::endl;
    
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

    // logs
    std::cout << "Thread " << thread_num << " : " <<  "Get NFC list reaquests\n";
    // std::cout << std::setw(4) << res << std::endl;

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
    // logs
    // std::cout << std::setw(4) << res << std::endl;

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
    //logs
    std::cout << "Thread " << thread_num << " : " << "Post device event request\n";

    json req = 
    {
        {"imei", imei},
        {"time", device_history_data.event_time},
        {"type", device_history_data.event_type},
        {"value", device_history_data.value}
    };

    std::string url = api + "/device_event";
    // std::cout << url << "\n";

    std::string res = post_req(url, req);
    // std::cout << res << "\n";

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
        {"updtime_NFC", device_ping_data.NFC_list_update_time},
        {"updtime_PIN", device_ping_data.PIN_list_update_time}
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
    //logs
    std::cout << "Thread " << thread_num << " : " << "read hand shake\n";

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
    
    device_map->insert({imei, this});

    std::cout << "device imei : " << imei << "\n";
    return true;
}

bool Device::read_ping()
{
    //logs
    std::cout << "Thread " << thread_num << " : " << "ping\n";

    uint8_t array[18];

    if(!read_data(array, 18))
    {
        send_status(0x00);
        return false;
    }
    
    device_ping_data.working_mode = array[0];

    device_ping_data.firmware_version = ((uint16_t)array[1] << 8) 
                                        | array[2];
    
    device_ping_data.SIM_info = array[3];
    device_ping_data.SIM1_connection_quality = array[4];
    device_ping_data.SIM2_connection_quality = array[5];

    device_ping_data.battery_voltage = ((uint16_t)array[6] << 8) | array[7];

    device_ping_data.NFC_list_update_time  = ((uint32_t)array[8]  << 24) 
                                            | ((uint32_t)array[9] << 16)
                                            | ((uint32_t)array[10] << 8)
                                            | ((uint32_t)array[11]);

    device_ping_data.PIN_list_update_time  = ((uint32_t)array[12] << 24) 
                                            | ((uint32_t)array[13] << 16)
                                            | ((uint32_t)array[14] << 8)
                                            | ((uint32_t)array[15]);

    device_ping_data.checksum = ((uint16_t)array[16] << 8) 
                                | array[17];

    if(device_ping_data.checksum != checksum(array, 16, device_ping_data.startbyte))
    {
        perror("checksom error");
        send_status(0x00);
        return false;
    }

    // std::cout << (int)device_ping_data.working_mode << "\n";
    // std::cout << (int)device_ping_data.firmware_version << "\n";
    // std::cout << (int)device_ping_data.SIM_info << "\n";
    // std::cout << (int)device_ping_data.SIM1_connection_quality << "\n";
    // std::cout << (int)device_ping_data.SIM2_connection_quality << "\n";
    // std::cout << (int)device_ping_data.battery_voltage << "\n";
    // std::cout << device_ping_data.NFC_list_update_time << "\n";
    // std::cout << device_ping_data.PIN_list_update_time << "\n";
    // std::cout << device_ping_data.checksum << "\n";

    send_status(0X01);
    return true;
}

bool Device::read_history()
{
    //logs
    std::cout << "Thread " << thread_num << " : " << "history\n";

    uint8_t array[15];

    if(!read_data(array, 15))
    {
        send_status(0x00);
        return false;
    }

    device_history_data.event_time = ((uint32_t)array[0]  << 24) 
                                    | ((uint32_t)array[1] << 16)
                                    | ((uint32_t)array[2] << 8)
                                    | ((uint32_t)array[3]);

    device_history_data.event_type = array[4];

    device_history_data.value = ((uint64_t)array[5] << 56)
                                | ((uint64_t)array[6] << 48)
                                | ((uint64_t)array[7] << 40)
                                | ((uint64_t)array[8] << 32)
                                | ((uint64_t)array[9] << 24)
                                | ((uint64_t)array[10] << 16)
                                | ((uint64_t)array[11] << 8)
                                | ((uint64_t)array[12]);

    device_history_data.checksum = ((uint16_t)array[13] << 8) 
                                | array[14];


    if(device_history_data.checksum != checksum(array, 13, device_history_data.startbyte))
    {
        perror("checksom error");
        send_status(0x00);
        return false;
    }

    // std::cout << device_history_data.event_time << "\n";
    // std::cout << (int)device_history_data.event_type << "\n";
    // std::cout << device_history_data.value << "\n";
    // std::cout << device_history_data.checksum << "\n";

    send_status(0x01);
    return true;
}

uint16_t Device::checksum(uint8_t *array, uint16_t array_length, uint8_t startbyte)
{
    if (array_length < 0) return 0;
    if (array_length < 2) return array[0];

    uint16_t retval = 0;
    retval += (uint16_t)startbyte;
    // uint16_t *arr_ptr = (uint16_t *)array;

    for (int i = 0; i < array_length; ++i)
    {
        retval += (uint16_t)array[i];
    }

    // std::cout << "checksum is : " << retval << "\n";

    return retval;
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
    if(res != CURLE_OK)
    {
        std::cout << "CURL exeption : " << curl_easy_strerror(res) << "\n";
        return json({});
    }

    // logs //

    // long http_code = 0;
    // curl_easy_getinfo (curl, CURLINFO_RESPONSE_CODE, &http_code);
    // std::cout << "CURL code status : " << http_code << "\n";

    curl_easy_cleanup(curl);
    
    return json::parse(readBuffer);
  }
  else
    return json({});

}



bool Device::Request_for_update(uint8_t req_code)
{
    uint8_t array[10];
    upd_request request;

    request.startbyte = 0xB1;
    if(req_code == 0x00)
    {
        request.datatype = 0x00;
        request.data_time = serv_updtime_NFC;
        request.data_count = NFC_list.size();
    }
    else if(req_code == 0x01)
    {
        request.datatype = 0x01;
        request.data_time = serv_updtime_PIN;
        request.data_count = PIN_list.size();
    }

    array[0] = request.startbyte;
    
    array[1] = request.datatype;
    
    array[2] = request.data_time >> 24;
    array[3] = request.data_time >> 16;
    array[4] = request.data_time >> 8;
    array[5] = request.data_time;
    
    array[6] = request.data_count >> 8;
    array[7] = request.data_count;
    
    request.checksum = checksum(array, 8, 0);
    array[8] = request.checksum >> 8;
    array[9] = request.checksum;
    
    if(send_data(array, 10))
    {
        perror("send upd data error");
        return false;
    }
    if(read_byte() == 0X00)
    {
        perror("Packet Recive status error");
        return false;
    }

    return true;
}

bool Device::separate_data_by_pakets(uint8_t req_code)
{

    if(req_code == 0x00)
    {
        uint16_t list_size = NFC_list.size();
        packet_count = (list_size + 16 - 1) / 16;
        data_list.clear();

        for (int i = 0; i < packet_count; ++i)
        {
            uint8_t array[128];
            int j = 0;
            while(list_size > 0 && j < 16)
            {
                long long lst = NFC_list[i * 16 + j].get<long long>();
                array[j * 8 + 0] = lst >> 56;
                array[j * 8 + 1] = lst >> 48;
                array[j * 8 + 2] = lst >> 40;
                array[j * 8 + 3] = lst >> 32;
                array[j * 8 + 4] = lst >> 24;
                array[j * 8 + 5] = lst >> 16;
                array[j * 8 + 6] = lst >> 8;
                array[j * 8 + 7] = lst;
                ++j;
                --list_size;
            }
            data_list.push_back(array);
        }
    }
    else if (req_code == 0x01)
    {
        uint16_t list_size = PIN_list.size();
        packet_count = (list_size + 16 - 1) / 16;
        for (int i = 0; i < packet_count; ++i)
        {
            uint8_t array[128];
            int j = 0;
            while(list_size > 0 && j < 16)
            {
                long long lst = PIN_list[i * 16 + j].get<long long>();
                array[j * 8 + 0] = lst >> 56;
                array[j * 8 + 1] = lst >> 48;
                array[j * 8 + 2] = lst >> 40;
                array[j * 8 + 3] = lst >> 32;
                array[j * 8 + 4] = lst >> 24;
                array[j * 8 + 5] = lst >> 16;
                array[j * 8 + 6] = lst >> 8;
                array[j * 8 + 7] = lst;
                --list_size;
                ++j;
            }
            data_list.push_back(array);
        }
    }
    else
    {
        std::cout << "separate_data_by_pakets error : unknown req_code!\n";
        return false;
    }
    return (true);
}

bool Device::Data_Body(uint8_t* body)
{
    uint8_t array[131];
    uint16_t sum;
    
    array[0] = 0xB2;
    
    int i = 0;
    while(++i < 128)
        array[i] = body[i - 1];
    
    sum = checksum(array, 129, 0);
    array[129] = sum >> 8;
    array[130] = sum;

    if(!send_data(array, 131))
    {
        perror("Packet Not Sended");
        return (false);
    }
    if(read_byte() == 0X00)
    {
        perror("Packet Recive status error");
        return false;
    }

    return true;
}