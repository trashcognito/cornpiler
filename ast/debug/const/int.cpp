#include "../../../ast.hpp"

void ast::IntegerConst::print_val(std::stringstream &stream) const {
    stream << "{\n\"ast\":\"IntegerConst\",";    //{"ast":"IntegerConst",
    stream << "\n\"from\":" << this->from << ",";  //"from": {FROM},
    stream << "\n\"bits\":" << this->bits << "}"; //"bits": {BITS} }
}