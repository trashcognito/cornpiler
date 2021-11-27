#include "class.hpp"

ast_types::class_n::class_n() { act = act_type::class_n; }
std::string ast_types::class_n::print_node() {
  return "\"class\": {\"name\":" + this->name.print_node() +
         ",\"values\": " + this->values.print_node() + "}";
}