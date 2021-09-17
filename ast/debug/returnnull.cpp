#include "../../ast.hpp"

void ast::ReturnNull::print_val(std::stringstream &stream) const {
    stream << "{\n\"ast\":\"ReturnNull\"}";    //{"ast":"ReturnNull"}
}