#include "../../ast.hpp"

ast::VoidType::VoidType() {

};
llvm::Type *ast::VoidType::get_type() const {
    return llvm::Type::getVoidTy(*TheContext);
}