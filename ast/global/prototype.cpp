#include "../../ast.hpp"

ast::GlobalPrototype::GlobalPrototype(std::string name, Type *type, bool is_constant) {
    this->name = name;
    this->constant = is_constant;
    this->type = type;
}

void ast::GlobalPrototype::codegen() const {
    auto t = this->type->get_type();
    if (t->isFunctionTy()) {
        auto f = llvm::Function::Create(static_cast<llvm::FunctionType *>(t), llvm::GlobalValue::ExternalLinkage, name, *TheModule);
    } else {
        //extern simple value
        //TODO: add address space stuff
        new llvm::GlobalVariable(*TheModule,t,this->constant, llvm::GlobalValue::ExternalLinkage, nullptr, name);
    }
}