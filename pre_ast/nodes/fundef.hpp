#pragma once
#include "../with/return_type.hpp"
#include "../with/args_with_type.hpp"
#include "../with/body.hpp"
#include "../types/string_t.hpp"

namespace ast_types {
class fundef : public with_args_with_type,
               public with_body,
               public with_return_type,
               public AST_node {
 public:
  string_t name;
  fundef();
  std::string print_node() override;
};
}  // namespace ast_types