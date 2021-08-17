#pragma once

#include <cstdint>
#include <llvm/ADT/APFloat.h>
#include <llvm/ADT/APInt.h>
#include <llvm/ADT/StringRef.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/Type.h>
#include <llvm/Support/CodeGen.h>
#include <llvm/Support/raw_ostream.h>
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

extern std::unique_ptr<llvm::LLVMContext> TheContext;
extern std::unique_ptr<llvm::IRBuilder<>> Builder;
extern std::unique_ptr<llvm::Module> TheModule;
namespace ast {
    class Base {
        public:
        virtual void codegen() const = 0;
    };

    class Value {
        public:
        virtual llvm::Value *codegen() const = 0;
    };

    using ValueArray=std::vector<Value *>;

    class BaseVardef : public Base {
        public:
        std::string varname;
        Value *val;
        void codegen() const;
        BaseVardef(std::string varname, Value *val);
    };

    class BaseCall : public Base {
        public:
        std::string function_name;
        ValueArray argvector;
        void codegen() const;
        BaseCall(std::string name, ValueArray args);
    };

    class BaseVarset : public Base {
        public:
        std::string name;
        Value *val;
        void codegen() const;
        BaseVarset(std::string name, Value *val);
    };

    class Body : public Base {
        public:
        std::vector<Base *> body;
        void codegen() const;
        Body(std::vector<Base *> body);
    };

    class While : public Base {
        public:
        Body *body;
        Value *condition;
        void codegen() const;
        While(Body *body_a, Value *condition);
    };

    class If : public Base {
        public:
        Body *body_t;
        Body *body_f;
        Value *condition;
        void codegen() const;
        If(Body *body_if,Body *body_else, Value *condition);
    };

    class ReturnVal : public Base {
        public:
        Value *val;
        void codegen() const;
        ReturnVal(Value *val);
    };

    class ReturnNull : public Base {
        public:
        void codegen() const;
    };

    class Type {
        public:
        std::string name;
        virtual llvm::Type *get_type() const = 0;
    };

    class IntType : public Type {
        public:
        int bits;
        IntType(int bits);
        llvm::Type *get_type() const;
    };

    class StringType : public Type {
        public:
        int length;
        StringType(int length);
        llvm::Type *get_type() const;
    };

    class ArrayType : public Type {
        public:
        int length;
        Type *inside;
        ArrayType(Type *inside, int len);
        llvm::Type *get_type() const;
    };

    class PointerType : public Type {
        public:
        Type *to;
        PointerType(Type *to);
        llvm::Type *get_type() const;
    };

    class VoidType : public Type {
        public:
        VoidType();
        llvm::Type *get_type() const;
    };

    class FunctionType : public Type {
        public:
        Type *return_type;
        std::vector<Type *> args;
        bool varargs;
        FunctionType(std::string name, std::vector<Type *> args, Type *return_type, bool varargs=false);
        llvm::Type *get_type() const;
    };
    //float
    //integer
    //array
    //string

    //maybe pointer const?
    class Const : public Value {
        public:
        llvm::Constant *codegen() const = 0;
    };
    class IntegerConst : public Const {
        public:
        intmax_t from;
        int bits;
        llvm::Constant *codegen() const;
        IntegerConst(intmax_t i, int bits);
    };
    //todo: add different floats? maybe template?
    class FloatConst : public Const {
        public:
        float from;
        llvm::Constant *codegen() const;
        FloatConst(float f);
    };
    //TODO: template specializations
    class ArrayConst : public Const {
        public:
        std::vector<Const *> getfrom;
        llvm::Constant *codegen() const;
        Type *t;
        ArrayConst(Type *subtype,std::vector<Const *> from);
    };
    class StringConst : public Const {
        public:
        std::string orig;
        llvm::Constant *codegen() const;
        StringConst(std::string from);
    };

    class Operand : public Value {
        public:
        enum OperandType {
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
        OperandType op;
        Value *arg1;
        Value *arg2;
        llvm::Value *codegen() const;
        Operand(Value *lhs, Value *rhs, OperandType op);
    };

    class ValueCall : public Value {
        public:
        std::string function_name;
        ValueArray argvector;
        llvm::Value *codegen() const;
        ValueCall(std::string name, ValueArray args);
    };

    class GetVar : public Value {
        public:
        std::string var_name;
        llvm::Value *codegen() const;
        GetVar(std::string name);
    };

    class GetVarPtr : public Value {
        public:
        std::string var_name;
        llvm::Value *codegen() const;
        GetVarPtr(std::string name);
    };

    class UnaryOp : public Value {
        public:
        Value *arg;
        enum UOps {
            NOT
        };
        UOps op;
        llvm::Value *codegen() const;
        UnaryOp(UOps operand, Value *arg);
    };

    class GlobalEntry {
        public:
        virtual void codegen() const = 0;
        std::string name;
    };

    //Extern declarations
    class GlobalPrototype : public GlobalEntry {
        public:
        Type *type;
        bool constant;
        GlobalPrototype(Type *type, bool is_constant=false);
        void codegen() const;
    };

    class GlobalFunction : public GlobalEntry {
        public:
        Body *body;
        FunctionType *type;
        std::vector<std::string> args;
        GlobalFunction(FunctionType *t, Body *body, std::vector<std::string> args);
        void codegen() const;
    };

    class GlobalVariable : public GlobalEntry {
        public:
        bool constant;
        Const *value;
        GlobalVariable(std::string name, Const *value, bool is_const=true);
        void codegen() const;
    };
}