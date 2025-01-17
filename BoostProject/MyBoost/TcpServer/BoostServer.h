#include <memory>
#include <iostream>
#include <array>
#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <atomic>

#include <boost/asio.hpp>
#include <boost/thread.hpp>

using namespace boost::asio;

typedef std::function<void(void)> Callback_Close;
typedef std::function<void(std::string)> Callback_SendMessage;

// 接收客户端发送来的消息
class Session_Recv : public std::enable_shared_from_this<Session_Recv>
{
public:
    Session_Recv(ip::tcp::socket socket);

    void SetClose(Callback_Close pClose);
    void SetMsg_Ntf(Callback_SendMessage pMsg);
    void async_read();
    bool GetState();

private:
    ip::tcp::socket socket_;
    std::array<char, 1024> data_;
    std::atomic<bool> connected_;

    Callback_Close ptr_close_;
    Callback_SendMessage ptr_message_;
};

// 向客户端发送消息
class Session_Send : public std::enable_shared_from_this<Session_Send>
{
public:
    Session_Send(ip::tcp::socket socket);

    void SetClose(Callback_Close pClose);
    void async_send(std::string message);
    bool GetState();

private:
    ip::tcp::socket socket_;
    std::atomic<bool> connected_;

    Callback_Close ptr_close_;
};

// 服务器 接收port_recv端口发来的数据    向port_send端口发送数据
class BoostServer
{
public:
    BoostServer(boost::asio::io_context& ctx, short port_recv, short port_send, std::string ip_recv = "", std::string ip_send = "");

    void Close_Session_Recv();
    void Close_Session_Send();
    void GetContent(std::string content);

private:
    void Async_Accept_Recv();
    void Async_Accept_Send();

    void SendMessage();

private:
    std::shared_ptr<std::thread> ptr_Thread_Msg;
    std::shared_ptr<boost::asio::ip::tcp::acceptor> Acceptor_Recv_;
    std::shared_ptr<boost::asio::ip::tcp::acceptor> Acceptor_Send_;

    std::shared_ptr<ip::tcp::endpoint> ptr_Recv_ep;
    std::shared_ptr<ip::tcp::endpoint> ptr_Send_ep;

    std::vector<std::shared_ptr<Session_Recv>> clients_recv;
    std::vector<std::shared_ptr<Session_Send>> clients_send;

    std::queue<std::string> Array_messages;
    std::atomic<bool> bStopSend;
    std::mutex mtx;
};
