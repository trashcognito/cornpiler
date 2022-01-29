#pragma once
#include "../types/number_t.hpp"

namespace ast_types {
class const_int : virtual public AST, public AST_node {
 public:
  number_t value;
  const_int();
  std::string print_node() override;
};
}  // namespace ast_types