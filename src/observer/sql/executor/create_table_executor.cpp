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

#include "sql/executor/create_table_executor.h"

#include "common/log/log.h"
#include "common/rc.h"
#include "common/value.h"
#include "event/session_event.h"
#include "event/sql_event.h"
#include "session/session.h"
#include "sql/expr/sub_select_expression.h"
#include "sql/expr/tuple.h"
#include "sql/expr/tuple_cell.h"
#include "sql/operator/physical_operator.h"
#include "sql/operator/project_physical_operator.h"
#include "sql/parser/parse_defs.h"
#include "sql/stmt/create_table_stmt.h"
#include "storage/db/db.h"
#include "storage/field/field_meta.h"
#include "storage/record/record.h"
#include "storage/table/table.h"
#include <cstring>
#include <vector>

RC CreateTableExecutor::execute(SQLStageEvent *sql_event)
{
  Stmt    *stmt    = sql_event->stmt();
  Session *session = sql_event->session_event()->session();
  ASSERT(stmt->type() == StmtType::CREATE_TABLE,
      "create table executor can not run this command: %d",
      static_cast<int>(stmt->type()));

  CreateTableStmt *create_table_stmt = static_cast<CreateTableStmt *>(stmt);

  const char *table_name = create_table_stmt->table_name().c_str();

  RC  rc;
  Db *db = sql_event->session_event()->session()->get_current_db();

  // create table select
  if (create_table_stmt->attr_infos().empty()) {
    std::vector<AttrInfoSqlNode> attr_infos;
    SubSelectExpr               *sub_select_expr   = static_cast<SubSelectExpr *>(create_table_stmt->sub_select_expr());
    PhysicalOperator            *physical_operator = sub_select_expr->physical_operator();
    if (physical_operator->type() != PhysicalOperatorType::PROJECT) {
      std::cout << "算子必须为投影" << std::endl;
      return RC::ERROR;
    }
    ProjectPhysicalOperator *project_physical_operator = static_cast<ProjectPhysicalOperator *>(physical_operator);
    project_physical_operator->tuple_meta(attr_infos, db);
    
    rc = session->get_current_db()->create_table(table_name, attr_infos, create_table_stmt->storage_format());
  } else {
    rc = session->get_current_db()->create_table(
        table_name, create_table_stmt->attr_infos(), create_table_stmt->storage_format());
  }
  if (OB_FAIL(rc)) {
    return rc;
  }

  // 若存在子查询，则像表里插入数据
  if (create_table_stmt->sub_select_expr() != nullptr) {
    SubSelectExpr    *sub_select_expr   = static_cast<SubSelectExpr *>(create_table_stmt->sub_select_expr());
    PhysicalOperator *physical_operator = sub_select_expr->physical_operator();

    Table *new_table = db->find_table(table_name);
    physical_operator->open(nullptr);
    Tuple *tuple;
    while (OB_SUCC(rc = physical_operator->next())) {
      tuple = physical_operator->current_tuple();
      std::vector<Value> values;
      for (int i = 0; i < tuple->cell_num(); i++) {
        Value tmp;
        tuple->cell_at(i, tmp);
        values.push_back(tmp);
      }
      Record new_record;

      if (OB_FAIL(new_table->make_record(values.size(), values.data(), new_record))) {
        db->drop_table(table_name);
        rc = physical_operator->close();
        return RC::ERROR;
      }
      if (OB_FAIL(new_table->insert_record(new_record))) {
        db->drop_table(table_name);
        rc = physical_operator->close();
        return RC::ERROR;
      }
    }
    rc = physical_operator->close();
  }

  return rc;
}