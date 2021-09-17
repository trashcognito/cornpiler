#include "../../ast.hpp"

ast::ConstUnaryOp::ConstUnaryOp(UOps operand, const Const *arg) {
    this->op = operand;
    this->arg = arg;
}
llvm::Constant *ast::ConstUnaryOp::codegen() const {
    auto arg1 = this->arg->codegen();
    switch(this->op) {
        case UOps::NOT:
            return llvm::ConstantExpr::getNot(arg1);
        case UOps::NEG:
            return llvm::ConstantExpr::getNeg(arg1);
    }
}