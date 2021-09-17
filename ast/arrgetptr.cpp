#include "../ast.hpp"

ast::Arrgetptr::Arrgetptr(Value *array, Value *index) {
    this->array = array;
    this->index = index;
}
llvm::Value *ast::Arrgetptr::codegen() const {
    auto arrval = this->array->codegen();
    auto idx = this->index->codegen();
    auto gepval = Builder->CreateGEP(arrval, std::vector<llvm::Value *>({ast::IntegerConst(0, 32).codegen(), idx}));
    //TODO: dont assume the underlying indexed type is an array
    auto elem_type = arrval->getType()->getPointerElementType();
    if (elem_type->getTypeID() == llvm::Type::TypeID::ArrayTyID) {
        return Builder->CreatePointerCast(gepval, elem_type->getArrayElementType()->getPointerTo());
    } else if (elem_type->getTypeID() == llvm::Type::TypeID::PointerTyID) {
        return Builder->CreatePointerCast(gepval, elem_type->getPointerElementType()->getPointerTo());
    } else {
        //Broken GEP
        throw elem_type;
    }
    
}