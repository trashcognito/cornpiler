#include "compile.hpp"
#include <fstream>
#include <iostream>
#include <vector>

int main() {
    std::ifstream is ("tests/bottlesofbeer99.crn");

    if(!is){
        std::cout << "File not found" << std::endl;
        return -1;
    }
    is.seekg (0, is.end);
    int is_length = is.tellg();
    is.seekg (0, is.beg);

    char *file_buffer = new char [is_length];

    std::vector<token> tokens;

    // traverse through the file char by char
    for(int i = 0; i < is_length; i++){
        
    }

    return 0;
}