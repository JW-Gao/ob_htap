#include "sled_db.h"
#include "sled_ffi.h" // 包含C FFI头文件
#include <iostream>

SledDB::SledDB(const std::string& path) {
    db_ptr_ = sled_open(path.c_str());
    if (!db_ptr_) {
        // 在实际项目中，这里应该抛出异常或进行更健壮的错误处理
        std::cerr << "Failed to open sled database at path: " << path << std::endl;
    }
}

SledDB::~SledDB() {
    if (db_ptr_) {
        sled_close(db_ptr_);
    }
}

bool SledDB::insert(const std::string& key, const std::string& value) {
    if (!db_ptr_) return false;
    SledResult result = sled_insert(
        db_ptr_,
        reinterpret_cast<const uint8_t*>(key.data()), key.length(),
        reinterpret_cast<const uint8_t*>(value.data()), value.length()
    );
    return result == SledResult::Success;
}

std::unique_ptr<std::vector<char>> SledDB::get(const std::string& key) {
    if (!db_ptr_) return nullptr;

    SledData data = sled_get(
        db_ptr_,
        reinterpret_cast<const uint8_t*>(key.data()), key.length()
    );

    if (data.ptr == nullptr) {
        return nullptr;
    }

    // 将数据从Rust端转移到C++端，并管理其生命周期
    auto vec = std::make_unique<std::vector<char>>(data.ptr, data.ptr + data.len);
    
    // 释放Rust分配的内存
    sled_free_data(data);

    return vec;
}

bool SledDB::remove(const std::string& key) {
    if (!db_ptr_) return false;
    SledResult result = sled_remove(
        db_ptr_,
        reinterpret_cast<const uint8_t*>(key.data()), key.length()
    );
    return result == SledResult::Success;
}