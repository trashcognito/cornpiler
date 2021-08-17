#pragma once

#include <vector>
#include <string>
class AST {
    public:
    virtual ~AST() = 0;
};

class AST_node {
    public:
};

class ast_body : virtual public AST {
    public:
    std::vector<AST*> body;
};

namespace ast_types {

    #pragma region
    class string_t : virtual public AST {
    public:
    std::string value;
    string_t();
    string_t(std::string v);
    string_t(char v);
    };
    class char_t : virtual public AST {
    public:
    char value;
    char_t();
    char_t(char v);
    };
    class number_t : virtual public AST {
    public:
    int value;
    number_t();
    number_t(int v);
    };
    #pragma endregion standard_types_as_ast

    #pragma region
    class with_args : virtual public AST {
    public:
    ast_body args;
    };
    class with_body : virtual public AST {
    public:
    ast_body body;
    };
    class with_second_body : virtual public AST {
    public:
    ast_body second_body;
    };
    class with_type : virtual public AST {
    public:
    ast_body type;
    };
    class with_return_type : virtual public AST {
    public:
    ast_body return_type;
    };

    class arg_with_type_t : public with_type, virtual public AST {
    public:
    string_t name;
    arg_with_type_t();
    };
    class with_args_with_type : virtual public AST {
    public:
    ast_body args;
    };
    #pragma endregion ast_global_types

    class global_scope : public with_body {
        
    };

    class statement : public with_args, public AST_node {
    public:
    string_t name;
    };
    class statement_with_body : public with_body, public with_args, public AST_node {
    public:
    string_t name;
    };
    class statement_with_two_bodies : public with_body, public with_second_body, public with_args, public AST_node {
    public:
    string_t name;
    };

    class varop : virtual public AST, public AST_node {
    public:
    string_t name;
    string_t var;
    };

    class vardef : public with_args, public with_type, public AST_node {
    public:
    string_t name;
    };

    class in_type : public AST_node, public AST {
    public:
    string_t type;
    };

    class out_type : public with_type, public AST_node {
    public:
    string_t name;
    ast_body length;
    };

    class extdef : public with_args_with_type, public with_return_type, public AST_node {
    public:
    string_t name;
    };

    class fundef : public with_args_with_type, public with_body, public with_return_type, public AST_node {
    public:
    string_t name;
    };

    class glbdef : public with_args, public with_type, public AST_node {
    public:
    string_t name;
    };

    class call : public with_args, public AST_node {
    public:
    string_t name;
    };

    class varset : public with_args, public AST_node {
    public:
    string_t name;
    };

    class getvar : virtual public AST, public AST_node {
    public:
    string_t name;
    };

    class const_str : virtual public AST, public AST_node {
    public:
    string_t value;
    };

    class const_int : virtual public AST, public AST_node {
    public:
    number_t value;
    };

    class oper : public with_args, public AST_node {
    public:
    string_t op;
    };

    class expr : public with_args, public AST_node {
    public:
    };

    class arrset : public with_args, public AST_node {
    public:
    };

    class arrget : virtual public AST, public AST_node {
    public:
    ast_body index;
    ast_body array;
    };
}  // namespace ast_types