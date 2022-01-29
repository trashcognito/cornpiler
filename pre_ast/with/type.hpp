#pragma once
#include "../ast.hpp"

namespace ast_types {
class with_type : virtual public AST {
 public:
  ast_body type;
};
}  // namespace ast_types