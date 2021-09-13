cmake_minimum_required(VERSION 3.0.0)
project(cornpiler VERSION 0.1.0)
include(CTest)
enable_testing()
set(CMAKE_CXX_STANDARD 17)
find_package(LLVM 12 REQUIRED CONFIG)
find_program(LLVM_CONFIG NAMES llvm-config-12)
message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

include_directories(${LLVM_INCLUDE_DIRS})
separate_arguments(LLVM_DEFINITIONS_LIST NATIVE_COMMAND ${LLVM_DEFINITIONS})
add_definitions(${LLVM_DEFINITIONS_LIST})

add_executable(cornpiler main.cpp ast.cpp compiler_frontend.cpp compile.cpp)
execute_process(COMMAND ${LLVM_CONFIG} --system-libs --link-shared --libs all
                OUTPUT_VARIABLE llvm_libs
                OUTPUT_STRIP_TRAILING_WHITESPACE)

# Link against LLVM libraries

target_link_libraries(cornpiler ${llvm_libs} ${targets})

target_compile_options(cornpiler PUBLIC -fstandalone-debug)

add_subdirectory(tests)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
