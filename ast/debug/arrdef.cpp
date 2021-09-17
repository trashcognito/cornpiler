#include "../../ast.hpp"

void ast::ArrDef::print_val(std::stringstream &stream) const {
    stream << "{\n\"ast\":\"ArrDef\",";    //{"ast":"ArrDef",
    stream << "\n\"name\":\"" << this->name << "\",";   //"name":"{NAME}",
    stream << "\n\"inner\":"; //"inner":
    this->inner_type->print_type(stream);
    stream << ",\n\"length\":"; //,"length":
    this->length->print_val(stream);
    stream << "}";
}