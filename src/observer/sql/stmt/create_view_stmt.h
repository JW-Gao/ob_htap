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
// Created by Wangyunlai on 2023/6/13.
//

#pragma once

#include <memory>
#include <string>
#include <vector>

#include "common/types.h"
#include "common/value.h"
#include "sql/expr/expression.h"
#include "sql/stmt/stmt.h"

class Db;

/**
 * @brief 表示创建表的语句
 * @ingroup Statement
 * @details 虽然解析成了stmt，但是与原始的SQL解析后的数据也差不多
 */
class CreateViewStmt : public Stmt
{
public:
  CreateViewStmt(const std::string &view_name, std::unique_ptr<Expression> &&sub_select)
      : view_name_(view_name), sub_select_(std::move(sub_select))
  {}
  virtual ~CreateViewStmt() = default;

  StmtType type() const override { return StmtType::CREATE_VIEW; }

  const std::string                  &view_name() const { return view_name_; }

  static RC            create(Db *db, CreateViewSqlNode &create_view, Stmt *&stmt);
  // static StorageFormat get_storage_format(const char *format_str);

  std::unique_ptr<Expression> &&sub_select_expr() { return std::move(sub_select_); }

private:
  std::string                  view_name_;
  std::unique_ptr<Expression>  sub_select_;
};