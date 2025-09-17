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
#include "common/type/null_type.h"
#include "common/value.h"
#include <cstdint>
#include <iomanip>

int NullType::compare(const Value &left, const Value &right) const
{
  ASSERT(left.attr_type() == AttrType::NULLS, "left type is not nulls");
  if(left.attr_type() == AttrType::NULLS && right.attr_type() == AttrType::NULLS){
    return 0;
  }
  return -INT32_MAX;  // null 比任何都小,但在表达式中，却不可比，所以返回 负的INTMAX
}

RC NullType::set_value_from_str(Value &val, const string &data) const
{
  RC rc = RC::SUCCESS;
  if (data != "null") {
    rc = RC::SCHEMA_FIELD_TYPE_MISMATCH;
  } else {
    val.set_null();
  }
  return rc;
}

RC NullType::to_string(const Value &val, string &result) const
{
  stringstream ss;
  ss << "null";
  result = ss.str();
  return RC::SUCCESS;
}