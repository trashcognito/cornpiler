#pragma once
#include "../types/string_t.hpp"
#include "../with/args.hpp"

namespace ast_types {
class oper : public with_args, public AST_node {
 public:
  string_t op;
  oper();
  std::string print_node() override;
};
}  // namespace ast_types