cmake_policy(SET CMP0076 NEW)
target_sources(cornpiler 
PUBLIC 
ast.cpp
filereader.cpp
scopes.cpp
)

add_subdirectory(with)
add_subdirectory(types)
add_subdirectory(nodes)