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
// Created by Wangyunlai on 2022/07/05.
//

#pragma once

#include <memory>
#include <string>
#include <vector>

#include "common/type/attr_type.h"
#include "common/value.h"
#include "sql/operator/physical_operator.h"
#include "sql/parser/parse_defs.h"
#include "sql/stmt/select_stmt.h"
#include "storage/field/field.h"
#include "sql/expr/aggregator.h"
#include "storage/common/chunk.h"
#include "sql/expr/expression.h"

// 单独写一个文件，是为了解决循环引用的问题
class SubSelectExpr : public Expression
{

public:
  SubSelectExpr(SelectStmt *subselect);
  virtual ~SubSelectExpr() = default;

  ExprType type() const override { return ExprType::SUBSELECT; }

  AttrType          value_type() const override { return AttrType::VALUESLISTS; }
  RC                get_value(const Tuple &tuple, Value &value) const override;
  RC                try_get_value(Value &value) const override;
  RC                set_father_tuple_for_physical_oper(Tuple *tuple, PhysicalOperator *oper) const;
  PhysicalOperator *physical_operator() const { return oper_.get(); }
  RC                field_meta(std::vector<AttrInfoSqlNode> &attr_infos, Db *db);
  RC                get_field_exprs(std::vector<Expression *> &field_exprs) const;

private:
  std::unique_ptr<SelectStmt>       subselect_;
  std::unique_ptr<PhysicalOperator> oper_;
  RC                                rc_;
};