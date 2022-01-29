#include "out_type.hpp"

ast_types::out_type::out_type() { act = act_type::outtype; }
std::string ast_types::out_type::print_node() {
  return "\"out_type\": {\"type\":" + this->name.print_node() + ",\"inner_type\":" + this->type.print_node() + ",\"length\":" + this->length.print_node() + "}";
}