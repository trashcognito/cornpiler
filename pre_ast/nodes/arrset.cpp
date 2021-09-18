#include "arrset.hpp"

ast_types::arrset::arrset() { act = act_type::arrset; }
std::string ast_types::arrset::print_node() {
  return "\"arrset\": {\"args\": " + this->args.print_node() + "}";
}