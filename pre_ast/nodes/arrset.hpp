#pragma once
#include "../with/args.hpp"

namespace ast_types {
class arrset : public with_args, public AST_node {
 public:
  arrset();
  std::string print_node() override;
};
}  // namespace ast_types