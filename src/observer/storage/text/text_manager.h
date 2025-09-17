#pragma once

#include "common/log/log.h"
#include "common/rc.h"
#include "common/types.h"
#include "storage/buffer/disk_buffer_pool.h"
#include "storage/buffer/page.h"
#include "storage/text/text.h"
#include "storage/text/text_util.h"

#define CUR_PAGE_OFFSET 0
#define NEXT_PAGE_OFFSET 4
#define PAGE_COUNT_OFFSET 8
#define TEXT_LENGTH_OFFSET 12
#define MAX_TEXT_LENGTH 65535
#define MAX_VECTOR_DIM 16000

class Table;

struct TextDataHeader {
  int32_t valid;
  int32_t next_page_num;
  uint32_t length;
};

static constexpr int MAX_TEXT_DATA_SIZE_EACH_PAGE = (BP_PAGE_DATA_SIZE - sizeof(TextDataHeader));

class TextFileHandler {
public:
  TextFileHandler(DiskBufferPool &disk_buffer_pool) : disk_buffer_pool_(disk_buffer_pool) {}

  RC get_text(PageNum page_num, string &res);

  RC write_text(const char *text, int total_len, PageNum &page_num);

  RC delete_text(PageNum page_num);

  RC update_text(PageNum in_page, const char *text, int total_len, PageNum &out_page);
private:
    // Table *table_ = nullptr;
    DiskBufferPool &disk_buffer_pool_;
    std::mutex file_mutex_;
};