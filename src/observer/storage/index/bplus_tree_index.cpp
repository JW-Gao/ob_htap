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
// Created by wangyunlai.wyl on 2021/5/19.
//

#include "storage/index/bplus_tree_index.h"
#include "common/log/log.h"
#include "common/type/attr_type.h"
#include "storage/table/table.h"
#include "storage/db/db.h"

BplusTreeIndex::~BplusTreeIndex() noexcept { close(); }

RC BplusTreeIndex::create(Table *table, const char *file_name, const IndexMeta &index_meta, const FieldMeta *field_metas[], int field_num)
{
  if (inited_) {
    LOG_WARN("Failed to create index due to the index has been created before. file_name:%s, index:%s, field:%s, unique:%d",
        file_name, index_meta.name(), index_meta.field(0), index_meta.unique());
    return RC::RECORD_OPENNED;
  }

  Index::init(index_meta, field_metas, field_num);

  BufferPoolManager &bpm = table->db()->buffer_pool_manager();

  AttrType attr_types[field_num];
  int attr_len[field_num];

  for(int i = 0; i < field_num; i++){
    attr_types[i] = field_metas[i]->type();
    attr_len[i] = field_metas[i]->len();
  }
  RC rc = index_handler_.create(table->db()->log_handler(), bpm, file_name, attr_types, attr_len, field_num, index_meta.unique());
  if (RC::SUCCESS != rc) {
    LOG_WARN("Failed to create index_handler, file_name:%s, index:%s, field:%s, rc:%s",
        file_name, index_meta.name(), index_meta.field(0), strrc(rc));
    return rc;
  }

  inited_ = true;
  table_  = table;
  LOG_INFO("Successfully create index, file_name:%s, index:%s, field:%s, unique:%d" ,
    file_name, index_meta.name(), index_meta.field(0), index_meta.unique());
  return RC::SUCCESS;
}

RC BplusTreeIndex::open(Table *table, const char *file_name, const IndexMeta &index_meta, const FieldMeta *field_metas[], int field_num)
{
  if (inited_) {
    LOG_WARN("Failed to open index due to the index has been initedd before. file_name:%s, index:%s, field:%s, unique:%d",
        file_name, index_meta.name(), index_meta.field(0), index_meta.unique());
    return RC::RECORD_OPENNED;
  }

  Index::init(index_meta, field_metas, field_num);

  BufferPoolManager &bpm = table->db()->buffer_pool_manager();
  RC rc = index_handler_.open(table->db()->log_handler(), bpm, file_name);
  if (RC::SUCCESS != rc) {
    LOG_WARN("Failed to open index_handler, file_name:%s, index:%s, field:%s, rc:%s, unique:%d",
        file_name, index_meta.name(), index_meta.field(0), strrc(rc), index_meta.unique());
    return rc;
  }

  inited_ = true;
  table_  = table;
  LOG_INFO("Successfully open index, file_name:%s, index:%s, field:%s, unique:%d",
    file_name, index_meta.name(), index_meta.field(0), index_meta.unique());
  return RC::SUCCESS;
}

RC BplusTreeIndex::close()
{
  if (inited_) {
    LOG_INFO("Begin to close index, index:%s, field:%s", index_meta_.name(), index_meta_.field(0));
    index_handler_.close();
    inited_ = false;
  }
  LOG_INFO("Successfully close index.");
  return RC::SUCCESS;
}

RC BplusTreeIndex::make_entry_key(const char *record, char *&entry_key) {
  int total_length = entry_length();
  entry_key = (char *)malloc(total_length);
  int offset = 0;
  for(int i = 0; i < field_num_; i++){
    memcpy(entry_key + offset, record + field_metas_[i].offset() , field_metas_[i].len());
    offset += field_metas_[i].len();
  }
  return RC::SUCCESS;
}

RC BplusTreeIndex::insert_entry(Record &record, const RID *rid, const int record_size, int field_indexes[])
{
  bool is_null = false;
  // 检查有无null
  if(field_metas_.size() == 1){
    memcpy(&is_null, record.data() + record_size - field_indexes[0] -1, 1);
  } else {
    for(int i = 0; i < field_metas_.size(); i++){
      memcpy(&is_null, record.data() + record_size - field_indexes[i] -1, 1);
      if(is_null == true){
        break;
      }
    }
  }

  char * entry_key;
  make_entry_key(record.data(), entry_key);
  return index_handler_.insert_entry(entry_key, rid, is_null);
}

RC BplusTreeIndex::delete_entry(const char *record, const RID *rid)
{
  char *entry_key;
  make_entry_key(record, entry_key);
  return index_handler_.delete_entry(entry_key, rid);
}

RC BplusTreeIndex::update_entry(const char *record, const char *new_record, const RID *rid, int record_null)
{
  char *old_entry_key;
  make_entry_key(record, old_entry_key);

  char *entry_key;
  make_entry_key(new_record, entry_key);

  if (memcmp(old_entry_key, entry_key, entry_length()) == 0) {
    return RC::SUCCESS;
  }
  return index_handler_.update_entry(old_entry_key, entry_key, rid, record_null);
}

IndexScanner *BplusTreeIndex::create_scanner(
    const char *left_key, int left_len, bool left_inclusive, const char *right_key, int right_len, bool right_inclusive)
{
  BplusTreeIndexScanner *index_scanner = new BplusTreeIndexScanner(index_handler_);
  RC rc = index_scanner->open(left_key, left_len, left_inclusive, right_key, right_len, right_inclusive);
  if (rc != RC::SUCCESS) {
    LOG_WARN("failed to open index scanner. rc=%d:%s", rc, strrc(rc));
    delete index_scanner;
    return nullptr;
  }
  return index_scanner;
}

RC BplusTreeIndex::sync() { return index_handler_.sync(); }

////////////////////////////////////////////////////////////////////////////////
BplusTreeIndexScanner::BplusTreeIndexScanner(BplusTreeHandler &tree_handler) : tree_scanner_(tree_handler) {}

BplusTreeIndexScanner::~BplusTreeIndexScanner() noexcept { tree_scanner_.close(); }

RC BplusTreeIndexScanner::open(
    const char *left_key, int left_len, bool left_inclusive, const char *right_key, int right_len, bool right_inclusive)
{
  return tree_scanner_.open(left_key, left_len, left_inclusive, right_key, right_len, right_inclusive);
}

RC BplusTreeIndexScanner::next_entry(RID *rid) { return tree_scanner_.next_entry(*rid); }

RC BplusTreeIndexScanner::destroy()
{
  delete this;
  return RC::SUCCESS;
}
