#include "../ast.hpp"

std::vector<std::map<std::string, llvm::AllocaInst *>> LocalScope;

const ast::Const* ast::Value::to_const() const {
    return dynamic_cast<const Const *>(this);
}

llvm::Value *ast::resolve_var_scope(std::string key) {
    for (auto it = LocalScope.rbegin(); it != LocalScope.rend(); ++it) {
        if (it->count(key)) {
            auto val = it->at(key);
            //auto type = val->getAllocatedType();
            //return Builder->CreateLoad(type, val);
            return val;
        }
    }
    auto glb = TheModule->getNamedGlobal(key); //TheModule->getGlobalVariable(key);
    if (glb) {
        //TODO: Global variables are probably broken, global variables are most likely going to get pointerized
        return glb;
    }
    //TODO: Proper error message
    
    llvm::errs() << "Could not resolve symbol \"" << key << "\"" << "\n";
    throw key;
}