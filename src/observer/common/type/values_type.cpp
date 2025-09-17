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
#include "common/type/values_type.h"
#include "common/value.h"
#include <cstdint>
#include <iomanip>

int ValuesType::compare(const Value &left, const Value &right) const
{
  ASSERT(left.attr_type() == AttrType::VALUESLISTS, "left type is not values");
  if (left.value_.values->size() != 1) {
    return INT32_MAX;  // values 数量不为1时，不等
  }
  return left.value_.values->front().compare(right);
}



RC ValuesType::to_string(const Value &val, string &result) const
{
  stringstream ss;
  ss << "unsupported";
  return RC::SUCCESS;
}


