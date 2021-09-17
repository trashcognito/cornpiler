#include "../ast.hpp"

ast::Arrset::Arrset(Value *array, Value *index, Value *val) {
    this->array = array;
    this->index = index;
    this->val = val;
}

llvm::Value *ast::Arrset::codegen() const {
    auto the_pointer = new Arrgetptr(this->array, this->index);
    auto value = this->val->codegen();
    Builder->CreateStore(value, the_pointer->codegen());
    return value;
}