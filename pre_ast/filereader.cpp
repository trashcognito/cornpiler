#include "filereader.hpp"

#include <fstream>

file_object read_file(const char *filename, logger::logger *logger) {
  std::ifstream is(filename);
  if (!is) {
    logger->log(logger::LOG_LEVEL::ERROR, "File not found");
    exit(-1);
  }

  // is.seekg(0, is.end);
  // int is_length = is.tellg();
  // is.seekg(0, is.beg);

  // char *file_buffer = new char[is_length];
  // int f_iter = 0;
  // while (!is.eof() && f_iter < is_length) {
  //   is.get(file_buffer[f_iter]); // reading single character from file to array
  //   f_iter++;
  // }
  // is.close();

  std::string content((std::istreambuf_iterator<char>(is)),
                      (std::istreambuf_iterator<char>()));

  return file_object(content, content.length());
}