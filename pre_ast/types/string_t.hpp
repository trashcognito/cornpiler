#pragma once
#include "../ast.hpp"

namespace ast_types {
class string_t : virtual public AST {
 public:
  std::string value;
  string_t(std::string v = "");
  string_t(char v);
  std::string print_node() override;
};
}  // namespace ast_types