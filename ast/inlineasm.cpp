#include "../ast.hpp"

llvm::Value *ast::InlineAsm::codegen() const {
    std::vector<llvm::Value *> arg_list;
    for (auto arg : this->args) {
        arg_list.push_back(arg->codegen());
    }
    std::vector<llvm::Type *> type_list;
    for (auto val : arg_list) {
        type_list.push_back(val->getType());
    }
    //TODO: maybe support arbitrary asm syntax?
    auto asm_func =  llvm::InlineAsm::get(
        llvm::FunctionType::get(
            llvm::Type::getVoidTy(*TheContext),
            type_list,
            false
        ),
        this->asmstring,
        this->constraints,
        this->is_volatile,
        this->is_align_stack
    );
    return Builder->CreateCall(asm_func, arg_list, "inline_asm");
}
ast::InlineAsm::InlineAsm(std::string assembly, std::string constraints, std::vector<Value *> args, bool is_volatile, bool is_align_stack) {
    this->asmstring = assembly;
    this->args = args;
    this->constraints = constraints;
    this->is_align_stack = is_align_stack;
    this->is_volatile = is_volatile;
}