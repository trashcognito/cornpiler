#include "varop.hpp"
ast_types::varop::varop() { act = act_type::varop; }
std::string ast_types::varop::print_node() {
  return "\"varop\": {\"name\":" + this->name.print_node() +
         ",\"var\": " + this->var.print_node() + "}";
}