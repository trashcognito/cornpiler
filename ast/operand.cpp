#include "../ast.hpp"

ast::Operand::Operand(Value *lhs, Value *rhs, OperandType op) {
    this->arg1 = lhs;
    this->arg2 = rhs;
    this->op = op;
}
const ast::Const *ast::Operand::to_const() const {
    auto const1 = arg1->to_const();
    if (!const1) return nullptr;

    auto const2 = arg2->to_const();
    if (!const2) return nullptr;
    
    return new ast::ConstOperand(const1, const2, this->op);
}

llvm::Value *ast::Operand::codegen() const {
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