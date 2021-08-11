#include <iostream>
#include <llvm-12/llvm/ADT/APFloat.h>
#include <llvm-12/llvm/ADT/APInt.h>
#include <llvm-12/llvm/ADT/StringRef.h>
#include <llvm-12/llvm/IR/Constants.h>
#include <llvm-12/llvm/IR/Function.h>
#include <llvm-12/llvm/IR/Type.h>
#include <llvm-12/llvm/Support/CodeGen.h>
#include <llvm-12/llvm/Support/raw_ostream.h>
#include <llvm/ADT/ArrayRef.h>
#include <llvm/ADT/Triple.h>
#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/Constant.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/InstrTypes.h>
#include <llvm/IR/Instructions.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/GlobalObject.h>
#include <llvm/IR/GlobalValue.h>
#include <llvm/IR/GlobalVariable.h>
#include <llvm/IR/LegacyPassManager.h>
#include <llvm/Target/TargetMachine.h>
#include <llvm/Target/TargetOptions.h>
#include <llvm/Target/TargetLoweringObjectFile.h>

#include <llvm/Support/TargetRegistry.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/Support/Host.h>
#include <llvm/Support/FileSystem.h>
#include <llvm/Support/raw_ostream.h>

#include <memory>
#include <string>
#include <vector>
std::unique_ptr<llvm::LLVMContext> TheContext;
std::unique_ptr<llvm::IRBuilder<>> Builder;
std::unique_ptr<llvm::Module> TheModule;
void init_module() {
    TheContext = std::make_unique<llvm::LLVMContext>();
    TheModule = std::make_unique<llvm::Module>("cornpiler", *TheContext);
    Builder = std::make_unique<llvm::IRBuilder<>>(*TheContext);
}
class ASTValue;
static std::vector<std::map<std::string, llvm::AllocaInst *>> LocalScope;
//static std::map<std::string, llvm::GlobalObject> GlobalScope;

class ASTBase {
    public:
    virtual void codegen() {
        llvm::errs() << "UNREACHABLE CODE REACHED ASTBase";
        //THIS SHOULD BE UNREACHABLE
    };
    //virtual ~ASTBase();
};

class ASTValue {
    public:
    virtual llvm::Value *codegen() {
        llvm::errs() << "UNREACHABLE CODE REACHED ASTValue";
        //THIS SHOULD BE UNREACHABLE
        return nullptr;
    };
    //virtual ~ASTValue();
};
using ASTValueArray=std::vector<ASTValue>;

class ASTBaseVardef : public ASTBase {
    public:
    std::string varname;
    ASTValue val;
    virtual void codegen() {
        auto local = LocalScope.back();
        auto value = val.codegen();
        auto var_storage = Builder->CreateAlloca(value->getType());
        Builder->CreateStore(value, var_storage);
        local[varname] = var_storage;
    }
    ASTBaseVardef(std::string varname, ASTValue val) {
        this->varname = varname;
        this->val = val;
    }
};
class ASTBaseCall : public ASTBase {
    public:
    std::string function_name;
    ASTValueArray argvector;
    virtual void codegen() {
        auto fun = TheModule->getFunction(function_name);
        std::vector<llvm::Value *> args;
        for (ASTValueArray::iterator i = argvector.begin(); i != argvector.end(); ++i) {
            args.push_back(i->codegen());
        }
        Builder->CreateCall(fun, args);
    }
    ASTBaseCall(std::string name, ASTValueArray args) {
        this->function_name = name;
        this->argvector = args;
    }
};
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
class ASTBaseVarset : public ASTBase {
    public:
    std::string name;
    ASTValue val;
    virtual void codegen() {
        auto location = resolve_var_scope(name);
        Builder->CreateStore(val.codegen(), location);
    }
    ASTBaseVarset(std::string name, ASTValue val) {
        this->name = name;
        this->val = val;
    }
};
class ASTBody : public ASTBase {
    public:
    std::vector<ASTBase> body;
    virtual void codegen() {
        //Creates a new local scope
        LocalScope.emplace(LocalScope.end());
        for (auto it = body.begin(); it != body.end(); ++it) {
            it->codegen();
        }
        LocalScope.pop_back();
    }
    ASTBody(std::vector<ASTBase> body) {
        this->body = body;
    }
};
class ASTWhile : public ASTBase {
    public:
    ASTBody body;
    ASTValue condition;
    virtual void codegen() {
        auto while_start = llvm::BasicBlock::Create(*TheContext);
        auto while_body = llvm::BasicBlock::Create(*TheContext);
        auto while_end = llvm::BasicBlock::Create(*TheContext);
        Builder->CreateBr(while_start);
        Builder->SetInsertPoint(while_start);
        Builder->CreateCondBr(condition.codegen(), while_body, while_end);
        Builder->SetInsertPoint(while_body);
        body.codegen();
        Builder->CreateBr(while_start);
        Builder->SetInsertPoint(while_end);
    };
    ASTWhile(ASTBody body_a, ASTValue condition) : body(body_a) {
        this->condition = condition;
    }
};
class ASTIf : public ASTBase {
    public:
    ASTBody body_t;
    ASTBody body_f;
    ASTValue condition;
    virtual void codegen() {
        auto if_start = llvm::BasicBlock::Create(*TheContext);
        auto if_body = llvm::BasicBlock::Create(*TheContext);
        auto if_else = llvm::BasicBlock::Create(*TheContext);
        auto if_end = llvm::BasicBlock::Create(*TheContext);
        Builder->CreateBr(if_start);
        Builder->SetInsertPoint(if_start);
        Builder->CreateCondBr(condition.codegen(), if_body, if_else);

        Builder->SetInsertPoint(if_body);
        body_t.codegen();
        Builder->CreateBr(if_end);

        Builder->SetInsertPoint(if_else);
        body_f.codegen();
        Builder->CreateBr(if_end);

        Builder->SetInsertPoint(if_end);
    }
    ASTIf(ASTBody body_if,ASTBody body_else, ASTValue condition) : body_t(body_if), body_f(body_else) {
        this->condition = condition;
    }
};
class ASTReturnVal : public ASTBase {
    public:
    ASTValue val;
    virtual void codegen() {
        Builder->CreateRet(val.codegen());
    }
    ASTReturnVal(ASTValue val) {
        this->val = val;
    }
};
class ASTReturnNull : public ASTBase {
    public:
    virtual void codegen() {
        Builder->CreateRetVoid();
    }
};
class ASTType {
    public:
    std::string name;
    virtual llvm::Type *get_type() {
        llvm::errs() << "UNREACHABLE CODE REACHED ASTType";
        //THIS SHOULD BE UNREACHABLE
        return nullptr;
    };
};
class ASTIntType : public ASTType {
    public:
    int bits;
    ASTIntType(int bits) {
        this->bits = bits;
    }
    virtual llvm::Type *get_type() {
        return llvm::IntegerType::get(*TheContext, this->bits);
    }
};
class ASTStringType : public ASTType {
    public:
    int length;
    ASTStringType(int length) {
        this->length = length;
    };
    virtual llvm::Type *get_type() {
        return llvm::ArrayType::get(
            llvm::IntegerType::getInt8Ty(*TheContext),
            this->length
        );
    }
};
class ASTArrayType : public ASTType {
    public:
    int length;
    ASTType inside;
    ASTArrayType(ASTType inside, int len) {
        this->inside = inside;
        this->length = len;
    }
    virtual llvm::Type *get_type() {
        return llvm::ArrayType::get(
            inside.get_type(),
            length
        );
    }
};
class ASTPointerType : public ASTType {
    public:
    ASTType to;
    ASTPointerType(ASTType to) {
        this->to = to;
    }
    virtual llvm::Type *get_type() {
        //TODO: do addrspace to const the pointer?
        return to.get_type()->getPointerTo();
    }
};
class ASTFunctionType : public ASTType {
    public:
    ASTType return_type;
    std::vector<ASTType> args;
    bool varargs;
    ASTFunctionType(std::string name, std::vector<ASTType> args, ASTType return_type, bool varargs=false) {
        this->name = name;
        this->args = args;
        this->return_type = return_type;
        this->varargs = varargs;
    }
    virtual llvm::Type *get_type() {
        std::vector<llvm::Type *> arg_types;
        for (std::vector<ASTType>::iterator it=this->args.begin(); it != this->args.end(); it++) {
            arg_types.push_back((*it).get_type());
        }
        return llvm::FunctionType::get(this->return_type.get_type(), arg_types, this->varargs);
    }
};
//TODO: split off ASTConst to different types to offload the work to the lexer?
class ASTConst : public ASTValue {
    public:
    ASTType type;
    std::string thing;
    virtual llvm::Value *codegen() {
        auto t = type.get_type();
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
                for (std::string::iterator it=this->thing.begin(); it != this->thing.end(); it++) {
                    string_array.push_back(llvm::ConstantInt::get(*TheContext, llvm::APInt(t->getIntegerBitWidth(), *it)));
                }
                //TODO: static cast here might be a horrible horrible idea that will break the program, unbreak this if the string is borked
                return llvm::ConstantArray::get(static_cast<llvm::ArrayType *>(t), string_array);
                }
            break;
            //case llvm::Type::FixedVectorTyID: 
            //case llvm::Type::ScalableVectorTyID : 
            default:
            llvm::errs() << "Invalid type: " << this->type.name;
            return nullptr;
        }
    }
    ASTConst(ASTType t, std::string container) {
        this->type = t;
        this->thing = container;
    }
};
class ASTOperand : public ASTValue {
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
    ASTOperand(ASTValue lhs, ASTValue rhs, Operand op) {
        this->arg1 = lhs;
        this->arg2 = rhs;
        this->op = op;
    }
};
class ASTValueCall : public ASTValue {
    public:
    std::string function_name;
    ASTValueArray argvector;
    virtual llvm::Value *codegen() {
        //TODO: implement calling function from local scope?
        auto fun = TheModule->getFunction(function_name);
        std::vector<llvm::Value *> args;
        for (ASTValueArray::iterator i = argvector.begin(); i != argvector.end(); ++i) {
            args.push_back(i->codegen());
        }

        return Builder->CreateCall(fun, args);
    }
    ASTValueCall(std::string name, ASTValueArray args) {
        this->function_name = name;
        this->argvector = args;
    }
};
class ASTGetVar : public ASTValue {
    public:
    std::string var_name;
    virtual llvm::Value *codegen() {
        return Builder->CreateLoad(resolve_var_scope(var_name));
    }
    ASTGetVar(std::string name) {
        this->var_name = name;
    }
};
class ASTGetVarPtr : public ASTValue {
    public:
    std::string var_name;
    virtual llvm::Value *codegen() {
        return resolve_var_scope(var_name);
    }
    ASTGetVarPtr(std::string name) {
        this->var_name = name;
    }
};
class ASTUnaryOp : public ASTValue {
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
                return Builder->CreateNot(val);
            break;
        }
    }
};

class ASTGlobalEntry {
    public:
    virtual void codegen() {
        llvm::errs() << "UNREACHABLE CODE REACHED ASTGlobalEntry";
        //THIS SHOULD BE UNREACHABLE
    };
    std::string name;
};
//Extern declarations
class ASTGlobalPrototype : public ASTGlobalEntry {
    public:
    ASTType type;
    bool constant;
    ASTGlobalPrototype(std::string name, ASTType type, bool is_constant=false) {
        this->name = name;
        this->type = type;
        this->constant = is_constant;
    }
    virtual void codegen() {
        auto t = this->type.get_type();
        if (t->isFunctionTy()) {
            auto f = llvm::Function::Create(static_cast<llvm::FunctionType *>(t), llvm::GlobalValue::ExternalLinkage, name, *TheModule);
        } else {
            //extern simple value
            //TODO: add address space stuff
            llvm::GlobalVariable(*TheModule,t,this->constant, llvm::GlobalValue::ExternalLinkage, nullptr, name);
        }
    }
};
class ASTGlobalFunction : public ASTGlobalEntry {
    public:
    ASTBody body;
    ASTFunctionType type;
    std::vector<std::string> args;
    ASTGlobalFunction(std::string name,ASTFunctionType t, ASTBody body, std::vector<std::string> args) : body(body), args(args), type(t) {
        this->name = name;
    };
    virtual void codegen() {
        auto prototype = TheModule->getFunction(name);
        if (!prototype) {
            //TODO: the static cast here may break stuff and segfault, simply split away the function declaration if this happens
            prototype = llvm::Function::Create(static_cast<llvm::FunctionType *>(type.get_type()), llvm::GlobalValue::ExternalLinkage, name, *TheModule);
        }
        llvm::BasicBlock *BB = llvm::BasicBlock::Create(*TheContext, name, prototype);
        Builder->SetInsertPoint(BB);
        LocalScope.emplace(LocalScope.end());
        int arg_offset = 0;
        auto local = LocalScope.back();
        for (auto arg = prototype->arg_begin(); arg != prototype->arg_end(); arg++) {
            auto alloca = Builder->CreateAlloca(arg->getType());
            Builder->CreateStore(arg, alloca);
            local[args[arg_offset]] = alloca;
        }
        body.codegen();
        LocalScope.pop_back();
    }
};
int main(int argc, char *argv[]) {
    //TODO: Get target triple from args?
    auto triple_name_str = llvm::sys::getDefaultTargetTriple();
    auto arch_name = std::string();    
    auto target_triple = llvm::Triple(triple_name_str);

    init_module();
    //TODO: Fill this up with the program
    std::vector<ASTGlobalEntry> program;

    //example program
    //TODO: get an actual program here
    auto args = std::vector<ASTType>();
    auto argnames = std::vector<std::string>();
    argnames.push_back("one");
    argnames.push_back("two");
    args.push_back(ASTIntType(64));
    args.push_back(ASTIntType(64));
    auto body = std::vector<ASTBase>();
    body.push_back(
        ASTReturnVal(
            ASTOperand(
                ASTGetVar("one"), 
                ASTGetVar("two"), 
                ASTOperand::ADD)
        )
    );
    program.push_back(ASTGlobalFunction(
        "add",
        ASTFunctionType(
            "add",
            args,
            ASTIntType(64)
        ),
        ASTBody(
            body
        ),
        argnames
    ));

    //PROGRAM CODEGEN
    for (auto entry = program.begin(); entry != program.end(); entry++) {
        entry->codegen();
    }


    llvm::InitializeAllTargetInfos();
    llvm::InitializeAllTargets();
    llvm::InitializeAllTargetMCs();
    //llvm::InitializeAllAsmParsers();
    llvm::InitializeAllAsmPrinters();

    std::string Error;
    auto target = llvm::TargetRegistry::lookupTarget(arch_name,target_triple, Error);
    if (!target) {
        llvm::errs() << Error;
        return -1;
    }
    TheModule->setTargetTriple(triple_name_str);
    //Boilerplate ELF emitter code from https://www.llvm.org/docs/tutorial/MyFirstLanguageFrontend/LangImpl08.html
    auto CPU = "generic";
    auto Features = "";
    llvm::TargetOptions opt;
    auto RM = llvm::Optional<llvm::Reloc::Model>();
    auto TheTargetMachine = target->createTargetMachine(triple_name_str, CPU, Features, opt, RM);
    TheModule->setDataLayout(TheTargetMachine->createDataLayout());

    //TODO: get output file name
    auto outfile = "output.o";
    std::error_code EC;
    llvm::raw_fd_ostream dest(outfile, EC, llvm::sys::fs::OF_None);

  if (EC) {
    llvm::errs() << "Could not open file: " << EC.message();
    return 1;
  }

  llvm::legacy::PassManager pass;
  auto FileType = llvm::CGFT_ObjectFile;
  //check module
    TheModule->print(llvm::errs(), nullptr);

  if (TheTargetMachine->addPassesToEmitFile(pass, dest, nullptr, FileType)) {
    llvm::errs() << "TheTargetMachine can't emit a file of this type";
    return 1;
  }

  pass.run(*TheModule);
  dest.flush();
  return 0;
}
