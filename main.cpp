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

#include <iostream>
#include <memory>
#include <ostream>
#include <string>
#include <vector>

#include "ast.hpp"
#include "compile.hpp"
#include "compiler_frontend.hpp"
#include "parse_cli.hpp"

std::unique_ptr<llvm::LLVMContext> TheContext;
std::unique_ptr<llvm::IRBuilder<>> Builder;
std::unique_ptr<llvm::Module> TheModule;

void init_module() {
  TheContext = std::make_unique<llvm::LLVMContext>();
  TheModule = std::make_unique<llvm::Module>("cornpiler", *TheContext);
  Builder = std::make_unique<llvm::IRBuilder<>>(*TheContext);
}

int main(int argc, char *argv[]) {
  //TODO: Handle help
  ArgParse args({
      Arg('h', "help", ArgType::flag),
      Arg('v', "version", ArgType::flag),
      Arg('O', "optimize", ArgType::value),
      Arg('o', "output", ArgType::val_always_sep),
  });

  args.parse_args(argc, argv);

  logger::logger logger(logger::LOG_LEVEL::DEBUG | logger::LOG_LEVEL::INFO |
                        logger::LOG_LEVEL::WARNING | logger::LOG_LEVEL::ERROR |
                        logger::LOG_LEVEL::NONE);

  // TODO: Get target triple from args?
  auto triple_name_str = llvm::sys::getDefaultTargetTriple();
  auto arch_name = std::string();
  auto target_triple = llvm::Triple(triple_name_str);

  init_module();
  // TODO: Fill this up with the program

  if (args.files.size() == 0) {
    logger.log(logger::LOG_LEVEL::ERROR, "No files given in command line!");
    exit(-1);
  }
  //TODO: Maybe handle better?
  if (args.files.size() > 1) {
    logger.log(logger::LOG_LEVEL::ERROR, "Too many files given in command line!");
    exit(-1);
  }
  llvm::PassBuilder::OptimizationLevel optim_level;
  if (args.adds.count("optimize")) {
    //optimize arg given - parse optimize arg
    //TODO: Error checking needed here
    //TODO: Optimize for size should also be parsed!
    auto opt = std::stoi(args.adds["optimize"]);
    if (opt < 0) {
      //TODO: Better error message
      logger.log(logger::LOG_LEVEL::ERROR, "Invalid optimizer level!");
      exit(-1);
    }
    switch (opt) {
      case 0:
        optim_level = llvm::PassBuilder::OptimizationLevel::O0;
        break;
      case 1:
        optim_level = llvm::PassBuilder::OptimizationLevel::O1;
        break;
      case 2:
        optim_level = llvm::PassBuilder::OptimizationLevel::O2;
        break;
      case 3:
        optim_level = llvm::PassBuilder::OptimizationLevel::O3;
        break;
      default:
        //Assumes O > 3
        // This is a bad idea, can we please reconsider?
        optim_level = llvm::PassBuilder::OptimizationLevel::O3;
        break;
    }
  } else {
    //No argument given, -O2 by default
    //TODO: Maybe reconsider?
    optim_level = llvm::PassBuilder::OptimizationLevel::O2;
  }
  std::string output_file;
  if (args.adds.count("output")) {
    output_file = args.adds["output"];
  } else {
    //TODO: maybe just generate a.out instead?
    logger.log(logger::LOG_LEVEL::ERROR, "No output file name given!");
    exit(-1);
  }

  auto program = get_program(&logger, args.files[0]);

  std::string str_program = "";
  std::stringstream program_ast(str_program);
  print_program_to(program, program_ast);
  std::cout << "" << program_ast.str() << std::endl;

  // PROGRAM CODEGEN
  for (auto entry = program.begin(); entry != program.end(); entry++) {
    (*entry)->codegen();
  }

  llvm::InitializeAllTargetInfos();
  llvm::InitializeAllTargets();
  llvm::InitializeAllTargetMCs();
  llvm::InitializeAllAsmParsers();
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
          optim_level);

  module_pass_manager.run(*TheModule, mam);
  // re-verify post optimization
  llvm::verifyModule(*TheModule, &llvm::errs());
  // TODO: get output file name
  std::error_code EC;
  llvm::raw_fd_ostream dest(output_file, EC, llvm::sys::fs::OF_None);

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
  llvm::outs() << "Wrote output to " << output_file << "\n";
  return 0;
}
