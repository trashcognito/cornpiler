#pragma once
#include "../types/string_t.hpp"

namespace ast_types {
class getvar : virtual public AST, public AST_node {
 public:
  string_t name;
  getvar();
  std::string print_node() override;
};
}  // namespace ast_types