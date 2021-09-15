#include <iostream>
#include <llvm/IR/Constants.h>
#include <llvm/IR/Instruction.h>
#include <llvm/IR/Constant.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/GlobalObject.h>
#include <llvm/IR/GlobalVariable.h>
#include <llvm/IR/Type.h>
#include <llvm/IR/Value.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/Verifier.h>
#include <llvm/IR/InlineAsm.h>
#include <memory>
#include <ostream>
#include <string>
#include <vector>

#include "ast.hpp"

static std::vector<std::map<std::string, llvm::AllocaInst *>> LocalScope;
namespace ast {
    const Const* Value::to_const() const {
        return dynamic_cast<const Const *>(this);
    }
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
    const Const *Operand::to_const() const {
        auto const1 = arg1->to_const();
        if (!const1) return nullptr;

        auto const2 = arg2->to_const();
        if (!const2) return nullptr;
        
        return new ConstOperand(const1, const2, this->op);
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
        if (val1->getType()->isFloatingPointTy() || val2->getType()->isFloatingPointTy()) {
            float_op = true;
        }
        if (float_op) {
            switch(op) {
                case OperandType::LT:
                    return Builder->CreateFCmp(llvm::CmpInst::Predicate::FCMP_OLT, val1, val2);
                break;
                case OperandType::GT:
                    return Builder->CreateFCmp(llvm::CmpInst::Predicate::FCMP_OGT, val1, val2);
                break;
                case OperandType::LE:
                    return Builder->CreateFCmp(llvm::CmpInst::Predicate::FCMP_OLE, val1, val2);
                break;
                case OperandType::GE:
                    return Builder->CreateFCmp(llvm::CmpInst::Predicate::FCMP_OGE, val1, val2);
                break;
                case OperandType::ADD:
                    return Builder->CreateFAdd(val1, val2);
                break;
                case OperandType::SUB:
                    return Builder->CreateFSub(val1, val2);
                break;
                case OperandType::DIV:
                    return Builder->CreateFDiv(val1, val2);
                break;
                case OperandType::MUL:
                    return Builder->CreateFMul(val1, val2);
                break;
                case OperandType::MOD:
                    throw "Cannot create floating point modulo!";
                    //return Builder->CreateSub(val1, 
                    //    Builder->CreateMul(val1, 
                    //        Builder->CreateSDiv(val1, val2)
                    //    )
                    //);
                break;
                case OperandType::BITAND:
                    return Builder->CreateAnd(val1, val2);
                break;
                case OperandType::BITOR:
                    return Builder->CreateOr(val1, val2);
                break;
                case OperandType::XOR:
                    return Builder->CreateXor(val1, val2);
                break;
                case OperandType::EQ:
                    return Builder->CreateFCmp(llvm::CmpInst::Predicate::FCMP_OEQ, val1, val2);
                break;
                case OperandType::NEQ:
                    return Builder->CreateFCmp(llvm::CmpInst::Predicate::FCMP_ONE, val1, val2);
                break;
                case OperandType::BOOL_OR:
                    return Builder->CreateOr(
                        Builder->CreateBitCast(val1, llvm::IntegerType::get(*TheContext, 1)), 
                        Builder->CreateBitCast(val2, llvm::IntegerType::get(*TheContext, 1))
                        );
                break;
                case OperandType::BOOL_AND:
                        return Builder->CreateAnd(
                        Builder->CreateBitCast(val1, llvm::IntegerType::get(*TheContext, 1)), 
                        Builder->CreateBitCast(val2, llvm::IntegerType::get(*TheContext, 1))
                        );
                break;
                }
        } else {
            //integer operations
            switch(op) {
                case OperandType::LT:
                    return Builder->CreateCmp(llvm::CmpInst::Predicate::ICMP_SLT, val1, val2);
                break;
                case OperandType::GT:
                    return Builder->CreateCmp(llvm::CmpInst::Predicate::ICMP_SGT, val1, val2);
                break;
                case OperandType::LE:
                    return Builder->CreateCmp(llvm::CmpInst::Predicate::ICMP_SLE, val1, val2);
                break;
                case OperandType::GE:
                    return Builder->CreateCmp(llvm::CmpInst::Predicate::ICMP_SGE, val1, val2);
                break;
                case OperandType::ADD:
                    return Builder->CreateAdd(val1, val2);
                break;
                case OperandType::SUB:
                    return Builder->CreateSub(val1, val2);
                break;
                case OperandType::DIV:
                    return Builder->CreateSDiv(val1, val2);
                break;
                case OperandType::MUL:
                    return Builder->CreateMul(val1, val2);
                break;
                case OperandType::MOD:
                    return Builder->CreateSub(val1, 
                        Builder->CreateMul(val1, 
                            Builder->CreateSDiv(val1, val2)
                        )
                    );
                break;
                case OperandType::BITAND:
                    return Builder->CreateAnd(val1, val2);
                break;
                case OperandType::BITOR:
                    return Builder->CreateOr(val1, val2);
                break;
                case OperandType::XOR:
                    return Builder->CreateXor(val1, val2);
                break;
                case OperandType::EQ:
                    return Builder->CreateCmp(llvm::CmpInst::Predicate::ICMP_EQ, val1, val2);
                break;
                case OperandType::NEQ:
                    return Builder->CreateCmp(llvm::CmpInst::Predicate::ICMP_NE, val1, val2);
                break;
                case OperandType::BOOL_OR:
                    return Builder->CreateOr(
                        Builder->CreateBitCast(val1, llvm::IntegerType::get(*TheContext, 1)), 
                        Builder->CreateBitCast(val2, llvm::IntegerType::get(*TheContext, 1))
                        );
                break;
                case OperandType::BOOL_AND:
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
        if (!fun) throw std::string("Could not find function ")
            .append(function_name);
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
            case UOps::NOT:
                return Builder->CreateNot(val);
            case UOps::NEG:
                return Builder->CreateNeg(val);
            break;
        }
    }

    const Const *UnaryOp::to_const() const {
        auto const1 = arg->to_const();
        if (!const1) return nullptr;
        return new ConstUnaryOp(this->op, const1);
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
    const Const *Expr::to_const() const {
        return this->actual->to_const();
    }
    Bitcast::Bitcast(Value *thing, Type *type) {
        this->thing = thing;
        this->type = type;
    }
    llvm::Value *Bitcast::codegen() const {
        //TODO: maybe use another bitcast instruction type?
        return Builder->CreatePointerBitCastOrAddrSpaceCast(this->thing->codegen(), this->type->get_type());
    }
    const Const *Bitcast::to_const() const {
        auto const1 = this->thing->to_const();
        if (!const1) return nullptr;
        return new ConstBitcast(const1, this->type);
    }
    ConstBitcast::ConstBitcast(const Const *thing, Type *type) {
        this->thing = thing;
        this->type = type;
    }
    llvm::Constant *ConstBitcast::codegen() const {
        return llvm::ConstantExpr::getPointerBitCastOrAddrSpaceCast(this->thing->codegen(), this->type->get_type());
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

    GlobalPrototype::GlobalPrototype(std::string name, Type *type, bool is_constant) {
        this->name = name;
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
    GlobalVariable::GlobalVariable(std::string name_a, const Const *value, bool is_const) {
        this->name = name_a;
        this->value = value;
        this->constant = is_const;
    }
    void GlobalVariable::codegen() const {
        auto val = this->value->codegen();
        auto the_var = new llvm::GlobalVariable(*TheModule, val->getType(), this->constant, llvm::GlobalObject::ExternalLinkage, val, this->name);
    }
    ConstUnaryOp::ConstUnaryOp(UOps operand, const Const *arg) {
        this->op = operand;
        this->arg = arg;
    }
    llvm::Constant *ConstUnaryOp::codegen() const {
        auto arg1 = this->arg->codegen();
        switch(this->op) {
            case UOps::NOT:
                return llvm::ConstantExpr::getNot(arg1);
            case UOps::NEG:
                return llvm::ConstantExpr::getNeg(arg1);
        }
    }
    //TODO: test this, get constraints, figure out what the constraints are, actually parse from AST
    llvm::Value *InlineAsm::codegen() const {
        std::vector<llvm::Value *> arg_list;
        for (auto arg : this->args) {
            arg_list.push_back(arg->codegen());
        }
        std::vector<llvm::Type *> type_list;
        for (auto val : arg_list) {
            type_list.push_back(val->getType());
        }
        //TODO: maybe support arbitrary asm syntax?
        auto asm_func =  llvm::InlineAsm::get(
            llvm::FunctionType::get(
                llvm::Type::getVoidTy(*TheContext),
                type_list,
                false
            ),
            this->asmstring,
            this->constraints,
            this->is_volatile,
            this->is_align_stack
        );
        return Builder->CreateCall(asm_func, arg_list, "inline_asm");
    }
    InlineAsm::InlineAsm(std::string assembly, std::string constraints, std::vector<Value *> args, bool is_volatile, bool is_align_stack) {
        this->asmstring = assembly;
        this->args = args;
        this->constraints = constraints;
        this->is_align_stack = is_align_stack;
        this->is_volatile = is_volatile;
    }
    ConstOperand::ConstOperand(const Const *lhs, const Const *rhs, OperandType op) {
        this->arg1 = lhs;
        this->arg2 = rhs;
        this->op = op;
    }
    llvm::Constant *ConstOperand::codegen() const {
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
        if (val1->getType()->isFloatingPointTy() || val2->getType()->isFloatingPointTy()) {
            float_op = true;
        }
        if (float_op) {
            switch(op) {
                case OperandType::LT:
                    return llvm::ConstantExpr::getFCmp(llvm::CmpInst::Predicate::FCMP_OLT, val1, val2);
                break;
                case OperandType::GT:
                    return llvm::ConstantExpr::getFCmp(llvm::CmpInst::Predicate::FCMP_OGT, val1, val2);
                break;
                case OperandType::LE:
                    return llvm::ConstantExpr::getFCmp(llvm::CmpInst::Predicate::FCMP_OLE, val1, val2);
                break;
                case OperandType::GE:
                    return llvm::ConstantExpr::getFCmp(llvm::CmpInst::Predicate::FCMP_OGE, val1, val2);
                break;
                case OperandType::ADD:
                    return llvm::ConstantExpr::getFAdd(val1, val2);
                break;
                case OperandType::SUB:
                    return llvm::ConstantExpr::getFSub(val1, val2);
                break;
                case OperandType::DIV:
                    return llvm::ConstantExpr::getFDiv(val1, val2);
                break;
                case OperandType::MUL:
                    return llvm::ConstantExpr::getFMul(val1, val2);
                break;
                case OperandType::MOD:
                    throw "Cannot create floating point modulo!";
                    //return llvm::ConstantExpr::getSub(val1, 
                    //    llvm::ConstantExpr::getMul(val1, 
                    //        llvm::ConstantExpr::getSDiv(val1, val2)
                    //    )
                    //);
                break;
                case OperandType::BITAND:
                    return llvm::ConstantExpr::getAnd(val1, val2);
                break;
                case OperandType::BITOR:
                    return llvm::ConstantExpr::getOr(val1, val2);
                break;
                case OperandType::XOR:
                    return llvm::ConstantExpr::getXor(val1, val2);
                break;
                case OperandType::EQ:
                    return llvm::ConstantExpr::getFCmp(llvm::CmpInst::Predicate::FCMP_OEQ, val1, val2);
                break;
                case OperandType::NEQ:
                    return llvm::ConstantExpr::getFCmp(llvm::CmpInst::Predicate::FCMP_ONE, val1, val2);
                break;
                case OperandType::BOOL_OR:
                    return llvm::ConstantExpr::getOr(
                        llvm::ConstantExpr::getBitCast(val1, llvm::IntegerType::get(*TheContext, 1)), 
                        llvm::ConstantExpr::getBitCast(val2, llvm::IntegerType::get(*TheContext, 1))
                        );
                break;
                case OperandType::BOOL_AND:
                        return llvm::ConstantExpr::getAnd(
                        llvm::ConstantExpr::getBitCast(val1, llvm::IntegerType::get(*TheContext, 1)), 
                        llvm::ConstantExpr::getBitCast(val2, llvm::IntegerType::get(*TheContext, 1))
                        );
                break;
                }
        } else {
            //integer operations
            switch(op) {
                case OperandType::LT:
                    return llvm::ConstantExpr::getICmp(llvm::CmpInst::Predicate::ICMP_SLT, val1, val2);
                break;
                case OperandType::GT:
                    return llvm::ConstantExpr::getICmp(llvm::CmpInst::Predicate::ICMP_SGT, val1, val2);
                break;
                case OperandType::LE:
                    return llvm::ConstantExpr::getICmp(llvm::CmpInst::Predicate::ICMP_SLE, val1, val2);
                break;
                case OperandType::GE:
                    return llvm::ConstantExpr::getICmp(llvm::CmpInst::Predicate::ICMP_SGE, val1, val2);
                break;
                case OperandType::ADD:
                    return llvm::ConstantExpr::getAdd(val1, val2);
                break;
                case OperandType::SUB:
                    return llvm::ConstantExpr::getSub(val1, val2);
                break;
                case OperandType::DIV:
                    return llvm::ConstantExpr::getSDiv(val1, val2);
                break;
                case OperandType::MUL:
                    return llvm::ConstantExpr::getMul(val1, val2);
                break;
                case OperandType::MOD:
                    return llvm::ConstantExpr::getSub(val1, 
                        llvm::ConstantExpr::getMul(val1, 
                            llvm::ConstantExpr::getSDiv(val1, val2)
                        )
                    );
                break;
                case OperandType::BITAND:
                    return llvm::ConstantExpr::getAnd(val1, val2);
                break;
                case OperandType::BITOR:
                    return llvm::ConstantExpr::getOr(val1, val2);
                break;
                case OperandType::XOR:
                    return llvm::ConstantExpr::getXor(val1, val2);
                break;
                case OperandType::EQ:
                    return llvm::ConstantExpr::getICmp(llvm::CmpInst::Predicate::ICMP_EQ, val1, val2);
                break;
                case OperandType::NEQ:
                    return llvm::ConstantExpr::getICmp(llvm::CmpInst::Predicate::ICMP_NE, val1, val2);
                break;
                case OperandType::BOOL_OR:
                    return llvm::ConstantExpr::getOr(
                        llvm::ConstantExpr::getBitCast(val1, llvm::IntegerType::get(*TheContext, 1)), 
                        llvm::ConstantExpr::getBitCast(val2, llvm::IntegerType::get(*TheContext, 1))
                        );
                break;
                case OperandType::BOOL_AND:
                        return llvm::ConstantExpr::getAnd(
                        llvm::ConstantExpr::getBitCast(val1, llvm::IntegerType::get(*TheContext, 1)), 
                        llvm::ConstantExpr::getBitCast(val2, llvm::IntegerType::get(*TheContext, 1))
                        );
                break;
            }
        }
    }

    //print commands
    void Varset::print_val(std::stringstream &stream) const {
        stream << "{\n\"ast\":\"Varset\",";    //{"ast":"Varset",
        stream << "\n\"name\":\"" << this->name << "\",";   //"name":"{NAME}",
        stream << "\n\"value\":"; //"value":
        this->val->print_val(stream);
        stream << "}";
    }
    void Vardef::print_val(std::stringstream &stream) const {
        stream << "{\n\"ast\":\"Vardef\",";    //{"ast":"Vardef",
        stream << "\n\"varname\":\"" << this->varname << "\",";   //"varname":"{VARNAME}",
        stream << "\n\"type\":"; //"type":
        this->ty->print_type(stream);
        stream << "}";
    }
    void Body::print_val(std::stringstream &stream) const {
        stream << "{\n\"ast\":\"Body\",";    //{"ast":"Body",
        stream << "\n\"inner\": ["; //"inner": [
        bool is_first_print = true;
        for (auto item : this->body) {
            if (is_first_print) {
                //dont print , for first element
                is_first_print = false;
            } else {
                stream << ",";
            }
            item->print_val(stream);
        }
        stream << "]}";
    }
    void While::print_val(std::stringstream &stream) const {
        stream << "{\n\"ast\":\"While\",";    //{"ast":"While",
        stream << "\n\"condition\":"; //"condition":
        this->condition->print_val(stream);
        stream << ",\n\"body\":"; //,"body":
        this->body->print_val(stream);
        stream << "}";
    }
    void If::print_val(std::stringstream &stream) const {
        stream << "{\n\"ast\":\"If\",";    //{"ast":"If",
        stream << "\n\"condition\":";    //"condition":
        this->condition->print_val(stream);
        stream << ",\n\"body_t\":"; //,"body_t":
        this->body_t->print_val(stream);
        stream << ",\n\"body_f\":"; //,"body_f":
        this->body_f->print_val(stream);
        stream << "}";
    }
    void ReturnVal::print_val(std::stringstream &stream) const {
        stream << "{\n\"ast\":\"ReturnVal\",";    //{"ast":"ReturnVal",
        stream << "\n\"value\":"; //"value":
        this->val->print_val(stream);
        stream << "}";
    }
    void ReturnNull::print_val(std::stringstream &stream) const {
        stream << "{\n\"ast\":\"ReturnNull\"}";    //{"ast":"ReturnNull"}
    }
    void IntType::print_type(std::stringstream &stream) const {
        stream << "{\n\"type_ast\":\"IntType\",";    //{"type_ast":"IntType",
        stream << "\n\"bits\":" << this->bits << "}"; //"bits": {BITS} }
    }
    void FloatType::print_type(std::stringstream &stream) const {
        stream << "{\n\"type_ast\":\"FloatType\"";    //{"type_ast":"FloatType"}
    }
    void StringType::print_type(std::stringstream &stream) const {
        stream << "{\n\"type_ast\":\"StringType\",";    //{"type_ast":"StringType",
        stream << "\n\"length\":" << this->length << "}"; //"length": {LENGTH} }
    }
    void ArrayType::print_type(std::stringstream &stream) const {
        stream << "{\n\"type_ast\":\"ArrayType\",";    //{"type_ast":"ArrayType",
        stream << "\n\"length\":" << this->length; //"length": {LENGTH}
        stream << ",\n\"inside\":";   //,"inside":
        this->inside->print_type(stream);
        stream << "}";
    }
    void PointerType::print_type(std::stringstream &stream) const {
        stream << "{\n\"type_ast\":\"PointerType\",";    //{"type_ast":"PointerType",
        stream << "\n\"to\":"; //"to":
        this->to->print_type(stream);
        stream << "}";
    }
    void VoidType::print_type(std::stringstream &stream) const {
        stream << "{\n\"type_ast\":\"VoidType\"}";    //{"type_ast":"VoidType"}
    }
    void FunctionType::print_type(std::stringstream &stream) const {
        stream << "{\n\"type_ast\":\"FunctionType\",";    //{"type_ast":"FunctionType",
        stream << "\n\"name\":\"" << this->name << "\",";   //"name":" {NAME} ",
        stream << "\n\"args\": [";    //"args": [
        bool is_first_print = true;
        for (auto thing : this->args) {
            if (is_first_print) {
                //dont print the first comma
                is_first_print = false;
            } else {
                stream << ",";
            }
            thing->print_type(stream);
        }
        stream << "]\n\"return_type\":";    //]"return_type":
        this->return_type->print_type(stream);
        stream << ",\n\"varargs\":" << (this->varargs ? "true" : "false") << "}";   //,"varargs": {VARARGS} }
    }
    void ArrDef::print_val(std::stringstream &stream) const {
        stream << "{\n\"ast\":\"ArrDef\",";    //{"ast":"ArrDef",
        stream << "\n\"name\":\"" << this->name << "\",";   //"name":"{NAME}",
        stream << "\n\"inner\":"; //"inner":
        this->inner_type->print_type(stream);
        stream << ",\n\"length\":"; //,"length":
        this->length->print_val(stream);
        stream << "}";
    }
    void ConstOperand::print_val(std::stringstream &stream) const {
        stream << "{\n\"ast\":\"ConstOperand\",";    //{"ast":"ConstOperand",
        stream << "\n\"op\":\"OperandType::";   //"op":"OperandType::,
        switch (this->op) {
            case OperandType::LT:
                stream << "LT";
                break;
            case OperandType::GT:
                stream << "GT";
                break;
            case OperandType::LE:
                stream << "LE";
                break;
            case OperandType::GE:
                stream << "GE";
                break;
            case OperandType::ADD:
                stream << "ADD";
                break;
            case OperandType::SUB:
                stream << "SUB";
                break;
            case OperandType::DIV:
                stream << "DIV";
                break;
            case OperandType::MUL:
                stream << "MUL";
                break;
            case OperandType::MOD:
                stream << "MOD";
                break;
            case OperandType::BITAND:
                stream << "BITAND";
                break;
            case OperandType::BITOR:
                stream << "BITOR";
                break;
            case OperandType::XOR:
                stream << "XOR";
                break;
            case OperandType::EQ:
                stream << "EQ";
                break;
            case OperandType::NEQ:
                stream << "NEQ";
                break;
            case OperandType::BOOL_OR:
                stream << "BOOL_OR";
                break;
            case OperandType::BOOL_AND:
                stream << "BOOL_AND";
                break;
        }
        stream << "\",\"arg1\":";    //","arg1":
        this->arg1->print_val(stream);
        stream << ",\"arg2\":"; //,"arg2":
        this->arg2->print_val(stream);
        stream << "}";
    }
    void ConstUnaryOp::print_val(std::stringstream &stream) const {
        stream << "{\n\"ast\":\"ConstUnaryOp\",";    //{"ast":"ConstUnaryOp",
        stream << "\n\"op\":\"UOps::";    //"op":"UOps::
        switch (this->op) {
            case UOps::NOT:
                stream << "NOT";
                break;
            case UOps::NEG:
                stream << "NEG";
                break;
        }

        stream << "\",\"arg\":";    //","arg":
        this->arg->print_val(stream);
        stream << "}";
    }
    void IntegerConst::print_val(std::stringstream &stream) const {
        stream << "{\n\"ast\":\"IntegerConst\",";    //{"ast":"IntegerConst",
        stream << "\n\"from\":" << this->from << ",";  //"from": {FROM},
        stream << "\n\"bits\":" << this->bits << "}"; //"bits": {BITS} }
    }
    void FloatConst::print_val(std::stringstream &stream) const {
        stream << "{\n\"ast\":\"FloatConst\",";    //{"ast":"FloatConst",
        stream << "\n\"from\":" << this->from << "}";  //"from": {FROM}}
    }
    void ArrayConst::print_val(std::stringstream &stream) const {
        stream << "{\n\"ast\":\"ArrayConst\",";    //{"ast":"ArrayConst",
        stream << "\n\"subtype\":";  //"subtype":
        this->t->print_type(stream);
        stream << ",\n\"from\": [";   //,"from": [
        bool is_first_print = true;
        for (auto thing : this->getfrom) {
            if (is_first_print) {
                //omit first comma
                is_first_print = false;
            } else {
                stream << ",";
            }
            thing->print_val(stream);
        }
        stream << "]}"; //]}
    }
    void StringConst::print_val(std::stringstream &stream) const {
        stream << "{\n\"ast\":\"StringConst\",";    //{"ast":"StringConst",
        stream << "\n\"orig\": \"" << this->orig << "\" }";  //"orig": "{ORIG}"" }
    }

    void Operand::print_val(std::stringstream &stream) const {
        stream << "{\n\"ast\":\"Operand\",";    //{"ast":"Operand",
        stream << "\n\"op\":\"OperandType::";   //"op":"OperandType::,
        switch (this->op) {
            case OperandType::LT:
                stream << "LT";
                break;
            case OperandType::GT:
                stream << "GT";
                break;
            case OperandType::LE:
                stream << "LE";
                break;
            case OperandType::GE:
                stream << "GE";
                break;
            case OperandType::ADD:
                stream << "ADD";
                break;
            case OperandType::SUB:
                stream << "SUB";
                break;
            case OperandType::DIV:
                stream << "DIV";
                break;
            case OperandType::MUL:
                stream << "MUL";
                break;
            case OperandType::MOD:
                stream << "MOD";
                break;
            case OperandType::BITAND:
                stream << "BITAND";
                break;
            case OperandType::BITOR:
                stream << "BITOR";
                break;
            case OperandType::XOR:
                stream << "XOR";
                break;
            case OperandType::EQ:
                stream << "EQ";
                break;
            case OperandType::NEQ:
                stream << "NEQ";
                break;
            case OperandType::BOOL_OR:
                stream << "BOOL_OR";
                break;
            case OperandType::BOOL_AND:
                stream << "BOOL_AND";
                break;
        }
        stream << "\",\"arg1\":";    //","arg1":
        this->arg1->print_val(stream);
        stream << ",\"arg2\":"; //,"arg2":
        this->arg2->print_val(stream);
        stream << "}";
    }
    void Call::print_val(std::stringstream &stream) const {
        stream << "{\n\"ast\":\"Call\",";    //{"ast":"Call",
        stream << "\n\"function_name\":\"" << this->function_name << "\",";   //"function_name":"{FUNCTION_NAME}",
        stream << "\n\"argvector\": ["; //"argsvector": [
        bool is_first_print = true;
        for (auto thing : this->argvector) {
            if (is_first_print) {
                //omit first comma
                is_first_print = false;
            } else {
                stream << ",";
            }
            thing->print_val(stream);
        }
        stream << "]}"; // ]}
    }
    void GetVar::print_val(std::stringstream &stream) const {
        stream << "{\n\"ast\":\"GetVar\",";    //{"ast":"GetVar",
        stream << "\n\"var_name\":\"" << this->var_name << "\"}";   //"var_name":"{NAME} }"
    }
    void GetVarPtr::print_val(std::stringstream &stream) const {
        stream << "{\n\"ast\":\"GetVarPtr\",";    //{"ast":"GetVarPtr",
        stream << "\n\"var_name\":\"" << this->var_name << "\"}";   //"var_name":"{NAME} }"
    }
    void Deref::print_val(std::stringstream &stream) const {
        stream << "{\n\"ast\":\"Deref\",";    //{"ast":"Deref",
        stream << "\n\"p\":"; //"p":
        this->p->print_val(stream);
        stream << "}";  //}
    }
    void UnaryOp::print_val(std::stringstream &stream) const {
        stream << "{\n\"ast\":\"UnaryOp\",";    //{"ast":"UnaryOp",
        stream << "\n\"op\":\"UOps::";    //"op":"UOps::
        switch (this->op) {
            case UOps::NOT:
                stream << "NOT";
                break;
            case UOps::NEG:
                stream << "NEG";
                break;
        }

        stream << "\",\"arg\":";    //","arg":
        this->arg->print_val(stream);
        stream << "}";
    }
    void Arrget::print_val(std::stringstream &stream) const {
        stream << "{\n\"ast\":\"Arrget\",";    //{"ast":"Arrget",
        stream << "\n\"array\":"; //"array":
        this->array->print_val(stream);
        stream << ",\n\"index\":";  //,"index":
        this->index->print_val(stream);
        stream << "}";  //}
    }
    void Arrset::print_val(std::stringstream &stream) const {
        stream << "{\n\"ast\":\"Arrset\",";    //{"ast":"Arrset",
        stream << "\n\"array\":"; //"array":
        this->array->print_val(stream);
        stream << ",\n\"index\":";  //,"index":
        this->index->print_val(stream);
        stream << ",\n\"val\":";  //,"val":
        this->val->print_val(stream);
        stream << "}";  //}
    }
    void Arrgetptr::print_val(std::stringstream &stream) const {
        stream << "{\n\"ast\":\"Arrgetptr\",";    //{"ast":"Arrgetptr",
        stream << "\n\"array\":"; //"array":
        this->array->print_val(stream);
        stream << ",\n\"index\":";  //,"index":
        this->index->print_val(stream);
        stream << "}";  //}
    }
    void Expr::print_val(std::stringstream &stream) const {
        stream << "{\n\"ast\":\"Expr\",";    //{"ast":"Expr",
        stream << "\n\"actual\":"; //"actual":
        this->actual->print_val(stream);
        stream << "}";  //}
    }
    void Bitcast::print_val(std::stringstream &stream) const {
        stream << "{\n\"ast\":\"Bitcast\",";    //{"ast":"Bitcast",
        stream << "\n\"thing\":";   //"thing":
        this->thing->print_val(stream);
        stream << ",\n\"type\":";   //,"type":
        this->type->print_type(stream);
        stream << "}";  //}
    }
    void ConstBitcast::print_val(std::stringstream &stream) const {
        stream << "{\n\"ast\":\"ConstBitcast\",";    //{"ast":"ConstBitcast",
        stream << "\n\"thing\":";   //"thing":
        this->thing->print_val(stream);
        stream << ",\n\"type\":";   //,"type":
        this->type->print_type(stream);
        stream << "}";  //}
    }
    void InlineAsm::print_val(std::stringstream &stream) const {
        stream << "{\n\"ast\":\"InlineAsm\",";    //{"ast":"InlineAsm",
        stream << "\n\"asmstring\": \"" << this->asmstring << "\",";    //"asmstring": " {ASMSTRING} ",
        stream << "\n\"constraints\": \"" << this->constraints << "\",";    //"constraints": " {CONSTRAINTS} ",
        stream << "\n\"args\": [";  //"args": [
        bool is_first_print = true;
        for (auto thing : this->args) {
            if (is_first_print) {
                is_first_print = false;
            } else {
                stream << ",";
            }
            thing->print_val(stream);
        }
        stream << "],\n\"is_volatile\":" << (this->is_volatile ? "true" : "false") << ",";  //],"is_volatile": {IS_VOLATILE} ,
        stream << "\n\"is_align_stack\":" << (this->is_align_stack ? "true" : "false") << "}";  //"is_align_stack": {IS_ALIGN_STACK} }
    }
    //Global declaration prints
    void GlobalPrototype::print_global(std::stringstream &stream) const {
        stream << "{\n\"global_ast\":\"GlobalPrototype\",";    //{"global_ast":"GlobalPrototype",
        stream << "\n\"name\": \"" << this->name << "\",";  //"name": " {NAME} ",
        stream << "\n\"type\":";    //"type":
        this->type->print_type(stream);
        stream << ",\n\"constant\": " << (this->constant ? "true" : "false") << "}";   //,"constant": {CONSTANT} }
    }
    void GlobalFunction::print_global(std::stringstream &stream) const {
        stream << "{\n\"global_ast\":\"GlobalFunction\",";    //{"global_ast":"GlobalFunction",
        stream << "\n\"name\": \"" << this->name << "\",";  //"name": " {NAME} ",
        stream << "\n\"type\":";    //"type":
        this->type->print_type(stream);
        stream << ",\n\"body\":";   //,"body":
        this->body->print_val(stream);
        stream << ",\n\"args\": [";   //,"args": [
        bool is_first_print = true;
        for (auto thing : this->args) {
            if (is_first_print) {
                //omit first comma
                is_first_print = false;
            } else {
                stream << ",";
            }
            stream << "\"" << thing << "\"";
        }
        stream << "]}"; //]}
    }
    void GlobalVariable::print_global(std::stringstream &stream) const {
        stream << "{\n\"global_ast\":\"GlobalVariable\",";    //{"global_ast":"GlobalVariable",
        stream << "\n\"name\": \"" << this->name << "\",";  //"name": " {NAME} ",
        stream << "\n\"value\":";    //"value":
        this->value->print_val(stream);
        stream << ",\n\"constant\": " << (this->constant ? "true" : "false") << "}";   //,"constant": {CONSTANT} }
    }
    void print_program_to(std::vector<GlobalEntry *> &program, std::stringstream &stream) {
        stream << "[";
        bool is_first_print = true;
        for (auto entry : program) {
            if (is_first_print) {
                //omit first comma
                is_first_print = false;
            } else {
                stream << ",";
            }
            entry->print_global(stream);
        }
        stream << "]";
    }
}