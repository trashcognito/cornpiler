#include "number_t.hpp"

ast_types::number_t::number_t(int v) { value = v; }
std::string ast_types::number_t::print_node() { return std::to_string(value); }