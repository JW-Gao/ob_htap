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
// Created by Wangyunlai on 2022/07/05.
//

#include "common/log/log.h"
#include "common/rc.h"
#include "common/type/attr_type.h"
#include "common/value.h"
#include "sql/expr/aggregator.h"
#include "sql/expr/expression.h"
#include "sql/expr/tuple.h"
#include "sql/expr/arithmetic_operator.hpp"
#include "sql/operator/logical_operator.h"
#include "sql/optimizer/logical_plan_generator.h"
#include "sql/optimizer/physical_plan_generator.h"
#include "sql/parser/parse_defs.h"
#include "sql/parser/yacc_sql.hpp"
#include <cstdint>
#include <iomanip>
#include <math.h>
#include <memory>
#include <vector>
#include <cmath>

using namespace std;

RC FieldExpr::get_value(const Tuple &tuple, Value &value) const { return tuple.find_cell(*field_spec_, value); }

bool FieldExpr::equal(const Expression &other) const
{
  if (this == &other) {
    return true;
  }
  if (other.type() != ExprType::FIELD) {
    return false;
  }
  const auto &other_field_expr = static_cast<const FieldExpr &>(other);
  return table_name() == other_field_expr.table_name() && field_name() == other_field_expr.field_name();
}

// TODO: 在进行表达式计算时，`chunk` 包含了所有列，因此可以通过 `field_id` 获取到对应列。
// 后续可以优化成在 `FieldExpr` 中存储 `chunk` 中某列的位置信息。
RC FieldExpr::get_column(Chunk &chunk, Column &column)
{
  if (pos_ != -1) {
    column.reference(chunk.column(pos_));
  } else {
    column.reference(chunk.column(field().meta()->field_id()));
  }
  return RC::SUCCESS;
}

bool ValueExpr::equal(const Expression &other) const
{
  if (this == &other) {
    return true;
  }
  if (other.type() != ExprType::VALUE) {
    return false;
  }
  const auto &other_value_expr = static_cast<const ValueExpr &>(other);
  return value_.compare(other_value_expr.get_value()) == 0;
}

RC ValueExpr::get_value(const Tuple &tuple, Value &value) const
{
  value = value_;
  return RC::SUCCESS;
}

RC ValueExpr::get_column(Chunk &chunk, Column &column)
{
  column.init(value_);
  return RC::SUCCESS;
}

/////////////////////////////////////////////////////////////////////////////////
CastExpr::CastExpr(unique_ptr<Expression> child, AttrType cast_type) : child_(std::move(child)), cast_type_(cast_type)
{}

CastExpr::~CastExpr() {}

RC CastExpr::cast(const Value &value, Value &cast_value) const
{
  RC rc = RC::SUCCESS;
  if (this->value_type() == value.attr_type()) {
    cast_value = value;
    return rc;
  }
  rc = Value::cast_to(value, cast_type_, cast_value);
  return rc;
}

RC CastExpr::get_value(const Tuple &tuple, Value &result) const
{
  Value value;
  RC    rc = child_->get_value(tuple, value);
  if (rc != RC::SUCCESS) {
    return rc;
  }

  return cast(value, result);
}

RC CastExpr::try_get_value(Value &result) const
{
  Value value;
  RC    rc = child_->try_get_value(value);
  if (rc != RC::SUCCESS) {
    return rc;
  }

  return cast(value, result);
}

////////////////////////////////////////////////////////////////////////////////

ComparisonExpr::ComparisonExpr(CompOp comp, unique_ptr<Expression> left, unique_ptr<Expression> right)
    : comp_(comp), left_(std::move(left)), right_(std::move(right))
{}

ComparisonExpr::~ComparisonExpr() {}

RC ComparisonExpr::compare_value(const Value &left, const Value &right, bool &result) const
{
  RC  rc         = RC::SUCCESS;
  int cmp_result = left.compare(right);
  result         = false;
  // 这里写一些比较特殊的情况，谁是高手可以优化一下

  // 这里处理一些子查询比较的情况
  if (left.attr_type() == AttrType::VALUESLISTS && left.get_valuelist()->size() != 1) {
    if (left.get_valuelist()->size() > 1) {
      result = false;
      return RC::ERROR;  // 返回错误，而不只是过滤掉tuple
    } else {
      result = false;
      if (comp_ == NOT_EQUAL) {
        return RC::ERROR;
      } else {
        return RC::SUCCESS;
      }
    }
  }
  if (right.attr_type() == AttrType::VALUESLISTS && right.get_valuelist()->size() != 1) {

    if (right.get_valuelist()->size() > 1) {
      result = false;
      return RC::ERROR;  // 返回错误，而不只是过滤掉tuple
    } else if (comp_ == NOT_IN_VALUELIST) {
      result = true;  // 可能是子查询没有值
      return RC::SUCCESS;
    } else {
      result = false;
      if (comp_ == NOT_EQUAL) {
        return RC::ERROR;
      } else {
        return RC::SUCCESS;
      }
    }
  }

  // 处理有null的情况
  if (left.attr_type() == AttrType::NULLS || right.attr_type() == AttrType::NULLS) {

    switch (comp_) {
      case IS_NOT:
        if (left.attr_type() == AttrType::NULLS && right.attr_type() == AttrType::NULLS) {
          result = false;
          return rc;
        } else if (left.attr_type() != AttrType::NULLS && right.attr_type() == AttrType::NULLS) {
          result = true;
          return rc;
        }
        break;
      case IS_NULL:
        if (left.attr_type() == AttrType::NULLS && right.attr_type() == AttrType::NULLS) {
          result = true;
          return rc;
        } else if (left.attr_type() != AttrType::NULLS && right.attr_type() == AttrType::NULLS) {
          result = false;
          return rc;
        }
        break;
      default: result = false; return rc;
    }
  }

  if (cmp_result == INT32_MAX || cmp_result == -INT32_MAX) {
    result = false;
    return rc;
  }

  switch (comp_) {
    case EQUAL_TO:
    // in 和 not in 的比较逻辑分别对应等于和不等于
    case IN_VALUELIST: {
      result = (0 == cmp_result);
    } break;
    case LESS_EQUAL: {
      result = (cmp_result <= 0);
    } break;
    case NOT_IN_VALUELIST:
    case NOT_EQUAL: {
      result = (cmp_result != 0);
    } break;
    case LESS_THAN: {
      result = (cmp_result < 0);
    } break;
    case GREAT_EQUAL: {
      result = (cmp_result >= 0);
    } break;
    case GREAT_THAN: {
      result = (cmp_result > 0);
    } break;
    case IS_NULL: {
      result = (cmp_result == 0);
    } break;
    case IS_NOT: {
      result = (cmp_result != 0);
    } break;
    default: {
      LOG_WARN("unsupported comparison. %d", comp_);
      rc = RC::INTERNAL;
    } break;
  }

  return rc;
}

RC ComparisonExpr::try_get_value(Value &cell) const
{
  if (left_->type() == ExprType::VALUE && right_->type() == ExprType::VALUE) {
    ValueExpr   *left_value_expr  = static_cast<ValueExpr *>(left_.get());
    ValueExpr   *right_value_expr = static_cast<ValueExpr *>(right_.get());
    const Value &left_cell        = left_value_expr->get_value();
    const Value &right_cell       = right_value_expr->get_value();

    bool value = false;
    RC   rc    = compare_value(left_cell, right_cell, value);
    if (rc != RC::SUCCESS) {
      LOG_WARN("failed to compare tuple cells. rc=%s", strrc(rc));
    } else {
      cell.set_boolean(value);
    }
    return rc;
  }

  return RC::INVALID_ARGUMENT;
}

RC ComparisonExpr::get_value(const Tuple &tuple, Value &value) const
{
  Value left_value;
  Value right_value;

  RC rc = left_->get_value(tuple, left_value);
  if (rc != RC::SUCCESS) {
    LOG_WARN("failed to get value of left expression. rc=%s", strrc(rc));
    return rc;
  }
  rc = right_->get_value(tuple, right_value);
  if (rc != RC::SUCCESS) {
    LOG_WARN("failed to get value of right expression. rc=%s", strrc(rc));
    return rc;
  }

  bool bool_value = false;
  switch (comp_) {
    case IN_VALUELIST: {
      if (right_value.attr_type() != AttrType::VALUESLISTS) {
        return RC::UNSUPPORTED;
      }
      std::vector<Value> &values = *right_value.get_valuelist();
      for (auto &val : values) {
        rc = compare_value(left_value, val, bool_value);
        if (bool_value) {
          break;
        }
      }
    } break;
    case NOT_IN_VALUELIST: {
      if (right_value.attr_type() != AttrType::VALUESLISTS) {
        return RC::UNSUPPORTED;
      }
      std::vector<Value> &values = *right_value.get_valuelist();
      if (values.empty()) {
        bool_value = true;
      }
      for (auto &val : values) {
        rc = compare_value(left_value, val, bool_value);
        if (!bool_value) {
          break;
        }
      }
    } break;

    case LIKE_OP: {
      bool_value = left_value.str_like(right_value);
      break;
    }
    case NOT_LIKE_OP: {
      bool_value = !left_value.str_like(right_value);
      break;
    }
    default: rc = compare_value(left_value, right_value, bool_value);
  }

  if (rc == RC::SUCCESS) {
    value.set_boolean(bool_value);
  }
  return rc;
}

RC ComparisonExpr::eval(Chunk &chunk, std::vector<uint8_t> &select)
{
  RC     rc = RC::SUCCESS;
  Column left_column;
  Column right_column;

  rc = left_->get_column(chunk, left_column);
  if (rc != RC::SUCCESS) {
    LOG_WARN("failed to get value of left expression. rc=%s", strrc(rc));
    return rc;
  }
  rc = right_->get_column(chunk, right_column);
  if (rc != RC::SUCCESS) {
    LOG_WARN("failed to get value of right expression. rc=%s", strrc(rc));
    return rc;
  }
  if (left_column.attr_type() != right_column.attr_type()) {
    LOG_WARN("cannot compare columns with different types");
    return RC::INTERNAL;
  }
  if (left_column.attr_type() == AttrType::INTS || left_column.attr_type() == AttrType::DATES) {
    rc = compare_column<int>(left_column, right_column, select);
  } else if (left_column.attr_type() == AttrType::FLOATS) {
    rc = compare_column<float>(left_column, right_column, select);
  } else {
    // TODO: support string compare
    LOG_WARN("unsupported data type %d", left_column.attr_type());
    return RC::INTERNAL;
  }
  return rc;
}

template <typename T>
RC ComparisonExpr::compare_column(const Column &left, const Column &right, std::vector<uint8_t> &result) const
{
  RC rc = RC::SUCCESS;

  bool left_const  = left.column_type() == Column::Type::CONSTANT_COLUMN;
  bool right_const = right.column_type() == Column::Type::CONSTANT_COLUMN;
  if (left_const && right_const) {
    compare_result<T, true, true>((T *)left.data(), (T *)right.data(), left.count(), result, comp_);
  } else if (left_const && !right_const) {
    compare_result<T, true, false>((T *)left.data(), (T *)right.data(), right.count(), result, comp_);
  } else if (!left_const && right_const) {
    compare_result<T, false, true>((T *)left.data(), (T *)right.data(), left.count(), result, comp_);
  } else {
    compare_result<T, false, false>((T *)left.data(), (T *)right.data(), left.count(), result, comp_);
  }
  return rc;
}

////////////////////////////////////////////////////////////////////////////////
ConjunctionExpr::ConjunctionExpr(Type type, vector<unique_ptr<Expression>> &children)
    : conjunction_type_(type), children_(std::move(children))
{}

RC ConjunctionExpr::get_value(const Tuple &tuple, Value &value) const
{
  RC rc = RC::SUCCESS;
  if (children_.empty()) {
    value.set_boolean(true);
    return rc;
  }

  Value tmp_value;
  for (const unique_ptr<Expression> &expr : children_) {
    rc = expr->get_value(tuple, tmp_value);
    if (rc != RC::SUCCESS) {
      LOG_WARN("failed to get value by child expression. rc=%s", strrc(rc));
      return rc;
    }
    bool bool_value = tmp_value.get_boolean();
    if ((conjunction_type_ == Type::AND && !bool_value) || (conjunction_type_ == Type::OR && bool_value)) {
      value.set_boolean(bool_value);
      return rc;
    }
  }

  bool default_value = (conjunction_type_ == Type::AND);
  value.set_boolean(default_value);
  return rc;
}

////////////////////////////////////////////////////////////////////////////////

ArithmeticExpr::ArithmeticExpr(ArithmeticExpr::Type type, Expression *left, Expression *right)
    : arithmetic_type_(type), left_(left), right_(right)
{}
ArithmeticExpr::ArithmeticExpr(ArithmeticExpr::Type type, unique_ptr<Expression> left, unique_ptr<Expression> right)
    : arithmetic_type_(type), left_(std::move(left)), right_(std::move(right))
{}

bool ArithmeticExpr::equal(const Expression &other) const
{
  if (this == &other) {
    return true;
  }
  if (type() != other.type()) {
    return false;
  }
  auto &other_arith_expr = static_cast<const ArithmeticExpr &>(other);
  return arithmetic_type_ == other_arith_expr.arithmetic_type() && left_->equal(*other_arith_expr.left_) &&
         right_->equal(*other_arith_expr.right_);
}
AttrType ArithmeticExpr::value_type() const
{
  if (!right_) {
    return left_->value_type();
  }

  if (left_->value_type() == AttrType::INTS && right_->value_type() == AttrType::INTS &&
      arithmetic_type_ != Type::DIV) {
    return AttrType::INTS;
  }

  if (left_->value_type() == AttrType::VECTORS && arithmetic_type_ != Type::DIV) {
    return AttrType::VECTORS;
  }

  return AttrType::FLOATS;
}

RC ArithmeticExpr::calc_value(const Value &left_value, const Value &right_value, Value &value) const
{
  RC rc = RC::SUCCESS;

  const AttrType target_type = value_type();
  value.set_type(target_type);

  switch (arithmetic_type_) {
    case Type::ADD: {
      Value::add(left_value, right_value, value);
    } break;

    case Type::SUB: {
      Value::subtract(left_value, right_value, value);
    } break;

    case Type::MUL: {
      Value::multiply(left_value, right_value, value);
    } break;

    case Type::DIV: {
      Value::divide(left_value, right_value, value);
    } break;

    case Type::NEGATIVE: {
      Value::negative(left_value, value);
    } break;

    default: {
      rc = RC::INTERNAL;
      LOG_WARN("unsupported arithmetic type. %d", arithmetic_type_);
    } break;
  }
  return rc;
}

template <bool LEFT_CONSTANT, bool RIGHT_CONSTANT>
RC ArithmeticExpr::execute_calc(
    const Column &left, const Column &right, Column &result, Type type, AttrType attr_type) const
{
  RC rc = RC::SUCCESS;
  switch (type) {
    case Type::ADD: {
      if (attr_type == AttrType::INTS) {
        binary_operator<LEFT_CONSTANT, RIGHT_CONSTANT, int, AddOperator>(
            (int *)left.data(), (int *)right.data(), (int *)result.data(), result.capacity());
      } else if (attr_type == AttrType::FLOATS) {
        binary_operator<LEFT_CONSTANT, RIGHT_CONSTANT, float, AddOperator>(
            (float *)left.data(), (float *)right.data(), (float *)result.data(), result.capacity());
      } else {
        rc = RC::UNIMPLEMENTED;
      }
    } break;
    case Type::SUB:
      if (attr_type == AttrType::INTS) {
        binary_operator<LEFT_CONSTANT, RIGHT_CONSTANT, int, SubtractOperator>(
            (int *)left.data(), (int *)right.data(), (int *)result.data(), result.capacity());
      } else if (attr_type == AttrType::FLOATS) {
        binary_operator<LEFT_CONSTANT, RIGHT_CONSTANT, float, SubtractOperator>(
            (float *)left.data(), (float *)right.data(), (float *)result.data(), result.capacity());
      } else {
        rc = RC::UNIMPLEMENTED;
      }
      break;
    case Type::MUL:
      if (attr_type == AttrType::INTS) {
        binary_operator<LEFT_CONSTANT, RIGHT_CONSTANT, int, MultiplyOperator>(
            (int *)left.data(), (int *)right.data(), (int *)result.data(), result.capacity());
      } else if (attr_type == AttrType::FLOATS) {
        binary_operator<LEFT_CONSTANT, RIGHT_CONSTANT, float, MultiplyOperator>(
            (float *)left.data(), (float *)right.data(), (float *)result.data(), result.capacity());
      } else {
        rc = RC::UNIMPLEMENTED;
      }
      break;
    case Type::DIV:
      if (attr_type == AttrType::INTS) {
        binary_operator<LEFT_CONSTANT, RIGHT_CONSTANT, int, DivideOperator>(
            (int *)left.data(), (int *)right.data(), (int *)result.data(), result.capacity());
      } else if (attr_type == AttrType::FLOATS) {
        binary_operator<LEFT_CONSTANT, RIGHT_CONSTANT, float, DivideOperator>(
            (float *)left.data(), (float *)right.data(), (float *)result.data(), result.capacity());
      } else {
        rc = RC::UNIMPLEMENTED;
      }
      break;
    case Type::NEGATIVE:
      if (attr_type == AttrType::INTS) {
        unary_operator<LEFT_CONSTANT, int, NegateOperator>((int *)left.data(), (int *)result.data(), result.capacity());
      } else if (attr_type == AttrType::FLOATS) {
        unary_operator<LEFT_CONSTANT, float, NegateOperator>(
            (float *)left.data(), (float *)result.data(), result.capacity());
      } else {
        rc = RC::UNIMPLEMENTED;
      }
      break;
    default: rc = RC::UNIMPLEMENTED; break;
  }
  if (rc == RC::SUCCESS) {
    result.set_count(result.capacity());
  }
  return rc;
}

RC ArithmeticExpr::get_value(const Tuple &tuple, Value &value) const
{
  RC rc = RC::SUCCESS;

  Value left_value;
  Value right_value;

  rc = left_->get_value(tuple, left_value);
  if (rc != RC::SUCCESS) {
    LOG_WARN("failed to get value of left expression. rc=%s", strrc(rc));
    return rc;
  }
  rc = right_->get_value(tuple, right_value);
  if (rc != RC::SUCCESS) {
    LOG_WARN("failed to get value of right expression. rc=%s", strrc(rc));
    return rc;
  }
  return calc_value(left_value, right_value, value);
}

RC ArithmeticExpr::get_column(Chunk &chunk, Column &column)
{
  RC rc = RC::SUCCESS;
  if (pos_ != -1) {
    column.reference(chunk.column(pos_));
    return rc;
  }
  Column left_column;
  Column right_column;

  rc = left_->get_column(chunk, left_column);
  if (rc != RC::SUCCESS) {
    LOG_WARN("failed to get column of left expression. rc=%s", strrc(rc));
    return rc;
  }
  rc = right_->get_column(chunk, right_column);
  if (rc != RC::SUCCESS) {
    LOG_WARN("failed to get column of right expression. rc=%s", strrc(rc));
    return rc;
  }
  return calc_column(left_column, right_column, column);
}

RC ArithmeticExpr::calc_column(const Column &left_column, const Column &right_column, Column &column) const
{
  RC rc = RC::SUCCESS;

  const AttrType target_type = value_type();
  column.init(target_type, left_column.attr_len(), std::max(left_column.count(), right_column.count()));
  bool left_const  = left_column.column_type() == Column::Type::CONSTANT_COLUMN;
  bool right_const = right_column.column_type() == Column::Type::CONSTANT_COLUMN;
  if (left_const && right_const) {
    column.set_column_type(Column::Type::CONSTANT_COLUMN);
    rc = execute_calc<true, true>(left_column, right_column, column, arithmetic_type_, target_type);
  } else if (left_const && !right_const) {
    column.set_column_type(Column::Type::NORMAL_COLUMN);
    rc = execute_calc<true, false>(left_column, right_column, column, arithmetic_type_, target_type);
  } else if (!left_const && right_const) {
    column.set_column_type(Column::Type::NORMAL_COLUMN);
    rc = execute_calc<false, true>(left_column, right_column, column, arithmetic_type_, target_type);
  } else {
    column.set_column_type(Column::Type::NORMAL_COLUMN);
    rc = execute_calc<false, false>(left_column, right_column, column, arithmetic_type_, target_type);
  }
  return rc;
}

RC ArithmeticExpr::try_get_value(Value &value) const
{
  RC rc = RC::SUCCESS;

  Value left_value;
  Value right_value;

  rc = left_->try_get_value(left_value);
  if (rc != RC::SUCCESS) {
    LOG_WARN("failed to get value of left expression. rc=%s", strrc(rc));
    return rc;
  }

  if (right_) {
    rc = right_->try_get_value(right_value);
    if (rc != RC::SUCCESS) {
      LOG_WARN("failed to get value of right expression. rc=%s", strrc(rc));
      return rc;
    }
  }

  return calc_value(left_value, right_value, value);
}

////////////////////////////////////////////////////////////////////////////////

UnboundAggregateExpr::UnboundAggregateExpr(const char *aggregate_name, Expression *child)
    : aggregate_name_(aggregate_name), child_(child)
{}

////////////////////////////////////////////////////////////////////////////////
AggregateExpr::AggregateExpr(Type type, Expression *child) : aggregate_type_(type), child_(child) {}

AggregateExpr::AggregateExpr(Type type, unique_ptr<Expression> child) : aggregate_type_(type), child_(std::move(child))
{}

RC AggregateExpr::get_column(Chunk &chunk, Column &column)
{
  RC rc = RC::SUCCESS;
  if (pos_ != -1) {
    column.reference(chunk.column(pos_));
  } else {
    rc = RC::INTERNAL;
  }
  return rc;
}

bool AggregateExpr::equal(const Expression &other) const
{
  if (this == &other) {
    return true;
  }
  if (other.type() != type()) {
    return false;
  }
  const AggregateExpr &other_aggr_expr = static_cast<const AggregateExpr &>(other);
  return aggregate_type_ == other_aggr_expr.aggregate_type() && child_->equal(*other_aggr_expr.child());
}

unique_ptr<Aggregator> AggregateExpr::create_aggregator() const
{
  unique_ptr<Aggregator> aggregator;
  switch (aggregate_type_) {
    case Type::SUM: {
      aggregator = make_unique<SumAggregator>();
      break;
    }
    case Type::MAX: {
      aggregator = make_unique<MaxAggregator>();
      break;
    }
    case Type::MIN: {
      aggregator = make_unique<MinAggregator>();
      break;
    }
    case Type::COUNT: {
      aggregator = make_unique<CountAggregator>();
      break;
    }
    case Type::AVG: {
      aggregator = make_unique<AvgAggregator>();
      break;
    }
    default: {
      ASSERT(false, "unsupported aggregate type");
      break;
    }
  }
  return aggregator;
}

RC AggregateExpr::get_value(const Tuple &tuple, Value &value) const
{
  return tuple.find_cell(TupleCellSpec(name()), value);
}

RC AggregateExpr::type_from_string(const char *type_str, AggregateExpr::Type &type)
{
  RC rc = RC::SUCCESS;
  if (0 == strcasecmp(type_str, "count")) {
    type = Type::COUNT;
  } else if (0 == strcasecmp(type_str, "sum")) {
    type = Type::SUM;
  } else if (0 == strcasecmp(type_str, "avg")) {
    type = Type::AVG;
  } else if (0 == strcasecmp(type_str, "max")) {
    type = Type::MAX;
  } else if (0 == strcasecmp(type_str, "min")) {
    type = Type::MIN;
  } else {
    rc = RC::INVALID_ARGUMENT;
  }
  return rc;
}


RC FunctionExpr::type_from_string(const char *function_name, FunctionExpr::Type &type) {
    RC rc = RC::SUCCESS;
    if (0 == strcasecmp(function_name, "length")) {
      type = FunctionExpr::Type::LENGTH;
    }
    else if (0 == strcasecmp(function_name, "round")) {
      type = FunctionExpr::Type::ROUND;
    }
    else if (0 == strcasecmp(function_name, "date_format")) {
      type = FunctionExpr::Type::DATE_FORMAT;
    }
    else if (0 == strcasecmp(function_name, "l2_distance")) {
      type = FunctionExpr::Type::L2_DISTANCE;
    }
    else if (0 == strcasecmp(function_name, "cosine_distance")) {
      type = FunctionExpr::Type::COSINE_DISTANCE;
    }
    else if (0 == strcasecmp(function_name, "inner_product")) {
      type = FunctionExpr::Type::INNER_PRODUCT;
    }
    else {
      rc = RC::INVALID_ARGUMENT;
    }
    return rc;
  }

RC FunctionExpr::get_value(const Tuple &tuple, Value &value) const{
  RC rc = RC::SUCCESS;
  Value tmp_value;
  Value result;

  if ((rc = child_[0]->get_value(tuple, tmp_value)) == RC::SUCCESS) {
  }
  else {
    rc = child_[0]->try_get_value(tmp_value);
  }
  if (OB_FAIL(rc)) {
    return rc;
  }
  vector<Value> args;
  args.emplace_back(tmp_value);
  if (child_.size() > 1) {
    for (int i = 1; i < child_.size(); i++) {
      rc = child_[i]->get_value(tuple,tmp_value);
      if (OB_FAIL(rc)) {
        return rc;
      }
      args.emplace_back(tmp_value);
    }
  }

  rc = execute(args, result);
  if (OB_FAIL(rc)) {
    LOG_WARN("Fail to execut function.");
    return rc;
  }
  value = result;

  return rc;
}

RC FunctionExpr::try_get_value(Value &value) const {
  RC rc = RC::SUCCESS;
  Value tmp_value;
  Value result;

  
  rc = child_[0]->try_get_value(tmp_value);
  if (OB_FAIL(rc)) {
    return rc;
  }
  vector<Value> args;
  args.emplace_back(tmp_value);
  if (child_.size() > 1) {
    for (int i = 1; i < child_.size(); i++) {
      rc = child_[i]->try_get_value(tmp_value);
      if (OB_FAIL(rc)) {
        return rc;
      }
      args.emplace_back(tmp_value);
    }
  }

  rc = execute(args, result);
  if (OB_FAIL(rc)) {
    LOG_WARN("Fail to execut function.");
    return rc;
  }
  value = result;

  return rc;  
}

RC FunctionExpr::execute(const vector<Value> &values, Value &value) const {
  // Value res;
  RC rc = RC::SUCCESS;

  switch (type_) {
    case Type::LENGTH: {
      if (values.size() != 1 || values[0].attr_type() != AttrType::CHARS) {
        rc = RC::ERROR;
      }
      else {
        std::string s = values[0].get_string();
        int s_len = s.length();
        value = Value(s_len);
      }
    } break;
    case Type::ROUND: {
      float v = values[0].get_float();
      int prec = 0;
      float ans = 0.0;
      if (values.size() > 1) {
        prec = values[1].get_int();
      }

      if (prec < 0) {
        float factor = std::pow(10, -prec);
        ans = std::round(v / factor) * factor;
      } else {
        float factor = std::pow(10, prec);
        ans = std::round(v * factor) / factor;
      }
      value = Value(ans);
    } break;
    case Type::DATE_FORMAT: {
      if (values.size() != 2) {
        LOG_ERROR("Invalid function argument.");
        rc = RC::ERROR;
      }
      if (values[0].attr_type() != AttrType::DATES || values[1].attr_type() != AttrType::CHARS) {
        LOG_ERROR("Invalid function argument.");
        rc = RC::ERROR;
      }
      value = values[0];
      value.set_date_format(values[1].get_string());
    } break;
    case Type::L2_DISTANCE:
    case Type::COSINE_DISTANCE:
    case Type::INNER_PRODUCT: 
    {
      if (values.size() != 2) {
        rc = RC::ERROR;
      }
      else {
        rc = execute_vec(values[0], values[1], value);
      }
    } break;
    default:
      rc = RC::ERROR;
      break;
  }
  // value = res;
  return rc;
}

RC FunctionExpr::execute_vec(const Value &left, const Value &right, Value &value) const {
  RC rc = RC::SUCCESS;
  if (left.attr_type() != AttrType::VECTORS || right.attr_type() != AttrType::VECTORS) {
    rc = RC::ERROR;
    return rc;
  }

  vector<float> v1;
  vector<float> v2;
  for (int i = 0; i < left.length() / 4; i++) {
    v1.push_back(left.get_vectors()[i]);
  }
  for (int i = 0 ; i < right.length() / 4; i++) {
    v2.push_back(right.get_vectors()[i]);
  }
  
  if (v1.size() != v2.size()) {
    rc = RC::ERROR;
    return rc;
  }

  float f_result = 0.0;

  switch (type_) {
    case Type::L2_DISTANCE: {
      for (int i = 0; i < v1.size(); i++) {
        f_result += (v1[i] - v2[i]) * (v1[i] - v2[i]);
      }
      f_result = sqrtf(f_result);
    } break;
    case Type::COSINE_DISTANCE: {
      float inner_p = 0.0;
      float mod_1 = 0.0;
      float mod_2 = 0.0;
      for (int i = 0; i < v1.size(); i++) {
        inner_p += v1[i] * v2[i];
        mod_1 += v1[i] * v1[i];
        mod_2 += v2[i] * v2[i];
      }
      mod_1 = sqrtf(mod_1);
      mod_2 = sqrtf(mod_2);
      f_result =1 - (inner_p / (mod_1 * mod_2));
    } break;
    case Type::INNER_PRODUCT: {
      for (int i = 0; i < v1.size(); i++) {
        f_result += v1[i] * v2[i];
      }
    } break;
    default:
      rc = RC::ERROR;
      break;
  }
  f_result = round(f_result * 100) / 100;
  value = Value(f_result);
  value.set_type(AttrType::FLOATS);
  return rc;
}