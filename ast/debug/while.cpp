#include "../../ast.hpp"

void ast::While::print_val(std::stringstream &stream) const {
    stream << "{\n\"ast\":\"While\",";    //{"ast":"While",
    stream << "\n\"condition\":"; //"condition":
    this->condition->print_val(stream);
    stream << ",\n\"body\":"; //,"body":
    this->body->print_val(stream);
    stream << "}";
}