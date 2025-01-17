#include <iostream>
#include <bsoncxx/builder/stream/document.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>
#include <bsoncxx/json.hpp>

class Mongo
{
public:
    //查询数据
    void queryDate(mongocxx::collection coll)
    {
        // 创建查询条件
        bsoncxx::builder::stream::document filter_builder{};
        filter_builder << "name" << "John"; // 查询名为 "John" 的文档

        // 执行查询
        auto cursor = coll.find(filter_builder.view());

        // 处理查询结果
        for (auto&& doc : cursor) {
            //std::cout << bsoncxx::to_json(doc) << std::endl;

            // 使用 operator[] 获取字段，并进行类型转换
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

    //新增数据
    void newDate(mongocxx::collection coll)
    {
        // 创建要插入的文档
        bsoncxx::builder::stream::document document{};
        document << "name" << "triber" << "age" << 30 << "city" << "London";

        // 插入文档
        auto result = coll.insert_one(document.view());

        // 检查插入是否成功
        if (result) {
            std::cout << "Document inserted successfully. ObjectId: " << std::endl;
        }
        else {
            std::cerr << "Failed to insert document." << std::endl;
        }
    }

    //更新数据
    void update(mongocxx::collection coll)
    {
        // 创建查询条件
        bsoncxx::builder::stream::document filter_builder{};
        filter_builder << "name" << "John"; // 查询名为 "John" 的文档

        // 执行查询
        auto cursor = coll.find(filter_builder.view());

        // 处理查询结果
        for (auto&& doc : cursor) {
            auto id = doc["_id"]; // 获取文档的 _id
            auto age = doc["age"]; // 获取文档的 age 字段

            if (id && age) {
                // 更新年龄字段
                auto new_age = age.get_int32() + 1;

                // 创建更新文档
                bsoncxx::builder::stream::document update_builder{};
                update_builder << "$set" << bsoncxx::builder::stream::open_document
                    << "age" << new_age
                    << bsoncxx::builder::stream::close_document;

                // 创建查询条件，指定更新的文档
                bsoncxx::builder::stream::document query_builder{};
                query_builder << "_id" << id.get_oid();

                // 执行更新操作
                coll.update_one(query_builder.view(), update_builder.view());

                std::cout << "Document updated successfully." << std::endl;
            }
            else {
                std::cerr << "Failed to get field values." << std::endl;
            }
        }
    }

    //删除数据
    void deleteDate(mongocxx::collection coll)
    {
        // 创建查询条件
        bsoncxx::builder::stream::document filter_builder{};
        filter_builder << "name" << "triber"; // 查询名为 "John" 的文档

        // 执行删除操作
        auto result = coll.delete_one(filter_builder.view());

        // 检查删除是否成功
        if (result) {
            std::cout << "Document deleted successfully. Deleted count: " << result->deleted_count() << std::endl;
        }
        else {
            std::cerr << "Failed to delete document." << std::endl;
        }
    }
};


//mongocxx::instance inst{}; // 实例化驱动程序实例
//int main() {
//
//    mongocxx::uri uri("mongodb://localhost:27017/"); // MongoDB URI
//    mongocxx::client client{ uri }; // 创建 MongoDB 客户端
//
//    // 获取或创建数据库
//    mongocxx::database db = client["test_DB"];
//
//    // 获取或创建集合
//    mongocxx::collection coll = db["test_collection"];
//
//    //newDate(coll);//增
//    //update(coll);//改
//    //queryDate(coll);//查
//    //deleteDate(coll);//删
//
//    return 0;
//}
