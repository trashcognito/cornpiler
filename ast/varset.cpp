#include "../ast.hpp"
ast::Varset::Varset(std::string name, Value *val) {
    this->val = val;
    this->name = name;
}
llvm::Value *ast::Varset::codegen() const {
    auto location = resolve_var_scope(name);
    auto value = val->codegen();
    Builder->CreateStore(value, location);
    return value;
}