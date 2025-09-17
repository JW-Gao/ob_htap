/* Copyright (c) 2021 OceanBase and/or its affiliates. All rights reserved.
miniob is licensed under Mulan PSL v2.
You can use this software according to the terms and conditions of the Mulan PSL v2.
You may obtain a copy of Mulan PSL v2 at:
         http://license.coscl.org.cn/MulanPSL2
THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
See the Mulan PSL v2 for more details. */

#include "common/lang/comparator.h"
#include "common/lang/sstream.h"
#include "common/log/log.h"
#include "common/type/attr_type.h"
#include "common/type/float_type.h"
#include "common/type/integer_type.h"
#include "common/value.h"

int IntegerType::compare(const Value &left, const Value &right) const
{
  if (right.attr_type() == AttrType::NULLS) {
    return INT32_MAX;
  }
  ASSERT(left.attr_type() == AttrType::INTS, "left type is not integer");
  ASSERT(right.attr_type() == AttrType::INTS || right.attr_type() == AttrType::FLOATS || right.attr_type() == AttrType::VALUESLISTS, "right type is not numeric");
  if (right.attr_type() == AttrType::INTS) {
    return common::compare_int((void *)&left.value_.int_value_, (void *)&right.value_.int_value_);
  } else if (right.attr_type() == AttrType::FLOATS) {
    float left_val  = left.get_float();
    float right_val = right.get_float();
    return common::compare_float((void *)&left_val, (void *)&right_val);
  } else if (right.attr_type() == AttrType::VALUESLISTS) {
    return -right.compare(left);
  }

  return INT32_MAX;
}

RC IntegerType::add(const Value &left, const Value &right, Value &result) const
{
  if (left.is_null() || right.is_null()) {
    result.set_null();
  } else {
    result.set_int(left.get_int() + right.get_int());
  }

  return RC::SUCCESS;
}

RC IntegerType::subtract(const Value &left, const Value &right, Value &result) const
{
  if (left.is_null() || right.is_null()) {
    result.set_null();
  } else {
    result.set_int(left.get_int() - right.get_int());
  }
  return RC::SUCCESS;
}

RC IntegerType::multiply(const Value &left, const Value &right, Value &result) const
{
  if (left.is_null() || right.is_null()) {
    result.set_null();
  } else {
    result.set_int(left.get_int() * right.get_int());
  }
  return RC::SUCCESS;
}

RC IntegerType::negative(const Value &val, Value &result) const
{
  if (val.is_null()) {
    result.set_null();
  } else {
    result.set_int(-val.get_int());
  }
  return RC::SUCCESS;
}

RC IntegerType::set_value_from_str(Value &val, const string &data) const
{
  RC           rc = RC::SUCCESS;
  stringstream deserialize_stream;
  deserialize_stream.clear();  // 清理stream的状态，防止多次解析出现异常
  deserialize_stream.str(data);
  int int_value;
  deserialize_stream >> int_value;
  if (!deserialize_stream || !deserialize_stream.eof()) {
    rc = RC::SCHEMA_FIELD_TYPE_MISMATCH;
  } else {
    val.set_int(int_value);
  }
  return rc;
}

RC IntegerType::to_string(const Value &val, string &result) const
{
  stringstream ss;
  ss << val.value_.int_value_;
  result = ss.str();
  return RC::SUCCESS;
}

RC IntegerType::cast_to(const Value &val, AttrType type, Value &result) const
{

  switch (type) {
    case AttrType::CHARS: {
      string char_value = val.get_string().c_str();
      result.set_string(char_value.c_str(), char_value.size());
      return RC::SUCCESS;
    }

    case AttrType::FLOATS: {
      float float_value = (float)(val.get_int());
      result.set_float(float_value);
      return RC::SUCCESS;
      break;
    }

    case AttrType::INTS: {
      result = val;
      return RC::SUCCESS;
    }
    case AttrType::BOOLEANS:
    case AttrType::DATES:
    case AttrType::NULLS:
    case AttrType::VALUESLISTS:
    case AttrType::MAXTYPE:

    default: return RC::UNIMPLEMENTED;
  }
}

int IntegerType::cast_cost(AttrType type)
{
  if (type == AttrType::FLOATS) {
    return 1;
  }
  return INT32_MAX;
}

RC IntegerType::divide(const Value &left, const Value &right, Value &result) const
{
  if (left.is_null() || right.is_null()) {
    result.set_null();
    return RC::SUCCESS;
  }
  if (right.get_float() > -EPSILON && right.get_float() < EPSILON) {
    // NOTE:
    // 设置为浮点数最大值是不正确的。通常的做法是设置为NULL，但是当前的miniob没有NULL概念，所以这里设置为浮点数最大值。
    result.set_null();
  } else {
    result.set_float(left.get_float() / right.get_float());
  }
  return RC::SUCCESS;
}