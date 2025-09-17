#pragma once

#include <cstdint>
#include <cstring>
inline int32_t get_int(const char *c) {
  return *reinterpret_cast<const int32_t *>(c);
}

inline void put_int(int32_t n, char *c) {
  std::memcpy(c, &n, sizeof(int32_t));
}