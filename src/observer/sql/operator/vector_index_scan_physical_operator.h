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
// Created by Wangyunlai on 2022/07/08.
//

#pragma once

#include "common/value.h"
#include "sql/expr/tuple.h"
#include "sql/operator/physical_operator.h"
#include "storage/record/record_manager.h"
#include <vector>

/**
 * @brief 向量索引扫描物理算子
 * @ingroup PhysicalOperator
 */
class VectorIndexScanPhysicalOperator : public PhysicalOperator
{
public:
  VectorIndexScanPhysicalOperator(
      Table *table, Index *index, ReadWriteMode mode, Value index_vector, std::string table_alias);

  virtual ~VectorIndexScanPhysicalOperator() = default;

  PhysicalOperatorType type() const override { return PhysicalOperatorType::VECTOR_INDEX_SCAN; }

  std::string param() const override;

  RC open(Trx *trx) override;
  RC next() override;
  RC close() override;

  Tuple *current_tuple() override;

  void set_predicates(std::vector<std::unique_ptr<Expression>> &&exprs);

  void set_papas(int limit, Value vector)
  {
    limit_  = limit;
    vector_ = vector;
  }

private:
  // 与TableScanPhysicalOperator代码相同，可以优化
  RC filter(RowTuple &tuple, bool &result);

private:
  Trx               *trx_            = nullptr;
  Table             *table_          = nullptr;
  Index             *index_          = nullptr;
  ReadWriteMode      mode_           = ReadWriteMode::READ_WRITE;
  IndexScanner      *index_scanner_  = nullptr;
  RecordFileHandler *record_handler_ = nullptr;
  RecordFileScanner  record_scanner_;
  Record             current_record_;
  RowTuple           tuple_;

  std::vector<std::unique_ptr<Expression>> predicates_;

  std::vector<RowTuple *> copied_tuples_;

  string table_alias_;

  int   limit_;
  Value vector_;  // 常量向量
};
