#include <iostream>
#include <bsoncxx/builder/stream/document.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>
#include <bsoncxx/json.hpp>

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
            //std::cout << bsoncxx::to_json(doc) << std::endl;

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
    void newDate(mongocxx::collection coll)
    {
        // ����Ҫ������ĵ�
        bsoncxx::builder::stream::document document{};
        document << "name" << "triber" << "age" << 30 << "city" << "London";

        // �����ĵ�
        auto result = coll.insert_one(document.view());

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
