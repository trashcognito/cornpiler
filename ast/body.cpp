#include "../ast.hpp"
ast::Body::Body(std::vector<Value *> body) {
    this->body = body;
}
llvm::Value *ast::Body::codegen() const {
    //Creates a new local scope
    LocalScope.emplace(LocalScope.end());
    //TODO: test if this works somehow
    llvm::Value *result;
    result = llvm::PoisonValue::get(llvm::Type::getVoidTy(*TheContext));
    for (auto it = body.begin(); it != body.end(); ++it) {
        result = (*it)->codegen();
    }
    LocalScope.pop_back();
    return result;
}