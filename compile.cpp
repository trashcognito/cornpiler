#include "compile.hpp"

#include <fstream>
#include <iostream>
#include <vector>

bool check_id_constraints(std::string id, char c) {
  bool retval = isalpha(c) || c == '_';
  if (!id.empty()) retval |= isdigit(c);
  // std::cout << "Checking id " << id << " for " << c << ": " << (retval ? "valid" : "invalid") << std::endl;
  return retval;
}

int main() {
  std::ifstream is("tests/bottlesofbeer99.crn");

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

  class {
   public:
    std::vector<token> tokens;
    std::string full_token = "";
    bool number = false;
    struct{
      bool going = false;
      bool escaping = false;
    } string;
    struct{
      bool going = false;
      std::string id = "";
    } identifier;
    bool symbol = false;
    bool comment = false;

    void reset() {
      full_token = "";
      number = false;
      string.going = false;
      string.escaping = false;
      identifier.going = false;
      identifier.id = "";
      symbol = false;
      comment = false;
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
  for (int i = 0; i < is_length; i++) {
    // std::cout << i << " " << std::endl;

    char c = i[file_buffer];

    if (status.comment) {
      if (c == '\n') status.comment = false;
      continue;
    }

    std::string old_full_token = status.full_token;  // set old full token here to check for symbols later

    if (status.string.going) {
      if (status.string.escaping) {
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
          return -1;
        }
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
            if (status.symbol) {
              status.push(token_type::sym);
              status.reset();
            }
            status.push(token_type::number);
            status.reset();
          }
          if (c == '"') {
            if (status.symbol) {
              status.push(token_type::sym);
              status.reset();
            }
            status.string.going = true;
          } else if (std::string("()[]{}").find(c) != std::string::npos) {
            if (status.symbol) {
              status.push(token_type::sym);
              status.reset();
            }
            status.full_token += c;
            status.push(token_type::bracket);
            status.reset();
          } else if (c == ';') {
            if (status.symbol) {
              status.push(token_type::sym);
              status.reset();
            }
            status.full_token += c;
            status.push(token_type::semi);
            status.reset();
          } else if (c == ',') {
            if (status.symbol) {
              status.push(token_type::sym);
              status.reset();
            }
            status.full_token += c;
            status.push(token_type::sep);
            status.reset();
          } else if (c == '#') {
            if (status.symbol) {
              status.push(token_type::sym);
              status.reset();
            }
            status.reset();
            status.comment = true;
          } else if (!isspace(c)) {
            status.symbol = true;
            status.full_token += c;
          }
        }
      }
    }
  }

  status.full_token = "}";
  status.push(token_type::bracket);
  status.reset();

  for (auto &t : status.tokens) {
    std::cout << "Token: " << t.value << "\t\t\tType: " << DEBUG_TOKEN_TYPES[t.type] << std::endl;
  }

  return 0;
}