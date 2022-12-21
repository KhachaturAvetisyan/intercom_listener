# ifndef DEVICE_HEADER
# define DEVICE_HEADER

# include "serv_include.hpp"

class Device
{
private:
    const int read_timelimit = 30 * 1000;
    const std::string api = "http://localhost:9090/";
    
    int device_socket;
    std::unordered_map<std::string, Device*> *device_map;

    history_struct* history_s;
    json NFC_list;
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
    uint32_t dev_updtime_NFC = 0;
    uint32_t dev_updtime_PIN = 0;
    uint8_t paket_count;

    // class constructor destructor
    Device(int dev_sock, std::unordered_map<std::string, Device*> *device_map);
    ~Device();

    // Device to listener read functions
    bool hand_shake();
    bool read_ping();
    bool read_history();

    // send read status functions
    void send_status(uint8_t status);
    uint8_t read_byte();

    // listener to server Post Get requests
    bool Get_device_status();
    bool Get_NFC_list();
    bool Get_PIN_list();
    bool Post_device_event();
    bool Post_device_updtime();

    // listener to device requests
    bool Request_for_update();
    bool Data_Body();
    bool separate_data_by_pakets();
};

# endif