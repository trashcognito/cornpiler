#include "../../ast.hpp"

ast::StringType::StringType(int length) {
    this->length = length;
};

llvm::Type * ast::StringType::get_type() const {
    return llvm::ArrayType::get(
        llvm::IntegerType::getInt8Ty(*TheContext),
        this->length
    );
}