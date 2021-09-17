#include "../../ast.hpp"

void ast::GetVarPtr::print_val(std::stringstream &stream) const {
    stream << "{\n\"ast\":\"GetVarPtr\",";    //{"ast":"GetVarPtr",
    stream << "\n\"var_name\":\"" << this->var_name << "\"}";   //"var_name":"{NAME} }"
}