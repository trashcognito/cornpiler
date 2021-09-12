#pragma once

#include <cstdint>
#include <iostream>
#include <string>
#include <variant>
#include <vector>

class file_object {
public:
  char *contents;
  int length;

  file_object(char *c, int l) : contents(c), length(l) {}
};

bool check_id_constraints(std::string id, char c);

namespace logger {
enum class LOG_LEVEL {
  NONE = 0b10000,
  ERROR = 0b01000,
  WARNING = 0b00100,
  INFO = 0b00010,
  DEBUG = 0b00001,
};
enum class SETTINGS {
  NEWLINE = 0b0001,
  TYPE = 0b0010,
  NONE = 0b0,
};

LOG_LEVEL operator|(LOG_LEVEL lhs, LOG_LEVEL rhs);
LOG_LEVEL operator&(LOG_LEVEL lhs, LOG_LEVEL rhs);
SETTINGS operator|(SETTINGS lhs, SETTINGS rhs);
SETTINGS operator&(SETTINGS lhs, SETTINGS rhs);

class logger {
public:
  LOG_LEVEL level;
  std::string log_level_text(LOG_LEVEL lvl);
  void log(LOG_LEVEL level, std::string msg,
           SETTINGS settings = SETTINGS::NEWLINE | SETTINGS::TYPE);
  logger(LOG_LEVEL log);
};
}; // namespace logger

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

enum class token_type { string, identifier, number, bracket, semi, sep, sym };

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
  outtype,
  global,
};

class entry_bracket {
public:
  char first;
  char second;
  entry_bracket(char f = 0, char s = 0);
};

class token {
public:
  token_type type;
  std::string value;

  int row;
  int col;

  int chr;

  token(token_type t, std::string v, int r, int c, int ch);
};

std::vector<token> tokenize_program(std::string program, int length);

class AST {
public:
  virtual ~AST() = 0;
};

class AST_node {
public:
  act_type act;
};

class ast_body : virtual public AST {
public:
  std::vector<AST *> body;
};

namespace ast_types {

class string_t : virtual public AST {
public:
  std::string value;
  string_t(std::string v = "");
  string_t(char v);
};
class char_t : virtual public AST {
public:
  char value;
  char_t(char v = '\0');
};
class number_t : virtual public AST {
public:
  int value;
  number_t(int v = 0);
};

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
  arg_with_type_t();
};
class with_args_with_type : virtual public AST {
public:
  ast_body args;
};

class global_scope : public with_body, public AST_node {
public:
global_scope();
};

class statement : public with_args, public AST_node {
public:
  string_t name;
  statement();
};
class statement_with_body : public with_body,
                            public with_args,
                            public AST_node {
public:
  string_t name;
  statement_with_body();
};
class statement_with_two_bodies : public with_body,
                                  public with_second_body,
                                  public with_args,
                                  public AST_node {
public:
  string_t name;
  statement_with_two_bodies();
};

class varop : virtual public AST, public AST_node {
public:
  string_t name;
  string_t var;
  varop();
};

class vardef : public with_args, public with_type, public AST_node {
public:
  string_t name;
  vardef();
};

class in_type : public AST_node, public AST {
public:
  string_t type;
  in_type();
};

class out_type : public with_type, public AST_node {
public:
  string_t name;
  ast_body length;
  out_type();
};

class extdef : public with_args_with_type,
               public with_return_type,
               public AST_node {
public:
  string_t name;
  extdef();
};

class fundef : public with_args_with_type,
               public with_body,
               public with_return_type,
               public AST_node {
public:
  string_t name;
  fundef();
};

class glbdef : public with_args, public with_type, public AST_node {
public:
  string_t name;
  glbdef();
};

class call : public with_args, public AST_node {
public:
  string_t name;
  call();
};

class varset : public with_args, public AST_node {
public:
  string_t name;
  varset();
};

class getvar : virtual public AST, public AST_node {
public:
  string_t name;
  getvar();
};

class const_str : virtual public AST, public AST_node {
public:
  string_t value;
  const_str();
};

class const_int : virtual public AST, public AST_node {
public:
  number_t value;
  const_int();
};

class oper : public with_args, public AST_node {
public:
  string_t op;
  oper();
};

class expr : public with_args, public AST_node {
public:
  expr();
};

class arrset : public with_args, public AST_node {
public:
  arrset();
};

class arrget : virtual public AST, public AST_node {
public:
  ast_body index;
  ast_body array;
  arrget();
};
} // namespace ast_types

file_object read_file(const char *filename, logger::logger *logger);
bool check_id_constraints(std::string id, char c);
std::vector<token> tokenize_program(char *program, int length,
                                    logger::logger *logger);
ast_types::global_scope lex_program(file_object input_file,
                                    std::vector<token> program_tokens,
                                    logger::logger *logger);