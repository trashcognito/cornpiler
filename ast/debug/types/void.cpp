#include "../../../ast.hpp"

void ast::VoidType::print_type(std::stringstream &stream) const {
    stream << "{\n\"type_ast\":\"VoidType\"}";    //{"type_ast":"VoidType"}
}