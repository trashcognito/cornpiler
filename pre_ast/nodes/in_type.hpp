#pragma once
#include "../ast.hpp"
#include "../types/string_t.hpp"

namespace ast_types {
class in_type : public AST_node, public AST {
 public:
  string_t type;
  in_type();
  std::string print_node() override;
};
}  // namespace ast_types