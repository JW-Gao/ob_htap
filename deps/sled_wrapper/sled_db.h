#pragma once

#include <string>
#include <vector>
#include <memory>

// fwd declaration
struct SledDb;

class SledDB {
public:
    // 构造函数：打开数据库
    explicit SledDB(const std::string& path);

    // 析构函数：自动关闭数据库
    ~SledDB();

    // 禁止拷贝和赋值
    SledDB(const SledDB&) = delete;
    SledDB& operator=(const SledDB&) = delete;

    // 插入键值对
    bool insert(const std::string& key, const std::string& value);

    // 获取值
    std::unique_ptr<std::vector<char>> get(const std::string& key);

    // 删除键
    bool remove(const std::string& key);

private:
    SledDb* db_ptr_ = nullptr;
};