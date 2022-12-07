# ifndef DEVICE_HEADER
# define DEVICE_HEADER

# include "serv_include.h"

class Device
{
private:
    int device_socket;
    std::string imei;
    int read_timelimit = 5000;
    int thread_num;
    std::string api = "https://api-generator.retool.com/lQk0Cx/data";

    template <typename T> 
    bool read_data(T data, size_t size);

public:
    bool upd_f;

    Device(int dev_sock, int thread_num);
    ~Device();
    bool check_imei();
    bool init_dev_struct();
    
};

# endif