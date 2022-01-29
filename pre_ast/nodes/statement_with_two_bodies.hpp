#pragma once
#include "../with/args.hpp"
#include "../with/body.hpp"
#include "../with/second_body.hpp"
#include "../types/string_t.hpp"

namespace ast_types {
class statement_with_two_bodies : public with_body,
                                  public with_second_body,
                                  public with_args,
                                  public AST_node {
 public:
  string_t name;
  statement_with_two_bodies();
  std::string print_node() override;
};

}  // namespace ast_types