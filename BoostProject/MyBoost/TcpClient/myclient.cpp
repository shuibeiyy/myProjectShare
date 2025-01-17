#include <iostream>
#include <boost/array.hpp>
#include <boost/asio.hpp>
#include <boost/asio/buffer.hpp>

using namespace std;
using boost::asio::ip::tcp;

int main(int argc, char* argv[])
{
    try
    {
        // 定义Socket对象
        boost::asio::io_service io;
        tcp::socket socket(io);

        // 尝试连接服务器
        tcp::endpoint end_point(boost::asio::ip::address::from_string("127.0.0.1"), 6666);
        socket.connect(end_point);

        while (1)
        {
            
            boost::system::error_code error;

            // 接受数据并存入buffer
            boost::array<char, 1024> buffer = { 0 };
            size_t len = socket.read_some(boost::asio::buffer(buffer), error);

            // 判断是否出错
            if (error == boost::asio::error::eof)
                break;
            else if (error)
                throw boost::system::system_error(error);

            std::cout << "接收到数据: " << buffer.data() << std::endl;
        }
    }
    catch (std::exception& e)
    {
        cout << e.what() << endl;
    }
    system("pause");
    return 0;
}