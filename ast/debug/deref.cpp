#include "../../ast.hpp"

void ast::Deref::print_val(std::stringstream &stream) const {
    stream << "{\n\"ast\":\"Deref\",";    //{"ast":"Deref",
    stream << "\n\"p\":"; //"p":
    this->p->print_val(stream);
    stream << "}";  //}
}