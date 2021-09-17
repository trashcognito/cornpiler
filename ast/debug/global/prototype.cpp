#include "../../../ast.hpp"

void ast::GlobalPrototype::print_global(std::stringstream &stream) const {
    stream << "{\n\"global_ast\":\"GlobalPrototype\",";    //{"global_ast":"GlobalPrototype",
    stream << "\n\"name\": \"" << this->name << "\",";  //"name": " {NAME} ",
    stream << "\n\"type\":";    //"type":
    this->type->print_type(stream);
    stream << ",\n\"constant\": " << (this->constant ? "true" : "false") << "}";   //,"constant": {CONSTANT} }
}