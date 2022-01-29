#include "string_t.hpp"

ast_types::string_t::string_t(std::string v) { value = v; }
ast_types::string_t::string_t(char v) { value = std::string(1, v); }
std::string ast_types::string_t::print_node() { return "\"" + value + "\""; }