#include "oper.hpp"

ast_types::oper::oper() { act = act_type::oper; }
std::string ast_types::oper::print_node() {
  return "\"oper\": {\"op\":" + this->op.print_node() +
         ",\"args\": " + this->args.print_node() + "}";
}