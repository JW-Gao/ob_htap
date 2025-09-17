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
// Created by WangYunlai on 2022/6/7.
//

#pragma once

#include "common/rc.h"
#include "sql/expr/tuple.h"
#include "sql/operator/physical_operator.h"
#include "storage/record/record_manager.h"
#include "common/types.h"
#include "storage/table/table.h"
#include <string>
#include <vector>
#include <storage/view/view.h>

class View;

/**
 * @brief 表扫描物理算子
 * @ingroup PhysicalOperator
 */
class ViewScanPhysicalOperator : public PhysicalOperator
{
public:
  ViewScanPhysicalOperator(Table *table, ReadWriteMode mode, std::string table_alias)
      : table_(table), mode_(mode), table_alias_(table_alias)
  {
    child_oper_ = table->view()->oper();
    if(mode_ == ReadWriteMode::READ_ONLY){

    }
  }

  virtual ~ViewScanPhysicalOperator() = default;

  std::string param() const override;

  PhysicalOperatorType type() const override { return PhysicalOperatorType::VIEW_SCAN; }

  RC open(Trx *trx) override;
  RC next() override;
  RC close() override;

  Tuple *current_tuple() override;

  void set_predicates(std::vector<std::unique_ptr<Expression>> &&exprs);

private:
  RC filter(RowTuple &tuple, bool &result);

private:
  Table                                   *table_ = nullptr;   // 这里的table 是虚拟表
  ReadWriteMode           mode_  = ReadWriteMode::READ_WRITE;
  Record                                   current_record_;
  RowTuple                                 tuple_;
  std::vector<std::unique_ptr<Expression>> predicates_;     // TODO chang predicate to view tuple filter
  std::vector<RowTuple *>                  copied_tuples_;  // for delete copied tuples
  string                                   table_alias_;
  PhysicalOperator *child_oper_;  // 这里不能用智能指针，否者会把视图中存的 算子取过来。视图就不能再次用了
  vector<Tuple *>                          tuples_;
  JoinedTuple                             current_tuple_;
};
