#include "../../../ast.hpp"

void ast::ArrayConst::print_val(std::stringstream &stream) const {
    stream << "{\n\"ast\":\"ArrayConst\",";    //{"ast":"ArrayConst",
    stream << "\n\"subtype\":";  //"subtype":
    this->t->print_type(stream);
    stream << ",\n\"from\": [";   //,"from": [
    bool is_first_print = true;
    for (auto thing : this->getfrom) {
        if (is_first_print) {
            //omit first comma
            is_first_print = false;
        } else {
            stream << ",";
        }
        thing->print_val(stream);
    }
    stream << "]}"; //]}
}