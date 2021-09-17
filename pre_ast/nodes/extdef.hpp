#pragma once
#include "../with/return_type.hpp"
#include "../with/args_with_type.hpp"
#include "../types/string_t.hpp"

namespace ast_types {
class extdef : public with_args_with_type,
               public with_return_type,
               public AST_node {
 public:
  string_t name;
  extdef();
  std::string print_node() override;
};
}  // namespace ast_types