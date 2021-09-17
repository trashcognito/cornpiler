#pragma once
#include "../with/args.hpp"
#include "../with/type.hpp"
#include "../types/string_t.hpp"

namespace ast_types {
class vardef : public with_args, public with_type, public AST_node {
 public:
  string_t name;
  vardef();
  std::string print_node() override;
};
}  // namespace ast_types