#include "getvar.hpp"

ast_types::getvar::getvar() { act = act_type::getvar; }
std::string ast_types::getvar::print_node() {
  return "\"getvar\": {\"name\":" + this->name.print_node() + "}";
}