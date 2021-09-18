#include "in_type.hpp"

ast_types::in_type::in_type() { act = act_type::type; }
std::string ast_types::in_type::print_node() {
  return "\"in_type\": {\"type\":" + this->type.print_node() + "}";
}