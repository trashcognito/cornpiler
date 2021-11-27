#pragma once
#include <string>

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
  decimal,
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
  const_decimal,
  oper,
  expr,
  arrset,
  arrget,
  outtype,
  global,
  scope,
};

enum class parsing_modes {
  error,
  statement,
  argument,
  arg_one,
  type,
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
