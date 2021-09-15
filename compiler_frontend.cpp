#include "compile.hpp"
#include "main.hpp"

std::vector<ast::GlobalEntry *> get_program(logger::logger *logger, std::string input_name) {
  file_object input_file = read_file(input_name.c_str(), logger);
  
  std::vector<token> program_tokens =
      tokenize_program(input_file.contents, input_file.length, logger);

  ast_types::global_scope lexed_program =
      lex_program(input_file, program_tokens, logger);

  std::cout << lexed_program.print_node() << std::endl;
  return translate_program(lexed_program, logger);
}