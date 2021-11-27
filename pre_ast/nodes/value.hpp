#pragma once
#include "../with/type.hpp"
#include "../with/args.hpp"
#include "../types/string_t.hpp"

namespace ast_types {
class value : public with_type, public with_args, public AST_node {
 public:
  string_t name;
  value();
  std::string print_node() override;
};
}  // namespace ast_types