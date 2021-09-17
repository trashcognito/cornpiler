#include "../../../ast.hpp"

void ast::StringType::print_type(std::stringstream &stream) const {
    stream << "{\n\"type_ast\":\"StringType\",";    //{"type_ast":"StringType",
    stream << "\n\"length\":" << this->length << "}"; //"length": {LENGTH} }
}