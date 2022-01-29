#include "extdef.hpp"

ast_types::extdef::extdef() { act = act_type::extdef; }
std::string ast_types::extdef::print_node() {
  return "\"extdef\": {\"name\":" + this->name.print_node() +
         ",\"args\": " + this->args.print_node() +
         ",\"return_type\": " + this->return_type.print_node() + "}";
}