#include "callptr.hpp"

ast_types::callptr::callptr() { act = act_type::callptr; }
std::string ast_types::callptr::print_node() {
  return "\"callptr\": {\"args\":" + this->args.print_node() +
         ",\"second_args\": " + this->second_args.print_node() + "}";
}