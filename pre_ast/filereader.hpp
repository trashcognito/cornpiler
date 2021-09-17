#pragma once
#include <string>

class file_object {
 public:
  std::string contents;
  int length;

  file_object(std::string c, int l) : contents(c), length(l){};
};