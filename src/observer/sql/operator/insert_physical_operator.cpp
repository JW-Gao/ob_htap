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
// Created by WangYunlai on 2021/6/9.
//

#include "sql/operator/insert_physical_operator.h"
#include "common/value.h"
#include "sql/stmt/insert_stmt.h"
#include "storage/db/db.h"
#include "storage/field/field_meta.h"
#include "storage/table/table.h"
#include "storage/table/table_meta.h"
#include "storage/trx/trx.h"
#include <cstring>
#include <unordered_set>
#include <vector>

using namespace std;

InsertPhysicalOperator::InsertPhysicalOperator(Table *table, vector<Value> &&values)
    : table_(table), values_(std::move(values))
{}

inline bool meta_is_exist(const FieldMeta &meta, const std::vector<FieldMeta> &metas, int &index)
{
  for (int i = 0; i < metas.size(); i++) {
    if (strcmp(meta.name(), metas[i].name()) == 0) {
      index = i;
      return true;
    }
  }
  index = -1;
  return false;
}

RC InsertPhysicalOperator::open(Trx *trx)
{
  if (table_->is_view()) {
    const TableMeta &view_meta = table_->table_meta();  // 保存View的meta

    if (!table_->view()->wirte_able()) {  // 存在聚合等其他表达式 则不可更新
      return RC::ERROR;
    }

    unordered_set<string> &origin_tables = table_->view()->origin_tables();  // 视图跨越多张表，则不可更新
    if (origin_tables.size() > 1) {
      return RC::ERROR;
    }
    table_ = table_->db()->find_table(origin_tables.begin()->c_str());  // 将View 替换为真实物理表
    const TableMeta &table_meta = table_->table_meta();                 // 保存物理表的meta

    std::vector<Value> new_values;
    for (auto &it : *table_meta.field_metas()) {
      int index = -1;
      if (meta_is_exist(it, *view_meta.field_metas(), index)) {
        new_values.push_back(values_[index]);
      } else {
        Value tmp;
        tmp.set_null();
        new_values.push_back(tmp);
      }
    }
    values_.clear();
    values_.swap(new_values);
  }

  // zrk, to do，起点
  Record record;
  // 就是这里
  RC     rc = table_->make_record(static_cast<int>(values_.size()), values_.data(), record);
  if (rc != RC::SUCCESS) {
    LOG_WARN("failed to make record. rc=%s", strrc(rc));
    return rc;
  }

  rc = trx->insert_record(table_, record);
  if (rc != RC::SUCCESS) {
    LOG_WARN("failed to insert record by transaction. rc=%s", strrc(rc));
  }
  // table_->sync();
  return rc;
}

RC InsertPhysicalOperator::next() { return RC::RECORD_EOF; }

RC InsertPhysicalOperator::close() { return RC::SUCCESS; }
