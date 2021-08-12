#include <iostream>

#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/Support/TargetRegistry.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/Support/Host.h>
#include <llvm/Support/FileSystem.h>
#include <llvm/Support/raw_ostream.h>

#include <memory>
#include <ostream>
#include <string>
#include <vector>
std::unique_ptr<llvm::LLVMContext> TheContext;
std::unique_ptr<llvm::IRBuilder<>> Builder;
std::unique_ptr<llvm::Module> TheModule;

#include "ast.hpp"

void init_module() {
    TheContext = std::make_unique<llvm::LLVMContext>();
    TheModule = std::make_unique<llvm::Module>("cornpiler", *TheContext);
    Builder = std::make_unique<llvm::IRBuilder<>>(*TheContext);
}

std::vector<ast::GlobalEntry *> get_program() {
    //example program
    //TODO: get an actual program here
    //TODO: unspaghettify the references to unspaghettify the example code
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
}

int main(int argc, char *argv[]) {
    //TODO: Get target triple from args?
    auto triple_name_str = llvm::sys::getDefaultTargetTriple();
    auto arch_name = std::string();    
    auto target_triple = llvm::Triple(triple_name_str);

    init_module();
    //TODO: Fill this up with the program
    auto program = get_program();

    //PROGRAM CODEGEN
    for (auto entry = program.begin(); entry != program.end(); entry++) {
        (*entry)->codegen();
    }


    llvm::InitializeAllTargetInfos();
    llvm::InitializeAllTargets();
    llvm::InitializeAllTargetMCs();
    //llvm::InitializeAllAsmParsers();
    llvm::InitializeAllAsmPrinters();

    std::string Error;
    auto target = llvm::TargetRegistry::lookupTarget(arch_name,target_triple, Error);
    if (!target) {
        llvm::errs() << Error;
        return -1;
    }
    TheModule->setTargetTriple(triple_name_str);
    //Boilerplate ELF emitter code from https://www.llvm.org/docs/tutorial/MyFirstLanguageFrontend/LangImpl08.html
    auto CPU = "generic";
    auto Features = "";
    llvm::TargetOptions opt;
    auto RM = llvm::Optional<llvm::Reloc::Model>();
    auto TheTargetMachine = target->createTargetMachine(triple_name_str, CPU, Features, opt, RM);
    TheModule->setDataLayout(TheTargetMachine->createDataLayout());

    //TODO: get output file name
    auto outfile = "output.o";
    std::error_code EC;
    llvm::raw_fd_ostream dest(outfile, EC, llvm::sys::fs::OF_None);

  if (EC) {
    llvm::errs() << "Could not open file: " << EC.message();
    return 1;
  }

  llvm::legacy::PassManager pass;
  auto FileType = llvm::CGFT_ObjectFile;
  //check module
    TheModule->print(llvm::errs(), nullptr);

  if (TheTargetMachine->addPassesToEmitFile(pass, dest, nullptr, FileType)) {
    llvm::errs() << "TheTargetMachine can't emit a file of this type";
    return 1;
  }

  pass.run(*TheModule);
  dest.flush();
  return 0;
}
