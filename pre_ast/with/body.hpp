#pragma once
#include "../ast.hpp"

namespace ast_types {
class with_body : virtual public AST {
 public:
  ast_body body;
};
}  // namespace ast_types