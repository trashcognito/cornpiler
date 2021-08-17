#pragma once

#include <cstdint>
#include <iostream>
#include <string>
#include <variant>
#include <vector>

// #define DEBUG

bool check_id_constraints(std::string id, char c);

enum class scope_element : int {
    global = -1,
    args = -2,
    body = -3,
    second_body = -4,
    type = -5,
    return_type = -6,
    arr_index = -7,
    arr_array = -8,
    argtype = -9,
    out_length = -10,
};

enum class token_type {
    string,
    identifier,
    number,
    bracket,
    semi,
    sep,
    sym
};

class entry_bracket {
    public:
    char first;
    char second;
    entry_bracket(char f, char s) {
        first = f;
        second = s;
    }
    entry_bracket() {
        first = 0;
        second = 0;
    }
};

class token {
    public:
    token_type type;
    std::string value;

    int row;
    int col;

    int chr;

    token(token_type t, std::string v, int r, int c, int ch) {  // god forgive me for writing a function inside a header file
        type = t;
        value = v;
        row = r;
        col = c;
        chr = ch;
    }
};

std::vector<token> tokenize_program(std::string program, int length);