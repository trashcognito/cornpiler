#include "global.hpp"

ast_types::global_scope::global_scope() { act = act_type::global; }
std::string ast_types::global_scope::print_node() {
  return "{\"globals\":" + this->body.print_node() + "}";
}