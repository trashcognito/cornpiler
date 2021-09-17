#include "../../ast.hpp"

void ast::Expr::print_val(std::stringstream &stream) const {
    stream << "{\n\"ast\":\"Expr\",";    //{"ast":"Expr",
    stream << "\n\"actual\":"; //"actual":
    this->actual->print_val(stream);
    stream << "}";  //}
}