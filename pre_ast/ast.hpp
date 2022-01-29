#pragma once

#include <string>
#include <vector>
#include "scopes.hpp"

class AST {
 public:
  virtual ~AST() = 0;
  virtual std::string print_node();
};

class AST_node {
 public:
  act_type act;
  std::string print_node();
};

class ast_body : virtual public AST {
 public:
  std::vector<AST *> body;
  std::string print_node() override;
};