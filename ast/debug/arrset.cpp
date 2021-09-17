#include "../../ast.hpp"

void ast::Arrset::print_val(std::stringstream &stream) const {
    stream << "{\n\"ast\":\"Arrset\",";    //{"ast":"Arrset",
    stream << "\n\"array\":"; //"array":
    this->array->print_val(stream);
    stream << ",\n\"index\":";  //,"index":
    this->index->print_val(stream);
    stream << ",\n\"val\":";  //,"val":
    this->val->print_val(stream);
    stream << "}";  //}
}