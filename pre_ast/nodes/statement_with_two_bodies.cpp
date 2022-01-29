#include "statement_with_two_bodies.hpp"

ast_types::statement_with_two_bodies::statement_with_two_bodies() { act = act_type::statement_with_two_bodies; }
std::string ast_types::statement_with_two_bodies::print_node() {
  return "\"statement_with_two_bodies\": {\"name\":" + this->name.print_node() +
         ",\"args\": " + this->args.print_node() +
         ",\"body1\": " + this->body.print_node() +
         ",\"body2\": " + this->second_body.print_node() + "}";
}