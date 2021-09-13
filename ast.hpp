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
    enum class OperandType {
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
    enum class UOps {
        NOT,
        NEG
    };

    class Const;
    class Value {
        public:
        virtual llvm::Value *codegen() const = 0;
        virtual Const* to_const();
    };

    using ValueArray=std::vector<Value *>;

    class Varset : public Value {
        public:
        std::string name;
        Value *val;
        llvm::Value *codegen() const;
        Varset(std::string name, Value *val);
    };

    class Body : public Value {
        public:
        std::vector<Value *> body;
        llvm::Value *codegen() const;
        Body(std::vector<Value *> body);
    };

    class While : public Value {
        public:
        Body *body;
        Value *condition;
        llvm::Value *codegen() const;
        While(Body *body_a, Value *condition);
    };

    class If : public Value {
        public:
        Body *body_t;
        Body *body_f;
        Value *condition;
        llvm::Value *codegen() const;
        If(Body *body_if,Body *body_else, Value *condition);
    };

    class ReturnVal : public Value {
        public:
        Value *val;
        llvm::Value *codegen() const;
        ReturnVal(Value *val);
    };

    class ReturnNull : public Value {
        public:
        llvm::Value *codegen() const;
    };

    class Type {
        public:
        std::string name;
        virtual llvm::Type *get_type() const = 0;
    };

    class Vardef : public Value {
        public:
        std::string varname;
        Type *ty;
        llvm::Value *codegen() const;
        Vardef(std::string varname, Type *type);
    };

    class IntType : public Type {
        public:
        int bits;
        IntType(int bits);
        llvm::Type *get_type() const;
    };

    class FloatType : public Type {
        public:
        FloatType();
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
    class ArrDef : public Value {
        public:
        Type *inner_type;
        Value *length;
        std::string name;
        ArrDef(std::string name, Type *inner, Value *len);
        llvm::Value *codegen() const;
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
    class ConstOperand : public Const {
        public:
        OperandType op;
        Const *arg1;
        Const *arg2;
        llvm::Constant *codegen() const;
        ConstOperand(Const *lhs, Const *rhs, OperandType op);
    };
    class ConstUnaryOp : public Const {
        public:
        Const *arg;
        UOps op;
        llvm::Constant *codegen() const;
        ConstUnaryOp(UOps operand, Const *arg);
    };
    class ValueConst : public Value {
        public:
        Value *value;
        llvm::Constant *codegen() const;
        ValueConst(Value* val);
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
        
        OperandType op;
        Value *arg1;
        Value *arg2;
        llvm::Value *codegen() const;
        Const* to_const();
        Operand(Value *lhs, Value *rhs, OperandType op);
    };

    class Call : public Value {
        public:
        std::string function_name;
        ValueArray argvector;
        llvm::Value *codegen() const;
        Call(std::string name, ValueArray args);
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

    class Deref : public Value {
        public:
        Value *p;
        llvm::Value *codegen() const;
        Deref(Value *ptr);
    };

    class UnaryOp : public Value {
        public:
        Value *arg;
        
        UOps op;
        llvm::Value *codegen() const;
        Const* to_const();
        UnaryOp(UOps operand, Value *arg);
    };
    class Arrget : public Value {
        public:
        Value *array;
        Value *index;
        llvm::Value *codegen() const;
        Arrget(Value *array, Value *index);
    };
    class Arrset : public Value {
        public:
        Value *array;
        Value *index;
        Value *val;
        llvm::Value *codegen() const;
        Arrset(Value *array, Value *index, Value *val);
    };
    class Arrgetptr : public Value {
        public:
        Value *array;
        Value *index;
        llvm::Value *codegen() const;
        Arrgetptr(Value *array, Value *indexes);
    };
    class Expr : public Value {
        public:
        Value *actual;
        llvm::Value *codegen() const;
        Const* to_const();
        Expr(Value *inner);
    };
    class Bitcast : public Value {
        public:
        Value *thing;
        Type *type;
        llvm::Value *codegen() const;
        Const *to_const();
        Bitcast(Value *thing, Type *type);
    };
    class ConstBitcast : public Const {
        public:
        Const *thing;
        Type *type;
        llvm::Constant *codegen() const;
        ConstBitcast(Const *thing, Type *type);
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