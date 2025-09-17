/* Copyright (c) 2021 OceanBase and/or its affiliates. All rights reserved.
miniob is licensed under Mulan PSL v2.
You can use this software according to the terms and conditions of the Mulan PSL v2.
You may obtain a copy of Mulan PSL v2 at:
         http://license.coscl.org.cn/MulanPSL2
THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
See the Mulan PSL v2 for more details. */

//
// Created by WangYunlai on 2023/06/28.
//

#include "common/value.h"

#include "common/lang/comparator.h"
#include "common/lang/exception.h"
#include "common/lang/sstream.h"
#include "common/lang/string.h"
#include "common/log/log.h"
#include "common/type/attr_type.h"
#include <cstdint>
#include <cstring>
#include <vector>
#include <regex>
#include "sql/expr/expression.h"

Value::Value(int val) { set_int(val); }

Value::Value(float val) { set_float(val); }

Value::Value(bool val) { set_boolean(val); }

Value::Value(const char *s, int len /*= 0*/)
{
  switch (len) {
    case -1: set_date(s); break;

    case -2: set_null(); break;

    case -3: set_vecs(s); break;

    default: set_string(s, len);
  }
}

Value::Value(std::vector<std::unique_ptr<Expression>> &value_exprs)
{
  set_valuelist();
  Value tmp_value;
  for (auto &expr : value_exprs) {
    expr->try_get_value(tmp_value);
    value_.values->push_back(tmp_value);
  }
}

Value::Value(const Value &other)
{
  this->attr_type_         = other.attr_type_;
  this->length_            = other.length_;
  this->text_file_handler_ = other.text_file_handler_;
  this->vector_handler_ = other.vector_handler_;

  switch (this->attr_type_) {
    case AttrType::CHARS: {
      this->own_data_ = other.own_data_;
      set_string_from_other(other);
    } break;

    case AttrType::VECTORS: {
      this->own_data_      = other.own_data_;
      this->value_.vectors = new char[other.length()];
      memcpy(this->value_.vectors, other.value_.vectors, other.length());

    } break;

    case AttrType::VALUESLISTS: {
      set_valuelist();
      for (auto &it : *other.value_.values) {
        value_.values->push_back(it);
      }
    } break;

    default: {
      this->own_data_       = other.own_data_;
      this->value_          = other.value_;
      this->date_formmat_s_ = other.date_formmat_s_;
    } break;
  }
}

Value::Value(Value &&other)
{
  this->attr_type_         = other.attr_type_;
  this->length_            = other.length_;
  this->own_data_          = other.own_data_;
  this->value_             = other.value_;
  this->date_formmat_s_    = other.date_formmat_s_;
  this->text_file_handler_ = other.text_file_handler_;
  other.text_file_handler_ = nullptr;
  this->vector_handler_ = other.vector_handler_;
  other.vector_handler_ = nullptr;
  other.date_formmat_s_.clear();
  other.own_data_ = false;
  other.length_   = 0;
}

Value &Value::operator=(const Value &other)
{
  if (this == &other) {
    return *this;
  }
  reset();
  this->attr_type_         = other.attr_type_;
  this->length_            = other.length_;
  this->text_file_handler_ = other.text_file_handler_;
  this->vector_handler_ = other.vector_handler_;

  switch (this->attr_type_) {
    case AttrType::CHARS: {
      this->own_data_ = other.own_data_;
      set_string_from_other(other);
    } break;

    case AttrType::VECTORS: {
      this->own_data_      = other.own_data_;
      this->value_.vectors = new char[other.length()];
      memcpy(this->value_.vectors, other.value_.vectors, other.length());

    } break;

    case AttrType::VALUESLISTS: {
      set_valuelist();
      for (auto &it : *other.value_.values) {
        value_.values->push_back(it);
      }
    } break;

    default: {
      this->own_data_       = other.own_data_;
      this->value_          = other.value_;
      this->date_formmat_s_ = other.date_formmat_s_;
    } break;
  }
  return *this;
}

Value &Value::operator=(Value &&other)
{
  if (this == &other) {
    return *this;
  }
  reset();
  this->attr_type_         = other.attr_type_;
  this->length_            = other.length_;
  this->own_data_          = other.own_data_;
  this->value_             = other.value_;
  this->text_file_handler_ = other.text_file_handler_;
  other.text_file_handler_ = nullptr;
  this->vector_handler_ = other.vector_handler_;
  other.vector_handler_ = nullptr;
  other.own_data_          = false;
  this->date_formmat_s_    = other.date_formmat_s_;
  other.date_formmat_s_.clear();
  other.length_ = 0;
  return *this;
}

void Value::reset()
{
  switch (attr_type_) {
    case AttrType::CHARS:
      if (own_data_ && value_.pointer_value_ != nullptr) {
        delete[] value_.pointer_value_;
        value_.pointer_value_ = nullptr;
      }
      break;

    case AttrType::VECTORS:
      if (own_data_ && value_.vectors != nullptr) {
        delete[] value_.vectors;
        value_.vectors = nullptr;
      }
      break;
    case AttrType::VALUESLISTS:

      if (own_data_ && value_.values != nullptr) {
        delete value_.values;
        own_data_     = false;
        value_.values = nullptr;
      }
      break;
    default: break;
  }

  text_file_handler_ = nullptr;
  vector_handler_ = nullptr;
  attr_type_ = AttrType::UNDEFINED;
  length_    = 0;
  own_data_  = false;
}

void Value::set_data(char *data, int length)
{
  switch (attr_type_) {
    case AttrType::CHARS: {
      set_string(data, length);
    } break;
    case AttrType::INTS: {
      value_.int_value_ = *(int *)data;
      length_           = length;
    } break;
    case AttrType::DATES: {
      value_.int_value_ = *(int *)data;
      length_           = length;
    } break;
    case AttrType::FLOATS: {
      value_.float_value_ = *(float *)data;
      length_             = length;
    } break;
    case AttrType::BOOLEANS: {
      value_.bool_value_ = *(int *)data != 0;
      length_            = length;
    } break;
    case AttrType::TEXTS: {
      value_.int_value_ = *(int *)data;
      length_           = length;
    } break;
    case AttrType::HIGH_VECTORS: {
      value_.int_value_ = *(int*)data;
      length_ = length;
    } break;

    // 这里默认已经Value已经调用过 set_valuelist() 函数
    case AttrType::VALUESLISTS: {
      value_.values->swap(*(std::vector<Value> *)data);
      delete (std::vector<Value> *)data;
    } break;
    case AttrType::VECTORS: {
      reset();
      attr_type_           = AttrType::VECTORS;
      this->own_data_      = true;
      length_              = length;
      this->value_.vectors = new char[length];
      memcpy(this->value_.vectors, data, length);
    } break;
    case AttrType::NULLS:  // 暂时不用做什么
    default: {
      LOG_WARN("unknown data type: %d", attr_type_);
    } break;
  }
}

void Value::set_int(int val)
{
  reset();
  attr_type_        = AttrType::INTS;
  value_.int_value_ = val;
  length_           = sizeof(val);
}

void Value::set_float(float val)
{
  reset();
  attr_type_          = AttrType::FLOATS;
  value_.float_value_ = val;
  length_             = sizeof(val);
}
void Value::set_boolean(bool val)
{
  reset();
  attr_type_         = AttrType::BOOLEANS;
  value_.bool_value_ = val;
  length_            = sizeof(val);
}

void Value::set_string(const char *s, int len /*= 0*/)
{
  reset();
  attr_type_ = AttrType::CHARS;
  if (s == nullptr) {
    value_.pointer_value_ = nullptr;
    length_               = 0;
  } else {
    own_data_ = true;
    if (len > 0) {
      len = strnlen(s, len);
    } else {
      len = strlen(s);
    }
    value_.pointer_value_ = new char[len + 1];
    length_               = len;
    memcpy(value_.pointer_value_, s, len);
    value_.pointer_value_[len] = '\0';
  }
}

bool check_date(int y, int m, int d)
{
  static int mon[] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
  bool       leap  = (y % 400 == 0 || (y % 100 && y % 4 == 0));
  return y > 0 && (m > 0) && (m <= 12) && (d > 0) && (d <= ((m == 2 && leap) ? 1 : 0) + mon[m]);
}

void Value::set_date(const char *s)
{
  attr_type_ = AttrType::DATES;
  int y, m, d;
  sscanf(s, "%d-%d-%d", &y, &m, &d);
  if (!check_date(y, m, d)) {
    attr_type_ = AttrType::CHARS;
  }
  int dv            = y * 10000 + m * 100 + d;
  value_.int_value_ = dv;
  length_           = 4;
}

void Value::set_vecs(const char *s) {
    reset();
    attr_type_ = AttrType::VECTORS;

    // 计算字符串中的向量个数
    int count = 0;
    for (const char *ptr = s; *ptr; ++ptr) {
        if (*ptr == ',') count++;
    }
    count++; // 最后一个数之后没有逗号，需加一

    // 预分配内存
    length_ = count * sizeof(float);
    value_.vectors = new char[length_];

    // 使用一个指向 char 的指针来填充内存
    float *vec_ptr = reinterpret_cast<float *>(value_.vectors);
    
    // 使用 std::strtok 来分割字符串
    char *s_copy = strdup(s); // 创建一个可修改的副本
    char *token = std::strtok(s_copy, ",");
    int index = 0;

    while (token != nullptr) {
        vec_ptr[index++] = std::stof(token); // 转换为 float
        token = std::strtok(nullptr, ","); // 获取下一个分割的字符串
    }

    free(s_copy); // 释放复制的字符串内存
    own_data_ = true;
}

void Value::set_vecs(vector<float> &vec_tmp)
{
  reset();
  attr_type_ = AttrType::VECTORS;
  string            temp;
  own_data_ = true;

  length_        = vec_tmp.size() * 4;
  value_.vectors = new char[vec_tmp.size() * sizeof(float)];
  // 使用 memcpy 将 vec 的数据拷贝到 int* 数组中
  std::memcpy(value_.vectors, vec_tmp.data(), vec_tmp.size() * sizeof(float));
}

void Value::set_null()
{
  reset();
  attr_type_        = AttrType::NULLS;
  value_.int_value_ = INT32_MAX;
  length_           = 4;
}

void Value::set_date(int val)
{
  reset();
  attr_type_        = AttrType::DATES;
  value_.int_value_ = val;
  length_           = sizeof(val);
}

void Value::set_valuelist()
{
  reset();
  attr_type_    = AttrType::VALUESLISTS;
  value_.values = new vector<Value>;
  length_       = sizeof(vector<Value> *);
  own_data_     = true;
}

void Value::set_value(const Value &value)
{
  switch (value.attr_type_) {
    case AttrType::INTS: {
      set_int(value.get_int());
    } break;
    case AttrType::DATES: {
      set_date(value.get_int());
    } break;
    case AttrType::FLOATS: {
      set_float(value.get_float());
    } break;
    case AttrType::CHARS: {
      set_string(value.get_string().c_str());
    } break;
    case AttrType::BOOLEANS: {
      set_boolean(value.get_boolean());
    } break;
    case AttrType::NULLS: {
      set_null();
    } break;
    case AttrType::TEXTS: {
      set_int(value.get_int());
    } break;
    case AttrType::HIGH_VECTORS: {
      set_int(value.get_int());
    } break;
    default: {
      ASSERT(false, "got an invalid value type");
    } break;
  }
}

void Value::set_string_from_other(const Value &other)
{
  ASSERT(attr_type_ == AttrType::CHARS, "attr type is not CHARS");
  if (own_data_ && other.value_.pointer_value_ != nullptr && length_ != 0) {
    this->value_.pointer_value_ = new char[this->length_ + 1];
    memcpy(this->value_.pointer_value_, other.value_.pointer_value_, this->length_);
    this->value_.pointer_value_[this->length_] = '\0';
  }
}

const char *Value::data() const
{
  switch (attr_type_) {
    case AttrType::CHARS: {
      return value_.pointer_value_;
    } break;
    case AttrType::VECTORS: {
      return value_.vectors;
    } break;
    case AttrType::VALUESLISTS: {
      return (char *)value_.values->data();
    } break;
    default: {
      return (const char *)&value_;
    } break;
  }
}

string Value::to_string() const
{
  string res;
  RC     rc = DataType::type_instance(this->attr_type_)->to_string(*this, res);
  if (OB_FAIL(rc)) {
    LOG_WARN("failed to convert value to string. type=%s", attr_type_to_string(this->attr_type_));
    return "";
  }
  return res;
}

int Value::compare(const Value &other) const
{
  return DataType::type_instance(this->attr_type_)->compare(*this, other);
}

int Value::get_int() const
{
  switch (attr_type_) {
    case AttrType::CHARS: {
      try {
        return (int)(std::stol(value_.pointer_value_));
      } catch (exception const &ex) {
        LOG_TRACE("failed to convert string to number. s=%s, ex=%s", value_.pointer_value_, ex.what());
        return 0;
      }
    }
    case AttrType::INTS: {
      return value_.int_value_;
    }
    case AttrType::DATES: {
      return value_.int_value_;
    }
    case AttrType::FLOATS: {
      return (int)(value_.float_value_);
    }
    case AttrType::BOOLEANS: {
      return (int)(value_.bool_value_);
    }
    case AttrType::TEXTS: {
      return value_.int_value_;
    }
    case AttrType::HIGH_VECTORS: {
      return value_.int_value_;
    }
    case AttrType::NULLS: {
      LOG_WARN("try to get null int. type=%d", attr_type_);
      return 0;
    }
    default: {
      LOG_WARN("unknown data type. type=%d", attr_type_);
      return 0;
    }
  }
  return 0;
}

float Value::get_float() const
{
  switch (attr_type_) {
    case AttrType::CHARS: {
      try {
        return std::stof(value_.pointer_value_);
      } catch (exception const &ex) {
        LOG_TRACE("failed to convert string to float. s=%s, ex=%s", value_.pointer_value_, ex.what());
        return 0.0;
      }
    } break;
    case AttrType::INTS: {
      return float(value_.int_value_);
    } break;
    case AttrType::FLOATS: {
      return value_.float_value_;
    } break;
    case AttrType::BOOLEANS: {
      return float(value_.bool_value_);
    } break;
    default: {
      LOG_WARN("unknown data type. type=%d", attr_type_);
      return 0;
    }
  }
  return 0;
}

string Value::get_string() const { return this->to_string(); }

bool Value::get_boolean() const
{
  switch (attr_type_) {
    case AttrType::CHARS: {
      try {
        float val = std::stof(value_.pointer_value_);
        if (val >= EPSILON || val <= -EPSILON) {
          return true;
        }

        int int_val = std::stol(value_.pointer_value_);
        if (int_val != 0) {
          return true;
        }

        return value_.pointer_value_ != nullptr;
      } catch (exception const &ex) {
        LOG_TRACE("failed to convert string to float or integer. s=%s, ex=%s", value_.pointer_value_, ex.what());
        return value_.pointer_value_ != nullptr;
      }
    } break;
    case AttrType::INTS: {
      return value_.int_value_ != 0;
    } break;
    case AttrType::FLOATS: {
      float val = value_.float_value_;
      return val >= EPSILON || val <= -EPSILON;
    } break;
    case AttrType::BOOLEANS: {
      return value_.bool_value_;
    } break;
    case AttrType::TEXTS: {
      return value_.int_value_ != 0;
    } break;
    default: {
      LOG_WARN("unknown data type. type=%d", attr_type_);
      return false;
    }
  }
  return false;
}

std::vector<Value> *Value::get_valuelist() { return value_.values; }

std::vector<Value> *Value::get_valuelist() const { return value_.values; }

TextFileHandler *Value::get_text_file_handler() { return text_file_handler_; }

TextFileHandler *Value::get_text_file_handler() const { return text_file_handler_; }

void Value::set_text_file_handler(TextFileHandler *text_file_handler) { text_file_handler_ = text_file_handler; }

bool Value::str_like(Value like_value) const
{
  std::string s1      = this->get_string();
  std::string s2      = like_value.get_string();
  std::string pattern = s2;
  // 替换 SQL 的 LIKE 模式为正则表达式模式
  // 转义 regex 特殊字符
  std::string regex_special_chars = "\\^.$|()[]*+?";
  for (char c : regex_special_chars) {
    std::string str_c(1, c);
    std::regex  r("\\" + str_c);
    pattern = std::regex_replace(pattern, r, "\\" + str_c);
  }
  // 替换 % 为 .*
  std::regex r1("%");
  pattern = std::regex_replace(pattern, r1, ".*");
  // 替换 _ 为 .
  std::regex r2("_");
  pattern = std::regex_replace(pattern, r2, ".");
  // 创建正则表达式
  std::regex r(pattern, std::regex::ECMAScript | std::regex::icase);
  // 匹配字符串
  return std::regex_match(s1, r);
}
