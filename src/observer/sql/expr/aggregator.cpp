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
// Created by Wangyunlai on 2024/05/29.
//

#include "sql/expr/aggregator.h"
#include "common/log/log.h"
#include "common/type/attr_type.h"
#include "common/type/null_type.h"
#include "common/value.h"

RC SumAggregator::accumulate(const Value &value)
{
  if (value_.attr_type() == AttrType::UNDEFINED || value_.attr_type() == AttrType::NULLS) {
    value_ = value;
    return RC::SUCCESS;
  }

  ASSERT(value.attr_type() == value_.attr_type(), "type mismatch. value type: %s, value_.type: %s", 
        attr_type_to_string(value.attr_type()), attr_type_to_string(value_.attr_type()));
  if (value.attr_type() != AttrType::NULLS) {
    Value::add(value, value_, value_);
  }
  return RC::SUCCESS;
}

RC SumAggregator::evaluate(Value &result)
{
  if (value_.attr_type() == AttrType::UNDEFINED) {
    result.set_null();
  } else {
    result = value_;
  }

  return RC::SUCCESS;
}

RC MaxAggregator::accumulate(const Value &value)
{
  if (value_.attr_type() == AttrType::UNDEFINED || value_.attr_type() == AttrType::NULLS) {
    value_ = value;
    return RC::SUCCESS;
  }
  if (value.attr_type() == AttrType::NULLS) {
    return RC::SUCCESS;
  }

  ASSERT(value.attr_type() == value_.attr_type(), "type mismatch. value type: %s, value_.type: %s", 
        attr_type_to_string(value.attr_type()), attr_type_to_string(value_.attr_type()));

  if (value.compare(value_) > 0) {
    value_ = value;
  }
  return RC::SUCCESS;
}

RC MaxAggregator::evaluate(Value &result)
{
  if (value_.attr_type() == AttrType::UNDEFINED) {
    result.set_null();
  } else {
    result = value_;
  }
  return RC::SUCCESS;
}

RC MinAggregator::accumulate(const Value &value)
{
  if (value_.attr_type() == AttrType::UNDEFINED || value_.attr_type() == AttrType::NULLS) {
    value_ = value;
    return RC::SUCCESS;
  }
  if (value.attr_type() == AttrType::NULLS) {
    return RC::SUCCESS;
  }
  if (value.compare(value_) < 0) {
    value_ = value;
  }
  return RC::SUCCESS;
}

RC MinAggregator::evaluate(Value &result)
{
  if (value_.attr_type() == AttrType::UNDEFINED) {
    result.set_null();
  } else {
    result = value_;
  }
  return RC::SUCCESS;
}

RC CountAggregator::accumulate(const Value &value)
{
  if (value_.attr_type() == AttrType::UNDEFINED) {
    value_.set_type(AttrType::INTS);
  }

  Value value_1(1);
  if (value.attr_type() != AttrType::NULLS) {
    value_.add(value_, value_1, value_);
  }
  return RC::SUCCESS;
}

RC CountAggregator::evaluate(Value &result)
{
  if (value_.attr_type() == AttrType::UNDEFINED) {
    result = Value(0);
  } else {
    result = value_;
  }
  return RC::SUCCESS;
}

RC AvgAggregator::accumulate(const Value &value)
{
  if (value_.attr_type() == AttrType::UNDEFINED || value_.is_null()) {
    value_ = value;
    if (!value_.is_null()) {
      count++;
    }

    return RC::SUCCESS;
  }

  ASSERT(value.attr_type() == value_.attr_type(), "type mismatch. value type: %s, value_.type: %s", 
        attr_type_to_string(value.attr_type()), attr_type_to_string(value_.attr_type()));
  if (value.attr_type() != AttrType::NULLS) {
    Value::add(value, value_, value_);
    count++;
  }
  return RC::SUCCESS;
}

RC AvgAggregator::evaluate(Value &result)
{
  if (value_.attr_type() == AttrType::UNDEFINED) {
    value_.set_null();
  }
  if (value_.is_null()) {
    result = value_;
    return RC::SUCCESS;
  }

  if (value_.attr_type() == AttrType::INTS) {
    value_.cast_to(value_, AttrType::FLOATS, value_);
  }
  Value value_count((float)count);
  value_.divide(value_, value_count, value_);
  result = value_;
  return RC::SUCCESS;
}
