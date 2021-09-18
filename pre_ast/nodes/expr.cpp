#include "expr.hpp"

ast_types::expr::expr() { act = act_type::expr; }
std::string ast_types::expr::print_node() {
  return "\"expr\": {\"args\": " + this->args.print_node() + "}";
}