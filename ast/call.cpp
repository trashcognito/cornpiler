#include "../ast.hpp"

ast::Call::Call(std::string name, ValueArray args) {
    this->function_name = name;
    this->argvector = args;
}

llvm::Value *ast::Call::codegen() const {
    //TODO: implement calling function from local scope?
    auto fun = TheModule->getFunction(function_name);
    if (!fun) throw std::string("Could not find function ")
        .append(function_name);
    std::vector<llvm::Value *> args;
    for (auto i = argvector.begin(); i != argvector.end(); ++i) {
        args.push_back((*i)->codegen());
    }
    return Builder->CreateCall(fun, args);
}