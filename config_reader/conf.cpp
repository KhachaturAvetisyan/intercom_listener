#include "conf_reader.h"

int main()
{
    auto ret = ConfigReader::Get_configuration("../configs/config.json");
    std::cout << "api : " << ret->api << "\n";
    std::cout << "timelimit : " << ret->read_timelimit << "\n";
}