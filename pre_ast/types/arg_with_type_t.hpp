#pragma once
#include "string_t.hpp"
#include "../with/type.hpp"

namespace ast_types {
class arg_with_type_t : public with_type, virtual public AST {
 public:
  string_t name;
  arg_with_type_t();
  std::string print_node() override;
};
}  // namespace ast_types