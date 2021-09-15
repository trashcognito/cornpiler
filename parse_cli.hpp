#pragma once

#include <string>
#include <vector>
#include <map>

enum class ArgType {
  flag,
  value,
  val_always_sep
};

class Arg {
 public:
  char short_name;
  std::string long_name;
  ArgType type;
  Arg(char sn, std::string ln, ArgType t);
};

class ArgParse {
 public:
  std::vector<Arg> args;
  std::vector<std::string> files;
  std::map<std::string, std::string> adds;
  ArgParse(std::vector<Arg> a);
  void parse_args(int argc, char* argv[]);
};