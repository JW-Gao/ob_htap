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
// Created by Longda on 2021/4/13.
//

#pragma once

#include "common/rc.h"
#include "session/session.h"
#include "sql/operator/limit_logical_operator.h"
#include "sql/operator/logical_operator.h"
#include "sql/operator/physical_operator.h"
#include "sql/operator/sort_logical_operator.h"
#include "sql/operator/table_get_logical_operator.h"
#include "sql/optimizer/logical_plan_generator.h"
#include "sql/optimizer/physical_plan_generator.h"

class SQLStageEvent;
class LogicalOperator;
class Stmt;
class LimitLogicalOperator;
class SortLogicalOperator;
class TableGetLogicalOperator;

class OptimizeRule
{
public:
  RC ann_optimize(std::unique_ptr<LogicalOperator> &oper)
  {
    for (auto &child_oper : oper->children()) {
      ann_optimize(child_oper);
    }

    // 这里优化近邻查询，简单处理 table_scan->order_by->limit的情况。可以补充
    if (oper->type() == LogicalOperatorType::LIMIT && oper->children()[0]->type() == LogicalOperatorType::ORDER_BY &&
        oper->children()[0]->children()[0]->type() == LogicalOperatorType::TABLE_GET) {
      LimitLogicalOperator    *limit         = static_cast<LimitLogicalOperator *>(oper.get());
      SortLogicalOperator     *sort          = static_cast<SortLogicalOperator *>(oper->children()[0].get());
      TableGetLogicalOperator *table_get     = static_cast<TableGetLogicalOperator *>(sort->children()[0].get());
      FunctionExpr            *function_expr = static_cast<FunctionExpr *>(sort->expressions().front().get());
      if (function_expr->child().size() != 2) {  // 这里取距离表达式的两个参数
        return RC::ERROR;
      }
      for (auto &it : function_expr->child()) {
        switch (it->type()) {
          case ExprType::FIELD: {
            FieldExpr *expr = static_cast<FieldExpr *>(it.get());
            table_get->set_index_field(*expr->field().meta());
          } break;
          case ExprType::VALUE: {
            ValueExpr *expr = static_cast<ValueExpr *>(it.get());
            Value      tmp;
            expr->try_get_value(tmp);
            table_get->set_index_vector(tmp);
          } break;
          default: {
          }
        }
      }
      if (table_get->table()->find_index_by_field(table_get->index_field_meta().name()) != nullptr) {
        table_get->set_limit(limit->limit_);
        table_get->set_need_vector_index_scan(true);
        if (sort->expressions().size() > 1) {
          return RC::ERROR;
        }
      } else {
        return RC::SUCCESS;
      }

      oper = std::move(sort->children()[0]);
      // sort->children().erase(sort->children().begin());
      // std::cout<<size<<std::endl;
    }

    return RC::SUCCESS;
  }
};
