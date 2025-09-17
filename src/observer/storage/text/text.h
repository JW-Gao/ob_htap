#pragma once
#include "common/types.h"
#include "storage/buffer/frame.h"
#include <list>

struct Text {
    PageNum page_num_;
    int page_count_;
    std::list<Frame *> frames;
};