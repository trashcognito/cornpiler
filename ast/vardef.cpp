#include "../ast.hpp"
llvm::Value *ast::Vardef::codegen() const {
    auto type = ty->get_type();
    auto var_storage = Builder->CreateAlloca(type);
    LocalScope.back()[varname] = var_storage;
    return llvm::PoisonValue::get(type);
}
ast::Vardef::Vardef(std::string varname, Type *type) {
    this->ty = type;
    this->varname = varname;
}