#include "../../ast.hpp"
void ast::Vardef::print_val(std::stringstream &stream) const {
    stream << "{\n\"ast\":\"Vardef\",";    //{"ast":"Vardef",
    stream << "\n\"varname\":\"" << this->varname << "\",";   //"varname":"{VARNAME}",
    stream << "\n\"type\":"; //"type":
    this->ty->print_type(stream);
    stream << "}";
}