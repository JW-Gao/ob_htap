/* Copyright (c) 2021 OceanBase and/or its affiliates. All rights reserved.
miniob is licensed under Mulan PSL v2.
You can use this software according to the terms and conditions of the Mulan PSL v2.
You may obtain a copy of Mulan PSL v2 at:
         http://license.coscl.org.cn/MulanPSL2
THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
See the Mulan PSL v2 for more details. */

#pragma once

#include "common/type/data_type.h"
#include <unordered_map>

/**
 * @brief 整型类型
 * @ingroup DataType
 */
class DateType : public DataType
{
public:
  DateType() : DataType(AttrType::DATES) {}
  virtual ~DateType() {}

  int compare(const Value &left, const Value &right) const override;


  RC set_value_from_str(Value &val, const string &data) const override;

  RC to_string(const Value &val, string &result) const override;

private:
  // static vector<const char *> week_num_to_str_;
  static vector<const char *> month_num_to_str_;
  static std::unordered_map<string, int> week_str_to_num_;

  std::string getDayOfWeek(int year, int month, int day) const {
    // 如果月份是1月或2月，则将其视为前一年的13月或14月
    if (month < 3) {
        month += 12;
        year--;
    }

    // 蔡勒公式
    int K = year % 100; // 年份的后两位
    int J = year / 100; // 年份的前两位

    // 计算星期几
    int f = day + (13 * (month + 1)) / 5 + K + K / 4 + J / 4 + 5 * J;
    int dayOfWeek = f % 7;

    // 星期几的名称
    switch (dayOfWeek) {
        case 0: return "Saturday";
        case 1: return "Sunday";
        case 2: return "Monday";
        case 3: return "Tuesday";
        case 4: return "Wednesday";
        case 5: return "Thursday";
        case 6: return "Friday";
        default: return ""; // 不会到这里
    }
}
};