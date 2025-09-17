#include "common/lang/sstream.h"
#include "common/log/log.h"
#include "common/type/attr_type.h"
#include "common/type/text_type.h"
#include "common/type/values_type.h"
#include "common/value.h"
#include "storage/text/text_manager.h"
#include <cstdint>
#include <iomanip>

RC TextType::to_string(const Value &val, string &result) const {
  RC rc = RC::SUCCESS;
  TextFileHandler *text_file_handler = val.get_text_file_handler();
  int page_num = val.get_int();

  if (text_file_handler == nullptr) {
    LOG_ERROR("Failed to get text handler.");
    rc = RC::ERROR;
    return rc;
  }
  
  rc = text_file_handler->get_text(page_num, result);
  if (rc != RC::SUCCESS) {
    return rc;
  }
  return rc;
}