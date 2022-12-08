# ifndef DEVICE_HEADER
# define DEVICE_HEADER

# include "serv_include.h"

class Device
{
private:
    int device_socket;
    std::string imei;
    int read_timelimit = 5000;
    std::string api = "https://api-generator.retool.com/lQk0Cx/data";

    static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp);
    json post_req(std::string url, json post);
    json get_req(std::string url);

    template <typename T> 
    bool read_data(T data, size_t size);

public:
    bool upd_f;

    Device(int dev_sock);
    ~Device();
    bool check_imei();
    bool send_rfid_list();
    bool init_dev_struct();
};

# endif