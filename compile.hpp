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

enum class scope_element : int {
  global = -1,
  args = -2,
  body = -3,
  second_body = -4,
  type = -5,
  return_type = -6,
  arr_index = -7,
  arr_array = -8,
  argtype = -9,
  out_length = -10,
};

enum class token_type {
  string,
  identifier,
  number,
  bracket,
  semi,
  sep,
  sym
};

enum class act_type {
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
  const_int,
  oper,
  expr,
  arrset,
  arrget,
  outtype
};

class entry_bracket {
  public:
    char first;
    char second;
    entry_bracket(char f, char s){
      first = f;
      second = s;
    }
    entry_bracket(){
      first = 0;
      second = 0;
    }
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
  public:
  virtual ~AST() {}
};

class AST_node {
 public:
  act_type act;
};

class ast_body : virtual public AST {
 public:
  std::vector<AST*> body;
};

namespace ast_types {

#pragma region
class string_t : virtual public AST {
 public:
  std::string value;
  string_t(){
    value = "";
  }
  string_t(std::string v){
    value = v;
  }
  string_t(char v){
    value = std::string(1, v);
  }
};
class char_t : virtual public AST {
 public:
  char value;
  char_t(){
    value = '\0';
  };
  char_t(char v){
    value = v;
  }
};
class number_t : virtual public AST {
 public:
  int value;
  number_t(){
    value = 0;
  }
  number_t(int v){
    value = v;
  }
};
#pragma endregion standard_types_as_ast

#pragma region
class with_args : virtual public AST {
 public:
  ast_body args;
};
class with_body : virtual public AST {
 public:
  ast_body body;
};
class with_second_body : virtual public AST {
 public:
  ast_body second_body;
};
class with_type : virtual public AST {
 public:
  ast_body type;
};
class with_return_type : virtual public AST {
 public:
  ast_body return_type;
};

class arg_with_type_t : public with_type, virtual public AST {
 public:
  string_t name;
  arg_with_type_t(){
    name = string_t("");
  }
};
class with_args_with_type : virtual public AST {
  public:
    ast_body args;
};
#pragma endregion ast_global_types


class global_scope : virtual public AST {
 public:
  ast_body global;
};

class statement : public with_args, public AST_node {
 public:
  string_t name;
  statement() {
    act = act_type::statement;
  }
};
class statement_with_body : public with_body, public with_args, public AST_node {
 public:
  string_t name;
  statement_with_body() {
    act = act_type::statement_with_body;
  }
};
class statement_with_two_bodies : public with_body, public with_second_body, public with_args, public AST_node {
 public:
  string_t name;
  statement_with_two_bodies() {
    act = act_type::statement_with_two_bodies;
  }
};

class varop : virtual public AST, public AST_node {
 public:
  string_t name;
  string_t var;
  varop() {
    act = act_type::varop;
  }
};

class vardef : public with_args, public with_type, public AST_node {
 public:
  string_t name;
  vardef() {
    act = act_type::vardef;
  }
};

class in_type : public AST_node, public AST {
 public:
  string_t type;
  in_type() {
    act = act_type::type;
  }
};

class out_type : public with_type, public AST_node {
 public:
  string_t name;
  ast_body length;
  out_type() {
    act = act_type::outtype;
  }
};

class extdef : public with_args_with_type, public with_return_type, public AST_node {
 public:
  string_t name;
  extdef() {
    act = act_type::extdef;
  }
};

class fundef : public with_args_with_type, public with_body, public with_return_type, public AST_node {
 public:
  string_t name;
  fundef() {
    act = act_type::fundef;
  }
};

class glbdef : public with_args, public with_type, public AST_node {
 public:
  string_t name;
  glbdef() {
    act = act_type::glbdef;
  }
};

class call : public with_args, public AST_node {
 public:
  string_t name;
  call() {
    act = act_type::call;
  }
};

class varset : public with_args, public AST_node {
 public:
  string_t name;
  varset() {
    act = act_type::varset;
  }
};

class getvar : virtual public AST, public AST_node {
 public:
  string_t name;
  getvar() {
    act = act_type::getvar;
  }
};

class const_str : virtual public AST, public AST_node {
 public:
  string_t value;
  const_str() {
    act = act_type::const_str;
  }
};

class const_int : virtual public AST, public AST_node {
 public:
  number_t value;
  const_int() {
    act = act_type::const_int;
  }
};

class oper : public with_args, public AST_node {
 public:
  string_t op;
  oper() {
    act = act_type::oper;
  }
};

class expr : public with_args, public AST_node {
 public:
  expr() {
    act = act_type::expr;
  }
};

class arrset : public with_args, public AST_node {
  public:
  arrset() {
    act = act_type::arrset;
  }
};

class arrget : virtual public AST, public AST_node {
 public:
  ast_body index;
  ast_body array;
  arrget() {
    act = act_type::arrget;
  }
};
}  // namespace ast_types