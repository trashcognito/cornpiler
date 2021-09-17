#include "../../../ast.hpp"

void ast::ConstBitcast::print_val(std::stringstream &stream) const {
    stream << "{\n\"ast\":\"ConstBitcast\",";    //{"ast":"ConstBitcast",
    stream << "\n\"thing\":";   //"thing":
    this->thing->print_val(stream);
    stream << ",\n\"type\":";   //,"type":
    this->type->print_type(stream);
    stream << "}";  //}
}