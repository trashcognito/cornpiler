#include "../../ast.hpp"
void ast::ReturnVal::print_val(std::stringstream &stream) const {
    stream << "{\n\"ast\":\"ReturnVal\",";    //{"ast":"ReturnVal",
    stream << "\n\"value\":"; //"value":
    this->val->print_val(stream);
    stream << "}";
}