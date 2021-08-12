#include "compile.hpp"

#include <cstdlib>
#include <fstream>
#include <functional>
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
    std::cout << "Token: " << t.value << "\t\t\tType: " << DEBUG_TOKEN_TYPES[(int)t.type] << std::endl;
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

  ast_types::global_scope globals;

  std::function<int(int, std::vector<scope_element>, int)> recursive_lex = [&](int old_itt, std::vector<scope_element> scope, int parsing_mode) {  // returns the new itt

#pragma region
    auto goto_ast_scope = [&](std::vector<scope_element> in_scope, std::function<void(AST *)> call_after_finished) {
      AST *that_ast = &(globals.global);
      int scope_size = scope.size();
      int scope_i = 0;
      for (auto s : in_scope) {  // copy, do not reference
        if ((int)s >= 0) {       // int
          that_ast = ((dynamic_cast<ast_body *>(that_ast))->body)[(int)s];
        } else {
          switch ((scope_element)s) {
            case scope_element::global:
              that_ast = &((dynamic_cast<ast_types::global_scope *>(that_ast))->global);
              break;
            case scope_element::args:
              that_ast = &((dynamic_cast<ast_types::with_args *>(that_ast))->args);
              break;
            case scope_element::body:
              that_ast = &((dynamic_cast<ast_types::with_body *>(that_ast))->body);
              break;
            case scope_element::second_body:
              that_ast = &((dynamic_cast<ast_types::with_second_body *>(that_ast))->second_body);
              break;
            case scope_element::type:
              that_ast = &((dynamic_cast<ast_types::with_type *>(that_ast))->type);
              break;
            case scope_element::return_type:
              that_ast = &((dynamic_cast<ast_types::with_return_type *>(that_ast))->return_type);
              break;
            case scope_element::arr_index:
              that_ast = &((dynamic_cast<ast_types::arrget *>(that_ast))->index);
              break;
            case scope_element::arr_array:
              that_ast = &((dynamic_cast<ast_types::arrget *>(that_ast))->array);
              break;
            case scope_element::argtype:
              that_ast = &((dynamic_cast<ast_types::with_args_with_type *>(that_ast))->args);
              break;
            default:
              std::cout << "Error: invalid scope element" << std::endl;
              exit(0);
          }
        }
        if (scope_size - 1 >= ++scope_i) {
          // *that_ast = val;
          call_after_finished(that_ast);
        }
      }
    };

    auto set_ast_scope = [&](std::vector<scope_element> scope, AST val) {
      goto_ast_scope(scope, [&](AST *that_ast) {
        *that_ast = val;
      });
    };

    auto append_ast_scope = [&](std::vector<scope_element> scope, AST val) {
      int index_inserted;
      goto_ast_scope(scope, [&](AST *that_ast) {
        index_inserted = (dynamic_cast<ast_body *>(that_ast))->body.size();
        (dynamic_cast<ast_body *>(that_ast))->body.push_back(new AST{val});
      });
      return index_inserted;
    };

    auto get_ast_scope = [&](std::vector<scope_element> scope) {  // returns a pointer to that ast
      AST *retval;
      goto_ast_scope(scope, [&](AST *that_ast) {
        retval = that_ast;
      });
      return retval;
    };

#pragma endregion lexer_functions

    int last_seen_op = -1;

    int itt;
    for (itt = old_itt; itt < program_tokens.size(); itt++) {
      auto look_ahead = [&](int count = 1) {
        itt += count;
        if (itt >= is_length) {
          std::cout << "Error: Premature end-of-file reached" << std::endl;
          exit(-1);
        }
        return program_tokens[itt];
      };

      auto look_behind = [&](int count = 1) {
        itt -= count;
        if (itt < 0) {
          std::cout << "Error: Premature end-of-file reached" << std::endl;
          exit(-1);
        }
        return program_tokens[itt];
      };
      token initial_token = look_ahead();  // get next token

      switch (initial_token.type) {
        case token_type::identifier: {
          if (initial_token.value == "if") {
            // double body statement
            ast_types::statement_with_two_bodies to_append;
            to_append.name = ast_types::string_t("if");
            int appended_index = append_ast_scope(scope, to_append);

            std::vector<scope_element> new_scope = scope;
            new_scope.push_back((scope_element)appended_index);
            new_scope.push_back(scope_element::args);

            while (program_tokens[itt].value != "(") {
              itt = recursive_lex(itt, new_scope, 1);  // args lexing
            }

            new_scope = scope;
            new_scope.push_back((scope_element)appended_index);
            new_scope.push_back(scope_element::body);
            itt = recursive_lex(itt, new_scope, 0);  // body lexing

            if (look_ahead().value == "else") {
              if (look_ahead().value == "if") {
                new_scope = scope;
                scope.push_back((scope_element)appended_index);
                scope.push_back(scope_element::second_body);
                itt = recursive_lex(--itt, new_scope, 0);  // else if body lexing
              } else {
                new_scope = scope;
                new_scope.push_back((scope_element)appended_index);
                new_scope.push_back(scope_element::second_body);
                itt = recursive_lex(--itt, new_scope, 0);  // else body lexing
              }
            } else {
              --itt;
            }
          } else if (initial_token.value == "while" || initial_token.value == "foreach") {
            // single body statement
            ast_types::statement_with_body to_append;
            to_append.name = ast_types::string_t(initial_token.value);
            int appended_index = append_ast_scope(scope, to_append);

            std::vector<scope_element> new_scope = scope;
            new_scope.push_back((scope_element)appended_index);
            new_scope.push_back(scope_element::args);

            while (program_tokens[itt].value != "(") {
              itt = recursive_lex(itt, new_scope, 1);  // args lexing
            }

            new_scope = scope;
            new_scope.push_back((scope_element)appended_index);
            new_scope.push_back(scope_element::body);
            itt = recursive_lex(itt, new_scope, 0);  // body lexing
          } else if (initial_token.value == "return" || initial_token.value == "break" || initial_token.value == "continue") {
            // statement
            ast_types::statement to_append;
            to_append.name = ast_types::string_t(initial_token.value);
            int appended_index = append_ast_scope(scope, to_append);

            if (look_ahead().value == "(") {
              std::vector<scope_element> new_scope = scope;
              new_scope.push_back((scope_element)appended_index);
              new_scope.push_back(scope_element::args);
              recursive_lex(itt, new_scope, 1);  // args lexing
            } else {
              --itt;
            }
          } else if (initial_token.value == "deref" || initial_token.value == "ref") {
            // varop
            ast_types::varop to_append;
            to_append.name = ast_types::string_t(initial_token.value);
            to_append.var = ast_types::string_t(look_ahead().value);
            append_ast_scope(scope, to_append);
          } else if (initial_token.value == "var") {
            // vardef
            ast_types::vardef to_append;
            std::vector<scope_element> new_scope = scope;
            new_scope.push_back((scope_element)append_ast_scope(scope, to_append));
            new_scope.push_back(scope_element::type);
            itt = recursive_lex(itt, new_scope, 3);  // var type lexing
            to_append.name = ast_types::string_t(look_ahead().value);
            append_ast_scope(scope, to_append);
          } else if (initial_token.value == "glvar") {  // TODO: autodetect glvar and deprecate it
            //gldef
            ast_types::glbdef to_append;
            std::vector<scope_element> new_scope = scope;
            new_scope.push_back((scope_element)append_ast_scope(scope, to_append));
            new_scope.push_back(scope_element::type);
            itt = recursive_lex(itt, new_scope, 4);  // var type lexing // I MAY BE WRONG HERE, THIS MIGHT NEED TO BE 3
            to_append.name = ast_types::string_t(look_ahead().value);
            append_ast_scope(scope, to_append);
          } else if (initial_token.value == "ptr" || initial_token.value == "arr" || initial_token.value == "unsigned") {
            // outer type
            ast_types::out_type to_append;
            to_append.name = initial_token.value;  // wait this works lmao so let me get this straight: it can convert
                                                   // a type to a class with no default constructor, but it can't convert
                                                   // an int enum to a fucking int. c'mon.
                                                   // cpp amirite
            if (initial_token.value == "arr") {
              to_append.length = std::stoi(look_ahead().value);  // TODO: make this in square brackets and parse it as args
            }

            std::vector<scope_element> new_scope = scope;
            new_scope.push_back((scope_element)append_ast_scope(scope, to_append));
            new_scope.push_back(scope_element::type);

            itt = recursive_lex(itt, new_scope, 4);  // type lexing
          } else if (initial_token.value == "str" || initial_token.value == "bool" || initial_token.value == "byte" || initial_token.value == "word" || initial_token.value == "int" || initial_token.value == "int64" || initial_token.value == "int128" || initial_token.value == "float") {
            ast_types::in_type to_append;
            to_append.type = initial_token.value;
            append_ast_scope(scope, to_append);
          } else if (initial_token.value == "fun") {
            // have FUN! jk function
            ast_types::fundef to_append;
            to_append.name = look_ahead().value;
            int appended_index = append_ast_scope(scope, to_append);

            while (program_tokens[itt].value != "=>") {
              std::string arg_name = look_ahead().value;
              if (look_ahead().value != ":") {
                std::cout << "Error: expected ':' in function argument list" << std::endl;
                exit(1);
              }
              std::vector<scope_element> new_scope = scope;
              new_scope.push_back((scope_element)appended_index);
              new_scope.push_back(scope_element::argtype);
              ast_types::arg_with_type_t arg_type_t;
              arg_type_t.name = arg_name;
              new_scope.push_back((scope_element)append_ast_scope(new_scope, arg_type_t));
              itt = recursive_lex(itt, new_scope, 4);  // type lexing
            }
            std::vector<scope_element> new_scope = scope;
            new_scope.push_back((scope_element)appended_index);
            new_scope.push_back(scope_element::return_type);
            itt = recursive_lex(itt, new_scope, 4);  // return type lexing

            if (look_ahead().value != "{") {
              std::cout << "Error: expected '{' in function definition" << std::endl;
              exit(1);
            }
            new_scope = scope;
            new_scope.push_back((scope_element)appended_index);
            new_scope.push_back(scope_element::body);
            itt = recursive_lex(itt, new_scope, 0);   // body lexing
          } else if (initial_token.value == "ext") {  // TODO: autodetect and deprecate
                                                      // external function
            ast_types::fundef to_append;
            to_append.name = look_ahead().value;
            int appended_index = append_ast_scope(scope, to_append);

            while (program_tokens[itt].value != "=>") {
              look_ahead();
              std::vector<scope_element> new_scope = scope;
              new_scope.push_back((scope_element)appended_index);
              new_scope.push_back(scope_element::argtype);
              ast_types::arg_with_type_t arg_type_t;
              new_scope.push_back((scope_element)append_ast_scope(new_scope, arg_type_t));
              itt = recursive_lex(itt, new_scope, 4);  // type lexing
            }
            std::vector<scope_element> new_scope = scope;
            new_scope.push_back((scope_element)appended_index);
            new_scope.push_back(scope_element::return_type);
            itt = recursive_lex(itt, new_scope, 4);  // return type lexing
          } else {
            look_ahead();
            if (program_tokens[itt].value == "(") {
              // function call
              ast_types::call to_append;
              to_append.name = ast_types::string_t(initial_token.value);
              int appended_index = append_ast_scope(scope, to_append);

              std::vector<scope_element> new_scope = scope;
              new_scope.push_back((scope_element)appended_index);
              new_scope.push_back(scope_element::args);
              recursive_lex(itt, new_scope, 1);  // args lexing
            } else {
              // this should be a variable
              // since we have already incremented itt, we can check whether it is a assignment or a reference right away
              if (program_tokens[itt].value == "=" || program_tokens[itt].value[1] == '=') {
                // assignment
                token operation = program_tokens[itt];
                token var_name = program_tokens[itt - 1];
                if(operation.value == "=") {
                  // simple assignment
                  ast_types::varset to_append;
                  to_append.name = var_name.value;
                  std::vector<scope_element> new_scope = scope;
                  new_scope.push_back((scope_element)append_ast_scope(scope, to_append));
                  new_scope.push_back(scope_element::args);
                  itt = recursive_lex(itt, new_scope, 1);  // args lexing
                } else{
                  // +=, -=, *=, /=, %=, &=, ^=, |=
                  ast_types::varset to_append;
                  to_append.name = var_name.value;
                  std::vector<scope_element> new_scope = scope;
                  new_scope.push_back((scope_element)append_ast_scope(scope, to_append));
                  new_scope.push_back(scope_element::args);
                  ast_types::oper opr;
                  opr.op = operation.value[0];
                  new_scope.push_back((scope_element)append_ast_scope(new_scope, opr));
                  new_scope.push_back(scope_element::args);
                  ast_types::getvar to_append2;
                  to_append2.name = var_name.value;
                  append_ast_scope(new_scope, to_append2);
                  itt = recursive_lex(itt, new_scope, 1);  // args lexing
                }
              } else {
                // reference
                --itt;
                ast_types::getvar to_append;
                to_append.name = ast_types::string_t(initial_token.value);
                append_ast_scope(scope, to_append);
              }
            }
          }
          break;
        }
        case token_type::number: {
          // number const
          ast_types::const_int to_append;
          to_append.value = stoi(program_tokens[itt].value);
          append_ast_scope(scope, to_append);
          break;
        }
        case token_type::string: {
          // string const
          ast_types::const_str to_append;
          to_append.value = program_tokens[itt].value;
          append_ast_scope(scope, to_append);
          break;
        }
        case token_type::bracket: {
          // brackets
          break;
        }
        case token_type::sym: {
          // symbols
          break;
        }
      }
      if (parsing_mode != 0) {
        if (program_tokens[itt].value == "," || program_tokens[itt].value == "." || program_tokens[itt].value == ")") {  // "." could be unnecessary
          break;
        }
      }

      if (parsing_mode == 2) {
        look_ahead(1);
        itt -= 1;  // check for end of line
        // check for operations
      } else if (parsing_mode == 3) {
        if (program_tokens[itt].value == "=") {
          break;
        }
      } else if (parsing_mode == 4) {
        break;
      }
    }
    return itt;
  };

  recursive_lex(-1, {scope_element::global}, 0);

  return EXIT_SUCCESS;
}