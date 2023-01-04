// http API
# include <pistache/endpoint.h>
# include <pistache/http.h>
# include <pistache/router.h>

// # include "rapidjson/document.h"

# include <iostream>

# include <nlohmann/json.hpp>

using json = nlohmann::json;
using namespace Pistache;

json devices = 
{
    {
        "859038861542972", 
        {
            {"updtime_NFS", 1671444808},
            {"updtime_PIN", 1671444506},
            {"NFC_list", {132456, 456789, 891567}},
            {"PIN_list", {4867, 1248, 9763}}
        }
    },

    {
        "767738917568351", 
        {
            {"updtime_NFS", 1348444808},
            {"updtime_PIN", 1647644506},
            {"NFC_list", {7159456, 454736, 894781}},
            {"PIN_list", {7891, 9756, 7813}}
        }
    }
};


void device_status(const Rest::Request& req, Http::ResponseWriter resp) 
{
    std::string restext;

    if(req.hasParam(":imei"))
    {
        std::string imei = req.param(":imei").as<std::string>();

        if(devices.contains(imei))
        {
            json resp_json;
            resp_json["updtime_NFS"] = devices[imei]["updtime_NFS"];
            resp_json["updtime_PIN"] = devices[imei]["updtime_PIN"];

            restext = to_string(resp_json);
        }
        else
            restext = "not exist!!";
    }
    else
        restext = "No parameter supplied.";

    std::cout << restext << "\n";
    resp.send(Http::Code::Ok, restext);
}

void NFC_list(const Rest::Request& req, Http::ResponseWriter resp) 
{
    std::string restext;

    if(req.hasParam(":imei"))
    {
        std::string imei = req.param(":imei").as<std::string>();

        if(devices.contains(imei))
        {
            restext = to_string(devices[imei]["NFC_list"]);
        }
        else
            restext = "not exist!!";
    }
    else
        restext = "No parameter supplied.";

    resp.send(Http::Code::Ok, restext);
}

void PIN_list(const Rest::Request& req, Http::ResponseWriter resp) 
{
    std::string restext;

    if(req.hasParam(":imei"))
    {
        std::string imei = req.param(":imei").as<std::string>();

        if(devices.contains(imei))
        {
            restext = to_string(devices[imei]["PIN_list"]);
        }
        else
            restext = "not exist!!";
    }
    else
        restext = "No parameter supplied.";

    resp.send(Http::Code::Ok, restext);
}

void device_event(const Rest::Request& req, Http::ResponseWriter resp) 
{
    // rapidjson::Document doc;
    // doc.Parse(req.body().c_str());

    std::string responseString;

    std::cout << responseString << "\n";
    resp.send(Http::Code::Ok, responseString);
}

void device_updtime(const Rest::Request& req, Http::ResponseWriter resp) 
{
    // rapidjson::Document doc;
    // doc.Parse(req.body().c_str());

    std::string responseString;

    std::cout << responseString << "\n";
    resp.send(Http::Code::Ok, responseString);
}

int main()
{
    using namespace Rest;

    Router router;      // POST/GET/etc. route handler
    Port port(9090);    // port to listen on
    Address addr(Ipv4::any(), port);
    std::shared_ptr<Http::Endpoint> endpoint = std::make_shared<Http::Endpoint>(addr);
    auto opts = Http::Endpoint::options().threads(1);   // how many threads for the server

    std::cout << "run http server\n";

    endpoint->init(opts);

    Routes::Get(router, "/device_status/:imei?", Routes::bind(&device_status));
    Routes::Get(router, "/NFC_list/:imei?", Routes::bind(&NFC_list));
    Routes::Get(router, "/PIN_list/:imei?", Routes::bind(&PIN_list));
    Routes::Post(router, "/device_event", Routes::bind(&device_event));
    Routes::Post(router, "/device_updtime", Routes::bind(&device_updtime));

    endpoint->setHandler(router.handler());
    endpoint->serve();
}

