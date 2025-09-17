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

#include "sql/operator/limit_physical_operator.h"
#include "common/log/log.h"
#include "sql/expr/tuple.h"

LimitPhysicalOperator::LimitPhysicalOperator(int limit) : limit_(limit) { count_ = 0; }

RC LimitPhysicalOperator::open(Trx *trx) { return children_[0]->open(trx); }

RC LimitPhysicalOperator::next()
{
  if (count_ < limit_) {
    count_++;
    return children_[0]->next();
  }
  return RC::RECORD_EOF;
}

RC LimitPhysicalOperator::close()
{
  return children_[0]->close();
  return RC::SUCCESS;
}

Tuple *LimitPhysicalOperator::current_tuple() { return children_[0]->current_tuple(); }
