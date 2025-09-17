/* Copyright (c) 2021 OceanBase and/or its affiliates. All rights reserved.
miniob is licensed under Mulan PSL v2.
You can use this software according to the terms and conditions of the Mulan PSL v2.
You may obtain a copy of Mulan PSL v2 at:
         http://license.coscl.org.cn/MulanPSL2
THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
See the Mulan PSL v2 for more details. */

#pragma once

#include "common/type/attr_type.h"
#include "common/type/data_type.h"
#include <cstdint>

/**
 * @brief null类型
 * @ingroup DataType
 */
class HighVectorType : public DataType
{
public:
  HighVectorType() : DataType(AttrType::HIGH_VECTORS) {}
  virtual ~HighVectorType() {}

  // int compare(const Value &left, const Value &right) const override;


  // RC set_value_from_str(Value &val, const string &data) const override;
  int cast_cost(AttrType type) override {
    if (type == attr_type_) {
      return 0;
    }
    if (type == AttrType::VECTORS) {
      return 1;
    }
    return INT32_MAX;
  }
  
  RC cast_to(const Value &val, AttrType type, Value &result) const override;

  RC to_string(const Value &val, string &result) const override;
};