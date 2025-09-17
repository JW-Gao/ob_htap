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

#include "sql/expr/sub_select_expression.h"
#include "common/log/log.h"
#include "common/rc.h"
#include "common/type/attr_type.h"
#include "common/value.h"
#include "sql/expr/aggregator.h"
#include "sql/expr/expression.h"
#include "sql/expr/tuple.h"
#include "sql/expr/arithmetic_operator.hpp"
#include "sql/operator/father_tuple_physical_operator.h"
#include "sql/operator/logical_operator.h"
#include "sql/operator/physical_operator.h"
#include "sql/operator/project_physical_operator.h"
#include "sql/optimizer/logical_plan_generator.h"
#include "sql/optimizer/physical_plan_generator.h"
#include "sql/parser/parse_defs.h"
#include <cstddef>
#include <memory>
#include <vector>

using namespace std;

SubSelectExpr::SubSelectExpr(SelectStmt *subselect) : subselect_(std::unique_ptr<SelectStmt>(subselect))
{
  std::unique_ptr<LogicalOperator> logical_operator;
  LogicalPlanGenerator             logical_plan_generator;
  rc_ = logical_plan_generator.create_plan(subselect_.get(), logical_operator, true);

  PhysicalPlanGenerator Physical_plan_generator;
  rc_ = Physical_plan_generator.create(*logical_operator, oper_);
}
RC SubSelectExpr::get_value(const Tuple &tuple, Value &value) const
{
  if (OB_FAIL(rc_)) {
    return rc_;
  }
  RC rc = set_father_tuple_for_physical_oper(const_cast<Tuple *>(&tuple), oper_.get());

  if (OB_FAIL(rc)) {
    return rc;
  }
  TupleSchema tuple_schema;
  oper_->tuple_schema(tuple_schema);
  if (tuple_schema.cell_num() != 1) {
    return RC ::ERROR;
  }

  rc = oper_->open(nullptr);
  if (OB_FAIL(rc)) {
    return rc;
  }

  std::vector<Tuple *> tuples;

  value.set_valuelist();
  while (OB_SUCC(rc = oper_->next())) {
    Value value_tmp;
    oper_->current_tuple()->cell_at(0, value_tmp);
    value.get_valuelist()->push_back(value_tmp);
  }
  oper_->close();
  if (rc != RC::RECORD_EOF) {  // Rc 不为RECORD_EOF 说明，运行出错
    return rc;
  }

  return RC::SUCCESS;
}

// 为子查询补充，上层查询的元组
RC SubSelectExpr::set_father_tuple_for_physical_oper(Tuple *tuple, PhysicalOperator *oper) const
{
  if (oper->type() == PhysicalOperatorType::FATHER_TUPLE) {
    FatherTuplePhysicalOperator *father_tuple_oper = static_cast<FatherTuplePhysicalOperator *>(oper);
    father_tuple_oper->set_tuple(tuple);
    return RC::SUCCESS;
  }
  RC rc = RC::ERROR;

  for (auto &child : oper->children()) {
    rc = set_father_tuple_for_physical_oper(tuple, child.get());
    if (OB_SUCC(rc)) {
      break;
    }
  }
  return rc;
}

RC SubSelectExpr::try_get_value(Value &value) const
{
  RowTuple                     empty_tuple;
  const std::vector<FieldMeta> fields;
  empty_tuple.set_schema(subselect_->tables()[0].second, &fields, "empty_table");
  return get_value(empty_tuple, value);
}

RC SubSelectExpr::field_meta(std::vector<AttrInfoSqlNode> &attr_infos, Db *db)
{
  if (physical_operator()->type() != PhysicalOperatorType::PROJECT) {
    return RC::ERROR;
  }
  ProjectPhysicalOperator *oper = static_cast<ProjectPhysicalOperator *>(physical_operator());
  oper->tuple_meta(attr_infos, db);
  return RC::SUCCESS;
}

RC SubSelectExpr::get_field_exprs(std::vector<Expression *> &field_exprs) const
{
  if (physical_operator()->type() != PhysicalOperatorType::PROJECT) {
    return RC::ERROR;
  }
  ProjectPhysicalOperator *oper = static_cast<ProjectPhysicalOperator *>(physical_operator());
  for (auto &expr : oper->expressions()) {
    field_exprs.push_back(expr.get());
  }
  return RC::SUCCESS;
}