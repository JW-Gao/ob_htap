#pragma once

#include <memory>
#include <string>
#include <vector>

#include "common/value.h"
#include "sql/expr/composite_tuple.h"
#include "sql/expr/expression_tuple.h"
#include "sql/expr/tuple.h"
template <typename ExprPointerType>
inline void get_row_tuples(Tuple *tuple, std::vector<RowTuple *> &tuples)
{
  switch (tuple->tuple_type()) {
    case TupleType::ROW: {
      tuples.push_back(static_cast<RowTuple *>(tuple));

    } break;
    case TupleType::JOIN: {
      get_row_tuples<ExprPointerType>(static_cast<JoinedTuple *>(tuple)->left(), tuples);
      get_row_tuples<ExprPointerType>(static_cast<JoinedTuple *>(tuple)->right(), tuples);
    } break;
    case TupleType::PROJECT: {
      get_row_tuples<ExprPointerType>(static_cast<ProjectTuple *>(tuple)->child_tuple(), tuples);
    } break;
    case TupleType::EXPRESSION: {
      get_row_tuples<ExprPointerType>(static_cast<ExpressionTuple<ExprPointerType> *>(tuple)->child_tuple(), tuples);
    } break;
    case TupleType::COMPOSITE: {
      for (auto &it : static_cast<CompositeTuple *>(tuple)->tuples()) {
        get_row_tuples<ExprPointerType>(it.get(), tuples);
      }

    } break;
    default: {
    }
  }
}