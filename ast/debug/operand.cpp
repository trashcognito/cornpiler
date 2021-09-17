#include "../../ast.hpp"

void ast::Operand::print_val(std::stringstream &stream) const {
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