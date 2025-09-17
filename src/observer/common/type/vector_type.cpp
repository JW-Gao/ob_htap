/* Copyright (c) 2021 OceanBase and/or its affiliates. All rights reserved.
miniob is licensed under Mulan PSL v2.
You can use this software according to the terms and conditions of the Mulan PSL v2.
You may obtain a copy of Mulan PSL v2 at:
         http://license.coscl.org.cn/MulanPSL2
THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
See the Mulan PSL v2 for more details. */

#include "common/type/attr_type.h"
#include "common/type/values_type.h"
#include "common/value.h"
#include "common/type/vector_type.h"
#include <vector>

int VectorType::compare(const Value &left, const Value &right) const
{
  if (left.attr_type_ != AttrType::VECTORS || right.attr_type_ != AttrType::VECTORS) {
    return INT32_MAX;
  }

  for (int i = 0; i < left.length() / 4; i++) {
    Value left_tmp(left.get_vectors()[i]);
    Value right_tmp(right.get_vectors()[i]);
    int   result;
    if ((result = left_tmp.compare(right_tmp)) != 0) {
      return result;
    }
  }
  return 0;
}
RC VectorType::add(const Value &left, const Value &right, Value &result) const
{
  vector<float> tmp;
  for (int i = 0; i < left.length() / 4; i++) {
    tmp.push_back(left.get_vectors()[i] + right.get_vectors()[i]);
  }
  result.set_vecs(tmp);
  return RC::SUCCESS;
}
RC VectorType::subtract(const Value &left, const Value &right, Value &result) const
{
  vector<float> tmp;
  for (int i = 0; i < left.length() / 4; i++) {
    tmp.push_back(left.get_vectors()[i] - right.get_vectors()[i]);
  }
  result.set_vecs(tmp);
  return RC::SUCCESS;
}
RC VectorType::multiply(const Value &left, const Value &right, Value &result) const
{
  vector<float> tmp;
  for (int i = 0; i < left.length() / 4; i++) {
    tmp.push_back(left.get_vectors()[i] * right.get_vectors()[i]);
  }
  result.set_vecs(tmp);
  return RC::SUCCESS;
}
RC VectorType::to_string(const Value &val, string &result) const
{
  result = "[";

  for (int i = 0; i < val.length() / 4; i++) {
    result += common::double_to_str(((float *)val.value_.vectors)[i]) + ',';
  }
  result[result.size() - 1] = ']';

  return RC::SUCCESS;
}
