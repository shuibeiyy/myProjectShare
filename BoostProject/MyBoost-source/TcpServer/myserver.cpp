#include <iostream>
#include <boost/asio.hpp>
#include <boost/asio/buffer.hpp>

using namespace boost::asio;

int main(int argc, char* argv[])
{
    io_service io;
    ip::tcp::acceptor acceptor(io, ip::tcp::endpoint(ip::tcp::v4(), 6666));

    while (1)
    {
        // 创建 socket 对象
        ip::tcp::socket socket(io);

        // 等待客户端连接
        acceptor.accept(socket);

        // 显示客户端IP
        std::cout << "本机地址: " << socket.local_endpoint().address() << std::endl;
        std::cout << "客户端地址: " << socket.remote_endpoint().address() << std::endl;


        // 向客户端发送 hello lyshark
        boost::system::error_code error;
        socket.write_some(buffer("hello lyshark"), error);

        // 如果出错,输出错误提示
        if (error)
        {
            std::cout << boost::system::system_error(error).what() << std::endl;
            break;
        }
    }
    system("pause");
    return 0;
}