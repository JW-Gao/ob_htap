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
// Created by Wangyunlai on 2022/5/22.
//

#include "sql/stmt/update_stmt.h"
#include "common/log/log.h"
#include "common/rc.h"
#include "common/type/attr_type.h"
#include "common/value.h"
#include "sql/parser/expression_binder.h"
#include "sql/parser/parse_defs.h"
#include "sql/stmt/filter_stmt.h"
#include "storage/db/db.h"
#include "storage/field/field_meta.h"
#include "storage/text/text_manager.h"
#include <memory>
#include <utility>

UpdateStmt::UpdateStmt(
    Table *table, std::vector<pair<const FieldMeta *, Value>> values, int value_amount, FilterStmt *filter_stmt)
    : table_(table), values_(std::move(values)), value_amount_(value_amount), filter_stmt_(filter_stmt)
{}

UpdateStmt::~UpdateStmt()
{
  if (nullptr != filter_stmt_) {
    delete filter_stmt_;
    filter_stmt_ = nullptr;
  }
}
//   除select外，其他写语句均不支持 alias
RC UpdateStmt::create(Db *db, UpdateSqlNode &update, Stmt *&stmt)
{
  const char *table_name = update.relation_name.c_str();
  if (nullptr == db || nullptr == table_name) {
    LOG_WARN("invalid argument. db=%p, table_name=%p", db, table_name);
    return RC::INVALID_ARGUMENT;
  }

  // check whether the table exists
  Table *table = db->find_table(table_name);
  if (nullptr == table) {
    LOG_WARN("no such table. db=%s, table_name=%s", db->name(), table_name);
    return RC::SCHEMA_TABLE_NOT_EXIST;
  }

  std::unordered_map<std::string, Table *> table_map;
  table_map.insert(std::pair<std::string, Table *>(std::string(table_name), table));

  vector<unique_ptr<Expression>> bound_expressions;
  BinderContext                  binder_context;
  binder_context.set_db(db);
  binder_context.add_table(table,table->name());  // 添加表
  ExpressionBinder expression_binder(binder_context);

  for (auto &update_node : update.update_values) {
    RC rc = expression_binder.bind_expression(update_node.expr, bound_expressions);
    if (OB_FAIL(rc)) {
      LOG_INFO("bind expression failed. rc=%s", strrc(rc));
      return rc;
    }
  }

  FilterStmt *filter_stmt = nullptr;
  RC          rc          = FilterStmt::create(db,
      table,
      &table_map,
      const_cast<ConditionSqlNode *>(update.conditions.data()),
      static_cast<int>(update.conditions.size()),
      filter_stmt);
  if (rc != RC::SUCCESS) {
    LOG_WARN("failed to create filter statement. rc=%d:%s", rc, strrc(rc));
    return rc;
  }

  std::vector<pair<const FieldMeta *, Value>> values;

  for (int i = 0; i < update.update_values.size(); i++) {
    auto updatingfieldMeta = table->table_meta().field(update.update_values[i].attribute_name.c_str());

    // 检查列的合法性
    if (updatingfieldMeta == nullptr) {
      return RC::SCHEMA_FIELD_NOT_EXIST;
    }

    Value result_value;
    bound_expressions[i]->try_get_value(result_value);
    
    // VALUESLISTS为子查询的结果
    if (result_value.attr_type() == AttrType::VALUESLISTS) {
      if (result_value.get_valuelist()->size() > 1) {
        // 这里不直接返回错误，原因是 oper 可能一条tuple 也没有，所以还可能返回success。这里将 values 清空以代表 stmt
        // 阶段出现错误 ，其他清空values的作用类似
        values.clear();
        stmt = new UpdateStmt(table, values, 1, filter_stmt);
        return rc;
      } else if (result_value.get_valuelist()->size() == 0) {
        result_value.set_null();
      } else {
        Value tmp = result_value.get_valuelist()->front();
        result_value.set_value(tmp);
      }
    }

    // 检查更新的值类型是否匹配
    if (result_value.attr_type() != updatingfieldMeta->type()) {
      // 若value为 null，检查是否可以为null
      if (result_value.is_null()) {
        if (!updatingfieldMeta->can_be_null()) {
          values.clear();
          stmt = new UpdateStmt(table, values, 1, filter_stmt);
          return RC::SUCCESS;
          // return RC::SCHEMA_FIELD_TYPE_MISMATCH;
        }
      }
      else if (updatingfieldMeta->type() == AttrType::TEXTS) {
        if (result_value.attr_type() == AttrType::CHARS) {
          // do nothing
        }
        else {
          rc = Value::cast_to(result_value, AttrType::CHARS, result_value);
          if (OB_FAIL(rc)) {
            return rc;
          }
        }
      }
      else if (updatingfieldMeta->type() == AttrType::HIGH_VECTORS) {
        if (result_value.attr_type() != AttrType::VECTORS) {
          rc = RC::ERROR;
          return rc;
        }
        else {
          if (result_value.length() != updatingfieldMeta->real_len() || result_value.length() > MAX_VECTOR_DIM * 4) {
            rc = RC::ERROR;
            return rc;
          }
        }
      }
      // cast 操作目前未实现，后续根据需要进行补充，这里的update 的逻辑已经完善了
      else if (OB_FAIL(result_value.cast_to(result_value, updatingfieldMeta->type(), result_value))) {
        values.clear();
        stmt = new UpdateStmt(table, values, 1, filter_stmt);
        return RC::SUCCESS;
      }
    }

    if(updatingfieldMeta->type() == AttrType::VECTORS && updatingfieldMeta->len() != result_value.length()){
      return RC::ERROR;
    }

    values.push_back(std::make_pair(updatingfieldMeta, result_value));
  }

  stmt = new UpdateStmt(table, values, 1, filter_stmt);
  return rc;
}
