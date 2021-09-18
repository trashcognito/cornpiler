#include "varset.hpp"

ast_types::varset::varset() { act = act_type::varset; }
std::string ast_types::varset::print_node() {
  return "\"varset\": {\"name\":" + this->name.print_node() +
         ",\"args\": " + this->args.print_node() + "}";
}