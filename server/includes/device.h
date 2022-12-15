# ifndef DEVICE_HEADER
# define DEVICE_HEADER

# include "serv_include.h"

class Device
{
private:
    const int read_timelimit = 30 * 1000;
    const std::string api = "https://api-generator.retool.com/lQk0Cx/data";
    
    int device_socket;
    uint32_t dev_updtime_NFC = 0;
    uint32_t dev_updtime_PIN = 0;

    history_struct* history_s;
    json NFT_list;
    json PIN_list;

    static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp);
    std::string post_req(std::string url, json post);
    json get_req(std::string url);

    template <typename T> 
    bool read_data(T data, size_t size);
    template <typename T> 
    bool send_data(T data, size_t size);

public:
    std::string imei;
    uint32_t serv_updtime_NFC = 0;
    uint32_t serv_updtime_PIN = 0;

    Device(int dev_sock);
    ~Device();

    bool init_dev_struct();
    bool read_ping();
    bool read_history();

    void send_status(uint8_t status);
    bool read_status();

    bool Get_device_status();
    bool Get_NFC_list();
    bool Get_PIN_list();
    bool Post_device_event();
    bool Post_device_updtime();
};

# endif