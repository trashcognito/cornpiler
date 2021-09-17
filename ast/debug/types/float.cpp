#include "../../../ast.hpp"

void ast::FloatType::print_type(std::stringstream &stream) const {
    stream << "{\n\"type_ast\":\"FloatType\"";    //{"type_ast":"FloatType"}
}