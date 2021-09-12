#include <iostream>
#include <llvm-12/llvm/IR/Constants.h>
#include <llvm-12/llvm/IR/Instruction.h>
#include <llvm/IR/Constant.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/GlobalObject.h>
#include <llvm/IR/GlobalVariable.h>
#include <llvm/IR/Type.h>
#include <llvm/IR/Value.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/Verifier.h>
#include <memory>
#include <ostream>
#include <string>
#include <vector>

#include "ast.hpp"

static std::vector<std::map<std::string, llvm::AllocaInst *>> LocalScope;
namespace ast {
    llvm::Value *Vardef::codegen() const {
        auto type = ty->get_type();
        auto var_storage = Builder->CreateAlloca(type);
        LocalScope.back()[varname] = var_storage;
        return llvm::PoisonValue::get(type);
    }
    Vardef::Vardef(std::string varname, Type *type) {
        this->ty = type;
        this->varname = varname;
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
        auto glb = TheModule->getNamedGlobal(key); //TheModule->getGlobalVariable(key);
        if (glb) {
            //TODO: Global variables are probably broken, global variables are most likely going to get pointerized
            return glb;
        }
        //TODO: Proper error message
        
        llvm::errs() << "Could not resolve symbol \"" << key << "\"" << "\n";
        throw key;
    }

    Varset::Varset(std::string name, Value *val) {
        this->val = val;
        this->name = name;
    }

    llvm::Value *Varset::codegen() const {
        auto location = resolve_var_scope(name);
        auto value = val->codegen();
        Builder->CreateStore(value, location);
        return value;
    }

    Body::Body(std::vector<Value *> body) {
        this->body = body;
    }
    llvm::Value *Body::codegen() const {
        //Creates a new local scope
        LocalScope.emplace(LocalScope.end());
        //TODO: test if this works somehow
        llvm::Value *result;
        result = llvm::PoisonValue::get(llvm::Type::getVoidTy(*TheContext));
        for (auto it = body.begin(); it != body.end(); ++it) {
            result = (*it)->codegen();
        }
        LocalScope.pop_back();
        return result;
    }

    While::While(Body *body_a, Value *condition) {
        this->body = body_a;
        this->condition = condition;
    }
    llvm::Value *While::codegen() const {
        auto parent = Builder->GetInsertBlock()->getParent();
        auto while_prelude = llvm::BasicBlock::Create(*TheContext, "", parent);
        auto while_start = llvm::BasicBlock::Create(*TheContext, "", parent);
        auto while_body = llvm::BasicBlock::Create(*TheContext, "", parent);
        auto while_end = llvm::BasicBlock::Create(*TheContext, "", parent);
        Builder->CreateBr(while_prelude);

        Builder->SetInsertPoint(while_start);
        Builder->CreateCondBr(condition->codegen(), while_body, while_end);
        Builder->SetInsertPoint(while_body);
        auto res = body->codegen();

        if (!res->getType()->isVoidTy()) {
            auto storeres_prelude = Builder->CreateBr(while_start);

            Builder->SetInsertPoint(while_prelude);
            auto resvar = Builder->CreateAlloca(res->getType());
            Builder->CreateBr(while_start);

            Builder->SetInsertPoint(storeres_prelude);
            Builder->CreateStore(res, resvar);


            Builder->SetInsertPoint(while_end);
            return Builder->CreateLoad(resvar);
        } else {
            Builder->CreateBr(while_start);

            Builder->SetInsertPoint(while_prelude);
            Builder->CreateBr(while_start);

            Builder->SetInsertPoint(while_end);
            return llvm::PoisonValue::get(res->getType());
        }
        
    }

    If::If(Body *body_if,Body *body_else, Value *condition) {
        this->body_t = body_if;
        this->body_f = body_else;
        this->condition = condition;
    }
    //TODO: Actually test this, it seems spaghetti
    llvm::Value *If::codegen() const {
        auto parent = Builder->GetInsertBlock()->getParent();
        auto if_prelude = llvm::BasicBlock::Create(*TheContext, "", parent);
        auto if_start = llvm::BasicBlock::Create(*TheContext, "", parent);
        auto if_body = llvm::BasicBlock::Create(*TheContext, "", parent);
        auto if_else = llvm::BasicBlock::Create(*TheContext, "", parent);
        auto if_end = llvm::BasicBlock::Create(*TheContext, "", parent);
        Builder->CreateBr(if_prelude);

        Builder->SetInsertPoint(if_start);
        Builder->CreateCondBr(condition->codegen(), if_body, if_else);
        //Create new result variable

        Builder->SetInsertPoint(if_body);
        auto res_t = body_t->codegen();
        auto br_t_inst = Builder->CreateBr(if_end);

        Builder->SetInsertPoint(if_else);
        auto res_f = body_f->codegen();
        auto br_f_inst = Builder->CreateBr(if_end);

        //create merge variable
        Builder->SetInsertPoint(if_prelude);
        auto ty_t = res_t->getType();
        auto ty_f = res_f->getType();
        llvm::Value *result;
        if (ty_t->getTypeID() != ty_f->getTypeID()) {
            llvm::errs() << "WARNING: Type mismatch: " << ty_t << " and " << ty_f << " are not compatible in if statement";
            result = llvm::PoisonValue::get(ty_t);
            Builder->CreateBr(if_start);
            Builder->SetInsertPoint(if_end);
        } else if (ty_t->isVoidTy()) {
            result = llvm::PoisonValue::get(ty_t);
            Builder->CreateBr(if_start);
            Builder->SetInsertPoint(if_end);
        } else {
            auto rvar = Builder->CreateAlloca(ty_t);
            Builder->CreateBr(if_start);
            Builder->SetInsertPoint(br_t_inst);
            Builder->CreateStore(res_t, rvar);
            Builder->SetInsertPoint(br_f_inst);
            Builder->CreateStore(res_f, rvar);
            Builder->SetInsertPoint(if_end);
            result = Builder->CreateLoad(rvar);
        }
        return result;
    }

    ReturnVal::ReturnVal(Value *val) {
        this->val = val;
    }
    llvm::Value *ReturnVal::codegen() const {
        Builder->CreateRet(val->codegen());
        //hopefully fix Terminator found in the middle of a basic block!
        auto parent = Builder->GetInsertBlock()->getParent();
        auto nextblock = llvm::BasicBlock::Create(*TheContext, "", parent);
        Builder->SetInsertPoint(nextblock);
        //this block should be optimized out as the next bit of code is unreachable
        //never use the value of a return statement in anything else
        return llvm::PoisonValue::get(llvm::Type::getVoidTy(*TheContext));
    }

    llvm::Value *ReturnNull::codegen() const {
        Builder->CreateRetVoid();
        auto parent = Builder->GetInsertBlock()->getParent();
        auto nextblock = llvm::BasicBlock::Create(*TheContext, "", parent);
        Builder->SetInsertPoint(nextblock);
        return llvm::PoisonValue::get(llvm::Type::getVoidTy(*TheContext));
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
    FloatType::FloatType() {

    }
    llvm::Type *FloatType::get_type() const {
        return llvm::Type::getFloatTy(*TheContext);
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
    ValueConst::ValueConst(Value *val) {
        this->value = val;
    }
    
    FloatConst::FloatConst(float f) {
        this->from = f;
    }
    llvm::Constant *FloatConst::codegen() const {
        return llvm::ConstantFP::get(*TheContext, llvm::APFloat(this->from));
    }

    IntegerConst::IntegerConst(intmax_t i, int bits) {
        this->from = i;
        this->bits = bits;
    }
    llvm::Constant *IntegerConst::codegen() const {
        return llvm::ConstantInt::get(*TheContext, llvm::APInt(this->bits,this->from));
    }

    StringConst::StringConst(std::string from) {
        this->orig = from;
    }
    llvm::Constant *StringConst::codegen() const {
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
    ArrayConst::ArrayConst(Type *subtype, std::vector<Const *> from) {
        this->getfrom = from;
        this->t = subtype;
    }
    llvm::Constant *ArrayConst::codegen() const {
        std::vector<llvm::Constant *> init_array;
        for (auto it=this->getfrom.begin(); it != this->getfrom.end(); it++) {
            init_array.push_back((*it)->codegen());
        }
        auto constval = llvm::ConstantArray::get(llvm::ArrayType::get(this->t->get_type(), this->getfrom.size()), init_array);
        //auto globconst = new llvm::GlobalVariable(*TheModule, constval->getType(), true, llvm::GlobalValue::PrivateLinkage, constval);
        //return globconst;
        return constval;
    };

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

    Call::Call(std::string name, ValueArray args) {
        this->function_name = name;
        this->argvector = args;
    }

    llvm::Value *Call::codegen() const {
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
    Deref::Deref(Value *ptr) {
        this->p = ptr;
    }
    llvm::Value *Deref::codegen() const {
        return Builder->CreateLoad(this->p->codegen());
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
            case NEG:
                return Builder->CreateNeg(val);
            break;
        }
    }
    Arrgetptr::Arrgetptr(Value *array, Value *index) {
        this->array = array;
        this->index = index;
    }
    Expr::Expr(Value *inner) {
        this->actual = inner;
    }
    llvm::Value *Expr::codegen() const {
        return this->actual->codegen();
    }
    llvm::Value *Arrgetptr::codegen() const {
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

    Arrget::Arrget(Value *array, Value *index) {
        this->array = array;
        this->index = index;
    }
    llvm::Value *Arrget::codegen() const {
        auto the_pointer = new Arrgetptr(this->array, this->index);
        return Builder->CreateLoad(the_pointer->codegen());
    }

    Arrset::Arrset(Value *array, Value *index, Value *val) {
        this->array = array;
        this->index = index;
        this->val = val;
    }
    ArrDef::ArrDef(std::string name, Type *inner, Value *len) {
        this->name = name;
        this->inner_type = inner;
        this->length = len;
    }
    llvm::Value *ArrDef::codegen() const {
        auto type = this->inner_type->get_type();
        auto var_storage = Builder->CreateAlloca(type, this->length->codegen());
        LocalScope.back()[this->name] = var_storage;
        return llvm::PoisonValue::get(type);
    }

    llvm::Value *Arrset::codegen() const {
        auto the_pointer = new Arrgetptr(this->array, this->index);
        auto value = this->val->codegen();
        Builder->CreateStore(value, the_pointer->codegen());
        return value;
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
            new llvm::GlobalVariable(*TheModule,t,this->constant, llvm::GlobalValue::ExternalLinkage, nullptr, name);
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
        //Automatically add return instruction
        auto ret_type = prototype->getReturnType();
        if (ret_type->isVoidTy()) {
            Builder->CreateRetVoid();
        } else {
            auto ret_const = llvm::Constant::getNullValue(ret_type);
            Builder->CreateRet(ret_const);
        }
        LocalScope.pop_back();
    }
    GlobalVariable::GlobalVariable(std::string name_a, Const *value, bool is_const) {
        this->name = name_a;
        this->value = value;
        this->constant = is_const;
    }
    void GlobalVariable::codegen() const {
        auto val = this->value->codegen();
        auto the_var = new llvm::GlobalVariable(*TheModule, val->getType(), this->constant, llvm::GlobalObject::ExternalLinkage, val, this->name);
    }
}