#pragma once
#include "../with/args.hpp"
#include "../types/string_t.hpp"

namespace ast_types {
class classget : public with_args, public AST_node {
 public:
  string_t var;
  classget();
  std::string print_node() override;
};
}  // namespace ast_types