#pragma once
#include "../ast.hpp"

namespace ast_types {
class decimal_t : virtual public AST {
 public:
  float value;
  decimal_t(float v = 0.0);
  std::string print_node() override;
};
}  // namespace ast_types