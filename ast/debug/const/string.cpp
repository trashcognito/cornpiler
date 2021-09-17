#include "../../../ast.hpp"

void ast::StringConst::print_val(std::stringstream &stream) const {
    stream << "{\n\"ast\":\"StringConst\",";    //{"ast":"StringConst",
    stream << "\n\"orig\": \"" << this->orig << "\" }";  //"orig": "{ORIG}"" }
}