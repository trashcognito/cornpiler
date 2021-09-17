#include "../../ast.hpp"

ast::IntType::IntType(int bits) {
    this->bits = bits;
}

llvm::Type * ast::IntType::get_type() const {
    return llvm::IntegerType::get(*TheContext, this->bits);
}