#include "fundef.hpp"

ast_types::fundef::fundef() { act = act_type::fundef; }
std::string ast_types::fundef::print_node() {
  return "\"fundef\": {\"name\":" + this->name.print_node() +
         ",\"args\": " + this->args.print_node() +
         ",\"return_type\": " + this->return_type.print_node() +
         ",\"body\": " + this->body.print_node() + "}";
}