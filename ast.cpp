#include <iostream>
#include <llvm-12/llvm/IR/Type.h>
#include <llvm-12/llvm/Support/raw_ostream.h>
#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/Verifier.h>
#include <memory>
#include <ostream>
#include <string>
#include <vector>

#include "ast.hpp"

static std::vector<std::map<std::string, llvm::AllocaInst *>> LocalScope;
namespace ast {
    void BaseVardef::codegen() const {
        auto value = val->codegen();
        auto var_storage = Builder->CreateAlloca(value->getType());
        Builder->CreateStore(value, var_storage);
        LocalScope.back()[varname] = var_storage;
    }
    BaseVardef::BaseVardef(std::string varname, Value *val) {
        this->val = val;
        this->varname = varname;
    }

    BaseCall::BaseCall(std::string name, ValueArray args) {
        this->function_name = name;
        this->argvector = args;
    }

    void BaseCall::codegen() const {
        auto fun = TheModule->getFunction(function_name);
        std::vector<llvm::Value *> args;
        for (auto i = argvector.begin(); i != argvector.end(); ++i) {
            args.push_back((*i)->codegen());
        }
        Builder->CreateCall(fun, args);
        
    }
    //************MAP DEBUGGING
    /*
    template <typename K, typename V>
    std::ostream& operator<<(std::ostream& os, const std::map<K, V>& m) {
        os << "{ ";
        for (typename std::map<K, V>::const_iterator i = m.begin(); i != m.end(); ++i)
        {
            if (i != m.begin()) os << ", ";
            os << i->first << ": " << i->second;
        }
        return os << " }";
    }
    */
    //************
    llvm::Value *resolve_var_scope(std::string key) {
        for (auto it = LocalScope.rbegin(); it != LocalScope.rend(); ++it) {
            if (it->count(key)) {
                auto val = it->at(key);
                //auto type = val->getAllocatedType();
                //return Builder->CreateLoad(type, val);
                return val;
            }
        }
        auto glb = TheModule->getGlobalVariable(key);
        if (glb) {
            //TODO: Global variables are probably broken, global variables are most likely going to get pointerized
            return glb;
        }
        //TODO: Proper error message
        throw key;
    }

    BaseVarset::BaseVarset(std::string name, Value *val) {
        this->val = val;
        this->name = name;
    }

    void BaseVarset::codegen() const {
        auto location = resolve_var_scope(name);
        Builder->CreateStore(val->codegen(), location);
    }

    Body::Body(std::vector<Base *> body) {
        this->body = body;
    }
    void Body::codegen() const {
        //Creates a new local scope
        LocalScope.emplace(LocalScope.end());
        for (auto it = body.begin(); it != body.end(); ++it) {
            (*it)->codegen();
        }
        LocalScope.pop_back();
    }

    While::While(Body *body_a, Value *condition) {
        this->body = body_a;
        this->condition = condition;
    }
    void While::codegen() const {
        auto parent = Builder->GetInsertBlock()->getParent();
        auto while_start = llvm::BasicBlock::Create(*TheContext, "", parent);
            auto while_body = llvm::BasicBlock::Create(*TheContext, "", parent);
            auto while_end = llvm::BasicBlock::Create(*TheContext, "", parent);
            Builder->CreateBr(while_start);
            Builder->SetInsertPoint(while_start);
            Builder->CreateCondBr(condition->codegen(), while_body, while_end);
            Builder->SetInsertPoint(while_body);
            body->codegen();
            Builder->CreateBr(while_start);
            Builder->SetInsertPoint(while_end);
    }

    If::If(Body *body_if,Body *body_else, Value *condition) {
        this->body_t = body_if;
        this->body_f = body_else;
        this->condition = condition;
    }
    void If::codegen() const {
        auto parent = Builder->GetInsertBlock()->getParent();
        auto if_start = llvm::BasicBlock::Create(*TheContext, "", parent);
        auto if_body = llvm::BasicBlock::Create(*TheContext, "", parent);
        auto if_else = llvm::BasicBlock::Create(*TheContext, "", parent);
        auto if_end = llvm::BasicBlock::Create(*TheContext, "", parent);
        Builder->CreateBr(if_start);
        Builder->SetInsertPoint(if_start);
        Builder->CreateCondBr(condition->codegen(), if_body, if_else);

        Builder->SetInsertPoint(if_body);
        body_t->codegen();
        Builder->CreateBr(if_end);

        Builder->SetInsertPoint(if_else);
        body_f->codegen();
        Builder->CreateBr(if_end);

        Builder->SetInsertPoint(if_end);
    }

    ReturnVal::ReturnVal(Value *val) {
        this->val = val;
    }
    void ReturnVal::codegen() const {
        Builder->CreateRet(val->codegen());
        //hopefully fix Terminator found in the middle of a basic block!
        auto parent = Builder->GetInsertBlock()->getParent();
        auto nextblock = llvm::BasicBlock::Create(*TheContext, "", parent);
        Builder->SetInsertPoint(nextblock);
        //this block should be optimized out as the next bit of code is unreachable
    }

    void ReturnNull::codegen() const {
        Builder->CreateRetVoid();
        auto parent = Builder->GetInsertBlock()->getParent();
        auto nextblock = llvm::BasicBlock::Create(*TheContext, "", parent);
        Builder->SetInsertPoint(nextblock);
    }

    IntType::IntType(int bits) {
        this->bits = bits;
    }

    llvm::Type * IntType::get_type() const {
        return llvm::IntegerType::get(*TheContext, this->bits);
    }

    StringType::StringType(int length) {
        this->length = length;
    };

    llvm::Type * StringType::get_type() const {
        return llvm::ArrayType::get(
            llvm::IntegerType::getInt8Ty(*TheContext),
            this->length
        );
    }

    ArrayType::ArrayType(Type *inside, int len) {
        this->inside = inside;
        this->length = len;
    }

    llvm::Type * ArrayType::get_type() const {
        return llvm::ArrayType::get(
            inside->get_type(),
            length
        );
    }
    VoidType::VoidType() {

    };
    llvm::Type * VoidType::get_type() const {
        return llvm::Type::getVoidTy(*TheContext);
    }

    PointerType::PointerType(Type *to) {
        this->to = to;
    }

    llvm::Type * PointerType::get_type() const {
        //TODO: do addrspace to const the pointer?
        return to->get_type()->getPointerTo();
    }

    FunctionType::FunctionType(std::string name, std::vector<Type *> args, Type *return_type, bool varargs) {
        this->name = name;
        this->args = args;
        this->varargs = varargs;
        this->return_type = return_type;
    }
    llvm::Type * FunctionType::get_type() const {
        std::vector<llvm::Type *> arg_types;
        for (auto it=this->args.begin(); it != this->args.end(); it++) {
            arg_types.push_back((*it)->get_type());
        }
        return llvm::FunctionType::get(this->return_type->get_type(), arg_types, this->varargs);
    }

    //TODO: split off Const to different types to offload the work to the lexer?

    Const::Const(Type *t, std::string container) {
        this->thing = container;
        this->type = t;
    }
    llvm::Value *Const::codegen() const {
        auto t = type->get_type();
        switch(t->getTypeID()) {
            //case llvm::Type::HalfTyID : 
            //case llvm::Type::BFloatTyID: 
            case llvm::Type::FloatTyID : 
                return llvm::ConstantFP::get(*TheContext, llvm::APFloat((float)std::stof(thing)));
            break;
            //case llvm::Type::DoubleTyID : 
            //case llvm::Type::X86_FP80TyID : 
            //case llvm::Type::FP128TyID : 
            //case llvm::Type::PPC_FP128TyID : 
            //case llvm::Type::VoidTyID : 
            //case llvm::Type::LabelTyID : 
            //case llvm::Type::MetadataTyID : 
            //case llvm::Type::X86_MMXTyID : 
            //case llvm::Type::X86_AMXTyID : 
            //case llvm::Type::TokenTyID : 
            case llvm::Type::IntegerTyID: 
                //parse integer
                //TODO: test if signed?
                return llvm::ConstantInt::get(*TheContext, llvm::APInt(t->getIntegerBitWidth(),std::stoi(thing)));
            break;
            //TODO: maybe resolve functions?
            //case llvm::Type::FunctionTyID : 
            //TODO: const pointers could be ints?
            //case llvm::Type::PointerTyID: 
            //case llvm::Type::StructTyID : 
            case llvm::Type::ArrayTyID: 
                //TODO: Assuming string here - this is bad
                {
                std::vector<llvm::Constant *> string_array;
                for (auto it=this->thing.begin(); it != this->thing.end(); it++) {
                    string_array.push_back(llvm::ConstantInt::get(*TheContext, llvm::APInt(t->getArrayElementType()->getIntegerBitWidth(), *it)));
                }
                //add null byte
                string_array.push_back(llvm::ConstantInt::get(*TheContext, llvm::APInt(t->getArrayElementType()->getIntegerBitWidth(), 0)));
                //TODO: static c here might be a horrible horrible idea that will break the program, unbreak this if the string is borked
                return llvm::ConstantArray::get(static_cast<llvm::ArrayType *>(t), string_array);
                }
            break;
            //case llvm::Type::FixedVectorTyID: 
            //case llvm::Type::ScalableVectorTyID : 
            default:
            llvm::errs() << "Invalid type: " << this->type->name;
            return nullptr;
        }
    }

    Operand::Operand(Value *lhs, Value *rhs, OperandType op) {
        this->arg1 = lhs;
        this->arg2 = rhs;
        this->op = op;
    }

    llvm::Value *Operand::codegen() const {
        auto val1 = arg1->codegen();
        auto val2 = arg2->codegen();
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
                    return Builder->CreateFCmp(llvm::CmpInst::Predicate::FCMP_OLT, val1, val2);
                break;
                case GT:
                    return Builder->CreateFCmp(llvm::CmpInst::Predicate::FCMP_OGT, val1, val2);
                break;
                case LE:
                    return Builder->CreateFCmp(llvm::CmpInst::Predicate::FCMP_OLE, val1, val2);
                break;
                case GE:
                    return Builder->CreateFCmp(llvm::CmpInst::Predicate::FCMP_OGE, val1, val2);
                break;
                case ADD:
                    return Builder->CreateFAdd(val1, val2);
                break;
                case SUB:
                    return Builder->CreateFSub(val1, val2);
                break;
                case DIV:
                    return Builder->CreateFDiv(val1, val2);
                break;
                case MUL:
                    return Builder->CreateFMul(val1, val2);
                break;
                case MOD:
                    throw "Cannot create floating point modulo!";
                    //return Builder->CreateSub(val1, 
                    //    Builder->CreateMul(val1, 
                    //        Builder->CreateSDiv(val1, val2)
                    //    )
                    //);
                break;
                case BITAND:
                    return Builder->CreateAnd(val1, val2);
                break;
                case BITOR:
                    return Builder->CreateOr(val1, val2);
                break;
                case XOR:
                    return Builder->CreateXor(val1, val2);
                break;
                case EQ:
                    return Builder->CreateFCmp(llvm::CmpInst::Predicate::FCMP_OEQ, val1, val2);
                break;
                case NEQ:
                    return Builder->CreateFCmp(llvm::CmpInst::Predicate::FCMP_ONE, val1, val2);
                break;
                case BOOL_OR:
                    return Builder->CreateOr(
                        Builder->CreateBitCast(val1, llvm::IntegerType::get(*TheContext, 1)), 
                        Builder->CreateBitCast(val2, llvm::IntegerType::get(*TheContext, 1))
                        );
                break;
                case BOOL_AND:
                        return Builder->CreateAnd(
                        Builder->CreateBitCast(val1, llvm::IntegerType::get(*TheContext, 1)), 
                        Builder->CreateBitCast(val2, llvm::IntegerType::get(*TheContext, 1))
                        );
                break;
            }
        } else {
            //integer operations
            switch(op) {
                case LT:
                    return Builder->CreateCmp(llvm::CmpInst::Predicate::ICMP_SLT, val1, val2);
                break;
                case GT:
                    return Builder->CreateCmp(llvm::CmpInst::Predicate::ICMP_SGT, val1, val2);
                break;
                case LE:
                    return Builder->CreateCmp(llvm::CmpInst::Predicate::ICMP_SLE, val1, val2);
                break;
                case GE:
                    return Builder->CreateCmp(llvm::CmpInst::Predicate::ICMP_SGE, val1, val2);
                break;
                case ADD:
                    return Builder->CreateAdd(val1, val2);
                break;
                case SUB:
                    return Builder->CreateSub(val1, val2);
                break;
                case DIV:
                    return Builder->CreateSDiv(val1, val2);
                break;
                case MUL:
                    return Builder->CreateMul(val1, val2);
                break;
                case MOD:
                    return Builder->CreateSub(val1, 
                        Builder->CreateMul(val1, 
                            Builder->CreateSDiv(val1, val2)
                        )
                    );
                break;
                case BITAND:
                    return Builder->CreateAnd(val1, val2);
                break;
                case BITOR:
                    return Builder->CreateOr(val1, val2);
                break;
                case XOR:
                    return Builder->CreateXor(val1, val2);
                break;
                case EQ:
                    return Builder->CreateCmp(llvm::CmpInst::Predicate::ICMP_EQ, val1, val2);
                break;
                case NEQ:
                    return Builder->CreateCmp(llvm::CmpInst::Predicate::ICMP_NE, val1, val2);
                break;
                case BOOL_OR:
                    return Builder->CreateOr(
                        Builder->CreateBitCast(val1, llvm::IntegerType::get(*TheContext, 1)), 
                        Builder->CreateBitCast(val2, llvm::IntegerType::get(*TheContext, 1))
                        );
                break;
                case BOOL_AND:
                        return Builder->CreateAnd(
                        Builder->CreateBitCast(val1, llvm::IntegerType::get(*TheContext, 1)), 
                        Builder->CreateBitCast(val2, llvm::IntegerType::get(*TheContext, 1))
                        );
                break;
            }
        }
    }

    ValueCall::ValueCall(std::string name, ValueArray args) {
        this->function_name = name;
        this->argvector = args;
    }

    llvm::Value * ValueCall::codegen() const {
        //TODO: implement calling function from local scope?
        auto fun = TheModule->getFunction(function_name);
        std::vector<llvm::Value *> args;
        for (auto i = argvector.begin(); i != argvector.end(); ++i) {
            args.push_back((*i)->codegen());
        }
        return Builder->CreateCall(fun, args);
    }

    GetVar::GetVar(std::string name) {
        this->var_name = name;
    }

    llvm::Value * GetVar::codegen() const {
        return Builder->CreateLoad(resolve_var_scope(var_name));
    }

    GetVarPtr::GetVarPtr(std::string name) {
        this->var_name = name;
    }

    llvm::Value * GetVarPtr::codegen() const {
        return resolve_var_scope(var_name);
    }

    UnaryOp::UnaryOp(UOps operand, Value *arg) {
        this->op = operand;
        this->arg = arg;
    }

    llvm::Value *UnaryOp::codegen() const {
        auto val = arg->codegen();
        switch (op) {
            case NOT:
                return Builder->CreateNot(val);
            break;
        }
    }

    GlobalPrototype::GlobalPrototype(Type *type, bool is_constant) {
        this->name = type->name;
        this->constant = is_constant;
        this->type = type;
    }

    void GlobalPrototype::codegen() const {
        auto t = this->type->get_type();
        if (t->isFunctionTy()) {
            auto f = llvm::Function::Create(static_cast<llvm::FunctionType *>(t), llvm::GlobalValue::ExternalLinkage, name, *TheModule);
        } else {
            //extern simple value
            //TODO: add address space stuff
            llvm::GlobalVariable(*TheModule,t,this->constant, llvm::GlobalValue::ExternalLinkage, nullptr, name);
        }
    }

    GlobalFunction::GlobalFunction(FunctionType *t, Body *body, std::vector<std::string> args) {
        this->type = t;
        this->body = body;
        this->args = args;
        this->name = t->name;
    };
    void GlobalFunction::codegen() const {
        auto prototype = TheModule->getFunction(name);
        if (!prototype) {
            //TODO: the static cast here may break stuff and segfault, simply split away the function declaration if this happens
            prototype = llvm::Function::Create(static_cast<llvm::FunctionType *>(type->get_type()), llvm::GlobalValue::ExternalLinkage, name, *TheModule);
        }
        llvm::BasicBlock *BB = llvm::BasicBlock::Create(*TheContext, name, prototype);
        Builder->SetInsertPoint(BB);
        int arg_offset = 0;
        std::map<std::string, llvm::AllocaInst *> local;
        for (auto arg = prototype->arg_begin(); arg != prototype->arg_end(); arg++) {
            auto alloca = Builder->CreateAlloca(arg->getType());
            Builder->CreateStore(arg, alloca);
            local[args[arg_offset]] = alloca;
            arg_offset++;
        }
        LocalScope.push_back(local);
        body->codegen();
        LocalScope.pop_back();
        llvm::verifyFunction(*prototype, &llvm::errs());
    }
}