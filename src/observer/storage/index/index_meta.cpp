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
// Created by Wangyunlai.wyl on 2021/5/18.
//

#include "storage/index/index_meta.h"
#include "common/lang/string.h"
#include "common/log/log.h"
#include "storage/field/field_meta.h"
#include "storage/table/table_meta.h"
#include "json/json.h"

const static Json::StaticString FIELD_NAME("name");
const static Json::StaticString FIELD_FIELD_NAME("field_name");
const static Json::StaticString FIELD_FIELD_NUM("field_num");
const static Json::StaticString FIELD_UNIQUE("unique");

RC IndexMeta::init(const char *name, const FieldMeta *fields[], int field_num, int is_unique)
{
  if (common::is_blank(name)) {
    LOG_ERROR("Failed to init index, name is empty.");
    return RC::INVALID_ARGUMENT;
  }

  name_      = name;
  field_num_ = field_num;
  unique_    = is_unique;
  for (int i = 0; i < field_num; i++) {
    fields_.push_back(fields[i]->name());
  }
  for (int i = 0; i < field_num; i++) {
    LOG_INFO("Field %d: %s", i, fields_[i].c_str());
  }
  return RC::SUCCESS;
}

RC IndexMeta::init(
    const char *name, const FieldMeta *fields[], int field_num, int is_unique, string dis_type, int lists, int probes)
{
  if (common::is_blank(name)) {
    LOG_ERROR("Failed to init index, name is empty.");
    return RC::INVALID_ARGUMENT;
  }

  name_      = name;
  field_num_ = field_num;
  unique_    = is_unique;
  for (int i = 0; i < field_num; i++) {
    fields_.push_back(fields[i]->name());
  }
  for (int i = 0; i < field_num; i++) {
    LOG_INFO("Field %d: %s", i, fields_[i].c_str());
  }

  lists_ =lists;
  probes_ =probes;
  dis_type_ =dis_type;
  return RC::SUCCESS;
}

void IndexMeta::to_json(Json::Value &json_value) const
{
  json_value[FIELD_NAME]      = name_;
  json_value[FIELD_FIELD_NUM] = field_num_;
  json_value[FIELD_UNIQUE]    = unique_;
  Json::Value fields;
  for (int i = 0; i < field_num_; ++i) {
    Json::Value field;
    field[std::to_string(i)] = fields_[i];
    fields.append(field);
  }
  json_value[FIELD_FIELD_NAME] = std::move(fields);
}

RC IndexMeta::from_json(const TableMeta &table, const Json::Value &json_value, IndexMeta &index)
{
  const Json::Value &name_value         = json_value[FIELD_NAME];
  const Json::Value &field_num_value    = json_value[FIELD_FIELD_NUM];
  const Json::Value &field_unique_value = json_value[FIELD_UNIQUE];
  if (!name_value.isString()) {
    LOG_ERROR("Index name is not a string. json value=%s", name_value.toStyledString().c_str());
    return RC::GENERIC_ERROR;
  }
  int                field_num    = field_num_value.asInt64();
  int                unique       = field_unique_value.asInt64();
  const Json::Value &fields_value = json_value[FIELD_FIELD_NAME];
  const FieldMeta   *fields[field_num];
  for (int i = 0; i < field_num; i++) {
    const Json::Value &field_value = fields_value[i][std::to_string(i)];
    if (!field_value.isString()) {
      LOG_ERROR("Field name of index [%s] is not a string. json value=%s",
        name_value.asCString(),
        field_value.toStyledString().c_str());
      return RC::GENERIC_ERROR;
    }
    const FieldMeta *field = table.field(field_value.asCString());
    if (nullptr == field) {
      LOG_ERROR("Deserialize index [%s]: no such field: %s", name_value.asCString(), field_value.asCString());
      return RC::SCHEMA_FIELD_MISSING;
    }
    fields[i] = field;
  }

  return index.init(name_value.asCString(), fields, field_num, unique);
}

const char *IndexMeta::name() const { return name_.c_str(); }

const char *IndexMeta::field(int id) const { return fields_[id].c_str(); }

void IndexMeta::desc(ostream &os) const { os << "index name=" << name_ << ", field=" << fields_[0]; }