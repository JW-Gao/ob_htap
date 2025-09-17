#include "storage/text/text_manager.h"
#include "common/log/log.h"
#include "common/rc.h"
#include "common/types.h"
#include <mutex>

RC TextFileHandler::get_text(PageNum page_num, string &res) {
    RC rc = RC::SUCCESS;
    string ans;

    Frame *frame = nullptr;
    rc = disk_buffer_pool_.get_this_page(page_num, &frame);
    if (OB_FAIL(rc)) {
      LOG_ERROR("Failed to get page in text get.");
      return rc;
    }

    TextDataHeader *header = nullptr;
    char *text_data = nullptr;

    // for debug
    list<Frame *> debug_frames;

    while (frame != nullptr) {
      debug_frames.push_back(frame);
      header = reinterpret_cast<TextDataHeader *>(frame->data());
      text_data = frame->data() + sizeof(TextDataHeader);

      if (header->valid == 0) {
        rc = RC::ERROR;
        break;
      }
      PageNum next_page_num = header->next_page_num;
      int cur_len = header->length;
      ans += string(text_data, cur_len);

      frame->unpin();
      if (next_page_num == -1) {
        frame = nullptr;
      }
      else {
        rc = disk_buffer_pool_.get_this_page(next_page_num, &frame);
        if (OB_FAIL(rc)) {
          break;
        }
      }
    }

  for (auto *frame : debug_frames) {
    LOG_DEBUG("zrk, frame : %s, pin count = %d.", frame->to_string().c_str(), frame->pin_count());
  }
    res = ans;
    return rc;
  }

RC TextFileHandler::write_text(const char *text, int total_len, PageNum &page_num) {
  std::scoped_lock lock(file_mutex_); // 一把大锁
  RC rc = RC::SUCCESS;

  int len = 0;

  Frame *prev_frame  = nullptr;
  Frame *cur_frame   = nullptr;
  PageNum first_page_num = -1;
  list<Frame *> frames;

  while (len < total_len) {

    rc = disk_buffer_pool_.allocate_page(&cur_frame);
    if (rc != RC::SUCCESS) {
      LOG_ERROR("Failed to allocate page.");
      break;
    }

    ASSERT(cur_frame != nullptr, "Failed to allocate page.");
    
    char *page_data = cur_frame->data();
    char *text_data = page_data + sizeof(TextDataHeader);
    int write_len = std::min(MAX_TEXT_DATA_SIZE_EACH_PAGE, total_len - len);

    TextDataHeader *text_header = reinterpret_cast<TextDataHeader *>(page_data);
    text_header->valid = 1;
    text_header->next_page_num = -1;
    text_header->length = write_len;

    memcpy(text_data, text + len, write_len);

    len += write_len;
    if (prev_frame == nullptr) {
      first_page_num = cur_frame->page_num();
    }
    else {
      auto *prev_header = reinterpret_cast<TextDataHeader *>(prev_frame->data());
      prev_header->next_page_num = cur_frame->page_num();
    }
    cur_frame->mark_dirty();
    frames.push_back(cur_frame);
    prev_frame = cur_frame;
  }
  
  page_num = first_page_num;


  // flush pages
  for (auto *frame : frames) {
    rc = disk_buffer_pool_.flush_page(*frame);
    if (OB_FAIL(rc)) {
      LOG_ERROR("Failed to flush text page.");
      break;
    }
  }

  if (OB_SUCC(rc)) {
    for (auto *frame : frames) {
      frame->unpin();
    }
  }


  // for debug
  for (auto *frame : frames) {
    LOG_DEBUG("zrk, frame : %s, pin count = %d.", frame->to_string().c_str(), frame->pin_count());
  }

  if (rc != RC::SUCCESS) {
    LOG_DEBUG("Failed to write text");
  }
  return rc;
}


RC TextFileHandler::delete_text(PageNum page_num) {
  std::scoped_lock lock(file_mutex_);
  RC rc = RC::SUCCESS;
  list<Frame *> frames;


  Frame *frame = nullptr;
  rc = disk_buffer_pool_.get_this_page(page_num, &frame); // it pins the frame
  if (OB_FAIL(rc)) {
    LOG_ERROR("Failed to get page in text get.");
    return rc;
  }

  if (frame->pin_count() > 1) {
    frame->unpin();
    LOG_WARN("The text was read by others.");
    rc = RC::ERROR;
    return rc;
  }

  PageNum next_page_num = -1;
  TextDataHeader *header = nullptr;
  // char *text_data = frame->data() + sizeof(TextDataHeader);

  
  
  while (frame != nullptr) {
    frames.push_back(frame);
    header = reinterpret_cast<TextDataHeader *>(frame->data());
    if (header->valid == 0) {
      LOG_ERROR("delete a invalid text file.");
      rc = RC::ERROR;
      return rc;
    }
    // text_data = frame->data() + sizeof(TextDataHeader);
    next_page_num = header->next_page_num;

    ASSERT(frame->pin_count() == 1, "Can't delete the text cause it's used by others.");

    if (next_page_num == -1) {
      frame = nullptr;
    }
    else {
      rc = disk_buffer_pool_.get_this_page(next_page_num, &frame);
      if (OB_FAIL(rc)) {
        LOG_ERROR("Failed to get page %d in file %s.", next_page_num, disk_buffer_pool_.filename());
        break;
      }
    }
  }


  for (auto *frame : frames) {
    frame->unpin();
    rc = disk_buffer_pool_.dispose_page(frame->page_num());
    if (OB_FAIL(rc)) {
      LOG_ERROR("zrk, Failed to purge page in text file.");
      break;
    }
  }
  return rc;
}

RC TextFileHandler::update_text(PageNum in_page, const char *text, int total_len, PageNum &out_page) {
  RC rc = RC::SUCCESS;

  // 偷懒，先删再建立
  rc = delete_text(in_page);
  if (OB_FAIL(rc)) {
    LOG_ERROR("zrk, update_text, failed in DELETE part.");
    return rc;
  }

  rc = write_text(text, total_len, out_page);
  if (OB_FAIL(rc)) {
    LOG_ERROR("zrk, update_text, failed in WRITE part.");
    return rc;
  }
  return rc;
}