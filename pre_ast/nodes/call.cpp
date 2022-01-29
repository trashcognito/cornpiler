#include "call.hpp"

ast_types::call::call() { act = act_type::call; }
std::string ast_types::call::print_node() {
  return "\"call\": {\"name\":" + this->name.print_node() +
         ",\"args\": " + this->args.print_node() + "}";
}