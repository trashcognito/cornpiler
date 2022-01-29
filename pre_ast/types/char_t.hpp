#pragma once
#include "../ast.hpp"

namespace ast_types {
class char_t : virtual public AST {
 public:
  char value;
  char_t(char v = '\0');
  std::string print_node() override;
};
}  // namespace ast_types