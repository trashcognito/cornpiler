#include "../../ast.hpp"

ast::IntegerConst::IntegerConst(intmax_t i, int bits) {
    this->from = i;
    this->bits = bits;
}
llvm::Constant *ast::IntegerConst::codegen() const {
    return llvm::ConstantInt::get(*TheContext, llvm::APInt(this->bits,this->from));
}