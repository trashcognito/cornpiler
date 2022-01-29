#pragma once
#include "../with/return_type.hpp"
#include "../with/args_with_type.hpp"
#include "../types/string_t.hpp"

namespace ast_types {
class fun_type : public with_args_with_type,
               public with_return_type,
               public AST_node {
 public:
  fun_type();
  std::string print_node() override;
};
}  // namespace ast_types