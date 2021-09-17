#include "../ast.hpp"

ast::Deref::Deref(Value *ptr) {
    this->p = ptr;
}
llvm::Value *ast::Deref::codegen() const {
    return Builder->CreateLoad(this->p->codegen());
}