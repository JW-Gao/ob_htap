#include "common/lang/sstream.h"
#include "common/log/log.h"
#include "common/type/attr_type.h"
#include "common/type/high_vector_type.h"
#include "common/type/values_type.h"
#include "common/value.h"
#include "storage/text/text_manager.h"


RC HighVectorType::cast_to(const Value &val, AttrType type, Value &result) const {
  RC rc = RC::SUCCESS;
  vector<float> vf;
  TextFileHandler *vector_handler= val.get_vector_handler();
  int page_num = val.get_int();

  if (vector_handler == nullptr) {
    LOG_ERROR("Failed to get text handler.");
    rc = RC::ERROR;
    return rc;
  }
  string tmp_result;
  rc = vector_handler->get_text(page_num, tmp_result);
  if (rc != RC::SUCCESS) {
    return rc;
  }
  const float *vecs = (const float *)tmp_result.c_str();
  for (int i = 0; i < tmp_result.length() / 4; i++) {
    vf.push_back(vecs[i]);
  }
  result.set_vecs(vf);
  result.set_type(AttrType::VECTORS);
  return rc;
}

RC HighVectorType::to_string(const Value &val, string &result) const {
  RC rc = RC::SUCCESS;
  TextFileHandler *vector_handler= val.get_vector_handler();
  int page_num = val.get_int();

  if (vector_handler == nullptr) {
    LOG_ERROR("Failed to get text handler.");
    rc = RC::ERROR;
    return rc;
  }
  result = "[";
  string tmp_result;
  rc = vector_handler->get_text(page_num, tmp_result);
  if (rc != RC::SUCCESS) {
    return rc;
  }

  const float *vecs = (const float *)tmp_result.c_str();
  for (int i = 0; i < tmp_result.length() / 4; i++) {
    result += common::double_to_str(vecs[i]) + ',';
  }
  result[result.size() - 1] = ']';
  return rc;
}