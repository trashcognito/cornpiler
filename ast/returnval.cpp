#include "../ast.hpp"

ast::ReturnVal::ReturnVal(Value *val) {
    this->val = val;
}
llvm::Value *ast::ReturnVal::codegen() const {
    Builder->CreateRet(val->codegen());
    //hopefully fix Terminator found in the middle of a basic block!
    auto parent = Builder->GetInsertBlock()->getParent();
    auto nextblock = llvm::BasicBlock::Create(*TheContext, "", parent);
    Builder->SetInsertPoint(nextblock);
    //this block should be optimized out as the next bit of code is unreachable
    //never use the value of a return statement in anything else
    return llvm::PoisonValue::get(llvm::Type::getVoidTy(*TheContext));
}