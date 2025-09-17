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
// Created by Meiyi & Wangyunlai on 2021/5/12.
//

#include "common/lang/span.h"
#include "common/lang/string.h"
#include "common/lang/algorithm.h"
#include "common/log/log.h"
#include "common/type/attr_type.h"
#include "storage/view/view_meta.h"
#include "json/json.h"

static const Json::StaticString FIELD_VIEW_ID("view_id");
static const Json::StaticString FIELD_VIEW_NAME("view_name");
static const Json::StaticString ORIGIN_SQL("origin_sql");

ViewMeta::ViewMeta(const ViewMeta &other)
    : view_id_(other.view_id_),
      name_(other.name_),
      fields_(other.fields_),
      record_size_(other.record_size_),
      origin_sql_(other.origin_sql_)
{}

void ViewMeta::swap(ViewMeta &other) noexcept
{
  name_.swap(other.name_);
  fields_.swap(other.fields_);
  std::swap(record_size_, other.record_size_);
  std::swap(origin_sql_, other.origin_sql_);
}

RC ViewMeta::init(int32_t view_id, const char *name, span<const AttrInfoSqlNode> attributes, string origin_sql)
{
  if (common::is_blank(name)) {
    LOG_ERROR("Name cannot be empty");
    return RC::INVALID_ARGUMENT;
  }

  if (attributes.size() == 0) {
    LOG_ERROR("Invalid argument. name=%s, field_num=%d", name, attributes.size());
    return RC::INVALID_ARGUMENT;
  }

  RC rc = RC::SUCCESS;

  int field_offset = 0;

  fields_.resize(attributes.size());

  for (size_t i = 0; i < attributes.size(); i++) {
    const AttrInfoSqlNode &attr_info = attributes[i];
    // `i` is the col_id of fields[i]
    rc = fields_[i].init(attr_info.name.c_str(),
        attr_info.type,
        field_offset,
        attr_info.length,
        true /*visible*/,
        i,
        attr_info.can_be_null);
    if (OB_FAIL(rc)) {
      LOG_ERROR("Failed to init field meta. view name=%s, field name: %s", name, attr_info.name.c_str());
      return rc;
    }

    field_offset += attr_info.length;
  }

  // record 记录增加，用于记录每个列是否为null，0代表非null
  record_size_ = field_offset + attributes.size();
  view_id_     = view_id;
  name_        = name;
  origin_sql_  = origin_sql;
  LOG_INFO("Sussessfully initialized view meta. view id=%d, name=%s", view_id, name);
  return RC::SUCCESS;
}

const char *ViewMeta::name() const { return name_.c_str(); }

const FieldMeta *ViewMeta::field(int index) const { return &fields_[index]; }
const FieldMeta *ViewMeta::field(const char *name) const
{
  if (nullptr == name) {
    return nullptr;
  }
  for (const FieldMeta &field : fields_) {
    if (0 == strcmp(field.name(), name)) {
      return &field;
    }
  }
  return nullptr;
}

int ViewMeta::find_field_index_by_name(const char *name) const
{
  if (nullptr == name) {
    return -1;
  }
  int size = fields_.size();
  for (int i = 0; i < size; ++i) {
    if (0 == strcmp(fields_[i].name(), name)) {
      return i;
    }
  }
  return -1;
}

const FieldMeta *ViewMeta::find_field_by_offset(int offset) const
{
  for (const FieldMeta &field : fields_) {
    if (field.offset() == offset) {
      return &field;
    }
  }
  return nullptr;
}
int ViewMeta::field_num() const { return fields_.size(); }

int ViewMeta::record_size() const { return record_size_; }
