#include "../../ast.hpp"

void ast::Arrget::print_val(std::stringstream &stream) const {
    stream << "{\n\"ast\":\"Arrget\",";    //{"ast":"Arrget",
    stream << "\n\"array\":"; //"array":
    this->array->print_val(stream);
    stream << ",\n\"index\":";  //,"index":
    this->index->print_val(stream);
    stream << "}";  //}
}