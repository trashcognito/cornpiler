#include "../ast.hpp"

ast::GetVarPtr::GetVarPtr(std::string name) {
    this->var_name = name;
}

llvm::Value *ast::GetVarPtr::codegen() const {
    return resolve_var_scope(var_name);
}