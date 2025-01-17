#include "boost_client.h"

#define DEFAULT_SERVER_IP   "127.0.0.1"
#define DEFAULT_SERVER_PORT 8888

int main(int argc, char** argv)
{

    ip::tcp::endpoint ep(ip::address_v4::from_string(DEFAULT_SERVER_IP), DEFAULT_SERVER_PORT);
    io_service service;

    BoostClient client(service, ep);
    while (1)
    {
        std::cout << "ÇëÊäÈë£º" << std::endl;
        std::string s;
        std::getline(std::cin,s);
        client.AsyncSendMessage(s);
        client.AsyncRecvMessage();
        service.run();
    }

    return 0;
}

