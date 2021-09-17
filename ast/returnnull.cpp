#include "../ast.hpp"

ast::ReturnNull::ReturnNull() {
    
}
llvm::Value *ast::ReturnNull::codegen() const {
    Builder->CreateRetVoid();
    auto parent = Builder->GetInsertBlock()->getParent();
    auto nextblock = llvm::BasicBlock::Create(*TheContext, "", parent);
    Builder->SetInsertPoint(nextblock);
    return llvm::PoisonValue::get(llvm::Type::getVoidTy(*TheContext));
}