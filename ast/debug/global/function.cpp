#include "../../../ast.hpp"

void ast::GlobalFunction::print_global(std::stringstream &stream) const {
    stream << "{\n\"global_ast\":\"GlobalFunction\",";    //{"global_ast":"GlobalFunction",
    stream << "\n\"name\": \"" << this->name << "\",";  //"name": " {NAME} ",
    stream << "\n\"type\":";    //"type":
    this->type->print_type(stream);
    stream << ",\n\"body\":";   //,"body":
    this->body->print_val(stream);
    stream << ",\n\"args\": [";   //,"args": [
    bool is_first_print = true;
    for (auto thing : this->args) {
        if (is_first_print) {
            //omit first comma
            is_first_print = false;
        } else {
            stream << ",";
        }
        stream << "\"" << thing << "\"";
    }
    stream << "]}"; //]}
}