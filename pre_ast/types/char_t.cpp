#include "char_t.hpp"

ast_types::char_t::char_t(char v) { value = v; }
std::string ast_types::char_t::print_node() { return "\"" + std::string(1, value) + "\""; }