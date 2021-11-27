#pragma once
#include "../with/values.hpp"
#include "../types/string_t.hpp"

namespace ast_types {
class class_n : public with_values, public AST_node {
 public:
  string_t name;
  class_n();
  std::string print_node() override;
};
}  // namespace ast_types