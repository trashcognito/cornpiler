#include "../ast.hpp"

ast::Bitcast::Bitcast(Value *thing, Type *type) {
    this->thing = thing;
    this->type = type;
}
llvm::Value *ast::Bitcast::codegen() const {
    //TODO: maybe use another bitcast instruction type?
    return Builder->CreatePointerBitCastOrAddrSpaceCast(this->thing->codegen(), this->type->get_type());
}
const ast::Const *ast::Bitcast::to_const() const {
    auto const1 = this->thing->to_const();
    if (!const1) return nullptr;
    return new ConstBitcast(const1, this->type);
}