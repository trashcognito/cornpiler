#include "../../ast.hpp"

void ast::Arrgetptr::print_val(std::stringstream &stream) const {
    stream << "{\n\"ast\":\"Arrgetptr\",";    //{"ast":"Arrgetptr",
    stream << "\n\"array\":"; //"array":
    this->array->print_val(stream);
    stream << ",\n\"index\":";  //,"index":
    this->index->print_val(stream);
    stream << "}";  //}
}