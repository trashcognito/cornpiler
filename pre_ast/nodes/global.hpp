#pragma once
#include "../with/body.hpp"

namespace ast_types {
class global_scope : public with_body, public AST_node {
 public:
  global_scope();
  std::string print_node() override;
};
}  // namespace ast_types