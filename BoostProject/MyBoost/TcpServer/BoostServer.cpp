#include "boostServer.h"

Session_Recv::Session_Recv(ip::tcp::socket socket) :socket_(std::move(socket))
{
    connected_ = true;
    ptr_close_ = nullptr;
    ptr_message_ = nullptr;
}


void Session_Recv::SetClose(Callback_Close pClose)
{
    ptr_close_ = pClose;
}


void Session_Recv::SetMsg_Ntf(Callback_SendMessage pMsg)
{
    ptr_message_ = pMsg;
}

void Session_Recv::async_read()
{
    auto self(shared_from_this());
    memset(data_.data(), 0, sizeof(data_));

    // 读操作完成时回调该函数, 读取到一些数据就会触发回调
    socket_.async_read_some(buffer(data_), [this, self](const boost::system::error_code& ec, size_t bytes) {
        if (!ec)
        {
            if (ptr_message_)
            {

                ptr_message_(std::string(data_.data(), bytes));
            }
            std::cout << "收到信息：" << data_.data() << std:: endl;
            std::cout << "size:" << bytes << std::endl;

            async_read();
        }
        else if (ec == error::eof)
        {
            // 断开连接
            std::cout << "read disconnect:" << ec.message() << std::endl;
            connected_ = false;
            if (ptr_close_)
            {
                ptr_close_();
            }
        }
        else
        {
            std::cout << "read error:" << ec.message() << std::endl;
        }
        });
}


bool Session_Recv::GetState()
{
    return connected_;
}

Session_Send::Session_Send(ip::tcp::socket socket) :socket_(std::move(socket))
{
    connected_ = true;
    ptr_close_ = nullptr;
}


void Session_Send::SetClose(Callback_Close pClose)
{
    ptr_close_ = pClose;
}

void Session_Send::async_send(std::string message)
{
    if (message.empty() || !connected_)
    {
        return;
    }

    auto self(shared_from_this());
    async_write(socket_, buffer(message.c_str(), message.size()), [this, self](const boost::system::error_code& ec, size_t writed_bytes)
        {
            if (!ec)
            {
                //	async_read();
            }
            else
            {
                std::cout << "send error:" << ec.message() << std::endl;
                connected_ = false;
                if (ptr_close_)
                {
                    ptr_close_();
                }
            }
        });
}


bool Session_Send::GetState()
{
    return connected_;
}


BoostServer::BoostServer(boost::asio::io_context& ctx, short port_recv, short port_send, std::string ip_recv, std::string ip_send)
{
    ptr_Thread_Msg.reset(new std::thread(std::bind(&BoostServer::SendMessage, this)));

    if ("" == ip_recv)
    {
        ptr_Recv_ep.reset(new ip::tcp::endpoint(ip::tcp::v4(), port_recv));
    }
    else
    {
        ptr_Recv_ep.reset(new ip::tcp::endpoint(ip::address::from_string(ip_recv), port_recv));
    }
    Acceptor_Recv_.reset(new ip::tcp::acceptor(ctx, *ptr_Recv_ep));

    if ("" == ip_send)
    {
        ptr_Send_ep.reset(new ip::tcp::endpoint(ip::tcp::v4(), port_send));
    }
    else
    {
        ptr_Send_ep.reset(new ip::tcp::endpoint(ip::address::from_string(ip_send), port_send));
    }
    Acceptor_Send_.reset(new ip::tcp::acceptor(ctx, *ptr_Send_ep));

    clients_recv.clear();
    clients_send.clear();

    Async_Accept_Recv();
    Async_Accept_Send();
}


void BoostServer::Async_Accept_Recv()
{
    Acceptor_Recv_->async_accept([this](boost::system::error_code ec, ip::tcp::socket socket)
        {
            if (!ec)
            {
                auto ptr = std::make_shared<Session_Recv>(std::move(socket));
                ptr->SetClose(std::bind(&BoostServer::Close_Session_Recv, this));
                ptr->SetMsg_Ntf(std::bind(&BoostServer::GetContent, this, std::placeholders::_1));
                ptr->async_read();
                clients_recv.push_back(ptr);
            }

            Async_Accept_Recv();
        });
}


void BoostServer::Async_Accept_Send()
{
    Acceptor_Send_->async_accept([this](boost::system::error_code ec, ip::tcp::socket socket)
        {
            if (!ec)
            {
                auto ptr = std::make_shared<Session_Send>(std::move(socket));
                ptr->SetClose(std::bind(&BoostServer::Close_Session_Send, this));
                clients_send.push_back(ptr);
            }

            Async_Accept_Send();
        });
}


void BoostServer::Close_Session_Recv()
{
    for (auto ptr = clients_recv.begin(); ptr != clients_recv.end();)
    {
        if (false == ptr->get()->GetState())
        {
            ptr->reset();
            clients_recv.erase(ptr);
            continue;
        }
        ptr++;
    }
    std::cout << "Send_Data_Clients: " << clients_recv.size() << ", Recv_Data_Clients: " << clients_send.size() << std::endl;
}


void BoostServer::Close_Session_Send()
{
    for (auto ptr = clients_send.begin(); ptr != clients_send.end();)
    {
        if (false == ptr->get()->GetState())
        {
            ptr->reset();
            clients_send.erase(ptr);
            continue;
        }
        ptr++;
    }
    std::cout << "Send_Data_Clients: " << clients_recv.size() << ", Recv_Data_Clients: " << clients_send.size() << std::endl;
}


void BoostServer::GetContent(std::string content)
{
    if (content.empty())
    {
        return;
    }

    if (mtx.try_lock())
    {
        Array_messages.push(content);
        mtx.unlock();
    }
}


void BoostServer::SendMessage()
{
    std::string message = "";
    bStopSend = false;
    while (!bStopSend)
    {
        if (mtx.try_lock())
        {
            if (!Array_messages.empty())
            {
                message = Array_messages.front();
                Array_messages.pop();
            }
            mtx.unlock();
        }

        if (!message.empty())
        {
            for (auto ptr = clients_send.begin(); ptr != clients_send.end(); ptr++)
            {
                std::cout << "send message" << std::endl;
                ptr->get()->async_send(message);
            }

            message.clear();
            continue;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }
}

