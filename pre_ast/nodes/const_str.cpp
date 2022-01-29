#include "const_str.hpp"

ast_types::const_str::const_str() { act = act_type::const_str; }
std::string ast_types::const_str::print_node() {
  return "\"const_str\": {\"value\":" + this->value.print_node() + "}";
}