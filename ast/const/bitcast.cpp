#include "../../ast.hpp"

ast::ConstBitcast::ConstBitcast(const Const *thing, Type *type) {
    this->thing = thing;
    this->type = type;
}
llvm::Constant *ast::ConstBitcast::codegen() const {
    return llvm::ConstantExpr::getPointerBitCastOrAddrSpaceCast(this->thing->codegen(), this->type->get_type());
}