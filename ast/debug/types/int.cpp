#include "../../../ast.hpp"
void ast::IntType::print_type(std::stringstream &stream) const {
    stream << "{\n\"type_ast\":\"IntType\",";    //{"type_ast":"IntType",
    stream << "\n\"bits\":" << this->bits << "}"; //"bits": {BITS} }
}