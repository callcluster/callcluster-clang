cmake_minimum_required(VERSION 3.18)

project(callclusterClang)

add_executable(callclusterClang main.c GatheredCallgraph.c CallgraphVisit.c Visitor.c)

include_directories(/usr/lib/llvm-10/include)

target_link_libraries(callclusterClang /usr/lib/llvm-10/lib/libclang-10.0.0.so)

include_directories(${PROJECT_SOURCE_DIR})





#find_library(LIBCLANG llvm-10/lib/libclang-10.0.0.so)

#target_link_libraries(callclusterClang PUBLIC "${LIBCLANG}")

#find_path(LIBCLANG_H lib/llvm-10/include)

#target_include_directories(callclusterClang PUBLIC "${LIBCLANG_H}")

#LINK_DIRECTORIES(/usr/lib/llvm-10/lib)

#TARGET_LINK_LIBRARIES(callclusterClang libclang-10.0.0)



#add_library(clang SHARED IMPORTED)

#set_target_properties(clang PROPERTIES IMPORTED_LOCATION "/usr/lib/llvm-10/lib/libclang-10.0.0.so")

#set_target_properties(clang PROPERTIES INTERFACE_SOURCES "/usr/lib/llvm-10/include/**/*")
