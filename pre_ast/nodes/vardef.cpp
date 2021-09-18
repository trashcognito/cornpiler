#include "vardef.hpp"

ast_types::vardef::vardef() { act = act_type::vardef; }
std::string ast_types::vardef::print_node() {
  return "\"vardef\": {\"name\":" + this->name.print_node() +
         ",\"type\": " + this->type.print_node() +
         ",\"args\": " + this->args.print_node() + "}";
}