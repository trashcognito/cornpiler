#include "../../ast.hpp"

void ast::Bitcast::print_val(std::stringstream &stream) const {
    stream << "{\n\"ast\":\"Bitcast\",";    //{"ast":"Bitcast",
    stream << "\n\"thing\":";   //"thing":
    this->thing->print_val(stream);
    stream << ",\n\"type\":";   //,"type":
    this->type->print_type(stream);
    stream << "}";  //}
}