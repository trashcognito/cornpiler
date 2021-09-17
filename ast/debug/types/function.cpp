#include "../../../ast.hpp"

void ast::FunctionType::print_type(std::stringstream &stream) const {
    stream << "{\n\"type_ast\":\"FunctionType\",";    //{"type_ast":"FunctionType",
    stream << "\n\"name\":\"" << this->name << "\",";   //"name":" {NAME} ",
    stream << "\n\"args\": [";    //"args": [
    bool is_first_print = true;
    for (auto thing : this->args) {
        if (is_first_print) {
            //dont print the first comma
            is_first_print = false;
        } else {
            stream << ",";
        }
        thing->print_type(stream);
    }
    stream << "],\n\"return_type\":";    //]"return_type":
    this->return_type->print_type(stream);
    stream << ",\n\"varargs\":" << (this->varargs ? "true" : "false") << "}";   //,"varargs": {VARARGS} }
}