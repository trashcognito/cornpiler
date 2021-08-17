#include "ast_front.hpp"

namespace ast_types {
    string_t::string_t() {
        this->value = "";
    }
    string_t::string_t(std::string v) {
        this->value = v;
    }
    string_t::string_t(char v) {
        this->value = std::string(1, v);
    }
    char_t::char_t() {
        this->value = '\0';
    }
    char_t::char_t(char v) {
        this->value = v;
    }
    number_t::number_t() {
        this->value = 0;
    }
    number_t::number_t(int v) {
        this->value = v;
    }
    arg_with_type_t::arg_with_type_t() {
        this->name = string_t("");
    }
}  // namespace ast_types