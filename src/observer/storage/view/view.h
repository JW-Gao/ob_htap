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
// Created by Meiyi & Wangyunlai on 2021/5/12.
//

#pragma once

#include "common/lang/unordered_set.h"
#include "common/types.h"
#include "sql/operator/physical_operator.h"
#include "storage/table/table.h"
#include "storage/view/view_meta.h"
#include "storage/table/table_meta.h"
#include "sql/expr/expression.h"
#include "sql/expr/sub_select_expression.h"
#include <memory>

class Db;

/**
 * @brief 表(虚拟表)
 * 实现思路：
 * execution 的主要框架不变，View 被封装为 table 的一个私有成员。
 * 在DB 中，既维护一个View_map,也维护一个view同名的 虚拟table，加入到table_map中，这个View
 * 就是虚拟 table 的一个成员，在调用table的接口函数时，若table 是View，接口函数再去调用View对应的功能。
 * 即对execution来说，物理表和虚拟表（View）都被抽象为table，过程中，只管调用table
 * 的接口，真正需要调用View的功能时，则由table的函数再去调用。 也就是说，View在 parse、stmt、logical 阶段都是不可见的。
 * 只有在生成物理计划和物理计划执行的时候，才会偶尔根据视图的需要做出一些修改。
 * 注：目前View只是内存结构，没有持久化到磁盘，有时间可以补充。简单的思路是将 构造View的sql 存到磁盘，DB启动时再重新解析
 * TODO：drop，持久化
 */
class View
{
public:
  View() = default;
  ~View();

  /**
   * 创建一个视图，View存储着一个子查询，其本质是一个可直接执行的物理计划
   */
  RC create(Db *db, int32_t view_id, const char *name, std::unique_ptr<Expression> &&sub_select);
  RC drop();

public:
  int32_t     view_id() const { return view_meta_.view_id(); }
  const char *name() const;

  Db *db() const { return db_; }

  const ViewMeta &view_meta() const;

  TableMeta table_meta();

  PhysicalOperator *oper();

  const SubSelectExpr *sub_select() { return static_cast<SubSelectExpr *>(sub_select_.get()); }

  unordered_set<string> &origin_tables() { return origin_tables_; }

  bool wirte_able() { return wirte_able_; }

private:
  Db                    *db_ = nullptr;
  ViewMeta               view_meta_;
  unique_ptr<Expression> sub_select_;
  string                 origin_sql_;  // 用于重启时重新解析Subselect；
  unordered_set<string>  origin_tables_;
  bool                   wirte_able_;
};
