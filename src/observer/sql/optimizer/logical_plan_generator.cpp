/* Copyright (c) 2023 OceanBase and/or its affiliates. All rights reserved.
miniob is licensed under Mulan PSL v2.
You can use this software according to the terms and conditions of the Mulan PSL v2.
You may obtain a copy of Mulan PSL v2 at:
         http://license.coscl.org.cn/MulanPSL2
THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
See the Mulan PSL v2 for more details. */

//
// Created by Wangyunlai on 2023/08/16.
//

#include "sql/optimizer/logical_plan_generator.h"

#include <common/log/log.h>
#include <cstddef>
#include <memory>

#include "common/rc.h"
#include "common/type/attr_type.h"
#include "common/value.h"
#include "sql/expr/expression.h"
#include "sql/expr/tuple.h"
#include "sql/operator/calc_logical_operator.h"
#include "sql/operator/delete_logical_operator.h"
#include "sql/operator/father_tuple_logical_operator.h"
#include "sql/operator/update_logical_operator.h"
#include "sql/operator/explain_logical_operator.h"
#include "sql/operator/insert_logical_operator.h"
#include "sql/operator/join_logical_operator.h"
#include "sql/operator/logical_operator.h"
#include "sql/operator/predicate_logical_operator.h"
#include "sql/operator/sort_logical_operator.h"
#include "sql/operator/project_logical_operator.h"
#include "sql/operator/table_get_logical_operator.h"
#include "sql/operator/group_by_logical_operator.h"
#include "sql/operator/limit_logical_operator.h"
#include "sql/parser/parse_defs.h"
#include "sql/stmt/calc_stmt.h"
#include "sql/stmt/delete_stmt.h"
#include "sql/stmt/explain_stmt.h"
#include "sql/stmt/filter_stmt.h"
#include "sql/stmt/insert_stmt.h"
#include "sql/stmt/select_stmt.h"
#include "sql/stmt/stmt.h"
#include "src/observer/storage/index/index.h"
#include "sql/expr/expression_iterator.h"
#include "sql/stmt/update_stmt.h"
#include <unordered_set>

using namespace std;
using namespace common;

RC LogicalPlanGenerator::create(Stmt *stmt, unique_ptr<LogicalOperator> &logical_operator)
{
  RC rc = RC::SUCCESS;
  switch (stmt->type()) {
    case StmtType::CALC: {
      CalcStmt *calc_stmt = static_cast<CalcStmt *>(stmt);

      rc = create_plan(calc_stmt, logical_operator);
    } break;

    case StmtType::SELECT: {
      SelectStmt *select_stmt = static_cast<SelectStmt *>(stmt);

      rc = create_plan(select_stmt, logical_operator);
    } break;

    case StmtType::UPDATE: {
      UpdateStmt *update_stmt = static_cast<UpdateStmt *>(stmt);

      rc = create_plan(update_stmt, logical_operator);
    } break;

    case StmtType::INSERT: {
      InsertStmt *insert_stmt = static_cast<InsertStmt *>(stmt);

      rc = create_plan(insert_stmt, logical_operator);
    } break;

    case StmtType::DELETE: {
      DeleteStmt *delete_stmt = static_cast<DeleteStmt *>(stmt);

      rc = create_plan(delete_stmt, logical_operator);
    } break;

    case StmtType::EXPLAIN: {
      ExplainStmt *explain_stmt = static_cast<ExplainStmt *>(stmt);

      rc = create_plan(explain_stmt, logical_operator);
    } break;
    default: {
      rc = RC::UNIMPLEMENTED;
    }
  }
  return rc;
}

RC LogicalPlanGenerator::create_plan(CalcStmt *calc_stmt, std::unique_ptr<LogicalOperator> &logical_operator)
{
  logical_operator.reset(new CalcLogicalOperator(std::move(calc_stmt->expressions())));
  return RC::SUCCESS;
}

RC LogicalPlanGenerator::create_plan(
    SelectStmt *select_stmt, unique_ptr<LogicalOperator> &logical_operator, bool is_sub_select)
{
  unique_ptr<LogicalOperator> *last_oper = nullptr;

  unique_ptr<LogicalOperator> table_oper(nullptr);
  last_oper = &table_oper;

  const std::vector<pair<string, Table *>> &tables = select_stmt->tables();

  std::unordered_set<std::string> table_set;
  FilterStmt                     *filter_stmt = select_stmt->filter_stmt();

  auto check = [&](FilterUnit *cond) -> bool {
    bool ans = true;
    if (cond->left().expr->type() != ExprType::FIELD && cond->left().expr->type() != ExprType::VALUE) {
      return false;
    }
    if (cond->right().expr->type() != ExprType::FIELD && cond->right().expr->type() != ExprType::VALUE) {
      return false;
    }
    if (cond->left().expr->type() == ExprType::FIELD) {
      FieldExpr *field_expr = dynamic_cast<FieldExpr *>(cond->left().expr.get());
      ans &= (table_set.count(field_expr->table_name()));
    }
    if (cond->right().expr->type() == ExprType::FIELD) {
      FieldExpr *field_expr = dynamic_cast<FieldExpr *>(cond->right().expr.get());
      ans &= (table_set.count(field_expr->table_name()));
    }
    return ans;
  };

  for (auto &table : tables) {
    const std::vector<FilterUnit *> &filter_units = filter_stmt->filter_units();

    unique_ptr<LogicalOperator> table_get_oper(
        new TableGetLogicalOperator(table.second, ReadWriteMode::READ_ONLY, table.first));

    table_set.insert(table.first);

    FilterStmt *cur_stmt = new FilterStmt;
    for (int i = 0; i < filter_units.size(); ++i) {
      if (check(filter_units[i])) {
        cur_stmt->add_filter_unit(filter_units[i]);
        filter_stmt->remove_filter_unit(i);
        i--;
      }
    }

    if (table_oper == nullptr) {
      table_oper = std::move(table_get_oper);
    } else {
      JoinLogicalOperator *join_oper = new JoinLogicalOperator;
      join_oper->add_child(std::move(table_oper));
      join_oper->add_child(std::move(table_get_oper));
      table_oper = unique_ptr<LogicalOperator>(join_oper);
    }

    std::unique_ptr<LogicalOperator> predicate_oper;
    RC                               rc = create_plan(cur_stmt, predicate_oper);
    if (OB_FAIL(rc)) {
      LOG_WARN("failed to create predicate logical plan. rc=%s", strrc(rc));
      return rc;
    }
    if (predicate_oper) {
      predicate_oper->add_child(std::move(table_oper));
      table_oper = std::move(predicate_oper);
    }
  }

  // zrk, 添加父tuple
  unique_ptr<LogicalOperator> joind_father_tuple_oper;  // 将 father_tuple 与 原始的表join ，实现复杂子查询
  if (is_sub_select) {
    unique_ptr<LogicalOperator> father_tuple_oper = make_unique<FatherTupleLogicalOperator>();
    JoinLogicalOperator        *join_oper         = new JoinLogicalOperator;
    // ***** 这里注意，由于目前的逻辑是，join的右孩子只能 open 和 close 一次，
    // 而father_tuple_oper每次open的返回的tuple不一致，所以必须把father_tuple_oper放到左孩子
    join_oper->add_child(std::move(father_tuple_oper));
    join_oper->add_child(std::move(*last_oper));
    joind_father_tuple_oper = unique_ptr<LogicalOperator>(join_oper);
    last_oper               = &joind_father_tuple_oper;
  }

  unique_ptr<LogicalOperator> predicate_oper;

  RC rc = create_plan(select_stmt->filter_stmt(), predicate_oper);
  if (OB_FAIL(rc)) {
    LOG_WARN("failed to create predicate logical plan. rc=%s", strrc(rc));
    return rc;
  }

  if (predicate_oper) {
    if (*last_oper) {
      predicate_oper->add_child(std::move(*last_oper));
    }

    last_oper = &predicate_oper;
  }

  unique_ptr<LogicalOperator> sort_oper;
  if (!select_stmt->order_by().order_by_attrs.empty()) {
    sort_oper = make_unique<SortLogicalOperator>(
        std::move(select_stmt->order_by().order_by_attrs), select_stmt->order_by().order_by_types);
    sort_oper->add_child(std::move(*last_oper));
    last_oper = &sort_oper;
  }

  unique_ptr<LogicalOperator> limit_oper;
  if (select_stmt->limit() >= 0) {
    limit_oper = make_unique<LimitLogicalOperator>(std::move(select_stmt->limit()));
    limit_oper->add_child(std::move(*last_oper));
    last_oper = &limit_oper;
  }

  unique_ptr<LogicalOperator> group_by_oper;
  rc = create_group_by_plan(select_stmt, group_by_oper);
  if (OB_FAIL(rc)) {
    LOG_WARN("failed to create group by logical plan. rc=%s", strrc(rc));
    return rc;
  }

  if (group_by_oper) {
    if (*last_oper) {
      group_by_oper->add_child(std::move(*last_oper));
    }
    last_oper = &group_by_oper;
  }

  unique_ptr<LogicalOperator> group_by_predicate_oper;
  rc = create_plan(select_stmt->group_by_filter_stmt(), group_by_predicate_oper);
  if (OB_FAIL(rc)) {
    LOG_WARN("failed to create predicate logical plan. rc=%s", strrc(rc));
    return rc;
  }
  if (group_by_predicate_oper) {
    if (*last_oper) {
      group_by_predicate_oper->add_child(std::move(*last_oper));
    }
    last_oper = &group_by_predicate_oper;
  }

  auto project_oper = make_unique<ProjectLogicalOperator>(std::move(select_stmt->query_expressions()));
  if (*last_oper) {
    project_oper->add_child(std::move(*last_oper));
  }

  logical_operator = std::move(project_oper);
  return RC::SUCCESS;
}

RC LogicalPlanGenerator::create_plan(UpdateStmt *update_stmt, unique_ptr<LogicalOperator> &logical_operator)
{
  unique_ptr<LogicalOperator> *last_oper = nullptr;

  unique_ptr<LogicalOperator> table_oper(nullptr);
  last_oper = &table_oper;
  unique_ptr<LogicalOperator> table_get_oper(
      new TableGetLogicalOperator(update_stmt->table(), ReadWriteMode::READ_WRITE));

  if (table_oper == nullptr) {
    table_oper = std::move(table_get_oper);
  }

  unique_ptr<LogicalOperator> predicate_oper;

  RC rc = create_plan(update_stmt->filter_stmt(), predicate_oper);
  if (OB_FAIL(rc)) {
    LOG_WARN("failed to create predicate logical plan. rc=%s", strrc(rc));
    return rc;
  }

  if (predicate_oper) {
    if (*last_oper) {
      predicate_oper->add_child(std::move(*last_oper));
    }

    last_oper = &predicate_oper;
  }

  unique_ptr<LogicalOperator> update_oper(new UpdateLogicalOperator(update_stmt->table(), update_stmt->values()));
  update_oper->add_child(std::move(*last_oper));
  last_oper = &update_oper;

  logical_operator = std::move(*last_oper);
  return RC::SUCCESS;
}

RC LogicalPlanGenerator::create_plan(FilterStmt *filter_stmt, unique_ptr<LogicalOperator> &logical_operator)
{
  RC                                  rc = RC::SUCCESS;
  std::vector<unique_ptr<Expression>> cmp_exprs;
  const std::vector<FilterUnit *>    &filter_units = filter_stmt->filter_units();
  for (FilterUnit *filter_unit : filter_units) {
    FilterObj &filter_obj_left  = filter_unit->left();
    FilterObj &filter_obj_right = filter_unit->right();

    unique_ptr<Expression> left = std::move(filter_obj_left.expression());

    unique_ptr<Expression> right = std::move(filter_obj_right.expression());
    switch (filter_unit->comp()) {
      case IS_NULL:
      case IS_NOT: break;

      case LIKE_OP:
      case NOT_LIKE_OP:
        if (right->value_type() != AttrType::CHARS || left->value_type() != AttrType::CHARS) {
          return RC::ERROR;
        }
        break;

      case IN_VALUELIST:
        if (right->value_type() != AttrType::VALUESLISTS) {
          return RC::UNSUPPORTED;
        }
        break;
      case NOT_IN_VALUELIST:
        if (right->value_type() != AttrType::VALUESLISTS) {
          return RC::UNSUPPORTED;
        }
        break;

      default:  // 普通的大于小于
        if (left->value_type() == AttrType ::VALUESLISTS || right->value_type() == AttrType ::VALUESLISTS) {
          break;
        }

        if (left->value_type() != right->value_type() && right->value_type() != AttrType::NULLS &&
            left->value_type() != AttrType::NULLS) {
          auto left_to_right_cost = implicit_cast_cost(left->value_type(), right->value_type());
          auto right_to_left_cost = implicit_cast_cost(right->value_type(), left->value_type());
          if (left_to_right_cost <= right_to_left_cost && left_to_right_cost != INT32_MAX) {
            ExprType left_type = left->type();
            auto     cast_expr = make_unique<CastExpr>(std::move(left), right->value_type());
            if (left_type == ExprType::VALUE) {
              Value left_val;
              if (OB_FAIL(rc = cast_expr->try_get_value(left_val))) {
                LOG_WARN("failed to get value from left child", strrc(rc));
                return rc;
              }
              left = make_unique<ValueExpr>(left_val);
            } else {
              left = std::move(cast_expr);
            }
          } else if (right_to_left_cost < left_to_right_cost && right_to_left_cost != INT32_MAX) {
            ExprType right_type = right->type();
            auto     cast_expr  = make_unique<CastExpr>(std::move(right), left->value_type());
            if (right_type == ExprType::VALUE) {
              Value right_val;
              if (OB_FAIL(rc = cast_expr->try_get_value(right_val))) {
                LOG_WARN("failed to get value from right child", strrc(rc));
                return rc;
              }
              right = make_unique<ValueExpr>(right_val);
            } else {
              right = std::move(cast_expr);
            }

          } else {
            rc = RC::UNSUPPORTED;
            LOG_WARN("unsupported cast from %s to %s", attr_type_to_string(left->value_type()), attr_type_to_string(right->value_type()));
            return rc;
          }
        };
    }

    ComparisonExpr *cmp_expr = new ComparisonExpr(filter_unit->comp(), std::move(left), std::move(right));
    cmp_exprs.emplace_back(cmp_expr);
  }

  unique_ptr<PredicateLogicalOperator> predicate_oper;

  // 目前ConjunctionExpr::Type 要么全为AND，要么全为OR.这里不能取filter_units 的第一个，因为一定为AND
  if (!cmp_exprs.empty()) {
    ConjunctionExpr::Type       type = filter_stmt->filter_units().back()->conjunction_type() == 0
                                           ? ConjunctionExpr::Type::AND
                                           : ConjunctionExpr::Type::OR;
    unique_ptr<ConjunctionExpr> conjunction_expr(new ConjunctionExpr(type, cmp_exprs));
    predicate_oper = unique_ptr<PredicateLogicalOperator>(new PredicateLogicalOperator(std::move(conjunction_expr)));
  }

  logical_operator = std::move(predicate_oper);
  return rc;
}

int LogicalPlanGenerator::implicit_cast_cost(AttrType from, AttrType to)
{
  if (from == to) {
    return 0;
  }
  return DataType::type_instance(from)->cast_cost(to);
}

RC LogicalPlanGenerator::create_plan(InsertStmt *insert_stmt, unique_ptr<LogicalOperator> &logical_operator)
{
  Table        *table = insert_stmt->table();
  vector<Value> values;
  values.swap(insert_stmt->values()[0]);

  InsertLogicalOperator *insert_operator = new InsertLogicalOperator(table, values);
  logical_operator.reset(insert_operator);
  return RC::SUCCESS;
}

RC LogicalPlanGenerator::create_plan(DeleteStmt *delete_stmt, unique_ptr<LogicalOperator> &logical_operator)
{
  Table                      *table       = delete_stmt->table();
  FilterStmt                 *filter_stmt = delete_stmt->filter_stmt();
  unique_ptr<LogicalOperator> table_get_oper(new TableGetLogicalOperator(table, ReadWriteMode::READ_WRITE));

  unique_ptr<LogicalOperator> predicate_oper;

  RC rc = create_plan(filter_stmt, predicate_oper);
  if (rc != RC::SUCCESS) {
    return rc;
  }

  unique_ptr<LogicalOperator> delete_oper(new DeleteLogicalOperator(table));

  if (predicate_oper) {
    predicate_oper->add_child(std::move(table_get_oper));
    delete_oper->add_child(std::move(predicate_oper));
  } else {
    delete_oper->add_child(std::move(table_get_oper));
  }

  logical_operator = std::move(delete_oper);
  return rc;
}

RC LogicalPlanGenerator::create_plan(ExplainStmt *explain_stmt, unique_ptr<LogicalOperator> &logical_operator)
{
  unique_ptr<LogicalOperator> child_oper;

  Stmt *child_stmt = explain_stmt->child();

  RC rc = create(child_stmt, child_oper);
  if (rc != RC::SUCCESS) {
    LOG_WARN("failed to create explain's child operator. rc=%s", strrc(rc));
    return rc;
  }

  logical_operator = unique_ptr<LogicalOperator>(new ExplainLogicalOperator);
  logical_operator->add_child(std::move(child_oper));
  return rc;
}

RC LogicalPlanGenerator::create_group_by_plan(SelectStmt *select_stmt, unique_ptr<LogicalOperator> &logical_operator)
{
  vector<unique_ptr<Expression>>             &group_by_expressions = select_stmt->group_by();
  vector<Expression *>                        aggregate_expressions;
  vector<unique_ptr<Expression>>             &query_expressions = select_stmt->query_expressions();
  function<RC(std::unique_ptr<Expression> &)> collector         = [&](unique_ptr<Expression> &expr) -> RC {
    RC rc = RC::SUCCESS;
    if (expr->type() == ExprType::AGGREGATION) {
      expr->set_pos(aggregate_expressions.size() + group_by_expressions.size());
      aggregate_expressions.push_back(expr.get());
    }
    rc = ExpressionIterator::iterate_child_expr(*expr, collector);
    return rc;
  };

  function<RC(std::unique_ptr<Expression> &)> bind_group_by_expr = [&](unique_ptr<Expression> &expr) -> RC {
    RC rc = RC::SUCCESS;
    for (size_t i = 0; i < group_by_expressions.size(); i++) {
      auto &group_by = group_by_expressions[i];
      if (expr->type() == ExprType::AGGREGATION) {
        break;
      } else if (expr->equal(*group_by)) {
        expr->set_pos(i);
        continue;
      } else {
        rc = ExpressionIterator::iterate_child_expr(*expr, bind_group_by_expr);
      }
    }
    return rc;
  };

  bool                                        found_unbound_column = false;
  function<RC(std::unique_ptr<Expression> &)> find_unbound_column  = [&](unique_ptr<Expression> &expr) -> RC {
    RC rc = RC::SUCCESS;
    if (expr->type() == ExprType::AGGREGATION) {
      // do nothing
    } else if (expr->pos() != -1) {
      // do nothing
    } else if (expr->type() == ExprType::FIELD) {
      found_unbound_column = true;
    } else {
      rc = ExpressionIterator::iterate_child_expr(*expr, find_unbound_column);
    }
    return rc;
  };

  for (unique_ptr<Expression> &expression : query_expressions) {
    bind_group_by_expr(expression);
  }

  for (unique_ptr<Expression> &expression : query_expressions) {
    find_unbound_column(expression);
  }

  // collect all aggregate expressions
  for (unique_ptr<Expression> &expression : query_expressions) {
    collector(expression);
  }

  // collect aggregate expressions in having_conditions

  for (auto &filter_unit : select_stmt->group_by_filter_stmt()->filter_units()) {
    collector(filter_unit->left().expr);
    collector(filter_unit->right().expr);
  }

  if (group_by_expressions.empty() && aggregate_expressions.empty()) {
    // 既没有group by也没有聚合函数，不需要group by
    return RC::SUCCESS;
  }

  if (found_unbound_column) {
    LOG_WARN("column must appear in the GROUP BY clause or must be part of an aggregate function");
    return RC::INVALID_ARGUMENT;
  }

  // 如果只需要聚合，但是没有group by 语句，需要生成一个空的group by 语句

  auto group_by_oper =
      make_unique<GroupByLogicalOperator>(std::move(group_by_expressions), std::move(aggregate_expressions));
  logical_operator = std::move(group_by_oper);
  return RC::SUCCESS;
}