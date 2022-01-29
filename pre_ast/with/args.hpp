#pragma once
#include "../ast.hpp"

namespace ast_types {
class with_args : virtual public AST {
 public:
  ast_body args;
};
}  // namespace ast_types