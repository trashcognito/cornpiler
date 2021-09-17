#include "../../ast.hpp"
ast::ArrayConst::ArrayConst(Type *subtype, std::vector<Const *> from) {
    this->getfrom = from;
    this->t = subtype;
}
llvm::Constant *ast::ArrayConst::codegen() const {
    std::vector<llvm::Constant *> init_array;
    for (auto it=this->getfrom.begin(); it != this->getfrom.end(); it++) {
        init_array.push_back((*it)->codegen());
    }
    auto constval = llvm::ConstantArray::get(llvm::ArrayType::get(this->t->get_type(), this->getfrom.size()), init_array);
    //auto globconst = new llvm::GlobalVariable(*TheModule, constval->getType(), true, llvm::GlobalValue::PrivateLinkage, constval);
    //return globconst;
    return constval;
};