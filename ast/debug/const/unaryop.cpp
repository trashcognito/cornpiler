#include "../../../ast.hpp"

void ast::ConstUnaryOp::print_val(std::stringstream &stream) const {
    stream << "{\n\"ast\":\"ConstUnaryOp\",";    //{"ast":"ConstUnaryOp",
    stream << "\n\"op\":\"UOps::";    //"op":"UOps::
    switch (this->op) {
        case UOps::NOT:
            stream << "NOT";
            break;
        case UOps::NEG:
            stream << "NEG";
            break;
    }

    stream << "\",\"arg\":";    //","arg":
    this->arg->print_val(stream);
    stream << "}";
}