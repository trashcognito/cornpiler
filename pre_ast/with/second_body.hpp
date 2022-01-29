#pragma once
#include "../ast.hpp"

namespace ast_types {
class with_second_body : virtual public AST {
 public:
  ast_body second_body;
};
}  // namespace ast_types