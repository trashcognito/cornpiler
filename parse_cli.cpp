#include <iostream>
#include <string>
#include <vector>

enum class arg_type {
  flag,
  value,
  val_always_sep
};

class arg {
 public:
  char short_name;
  std::string long_name;
  arg_type type;
  arg(char sn, std::string ln, arg_type t) {
    short_name = sn;
    long_name = ln;
    type = t;
  }
};

class arg_add {
 public:
  std::string name;
  std::string value;
  arg_add(std::string n, std::string v) {
    name = n;
    value = v;
  }
};

class argparse {
 public:
  std::vector<arg> args;
  std::vector<std::string> files;
  std::vector<arg_add> adds;
  argparse(std::vector<arg> a) {
    args = a;
  }
  void parse_args(int argc, char* argv[]) {
    for (int i = 1; i < argc; i++) {
      if (argv[i][0] == '-') {
        if (argv[i][1] == '-') {
          bool found = false;
          for (int j = 0; j < args.size(); j++) {
            if (args[j].type == arg_type::flag) {
              if (std::string(argv[i]) == ("--" + args[j].long_name)) {
                adds.push_back(arg_add(args[j].long_name, ""));
                found = true;
                break;
              }
            } else if (args[j].type == arg_type::value | args[j].type == arg_type::val_always_sep) {
              if (std::string(argv[i]) == ("--" + args[j].long_name)) {
                if(++i >= argc) {
                  std::cout << "Error: flag '" << args[j].long_name << "' requires a value" << std::endl;
                  exit(-1);
                }
                adds.push_back(arg_add(args[j].long_name, argv[i]));
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
              if (args[j].type == arg_type::flag) {
                adds.push_back(arg_add(args[j].long_name, ""));
                found = true;
                break;
              }else if (args[j].type == arg_type::value) {
                if(argv[i][2] == '\0'){
                  std::cout << "Error: flag '" << args[j].long_name << "' requires a value" << std::endl;
                  exit(-1);
                }
                adds.push_back(arg_add(args[j].long_name, std::string(argv[i]).substr(2)));
                found = true;
                break;
              }else if(args[j].type == arg_type::val_always_sep){
                if(++i >= argc) {
                  std::cout << "Error: flag '" << args[j].long_name << "' requires a value" << std::endl;
                  exit(-1);
                }
                adds.push_back(arg_add(args[j].long_name, argv[i]));
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
};

int main(int argc, char* argv[]) {
  argparse args({
      arg('h', "help", arg_type::flag),
      arg('v', "version", arg_type::flag),
      arg('O', "optimize", arg_type::value),
      arg('o', "output", arg_type::val_always_sep),
  });

  args.parse_args(argc, argv);

  for (int i = 0; i < args.adds.size(); i++) {
    std::cout << args.adds[i].name << " " << args.adds[i].value << std::endl;
  }
  std::cout << "FILES:" << std::endl;
  for (int i = 0; i < args.files.size(); i++) {
    std::cout << args.files[i] << std::endl;
  }
}