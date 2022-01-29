#pragma once
#include "../ast.hpp"

namespace ast_types {
class with_args_with_type : virtual public AST {
 public:
  ast_body args;
};
}  // namespace ast_types