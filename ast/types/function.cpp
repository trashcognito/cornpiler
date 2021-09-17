#include "../../ast.hpp"
ast::FunctionType::FunctionType(std::string name, std::vector<Type *> args, Type *return_type, bool varargs) {
    this->name = name;
    this->args = args;
    this->varargs = varargs;
    this->return_type = return_type;
}
llvm::Type *ast::FunctionType::get_type() const {
    std::vector<llvm::Type *> arg_types;
    for (auto it=this->args.begin(); it != this->args.end(); it++) {
        arg_types.push_back((*it)->get_type());
    }
    return llvm::FunctionType::get(this->return_type->get_type(), arg_types, this->varargs);
}