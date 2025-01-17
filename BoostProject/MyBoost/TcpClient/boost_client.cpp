#include "boost_client.h"

BoostClient::BoostClient(io_service& service, ip::tcp::endpoint ep) :m_ios(service)
{
    pSocket.reset(new ip::tcp::socket(service));

    boost::system::error_code ec;
    pSocket->connect(ep, ec);
    if (ec)
    {
        std::cerr << "Error connecting to server: " << ec.message() << std::endl;
        bConnected = false;
        return;
    }
    bConnected = true;
}

void BoostClient::AsyncRecvMessage()
{
    memset(data_.data(), 0, sizeof(data_));
    pSocket->async_read_some(buffer(data_), [this](const boost::system::error_code& ec, size_t bytes) {
        if (!ec)
        {
            std::cout << "recv size:" << bytes << std::endl;
            std::cout << "recv data:" << data_.data() << std::endl;
            AsyncRecvMessage();
        }
        else if (ec == error::eof)
        {
            // ¶Ï¿ªÁ¬½Ó
            bConnected = false;
            std::cout << "disconnected !" << std::endl;
        }
        else
        {
            std::cout << "read error: " << ec.message() << std::endl;
        }
        });
}

void BoostClient::AsyncSendMessage(std::string message)
{
    if (message.empty() || !bConnected)
    {
        return;
    }

    async_write(*pSocket, buffer(message.c_str(), message.size()), [this](const boost::system::error_code& ec, size_t writed_bytes)
        {
            if (!ec)
            {
                printf("send successfully");
            }
            else
            {
                std::cout << "send error:" << ec.message() << std::endl;
                bConnected = false;
            }
        });
}
