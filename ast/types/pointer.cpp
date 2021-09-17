#include "../../ast.hpp"

ast::PointerType::PointerType(Type *to) {
    this->to = to;
}

llvm::Type *ast::PointerType::get_type() const {
    //TODO: do addrspace to const the pointer?
    return to->get_type()->getPointerTo();
}