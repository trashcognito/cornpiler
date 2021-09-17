#include "../../../ast.hpp"

void ast::GlobalVariable::print_global(std::stringstream &stream) const {
    stream << "{\n\"global_ast\":\"GlobalVariable\",";    //{"global_ast":"GlobalVariable",
    stream << "\n\"name\": \"" << this->name << "\",";  //"name": " {NAME} ",
    stream << "\n\"value\":";    //"value":
    this->value->print_val(stream);
    stream << ",\n\"constant\": " << (this->constant ? "true" : "false") << "}";   //,"constant": {CONSTANT} }
}