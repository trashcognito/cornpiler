#include "../../ast.hpp"
void ast::print_program_to(std::vector<GlobalEntry *> &program, std::stringstream &stream) {
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