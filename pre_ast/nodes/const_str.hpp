#pragma once
#include "../types/string_t.hpp"

namespace ast_types {
class const_str : virtual public AST, public AST_node {
 public:
  string_t value;
  const_str();
  std::string print_node() override;
};
}  // namespace ast_types