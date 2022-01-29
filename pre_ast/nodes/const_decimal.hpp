#pragma once
#include "../types/decimal_t.hpp"

namespace ast_types {
class const_decimal : virtual public AST, public AST_node {
 public:
  decimal_t value;
  const_decimal();
  std::string print_node() override;
};
}  // namespace ast_types