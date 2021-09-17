#include "../../ast.hpp"

ast::GlobalVariable::GlobalVariable(std::string name_a, const Const *value, bool is_const) {
    this->name = name_a;
    this->value = value;
    this->constant = is_const;
}
void ast::GlobalVariable::codegen() const {
    auto val = this->value->codegen();
    auto the_var = new llvm::GlobalVariable(*TheModule, val->getType(), this->constant, llvm::GlobalObject::ExternalLinkage, val, this->name);
}