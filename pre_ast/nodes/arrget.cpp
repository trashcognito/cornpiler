#include "arrget.hpp"

ast_types::arrget::arrget() { act = act_type::arrget; }
std::string ast_types::arrget::print_node() {
  return "\"arrget\": {\"index\":" + this->index.print_node() +
         ",\"arr\": " + this->array.print_node() + "}";
}