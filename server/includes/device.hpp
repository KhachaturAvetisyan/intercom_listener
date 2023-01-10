# ifndef DEVICE_HEADER
# define DEVICE_HEADER

# include "serv_include.hpp"

class Device
{
private:
    const int read_timelimit = 30 * 1000;
    const std::string api = "http://localhost:9090/";

    int device_socket;
    int thread_num;
    std::unordered_map<std::string, Device*> *device_map;

    history_struct* history_s;
    json NFC_list;
    json PIN_list;

    long long *nfcs;
    static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp);
    std::string post_req(std::string url, json post);
    json get_req(std::string url);

    template <typename T> 
    bool read_data(T data, size_t size);
    template <typename T> 
    bool send_data(T data, size_t size);

    //checksum
    uint16_t checksum(upd_request upd);

public:
    std::string imei;
    uint32_t serv_updtime_NFC = 0;
    uint32_t serv_updtime_PIN = 0;
    uint8_t paket_count;

    ping_struct device_ping_data;
    history_struct device_history_data;

    // class constructor destructor
    Device(int dev_sock, int thread_num, std::unordered_map<std::string, Device*> *device_map);
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
    bool separate_data_by_pakets();
    bool Request_for_update(uint8_t req_code);
    // bool Data_Body();

    //utils
    uint8_t calc_packets(long long *nfcs);
};

# endif