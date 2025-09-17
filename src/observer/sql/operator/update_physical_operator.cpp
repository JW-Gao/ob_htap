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
// Created by WangYunlai on 2022/6/27.
//

#include "sql/operator/update_physical_operator.h"
#include "common/log/log.h"
#include "common/rc.h"
#include "common/type/attr_type.h"
#include "sql/expr/expression.h"
#include "sql/expr/tuple.h"
#include "sql/expr/tuple_tools.h"
#include "sql/operator/physical_operator.h"
#include "storage/record/record.h"
#include "storage/table/table.h"
#include "storage/text/text_manager.h"
#include "storage/view/view.h"
#include "storage/trx/trx.h"
#include <cstring>
#include <vector>

RC UpdatePhysicalOperator::open(Trx *trx)
{
  if (children_.empty()) {
    return RC::SUCCESS;
  }

  // 处理View
  if (table_->is_view()) {
    std::vector<Expression *> origin_exprs;
    const SubSelectExpr      *sub_select = table_->view()->sub_select();
    if (OB_FAIL(sub_select->get_field_exprs(origin_exprs))) {
      return RC::ERROR;
    }

    if (!table_->view()->wirte_able()) {
      return RC::ERROR;
    }

    // 替换更新视图的属性列为物理表的属性列,默认即使是View，一次也最多更新一个表，若同时更新多表的列则报错
    for (auto &it : values_) {
      auto expr = origin_exprs[it.first->field_id()];
      it.first  = static_cast<FieldExpr *>(expr)->field().meta();
      table_    = const_cast<Table *>(static_cast<FieldExpr *>(expr)->field().table());
    }
  }

  std::unique_ptr<PhysicalOperator> &child = children_[0];

  RC rc = child->open(trx);
  if (rc != RC::SUCCESS) {
    LOG_WARN("failed to open child operator: %s", strrc(rc));
    return rc;
  }

  trx_ = trx;

  while (OB_SUCC(rc = child->next())) {

    // values_ 为空说明，stmt 阶段出现错误，比如要更新的值类型不匹配等
    if (values_.empty()) {
      return RC::ERROR;
    }

    Tuple *tuple = child->current_tuple();
    if (nullptr == tuple) {
      LOG_WARN("failed to get current record: %s", strrc(rc));
      return rc;
    }
    vector<RowTuple *> tuples;
    RowTuple          *row_tuple = nullptr;
    if (tuple->tuple_type() != TupleType::ROW) {  // 这里处理更新 View 的情况,
      get_row_tuples<std::unique_ptr<Expression>>(tuple, tuples);

      // 找的物理表的tuple
      for (auto &it : tuples) {
        if (strcmp(table_->name(), it->table()->name()) == 0) {
          row_tuple = it;
          break;
        }
      }
    } else {  // 这里为普通的物理表更新
      row_tuple = static_cast<RowTuple *>(tuple);
    }

    Record &record = row_tuple->record();
    // Record   new_record(record);
    char *new_record_data = (char *)malloc(record.len() * sizeof(char));
    memcpy(new_record_data, record.data(), record.len());

    for (auto &it : values_) {
      if (it.first->type() == AttrType::TEXTS) {
        int copy_len = it.first->len();
        bool value_is_null = it.second.is_null();
        if (value_is_null) {
          int tmp_invalid_page = -1;
          memcpy(new_record_data + it.first->offset(), &(tmp_invalid_page), copy_len);
        }
        else {
          if (it.second.attr_type() != AttrType::CHARS) {
            rc = RC::ERROR;
            break;
          }
          PageNum origin_page_num = *(int*)(record.data() + it.first->offset());
          PageNum new_page_num;
          string n_str = it.second.get_string();
          if (n_str.length() > MAX_TEXT_LENGTH) {
            rc = RC::ERROR;
            break;
          }
          table_->text_file_handler()->update_text(origin_page_num, n_str.c_str(), n_str.length(), new_page_num);
          memcpy(new_record_data + it.first->offset(), &new_page_num, copy_len);
        }
        memcpy(new_record_data + record.len() - it.first->field_id() - 1, &value_is_null, 1);  // 修改null标记位
      }
      else if (it.first->type() == AttrType::HIGH_VECTORS) {
        int copy_len = it.first->len();
        bool value_is_null = it.second.is_null();
        if (value_is_null) {
          int tmp_invalid_page = -1;
          memcpy(new_record_data + it.first->offset(), &(tmp_invalid_page), copy_len);
        }
        else {
          if (it.second.attr_type() != AttrType::VECTORS) {
            rc = RC::ERROR;
            break;
          }
          PageNum origin_page_num = *(int*)(record.data() + it.first->offset());
          PageNum new_page_num;
          const char *data = it.second.data();
          int total_len = it.second.length();
          if (total_len > MAX_VECTOR_DIM * 4) {
            rc = RC::ERROR;
            break;
          }
          if (total_len != it.first->real_len()) {
            rc = RC::ERROR;
            break;
          }
          rc = table_->vector_handler()->update_text(origin_page_num, data, total_len, new_page_num);
          if (OB_FAIL(rc)) {
            break;
          }
          memcpy(new_record_data + it.first->offset(), &new_page_num, copy_len);
        } 
      }
      else {
        int copy_len = it.second.length();
        if (it.second.attr_type() == AttrType::CHARS && copy_len < it.first->len()) {
          ++copy_len;
        }
        bool value_is_null = it.second.is_null();
        memcpy(new_record_data + it.first->offset(), it.second.data(), copy_len);
        memcpy(new_record_data + record.len() - it.first->field_id() - 1, &value_is_null, 1);  // 修改null标记位
      }
    }

    if (OB_FAIL(rc)) {
      return rc;
    }

    // TODO: 需要做unique处理, 否则影响唯一性约束
    //
    rc = trx_->update_record(table_, record, new_record_data, record.len());

    if (rc != RC::SUCCESS) {
      LOG_WARN("failed to Update record: %s", strrc(rc));
      return rc;
    }
  }

  // if (!table_->is_view()) {
  //   table_->data_buffer_pool()->flush_all_pages();
  // }

  return RC::SUCCESS;
}

RC UpdatePhysicalOperator::next() { return RC::RECORD_EOF; }

RC UpdatePhysicalOperator::close()
{
  children_[0]->close();
  return RC::SUCCESS;
}
