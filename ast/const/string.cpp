#include "../../ast.hpp"

ast::StringConst::StringConst(std::string from) {
    this->orig = from;
}
llvm::Constant *ast::StringConst::codegen() const {
    std::vector<llvm::Constant *> string_array;
    for (auto it=this->orig.begin(); it != this->orig.end(); it++) {
        string_array.push_back(llvm::ConstantInt::get(*TheContext, llvm::APInt(8, *it)));
    }
    //add null byte
    string_array.push_back(llvm::ConstantInt::get(*TheContext, llvm::APInt(8, 0)));
    //TODO: static c here might be a horrible horrible idea that will break the program, unbreak this if the string is borked
    auto constval = llvm::ConstantArray::get(llvm::ArrayType::get(llvm::IntegerType::get(*TheContext, 8), string_array.size()), string_array);
    auto globconst = new llvm::GlobalVariable(*TheModule, constval->getType(), true, llvm::GlobalValue::PrivateLinkage, constval);
    return globconst;
}