#pragma once

#include <cstdint>
#include <iostream>
#include <string>
#include <variant>
#include <vector>

#include "logger.hpp"

#include "pre_ast/ast.hpp"
#include "pre_ast/filereader.hpp"
#include "pre_ast/scopes.hpp"

#include "pre_ast/types/string_t.hpp"
#include "pre_ast/types/char_t.hpp"
#include "pre_ast/types/decimal_t.hpp"
#include "pre_ast/types/number_t.hpp"
#include "pre_ast/types/arg_with_type_t.hpp"

#include "pre_ast/with/args.hpp"
#include "pre_ast/with/body.hpp"
#include "pre_ast/with/return_type.hpp"
#include "pre_ast/with/second_body.hpp"
#include "pre_ast/with/type.hpp"

#include "pre_ast/nodes/arrget.hpp"
#include "pre_ast/nodes/arrset.hpp"
#include "pre_ast/nodes/call.hpp"
#include "pre_ast/nodes/const_decimal.hpp"
#include "pre_ast/nodes/const_int.hpp"
#include "pre_ast/nodes/const_str.hpp"
#include "pre_ast/nodes/expr.hpp"
#include "pre_ast/nodes/extdef.hpp"
#include "pre_ast/nodes/fundef.hpp"
#include "pre_ast/nodes/getvar.hpp"
#include "pre_ast/nodes/glbdef.hpp"
#include "pre_ast/nodes/global.hpp"
#include "pre_ast/nodes/in_type.hpp"
#include "pre_ast/nodes/oper.hpp"
#include "pre_ast/nodes/out_type.hpp"
#include "pre_ast/nodes/scope.hpp"
#include "pre_ast/nodes/statement_with_body.hpp"
#include "pre_ast/nodes/statement_with_two_bodies.hpp"
#include "pre_ast/nodes/statement.hpp"
#include "pre_ast/nodes/vardef.hpp"
#include "pre_ast/nodes/varop.hpp"
#include "pre_ast/nodes/varset.hpp"

bool check_id_constraints(std::string id, char c);

std::vector<token> tokenize_program(std::string program, int length);

bool check_id_constraints(std::string id, char c);
std::vector<token> tokenize_program(std::string program, int length,
                                    logger::logger *logger);
ast_types::global_scope lex_program(file_object input_file,
                                    std::vector<token> program_tokens,
                                    logger::logger *logger);