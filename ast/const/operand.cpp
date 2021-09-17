#include "../../ast.hpp"

ast::ConstOperand::ConstOperand(const Const *lhs, const Const *rhs, OperandType op) {
    this->arg1 = lhs;
    this->arg2 = rhs;
    this->op = op;
}
llvm::Constant *ast::ConstOperand::codegen() const {
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