#include "../ast.hpp"

ast::UnaryOp::UnaryOp(UOps operand, Value *arg) {
    this->op = operand;
    this->arg = arg;
}

llvm::Value *ast::UnaryOp::codegen() const {
    auto val = arg->codegen();
    switch (op) {
        case UOps::NOT:
            return Builder->CreateNot(val);
        case UOps::NEG:
            return Builder->CreateNeg(val);
        break;
    }
}

const ast::Const *ast::UnaryOp::to_const() const {
    auto const1 = arg->to_const();
    if (!const1) return nullptr;
    return new ConstUnaryOp(this->op, const1);
}