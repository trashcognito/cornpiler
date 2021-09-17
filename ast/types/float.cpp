#include "../../ast.hpp"

ast::FloatType::FloatType() {

}
llvm::Type *ast::FloatType::get_type() const {
    return llvm::Type::getFloatTy(*TheContext);
}