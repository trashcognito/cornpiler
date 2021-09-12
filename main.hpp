#include "ast.hpp"
#include "compile.hpp"

std::vector<ast::GlobalEntry *> translate_program(ast_types::global_scope program, logger::logger *logger);