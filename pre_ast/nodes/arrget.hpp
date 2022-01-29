#pragma once
#include "../ast.hpp"

namespace ast_types {
class arrget : virtual public AST, public AST_node {
 public:
  ast_body index;
  ast_body array;
  arrget();
  std::string print_node() override;
};
}  // namespace ast_types