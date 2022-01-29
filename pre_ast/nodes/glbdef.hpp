#pragma once
#include "../with/args.hpp"
#include "../with/type.hpp"
#include "../types/string_t.hpp"

namespace ast_types {
class glbdef : public with_args, public with_type, public AST_node {
 public:
  string_t name;
  glbdef();
  std::string print_node() override;
};
}  // namespace ast_types