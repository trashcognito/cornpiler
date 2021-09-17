#include "../../ast.hpp"

void ast::GetVar::print_val(std::stringstream &stream) const {
    stream << "{\n\"ast\":\"GetVar\",";    //{"ast":"GetVar",
    stream << "\n\"var_name\":\"" << this->var_name << "\"}";   //"var_name":"{NAME} }"
}