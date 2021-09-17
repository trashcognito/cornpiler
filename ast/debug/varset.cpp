#include "../../ast.hpp"
void ast::Varset::print_val(std::stringstream &stream) const {
    stream << "{\n\"ast\":\"Varset\",";    //{"ast":"Varset",
    stream << "\n\"name\":\"" << this->name << "\",";   //"name":"{NAME}",
    stream << "\n\"value\":"; //"value":
    this->val->print_val(stream);
    stream << "}";
}