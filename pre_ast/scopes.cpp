#include "scopes.hpp"



token::token(token_type t, std::string v, int r, int c, int ch) {
  type = t;
  value = v;
  row = r;
  col = c;
  chr = ch;
}