#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include "sled_ffi.h"

// RAII 包装器，用于自动关闭数据库
struct SledDbDeleter {
    void operator()(SledDb* db) const {
        if (db) {
            sled_close(db);
        }
    }
};
using UniqueSledDb = std::unique_ptr<SledDb, SledDbDeleter>;

// RAII 包装器，用于自动释放Rust返回的数据
struct SledDataDeleter {
    void operator()(SledData* data) const {
        if (data && data->ptr) {
            sled_free_data(*data);
        }
        delete data; // 删除包装器本身
    }
};
using UniqueSledData = std::unique_ptr<SledData, SledDataDeleter>;

// 辅助函数，方便地将 std::string 转换为 (ptr, len)
template<typename T>
struct DataView {
    const T* ptr;
    size_t len;
};

DataView<uint8_t> to_data_view(const std::string& s) {
    return {reinterpret_cast<const uint8_t*>(s.data()), s.length()};
}

int main() {
    // 1. 打开数据库
    UniqueSledDb db(sled_open("./my_cpp_db"));
    if (!db) {
        std::cerr << "Failed to open sled database!" << std::endl;
        return 1;
    }
    std::cout << "Database opened successfully." << std::endl;

    // 2. 插入数据
    std::string key1 = "hello";
    std::string value1 = "world from c++";
    auto key1_view = to_data_view(key1);
    auto val1_view = to_data_view(value1);

    if (sled_insert(db.get(), key1_view.ptr, key1_view.len, val1_view.ptr, val1_view.len) == SledResult::Success) {
        std::cout << "Inserted: '" << key1 << "' -> '" << value1 << "'" << std::endl;
    } else {
        std::cerr << "Failed to insert data." << std::endl;
    }

    // 3. 获取数据
    std::cout << "Attempting to retrieve key: '" << key1 << "'" << std::endl;
    SledData retrieved_data = sled_get(db.get(), key1_view.ptr, key1_view.len);
    
    if (retrieved_data.ptr) {
        // 将Rust返回的数据转换为C++字符串
        std::string value_str(reinterpret_cast<const char*>(retrieved_data.ptr), retrieved_data.len);
        std::cout << "Retrieved: '" << key1 << "' -> '" << value_str << "'" << std::endl;
        // 释放内存
        sled_free_data(retrieved_data);
    } else {
        std::cout << "Key '" << key1 << "' not found." << std::endl;
    }

    // 4. 删除数据
    std::cout << "Removing key: '" << key1 << "'" << std::endl;
    sled_remove(db.get(), key1_view.ptr, key1_view.len);

    // 5. 再次获取以确认删除
    std::cout << "Attempting to retrieve key '" << key1 << "' again..." << std::endl;
    retrieved_data = sled_get(db.get(), key1_view.ptr, key1_view.len);
    if (!retrieved_data.ptr) {
        std::cout << "Key '" << key1 << "' not found, as expected." << std::endl;
    } else {
        std::cerr << "Key was not removed properly!" << std::endl;
        sled_free_data(retrieved_data);
    }
    
    // 6. 关闭数据库
    // 当 main 函数结束时，UniqueSledDb 的析构函数会自动调用 sled_close。
    std::cout << "Program finished. Database will be closed automatically by RAII." << std::endl;

    return 0;
}