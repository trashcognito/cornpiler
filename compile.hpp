#include <cstdint>
#include <vector>
#include <string>

class ast_body {
    public:
        std::vector<AST*> body;
};

class AST {
    uint8_t act;    // action: 
                    // 0:   error
                    // 1:   statement
                    // 2:   statement with body
                    // 3:   statement with two bodies
                    // 4:   varop
                    // 5:   vardef
                    // 6:   type
                    // 7:   fundef
                    // 8:   extdef
                    // 9:   glbdef
                    // 10:  call
                    // 11:  varset
                    // 12:  getvar
                    // 13:  const/str
                    // 14:  const/number
                    // 15:  oper
                    // 16:  expr
                    // 17:  arrset
                    // 18:  arrget
                    // 19:  outtype

};

namespace ast_types {

    class statement : AST {
        public:
            std::string name;
            ast_body args;
    };
    class statement_with_body : statement {
        public:
            ast_body body;
    };
    class statement_with_two_bodies : statement_with_body {
        public:
            ast_body second_body;
    };

    class varop : AST {
        public:
            std::string name;
            std::string var;
    };

    class vardef : AST {
        public:
            std::string name;
            ast_body args;
            ast_body type;
    };

    class type : AST {
        public:
            std::string type;
    };

    class out_type : AST {
        public:
            std::string name;
            ast_body type;
    };

    class extdef : AST {
        public:
            std::string name;
            ast_body args;
            ast_body return_type;
    };

    class fundef : AST {
        public:
            std::string name;
            ast_body args;
            ast_body return_type;
            ast_body body;
    };

    class glbdef : vardef {

    };

    class call : AST {
        public:
            std::string name;
            ast_body args;
    };

    class varset : AST {
        public:
            std::string name;
            ast_body args;
    };

    class getvar : AST {
        public:
            std::string name;
    };

    class const_str : AST {
        public:
            std::string value;
    };

    class const_int : AST {
        public:
            int value;
    };

    class oper : AST {
        public:
            char op;
            AST args[2];
    };

    class expr : AST {
        public:
            AST arg;
    };

    class arrset : AST {
        public:
            ast_body args;
    };

    class arrget : AST {
        public:
            AST index;
            AST array;
    };
}