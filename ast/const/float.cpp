#include "../../ast.hpp"
ast::FloatConst::FloatConst(float f) {
    this->from = f;
}
llvm::Constant *ast::FloatConst::codegen() const {
    return llvm::ConstantFP::get(*TheContext, llvm::APFloat(this->from));
}