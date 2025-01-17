#include <iostream>
#include <array>
#include <boost/asio.hpp>

using namespace boost::asio;

class BoostClient
{
public:
    BoostClient(io_service& service, ip::tcp::endpoint ep);

    void AsyncRecvMessage();

    void AsyncSendMessage(std::string message);

private:
    bool bConnected;
    // io_service &ios;
    io_service& m_ios;
    std::shared_ptr<ip::tcp::socket> pSocket;

    std::array<char, 1024> data_;
};
