#include "../../ast.hpp"

ast::ArrayType::ArrayType(Type *inside, int len) {
    this->inside = inside;
    this->length = len;
}
llvm::Type *ast::ArrayType::get_type() const {
    return llvm::ArrayType::get(
        inside->get_type(),
        length
    );
}