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
// Created by WangYunlai on 2022/12/30.
//

#include "sql/operator/father_tuple_physical_operator.h"
#include "common/log/log.h"
#include "sql/expr/tuple.h"

FatherTuplePhysicalOperator::FatherTuplePhysicalOperator() { father_tuple_ = &empty_tuple_; }

RC FatherTuplePhysicalOperator::open(Trx *trx) { return RC ::SUCCESS; }

RC FatherTuplePhysicalOperator::next()
{
  if (first_called_) {
    ASSERT(father_tuple_ != nullptr," 父tuple 未设置");
    first_called_ = false;
    return RC::SUCCESS;
  }
  return RC::RECORD_EOF;
}

RC FatherTuplePhysicalOperator::close()
{ 
  first_called_ = true;
  father_tuple_ = &empty_tuple_;
  return RC::SUCCESS;
}

Tuple *FatherTuplePhysicalOperator::current_tuple() { return father_tuple_; }
