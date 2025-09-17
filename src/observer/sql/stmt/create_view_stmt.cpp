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
// Created by Wangyunlai on 2023/6/13.
//

#include "common/log/log.h"
#include "common/rc.h"
#include "common/types.h"
#include "sql/expr/expression.h"
#include "sql/parser/expression_binder.h"
#include "sql/stmt/create_view_stmt.h"
#include "event/sql_debug.h"

RC CreateViewStmt::create(Db *db, CreateViewSqlNode &create_View, Stmt *&stmt)
{
  vector<unique_ptr<Expression>> bound_expressions;

  BinderContext binder_context;
  binder_context.set_db(db);
  ExpressionBinder expression_binder(binder_context);
  RC               rc = expression_binder.bind_expression(create_View.sub_select, bound_expressions);
  if (OB_FAIL(rc) || bound_expressions.front()->type() != ExprType::SUBSELECT) {
    return RC::ERROR;
  }

  stmt = new CreateViewStmt(create_View.relation_name, std::move(bound_expressions.front()));

  sql_debug("create View statement: View name %s", create_View.relation_name.c_str());
  return RC::SUCCESS;
}
