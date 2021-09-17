#include "../../../ast.hpp"

void ast::PointerType::print_type(std::stringstream &stream) const {
    stream << "{\n\"type_ast\":\"PointerType\",";    //{"type_ast":"PointerType",
    stream << "\n\"to\":"; //"to":
    this->to->print_type(stream);
    stream << "}";
}