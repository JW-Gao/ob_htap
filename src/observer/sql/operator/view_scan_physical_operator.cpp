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

#include "sql/operator/view_scan_physical_operator.h"
#include "common/rc.h"
#include "common/value.h"
#include "event/sql_debug.h"
#include "sql/expr/tuple.h"
#include "storage/record/record.h"
#include "storage/view/view.h"
#include <vector>

using namespace std;

RC ViewScanPhysicalOperator::open(Trx *trx)
{
  tuple_.set_schema(table_, table_->table_meta().field_metas(), table_alias_);
  return child_oper_->open(trx);
}

RC ViewScanPhysicalOperator::next()
{
  RC rc = RC::SUCCESS;

  bool   filter_result = false;
  Tuple *tuple;
  while (OB_SUCC(rc = child_oper_->next())) {
    tuple = child_oper_->current_tuple();

    vector<Value> values;
    for (int i = 0; i < tuple->cell_num(); i++) {
      Value tmp;
      tuple->cell_at(i, tmp);
      values.push_back(tmp);
    }
    table_->make_record(values.size(), values.data(), current_record_);
    tuple_.set_record(&current_record_);

    rc = filter(tuple_, filter_result);
    if (rc != RC::SUCCESS) {
      LOG_TRACE("record filtered failed=%s", strrc(rc));
      return rc;
    }

    if (filter_result) {
      Record   *copied_record = new Record(current_record_);
      RowTuple *copied_tuple  = new RowTuple();
      copied_tuples_.push_back(copied_tuple);

      // owner为true，copy了一份
      // copied_record->copy_data(current_record_.data(), current_record_.len()); //
      // 相比table_scan,这不不复制，这里本身就是新record

      copied_tuple->set_record(copied_record);
      copied_tuple->set_schema(table_, table_->table_meta().field_metas(), table_alias_);

      current_tuple_.set_left(copied_tuple);
      current_tuple_.set_right(tuple);  // 保留这个tuple 是为了在update View时，找到最original_rowtuple,再没有其他作用
      sql_debug("get a tuple: %s", tuple_.to_string().c_str());
      break;
    } else {
      sql_debug("a tuple is filtered: %s", tuple_.to_string().c_str());
    }
  }
  return rc;
}

RC ViewScanPhysicalOperator::close() { return child_oper_->close(); }

Tuple *ViewScanPhysicalOperator::current_tuple()
{
  JoinedTuple *tuple = new JoinedTuple(current_tuple_);
  return tuple;
}

string ViewScanPhysicalOperator::param() const { return table_->name(); }

void ViewScanPhysicalOperator::set_predicates(vector<unique_ptr<Expression>> &&exprs)
{
  predicates_ = std::move(exprs);
}

RC ViewScanPhysicalOperator::filter(RowTuple &tuple, bool &result)
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
