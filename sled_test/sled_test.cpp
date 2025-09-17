#include "sled_db.h"
#include <iostream>
#include <memory>

int main() {
    // SledDB db("my_sled_db");
    std::shared_ptr<SledDB> db = std::make_shared<SledDB>("my_sled_db.table");
    // 插入数据
    if (db->insert("key1", "value1")) {
        std::cout << "Inserted key1:value1" << std::endl;
    } else {
        std::cout << "Failed to insert key1" << std::endl;
    }

    // 获取数据
    auto value = db->get("key1");
    if (value) {
        std::cout << "Retrieved key1:" << std::string(value->begin(), value->end()) << std::endl;
    } else {
        std::cout << "key1 not found" << std::endl;
    }

    // 删除数据
    if (db->remove("key1")) {
        std::cout << "Deleted key1" << std::endl;
    } else {
        std::cout << "Failed to delete key1" << std::endl;
    }

    return 0;
}