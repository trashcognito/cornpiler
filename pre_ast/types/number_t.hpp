#pragma once
#include "../ast.hpp"

namespace ast_types {
class number_t : virtual public AST {
 public:
  int value;
  number_t(int v = 0);
  std::string print_node() override;
};
}  // namespace ast_types