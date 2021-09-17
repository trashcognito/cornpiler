#include "../ast.hpp"

ast::Arrget::Arrget(Value *array, Value *index) {
    this->array = array;
    this->index = index;
}
llvm::Value *ast::Arrget::codegen() const {
    auto the_pointer = new Arrgetptr(this->array, this->index);
    return Builder->CreateLoad(the_pointer->codegen());
}