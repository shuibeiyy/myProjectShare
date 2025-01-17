#include<iostream>
#include"boost/asio.hpp"
using namespace std;
using namespace boost::asio;
int main() {
	io_context io;

	ip::tcp::socket sock(io);

	sock.connect(ip::tcp::endpoint(ip::address::from_string("127.0.0.1"), 6688));

	char buf[0xFF];
	while (true) {
		cin >> buf;
		sock.send(buffer(buf));
		memset(buf, 0, 0xFF);
		sock.receive(buffer(buf));
		cout << buf << endl;
	}
	sock.close();
	::system("pause");
}