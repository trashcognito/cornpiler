#include "../ast.hpp"
ast::While::While(Body *body_a, Value *condition) {
    this->body = body_a;
    this->condition = condition;
}
llvm::Value *ast::While::codegen() const {
    auto parent = Builder->GetInsertBlock()->getParent();
    auto while_prelude = llvm::BasicBlock::Create(*TheContext, "", parent);
    auto while_start = llvm::BasicBlock::Create(*TheContext, "", parent);
    auto while_body = llvm::BasicBlock::Create(*TheContext, "", parent);
    auto while_end = llvm::BasicBlock::Create(*TheContext, "", parent);
    Builder->CreateBr(while_prelude);

    Builder->SetInsertPoint(while_start);
    Builder->CreateCondBr(condition->codegen(), while_body, while_end);
    Builder->SetInsertPoint(while_body);
    auto res = body->codegen();

    if (!res->getType()->isVoidTy()) {
        auto storeres_prelude = Builder->CreateBr(while_start);

        Builder->SetInsertPoint(while_prelude);
        auto resvar = Builder->CreateAlloca(res->getType());
        Builder->CreateBr(while_start);

        Builder->SetInsertPoint(storeres_prelude);
        Builder->CreateStore(res, resvar);


        Builder->SetInsertPoint(while_end);
        return Builder->CreateLoad(resvar);
    } else {
        Builder->CreateBr(while_start);

        Builder->SetInsertPoint(while_prelude);
        Builder->CreateBr(while_start);

        Builder->SetInsertPoint(while_end);
        return llvm::PoisonValue::get(res->getType());
    }
    
}