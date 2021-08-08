#include "compile.hpp"

#include <cstdlib>
#include <fstream>
#include <iostream>
#include <vector>

bool check_id_constraints(std::string id, char c) {
  bool retval = isalpha(c) || c == '_';
  if (!id.empty()) retval |= isdigit(c);
  return retval;
}
std::vector<token> tokenize_program(char *program, int length) {
  class {
   public:
    std::vector<token> tokens;
    std::string full_token = "";
    bool number = false;
    struct {
      bool going = false;
      bool escaping = false;
    } string;
    struct {
      bool going = false;
      std::string id = "";
    } identifier;
    bool symbol = false;
    bool comment = false;
    struct {
      bool going = false;
      bool escaping = false;
    } chr;

    void reset() {
      full_token = "";
      number = false;
      string.going = false;
      string.escaping = false;
      identifier.going = false;
      identifier.id = "";
      symbol = false;
      comment = false;
      chr.going = false;
      chr.escaping = false;
    }

    void push(token_type t) {
      // std::cout << "Pushing " << t << " " << full_token << std::endl;
      tokens.push_back(token(t, full_token));
    }
  } status;

  status.full_token = "{";
  status.push(token_type::bracket);
  status.reset();

  // traverse through the file char by char
  for (int i = 0; i < length; i++) {
    // std::cout << i << " " << std::endl;

    char c = i[program];

    auto escape_chr = [&]() {
      if (c == '\\') {
        status.full_token += '\\';
      } else if (c == '"') {
        status.full_token += c;
      } else if (c == 'n') {
        status.full_token += '\n';
      } else if (c == 't') {
        status.full_token += '\t';
      } else if (c == 'r') {
        status.full_token += '\r';
      } else {
        std::cout << "Invalid escape character: \\" << c << std::endl;  // we can maybe do something more useful here like appending the character by itself but who cares im in charge now
        exit(-1);                                                       // it should be safe to use exit here - https://stackoverflow.com/questions/30250934/how-to-end-c-code
      }
    };

    if (status.comment) {
      if (c == '\n') status.comment = false;
      continue;
    }

    std::string old_full_token = status.full_token;  // set old full token here to check for symbols later

    if (status.string.going) {
      if (status.string.escaping) {
        escape_chr();
        status.string.escaping = false;
      } else {
        if (c == '"') {
          status.push(token_type::string);
          status.reset();
        } else if (c == '\\') {
          status.string.escaping = true;
        } else {
          status.full_token += c;
        }
      }
    } else if (status.chr.going) {
      if (status.chr.escaping) {
        escape_chr();
        status.chr.escaping = false;
      } else {
        if (c == '\'') {
          if (status.full_token.length() != 1) {
            std::cout << "Invalid character literal size: " << status.full_token.length() << std::endl;
            exit(-1);  // next time throw an exception lmao
          } else {
            status.full_token = std::to_string(status.full_token[0]);
            status.push(token_type::number);
            status.reset();
          }
        } else if (c == '\\') {
          status.chr.escaping = true;
        } else {
          status.full_token += c;
        }
      }
    } else if (check_id_constraints(status.identifier.id, c)) {
      if (status.symbol) {
        status.push(token_type::sym);
        status.reset();
      }
      status.identifier.going = true;
      status.identifier.id += c;
      status.full_token += c;
    } else {
      if (status.identifier.going) {
        status.push(token_type::identifier);
        status.reset();
      }

      if (isdigit(c)) {
        if (status.symbol) {
          status.push(token_type::sym);
          status.reset();
        }
        status.number = true;
        status.full_token += c;
      } else {
        if (c == '.' && status.number) {
          status.full_token += c;
        } else {
          if (status.number) {
            status.push(token_type::number);
            status.reset();
          }
          if (c == '"') {
            status.string.going = true;
          } else if (c == '\'') {
            status.chr.going = true;
          } else if (std::string("()[]{}").find(c) != std::string::npos) {
            status.full_token += c;
            status.push(token_type::bracket);
            status.reset();
          } else if (c == ';') {
            status.full_token += c;
            status.push(token_type::semi);
            status.reset();
          } else if (c == ',') {
            status.full_token += c;
            status.push(token_type::sep);
            status.reset();
          } else if (c == '#') {
            status.reset();
            status.comment = true;
          } else if (!isspace(c)) {
            status.symbol = true;
            status.full_token += c;
          } else if (status.symbol) {
            status.push(token_type::sym);
            status.reset();
          }
        }
      }
    }
  }

  status.full_token = "}";
  status.push(token_type::bracket);
  status.reset();

#ifdef DEBUG
  for (auto &t : status.tokens) {
    std::cout << "Token: " << t.value << "\t\t\tType: " << DEBUG_TOKEN_TYPES[t.type] << std::endl;
  }
#endif

return status.tokens;
}

int main() {
  std::ifstream is("tests/vartest.crn");

  if (!is) {
    std::cout << "File not found" << std::endl;
    return -1;
  }
  is.seekg(0, is.end);
  int is_length = is.tellg();
  is.seekg(0, is.beg);
  char *file_buffer = new char[is_length];
  int f_iter = 0;
  while (!is.eof() && f_iter < is_length) {
    is.get(file_buffer[f_iter]);  //reading single character from file to array
    f_iter++;
  }
  is.close();

  std::vector<token> program_tokens = tokenize_program(file_buffer, is_length);
  delete[] file_buffer;

  return EXIT_SUCCESS;
}