#pragma once
#include "../with/body.hpp"

namespace ast_types {
class scope : public with_body, public AST_node {
 public:
  scope();
  std::string print_node() override;
};
}  // namespace ast_types