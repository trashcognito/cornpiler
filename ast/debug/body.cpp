#include "../../ast.hpp"

void ast::Body::print_val(std::stringstream &stream) const {
    stream << "{\n\"ast\":\"Body\",";    //{"ast":"Body",
    stream << "\n\"inner\": ["; //"inner": [
    bool is_first_print = true;
    for (auto item : this->body) {
        if (is_first_print) {
            //dont print , for first element
            is_first_print = false;
        } else {
            stream << ",";
        }
        item->print_val(stream);
    }
    stream << "]}";
}