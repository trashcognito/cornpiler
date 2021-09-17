#pragma once
#include "../with/args.hpp"

namespace ast_types {
class expr : public with_args, public AST_node {
 public:
  expr();
  std::string print_node() override;
};
}  // namespace ast_types