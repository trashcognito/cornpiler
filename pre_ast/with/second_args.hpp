#pragma once
#include "../ast.hpp"

namespace ast_types {
class with_second_args : virtual public AST {
 public:
  ast_body second_args;
};
}  // namespace ast_types