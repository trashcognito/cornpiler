#include "../ast.hpp"

ast::ArrDef::ArrDef(std::string name, Type *inner, Value *len) {
    this->name = name;
    this->inner_type = inner;
    this->length = len;
}
llvm::Value *ast::ArrDef::codegen() const {
    auto type = this->inner_type->get_type();
    
    auto var_storage = Builder->CreateAlloca(type, this->length->codegen());
    LocalScope.back()[this->name] = var_storage;
    return llvm::PoisonValue::get(type);
}