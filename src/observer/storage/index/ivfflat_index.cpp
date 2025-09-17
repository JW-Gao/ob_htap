/* Copyright (c) 2021 OceanBase and/or its affiliates. All rights reserved.
miniob is licensed under Mulan PSL v2.
You can use this software according to the terms and conditions of the Mulan PSL v2.
You may obtain a copy of Mulan PSL v2 at:
         http://license.coscl.org.cn/MulanPSL2
THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
See the Mulan PSL v2 for more details. */

#include "storage/index/ivfflat_index.h"
#include "common/lang/string.h"
#include "storage/index/index.h"
#include "storage/text/text_manager.h"
#include <vector>
#include <cmath>
#include <limits>
#include <unordered_map>
#include "storage/table/table.h"
#include "storage/db/db.h"

RC IvfflatIndex::create(
    Table *table, const char *file_name, const IndexMeta &index_meta, const FieldMeta *field_metas[], int field_num)
{
  if (inited_) {
    LOG_WARN("Failed to create index due to the index has been created before. file_name:%s, index:%s, field:%s, unique:%d",
        file_name, index_meta.name(), index_meta.field(0), index_meta.unique());
    return RC::RECORD_OPENNED;
  }

  Index::init(index_meta, field_metas, field_num);

  BufferPoolManager &bpm = table->db()->buffer_pool_manager();

  AttrType attr_types[field_num];
  int      attr_len[field_num];

  for (int i = 0; i < field_num; i++) {
    attr_types[i] = field_metas[i]->type();
    attr_len[i]   = field_metas[i]->len();
  }
  // 这个b+数先不删，删了之后的同步有问题，目前只做内存结构，为了不报错，先保留
  RC rc = index_handler_.create(
      table->db()->log_handler(), bpm, file_name, attr_types, attr_len, field_num, index_meta.unique());
  if (RC::SUCCESS != rc) {
    LOG_WARN("Failed to create index_handler, file_name:%s, index:%s, field:%s, rc:%s",
        file_name, index_meta.name(), index_meta.field(0), strrc(rc));
    return rc;
  }

  inited_ = true;
  table_  = table;
  LOG_INFO("Successfully create index, file_name:%s, index:%s, field:%s, unique:%d" ,
    file_name, index_meta.name(), index_meta.field(0), index_meta.unique());

  dim_            = field_metas[0]->len() / 4;
  lists_          = index_meta.lists();
  probes_         = index_meta.probes();
  offset_         = field_metas[0]->offset();
  string type_str = index_meta.dis_type();
  type_str        = common::str_to_lower(type_str);
  if (type_str == "l2_distance") {
    type_ = l2_;
  } else if (type_str == "inner_product") {
    type_ = inner_;
  } else if (type_str == "cosine_distance") {
    type_ = cosine_;
  }
  kmeans.init(lists_, dim_);

  return RC::SUCCESS;
};


  RC IvfflatIndex::insert_entry(Record &record, const RID *rid, const int record_size, int field_indexs[]) 
  {
    int           cluster_id = kmeans.assign_cluster((float *)(record.data() + offset_), type_);
    vector<float> tmp;
    tmp.reserve(dim_);
    for (int i = 0; i < dim_; i++) {
      tmp.push_back(((float *)(record.data() + offset_))[i]);
    }
    inverted_list[cluster_id].push_back(make_pair(*rid, std::move(tmp)));
    return RC::SUCCESS;
  };


    // 搜索最近的向量
  std::vector<RID> IvfflatIndex::search(float *query, int limit)
  {
    // 1. 找到最近的聚类中心
    std::vector<int> cluster_ids = kmeans.assign_cluster_with_probes(query, inverted_list, type_, probes_);

    // 2. 在该聚类内搜索最近的向量
    VacuousTrx       trx;
    Record           record;
    std::vector<RID> closest_rids;

    // 定义一个优先队列（小顶堆），存储距离和对应的 RID
    using DistanceAndRID = std::pair<float, RID>;
    std::priority_queue<DistanceAndRID, std::vector<DistanceAndRID>, CompareDistanceAndRID> closest_queue;

#pragma omp parallel for
    for (int i = 0; i < cluster_ids.size(); i++) {
#pragma omp parallel for
      for (const auto &rid : inverted_list[cluster_ids[i]]) {
        // table_->get_record(rid, record);
        float dist = euclidean_distance(query, rid.second.data(), dim_, type_);

// 如果小顶堆的大小小于 limit，直接插入
#pragma omp critical
        if (closest_queue.size() < limit) {
          closest_queue.emplace(dist, rid.first);
        } else {
          // 如果堆已满，且当前距离比堆顶大，则替换堆顶
          if (dist < closest_queue.top().first) {
            closest_queue.pop();                     // 移除堆顶
            closest_queue.emplace(dist, rid.first);  // 插入当前更近的向量
          }
        }
      }
    }
    // // 合并
    // std::priority_queue<DistanceAndRID, std::vector<DistanceAndRID>, CompareDistanceAndRID> closest_queue_merge;
    // for (int i = 0; i < cluster_ids.size(); i++) {
    //   while (!closest_queue[i].empty()) {
    //     float dist = closest_queue[i].top().first;
    //     RID   rid  = closest_queue[i].top().second;
    //     closest_queue[i].pop();

    //     // 如果全局堆的大小小于 limit，直接插入
    //     if (closest_queue_merge.size() < limit) {
    //       closest_queue_merge.emplace(dist, rid);
    //     } else {
    //       // 如果全局堆已满，且当前距离比堆顶小，则替换堆顶
    //       if (dist < closest_queue_merge.top().first) {
    //         closest_queue_merge.pop();               // 移除堆顶
    //         closest_queue_merge.emplace(dist, rid);  // 插入当前更近的向量
    //       }
    //     }
    //   }
    // }
    // 将小顶堆中的结果放入 closest_rids 中
    while (!closest_queue.empty()) {
      closest_rids.push_back(closest_queue.top().second);
      closest_queue.pop();
    }
    std::reverse(closest_rids.begin(), closest_rids.end());

    return closest_rids;  // 返回 limit 个最近的向量
  }

  void IvfflatIndex::update_centroids()
  {
    kmeans.update_centroids(inverted_list, table_, offset_);
    inverted_list.clear();
    Record            record;
    RecordFileScanner scanner;
    VacuousTrx        trx;
    RC                rc = table_->get_record_scanner(scanner, &trx, ReadWriteMode::READ_ONLY);
    while (OB_SUCC(rc = scanner.next(record))) {
      rc = insert_entry(record, &record.rid(), -1, nullptr);
    }
    if (RC::RECORD_EOF == rc) {
      rc = RC::SUCCESS;
    } else {
    }
    scanner.close_scan();
  }