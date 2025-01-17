int main(int argc, const char* argv[]) {

    try {
        if (argc != 3) {
            std::cerr << "Usage: chat_client <host> <port>\n";
            return 1;
        }

        boost::asio::io_service io_service;
        tcp::resolver resolver(io_service);
        auto endpoint_iterator = resolver.resolve({ argv[1], argv[2] });
        chat_client c(io_service, endpoint_iterator);

        std::thread t([&io_service]() {
            io_service.run();//这样，与io_service绑定的事件源的回调均在子线程上执行（这里指的是boost::asio::async_xxx中的lambda函数）。
            });

        char line[chat_message::max_body_length + 1] = "";
        while (std::cin.getline(line, chat_message::max_body_length + 1)) {
            chat_message msg;
            msg.body_length(std::strlen(line));
            std::memcpy(msg.body(), line, msg.body_length());
            msg.encode_header();
            c.write(msg);
        }

        c.close();//这里必须close，否则子线程中run不会退出，因为boost::asio::async_read事件源一直注册在io_service中.
        t.join();

    }
    catch (std::exception& e) {
        std::cerr << e.what() << std::endl;
    }

    return 0;
}