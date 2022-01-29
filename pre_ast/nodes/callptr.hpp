#pragma once
#include "../with/args.hpp"
#include "../with/second_args.hpp"

namespace ast_types {
class callptr : public with_args, public with_second_args, public AST_node {
 public:
  callptr();
  std::string print_node() override;
};
}  // namespace ast_types