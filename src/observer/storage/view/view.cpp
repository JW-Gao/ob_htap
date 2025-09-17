/* Copyright (c) 2021 Xie Meiyi(xiemeiyi@hust.edu.cn) and OceanBase and/or its affiliates. All rights reserved.
miniob is licensed under Mulan PSL v2.
You can use this software according to the terms and conditions of the Mulan PSL v2.
You may obtain a copy of Mulan PSL v2 at:
         http://license.coscl.org.cn/MulanPSL2
THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
See the Mulan PSL v2 for more details. */

//
// Created by Meiyi & Wangyunlai on 2021/5/13.
//

#include <cstring>
#include <memory>
#include <string.h>
#include <vector>

#include "storage/db/db.h"
#include "storage/view/view.h"


View::~View() { LOG_INFO("view has been closed: %s", name()); }

RC View::create(Db *db, int32_t view_id, const char *name, std::unique_ptr<Expression> &&sub_select)
{
  SubSelectExpr               *sub_select_expr = static_cast<SubSelectExpr *>(sub_select.get());
  std::vector<AttrInfoSqlNode> attributes;
  sub_select_expr->field_meta(attributes, db);

  if (view_id < 0) {
    LOG_WARN("invalid view id. view_id=%d, view_name=%s", view_id, name);
    return RC::INVALID_ARGUMENT;
  }

  if (common::is_blank(name)) {
    LOG_WARN("Name cannot be empty");
    return RC::INVALID_ARGUMENT;
  }

  if (attributes.size() == 0) {
    LOG_WARN("Invalid arguments. view_name=%s, attribute_count=%d", name, attributes.size());
    return RC::INVALID_ARGUMENT;
  }

  RC rc = RC::SUCCESS;
  if ((rc = view_meta_.init(view_id, name, attributes, sub_select->name())) != RC::SUCCESS) {
    LOG_ERROR("Failed to init view meta. name:%s, ret:%d", name, rc);
    return rc;  //
  }
  db_         = db;
  sub_select_ = move(sub_select);
  wirte_able_ = true;

  std::vector<Expression *> origin_exprs;
  sub_select_expr->get_field_exprs(origin_exprs);

  for (auto &expr : origin_exprs) {  // 只允许更新视图中的属性字段,检查是否可以更新
    if (expr->type() != ExprType::FIELD) {
      wirte_able_ = false;
    } else {
      FieldExpr *field_expr = static_cast<FieldExpr *>(expr);
      origin_tables_.insert(field_expr->table_name());
    }
  }

  return rc;
}

RC View::drop() { return RC::UNSUPPORTED; }

const char *View::name() const { return view_meta_.name(); }

const ViewMeta &View::view_meta() const { return view_meta_; }

TableMeta View::table_meta()
{
  TableMeta table_meta;
  table_meta.init(&view_meta_);
  return table_meta;
}

PhysicalOperator *View::oper()
{
  SubSelectExpr *expr = static_cast<SubSelectExpr *>(sub_select_.get());
  return expr->physical_operator();
}
