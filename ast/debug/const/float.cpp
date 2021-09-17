#include "../../../ast.hpp"

void ast::FloatConst::print_val(std::stringstream &stream) const {
    stream << "{\n\"ast\":\"FloatConst\",";    //{"ast":"FloatConst",
    stream << "\n\"from\":" << this->from << "}";  //"from": {FROM}}
}