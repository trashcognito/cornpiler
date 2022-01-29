#include "glbdef.hpp"

ast_types::glbdef::glbdef() { act = act_type::glbdef; }
std::string ast_types::glbdef::print_node() {
  return "\"vardef\": {\"name\":" + this->name.print_node() +
         ",\"type\": " + this->type.print_node() +
         ",\"args\": " + this->args.print_node() + "}";
}