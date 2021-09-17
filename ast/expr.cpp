#include "../ast.hpp"

ast::Expr::Expr(Value *inner) {
    this->actual = inner;
}
llvm::Value *ast::Expr::codegen() const {
    return this->actual->codegen();
}
const ast::Const *ast::Expr::to_const() const {
    return this->actual->to_const();
}