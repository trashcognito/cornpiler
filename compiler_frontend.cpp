#include "ast.hpp"
std::vector<ast::GlobalEntry *> get_program() {
    //example program
    //TODO: get an actual program here
    //TODO: unspaghettify the references to unspaghettify the example code

    /*
        uint64_t add(uint64_t one, uint64_t two) {
            return one + two;
        }
    */
    /*
    return std::vector<ast::GlobalEntry *> ({
        new ast::GlobalFunction(
        new ast::FunctionType(
            "add",
            std::vector<ast::Type *>(
                {
                    new ast::IntType(64), 
                    new ast::IntType(64)
                }
            ),
            new ast::IntType(64)
        ),
        new ast::Body(
            std::vector<ast::Base *>({new ast::ReturnVal(
            new ast::Operand(
                new ast::GetVar("one"), 
                new ast::GetVar("two"), 
                ast::Operand::ADD)
            )})
        ),
        std::vector<std::string>(
            {
                "one", 
                "two"
            }
        )
    )});
    */

    /*
        uint64_t some_thing(uint64_t first) {
            if (first > 5) {
                return 2;
            } else {
                return 7;
            }
        }
    */
    /*
    return std::vector<ast::GlobalEntry *> ({
        new ast::GlobalFunction(
        new ast::FunctionType(
            "some_thing",
            std::vector<ast::Type *>(
                {
                    new ast::IntType(64)
                }
            ),
            new ast::IntType(64)
        ),
        new ast::Body(
            std::vector<ast::Base *>({
                new ast::If(
                    new ast::Body(
                        std::vector<ast::Base *>({
                            new ast::ReturnVal(
                                new ast::IntegerConst(2, 64)
                            )
                        })
                    ), 
                    new ast::Body(
                        std::vector<ast::Base *>({
                            new ast::ReturnVal(
                                new ast::IntegerConst(7, 64)
                            )
                        })
                    ), new ast::Operand(
                        new ast::GetVar("first"), 
                        new ast::IntegerConst(5, 64), 
                        ast::Operand::OperandType::GT
                        )
                )
            })
        ),
        std::vector<std::string>(
            {
                "first"
            }
        )
    )});
    */
    
    /*
        extern int puts(const char *);
        void main() {
            puts("Hello, world!");
        }
    */
    
    return std::vector<ast::GlobalEntry *>(
        {
            new ast::GlobalPrototype(
                new ast::FunctionType(
                    "puts",
                    std::vector<ast::Type *>({
                        new ast::PointerType(
                            new ast::IntType(8)
                        )
                    }),
                    new ast::IntType(32)
                ),
                true
            ),
            new ast::GlobalFunction(
                new ast::FunctionType(
                    "main",
                    std::vector<ast::Type *>({}),
                    new ast::VoidType()
                ),
                new ast::Body(
                    std::vector<ast::Base *> ({
                        new ast::BaseCall(
                            "puts",
                            std::vector<ast::Value *>({
                                new ast::StringConst(
                                    "Hello World"
                                )
                            })
                        ),
                        new ast::ReturnNull()
                    })
                ),
                std::vector<std::string>()
            )
        }
    );
    
}