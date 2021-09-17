#include "../ast.hpp"
ast::If::If(Body *body_if,Body *body_else, Value *condition) {
    this->body_t = body_if;
    this->body_f = body_else;
    this->condition = condition;
}
//TODO: Actually test this, it seems spaghetti
llvm::Value *ast::If::codegen() const {
    auto parent = Builder->GetInsertBlock()->getParent();
    auto if_prelude = llvm::BasicBlock::Create(*TheContext, "", parent);
    auto if_start = llvm::BasicBlock::Create(*TheContext, "", parent);
    auto if_body = llvm::BasicBlock::Create(*TheContext, "", parent);
    auto if_else = llvm::BasicBlock::Create(*TheContext, "", parent);
    auto if_end = llvm::BasicBlock::Create(*TheContext, "", parent);
    Builder->CreateBr(if_prelude);

    Builder->SetInsertPoint(if_start);
    Builder->CreateCondBr(condition->codegen(), if_body, if_else);
    //Create new result variable

    Builder->SetInsertPoint(if_body);
    auto res_t = body_t->codegen();
    auto br_t_inst = Builder->CreateBr(if_end);

    Builder->SetInsertPoint(if_else);
    auto res_f = body_f->codegen();
    auto br_f_inst = Builder->CreateBr(if_end);

    //create merge variable
    Builder->SetInsertPoint(if_prelude);
    auto ty_t = res_t->getType();
    auto ty_f = res_f->getType();
    llvm::Value *result;
    if (ty_t->getTypeID() != ty_f->getTypeID()) {
        llvm::errs() << "WARNING: Type mismatch: " << ty_t << " and " << ty_f << " are not compatible in if statement";
        result = llvm::PoisonValue::get(ty_t);
        Builder->CreateBr(if_start);
        Builder->SetInsertPoint(if_end);
    } else if (ty_t->isVoidTy()) {
        result = llvm::PoisonValue::get(ty_t);
        Builder->CreateBr(if_start);
        Builder->SetInsertPoint(if_end);
    } else {
        auto rvar = Builder->CreateAlloca(ty_t);
        Builder->CreateBr(if_start);
        Builder->SetInsertPoint(br_t_inst);
        Builder->CreateStore(res_t, rvar);
        Builder->SetInsertPoint(br_f_inst);
        Builder->CreateStore(res_f, rvar);
        Builder->SetInsertPoint(if_end);
        result = Builder->CreateLoad(rvar);
    }
    return result;
}