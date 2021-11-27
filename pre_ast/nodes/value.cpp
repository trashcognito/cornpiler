#include "value.hpp"

ast_types::value::value() { act = act_type::value; }
std::string ast_types::value::print_node() {
  return "\"value\": {\"name\":" + this->name.print_node() +
         ",\"type\": " + this->type.print_node() + 
         ",\"args\": " + this->args.print_node() + "}";
}