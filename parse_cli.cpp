#include <iostream>
#include <ostream>
#include <string>
#include <vector>
#include <map>

#include "parse_cli.hpp"

Arg::Arg(char sn, std::string ln, ArgType t) {
  short_name = sn;
  long_name = ln;
  type = t;
}

ArgParse::ArgParse(std::vector<Arg> a) {
  args = a;
}

void ArgParse::parse_args(int argc, char* argv[]) {
    for (int i = 1; i < argc; i++) {
      if(std::string(argv[i]) == ""){
        continue;
      }
      if (argv[i][0] == '-') {
        if (argv[i][1] == '-') {
          bool found = false;
          for (int j = 0; j < args.size(); j++) {
            if (args[j].type == ArgType::flag) {
              if (std::string(argv[i]) == ("--" + args[j].long_name)) {
                adds[args[j].long_name] = "";
                found = true;
                break;
              }
            } else if (args[j].type == ArgType::value | args[j].type == ArgType::val_always_sep) {
              if (std::string(argv[i]) == ("--" + args[j].long_name)) {
                if(++i >= argc) {
                  std::cout << "Error: flag '" << args[j].long_name << "' requires a value" << std::endl;
                  exit(-1);
                }
                adds[args[j].long_name] = argv[i];
                found = true;
                break;
              }
            }
          }
          if(!found){
            std::cout << "Error: unknown flag '" << argv[i] << "'" << std::endl;
            exit(-1);
          }
        } else {
          bool found = false;
          for (int j = 0; j < args.size(); j++) {
            if (argv[i][1] == args[j].short_name) {
              if (args[j].type == ArgType::flag) {
                adds[args[j].long_name] = "";
                found = true;
                break;
              }else if (args[j].type == ArgType::value) {
                if(argv[i][2] == '\0'){
                  std::cout << "Error: flag '" << args[j].long_name << "' requires a value" << std::endl;
                  exit(-1);
                }
                adds[args[j].long_name] = std::string(argv[i]).substr(2);
                found = true;
                break;
              }else if(args[j].type == ArgType::val_always_sep){
                if(++i >= argc) {
                  std::cout << "Error: flag '" << args[j].long_name << "' requires a value" << std::endl;
                  exit(-1);
                }
                adds[args[j].long_name] = argv[i];
                found = true;
                break;
              }
            }
          }
          if(!found){
            std::cout << "Error: unknown flag '" << argv[i] << "'" << std::endl;
          }
        }
      } else {
        files.push_back(argv[i]);
      }
    }
  }

//Example Usage
/*
int main(int argc, char* argv[]) {
  ArgParse args({
      Arg('h', "help", ArgType::flag),
      Arg('v', "version", ArgType::flag),
      Arg('O', "optimize", ArgType::value),
      Arg('o', "output", ArgType::val_always_sep),
  });

  args.parse_args(argc, argv);

  for (auto pair : args.adds) {
    std::cout << pair.first << " " << pair.second << std::endl;
  }
  std::cout << "FILES:" << std::endl;
  for (int i = 0; i < args.files.size(); i++) {
    std::cout << args.files[i] << std::endl;
  }
}
*/