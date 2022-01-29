#include "const_int.hpp"

ast_types::const_int::const_int() { act = act_type::const_int; }
std::string ast_types::const_int::print_node() {
  return "\"const_int\": {\"value\":" + this->value.print_node() + "}";
}