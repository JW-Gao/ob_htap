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

#include "sql/operator/table_scan_physical_operator.h"
#include "event/sql_debug.h"
#include "sql/expr/tuple.h"
#include "storage/record/record.h"
#include "storage/table/table.h"

using namespace std;

RC TableScanPhysicalOperator::open(Trx *trx)
{
  RC rc = table_->get_record_scanner(record_scanner_, trx, mode_);
  if (rc == RC::SUCCESS) {
    tuple_.set_schema(table_, table_->table_meta().field_metas(),table_alias_);
  }
  trx_ = trx;
  return rc;
}

RC TableScanPhysicalOperator::next()
{
  RC rc = RC::SUCCESS;

  bool filter_result = false;
  while (OB_SUCC(rc = record_scanner_.next(current_record_))) {
    LOG_TRACE("got a record. rid=%s", current_record_.rid().to_string().c_str());

    tuple_.set_record(&current_record_);
    rc = filter(tuple_, filter_result);
    if (rc != RC::SUCCESS) {
      LOG_TRACE("record filtered failed=%s", strrc(rc));
      return rc;
    }

    if (filter_result) {
      sql_debug("get a tuple: %s", tuple_.to_string().c_str());
      break;
    } else {
      sql_debug("a tuple is filtered: %s", tuple_.to_string().c_str());
    }
  }
  return rc;
}

RC TableScanPhysicalOperator::close()
{ 
  // 我已经修改了join的逻辑，每张表只会open一次，并在树顶部的算子close时，递归close，所有将内存回收放在这里。index_scan 同理
  for (auto tuple : copied_tuples_) {
    delete &tuple->record();
    delete tuple;
  }
  copied_tuples_.clear();

  return record_scanner_.close_scan();
}

Tuple *TableScanPhysicalOperator::current_tuple()
{
  // 拷贝是为了排序等操作的tuple的收集,如果不用排序应该可以不用拷贝，这里全部拷贝
  Record   *copied_record = new Record(current_record_);
  RowTuple *tuple         = new RowTuple();
  copied_tuples_.push_back(tuple);

  // owner为true，copy了一份
  copied_record->copy_data(current_record_.data(), current_record_.len());

  tuple->set_record(copied_record);
  tuple->set_schema(table_, table_->table_meta().field_metas(),table_alias_);
  return tuple;
}

string TableScanPhysicalOperator::param() const { return table_->name(); }

void TableScanPhysicalOperator::set_predicates(vector<unique_ptr<Expression>> &&exprs)
{
  predicates_ = std::move(exprs);
}

RC TableScanPhysicalOperator::filter(RowTuple &tuple, bool &result)
{
  RC    rc = RC::SUCCESS;
  Value value;
  for (unique_ptr<Expression> &expr : predicates_) {
    rc = expr->get_value(tuple, value);
    if (rc != RC::SUCCESS) {
      return rc;
    }

    bool tmp_result = value.get_boolean();
    if (!tmp_result) {
      result = false;
      return rc;
    }
  }

  result = true;
  return rc;
}
