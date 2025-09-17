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
// Created by Wangyunlai on 2021/5/12.
//

#pragma once

#include <string>
#include <vector>
#include <span>

#include "common/lang/serializable.h"
#include "common/rc.h"
#include "common/types.h"
#include "storage/field/field_meta.h"
#include "storage/index/index_meta.h"

/**
 * @brief 视图元数据
 * 相比tablemeta，移除了 trx_fields ，因为不知道这个用处是什么，而且View应该不考虑事务？后面需要再补回来
 *
 */
class ViewMeta : public common::Serializable
{
public:
  ViewMeta()          = default;
  virtual ~ViewMeta() = default;

  ViewMeta(const ViewMeta &other);

  void swap(ViewMeta &other) noexcept;

  RC init(int32_t view_id, const char *name, std::span<const AttrInfoSqlNode> attributes, string origin_sql);

public:
  int32_t          view_id() const { return view_id_; }
  const char      *name() const;
  const FieldMeta *field(int index) const;
  const FieldMeta *field(const char *name) const;
  const FieldMeta *find_field_by_offset(int offset) const;
  int              find_field_index_by_name(const char *name) const;
  auto             field_metas() -> std::vector<FieldMeta>             &{ return fields_; }
  int              field_num() const;  // sys field included
  int              record_size() const;

public:
  // todo:这些都没实现
  int  serialize(std::ostream &os) const override { return 0; }
  int  deserialize(std::istream &is) override { return 0; }
  int  get_serial_size() const override { return 0; }
  void to_string(std::string &output) const override { return; }
  void desc(std::ostream &os) const { return; }

protected:
  int32_t                view_id_ = -1;
  std::string            name_;
  std::vector<FieldMeta> fields_;
  int                    record_size_ = 0;
  string                 origin_sql_;
};
