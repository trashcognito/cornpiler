#include "../../ast.hpp"

ast::GlobalFunction::GlobalFunction(FunctionType *t, Body *body, std::vector<std::string> args) {
    this->type = t;
    this->body = body;
    this->args = args;
    this->name = t->name;
};
void ast::GlobalFunction::codegen() const {
    auto prototype = TheModule->getFunction(name);
    if (!prototype) {
        //TODO: the static cast here may break stuff and segfault, simply split away the function declaration if this happens
        prototype = llvm::Function::Create(static_cast<llvm::FunctionType *>(type->get_type()), llvm::GlobalValue::ExternalLinkage, name, *TheModule);
    }
    llvm::BasicBlock *BB = llvm::BasicBlock::Create(*TheContext, name, prototype);
    Builder->SetInsertPoint(BB);
    int arg_offset = 0;
    std::map<std::string, llvm::AllocaInst *> local;
    for (auto arg = prototype->arg_begin(); arg != prototype->arg_end(); arg++) {
        auto alloca = Builder->CreateAlloca(arg->getType());
        Builder->CreateStore(arg, alloca);
        local[args[arg_offset]] = alloca;
        arg_offset++;
    }
    LocalScope.push_back(local);
    body->codegen();
    //Automatically add return instruction
    auto ret_type = prototype->getReturnType();
    if (ret_type->isVoidTy()) {
        Builder->CreateRetVoid();
    } else {
        auto ret_const = llvm::Constant::getNullValue(ret_type);
        Builder->CreateRet(ret_const);
    }
    LocalScope.pop_back();
}