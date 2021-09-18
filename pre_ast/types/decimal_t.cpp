#include "decimal_t.hpp"

ast_types::decimal_t::decimal_t(float v) { value = v; }
std::string ast_types::decimal_t::print_node() { return std::to_string(value); }