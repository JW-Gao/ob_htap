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
// Created by Wangyunlai on 2022/6/6.
//

#include "sql/stmt/select_stmt.h"
#include "common/lang/string.h"
#include "common/log/log.h"
#include "sql/parser/parse_defs.h"
#include "sql/stmt/filter_stmt.h"
#include "storage/db/db.h"
#include "storage/table/table.h"
#include "sql/parser/expression_binder.h"
#include <vector>

using namespace std;
using namespace common;

SelectStmt::~SelectStmt()
{
  if (nullptr != filter_stmt_) {
    delete filter_stmt_;
    filter_stmt_ = nullptr;
  }
}

RC SelectStmt::create(Db *db, SelectSqlNode &select_sql, Stmt *&stmt)
{
  if (nullptr == db) {
    LOG_WARN("invalid argument. db is null");
    return RC::INVALID_ARGUMENT;
  }

  BinderContext binder_context;
  binder_context.set_db(db);

  // 将join table的相关信息直接合并到原先的select 语句中
  if (!select_sql.join_info.empty()) {
    for (JoinEntry &entry : select_sql.join_info) {
      // select_sql.relations.emplace_back(entry.join_table);
      if (!entry.join_conditions.empty()) {
        for (ConditionSqlNode &join_cond : entry.join_conditions) {
          select_sql.conditions.push_back(std::move(join_cond));
        }
        // std::copy(entry.join_conditions.begin(), entry.join_conditions.end(),
        // std::back_inserter(select_sql.conditions));
      }
    }
    for (auto ite = select_sql.join_info.rbegin(); ite != select_sql.join_info.rend(); ++ite) {
      select_sql.relations.emplace_back((*ite).join_table);
    }
  }

  // collect tables in `from` statement
  std::vector<std::pair<string, Table *>> tables;
  unordered_map<string, Table *>          table_map;
  for (size_t i = 0; i < select_sql.relations.size(); i++) {
    const char *table_name = select_sql.relations[i].second.c_str();
    if (nullptr == table_name) {
      LOG_WARN("invalid argument. relation name is null. index=%d", i);
      return RC::INVALID_ARGUMENT;
    }

    Table *table = db->find_table(table_name);
    if (nullptr == table) {
      LOG_WARN("no such table. db=%s, table_name=%s", db->name(), table_name);
      return RC::SCHEMA_TABLE_NOT_EXIST;
    }

    binder_context.add_table(table, select_sql.relations[i].first);
    tables.push_back({select_sql.relations[i].first, table});
    if (table_map.count(select_sql.relations[i].first) == 0) {
      table_map.insert({select_sql.relations[i].first, table});
    } else {
      return RC::ERROR;  // 出现相同 alias
    }
  }

  std::vector<pair<string, Table *>> father_tables;
  for (auto father_relation : select_sql.father_relations) {
    Table *father_table = db->find_table(father_relation.second.c_str());
    binder_context.add_table(father_table, father_relation.first);
    father_tables.push_back({father_relation.first, father_table});
  }

  // collect query fields in `select` statement
  vector<unique_ptr<Expression>> bound_expressions;
  ExpressionBinder               expression_binder(binder_context);

  // bind condition expression, 在做filter stmt的时候会绑定，此处不需要
  // for (ConditionSqlNode &cond : select_sql.conditions) {
  //   std::vector<std::unique_ptr<Expression>> left_bound;
  //   std::vector<std::unique_ptr<Expression>> right_bound;
  //   for (std::unique_ptr<Expression> &exp : cond.left_expression) {
  //     expression_binder.bind_expression(exp, left_bound);
  //   }
  //   cond.left_expression.swap(left_bound);

  //   for (std::unique_ptr<Expression> &exp : cond.right_expression) {
  //     expression_binder.bind_expression(exp, right_bound);
  //   }
  //   cond.left_expression.swap(right_bound);
  // }

  for (unique_ptr<Expression> &expression : select_sql.expressions) {
    RC rc = expression_binder.bind_expression(expression, bound_expressions);
    if (OB_FAIL(rc)) {
      LOG_INFO("bind expression failed. rc=%s", strrc(rc));
      return rc;
    }
  }

  vector<unique_ptr<Expression>> group_by_expressions;
  for (unique_ptr<Expression> &expression : select_sql.group_by) {
    RC rc = expression_binder.bind_expression(expression, group_by_expressions);
    if (OB_FAIL(rc)) {
      LOG_INFO("bind expression failed. rc=%s", strrc(rc));
      return rc;
    }
  }

  Table *default_table = nullptr;
  if (tables.size() == 1) {
    default_table = tables[0].second;
  }

  // create filter statement in `where` statement
  FilterStmt *filter_stmt = nullptr;
  RC          rc          = FilterStmt::create(db,
      default_table,
      &table_map,
      select_sql.conditions.data(),
      static_cast<int>(select_sql.conditions.size()),
      filter_stmt,
      father_tables);
  if (rc != RC::SUCCESS) {
    LOG_WARN("cannot construct filter stmt");
    return rc;
  }

  // create group by filter statement in `where` statement
  FilterStmt *group_by_filter_stmt = nullptr;
  rc                               = FilterStmt::create(db,
      default_table,
      &table_map,
      select_sql.group_by_conditions.data(),
      static_cast<int>(select_sql.group_by_conditions.size()),
      group_by_filter_stmt,
      father_tables);
  if (rc != RC::SUCCESS) {
    LOG_WARN("cannot construct filter stmt");
    return rc;
  }

  vector<unique_ptr<Expression>> order_by_expressions;
  for (auto &expression : select_sql.order_by.order_by_attrs) {
    RC rc = expression_binder.bind_expression(expression, order_by_expressions);
    if (OB_FAIL(rc)) {
      LOG_INFO("bind expression failed. rc=%s", strrc(rc));
      return rc;
    }
  }

  // everything alright
  SelectStmt *select_stmt = new SelectStmt();

  select_stmt->tables_.swap(tables);
  select_stmt->query_expressions_.swap(bound_expressions);
  select_stmt->filter_stmt_          = filter_stmt;
  select_stmt->group_by_filter_stmt_ = group_by_filter_stmt;
  select_stmt->group_by_.swap(group_by_expressions);
  select_stmt->order_by_.order_by_attrs.swap(order_by_expressions);
  select_stmt->order_by_.order_by_types.swap(select_sql.order_by.order_by_types);
  select_stmt->limit_ = select_sql.limit;
  stmt                = select_stmt;
  return RC::SUCCESS;
}
