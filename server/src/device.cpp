# include "../includes/device.h"

Device::Device(int dev_sock, int thr_num)
{
    device_socket = dev_sock;
    thread_num = thr_num;
}

Device::~Device()
{
    std::cout << "close client\n";
    close(device_socket);
}

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
            exit(EXIT_FAILURE);
        }
    }
    else
    {
        std::cout << "read timeout\n";
        return false;
    }

    return true;
}

bool Device::init_dev_struct()
{
    // struct init
    init_struct* init_s;
    init_s = (init_struct*)malloc(sizeof(init_struct));

    if(!read_data(init_s, 20))
        return false;

    if (init_s->startword != 0x11223344)
    {
        std::cout << "start word error\n";
        return false;
    }
    
    uint8_t loc_checksum = 0;
    for (int i = 0; i < 19; ++i)
        loc_checksum ^= ((uint8_t*)init_s)[i];

    if (loc_checksum != init_s->checksum)
    {
        std::cout << "checksum error\n";
        return false;
    }

    for (int i = 0; i < 15; ++i)
        imei += init_s->imei[i];

    std::cout << imei << "\n";

    free(init_s);
    return true;
}