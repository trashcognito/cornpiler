#include "classget.hpp"

ast_types::classget::classget() { act = act_type::classget; }
std::string ast_types::classget::print_node() {
  return "\"classget\": {\"class\":" + this->args.print_node() +
         ",\"var\": " + this->var.print_node() + "}";
}