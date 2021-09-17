#include "../../ast.hpp"

void ast::Call::print_val(std::stringstream &stream) const {
    stream << "{\n\"ast\":\"Call\",";    //{"ast":"Call",
    stream << "\n\"function_name\":\"" << this->function_name << "\",";   //"function_name":"{FUNCTION_NAME}",
    stream << "\n\"argvector\": ["; //"argsvector": [
    bool is_first_print = true;
    for (auto thing : this->argvector) {
        if (is_first_print) {
            //omit first comma
            is_first_print = false;
        } else {
            stream << ",";
        }
        thing->print_val(stream);
    }
    stream << "]}"; // ]}
}