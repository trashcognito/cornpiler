#include "../../../ast.hpp"

void ast::ArrayType::print_type(std::stringstream &stream) const {
    stream << "{\n\"type_ast\":\"ArrayType\",";    //{"type_ast":"ArrayType",
    stream << "\n\"length\":" << this->length; //"length": {LENGTH}
    stream << ",\n\"inside\":";   //,"inside":
    this->inside->print_type(stream);
    stream << "}";
}