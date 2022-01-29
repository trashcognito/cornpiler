#include "scope.hpp"

ast_types::scope::scope() { act = act_type::scope; }
std::string ast_types::scope::print_node() {
  return "\"scope\": {\"body\":" + this->body.print_node() + "}";
}