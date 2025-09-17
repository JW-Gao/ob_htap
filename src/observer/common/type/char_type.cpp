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
#include "common/log/log.h"
#include "common/type/attr_type.h"
#include "common/type/char_type.h"
#include "common/value.h"
#include <cstdint>

int CharType::compare(const Value &left, const Value &right) const
{
  if (right.attr_type() == AttrType::NULLS) {
    return INT32_MAX;
  }
  ASSERT(left.attr_type() == AttrType::CHARS && right.attr_type() == AttrType::CHARS, "invalid type");
  return common::compare_string(
      (void *)left.value_.pointer_value_, left.length_, (void *)right.value_.pointer_value_, right.length_);
}

RC CharType::set_value_from_str(Value &val, const string &data) const
{
  val.set_string(data.c_str());
  return RC::SUCCESS;
}

RC CharType::cast_to(const Value &val, AttrType type, Value &result) const
{
  switch (type) {
    case AttrType::INTS:
    {
      std::string s = val.get_string();
      int ans = 0;
      int end = 0;
      while (end < s.length() && (s[end] >= '0' && s[end] <= '9')) 
        end++;
      for (int i = 0; i < end; i++) {
        ans = ans * 10 + (s[i] - '0');
      }
      result = Value(ans);
    }
    break;
    case AttrType::FLOATS : {
      std::string s = val.get_string();
      float int_part = 0.0, dec_part = 0.0;
      float ans = 0.0;
      int end = 0;
      int dot_point = -1;
      for (end = 0; end < s.length(); end++) {
        if (s[end] == '.') {
          if (end == 0 || dot_point > 0) 
            break;
          else {
            dot_point = end;
          }
        }
        else if (s[end] < '0' || s[end] > '9')
          break;
      }

      if (dot_point == -1 || (dot_point + 1 == end)) {
        for (int i = 0; i < end; i++)
          int_part = int_part * 10 + (s[i] - '0');
      }
      else {
        for (int i = 0; i < dot_point; i++) {
          int_part = int_part * 10 + (s[i] - '0');
        }
        for (int i = end - 1; i > dot_point; i--) {
          dec_part = dec_part * 0.1 + (s[i] - '0') * 0.1;
        }
      }
      ans = int_part + dec_part;
      result = Value(ans);
    }
    break;
    default: return RC::UNIMPLEMENTED;
  }
  return RC::SUCCESS;
}

int CharType::cast_cost(AttrType type)
{
  if (type == AttrType::CHARS) {
    return 0;
  }
  if (type == AttrType::INTS) {
    return 10;
  }
  if (type == AttrType::FLOATS) {
    return 10;
  }
  return INT32_MAX;
}

RC CharType::to_string(const Value &val, string &result) const
{
  stringstream ss;
  ss << val.value_.pointer_value_;
  result = ss.str();
  return RC::SUCCESS;
}