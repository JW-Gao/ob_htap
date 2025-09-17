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

#pragma once

#include "sql/expr/expression.h"
#include "sql/operator/physical_operator.h"

class SortStmt;

/**
 * @brief 过滤/谓词物理算子
 * @ingroup PhysicalOperator
 */
class SortPhysicalOperator : public PhysicalOperator
{
public:
  SortPhysicalOperator(std::vector<std::unique_ptr<Expression>> &&expressions,  std::vector<OrderByType>   order_by_type);

  virtual ~SortPhysicalOperator() = default;

  PhysicalOperatorType type() const override { return PhysicalOperatorType::ORDER_BY; }

  RC open(Trx *trx) override;
  RC next() override;
  RC close() override;

  Tuple *current_tuple() override;

  RC tuple_schema(TupleSchema &schema) const override;

private:
  std::vector<std::unique_ptr<Expression>> expressions_;
  std::vector<OrderByType>                  order_by_type_;
  std::vector<Tuple *>                     tuple_set_;
  Tuple                                   *tuple_ = nullptr;
};
