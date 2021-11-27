#include "fun_type.hpp"

ast_types::fun_type::fun_type() { act = act_type::fun_type; }
std::string ast_types::fun_type::print_node() {
  return "\"fun_type\": ,\"args\": " + this->args.print_node() +
         ",\"return_type\": " + this->return_type.print_node() + "}";
}