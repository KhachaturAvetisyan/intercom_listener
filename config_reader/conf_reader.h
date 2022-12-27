#ifndef CONF_READER
#define CONF_READER

#include <iostream>
#include <memory>
#include <fstream>
#include <nlohmann/json.hpp>

class ConfigReader
{
private:
    

public:
    ConfigReader(){};
    std::string api;
    size_t read_timelimit;

    ConfigReader(const ConfigReader&) = delete;

    [[nodiscard]]
    static std::shared_ptr<ConfigReader> Get_configuration(const std::string &path)
    {
        namespace json = nlohmann;
        
        auto ret = std::make_shared<ConfigReader>();

        std::ifstream in {path};
        const auto&& doc = json::json::parse(in);

        if (!doc.contains("api"))
        {
            throw std::runtime_error{R"(Configuration parameter "api" wasn`t specified in )" + path};
        }
        ret->api = to_string(doc["api"]);

        if (!doc.contains("timelimit"))
        {
            throw std::runtime_error{R"(Configuration parameter "timelimit" wasn`t specified in )" + path};
        }
        ret->read_timelimit = doc["timelimit"].get<size_t>();

        return ret;
    }
};

#endif

// g++ -std=c++20 conf.cpp