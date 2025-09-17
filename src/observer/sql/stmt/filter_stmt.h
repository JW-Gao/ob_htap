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

#pragma once

#include "sql/expr/expression.h"
#include "sql/parser/parse_defs.h"
#include "sql/stmt/stmt.h"
#include <memory>
#include <unordered_map>
#include <vector>

class Db;
class Table;
class FieldMeta;

struct FilterObj
{

  std::unique_ptr<Expression> expr;

  void init(std::unique_ptr<Expression> &&expression) { expr = std::move(expression); }

  std::unique_ptr<Expression> &&expression() { return std::move(expr); }
};

class FilterUnit
{
public:
  FilterUnit() = default;
  ~FilterUnit() {}

  void set_comp(CompOp comp) { comp_ = comp; }

  CompOp comp() const { return comp_; }

  void       set_left(FilterObj &&obj) { left_ = std::move(obj); }
  void       set_right(FilterObj &&obj) { right_ = std::move(obj); }
  void       set_conjunction_type(int type) { conjunction_type_ = type; }
  FilterObj &left() { return left_; }
  FilterObj &right() { return right_; }
  int        conjunction_type() { return conjunction_type_; }

private:
  CompOp    comp_ = NO_OP;
  FilterObj left_;
  FilterObj right_;
  int       conjunction_type_;
};

/**
 * @brief Filter/谓词/过滤语句
 * @ingroup Statement
 */
class FilterStmt
{
public:
  FilterStmt() = default;
  virtual ~FilterStmt();

public:
  const std::vector<FilterUnit *> &filter_units() const { return filter_units_; }
  void add_filter_unit(FilterUnit *filter_unit) {
    filter_units_.emplace_back(filter_unit);
  }

  RC remove_filter_unit(size_t index) {
    if (index >= filter_units_.size()) {
      return RC::ERROR;
    }
    filter_units_.erase(filter_units_.begin() + index);
    return RC::SUCCESS;
  }
public:
  // 目前 这个father_tables 只有select支持，其他的不确定有没有影响
  static RC create(Db *db, Table *default_table, std::unordered_map<std::string, Table *> *tables,
      ConditionSqlNode *conditions, int condition_num, FilterStmt *&stmt, std::vector<pair<string,Table *>> father_tables = {});

  static RC create_filter_unit(Db *db, Table *default_table, std::unordered_map<std::string, Table *> *tables,
      ConditionSqlNode &condition, FilterUnit *&filter_unit, std::vector<pair<string,Table *>> father_tables = {});

private:
  std::vector<FilterUnit *> filter_units_;  // 默认当前都是AND关系
};
