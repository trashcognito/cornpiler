#pragma once
#include "../ast.hpp"

namespace ast_types {
class with_values : virtual public AST {
 public:
  ast_body values;
};
}  // namespace ast_types