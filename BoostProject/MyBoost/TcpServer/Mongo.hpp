#include <iostream>
#include <bsoncxx/builder/stream/document.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>

using boost::asio::ip::tcp;

class Mongo
{
public:
    //��ѯ����
    void queryDate(mongocxx::collection coll)
    {
        // ������ѯ����
        bsoncxx::builder::stream::document filter_builder{};
        filter_builder << "name" << "John"; // ��ѯ��Ϊ "John" ���ĵ�

        // ִ�в�ѯ
        auto cursor = coll.find(filter_builder.view());

        // �����ѯ���
        for (auto&& doc : cursor) {

            // ʹ�� operator[] ��ȡ�ֶΣ�����������ת��
            auto name = doc["name"];
            auto age = doc["age"];
            auto city = doc["city"];

            if (name && age && city) {

                std::cout << "Name: " << name.get_utf8().value.to_string() << std::endl;
                std::cout << "Age: " << age.get_int32() << std::endl;
                std::cout << "City: " << city.get_utf8().value.to_string() << std::endl;
                std::cout << "--------------------------" << std::endl;
            }
            else {
                std::cerr << "Failed to get field values." << std::endl;
            }
        }
    }

    //��������
    void newDate(mongocxx::collection coll, tcp::socket m_socket, chat_message m_read_msg)
    {
        //// ����Ҫ������ĵ�
        //bsoncxx::builder::stream::document document{};
        //document << "name" << "triber" << "age" << 30 << "city" << "London";
        //
        //// �����ĵ�
        //auto result = coll.insert_one(document.view());

        bsoncxx::builder::stream::document document{};
        document << "socket" << m_socket.remote_endpoint().address().to_string() << "content" << m_read_msg.data();
        bsoncxx::document::value doc_value = document.extract();
        const mongocxx::v_noabi::collection& const_coll = coll;
        auto result = const_cast<mongocxx::v_noabi::collection&>(const_coll).insert_one(doc_value.view());

        // �������Ƿ�ɹ�
        if (result) {
            std::cout << "Document inserted successfully. ObjectId: " << std::endl;
        }
        else {
            std::cerr << "Failed to insert document." << std::endl;
        }
    }

    //��������
    void update(mongocxx::collection coll)
    {
        // ������ѯ����
        bsoncxx::builder::stream::document filter_builder{};
        filter_builder << "name" << "John"; // ��ѯ��Ϊ "John" ���ĵ�

        // ִ�в�ѯ
        auto cursor = coll.find(filter_builder.view());

        // �����ѯ���
        for (auto&& doc : cursor) {
            auto id = doc["_id"]; // ��ȡ�ĵ��� _id
            auto age = doc["age"]; // ��ȡ�ĵ��� age �ֶ�

            if (id && age) {
                // ���������ֶ�
                auto new_age = age.get_int32() + 1;

                // ���������ĵ�
                bsoncxx::builder::stream::document update_builder{};
                update_builder << "$set" << bsoncxx::builder::stream::open_document
                    << "age" << new_age
                    << bsoncxx::builder::stream::close_document;

                // ������ѯ������ָ�����µ��ĵ�
                bsoncxx::builder::stream::document query_builder{};
                query_builder << "_id" << id.get_oid();

                // ִ�и��²���
                coll.update_one(query_builder.view(), update_builder.view());

                std::cout << "Document updated successfully." << std::endl;
            }
            else {
                std::cerr << "Failed to get field values." << std::endl;
            }
        }
    }

    //ɾ������
    void deleteDate(mongocxx::collection coll)
    {
        // ������ѯ����
        bsoncxx::builder::stream::document filter_builder{};
        filter_builder << "name" << "triber"; // ��ѯ��Ϊ "John" ���ĵ�

        // ִ��ɾ������
        auto result = coll.delete_one(filter_builder.view());

        // ���ɾ���Ƿ�ɹ�
        if (result) {
            std::cout << "Document deleted successfully. Deleted count: " << result->deleted_count() << std::endl;
        }
        else {
            std::cerr << "Failed to delete document." << std::endl;
        }
    }
};


//mongocxx::instance inst{}; // ʵ������������ʵ��
//int main() {
//
//    mongocxx::uri uri("mongodb://localhost:27017/"); // MongoDB URI
//    mongocxx::client client{ uri }; // ���� MongoDB �ͻ���
//
//    // ��ȡ�򴴽����ݿ�
//    mongocxx::database db = client["test_DB"];
//
//    // ��ȡ�򴴽�����
//    mongocxx::collection coll = db["test_collection"];
//
//    //newDate(coll);//��
//    //update(coll);//��
//    //queryDate(coll);//��
//    //deleteDate(coll);//ɾ
//
//    return 0;
//}
