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
            std::vector<ast::Value *>({
                new ast::If(
                    new ast::Body(
                        std::vector<ast::Value *>({
                            new ast::ReturnVal(
                                new ast::IntegerConst(2, 64)
                            )
                        })
                    ), 
                    new ast::Body(
                        std::vector<ast::Value *>({
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
    //similar to above, with value if
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
            std::vector<ast::Value *>({
                new ast::ReturnVal(
                    new ast::If(
                        new ast::Body(
                            std::vector<ast::Value *>({
                                new ast::IntegerConst(2, 64)
                            })
                        ), 
                        new ast::Body(
                            std::vector<ast::Value *>({
                                new ast::IntegerConst(7, 64)
                            })
                        ), new ast::Operand(
                            new ast::GetVar("first"), 
                            new ast::IntegerConst(5, 64), 
                            ast::Operand::OperandType::GT
                        )
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

    //value while
    /*
    return std::vector<ast::GlobalEntry *>({
        new ast::GlobalPrototype(
            new ast::FunctionType(
                "putchar",
                std::vector<ast::Type *>({
                    new ast::IntType(32)
                }),
                new ast::IntType(32),
                false
            ),
            true
        ),
        new ast::GlobalPrototype(
            new ast::FunctionType(
                "getchar",
                std::vector<ast::Type *>({
                }),
                new ast::IntType(32),
                false
            ),
            true
        ),
        new ast::GlobalFunction(
            new ast::FunctionType(
                "main",
                std::vector<ast::Type *>(),
                new ast::VoidType,
                false
            ),
            new ast::Body(std::vector<ast::Value *>({
                new ast::Vardef(
                    "i",
                    new ast::IntegerConst(0, 32)
                ),
                new ast::Call(
                    "putchar",
                    std::vector<ast::Value *>({
                        new ast::While(
                            new ast::Body({
                                new ast::Vardef("c", new ast::Call("getchar", std::vector<ast::Value *>())),
                                new ast::Varset("i", new ast::Operand(
                                    new ast::GetVar("i"),
                                    new ast::IntegerConst(1, 32),
                                    ast::Operand::OperandType::ADD
                                )),
                                new ast::GetVar("c")
                            }),
                            new ast::Operand(
                                new ast::GetVar("i"),
                                new ast::IntegerConst(5, 32),
                                ast::Operand::OperandType::LT
                            )
                        )
                    })
                )
            })),
            std::vector<std::string>()
        )   
    });
    */
    /*
        extern int puts(const char *);
        void main() {
            puts("Hello, world!");
        }
    */
    /*
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
    */
    //array test
    return std::vector<ast::GlobalEntry *>({
        new ast::GlobalPrototype(
            new ast::FunctionType(
                "printf",
                std::vector<ast::Type *>({
                    new ast::PointerType(
                        new ast::IntType(8)
                    )
                }),
                new ast::IntType(32),
                true
            ),
            true
        ),
        new ast::GlobalPrototype(
            new ast::FunctionType(
                "scanf",
                std::vector<ast::Type *>({
                    new ast::PointerType(
                        new ast::IntType(8)
                    )
                }),
                new ast::IntType(32),
                true
            ),
            true
        ),
        new ast::GlobalFunction(
            new ast::FunctionType(
                "main",
                std::vector<ast::Type *>({
                    new ast::IntType(32),
                    new ast::PointerType(new ast::PointerType(new ast::IntType(32))),
                    new ast::PointerType(new ast::PointerType(new ast::IntType(32)))
                }),
                new ast::IntType(32)
            ),
            new ast::Body({
                new ast::Vardef(
                    "testarray",
                    new ast::ArrayType(
                        new ast::ArrayType(
                            new ast::IntType(32),
                            2
                        ),
                        2
                    )
                ),
                new ast::Varset(
                    "testarray",
                    new ast::ArrayConst(
                        new ast::ArrayType(new ast::IntType(32), 2),
                        std::vector<ast::Const *>({
                            new ast::ArrayConst(
                                new ast::ArrayType(new ast::IntType(32), 2),
                                std::vector<ast::Const *>({
                                    new ast::IntegerConst(1, 32),
                                    new ast::IntegerConst(2, 32)
                                })
                            ),
                            new ast::ArrayConst(
                                new ast::ArrayType(new ast::IntType(32), 2),
                                std::vector<ast::Const *>({
                                    new ast::IntegerConst(3, 32),
                                    new ast::IntegerConst(4, 32)
                                })
                            )
                        })
                    )
                ),
                new ast::Vardef(
                    "i",
                    new ast::IntType(32)
                ),
                new ast::Varset(
                    "i",
                    new ast::IntegerConst(0, 32)
                ),
                new ast::Call(
                    "scanf",
                    std::vector<ast::Value *>({
                        new ast::StringConst("%d"),
                        new ast::GetVarPtr("i")
                    })
                ),
                new ast::Call(
                    "printf",
                    std::vector<ast::Value *>({
                        new ast::StringConst("%d %d\n"),
                        new ast::GetVar("i"),
                        new ast::Arrget(
                            new ast::Arrgetptr(
                                new ast::GetVarPtr("testarray"),
                                new ast::GetVar("i")
                            ),
                            new ast::IntegerConst(1, 32)
                        )
                    })
                )
            }),
            std::vector<std::string>({
                "argc",
                "argv",
                "envp"
            })
        )
    });
}