cmake_minimum_required(VERSION 3.0.0)
project(cornpiler VERSION 0.1.0)

include(CTest)
enable_testing()
set(CMAKE_CXX_STANDARD 17)
find_package(LLVM 12 REQUIRED CONFIG PATHS ${CMAKE_SOURCE_DIR}/llvm/build NO_DEFAULT_PATH)

message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

include_directories(${LLVM_INCLUDE_DIRS})
separate_arguments(LLVM_DEFINITIONS_LIST NATIVE_COMMAND ${LLVM_DEFINITIONS})
add_definitions(${LLVM_DEFINITIONS_LIST})

add_executable(cornpiler main.cpp compiler_frontend.cpp compile.cpp parse_cli.cpp translate.cpp logger.cpp)

# Static start
set_target_properties(cornpiler PROPERTIES LINK_SEARCH_START_STATIC 1)
set_target_properties(cornpiler PROPERTIES LINK_SEARCH_END_STATIC 1)
# Set Linker flags
set(CMAKE_EXE_LINKER_FLAGS "-static-libgcc -static-libstdc++")

add_subdirectory(ast)

add_subdirectory(pre_ast)

# Link against LLVM libraries

target_link_libraries(cornpiler ${CMAKE_SOURCE_DIR}/llvm/build/lib/libLLVM-12.so)

target_compile_options(cornpiler PUBLIC )

add_subdirectory(tests)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
