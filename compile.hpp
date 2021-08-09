#pragma once

#include <cstdint>
#include <string>
#include <variant>
#include <vector>

#define DEBUG

bool check_id_constraints(std::string id, char c);

#ifdef DEBUG
std::string DEBUG_TOKEN_TYPES[] = {"str", "identifier", "number", "bracket", "semi", "sep", "sym"};
#endif

typedef std::variant<int, std::string> scope_element;

enum token_type {
  string,
  identifier,
  number,
  bracket,
  semi,
  sep,
  sym
};

enum act_type {
  error,
  statement,
  statement_with_body,
  statement_with_two_bodies,
  varop,
  vardef,
  type,
  fundef,
  extdef,
  glbdef,
  call,
  varset,
  getvar,
  const_str,
  const_number,
  oper,
  expr,
  arrset,
  arrget,
  outtype
};

class token {
 public:
  token_type type;
  std::string value;
  token(token_type t, std::string v) {  // god forgive me for writing a function inside a header file
    type = t;
    value = v;
  }
};

std::vector<token> tokenize_program(std::string program, int length);

class AST {
  act_type act;
};

class ast_body : public AST {
 public:
  std::vector<AST*> body;
};

namespace ast_types {

#pragma region
class with_args : public AST {
 public:
  ast_body args;
};
class with_body : public AST {
 public:
  ast_body body;
};
class with_second_body : public AST {
 public:
  ast_body second_body;
};
class with_type : public AST {
 public:
  ast_body type;
};
class with_return_type : public AST {
 public:
  ast_body return_type;
};
#pragma endregion ast_global_types

#pragma region
  class string_t : public AST {
    public:
      std::string value;
  };
  class char_t : public AST {
    public:
      char value;
  };
  class number_t : public AST {
    public:
      int value;
  };
#pragma endregion standard_types_as_ast

class global_scope : public AST {
 public:
  ast_body global;
};

class statement : public with_args {
 public:
  string_t name;
};
class statement_with_body : public statement, public with_body {
};
class statement_with_two_bodies : public statement_with_body, public with_second_body {
};

class varop : public AST {
 public:
  string_t name;
  string_t var;
};

class vardef : public with_args, public with_type {
 public:
  string_t name;
};

class type : public AST {
 public:
  string_t type;
};

class out_type : public with_type {
 public:
  string_t name;
};

class extdef : public with_args {
 public:
  string_t name;
};

class fundef : public with_args, public with_body, public with_return_type {
 public:
  string_t name;
};

class glbdef : vardef {
};

class call : public with_args {
 public:
  string_t name;
};

class varset : public with_args {
 public:
  string_t name;
};

class getvar : public AST {
 public:
  string_t name;
};

class const_str : public AST {
 public:
  string_t value;
};

class const_int : public AST {
 public:
  number_t value;
};

class oper : public with_args {
 public:
  char_t op;
};

class expr : public with_args {
};

class arrset : public with_args { // only 2 args
};

class arrget : public AST {
 public:
  AST index;
  AST array;
};
}  // namespace ast_types