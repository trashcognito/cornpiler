#include "compile.hpp"

#include <cstdlib>
#include <fstream>
#include <functional>
#include <iostream>
#include <vector>

std::string DEBUG_TOKEN_TYPES[] = {"str", "identifier", "number", "decimal", "bracket",
                                   "semi", "sep", "sym"};
AST::~AST() {}

logger::LOG_LEVEL logger::operator|(LOG_LEVEL lhs, LOG_LEVEL rhs) {
  return (LOG_LEVEL)((int)lhs | (int)rhs);
}
logger::LOG_LEVEL logger::operator&(LOG_LEVEL lhs, LOG_LEVEL rhs) {
  return (LOG_LEVEL)((int)lhs & (int)rhs);
}
logger::SETTINGS logger::operator|(SETTINGS lhs, SETTINGS rhs) {
  return (SETTINGS)((int)lhs | (int)rhs);
}
logger::SETTINGS logger::operator&(SETTINGS lhs, SETTINGS rhs) {
  return (SETTINGS)((int)lhs & (int)rhs);
}
std::string logger::logger::log_level_text(LOG_LEVEL lvl) {
  std::string retval = "";
  if ((int)lvl & (int)LOG_LEVEL::NONE) {
    retval += "| NONE ";
  }
  if ((int)lvl & (int)LOG_LEVEL::ERROR) {
    retval += "| ERROR ";
  }
  if ((int)lvl & (int)LOG_LEVEL::WARNING) {
    retval += "| WARNING ";
  }
  if ((int)lvl & (int)LOG_LEVEL::INFO) {
    retval += "| INFO ";
  }
  if ((int)lvl & (int)LOG_LEVEL::DEBUG) {
    retval += "| DEBUG ";
  }
  retval.erase(0, 1);
  return retval;
}
void logger::logger::log(LOG_LEVEL level, std::string msg, SETTINGS settings) {
  if ((int)(level & logger::level) == 0)
    return;
  if ((int)(settings & SETTINGS::TYPE) != 0)
    std::cout << "[" << log_level_text(level) << "] ";
  std::cout << msg;
  if ((int)(settings & SETTINGS::NEWLINE) != 0)
    std::cout << std::endl;
}

logger::logger::logger(LOG_LEVEL log) { level = log; }
entry_bracket::entry_bracket(char f, char s) {
  first = f;
  second = s;
}
token::token(token_type t, std::string v, int r, int c, int ch) {
  type = t;
  value = v;
  row = r;
  col = c;
  chr = ch;
}
ast_types::string_t::string_t(std::string v) { value = v; }
ast_types::string_t::string_t(char v) { value = std::string(1, v); }
ast_types::char_t::char_t(char v) { value = v; }
ast_types::number_t::number_t(int v) { value = v; }
ast_types::decimal_t::decimal_t(float v) { value = v; }
ast_types::arg_with_type_t::arg_with_type_t() { name = string_t(""); }
ast_types::global_scope::global_scope() { act = act_type::global; }
ast_types::statement::statement() { act = act_type::statement; }
ast_types::statement_with_body::statement_with_body() {
  act = act_type::statement_with_body;
}
ast_types::statement_with_two_bodies::statement_with_two_bodies() {
  act = act_type::statement_with_two_bodies;
}
ast_types::varop::varop() { act = act_type::varop; }
ast_types::vardef::vardef() { act = act_type::vardef; }
ast_types::in_type::in_type() { act = act_type::type; }
ast_types::out_type::out_type() { act = act_type::outtype; }
ast_types::extdef::extdef() { act = act_type::extdef; }
ast_types::fundef::fundef() { act = act_type::fundef; }
ast_types::glbdef::glbdef() { act = act_type::glbdef; }
ast_types::call::call() { act = act_type::call; }
ast_types::varset::varset() { act = act_type::varset; }
ast_types::getvar::getvar() { act = act_type::getvar; }
ast_types::const_str::const_str() { act = act_type::const_str; }
ast_types::const_int::const_int() { act = act_type::const_int; }
ast_types::const_decimal::const_decimal() { act = act_type::const_decimal; }
ast_types::oper::oper() { act = act_type::oper; }
ast_types::expr::expr() { act = act_type::expr; }
ast_types::arrset::arrset() { act = act_type::arrset; }
ast_types::arrget::arrget() { act = act_type::arrget; }

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

std::string ast_types::string_t::print_node() { return "\"" + value + "\""; }
std::string ast_types::char_t::print_node() { return "'" + std::string(1, value) + "'"; }
std::string ast_types::number_t::print_node() { return std::to_string(value); }
std::string ast_types::decimal_t::print_node() { return std::to_string(value); }

std::string ast_types::global_scope::print_node() {
  return "{\"globals\":" + this->body.print_node() + "}";
}
std::string ast_types::statement::print_node() { return "\"statement\": { \"name\":" +
                                                        this->name.print_node() +
                                                        ",\"args\": " + this->args.print_node() + "}"; }
std::string ast_types::statement_with_body::print_node() {
  return "\"statement_with_body\": {\"name\":" + this->name.print_node() +
         ",\"args\": " + this->args.print_node() +
         ",\"body\": " + this->body.print_node() + "}";
}
std::string ast_types::statement_with_two_bodies::print_node() {
  return "\"statement_with_two_bodies\": {\"name\":" + this->name.print_node() +
         ",\"args\": " + this->args.print_node() +
         ",\"body1\": " + this->body.print_node() +
         ",\"body2\": " + this->second_body.print_node() + "}";
}
std::string ast_types::varop::print_node() {
  return "\"varop\": {\"name\":" + this->name.print_node() +
         ",\"var\": " + this->var.print_node() + "}";
}
std::string ast_types::vardef::print_node() {
  return "\"vardef\": {\"name\":" + this->name.print_node() +
         ",\"type\": " + this->type.print_node() +
         ",\"args\": " + this->args.print_node() + "}";
}
std::string ast_types::in_type::print_node() {
  return "\"in_type\": {\"type\":" + this->type.print_node() + "}";
}
std::string ast_types::out_type::print_node() {
  return "\"out_type\": {\"type\":" + this->name.print_node() + ",\"inner_type\":" + this->type.print_node() + ",\"length\":" + this->length.print_node() + "}";
}
std::string ast_types::extdef::print_node() {
  return "\"extdef\": {\"name\":" + this->name.print_node() +
         ",\"args\": " + this->args.print_node() +
         ",\"return_type\": " + this->return_type.print_node() + "}";
}
std::string ast_types::fundef::print_node() {
  return "\"fundef\": {\"name\":" + this->name.print_node() +
         ",\"args\": " + this->args.print_node() +
         ",\"return_type\": " + this->return_type.print_node() +
         ",\"body\": " + this->body.print_node() + "}";
}
std::string ast_types::glbdef::print_node() {
  return "\"vardef\": {\"name\":" + this->name.print_node() +
         ",\"type\": " + this->type.print_node() +
         ",\"args\": " + this->args.print_node() + "}";
}
std::string ast_types::call::print_node() {
  return "\"call\": {\"name\":" + this->name.print_node() +
         ",\"args\": " + this->args.print_node() + "}";
}
std::string ast_types::varset::print_node() {
  return "\"varset\": {\"name\":" + this->name.print_node() +
         ",\"args\": " + this->args.print_node() + "}";
}
std::string ast_types::getvar::print_node() {
  return "\"getvar\": {\"name\":" + this->name.print_node() + "}";
}
std::string ast_types::const_str::print_node() {
  return "\"const_str\": {\"value\":" + this->value.print_node() + "}";
}
std::string ast_types::const_int::print_node() {
  return "\"const_int\": {\"value\":" + this->value.print_node() + "}";
}
std::string ast_types::const_decimal::print_node() {
  return "\"const_decimal\": {\"value\":" + this->value.print_node() + "}";
}
std::string ast_types::oper::print_node() {
  return "\"oper\": {\"op\":" + this->op.print_node() +
         ",\"args\": " + this->args.print_node() + "}";
}
std::string ast_types::expr::print_node() {
  return "\"expr\": {\"args\": " + this->args.print_node() + "}";
}
std::string ast_types::arrset::print_node() {
  return "\"arrset\": {\"args\": " + this->args.print_node() + "}";
}
std::string ast_types::arrget::print_node() {
  return "\"arrget\": {\"index\":" + this->index.print_node() +
         ",\"arr\": " + this->array.print_node() + "}";
}
std::string ast_types::arg_with_type_t::print_node() {
  return "\"arg_with_type\": {\"type\":" + this->type.print_node() +
         ",\"name\":" + this->name.print_node() + "}";
}

file_object read_file(const char *filename, logger::logger *logger) {
  std::ifstream is(filename);
  if (!is) {
    logger->log(logger::LOG_LEVEL::ERROR, "File not found");
    exit(-1);
  }

  // is.seekg(0, is.end);
  // int is_length = is.tellg();
  // is.seekg(0, is.beg);

  // char *file_buffer = new char[is_length];
  // int f_iter = 0;
  // while (!is.eof() && f_iter < is_length) {
  //   is.get(file_buffer[f_iter]); // reading single character from file to array
  //   f_iter++;
  // }
  // is.close();

  std::string content((std::istreambuf_iterator<char>(is)),
                      (std::istreambuf_iterator<char>()));

  return file_object(content, content.length());
}
bool check_id_constraints(std::string id, char c) {
  bool retval = isalpha(c) || c == '_';
  if (!id.empty())
    retval |= isdigit(c);
  return retval;
}
std::vector<token> tokenize_program(std::string program, int length,
                                    logger::logger *logger) {
  class {
   public:
    int row = 0;
    int col = 0;
    int chr_c = 0;
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
      tokens.push_back(token(t, full_token, row, col, chr_c));
    }
  } status;

  auto error_out = [&](std::string errmsg,
                       logger::LOG_LEVEL level = logger::LOG_LEVEL::ERROR,
                       bool _exit = true) {
    logger->log(level, errmsg);
    std::string line = "";
    std::string error_show = "^";
    int k = status.chr_c;
    while (k >= 0 && program[k] != '\n') {
      line = program[k] + line;
      error_show = "-" + error_show;
      k--;
    }
    k = status.chr_c + 1;
    while (k < length && program[k] != '\n') {
      line += program[k];
      k++;
    }
    logger->log(level, "");
    logger->log(level, std::to_string(status.row + 1) + " | " + line);

    for (int j = 0;
         j < std::string(std::to_string(status.row + 1) + " | ").length();
         j++) {
      error_show = " " + error_show;
    }
    logger->log(level, error_show);
    if (_exit)
      exit(-1);
  };

  status.full_token = "{";
  status.push(token_type::bracket);
  status.reset();

  // traverse through the file char by char
  for (int i = 0; i < length; i++) {
    char c = program[i];

    if (c == '\n') {
      status.row++;
      status.col = 0;
    }

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
        error_out(std::string("Invalid escape character: \\") + (c));
      }
    };

    if (status.comment) {
      if (c == '\n')
        status.comment = false;
      continue;
    }

    std::string old_full_token =
        status.full_token;  // set old full token here to check for symbols later

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
            error_out("Invalid character literal size: " +
                      std::to_string(status.full_token.length()));
          } else {
            status.full_token = std::to_string(status.full_token[0]);
            if (status.full_token.find('.') == std::string::npos) {
              status.push(token_type::number);
            } else {
              status.push(token_type::decimal);
            }
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
            if (status.full_token.find('.') == std::string::npos) {
              status.push(token_type::number);
            } else {
              status.push(token_type::decimal);
            }
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
    status.chr_c++;
    status.col++;
  }

  status.full_token = "}";
  status.push(token_type::bracket);
  status.reset();

  for (auto &t : status.tokens) {
    logger->log(logger::LOG_LEVEL::DEBUG, "Token: " + t.value + "\t\t\tType: " +
                                              DEBUG_TOKEN_TYPES[(int)t.type]);
  }

  return status.tokens;
}
ast_types::global_scope lex_program(file_object input_file,
                                    std::vector<token> program_tokens,
                                    logger::logger *logger) {
  ast_types::global_scope globals;

  std::function<int(int, std::vector<scope_element>, int, entry_bracket)>
      recursive_lex = [&](int old_itt, std::vector<scope_element> scope,
                          int parsing_mode,
                          entry_bracket entr) {  // returns the new itt
        auto error_out = [&](std::string error_message, token tk,
                             logger::LOG_LEVEL level = logger::LOG_LEVEL::ERROR,
                             bool _exit = true) {
          logger->log(level, error_message);
          std::string line = "";
          std::string error_show = "^";
          int k = tk.chr;
          while (k >= 0 && input_file.contents[k] != '\n') {
            line = input_file.contents[k] + line;
            error_show = "-" + error_show;
            k--;
          }
          k = tk.chr + 1;
          while (k < input_file.length && input_file.contents[k] != '\n') {
            line += input_file.contents[k];
            k++;
          }
          logger->log(level, "");
          logger->log(level, std::to_string(tk.row + 1) + " | " + line);
          for (int j = 0;
               j < std::string(std::to_string(tk.row + 1) + " | ").length();
               j++) {
            error_show = " " + error_show;
          }
          logger->log(level, error_show);
          if (_exit)
            exit(-1);
        };

        auto goto_ast_scope = [&](std::vector<scope_element> in_scope) {
          logger->log(logger::LOG_LEVEL::DEBUG, "Reaching into ",
                      logger::SETTINGS::TYPE);
          AST *that_ast = &(globals);
          int scope_i = 0;
          for (auto s : in_scope) {  // copy, do not reference
            if ((int)s >= 0) {       // int
              ast_body *_temp_body = dynamic_cast<ast_body *>(that_ast);
              std::vector<AST *> _temp_inhere = _temp_body->body;
              that_ast = _temp_inhere[(int)s];

              logger->log(logger::LOG_LEVEL::DEBUG,
                          "> [" + std::to_string((int)s) + "] ",
                          logger::SETTINGS::NONE);
            } else {
              switch ((scope_element)s) {
                case scope_element::global:
                  that_ast = &(
                      (dynamic_cast<ast_types::global_scope *>(that_ast))->body);

                  logger->log(logger::LOG_LEVEL::DEBUG, "> global",
                              logger::SETTINGS::NONE);
                  break;
                case scope_element::args:
                  that_ast =
                      &((dynamic_cast<ast_types::with_args *>(that_ast))->args);

                  logger->log(logger::LOG_LEVEL::DEBUG, "> args",
                              logger::SETTINGS::NONE);
                  break;
                case scope_element::body:
                  that_ast =
                      &((dynamic_cast<ast_types::with_body *>(that_ast))->body);

                  logger->log(logger::LOG_LEVEL::DEBUG, "> body",
                              logger::SETTINGS::NONE);
                  break;
                case scope_element::second_body:
                  that_ast =
                      &((dynamic_cast<ast_types::with_second_body *>(that_ast))
                            ->second_body);

                  logger->log(logger::LOG_LEVEL::DEBUG, "> second_body",
                              logger::SETTINGS::NONE);
                  break;
                case scope_element::type:
                  that_ast =
                      &((dynamic_cast<ast_types::with_type *>(that_ast))->type);

                  logger->log(logger::LOG_LEVEL::DEBUG, "> type",
                              logger::SETTINGS::NONE);
                  break;
                case scope_element::return_type:
                  that_ast =
                      &((dynamic_cast<ast_types::with_return_type *>(that_ast))
                            ->return_type);

                  logger->log(logger::LOG_LEVEL::DEBUG, "> return_type",
                              logger::SETTINGS::NONE);
                  break;
                case scope_element::arr_index:
                  that_ast =
                      &((dynamic_cast<ast_types::arrget *>(that_ast))->index);

                  logger->log(logger::LOG_LEVEL::DEBUG, "> arr_index",
                              logger::SETTINGS::NONE);
                  break;
                case scope_element::arr_array:
                  that_ast =
                      &((dynamic_cast<ast_types::arrget *>(that_ast))->array);

                  logger->log(logger::LOG_LEVEL::DEBUG, "> arr_array",
                              logger::SETTINGS::NONE);
                  break;
                case scope_element::argtype:
                  that_ast =
                      &((dynamic_cast<ast_types::with_args_with_type *>(that_ast))
                            ->args);

                  logger->log(logger::LOG_LEVEL::DEBUG, "> argtype",
                              logger::SETTINGS::NONE);
                  break;
                case scope_element::out_length:
                  that_ast =
                      &((dynamic_cast<ast_types::out_type *>(that_ast))->length);

                  logger->log(logger::LOG_LEVEL::DEBUG, "> out_length",
                              logger::SETTINGS::NONE);
                default:

                  logger->log(logger::LOG_LEVEL::ERROR,
                              "\nInvalid scope element: " + std::to_string((int)s));
                  exit(0);
              }
            }
          }
          logger->log(logger::LOG_LEVEL::DEBUG, "", logger::SETTINGS::NEWLINE);
          return that_ast;
        };

        auto set_ast_scope = [&](std::vector<scope_element> scope, AST *val) {
          *goto_ast_scope(scope) = *val;
        };

        auto append_ast_scope = [&](std::vector<scope_element> scope,
                                    AST *val) {
          int index_inserted;
          AST *that_ast = goto_ast_scope(scope);
          index_inserted = (dynamic_cast<ast_body *>(that_ast))->body.size();

          (dynamic_cast<ast_body *>(that_ast))->body.push_back(val);
          return index_inserted;
        };

        auto get_ast_scope = [&](std::vector<scope_element>
                                     scope) {  // returns a pointer to that ast
          return goto_ast_scope(scope);
        };

        auto check_if_operation = [](token kword) {
          return (
              kword.value == "+" || kword.value == "-" || kword.value == "*" ||
              kword.value == "/" || kword.value == ">" || kword.value == "<" ||
              kword.value == "==" || kword.value == "<=" ||
              kword.value == ">=" || kword.value == "&&" ||
              kword.value == "||" || kword.value == "&" || kword.value == "|" ||
              kword.value == "^" || kword.value == "!=" || kword.value == "!" ||
              kword.value == "%");
        };
        int itt = old_itt;
        while (true) {
          bool try_continue = true;
          auto look_ahead = [&](int count = 1) {
            itt += count;
            if (itt >= program_tokens.size()) {
              error_out("Premature end-of-file reached",
                        program_tokens[itt - count]);
            }
            return program_tokens[itt];
          };

          auto look_behind = [&](int count = 1) {
            itt -= count;
            if (itt < 0) {
              error_out("Premature end-of-file reached",
                        program_tokens[itt + count]);
            }
            return program_tokens[itt];
          };
          token initial_token = look_ahead();  // get next token

          logger->log(logger::LOG_LEVEL::DEBUG,
                      "recursive lexer loop entry, itt: " +
                          std::to_string(itt) + " total token amount: " +
                          std::to_string(program_tokens.size()));
          logger->log(logger::LOG_LEVEL::DEBUG,
                      "Token info: " + initial_token.value + "\t\t\t" +
                          DEBUG_TOKEN_TYPES[(int)initial_token.type]);
          switch (initial_token.type) {
            case token_type::identifier: {
              if (initial_token.value == "for") {
                ast_types::statement_with_body *to_append = new ast_types::statement_with_body;
                to_append->name = ast_types::string_t("while");
                int appended_index = -1;
                look_ahead();

                int incr_cnt = 0;
                while (program_tokens[itt].value != ")") {
                  std::vector<scope_element> new_scope = scope;
                  if (incr_cnt == 1) {
                    new_scope.push_back((scope_element)appended_index);
                    new_scope.push_back(scope_element::args);
                  } else if (incr_cnt == 2) {
                    new_scope.push_back((scope_element)appended_index);
                    new_scope.push_back(scope_element::body);
                  }
                  itt = recursive_lex(itt, new_scope, 1, entry_bracket('(', ')'));
                  if (incr_cnt == 0) {
                    appended_index = append_ast_scope(new_scope, to_append);
                  }
                  incr_cnt++;
                }
                if (incr_cnt != 3) {
                  error_out("Invalid argument count in for loop", initial_token);
                }
                look_ahead();
                std::vector<scope_element> new_scope = scope;
                new_scope.push_back((scope_element)appended_index);
                new_scope.push_back(scope_element::body);
                itt = recursive_lex(itt, new_scope, 0, entry_bracket('{', '}'));
              } else if (initial_token.value == "if") {
                // double body statement
                ast_types::statement_with_two_bodies *to_append =
                    new ast_types::statement_with_two_bodies;
                to_append->name = ast_types::string_t("if");
                int appended_index = append_ast_scope(scope, to_append);

                std::vector<scope_element> new_scope = scope;
                new_scope.push_back((scope_element)appended_index);
                new_scope.push_back(scope_element::args);

                while (program_tokens[itt].value != ")") {
                  itt = recursive_lex(itt, new_scope, 1,
                                      entry_bracket('(', ')'));  // args lexing
                }

                new_scope = scope;
                new_scope.push_back((scope_element)appended_index);
                new_scope.push_back(scope_element::body);
                itt = recursive_lex(itt, new_scope, 0,
                                    entry_bracket('{', '}'));  // body lexing

                if (look_ahead().value == "else") {
                  if (look_ahead().value == "if") {
                    new_scope = scope;
                    scope.push_back((scope_element)appended_index);
                    scope.push_back(scope_element::second_body);
                    itt = recursive_lex(
                        --itt, new_scope, 0,
                        entry_bracket('{', '}'));  // else if body lexing
                  } else {
                    new_scope = scope;
                    new_scope.push_back((scope_element)appended_index);
                    new_scope.push_back(scope_element::second_body);
                    itt = recursive_lex(
                        --itt, new_scope, 0,
                        entry_bracket('{', '}'));  // else body lexing
                  }
                } else {
                  --itt;
                }
              } else if (initial_token.value == "while" ||
                         initial_token.value == "foreach") {
                // single body statement
                ast_types::statement_with_body *to_append =
                    new ast_types::statement_with_body;
                to_append->name = ast_types::string_t(initial_token.value);
                int appended_index = append_ast_scope(scope, to_append);

                std::vector<scope_element> new_scope = scope;
                new_scope.push_back((scope_element)appended_index);
                new_scope.push_back(scope_element::args);

                while (program_tokens[itt].value != ")") {
                  itt = recursive_lex(itt, new_scope, 1,
                                      entry_bracket('(', ')'));  // args lexing
                }

                new_scope = scope;
                new_scope.push_back((scope_element)appended_index);
                new_scope.push_back(scope_element::body);
                itt = recursive_lex(itt, new_scope, 0,
                                    entry_bracket('{', '}'));  // body lexing
              } else if (initial_token.value == "return" ||
                         initial_token.value == "break" ||
                         initial_token.value == "continue" ||
                         initial_token.value == "deref") {
                // statement
                ast_types::statement *to_append = new ast_types::statement;
                to_append->name = ast_types::string_t(initial_token.value);
                int appended_index = append_ast_scope(scope, to_append);

                if (look_ahead().value == "(") {
                  std::vector<scope_element> new_scope = scope;
                  new_scope.push_back((scope_element)appended_index);
                  new_scope.push_back(scope_element::args);
                  itt = recursive_lex(--itt, new_scope, 1,
                                      entry_bracket('(', ')'));  // args lexing
                } else {
                  --itt;
                }
              } else if (
                  initial_token.value == "ref") {
                // varop
                ast_types::varop *to_append = new ast_types::varop;
                to_append->name = ast_types::string_t(initial_token.value);
                to_append->var = ast_types::string_t(look_ahead().value);
                append_ast_scope(scope, to_append);
              } else if (initial_token.value == "var") {
                // vardef
                ast_types::vardef *to_append = new ast_types::vardef;
                std::vector<scope_element> new_scope = scope;
                int appended_index = append_ast_scope(scope, to_append);
                new_scope.push_back(
                    (scope_element)appended_index);
                new_scope.push_back(scope_element::type);
                itt = recursive_lex(itt, new_scope, 4,
                                    entry_bracket('(', ')'));  // var type lexing
                to_append->name = ast_types::string_t(look_ahead().value);
                if (look_ahead().value == "=") {
                  new_scope = scope;
                  new_scope.push_back((scope_element)appended_index);
                  new_scope.push_back(scope_element::args);
                  itt = recursive_lex(itt, new_scope, 2, entry_bracket('(', ')'));
                } else {
                  --itt;
                }
              } else if (initial_token.value ==
                         "glvar") {  // TODO: autodetect glvar and deprecate it
                // gldef
                ast_types::glbdef *to_append = new ast_types::glbdef;
                std::vector<scope_element> new_scope = scope;
                new_scope.push_back(
                    (scope_element)append_ast_scope(scope, to_append));
                new_scope.push_back(scope_element::type);
                itt = recursive_lex(
                    itt, new_scope, 4,
                    entry_bracket('(', ')'));  // var type lexing // I MAY BE WRONG
                                               // HERE, THIS MIGHT NEED TO BE 3
                to_append->name = ast_types::string_t(look_ahead().value);
                append_ast_scope(scope, to_append);
              } else if (initial_token.value == "ptr" ||
                         initial_token.value == "arr" ||
                         initial_token.value == "unsigned") {
                // outer type
                ast_types::out_type *to_append = new ast_types::out_type;
                to_append->name =
                    initial_token
                        .value;  // wait this works lmao so let me get this
                                 // straight: it can convert a type to a class with
                                 // no default constructor, but it can't convert an
                                 // int enum to a fucking int. c'mon. cpp amirite

                int appended_index = append_ast_scope(scope, to_append);
                if (initial_token.value == "arr") {
                  if (look_ahead().value != "[") {
                    error_out("array type must be followed by [",
                              program_tokens[itt]);
                  }
                  std::vector<scope_element> new_scope = scope;
                  new_scope.push_back((scope_element)appended_index);
                  new_scope.push_back(scope_element::out_length);
                  itt = recursive_lex(
                      itt, new_scope, 4,
                      entry_bracket('[', ']'));  // array length lexing
                }

                std::vector<scope_element> new_scope = scope;
                new_scope.push_back((scope_element)appended_index);
                new_scope.push_back(scope_element::type);

                itt = recursive_lex(itt, new_scope, 4,
                                    entry_bracket('(', ')'));  // type lexing
              } else if (initial_token.value == "str" ||
                         initial_token.value == "none" ||
                         initial_token.value == "bool" ||
                         initial_token.value == "nibble" ||
                         initial_token.value == "byte" ||
                         initial_token.value == "word" ||
                         initial_token.value == "int" ||
                         initial_token.value == "int64" ||
                         initial_token.value == "int128" ||
                         initial_token.value == "float") {
                ast_types::in_type *to_append = new ast_types::in_type;
                to_append->type = initial_token.value;
                append_ast_scope(scope, to_append);
              } else if (initial_token.value == "fun") {
                // have FUN! jk function
                ast_types::fundef *to_append = new ast_types::fundef;
                to_append->name = look_ahead().value;
                int appended_index = append_ast_scope(scope, to_append);

                look_ahead();
                while (program_tokens[itt].value != "=>") {
                  std::string arg_name = program_tokens[itt].value;
                  if (look_ahead().value != ":") {
                    error_out("expected ':' in function argument list",
                              program_tokens[itt]);
                  }
                  std::vector<scope_element> new_scope = scope;
                  new_scope.push_back((scope_element)appended_index);
                  new_scope.push_back(scope_element::argtype);
                  ast_types::arg_with_type_t *arg_type_t =
                      new ast_types::arg_with_type_t;
                  arg_type_t->name = ast_types::string_t(arg_name);
                  new_scope.push_back(
                      (scope_element)append_ast_scope(new_scope, arg_type_t));
                  new_scope.push_back(scope_element::type);
                  itt = recursive_lex(itt, new_scope, 4,
                                      entry_bracket('(', ')'));  // type lexing
                  look_ahead();
                }
                std::vector<scope_element> new_scope = scope;
                new_scope.push_back((scope_element)appended_index);
                new_scope.push_back(scope_element::return_type);
                itt =
                    recursive_lex(itt, new_scope, 4,
                                  entry_bracket('(', ')'));  // return type lexing

                if (look_ahead().value != "{") {
                  error_out("expected '{' in function definition",
                            program_tokens[itt - 1]);
                }
                new_scope = scope;
                new_scope.push_back((scope_element)appended_index);
                new_scope.push_back(scope_element::body);
                itt = recursive_lex(itt, new_scope, 0,
                                    entry_bracket('{', '}'));  // body lexing
              } else if (initial_token.value ==
                         "ext") {  // TODO: autodetect and deprecate
                                   // external function
                ast_types::extdef *to_append = new ast_types::extdef;
                to_append->name = look_ahead().value;
                int appended_index = append_ast_scope(scope, to_append);

                while (program_tokens[itt].value != "=>") {
                  std::vector<scope_element> new_scope = scope;
                  new_scope.push_back((scope_element)appended_index);
                  new_scope.push_back(scope_element::argtype);
                  ast_types::arg_with_type_t *arg_type_t =
                      new ast_types::arg_with_type_t;
                  new_scope.push_back(
                      (scope_element)append_ast_scope(new_scope, arg_type_t));
                  new_scope.push_back(scope_element::type);
                  int tmp_itt = recursive_lex(itt, new_scope, 4,
                                              entry_bracket('(', ')'));  // type lexing
                  itt = tmp_itt;
                }
                std::vector<scope_element> new_scope = scope;
                new_scope.push_back((scope_element)appended_index);
                new_scope.push_back(scope_element::return_type);
                itt =
                    recursive_lex(itt, new_scope, 4,
                                  entry_bracket('(', ')'));  // return type lexing
              } else {
                look_ahead();
                if (program_tokens[itt].value == "(") {
                  // function call
                  bool self_function = program_tokens[itt - 1].value == ".";

                  ast_types::call *to_append = new ast_types::call;
                  to_append->name = ast_types::string_t(initial_token.value);
                  int appended_index = append_ast_scope(scope, to_append);

                  if (self_function) {
                    // to_append->args.body.push_back(); TODO
                  }

                  std::vector<scope_element> new_scope = scope;
                  new_scope.push_back((scope_element)appended_index);
                  new_scope.push_back(scope_element::args);
                  while (program_tokens[itt].value != ")") {
                    itt = recursive_lex(itt, new_scope, 1,
                                        entry_bracket('(', ')'));  // args lexing
                  }
                } else {
                  // this should be a variable
                  // since we have already incremented itt, we can check whether
                  // it is a assignment or a reference right away
                  if (program_tokens[itt].value == "=" ||
                      (program_tokens[itt].value[1] == '=' && program_tokens[itt].value[0] != '=' && program_tokens[itt].value[0] != '!' && program_tokens[itt].value[0] != '<' && program_tokens[itt].value[0] != '>')) {
                    // assignment
                    token operation = program_tokens[itt];
                    token var_name = program_tokens[itt - 1];
                    if (operation.value == "=") {
                      // simple assignment
                      ast_types::varset *to_append = new ast_types::varset;
                      to_append->name = var_name.value;
                      std::vector<scope_element> new_scope = scope;
                      new_scope.push_back(
                          (scope_element)append_ast_scope(scope, to_append));
                      new_scope.push_back(scope_element::args);
                      itt = recursive_lex(itt, new_scope, 2,
                                          entry_bracket('(', ')'));  // args lexing
                    } else {
                      // +=, -=, *=, /=, %=, &=, ^=, |=
                      ast_types::varset *to_append = new ast_types::varset;
                      to_append->name = var_name.value;
                      std::vector<scope_element> new_scope = scope;
                      new_scope.push_back(
                          (scope_element)append_ast_scope(scope, to_append));
                      new_scope.push_back(scope_element::args);
                      ast_types::oper *opr = new ast_types::oper;
                      opr->op = ast_types::string_t(operation.value[0]);
                      new_scope.push_back(
                          (scope_element)append_ast_scope(new_scope, opr));
                      new_scope.push_back(scope_element::args);
                      ast_types::getvar *to_append2 = new ast_types::getvar;
                      to_append2->name = var_name.value;
                      append_ast_scope(new_scope, to_append2);
                      itt = recursive_lex(itt, new_scope, 2,
                                          entry_bracket('(', ')'));  // args lexing
                    }
                  } else {
                    // reference
                    --itt;
                    ast_types::getvar *to_append = new ast_types::getvar;
                    to_append->name = ast_types::string_t(initial_token.value);
                    append_ast_scope(scope, to_append);
                  }
                }
              }
              break;
            }
            case token_type::number: {
              // number const
              ast_types::const_int *to_append = new ast_types::const_int;
              to_append->value = stoi(program_tokens[itt].value);
              append_ast_scope(scope, to_append);
              break;
            }
            case token_type::decimal: {
              // decimal const
              ast_types::const_decimal *to_append = new ast_types::const_decimal;
              to_append->value = stod(program_tokens[itt].value);
              append_ast_scope(scope, to_append);
              break;
            }
            case token_type::string: {
              // string const
              ast_types::const_str *to_append = new ast_types::const_str;
              to_append->value = program_tokens[itt].value;
              append_ast_scope(scope, to_append);
              break;
            }
            case token_type::bracket: {
              // brackets
              if (initial_token.value == "(") {
                if (old_itt + 1 != itt) {
                  ast_types::expr *to_append = new ast_types::expr;
                  std::vector<scope_element> new_scope = scope;
                  new_scope.push_back(
                      (scope_element)append_ast_scope(new_scope, to_append));
                  new_scope.push_back(scope_element::args);
                  itt = recursive_lex(itt, new_scope, 1,
                                      entry_bracket('(', ')'));  // args lexing
                  continue;
                }
              } else if (initial_token.value == "[") {
                // defining an array
                if (program_tokens[itt - 1].type == token_type::sym) {
                  ast_types::arrset *to_append = new ast_types::arrset;
                  std::vector<scope_element> new_scope = scope;
                  new_scope.push_back(
                      (scope_element)append_ast_scope(new_scope, to_append));
                  new_scope.push_back(scope_element::args);
                  while (program_tokens[itt].value != "]") {
                    itt =
                        recursive_lex(itt, new_scope, 1, entry_bracket('(', ')'));
                  }
                } else {
                  // indexing an array
                  AST *old_stat_place =
                      (AST *)(dynamic_cast<ast_body *>(get_ast_scope(scope))
                                  ->body.back());
                  (dynamic_cast<ast_body *>(get_ast_scope(scope)))
                      ->body.pop_back();
                  ast_types::arrget *to_append = new ast_types::arrget;
                  to_append->array.body.push_back(old_stat_place);
                  std::vector<scope_element> new_scope = scope;
                  new_scope.push_back(
                      (scope_element)append_ast_scope(scope, to_append));
                  new_scope.push_back(scope_element::arr_index);

                  itt = recursive_lex(itt, new_scope, 1,
                                      entry_bracket('(', ')'));  // args lexing
                }
              } else if (entr.second == program_tokens[itt].value[0]) {
                // scope end
                // if (scope.size() == 3) {
                //   // function end
                //   ast_types::statement *to_append = new ast_types::statement;
                //   to_append->name = ast_types::string_t("return");
                //   append_ast_scope(scope, to_append);
                // }
                try_continue = false;
              }
              break;
            }
            case token_type::sym: {
              // symbols
              if (check_if_operation(initial_token)) {
                if (initial_token.value == "-" &&
                    dynamic_cast<ast_body *>(get_ast_scope(scope))->body.size() ==
                        0) {
                  // unary minus
                  ast_types::statement *to_append = new ast_types::statement;
                  to_append->name = ast_types::string_t("neg");
                  std::vector<scope_element> new_scope = scope;
                  new_scope.push_back(
                      (scope_element)append_ast_scope(scope, to_append));
                  new_scope.push_back(scope_element::args);
                  itt = recursive_lex(itt, new_scope,
                                      parsing_mode == 0 ? 1 : parsing_mode,
                                      entry_bracket('(', ')'));  // args lexing
                } else if (initial_token.value == "!") {
                  ast_types::statement *to_append = new ast_types::statement;
                  to_append->name = ast_types::string_t("not");
                  std::vector<scope_element> new_scope = scope;
                  new_scope.push_back(
                      (scope_element)append_ast_scope(scope, to_append));
                  new_scope.push_back(scope_element::args);
                  itt = recursive_lex(itt, new_scope,
                                      parsing_mode == 0 ? 1 : parsing_mode,
                                      entry_bracket('(', ')'));  // args lexing
                } else {
                  // operation
                  AST *old_stat_place =
                      (AST *)(dynamic_cast<ast_body *>(get_ast_scope(scope))
                                  ->body.back());
                  (dynamic_cast<ast_body *>(get_ast_scope(scope)))
                      ->body.pop_back();
                  ast_types::oper *to_append = new ast_types::oper;
                  to_append->args.body.push_back(old_stat_place);
                  to_append->op = initial_token.value;
                  std::vector<scope_element> new_scope = scope;
                  new_scope.push_back(
                      (scope_element)append_ast_scope(scope, to_append));
                  new_scope.push_back(scope_element::args);
                  itt = recursive_lex(itt, new_scope,
                                      parsing_mode == 0 ? 1 : parsing_mode,
                                      entry_bracket('(', ')'));  // args lexing
                }
              }
              break;
            }
            case token_type::sep: {
              // error_out("Unnecessary seperator found", program_tokens[itt],
              //           logger::LOG_LEVEL::WARNING, false);
            }
            case token_type::semi:
              break;
          }
          if (!try_continue)
            break;
          if (parsing_mode != 0) {
            if (program_tokens[itt].value == "," ||
                program_tokens[itt].value == "." ||
                program_tokens[itt].value == ")") {  // "." could be unnecessary
              break;
            }
          }

          if (parsing_mode == 2) {
            look_ahead();
            itt -= 1;  // check for end of file
            if ((!check_if_operation(program_tokens[itt])) &&
                (!check_if_operation(program_tokens[itt + 1]))) {
              break;
            }
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

  recursive_lex(-1, {scope_element::global}, 0, entry_bracket('{', '}'));

  return globals;
}

// int main() {
//   logger::logger logger(logger::LOG_LEVEL::DEBUG | logger::LOG_LEVEL::INFO |
//   logger::LOG_LEVEL::WARNING | logger::LOG_LEVEL::ERROR |
//   logger::LOG_LEVEL::NONE);

//   logger.log(logger::LOG_LEVEL::DEBUG, "Logger Test");

//   file_object input_file = read_file("tests/helloworld.crn", &logger);
//   std::vector<token> program_tokens = tokenize_program(input_file.contents,
//   input_file.length, &logger);

//   ast_types::global_scope lexed_program = lex_program(input_file,
//   program_tokens, &logger);

//   return EXIT_SUCCESS;
// }