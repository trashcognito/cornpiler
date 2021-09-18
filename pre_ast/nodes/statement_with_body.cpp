#include "statement_with_body.hpp"

ast_types::statement_with_body::statement_with_body() { act = act_type::statement_with_body; }
std::string ast_types::statement_with_body::print_node() {
  return "\"statement_with_body\": {\"name\":" + this->name.print_node() +
         ",\"args\": " + this->args.print_node() +
         ",\"body\": " + this->body.print_node() + "}";
}