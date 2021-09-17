#include "../ast.hpp"

ast::GetVar::GetVar(std::string name) {
    this->var_name = name;
}

llvm::Value *ast::GetVar::codegen() const {
    return Builder->CreateLoad(resolve_var_scope(var_name));
}