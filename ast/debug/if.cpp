#include "../../ast.hpp"

void ast::If::print_val(std::stringstream &stream) const {
    stream << "{\n\"ast\":\"If\",";    //{"ast":"If",
    stream << "\n\"condition\":";    //"condition":
    this->condition->print_val(stream);
    stream << ",\n\"body_t\":"; //,"body_t":
    this->body_t->print_val(stream);
    stream << ",\n\"body_f\":"; //,"body_f":
    this->body_f->print_val(stream);
    stream << "}";
}