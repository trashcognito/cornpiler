#include "arg_with_type_t.hpp"

ast_types::arg_with_type_t::arg_with_type_t() { name = string_t(""); }
std::string ast_types::arg_with_type_t::print_node() {
  return "\"arg_with_type\": {\"type\":" + this->type.print_node() +
         ",\"name\":" + this->name.print_node() + "}";
}