#include <iostream>

#include <llvm/Analysis/CGSCCPassManager.h>
#include <llvm/Analysis/LoopAnalysisManager.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/LegacyPassManager.h>
#include <llvm/IR/PassManager.h>
#include <llvm/IR/Verifier.h>
#include <llvm/Passes/PassBuilder.h>
#include <llvm/Support/CodeGen.h>
#include <llvm/Support/FileSystem.h>
#include <llvm/Support/Host.h>
#include <llvm/Support/TargetRegistry.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/Target/TargetLoweringObjectFile.h>
#include <llvm/Target/TargetMachine.h>
#include <llvm/Target/TargetOptions.h>

#include <memory>
#include <ostream>
#include <string>
#include <vector>

#include "ast.hpp"
#include "compiler_frontend.hpp"

#include "compile.hpp"

std::unique_ptr<llvm::LLVMContext> TheContext;
std::unique_ptr<llvm::IRBuilder<>> Builder;
std::unique_ptr<llvm::Module> TheModule;

void init_module() {
  TheContext = std::make_unique<llvm::LLVMContext>();
  TheModule = std::make_unique<llvm::Module>("cornpiler", *TheContext);
  Builder = std::make_unique<llvm::IRBuilder<>>(*TheContext);
}

std::vector<ast::GlobalEntry *>
translate_program(ast_types::global_scope program, logger::logger *logger) {
  std::vector<ast::GlobalEntry *> global_entries;

  auto goto_ast_scope = [&](std::vector<scope_element> in_scope) {
    logger->log(logger::LOG_LEVEL::DEBUG, "Reaching into ",
                logger::SETTINGS::TYPE);
    AST *that_ast = &(program);
    int scope_i = 0;
    for (auto s : in_scope) { // copy, do not reference
      if ((int)s >= 0) {      // int
        ast_body *_temp_body = dynamic_cast<ast_body *>(that_ast);
        std::vector<AST *> _temp_inhere = _temp_body->body;
        that_ast = _temp_inhere[(int)s];

        logger->log(logger::LOG_LEVEL::DEBUG,
                    "> [" + std::to_string((int)s) + "] ",
                    logger::SETTINGS::NONE);
      } else {
        switch ((scope_element)s) {
        case scope_element::global:
          that_ast =
              &((dynamic_cast<ast_types::global_scope *>(that_ast))->body);

          logger->log(logger::LOG_LEVEL::DEBUG, "> global",
                      logger::SETTINGS::NONE);
          break;
        case scope_element::args:
          that_ast = &((dynamic_cast<ast_types::with_args *>(that_ast))->args);

          logger->log(logger::LOG_LEVEL::DEBUG, "> args",
                      logger::SETTINGS::NONE);
          break;
        case scope_element::body:
          that_ast = &((dynamic_cast<ast_types::with_body *>(that_ast))->body);

          logger->log(logger::LOG_LEVEL::DEBUG, "> body",
                      logger::SETTINGS::NONE);
          break;
        case scope_element::second_body:
          that_ast = &((dynamic_cast<ast_types::with_second_body *>(that_ast))
                           ->second_body);

          logger->log(logger::LOG_LEVEL::DEBUG, "> second_body",
                      logger::SETTINGS::NONE);
          break;
        case scope_element::type:
          that_ast = &((dynamic_cast<ast_types::with_type *>(that_ast))->type);

          logger->log(logger::LOG_LEVEL::DEBUG, "> type",
                      logger::SETTINGS::NONE);
          break;
        case scope_element::return_type:
          that_ast = &((dynamic_cast<ast_types::with_return_type *>(that_ast))
                           ->return_type);

          logger->log(logger::LOG_LEVEL::DEBUG, "> return_type",
                      logger::SETTINGS::NONE);
          break;
        case scope_element::arr_index:
          that_ast = &((dynamic_cast<ast_types::arrget *>(that_ast))->index);

          logger->log(logger::LOG_LEVEL::DEBUG, "> arr_index",
                      logger::SETTINGS::NONE);
          break;
        case scope_element::arr_array:
          that_ast = &((dynamic_cast<ast_types::arrget *>(that_ast))->array);

          logger->log(logger::LOG_LEVEL::DEBUG, "> arr_array",
                      logger::SETTINGS::NONE);
          break;
        case scope_element::argtype:
          that_ast = &(
              (dynamic_cast<ast_types::with_args_with_type *>(that_ast))->args);

          logger->log(logger::LOG_LEVEL::DEBUG, "> argtype",
                      logger::SETTINGS::NONE);
          break;
        case scope_element::out_length:
          that_ast = &((dynamic_cast<ast_types::out_type *>(that_ast))->length);

          logger->log(logger::LOG_LEVEL::DEBUG, "> out_length",
                      logger::SETTINGS::NONE);
        default:

          logger->log(logger::LOG_LEVEL::ERROR, "\nInvalid scope element");
          exit(0);
        }
      }
    }
    logger->log(logger::LOG_LEVEL::DEBUG, "", logger::SETTINGS::NEWLINE);
    return that_ast;
  };

  auto translate_data_types = [&]() {

  };

  std::function<std::vector<ast::Value *>(std::vector<scope_element> scope)>
      recursive_translate_body = [&](std::vector<scope_element> scope) {
        ast_body *curr_scope = dynamic_cast<ast_body *>(goto_ast_scope(scope));
        std::vector<ast::Value *> args;

        int i = 0;
        for (auto e : curr_scope->body) {
          switch (((AST_node *)e)->act) {
          case act_type::statement:
            if(dynamic_cast<ast_types::statement *>(e)->name.value == "return"){
              if(dynamic_cast<ast_types::statement *>(e)->args.body.size() == 0){
                args.push_back((ast::Value*)new ast::ReturnNull());
              }else{
                std::vector<scope_element> new_scope = scope;
                new_scope.push_back((scope_element)i);
                new_scope.push_back(scope_element::args);
                args.push_back((ast::Value*)new ast::ReturnVal(recursive_translate_body(new_scope)[0]));
              }
            }
          break;
          case act_type::statement_with_body:
            if(dynamic_cast<ast_types::statement_with_body *>(e)->name.value == "while"){
                std::vector<scope_element> new_scope_args = scope;
                new_scope_args.push_back((scope_element)i);
                new_scope_args.push_back(scope_element::args);

                std::vector<scope_element> new_scope_body = scope;
                new_scope_body.push_back((scope_element)i);
                new_scope_body.push_back(scope_element::body);

                args.push_back((ast::Value*)new ast::While(
                  recursive_translate_body(new_scope_body),
                  recursive_translate_body(new_scope_args)[0]));
            }
          break;
          }
          i++;
        }
        return args;
      };

  auto recursive_translate_global = [&] {
    ast_body *curr_scope =
        dynamic_cast<ast_body *>(goto_ast_scope({scope_element::global}));
    for (auto &e : curr_scope->body) {
      switch (((AST_node *)e)->act) {
      case act_type::fundef:
        return (ast::GlobalEntry *)new ast::GlobalFunction(
            new ast::FunctionType(
                dynamic_cast<ast_types::fundef *>(e)->name.value,
                std::vector<ast::Type *>(), new ast::IntType(32)),
            new ast::Body({}), std::vector<std::string>());
        break;
      }
    }
  };

  recursive_translate_global();
  return global_entries;
}

int main(int argc, char *argv[]) {

  logger::logger logger(logger::LOG_LEVEL::DEBUG | logger::LOG_LEVEL::INFO |
                        logger::LOG_LEVEL::WARNING | logger::LOG_LEVEL::ERROR |
                        logger::LOG_LEVEL::NONE);

  // TODO: Get target triple from args?
  auto triple_name_str = llvm::sys::getDefaultTargetTriple();
  auto arch_name = std::string();
  auto target_triple = llvm::Triple(triple_name_str);

  init_module();
  // TODO: Fill this up with the program
  auto program = get_program(&logger);

  // PROGRAM CODEGEN
  for (auto entry = program.begin(); entry != program.end(); entry++) {
    (*entry)->codegen();
  }

  llvm::InitializeAllTargetInfos();
  llvm::InitializeAllTargets();
  llvm::InitializeAllTargetMCs();
  // llvm::InitializeAllAsmParsers();
  llvm::InitializeAllAsmPrinters();

  std::string Error;
  auto target =
      llvm::TargetRegistry::lookupTarget(arch_name, target_triple, Error);
  if (!target) {
    llvm::errs() << Error;
    return -1;
  }
  TheModule->setTargetTriple(triple_name_str);
  // Boilerplate ELF emitter code from
  // https://www.llvm.org/docs/tutorial/MyFirstLanguageFrontend/LangImpl08.html
  auto CPU = "generic";
  auto Features = "";
  llvm::TargetOptions opt;
  auto RM = llvm::Optional<llvm::Reloc::Model>(llvm::Reloc::Model::PIC_);
  auto TheTargetMachine =
      target->createTargetMachine(triple_name_str, CPU, Features, opt, RM);
  TheModule->setDataLayout(TheTargetMachine->createDataLayout());
  llvm::verifyModule(*TheModule, &llvm::errs());
  // OPTIMIZATION PASSES
  // pre-opt print
  TheModule->print(llvm::errs(), nullptr);

  llvm::PassBuilder pb;
  llvm::LoopAnalysisManager lam(true);
  llvm::FunctionAnalysisManager fam(true);
  llvm::CGSCCAnalysisManager cgsccam(true);
  llvm::ModuleAnalysisManager mam(true);

  pb.registerModuleAnalyses(mam);
  pb.registerCGSCCAnalyses(cgsccam);
  pb.registerFunctionAnalyses(fam);
  pb.registerLoopAnalyses(lam);

  pb.crossRegisterProxies(lam, fam, cgsccam, mam);

  llvm::ModulePassManager module_pass_manager =
      pb.buildPerModuleDefaultPipeline(
          llvm::PassBuilder::OptimizationLevel::O2);

  module_pass_manager.run(*TheModule, mam);
  // re-verify post optimization
  llvm::verifyModule(*TheModule, &llvm::errs());
  // TODO: get output file name
  auto outfile = "output.o";
  std::error_code EC;
  llvm::raw_fd_ostream dest(outfile, EC, llvm::sys::fs::OF_None);

  if (EC) {
    llvm::errs() << "Could not open file: " << EC.message();
    return 1;
  }

  llvm::legacy::PassManager pass;
  auto FileType = llvm::CGFT_ObjectFile;
  // check module
  TheModule->print(llvm::errs(), nullptr);

  if (TheTargetMachine->addPassesToEmitFile(pass, dest, nullptr, FileType)) {
    llvm::errs() << "TheTargetMachine can't emit a file of this type";
    return 1;
  }

  pass.run(*TheModule);
  dest.flush();
  llvm::outs() << "Wrote output to " << outfile << "\n";
  return 0;
}
