#include "const_decimal.hpp"

ast_types::const_decimal::const_decimal() { act = act_type::const_decimal; }
std::string ast_types::const_decimal::print_node() {
  return "\"const_decimal\": {\"value\":" + this->value.print_node() + "}";
}