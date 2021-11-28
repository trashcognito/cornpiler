#include "ptrset.hpp"

ast_types::ptrset::ptrset() { act = act_type::ptrset; }
std::string ast_types::ptrset::print_node() {
  return "\"ptrset\": {\"args\":" + this->args.print_node() +
         ",\"second_args\": " + this->second_args.print_node() + "}";
}