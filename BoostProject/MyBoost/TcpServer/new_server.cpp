#define _CRT_SECURE_NO_WARNINGS
#include "new_server.hpp"  

//#include "Mongo.hpp"

using boost::asio::ip::tcp;

using chat_message_queue = std::deque<chat_message>;

mongocxx::instance inst{};
mongocxx::collection coll;

//聊天成员
class chat_participant{
public:
    virtual ~chat_participant() {}
    virtual void deliver(const chat_message& msg) = 0;
};

typedef std::shared_ptr<chat_participant> chat_participant_ptr;

//聊天室  
class chat_room {
    //聊天室中所有成员 
    std::list<chat_participant_ptr> m_participants;
    //历史消息(最多100条)
    enum { max_recent_msgs = 100 };
    chat_message_queue m_recent_msgs;

public:
    void join(chat_participant_ptr participant) {
        m_participants.push_back(participant);
        for (auto& msg : m_recent_msgs) {
            participant->deliver(msg);
        }
    }

    //成员离开聊天室
    void leave(chat_participant_ptr participant) {
        m_participants.remove(participant);
    }

    //传递消息
    void deliver(const chat_message& msg, chat_participant_ptr sender) {
        m_recent_msgs.push_back(msg);
        while (m_recent_msgs.size() > max_recent_msgs) {
            m_recent_msgs.pop_front();
        }

        
        for (auto participant : m_participants) {
            //将消息分发给除了发送消息的客户端的其他客户端
            /*if (participant != sender) {
                participant->deliver(msg);
            }*/
            //所有客户端都能收到消息
            participant->deliver(msg);
        }
    }
};

//转化为utf8字符
std::string convert_to_utf8(const std::string& str, const std::string& charset) {
    return boost::locale::conv::to_utf<char>(str, charset);
}


//与客户端的聊天会话,即聊天成员（当接受客户端发送过来的请求后，就会创建一个chat_session，用它读写消息）
class chat_session : public chat_participant, public std::enable_shared_from_this<chat_session> {
    
    
public:

    tcp::socket m_socket;//与客户端建立连接的socket
    chat_room& m_room;//所属聊天室
    chat_message m_read_msg;//当前消息
    chat_message_queue m_write_msgs;//写给客户端的消息队列


    chat_session(boost::asio::io_service& io_service, chat_room& room) :
        m_socket(io_service), m_room(room) {
    }

    tcp::socket& socket() {
        return m_socket;
    }


    void start(mongocxx::collection coll){
        
        m_room.join(shared_from_this());//聊天室增加成员

        do_read_header(coll);//读取成员传递过来的消息头
    }

    virtual void deliver(const chat_message& msg) {
        //第一次调用时为false
        bool write_in_progress = !m_write_msgs.empty();
        //存放写给客户端的消息队列
        m_write_msgs.push_back(msg);

        if (!write_in_progress) {
            do_write();//将历史消息写给客户端
        }
    }

    //读取成员传递过来的消息头
    void do_read_header(mongocxx::collection coll) {
        auto sharedFromThis = shared_from_this();
        //异步读取4个字节
        boost::asio::async_read(m_socket,
            boost::asio::buffer(m_read_msg.data(), chat_message::header_length),
            [this, sharedFromThis, coll](boost::system::error_code ec, std::size_t length) {
                if (!ec && m_read_msg.decoder_header()) {
                    do_read_body(coll);
                }
                else {
                    m_room.leave(sharedFromThis);
                }
            });
    }

    //新增数据(增)
    void newDate(mongocxx::collection coll, tcp::socket& socket, chat_message read_msg)
    {
        //// 创建要插入的文档

        bsoncxx::builder::stream::document document{};
        std::string socketAddress = socket.remote_endpoint().address().to_string();
        //document << "socket" << bsoncxx::types::b_utf8{ socketAddress };
        //auto content = bsoncxx::types::b_utf8{ read_msg.body_content() };

        auto content = convert_to_utf8(read_msg.body_content(), "GBK");
        auto time = read_msg.body_time();
        document << "ip" << bsoncxx::types::b_utf8{ socketAddress } << "content" << bsoncxx::types::b_utf8{ content }
        << "time" << bsoncxx::types::b_date{std::chrono::milliseconds(time *1000)};
        //document << "socket:" << bsoncxx::types::b_utf8{ socketAddress } << "content:" << read_msg.body();
        bsoncxx::document::value doc_value = document.extract();


        // 插入文档
        auto result = coll.insert_one(doc_value.view());

    }


    //读取成员传递过来的消息体
    void do_read_body(mongocxx::collection coll) {
        auto sharedFromThis = shared_from_this();
        boost::asio::async_read(m_socket,
            boost::asio::buffer(m_read_msg.body(), m_read_msg.body_length()),
            [this, sharedFromThis, coll](boost::system::error_code ec, std::size_t length) {
                if (!ec) {
                    m_room.deliver(m_read_msg, sharedFromThis);
//-----------------------------------------------------------------------------
                    newDate(coll, m_socket, m_read_msg);

//-----------------------------------------------------------------------------
                    //继续读取下一条消息的
                    do_read_header(coll);
                }
                else {
                    m_room.leave(sharedFromThis);
                }
            });
    }

    //将m_write_msgs中的第一条写给客户端
    void do_write() {
        auto sharedFromThis = shared_from_this();
        boost::asio::async_write(m_socket,
            boost::asio::buffer(m_write_msgs.front().data(), m_write_msgs.front().length()),
            [this, sharedFromThis](boost::system::error_code ec, std::size_t length) {
                if (!ec) {
                    m_write_msgs.pop_front();
                    if (!m_write_msgs.empty()) {
                        do_write();
                    }
                }
                else {
                    m_room.leave(sharedFromThis);
                }
            });
    }
};

//聊天服务器，当接受客户端发送过来的请求后，就会创建一个chat_session，用它读写消息
class chat_server {
    boost::asio::io_service& m_io_service;
    tcp::acceptor m_acceptor;
    chat_room m_room;

public:

    chat_server(boost::asio::io_service& io_service,
        const tcp::endpoint& endpoint) : m_io_service(io_service),
        m_acceptor(io_service, endpoint) {
        do_accept();
    }




    //删除数据（删）
    void delDate(mongocxx::collection coll)
    {
        std::cout << "全部删除（a），还是根据ip删除（i）还是内容删除（c）？" << std::endl;
        char b;
        std::cin >> b;
        if (b == 'a')
        {
            coll.delete_many({});
            std::cout << "已经全部删除" << std::endl;
        }
        else if (b == 'i')
        {
            std::cout << "输入要删除的ip:" << std::endl;
            // 删除操作
            char a[20];
            std::cin >> a;
            bsoncxx::builder::stream::document filter_builder{};
            filter_builder << "ip" << a;  // 删除条件
            auto result = coll.delete_many(filter_builder.view());

            if (result) {
                std::cout << "查找并删除了" << result->deleted_count() << "项记录" << std::endl;
            }
            else {
                std::cout << "无记录\n";
            }
        }
        else if (b == 'c')
        {
            std::cout << "输入要删除的content:" << std::endl;
            // 删除操作

            const int N = 1e4;
            std::vector<char> a(N);
            std::string input;

            std::cin >> input;
            if (input.size() > N) {
                std::cerr << "Input is too large.\n";
                return;
            }

            std::copy(input.begin(), input.end(), a.begin());
            auto content = convert_to_utf8(a.data(), "GBK");
            if (content.size())
            {
                bsoncxx::builder::stream::document filter_builder{};
                filter_builder << "content" << bsoncxx::types::b_utf8{ content };  // 删除条件
                auto result = coll.delete_many(filter_builder.view());
                if (result) {
                    std::cout << "查找并删除了" << result->deleted_count() << "项记录" << std::endl;
                }
                else {
                    std::cout << "无记录\n";
                }
            }
            else std::cout << "content为空" << std::endl;

        }

    }

    //查询数据库中的内容（查）
    void queryDate(mongocxx::collection coll)
    {
        bsoncxx::builder::stream::document filter_builder;
        char c;
        std::cout << "是否需要根据条件查询？y/n" << std::endl;
        std::cin >> c;
        if (c == 'y')
        {
            std::cout << "请输入ip：" << std::endl;
            char a[20];
            std::cin >> a;
            //构建查询条件
            filter_builder << "ip" << a; // 查询条件：socket字段是10.20.111.7
            auto filter = filter_builder.view();

            // 执行查询操作
            auto cursor = coll.find(filter);

            for (auto&& doc : cursor) {
                // 处理查询结果
                std::string ip = boost::locale::conv::from_utf(doc["ip"].get_utf8().value.to_string(), "GBK");
                //std::string socket = doc["socket"].get_utf8().value.to_string();
                std::string content = boost::locale::conv::from_utf(doc["content"].get_utf8().value.to_string(), "GBK");
                //std::string content = doc["content"].get_utf8().value.to_string();
                std::cout << "ip: " << ip << "   " << "content: " << content << std::endl;
            }

        }
        else if (c == 'n')
        {
            //auto cursor = coll.find({});
            mongocxx::cursor cursor = coll.find({}); // 查询名称为'test'的文档


            for (auto&& doc : cursor) {

                // 处理查询结果
                std::string ip = boost::locale::conv::from_utf(doc["ip"].get_utf8().value.to_string(), "GBK");
                //std::string socket = doc["socket"].get_utf8().value.to_string();
                std::string content = boost::locale::conv::from_utf(doc["content"].get_utf8().value.to_string(), "GBK");
                //std::string content = doc["content"].get_utf8().value.to_string();
                std::cout << "ip:" << ip << "   " << "content:" << content << std::endl;
            }

        }
    }

    //更改数据（改）批量修改
    void update(mongocxx::collection coll)
    {
        std::cout << "根据ip更改（i）/根据content更改（c）" << std::endl;
        char b;
        std::cin >> b;
        if (b == 'i')
        {
            std::cout << "请输入想要更新的ip：" << std::endl;
            char a[20];
            std::cin >> a;
            bsoncxx::builder::stream::document filter_builder{};
            filter_builder << "ip" << a;
            std::cout << "需要修改的内容：ip（i）/content（c）" << std::endl;
            char input;
            std::cin >> input;
            if (input == 'i')
            {
                std::cout << "输入修改后的ip：" << std::endl;
                char ip_new[20];
                std::cin >> ip_new;
                bsoncxx::builder::stream::document update_builder{};
                update_builder << "$set" << bsoncxx::builder::stream::open_document
                    << "ip" << ip_new
                    << bsoncxx::builder::stream::close_document;  // 更新操作
                coll.update_many(filter_builder.view(), update_builder.view());
            }
            else if (input == 'c')
            {
                std::cout << "输入修改后的content：" << std::endl;

                const int N = 1e4;
                std::vector<char> a(N);
                std::string content_new;

                std::cin >> content_new;
                if (content_new.size() > N) {
                    std::cerr << "Input is too large.\n";
                    return;
                }

                std::copy(content_new.begin(), content_new.end(), a.begin());
                auto content = convert_to_utf8(a.data(), "GBK");
                if (content.size())
                {
                    bsoncxx::builder::stream::document update_builder{};
                    update_builder << "$set" << bsoncxx::builder::stream::open_document
                        << "content" << bsoncxx::types::b_utf8{ content }
                    << bsoncxx::builder::stream::close_document;  // 更新操作：将age字段设置为30
                    coll.update_many(filter_builder.view(), update_builder.view());
                }
            }
            std::cout << "更新成功" << std::endl;

        }
        else if (b == 'c')
        {
            std::cout << "请输入想要更新的content：" << std::endl;

            const int N = 1e4;
            std::vector<char> b(N);
            std::string content_input;

            std::cin >> content_input;
            if (content_input.size() > N) {
                std::cerr << "Input is too large.\n";
                return;
            }

            std::copy(content_input.begin(), content_input.end(), b.begin());
            auto content = convert_to_utf8(b.data(), "GBK");
            bsoncxx::builder::stream::document filter_builder{};
            filter_builder << "content" << bsoncxx::types::b_utf8{ content };

            std::cout << "需要修改的内容：ip（i）/content（c）" << std::endl;
            char input;
            std::cin >> input;
            if (input == 'i')
            {
                std::cout << "输入修改后的ip：" << std::endl;
                char ip_new[20];
                std::cin >> ip_new;
                bsoncxx::builder::stream::document update_builder{};
                update_builder << "$set" << bsoncxx::builder::stream::open_document
                    << "ip" << ip_new
                    << bsoncxx::builder::stream::close_document;  // 更新操作
                coll.update_many(filter_builder.view(), update_builder.view());
            }
            else if (input == 'c')
            {
                std::cout << "输入修改后的content：" << std::endl;

                const int N = 1e4;
                std::vector<char> a(N);
                std::string content_new;

                std::cin >> content_new;
                if (content_new.size() > N) {
                    std::cerr << "Input is too large.\n";
                    return;
                }

                std::copy(content_new.begin(), content_new.end(), a.begin());
                auto content = convert_to_utf8(a.data(), "GBK");
                if (content.size())
                {
                    bsoncxx::builder::stream::document update_builder{};
                    update_builder << "$set" << bsoncxx::builder::stream::open_document
                        << "content" << bsoncxx::types::b_utf8{ content }
                    << bsoncxx::builder::stream::close_document;  // 更新操作：将age字段设置为30
                    coll.update_many(filter_builder.view(), update_builder.view());
                }
            }
            std::cout << "更新成功" << std::endl;
        }
    }


    //输入要进行的操作
    void handle_command(const std::string& command)
    {
        // 解析命令并执行相应的数据库操作
        if (command == "delete") {
            delDate(coll);
        }
        else if (command == "query") {
            queryDate(coll);
        }
        else if (command == "update") {
            update(coll);
        }
        else if (command == "exit")
        {
            std::cout << "已退出...\n";
            exit(0);
        }
        else {
            std::cout << "Unknown command" << std::endl;
        }
    }

 

    //异步接受客户端的请求
    void do_accept() {
        auto session = std::make_shared<chat_session>(m_io_service, m_room);

        m_acceptor.async_accept(session->socket(), [this, session](boost::system::error_code ec) {
            if (!ec) {
                session->start(coll);
            }

            do_accept();
        });
    }

};

int main(int argc, const char* argv[]) {
    try {
        if (argc < 2) {
            std::cerr << "Usage: chat_server <port> [<port>...]\n";
            return 1;
        }

        mongocxx::uri uri("mongodb://localhost:27017/"); // MongoDB URI
        //std::shared_ptr<mongocxx::client> conn;
        mongocxx::client client{ uri }; // 创建 MongoDB 客户端

        // 获取或创建数据库
        mongocxx::database db = client["mongodb0"];

        // 获取或创建集合
        coll = db["list0"];


        boost::asio::io_service io_service;
        std::list<chat_server*> servers;
        for (int i = 1; i < argc; ++i) {
            tcp::endpoint endpoint(tcp::v4(), std::atoi(argv[i]));
            servers.push_back(new chat_server(io_service, endpoint));   //注意容器里保存的指针，程序最后要负责释放所指向的堆
        }

        std::thread command_thread([&]() {
            while (true) {
                std::string command;
                std::cout << "Enter a command:(query/delete/update/exit) " << std::endl;
                std::cin >> command;
                servers.front()->handle_command(command);
                //for (auto server : servers) {
                //    server->handle_command(command);
                //}
            }
        });

        io_service.run();


        // 确保在程序结束前 join 线程
        if (command_thread.joinable()) {
            command_thread.join();
        }
        //for (auto server : servers) {
        //    server->start_accepting_commands(); // 开始接受命令
        //}
    }
    catch (std::exception& e) {
        std::cout << e.what() << std::endl;
    }

    return 0;
}
