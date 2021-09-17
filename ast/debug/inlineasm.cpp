#include "../../ast.hpp"

void ast::InlineAsm::print_val(std::stringstream &stream) const {
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