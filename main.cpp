#include <iostream>
#include <llvm-12/llvm/ADT/ArrayRef.h>
#include <llvm-12/llvm/IR/BasicBlock.h>
#include <llvm-12/llvm/IR/Constant.h>
#include <llvm-12/llvm/IR/DerivedTypes.h>
#include <llvm-12/llvm/IR/InstrTypes.h>
#include <llvm-12/llvm/IR/Instructions.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/GlobalObject.h>
#include <llvm/IR/GlobalValue.h>
#include <llvm/IR/GlobalVariable.h>
#include <vector>
static llvm::LLVMContext TheContext;
static llvm::IRBuilder<> Builder(TheContext);
static std::unique_ptr<llvm::Module> TheModule;
class ASTValue;
static std::vector<std::map<std::string, llvm::AllocaInst *>> LocalScope;
static std::map<std::string, llvm::GlobalObject> GlobalScope;

class ASTBase {
    public:
    virtual void codegen();
    //virtual ~ASTBase();
};

class ASTValue {
    public:
    virtual llvm::Value *codegen();
    //virtual ~ASTValue();
};
class ASTBaseVardef : ASTBase {
    public:
    std::string varname;
    ASTValue val;
    virtual void codegen() {
        auto local = LocalScope.front();
        auto value = val.codegen();
        auto var_storage = Builder.CreateAlloca(value->getType());
        Builder.CreateStore(value, var_storage);
        local[varname] = var_storage;
    }
    ASTBaseVardef(std::string varname, ASTValue val) {
        this->varname = varname;
        this->val = val;
    }
};
class ASTBaseCall : ASTBase {
    public:
    std::string function_name;
    std::vector<ASTValue> argvector;
    virtual void codegen() {
        auto type = GlobalScope[function_name].getValueType();
        std::vector<llvm::Value *> args;
        for (std::vector<ASTValue>::iterator i = argvector.begin(); i != argvector.end(); ++i) {
            args.push_back(i->codegen());
        }
        Builder.CreateCall((llvm::FunctionType *) type, (llvm::Value *)&GlobalScope[function_name], args);
    }
    ASTBaseCall(std::string name, std::vector<ASTValue> args) {
        this->function_name = name;
        this->argvector = args;
    }
};
llvm::Value *resolve_var_scope(std::string key) {
    for (std::vector<std::map<std::string, llvm::AllocaInst *>>::reverse_iterator it = LocalScope.rbegin(); it != LocalScope.rend(); ++it) {
        if (it->count(key)) {
            auto val = it->at(key);
            //auto type = val->getAllocatedType();
            //return Builder.CreateLoad(type, val);
            return val;
        }
    }
    if (GlobalScope.count(key)) {
        //TODO: Global variables are probably broken, global variables are most likely going to get pointerized
        return &GlobalScope[key];
    }
    //TODO: Proper error message
    throw key;
}
class ASTBaseVarset : ASTBase {
    public:
    std::string name;
    ASTValue val;
    virtual void codegen() {
        auto location = resolve_var_scope(name);
        Builder.CreateStore(val.codegen(), location);
    }
    ASTBaseVarset(std::string name, ASTValue val) {
        this->name = name;
        this->val = val;
    }
};
class ASTBody : ASTBase {
    public:
    std::vector<ASTBase> body;
    virtual void codegen() {
        for (auto it = body.begin(); it != body.end(); ++it) {
            it->codegen();
        }
    }
    ASTBody(std::vector<ASTBase> body) {
        this->body = body;
    }
};
class ASTWhile : ASTBase {
    public:
    ASTBody body;
    ASTValue condition;
    virtual void codegen() {
        auto while_start = llvm::BasicBlock::Create(TheContext);
        auto while_body = llvm::BasicBlock::Create(TheContext);
        auto while_end = llvm::BasicBlock::Create(TheContext);
        Builder.CreateBr(while_start);
        Builder.SetInsertPoint(while_start);
        Builder.CreateCondBr(condition.codegen(), while_body, while_end);
        Builder.SetInsertPoint(while_body);
        body.codegen();
        Builder.CreateBr(while_start);
        Builder.SetInsertPoint(while_end);
    };
    ASTWhile(ASTBody body_a, ASTValue condition) : body(body_a) {
        this->condition = condition;
    }
};
class ASTIf : ASTBase {
    public:
    ASTBody body_t;
    ASTBody body_f;
    ASTValue condition;
    virtual void codegen() {
        auto if_start = llvm::BasicBlock::Create(TheContext);
        auto if_body = llvm::BasicBlock::Create(TheContext);
        auto if_else = llvm::BasicBlock::Create(TheContext);
        auto if_end = llvm::BasicBlock::Create(TheContext);
        Builder.CreateBr(if_start);
        Builder.SetInsertPoint(if_start);
        Builder.CreateCondBr(condition.codegen(), if_body, if_else);

        Builder.SetInsertPoint(if_body);
        body_t.codegen();
        Builder.CreateBr(if_end);

        Builder.SetInsertPoint(if_else);
        body_f.codegen();
        Builder.CreateBr(if_end);

        Builder.SetInsertPoint(if_end);
    }
    ASTIf(ASTBody body_if,ASTBody body_else, ASTValue condition) : body_t(body_if), body_f(body_else) {
        this->condition = condition;
    }
};
class ASTReturnVal : ASTBase {
    public:
    ASTValue val;
    virtual void codegen() {
        Builder.CreateRet(val.codegen());
    }
    ASTReturnVal(ASTValue val) {
        this->val = val;
    }
};
class ASTReturnNull : ASTBase {
    public:
    virtual void codegen() {
        Builder.CreateRetVoid();
    }
};
class ASTConst : ASTValue {
    public:
    llvm::Constant *c;
    virtual llvm::Value *codegen() {
        return c;
    }
    ASTConst(llvm::Constant *constant) {
        this->c = constant;
    }
};
class ASTOperand : ASTValue {
    public:
    enum Operand {
        LT,
        GT,
        LE,
        GE,
        ADD,
        SUB,
        DIV,
        MUL,
        MOD,
        BITAND,
        BITOR,
        XOR,
        EQ,
        NEQ,
        BOOL_OR,
        BOOL_AND
    };
    Operand op;
    ASTValue arg1;
    ASTValue arg2;
    virtual llvm::Value *codegen() {
        auto val1 = arg1.codegen();
        auto val2 = arg2.codegen();
        bool float_op = false;
        //strict type checking
        //if (val1->getType()->isFloatingPointTy()) {
        //    if (val2->getType()->isFloatingPointTy()) {
        //        float_op = true;
        //    } else {
        //        throw "Float and int ops are incompatible!";
        //    }
        //} else if (val2->getType()->isFloatingPointTy()) {
        //    throw "Float and int ops are incompatible!";
        //}
        if (val1->getType()->isFloatingPointTy() || val1->getType()->isFloatingPointTy()) {
            float_op = true;
        }
        if (float_op) {
            switch(op) {
                case LT:
                    return Builder.CreateFCmp(llvm::CmpInst::Predicate::FCMP_OLT, val1, val2);
                break;
                case GT:
                    return Builder.CreateFCmp(llvm::CmpInst::Predicate::FCMP_OGT, val1, val2);
                break;
                case LE:
                    return Builder.CreateFCmp(llvm::CmpInst::Predicate::FCMP_OLE, val1, val2);
                break;
                case GE:
                    return Builder.CreateFCmp(llvm::CmpInst::Predicate::FCMP_OGE, val1, val2);
                break;
                case ADD:
                    return Builder.CreateFAdd(val1, val2);
                break;
                case SUB:
                    return Builder.CreateFSub(val1, val2);
                break;
                case DIV:
                    return Builder.CreateFDiv(val1, val2);
                break;
                case MUL:
                    return Builder.CreateFMul(val1, val2);
                break;
                case MOD:
                    throw "Cannot create floating point modulo!";
                    //return Builder.CreateSub(val1, 
                    //    Builder.CreateMul(val1, 
                    //        Builder.CreateSDiv(val1, val2)
                    //    )
                    //);
                break;
                case BITAND:
                    return Builder.CreateAnd(val1, val2);
                break;
                case BITOR:
                    return Builder.CreateOr(val1, val2);
                break;
                case XOR:
                    return Builder.CreateXor(val1, val2);
                break;
                case EQ:
                    return Builder.CreateFCmp(llvm::CmpInst::Predicate::FCMP_OEQ, val1, val2);
                break;
                case NEQ:
                    return Builder.CreateFCmp(llvm::CmpInst::Predicate::FCMP_ONE, val1, val2);
                break;
                case BOOL_OR:
                    return Builder.CreateOr(
                        Builder.CreateBitCast(val1, llvm::IntegerType::get(TheContext, 1)), 
                        Builder.CreateBitCast(val2, llvm::IntegerType::get(TheContext, 1))
                        );
                break;
                case BOOL_AND:
                        return Builder.CreateAnd(
                        Builder.CreateBitCast(val1, llvm::IntegerType::get(TheContext, 1)), 
                        Builder.CreateBitCast(val2, llvm::IntegerType::get(TheContext, 1))
                        );
                break;
            }
        } else {
            //integer operations
            switch(op) {
                case LT:
                    return Builder.CreateCmp(llvm::CmpInst::Predicate::ICMP_SLT, val1, val2);
                break;
                case GT:
                    return Builder.CreateCmp(llvm::CmpInst::Predicate::ICMP_SGT, val1, val2);
                break;
                case LE:
                    return Builder.CreateCmp(llvm::CmpInst::Predicate::ICMP_SLE, val1, val2);
                break;
                case GE:
                    return Builder.CreateCmp(llvm::CmpInst::Predicate::ICMP_SGE, val1, val2);
                break;
                case ADD:
                    return Builder.CreateAdd(val1, val2);
                break;
                case SUB:
                    return Builder.CreateSub(val1, val2);
                break;
                case DIV:
                    return Builder.CreateSDiv(val1, val2);
                break;
                case MUL:
                    return Builder.CreateMul(val1, val2);
                break;
                case MOD:
                    return Builder.CreateSub(val1, 
                        Builder.CreateMul(val1, 
                            Builder.CreateSDiv(val1, val2)
                        )
                    );
                break;
                case BITAND:
                    return Builder.CreateAnd(val1, val2);
                break;
                case BITOR:
                    return Builder.CreateOr(val1, val2);
                break;
                case XOR:
                    return Builder.CreateXor(val1, val2);
                break;
                case EQ:
                    return Builder.CreateCmp(llvm::CmpInst::Predicate::ICMP_EQ, val1, val2);
                break;
                case NEQ:
                    return Builder.CreateCmp(llvm::CmpInst::Predicate::ICMP_NE, val1, val2);
                break;
                case BOOL_OR:
                    return Builder.CreateOr(
                        Builder.CreateBitCast(val1, llvm::IntegerType::get(TheContext, 1)), 
                        Builder.CreateBitCast(val2, llvm::IntegerType::get(TheContext, 1))
                        );
                break;
                case BOOL_AND:
                        return Builder.CreateAnd(
                        Builder.CreateBitCast(val1, llvm::IntegerType::get(TheContext, 1)), 
                        Builder.CreateBitCast(val2, llvm::IntegerType::get(TheContext, 1))
                        );
                break;
            }
        }
    }
    ASTOperand(ASTValue lhs, ASTValue rhs, Operand op) {
        this->arg1 = lhs;
        this->arg2 = rhs;
        this->op = op;
    }
};
class ASTValueCall : ASTValue {
    public:
    std::string function_name;
    std::vector<ASTValue> argvector;
    virtual llvm::Value *codegen() {
        //TODO: implement calling function from local scope?
        auto type = GlobalScope[function_name].getValueType();
        std::vector<llvm::Value *> args;
        for (std::vector<ASTValue>::iterator i = argvector.begin(); i != argvector.end(); ++i) {
            args.push_back(i->codegen());
        }
        return Builder.CreateCall((llvm::FunctionType *) type, (llvm::Value *)&GlobalScope[function_name], args);
    }
    ASTValueCall(std::string name, std::vector<ASTValue> args) {
        this->function_name = name;
        this->argvector = args;
    }
};
class ASTGetVar : ASTValue {
    public:
    std::string var_name;
    virtual llvm::Value *codegen() {
        return Builder.CreateLoad(resolve_var_scope(var_name));
    }
    ASTGetVar(std::string name) {
        this->var_name = name;
    }
};
class ASTGetVarPtr : ASTValue {
    public:
    std::string var_name;
    virtual llvm::Value *codegen() {
        return resolve_var_scope(var_name);
    }
    ASTGetVarPtr(std::string name) {
        this->var_name = name;
    }
};
class ASTUnaryOp : ASTValue {
    public:
    ASTValue arg;
    enum UOps {
        NOT
    };
    UOps op;
    virtual llvm::Value *codegen() {
        auto val = arg.codegen();
        switch (op) {
            case NOT:
                return Builder.CreateNot(val);
            break;
        }
    }
};
int main(int argc, char *argv[]) {
    
}
