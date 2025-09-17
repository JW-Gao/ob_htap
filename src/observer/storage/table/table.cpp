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

#include <algorithm>
#include <cassert>
#include <filesystem>
#include <limits.h>
#include <memory>
#include <string.h>
#include <string>

#include "common/defs.h"
#include "common/lang/string.h"
#include "common/lang/span.h"
#include "common/lang/algorithm.h"
#include "common/log/log.h"
#include "common/global_context.h"
#include "common/rc.h"
#include "common/type/attr_type.h"
#include "sql/parser/parse_defs.h"
#include "sql/operator/physical_operator.h"
#include "storage/db/db.h"
#include "storage/buffer/disk_buffer_pool.h"
#include "storage/common/condition_filter.h"
#include "storage/common/meta_util.h"
#include "storage/index/bplus_tree_index.h"
#include "storage/index/index.h"
#include "storage/index/ivfflat_index.h"
#include "storage/record/record_manager.h"
#include "storage/table/table.h"
#include "storage/text/text_manager.h"
#include "storage/trx/trx.h"
#include "storage/view/view.h"

Table::~Table()
{
  if (record_handler_ != nullptr) {
    delete record_handler_;
    record_handler_ = nullptr;
  }

  if (data_buffer_pool_ != nullptr) {
    data_buffer_pool_->close_file();
    data_buffer_pool_ = nullptr;
  }

  for (vector<Index *>::iterator it = indexes_.begin(); it != indexes_.end(); ++it) {
    Index *index = *it;
    delete index;
  }
  indexes_.clear();

  LOG_INFO("Table has been closed: %s", name());
}

RC Table::create(Db *db, int32_t table_id, const char *path, const char *name, const char *base_dir,
    span<const AttrInfoSqlNode> attributes, StorageFormat storage_format)
{
  if (table_id < 0) {
    LOG_WARN("invalid table id. table_id=%d, table_name=%s", table_id, name);
    return RC::INVALID_ARGUMENT;
  }

  if (common::is_blank(name)) {
    LOG_WARN("Name cannot be empty");
    return RC::INVALID_ARGUMENT;
  }
  LOG_INFO("Begin to create table %s:%s", base_dir, name);

  if (attributes.size() == 0) {
    LOG_WARN("Invalid arguments. table_name=%s, attribute_count=%d", name, attributes.size());
    return RC::INVALID_ARGUMENT;
  }

  RC rc = RC::SUCCESS;

  // 使用 table_name.table记录一个表的元数据
  // 判断表文件是否已经存在
  int fd = ::open(path, O_WRONLY | O_CREAT | O_EXCL | O_CLOEXEC, 0600);
  if (fd < 0) {
    if (EEXIST == errno) {
      LOG_ERROR("Failed to create table file, it has been created. %s, EEXIST, %s", path, strerror(errno));
      return RC::SCHEMA_TABLE_EXIST;
    }
    LOG_ERROR("Create table file failed. filename=%s, errmsg=%d:%s", path, errno, strerror(errno));
    return RC::IOERR_OPEN;
  }

  close(fd);

  // 创建文件
  const vector<FieldMeta> *trx_fields = db->trx_kit().trx_fields();
  if ((rc = table_meta_.init(table_id, name, trx_fields, attributes, storage_format)) != RC::SUCCESS) {
    // LOG_ERROR("Failed to init table meta. name:%s, ret:%d", name, rc);
    std::remove(path);
    return rc;  // delete table file
  }

  fstream fs;
  fs.open(path, ios_base::out | ios_base::binary);
  if (!fs.is_open()) {
    LOG_ERROR("Failed to open file for write. file name=%s, errmsg=%s", path, strerror(errno));
    return RC::IOERR_OPEN;
  }

  // 记录元数据到文件中
  table_meta_.serialize(fs);
  fs.close();

  db_       = db;
  base_dir_ = base_dir;



  
  string             data_file = table_data_file(base_dir, name);
  // std::shared_ptr<SledDB> sled_kv_engine = std::make_shared<SledDB>(data_file);
  // sled_kv_engine_ = sled_kv_engine;
  // LOG_INFO("zrk, Successfully create table %s:%s using sled kv engine", base_dir, name);
  // sled_kv_engine_->insert("a", "b");
  return RC::SUCCESS;

  // 下面的这些都不需要了，全都交给sled_kv_engien就可以了
  BufferPoolManager &bpm       = db->buffer_pool_manager();
  rc                           = bpm.create_file(data_file.c_str());
  if (rc != RC::SUCCESS) {
    LOG_ERROR("Failed to create disk buffer pool of data file. file name=%s", data_file.c_str());
    return rc;
  }

  rc = init_record_handler(base_dir);
  if (rc != RC::SUCCESS) {
    LOG_ERROR("Failed to create table %s due to init record handler failed.", data_file.c_str());
    // don't need to remove the data_file
    return rc;
  }

  // 若含有数据类型为text，则新建一个文件，用于存储text相关数据
  bool text_flag = (std::find_if(attributes.begin(), attributes.end(), [&](const AttrInfoSqlNode &attr) {
    return attr.type == AttrType::TEXTS;
  }) != attributes.end());

  if (text_flag) {
    std::string text_file = table_text_file(base_dir, name);
    rc                    = db->buffer_pool_manager().create_file(text_file.c_str());
    if (rc != RC::SUCCESS) {
      LOG_ERROR("Failed to create text file.");
      return rc;
    }
    rc = init_text_handler(base_dir);
    if (OB_FAIL(rc)) {
      LOG_ERROR("Failed to init text file handler.");
      return rc;
    }
  }

  // high vector, 偷懒，采取的与text同样的逻辑
  bool vector_flag = (std::find_if(attributes.begin(), attributes.end(), [&](const AttrInfoSqlNode &attr) {
    return attr.type == AttrType::HIGH_VECTORS;
  }) != attributes.end());

  if (vector_flag) {
    std::string vector_file = table_vector_file(base_dir, name);
    rc                    = db->buffer_pool_manager().create_file(vector_file.c_str());
    if (rc != RC::SUCCESS) {
      LOG_ERROR("Failed to create vector file.");
      return rc;
    }
    rc = init_vector_handler(base_dir);
    if (OB_FAIL(rc)) {
      LOG_ERROR("Failed to init vector file handler.");
      return rc;
    }
  }

  // create a text handler to handle the text file
  LOG_INFO("Successfully create table %s:%s", base_dir, name);
  return rc;
}

RC Table::create(Db *db, int32_t table_id, View *view)
{
  db_         = db;
  view_       = view;
  table_meta_ = view->table_meta();
  return RC::SUCCESS;
}

RC Table::drop()
{
  if (is_view()) {
    std::cout << "view drop not supported" << std::endl;
    return RC::ERROR;
  }
  RC rc = RC::SUCCESS;
  // 删除索引文件
  for (auto &idx : indexes_) {
    std::remove(table_index_file(base_dir_.c_str(), table_meta_.name(), idx->index_meta().name()).c_str());
  }
  // 删除数据文件
  BufferPoolManager &bpm = db_->buffer_pool_manager();
  bpm.close_file(table_data_file(base_dir_.c_str(), table_meta_.name()).c_str());
  std::remove(table_data_file(base_dir_.c_str(), table_meta_.name()).c_str());

  // 删除元文件
  std::remove(table_meta_file(base_dir_.c_str(), table_meta_.name()).c_str());

  // 删除text文件
  bpm.close_file(table_text_file(base_dir_.c_str(), table_meta_.name()).c_str());
  std::remove(table_text_file(base_dir_.c_str(), table_meta_.name()).c_str());

  // 删除vector文件
  bpm.close_file(table_vector_file(base_dir_.c_str(), table_meta_.name()).c_str());
  std::remove(table_vector_file(base_dir_.c_str(), table_meta_.name()).c_str());
  LOG_INFO("Successfully drop table %s", base_dir_.c_str());
  return rc;
}

RC Table::open(Db *db, const char *meta_file, const char *base_dir)
{
  if (is_view()) {
    std::cout << "can not be called by view" << std::endl;
    return RC::ERROR;
  }
  // 加载元数据文件
  fstream fs;
  string  meta_file_path = string(base_dir) + common::FILE_PATH_SPLIT_STR + meta_file;
  fs.open(meta_file_path, ios_base::in | ios_base::binary);
  if (!fs.is_open()) {
    LOG_ERROR("Failed to open meta file for read. file name=%s, errmsg=%s", meta_file_path.c_str(), strerror(errno));
    return RC::IOERR_OPEN;
  }
  if (table_meta_.deserialize(fs) < 0) {
    LOG_ERROR("Failed to deserialize table meta. file name=%s", meta_file_path.c_str());
    fs.close();
    return RC::INTERNAL;
  }
  fs.close();

  db_       = db;
  base_dir_ = base_dir;

  // 加载数据文件
  RC rc = init_record_handler(base_dir);
  if (rc != RC::SUCCESS) {
    LOG_ERROR("Failed to open table %s due to init record handler failed.", base_dir);
    // don't need to remove the data_file
    return rc;
  }

  rc = init_text_handler(base_dir);
  if (rc != RC::SUCCESS) {
    LOG_ERROR("Failed to open table %s due to init text handler.", base_dir);
    return rc;
  }

  rc = init_vector_handler(base_dir);
  if (rc != RC::SUCCESS) {
    LOG_ERROR("Failed to open table %s due to init vector handler.", base_dir);
    return rc;
  }

  const int index_num = table_meta_.index_num();
  for (int i = 0; i < index_num; i++) {
    const IndexMeta *index_meta = table_meta_.index(i);
    const FieldMeta *field_metas[index_meta->field_num()];
    for (int i = 0; i < index_meta->field_num(); ++i) {
      field_metas[i] = table_meta_.field(index_meta->field(i));
      if (field_metas[i] == nullptr) {
        LOG_ERROR("Found invalid index meta info which has a non-exists field. table=%s, index=%s, field=%s",
            name(),
            index_meta->name(),
            index_meta->field(i));
        // skip cleanup
        //  do all cleanup action in destructive Table function
        return RC::GENERIC_ERROR;
      }
    }

    BplusTreeIndex *index      = new BplusTreeIndex();
    string          index_file = table_index_file(base_dir, name(), index_meta->name());

    rc = index->open(this, index_file.c_str(), *index_meta, field_metas, index_meta->field_num());
    if (rc != RC::SUCCESS) {
      delete index;
      LOG_ERROR("Failed to open index. table=%s, index=%s, file=%s, rc=%s",
                name(), index_meta->name(), index_file.c_str(), strrc(rc));
      // skip cleanup
      //  do all cleanup action in destructive Table function.
      return rc;
    }
    indexes_.push_back(index);
  }

  return rc;
}

// zrk, to do
RC Table::insert_record(Record &record)
{
  if (is_view()) {
    std::cout << "can not be called by view" << std::endl;
    return RC::ERROR;
  }

  std::string rec_key = record_encode_key(table_meta_.table_id(), record);
  std::string rec_value = std::string(record.data(), record.len());

  db_->get_storage_engine()->insert(rec_key, rec_value);
  
  return RC::SUCCESS;

  // 这些东西都不需要了，我感觉
  RC rc = RC::SUCCESS;
  rc    = record_handler_->insert_record(record.data(), table_meta_.record_size(), &record.rid());
  if (rc != RC::SUCCESS) {
    LOG_ERROR("Insert record failed. table name=%s, rc=%s", table_meta_.name(), strrc(rc));
    return rc;
  }

  rc = insert_entry_of_indexes(record, record.rid());
  if (rc != RC::SUCCESS) {  // 可能出现了键值重复
    RC rc2 = delete_entry_of_indexes(record.data(), record.rid(), false /*error_on_not_exists*/);
    // if (rc2 != RC::SUCCESS) {
    //   LOG_ERROR("Failed to rollback index data when insert index entries failed. table name=%s, rc=%d:%s",
    //             name(), rc2, strrc(rc2));
    // }
    rc2 = record_handler_->delete_record(&record.rid());
    if (rc2 != RC::SUCCESS) {
      LOG_PANIC("Failed to rollback record data when insert index entries failed. table name=%s, rc=%d:%s",
                name(), rc2, strrc(rc2));
    }
  }
  return rc;
}

RC Table::visit_record(const RID &rid, function<bool(Record &)> visitor)
{
  if (is_view()) {
    std::cout << "can not be called by view" << std::endl;
    return RC::ERROR;
  }
  return record_handler_->visit_record(rid, visitor);
}

RC Table::get_record(const RID &rid, Record &record)
{
  if (is_view()) {
    std::cout << "can not be called by view" << std::endl;
    return RC::ERROR;
  }
  RC rc = record_handler_->get_record(rid, record);
  if (rc != RC::SUCCESS) {
    LOG_WARN("failed to visit record. rid=%s, table=%s, rc=%s", rid.to_string().c_str(), name(), strrc(rc));
    return rc;
  }

  return rc;
}

RC Table::recover_insert_record(Record &record)
{
  if (is_view()) {
    std::cout << "can not be called by view" << std::endl;
    return RC::ERROR;
  }
  RC rc = RC::SUCCESS;
  rc    = record_handler_->recover_insert_record(record.data(), table_meta_.record_size(), record.rid());
  if (rc != RC::SUCCESS) {
    LOG_ERROR("Insert record failed. table name=%s, rc=%s", table_meta_.name(), strrc(rc));
    return rc;
  }

  rc = insert_entry_of_indexes(record, record.rid());
  if (rc != RC::SUCCESS) {  // 可能出现了键值重复
    RC rc2 = delete_entry_of_indexes(record.data(), record.rid(), false /*error_on_not_exists*/);
    if (rc2 != RC::SUCCESS) {
      LOG_ERROR("Failed to rollback index data when insert index entries failed. table name=%s, rc=%d:%s",
                name(), rc2, strrc(rc2));
    }
    rc2 = record_handler_->delete_record(&record.rid());
    if (rc2 != RC::SUCCESS) {
      LOG_PANIC("Failed to rollback record data when insert index entries failed. table name=%s, rc=%d:%s",
                name(), rc2, strrc(rc2));
    }
  }
  return rc;
}

const char *Table::name() const { return table_meta_.name(); }

const TableMeta &Table::table_meta() const { return table_meta_; }

RC Table::make_record(int value_num, const Value *values, Record &record)
{
  RC rc = RC::SUCCESS;
  // 检查字段类型是否一致
  if (value_num + table_meta_.sys_field_num() != table_meta_.field_num()) {
    LOG_WARN("Input values don't match the table's schema, table name:%s", table_meta_.name());
    return RC::SCHEMA_FIELD_MISSING;
  }

  const int normal_field_start_index = table_meta_.sys_field_num();
  // 复制所有字段的值
  int   record_size = table_meta_.record_size();
  char *record_data = (char *)malloc(record_size);
  memset(record_data, 0, record_size);

  for (int i = 0; i < value_num && OB_SUCC(rc); i++) {
    const FieldMeta *field = table_meta_.field(i + normal_field_start_index);
    const Value     &value = values[i];
    if (field->type() != value.attr_type()) {
      if (value.is_null() && field->can_be_null()) {
        rc = set_value_to_record(record_data, value, field, record_size);
      } else if (field->type() == AttrType::TEXTS) {
        if (value.attr_type() != AttrType::CHARS) {
          rc = RC::ERROR;
          break;
        }
        std::string text_str = value.get_string();
        if (text_str.length() > MAX_TEXT_LENGTH) {
          rc = RC::ERROR;
          break;
        }
        PageNum page_num;
        rc = text_file_handler_->write_text(text_str.c_str(), text_str.length(), page_num);
        if (OB_FAIL(rc)) {
          LOG_ERROR("Failed to write text data to frame/file.");
          break;
        }
        Value real_value(page_num);
        rc = set_value_to_record(record_data, real_value, field, record_size);
      } else if (field->type() == AttrType::HIGH_VECTORS) {
        if (value.attr_type() != AttrType::VECTORS) {
          rc = RC::ERROR;
          break;
        }
        const char *data = value.data();
        const int total_len = value.length();
        if (total_len > MAX_VECTOR_DIM * 4) {
          rc = RC::ERROR;
          break;
        }
        if (total_len != field->real_len()) {
          rc = RC::ERROR;
          break;
        }
        ASSERT(vector_handler_ != nullptr, "there is no vector file.");
        
        PageNum vector_page_num = -1;
        rc = vector_handler_->write_text(data, total_len, vector_page_num);
        if (OB_FAIL(rc)) {
          LOG_ERROR("Fail to write vector.");
          break;
        }
        rc = set_value_to_record(record_data, Value(vector_page_num), field, record_size);
      } else {
        // 只会分支到这里
        Value real_value;
        rc = Value::cast_to(value, field->type(), real_value);
        if (OB_FAIL(rc)) {
          LOG_WARN("failed to cast value. table name:%s,field name:%s,value:%s ",
            table_meta_.name(), field->name(), value.to_string().c_str());
          break;
        }
        rc = set_value_to_record(record_data, real_value, field, record_size);
      }

    } else if (field->type() == AttrType::VECTORS) {
      if (field->len() != value.length()) {
        return RC::ERROR;
      }
      rc = set_value_to_record(record_data, value, field, record_size);
    } else {
      rc = set_value_to_record(record_data, value, field, record_size);
    }
  }
  if (OB_FAIL(rc)) {
    LOG_WARN("failed to make record. table name:%s", table_meta_.name());
    free(record_data);
    return rc;
  }

  record.set_data_owner(record_data, record_size);
  return RC::SUCCESS;
}


std::string Table::record_encode_key(int table_id, Record &record) {
  string key_ret = string("t") + std::to_string(table_id) + "_r";
  const char *data_beg = record.data();
  int field0_len = table_meta_.field(0)->len();
  key_ret += std::string(data_beg, field0_len);
  return key_ret;
  // std::to_string()
}

RC Table::set_value_to_record(char *record_data, const Value &value, const FieldMeta *field, const int record_len)
{

  size_t       copy_len = field->len();
  const size_t data_len = value.length();
  if (value.attr_type() == AttrType::CHARS) {
    if(field->len()<value.length() ){
      return RC::ERROR;
    }
    if (copy_len > data_len) {
      copy_len = data_len + 1;
    }
  }
  memcpy(record_data + field->offset(), value.data(), copy_len);
  bool is_null = value.is_null();
  memcpy(record_data + record_len - field->field_id() - 1, &is_null, 1);
  return RC::SUCCESS;
}

RC Table::init_record_handler(const char *base_dir)
{
  if (is_view()) {
    std::cout << "can not be called by view" << std::endl;
    return RC::ERROR;
  }
  string data_file = table_data_file(base_dir, table_meta_.name());

  BufferPoolManager &bpm = db_->buffer_pool_manager();
  RC                 rc  = bpm.open_file(db_->log_handler(), data_file.c_str(), data_buffer_pool_);
  if (rc != RC::SUCCESS) {
    LOG_ERROR("Failed to open disk buffer pool for file:%s. rc=%d:%s", data_file.c_str(), rc, strrc(rc));
    return rc;
  }

  record_handler_ = new RecordFileHandler(table_meta_.storage_format());

  rc = record_handler_->init(*data_buffer_pool_, db_->log_handler(), &table_meta_);
  if (rc != RC::SUCCESS) {
    LOG_ERROR("Failed to init record handler. rc=%s", strrc(rc));
    data_buffer_pool_->close_file();
    data_buffer_pool_ = nullptr;
    delete record_handler_;
    record_handler_ = nullptr;
    return rc;
  }

  return rc;
}

RC Table::init_text_handler(const char *base_dir)
{
  RC     rc        = RC::SUCCESS;
  string text_file = table_text_file(base_dir, table_meta_.name());
  if (!std::filesystem::exists(std::filesystem::path(text_file))) {
    return rc;
  }

  auto &bpm = db_->buffer_pool_manager();
  rc        = bpm.open_file(db_->log_handler(), text_file.c_str(), text_buffer_pool_);
  if (rc != RC::SUCCESS) {
    LOG_ERROR("Failed to open text file.");
    return rc;
  }

  text_file_handler_ = new TextFileHandler(*text_buffer_pool_);
  return rc;
}

RC Table::init_vector_handler(const char *base_dir) {
  RC     rc        = RC::SUCCESS;
  string vector_file = table_vector_file(base_dir, table_meta_.name());
  if (!std::filesystem::exists(std::filesystem::path(vector_file))) {
    return rc;
  }

  auto &bpm = db_->buffer_pool_manager();
  rc        = bpm.open_file(db_->log_handler(), vector_file.c_str(), vector_buffer_pool_);
  if (rc != RC::SUCCESS) {
    LOG_ERROR("Failed to open text file.");
    return rc;
  }

  vector_handler_ = new TextFileHandler(*vector_buffer_pool_);
  return rc;
}

RC Table::get_record_scanner(RecordFileScanner &scanner, Trx *trx, ReadWriteMode mode)
{
  if (is_view()) {
    std::cout << "can not be called by view" << std::endl;
    return RC::ERROR;
  }
  RC rc = scanner.open_scan(this, *data_buffer_pool_, trx, db_->log_handler(), mode, nullptr);
  if (rc != RC::SUCCESS) {
    LOG_ERROR("failed to open scanner. rc=%s", strrc(rc));
  }
  return rc;
}

RC Table::get_chunk_scanner(ChunkFileScanner &scanner, Trx *trx, ReadWriteMode mode)
{
  RC rc = scanner.open_scan_chunk(this, *data_buffer_pool_, db_->log_handler(), mode);
  if (rc != RC::SUCCESS) {
    LOG_ERROR("failed to open scanner. rc=%s", strrc(rc));
  }
  return rc;
}

RC Table::create_index(Trx *trx, vector<const FieldMeta *> field_meta, const char *index_name, int unique)
{
  if (is_view()) {
    std::cout << "can not be called by view" << std::endl;
    return RC::ERROR;
  }
  if (common::is_blank(index_name) || nullptr == index_name) {
    LOG_INFO("Invalid input arguments, table name is %s, index_name is blank or attribute_name is blank", name());
    return RC::INVALID_ARGUMENT;
  }

  const int        attr_num = field_meta.size();
  const FieldMeta *field_metas[attr_num];
  for (int i = 0; i < attr_num; ++i) {
    field_metas[i] = field_meta[i];
  }

  IndexMeta new_index_meta;

  RC rc = new_index_meta.init(index_name, field_metas, attr_num, unique);
  if (rc != RC::SUCCESS) {
    LOG_INFO("Failed to init IndexMeta in table:%s, index_name:%s, field_name:%s", 
             name(), index_name, field_metas[0]->name());
    return rc;
  }

  // 创建索引相关数据
  BplusTreeIndex *index      = new BplusTreeIndex();
  string          index_file = table_index_file(base_dir_.c_str(), name(), index_name);
  rc                         = index->create(this, index_file.c_str(), new_index_meta, field_metas, attr_num);
  if (rc != RC::SUCCESS) {
    delete index;
    LOG_ERROR("Failed to create bplus tree index. file name=%s, rc=%d:%s", index_file.c_str(), rc, strrc(rc));
    return rc;
  }

  // 遍历当前的所有数据，插入这个索引
  RecordFileScanner scanner;
  rc = get_record_scanner(scanner, trx, ReadWriteMode::READ_ONLY);
  if (rc != RC::SUCCESS) {
    LOG_WARN("failed to create scanner while creating index. table=%s, index=%s, rc=%s", 
             name(), index_name, strrc(rc));
    return rc;
  }

  Record record;
  while (OB_SUCC(rc = scanner.next(record))) {
    int field_indexes[20];
    for (int i = 0; i < index->field_metas().size(); i++) {
      const int field_index = this->table_meta().find_field_index_by_name(index->field_metas()[i].name()) -
                              this->table_meta().sys_field_num();
      field_indexes[i] = field_index;
    }
    rc = index->insert_entry(record, &record.rid(), table_meta_.record_size(), field_indexes);
    if (rc != RC::SUCCESS) {
      LOG_WARN("failed to insert record into index while creating index. table=%s, index=%s, rc=%s",
               name(), index_name, strrc(rc));
      return rc;
    }
  }
  if (RC::RECORD_EOF == rc) {
    rc = RC::SUCCESS;
  } else {
    LOG_WARN("failed to insert record into index while creating index. table=%s, index=%s, rc=%s",
             name(), index_name, strrc(rc));
    return rc;
  }
  scanner.close_scan();
  LOG_INFO("inserted all records into new index. table=%s, index=%s", name(), index_name);

  indexes_.push_back(index);

  /// 接下来将这个索引放到表的元数据中
  TableMeta new_table_meta(table_meta_);
  rc = new_table_meta.add_index(new_index_meta);
  if (rc != RC::SUCCESS) {
    LOG_ERROR("Failed to add index (%s) on table (%s). error=%d:%s", index_name, name(), rc, strrc(rc));
    return rc;
  }

  /// 内存中有一份元数据，磁盘文件也有一份元数据。修改磁盘文件时，先创建一个临时文件，写入完成后再rename为正式文件
  /// 这样可以防止文件内容不完整
  // 创建元数据临时文件
  string  tmp_file = table_meta_file(base_dir_.c_str(), name()) + ".tmp";
  fstream fs;
  fs.open(tmp_file, ios_base::out | ios_base::binary | ios_base::trunc);
  if (!fs.is_open()) {
    LOG_ERROR("Failed to open file for write. file name=%s, errmsg=%s", tmp_file.c_str(), strerror(errno));
    return RC::IOERR_OPEN;  // 创建索引中途出错，要做还原操作
  }
  if (new_table_meta.serialize(fs) < 0) {
    LOG_ERROR("Failed to dump new table meta to file: %s. sys err=%d:%s", tmp_file.c_str(), errno, strerror(errno));
    return RC::IOERR_WRITE;
  }
  fs.close();

  // 覆盖原始元数据文件
  string meta_file = table_meta_file(base_dir_.c_str(), name());

  int ret = rename(tmp_file.c_str(), meta_file.c_str());
  if (ret != 0) {
    LOG_ERROR("Failed to rename tmp meta file (%s) to normal meta file (%s) while creating index (%s) on table (%s). "
              "system error=%d:%s",
              tmp_file.c_str(), meta_file.c_str(), index_name, name(), errno, strerror(errno));
    return RC::IOERR_WRITE;
  }

  table_meta_.swap(new_table_meta);

  LOG_INFO("Successfully added a new index (%s) on the table (%s)", index_name, name());
  return rc;
}

RC Table::delete_record(const RID &rid)
{
  if (is_view()) {
    std::cout << "can not be called by view" << std::endl;
    return RC::ERROR;
  }
  RC     rc = RC::SUCCESS;
  Record record;
  rc = get_record(rid, record);
  if (OB_FAIL(rc)) {
    return rc;
  }

  return delete_record(record);
}

RC Table::delete_record(const Record &record)
{
  if (is_view()) {
    std::cout << "can not be called by view" << std::endl;
    return RC::ERROR;
  }
  RC rc = RC::SUCCESS;
  for (Index *index : indexes_) {
    rc = index->delete_entry(record.data(), &record.rid());
    ASSERT(RC::SUCCESS == rc, 
           "failed to delete entry from index. table name=%s, index name=%s, rid=%s, rc=%s",
           name(), index->index_meta().name(), record.rid().to_string().c_str(), strrc(rc));
  }
  rc = record_handler_->delete_record(&record.rid());
  return rc;
}

RC Table::update_record(Record &record, char *new_record_data, int new_record_len)
{
  if (is_view()) {
    std::cout << "can not be called by view" << std::endl;
    return RC::ERROR;
  }

  RC rc = RC::SUCCESS;
  if (record.data() == nullptr) {
    return RC::EMPTY;
  }

  rc = update_entry_of_indexes(record.data(), new_record_data, record.rid());
  if (rc != RC::SUCCESS) {
    // LOG_ERROR("Failed to update indexes of record (rid=%d.%d). rc=%d:%s",
    //            record.rid().page_num, record.rid().slot_num, rc, strrc(rc));
    return rc;
  }

  record.set_data_owner(new_record_data, new_record_len);
  rc = record_handler_->update_record(&record);

  return rc;
}

RC Table::update_entry_of_indexes(const char *record, const char *new_record, const RID &rid)
{
  if (is_view()) {
    std::cout << "can not be called by view" << std::endl;
    return RC::ERROR;
  }
  RC  rc          = RC::SUCCESS;
  int record_size = table_meta_.record_size();
  for (Index *index : indexes_) {
    int record_null = 0;
    for (FieldMeta field_meta : index->field_metas()) {
      if (*(bool *)(record + record_size - field_meta.field_id() - 1)) {
        record_null = 1;
        break;
      }
    }
    rc = index->update_entry(record, new_record, &rid, record_null);
    if (rc != RC::SUCCESS) {
      return rc;
    }
  }
  return rc;
}

RC Table::insert_entry_of_indexes(Record &record, const RID &rid)
{
  if (is_view()) {
    std::cout << "can not be called by view" << std::endl;
    return RC::ERROR;
  }
  RC rc = RC::SUCCESS;
  for (Index *index : indexes_) {
    int field_indexes[20];
    for (int i = 0; i < index->field_metas().size(); i++) {
      field_indexes[i] = index->field_metas()[i].field_id();
    }
    rc = index->insert_entry(record, &rid, table_meta_.record_size(), field_indexes);
    if (rc != RC::SUCCESS) {
      break;
    }
  }
  return rc;
}

RC Table::delete_entry_of_indexes(const char *record, const RID &rid, bool error_on_not_exists)
{
  if (is_view()) {
    std::cout << "can not be called by view" << std::endl;
    return RC::ERROR;
  }
  RC rc = RC::SUCCESS;
  for (Index *index : indexes_) {
    rc = index->delete_entry(record, &rid);
    if (rc != RC::SUCCESS) {
      if (rc != RC::RECORD_INVALID_KEY || !error_on_not_exists) {
        break;
      }
    }
  }
  return rc;
}

Index *Table::find_index(const char *index_name) const
{
  if (is_view()) {
    std::cout << "can not be called by view" << std::endl;
    return nullptr;
  }
  for (Index *index : indexes_) {
    if (0 == strcmp(index->index_meta().name(), index_name)) {
      return index;
    }
  }
  return nullptr;
}
Index *Table::find_index_by_field(const char *field_name) const
{
  if (is_view()) {
    std::cout << "can not be called by view" << std::endl;
    return nullptr;
  }
  const TableMeta &table_meta = this->table_meta();
  const IndexMeta *index_meta = table_meta.find_index_by_field(field_name);
  if (index_meta != nullptr) {
    return this->find_index(index_meta->name());
  }
  return nullptr;
}

RC Table::sync()
{
  if (is_view()) {
    std::cout << "view sync should do nothing" << std::endl;
    return RC::SUCCESS;
  }
  // 没关系了
  return RC::SUCCESS;
  RC rc = RC::SUCCESS;
  for (Index *index : indexes_) {
    rc = index->sync();
    if (rc != RC::SUCCESS) {
      LOG_ERROR("Failed to flush index's pages. table=%s, index=%s, rc=%d:%s",
          name(),
          index->index_meta().name(),
          rc,
          strrc(rc));
      return rc;
    }
  }

  rc = data_buffer_pool_->flush_all_pages();
  LOG_INFO("Sync table over. table=%s", name());
  return rc;
}


RC Table::create_index(Trx *trx, vector<const FieldMeta *> field_meta, const char *index_name, int unique,string dis_type_, int lists_, int probes_)
{
  if (is_view()) {
    std::cout << "can not be called by view" << std::endl;
    return RC::ERROR;
  }
  if (common::is_blank(index_name) || nullptr == index_name) {
    LOG_INFO("Invalid input arguments, table name is %s, index_name is blank or attribute_name is blank", name());
    return RC::INVALID_ARGUMENT;
  }

  const int        attr_num = field_meta.size();
  const FieldMeta *field_metas[attr_num];
  for (int i = 0; i < attr_num; ++i) {
    field_metas[i] = field_meta[i];
  }

  IndexMeta new_index_meta;

  RC rc = new_index_meta.init(index_name, field_metas, attr_num, unique,dis_type_,lists_,probes_);
  if (rc != RC::SUCCESS) {
    LOG_INFO("Failed to init IndexMeta in table:%s, index_name:%s, field_name:%s", 
             name(), index_name, field_metas[0]->name());
    return rc;
  }

  // 创建索引相关数据
  IvfflatIndex *index      = new IvfflatIndex();
  string          index_file = table_index_file(base_dir_.c_str(), name(), index_name);
  rc                         = index->create(this, index_file.c_str(), new_index_meta, field_metas, attr_num);
  if (rc != RC::SUCCESS) {
    delete index;
    LOG_ERROR("Failed to create bplus tree index. file name=%s, rc=%d:%s", index_file.c_str(), rc, strrc(rc));
    return rc;
  }

  //遍历当前的所有数据，插入这个索引
  RecordFileScanner scanner;
  rc = get_record_scanner(scanner, trx, ReadWriteMode::READ_ONLY);
  if (rc != RC::SUCCESS) {
    LOG_WARN("failed to create scanner while creating index. table=%s, index=%s, rc=%s", 
             name(), index_name, strrc(rc));
    return rc;
  }

  Record record;
  while (OB_SUCC(rc = scanner.next(record))) {
    int field_indexes[20];
    for (int i = 0; i < index->field_metas().size(); i++) {
      const int field_index = this->table_meta().find_field_index_by_name(index->field_metas()[i].name()) -
                              this->table_meta().sys_field_num();
      field_indexes[i] = field_index;
    }
    rc = index->insert_entry(record, &record.rid(), table_meta_.record_size(), field_indexes);
    if (rc != RC::SUCCESS) {
      LOG_WARN("failed to insert record into index while creating index. table=%s, index=%s, rc=%s",
               name(), index_name, strrc(rc));
      return rc;
    }
  }
  if (RC::RECORD_EOF == rc) {
    rc = RC::SUCCESS;
  } else {
    LOG_WARN("failed to insert record into index while creating index. table=%s, index=%s, rc=%s",
             name(), index_name, strrc(rc));
    return rc;
  }
  scanner.close_scan();
  index->update_centroids();
  LOG_INFO("inserted all records into new index. table=%s, index=%s", name(), index_name);

  indexes_.push_back(index);

  /// 接下来将这个索引放到表的元数据中
  TableMeta new_table_meta(table_meta_);
  rc = new_table_meta.add_index(new_index_meta);
  if (rc != RC::SUCCESS) {
    LOG_ERROR("Failed to add index (%s) on table (%s). error=%d:%s", index_name, name(), rc, strrc(rc));
    return rc;
  }

  /// 内存中有一份元数据，磁盘文件也有一份元数据。修改磁盘文件时，先创建一个临时文件，写入完成后再rename为正式文件
  /// 这样可以防止文件内容不完整
  // 创建元数据临时文件
  string  tmp_file = table_meta_file(base_dir_.c_str(), name()) + ".tmp";
  fstream fs;
  fs.open(tmp_file, ios_base::out | ios_base::binary | ios_base::trunc);
  if (!fs.is_open()) {
    LOG_ERROR("Failed to open file for write. file name=%s, errmsg=%s", tmp_file.c_str(), strerror(errno));
    return RC::IOERR_OPEN;  // 创建索引中途出错，要做还原操作
  }
  if (new_table_meta.serialize(fs) < 0) {
    LOG_ERROR("Failed to dump new table meta to file: %s. sys err=%d:%s", tmp_file.c_str(), errno, strerror(errno));
    return RC::IOERR_WRITE;
  }
  fs.close();

  // 覆盖原始元数据文件
  string meta_file = table_meta_file(base_dir_.c_str(), name());

  int ret = rename(tmp_file.c_str(), meta_file.c_str());
  if (ret != 0) {
    LOG_ERROR("Failed to rename tmp meta file (%s) to normal meta file (%s) while creating index (%s) on table (%s). "
              "system error=%d:%s",
              tmp_file.c_str(), meta_file.c_str(), index_name, name(), errno, strerror(errno));
    return RC::IOERR_WRITE;
  }

  table_meta_.swap(new_table_meta);

  LOG_INFO("Successfully added a new index (%s) on the table (%s)", index_name, name());
  return rc;
}
