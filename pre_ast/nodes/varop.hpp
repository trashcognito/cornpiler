#pragma once
#include "../ast.hpp"
#include "../types/string_t.hpp"

namespace ast_types {
class varop : virtual public AST, public AST_node {
 public:
  string_t name;
  string_t var;
  varop();
  std::string print_node() override;
};
}  // namespace ast_types