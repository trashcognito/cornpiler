#pragma once
#include "../with/type.hpp"
#include "../types/string_t.hpp"

namespace ast_types {
class out_type : public with_type, public AST_node {
 public:
  string_t name;
  ast_body length;
  out_type();
  std::string print_node() override;
};
}  // namespace ast_types