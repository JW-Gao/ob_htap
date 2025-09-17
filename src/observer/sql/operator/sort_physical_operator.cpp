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
// Created by WangYunlai on 2022/6/27.
//

#include "sql/operator/sort_physical_operator.h"
#include "common/log/log.h"
#include "common/rc.h"
#include "sql/expr/tuple.h"
#include "sql/parser/parse_defs.h"
#include "sql/stmt/filter_stmt.h"
#include "storage/field/field.h"
#include "storage/record/record.h"
#include <execution>

SortPhysicalOperator::SortPhysicalOperator(
    std::vector<std::unique_ptr<Expression>> &&expressions, std::vector<OrderByType> order_by_type)
    : expressions_(std::move(expressions)), order_by_type_(order_by_type)
{}

inline bool compare(const Tuple *x, const Tuple *y, std::vector<std::unique_ptr<Expression>> &order_by_exprs,
    std::vector<OrderByType> &order_type)
{
  int count = 0;
  for (const auto &expression : order_by_exprs) {

    // 获取x和y的对应列的值
    Value value_x;
    expression->get_value(*x, value_x);
    Value value_y;
    expression->get_value(*y, value_y);

    // 比较x和y的值
    int cmp_result = value_x.compare(value_y);
    if (cmp_result != 0) {
      // 根据OrderByType决定返回值
      if (order_type[count] == ASC_SORT) {
        return cmp_result < 0;  // 升序时，x < y 返回 true
      }
      return cmp_result > 0;  // 降序时，x > y 返回 true
    }
    // 如果这个字段相等，继续比较下一个字段
    ++count;
  }
  return false;
}

RC SortPhysicalOperator::open(Trx *trx)
{
  if (children_.size() != 1) {
    LOG_WARN("Sort operator must has one child");
    return RC::INTERNAL;
  }

  std::unique_ptr<PhysicalOperator> &child = children_[0];
  RC                                 rc    = child->open(trx);
  if (rc != RC::SUCCESS) {
    LOG_WARN("failed to open child operator: %s", strrc(rc));
    return rc;
  }

  while (OB_SUCC(rc = child->next())) {
    tuple_set_.push_back(child->current_tuple());
  }

  std::sort(std::execution::par, tuple_set_.begin(), tuple_set_.end(), [&](const Tuple *x, const Tuple *y) {
    return compare(x, y, expressions_, order_by_type_);
  });

  return RC::SUCCESS;
}

RC SortPhysicalOperator::next()
{
  if (tuple_set_.empty()) {
    return RC::RECORD_EOF;
  }
  tuple_ = tuple_set_.front();
  tuple_set_.erase(tuple_set_.begin());
  return RC::SUCCESS;
}

RC SortPhysicalOperator::close()
{
  children_[0]->close();
  return RC::SUCCESS;
}

Tuple *SortPhysicalOperator::current_tuple() { return tuple_; }

RC SortPhysicalOperator::tuple_schema(TupleSchema &schema) const { return children_[0]->tuple_schema(schema); }
