/* Copyright (c) 2021OceanBase and/or its affiliates. All rights reserved.
miniob is licensed under Mulan PSL v2.
You can use this software according to the terms and conditions of the Mulan PSL v2.
You may obtain a copy of Mulan PSL v2 at:
         http://license.coscl.org.cn/MulanPSL2
THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
See the Mulan PSL v2 for more details. */

//
// Created by Wangyunlai on 2022/5/22.
//

#include "sql/stmt/insert_stmt.h"
#include "common/log/log.h"
#include "common/type/attr_type.h"
#include "common/value.h"
#include "storage/db/db.h"
#include "storage/table/table.h"
#include "sql/expr/expression.h"

InsertStmt::InsertStmt(Table *table, std::vector<std::vector<Value>> &multi_values, int value_amount)
    : table_(table), value_amount_(value_amount)
{
  multi_values_.swap(multi_values);
}

RC InsertStmt::create(Db *db, InsertSqlNode &inserts, Stmt *&stmt)
{
  const char *table_name = inserts.relation_name.c_str();
  if (nullptr == db || nullptr == table_name || inserts.values.empty()) {
    LOG_WARN("invalid argument. db=%p, table_name=%p, value_num=%d",
        db, table_name, static_cast<int>(inserts.values.size()));
    return RC::INVALID_ARGUMENT;
  }

  // check whether the table exists
  Table *table = db->find_table(table_name);
  if (nullptr == table) {
    LOG_WARN("no such table. db=%s, table_name=%s", db->name(), table_name);
    return RC::SCHEMA_TABLE_NOT_EXIST;
  }

  // check the fields number
  std::vector<std::unique_ptr<Expression>> values_expressions;
  std::vector<std::vector<Value>>          multi_values;
  values_expressions.swap(inserts.values);

  // 目前只实现插入一条，所以只检查第一个
  if (values_expressions.front()->type() != ExprType::VALUE) {
    return RC::ERROR;
  }
  Value valuelists;
  values_expressions.front()->try_get_value(valuelists);
  if (valuelists.attr_type() == AttrType::VALUESLISTS) {
    multi_values.emplace_back();  // 新增一行
    multi_values[0].swap(*valuelists.get_valuelist());
  } else { // 插入的数据只有一列，类型不为VALUESLISTS
    multi_values.emplace_back();
    multi_values[0].push_back(valuelists);
  }

  int              value_num  = multi_values[0].size();
  const TableMeta &table_meta = table->table_meta();
  const int        field_num  = table_meta.field_num() - table_meta.sys_field_num();
  if (field_num != value_num) {
    LOG_WARN("schema mismatch. value num=%d, field num in schema=%d", value_num, field_num);
    return RC::SCHEMA_FIELD_MISSING;
  }

  // everything alright
  stmt = new InsertStmt(table, multi_values, value_num);
  return RC::SUCCESS;
}
