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
#include <sstream>

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
        virtual void print_val(std::stringstream &) const = 0;
        //TODO: maybe make this pure virtual as well?
        virtual const Const* to_const() const;
    };

    using ValueArray=std::vector<Value *>;

    class Varset : public Value {
        public:
        std::string name;
        Value *val;
        llvm::Value *codegen() const;
        Varset(std::string name, Value *val);
        void print_val(std::stringstream &) const;
    };

    class Body : public Value {
        public:
        std::vector<Value *> body;
        llvm::Value *codegen() const;
        Body(std::vector<Value *> body);
        void print_val(std::stringstream &) const;
    };

    class While : public Value {
        public:
        Body *body;
        Value *condition;
        llvm::Value *codegen() const;
        While(Body *body_a, Value *condition);
        void print_val(std::stringstream &) const;
    };

    class If : public Value {
        public:
        Body *body_t;
        Body *body_f;
        Value *condition;
        llvm::Value *codegen() const;
        If(Body *body_if,Body *body_else, Value *condition);
        void print_val(std::stringstream &) const;
    };

    class ReturnVal : public Value {
        public:
        Value *val;
        llvm::Value *codegen() const;
        ReturnVal(Value *val);
        void print_val(std::stringstream &) const;
    };

    class ReturnNull : public Value {
        public:
        llvm::Value *codegen() const;
        void print_val(std::stringstream &) const;
    };

    class Type {
        public:
        virtual llvm::Type *get_type() const = 0;
        virtual void print_type(std::stringstream &) const = 0;
    };

    class Vardef : public Value {
        public:
        std::string varname;
        Type *ty;
        llvm::Value *codegen() const;
        Vardef(std::string varname, Type *type);
        void print_val(std::stringstream &) const;
    };

    class IntType : public Type {
        public:
        int bits;
        IntType(int bits);
        llvm::Type *get_type() const;
        void print_type(std::stringstream &) const;
    };

    class FloatType : public Type {
        public:
        FloatType();
        llvm::Type *get_type() const;
        void print_type(std::stringstream &) const;
    };

    class StringType : public Type {
        public:
        int length;
        StringType(int length);
        llvm::Type *get_type() const;
        void print_type(std::stringstream &) const;
    };

    class ArrayType : public Type {
        public:
        int length;
        Type *inside;
        ArrayType(Type *inside, int len);
        llvm::Type *get_type() const;
        void print_type(std::stringstream &) const;
    };

    class PointerType : public Type {
        public:
        Type *to;
        PointerType(Type *to);
        llvm::Type *get_type() const;
        void print_type(std::stringstream &) const;
    };

    class VoidType : public Type {
        public:
        VoidType();
        llvm::Type *get_type() const;
        void print_type(std::stringstream &) const;
    };

    class FunctionType : public Type {
        public:
        std::string name;
        Type *return_type;
        std::vector<Type *> args;
        bool varargs;
        FunctionType(std::string name, std::vector<Type *> args, Type *return_type, bool varargs=false);
        llvm::Type *get_type() const;
        void print_type(std::stringstream &) const;
    };
    class ArrDef : public Value {
        public:
        Type *inner_type;
        Value *length;
        std::string name;
        ArrDef(std::string name, Type *inner, Value *len);
        llvm::Value *codegen() const;
        void print_val(std::stringstream &) const;
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
        const Const *arg1;
        const Const *arg2;
        llvm::Constant *codegen() const;
        ConstOperand(const Const *lhs, const Const *rhs, OperandType op);
        void print_val(std::stringstream &) const;
    };
    class ConstUnaryOp : public Const {
        public:
        const Const *arg;
        UOps op;
        llvm::Constant *codegen() const;
        ConstUnaryOp(UOps operand, const Const *arg);
        void print_val(std::stringstream &) const;
    };
    class IntegerConst : public Const {
        public:
        intmax_t from;
        int bits;
        llvm::Constant *codegen() const;
        IntegerConst(intmax_t i, int bits);
        void print_val(std::stringstream &) const;
    };
    //todo: add different floats? maybe template?
    class FloatConst : public Const {
        public:
        float from;
        llvm::Constant *codegen() const;
        FloatConst(float f);
        void print_val(std::stringstream &) const;
    };
    //TODO: template specializations
    class ArrayConst : public Const {
        public:
        std::vector<Const *> getfrom;
        llvm::Constant *codegen() const;
        Type *t;
        ArrayConst(Type *subtype,std::vector<Const *> from);
        void print_val(std::stringstream &) const;
    };
    class StringConst : public Const {
        public:
        std::string orig;
        llvm::Constant *codegen() const;
        StringConst(std::string from);
        void print_val(std::stringstream &) const;
    };

    class Operand : public Value {
        public:
        
        OperandType op;
        Value *arg1;
        Value *arg2;
        llvm::Value *codegen() const;
        const Const* to_const() const;
        Operand(Value *lhs, Value *rhs, OperandType op);
        void print_val(std::stringstream &) const;
    };

    class Call : public Value {
        public:
        std::string function_name;
        ValueArray argvector;
        llvm::Value *codegen() const;
        Call(std::string name, ValueArray args);
        void print_val(std::stringstream &) const;
    };

    class GetVar : public Value {
        public:
        std::string var_name;
        llvm::Value *codegen() const;
        GetVar(std::string name);
        void print_val(std::stringstream &) const;
    };

    class GetVarPtr : public Value {
        public:
        std::string var_name;
        llvm::Value *codegen() const;
        GetVarPtr(std::string name);
        void print_val(std::stringstream &) const;
    };

    class Deref : public Value {
        public:
        Value *p;
        llvm::Value *codegen() const;
        Deref(Value *ptr);
        void print_val(std::stringstream &) const;
    };

    class UnaryOp : public Value {
        public:
        Value *arg;
        
        UOps op;
        llvm::Value *codegen() const;
        const Const* to_const() const;
        UnaryOp(UOps operand, Value *arg);
        void print_val(std::stringstream &) const;
    };
    class Arrget : public Value {
        public:
        Value *array;
        Value *index;
        llvm::Value *codegen() const;
        Arrget(Value *array, Value *index);
        void print_val(std::stringstream &) const;
    };
    class Arrset : public Value {
        public:
        Value *array;
        Value *index;
        Value *val;
        llvm::Value *codegen() const;
        Arrset(Value *array, Value *index, Value *val);
        void print_val(std::stringstream &) const;
    };
    class Arrgetptr : public Value {
        public:
        Value *array;
        Value *index;
        llvm::Value *codegen() const;
        Arrgetptr(Value *array, Value *indexes);
        void print_val(std::stringstream &) const;
    };
    class Expr : public Value {
        public:
        Value *actual;
        llvm::Value *codegen() const;
        const Const* to_const() const;
        Expr(Value *inner);
        void print_val(std::stringstream &) const;
    };
    class Bitcast : public Value {
        public:
        Value *thing;
        Type *type;
        llvm::Value *codegen() const;
        const Const* to_const() const;
        Bitcast(Value *thing, Type *type);
        void print_val(std::stringstream &) const;
    };
    class ConstBitcast : public Const {
        public:
        const Const *thing;
        Type *type;
        llvm::Constant *codegen() const;
        ConstBitcast(const Const *thing, Type *type);
        void print_val(std::stringstream &) const;
    };

    class InlineAsm : public Value {
        public:
        llvm::Value *codegen() const;
        std::vector<Value *> args;
        //TODO: merge inside AST?
        std::string constraints;
        //TODO: get asm string?
        std::string asmstring;
        bool is_volatile;
        bool is_align_stack;
        //TODO: The constructor is probably gonna need to be changed
        InlineAsm(std::string assembly, std::string constraints, std::vector<Value *> args, bool is_volatile, bool is_align_stack);

        void print_val(std::stringstream &) const;
    };

    class GlobalEntry {
        public:
        virtual void codegen() const = 0;
        std::string name;
        virtual void print_global(std::stringstream &) const = 0;
    };

    //Extern declarations
    class GlobalPrototype : public GlobalEntry {
        public:
        Type *type;
        bool constant;
        GlobalPrototype(std::string name, Type *type, bool is_constant=false);
        void codegen() const;
        void print_global(std::stringstream &) const;
    };

    class GlobalFunction : public GlobalEntry {
        public:
        Body *body;
        FunctionType *type;
        std::vector<std::string> args;
        GlobalFunction(FunctionType *t, Body *body, std::vector<std::string> args);
        void codegen() const;
        void print_global(std::stringstream &) const;
    };

    class GlobalVariable : public GlobalEntry {
        public:
        bool constant;
        const Const *value;
        GlobalVariable(std::string name, const Const *value, bool is_const=true);
        void codegen() const;
        void print_global(std::stringstream &) const;
    };

    void print_program_to(std::vector<GlobalEntry *> &, std::stringstream &);
}