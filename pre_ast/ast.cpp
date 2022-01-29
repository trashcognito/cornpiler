#include "ast.hpp"

AST::~AST() {}
std::string AST::print_node() { return "WHY THE FUCK ARE YOU PRINTING ME DUMBASS"; }
std::string AST_node::print_node() { return "WHY THE FUCK ARE YOU PRINTING ME DUMBASS"; }
std::string ast_body::print_node() {
  std::string retval = "[";
  for (auto &i : this->body) {
    retval += ",{" + i->print_node() + "}";
  }
  if (retval.size() > 1) {
    retval.erase(1, 1);
  }

  retval += "]";
  return retval;
}