#pragma once
#include "../ast.hpp"

namespace ast_types {
class with_return_type : virtual public AST {
 public:
  ast_body return_type;
};
}  // namespace ast_types