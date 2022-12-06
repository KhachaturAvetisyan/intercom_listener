# ifndef DEVICE_HEADER
# define DEVICE_HEADER

# include "serv_include.h"

class Device
{
private:
    int device_socket;
    std::string imei;
    int read_timelimit = 2000;

    template <typename T> 
    bool read_data(T data, size_t size);

public:
    Device(int dev_sock);
    ~Device();
    bool init_dev_struct();
    
};

# endif