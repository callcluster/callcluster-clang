cmake_minimum_required(VERSION 3.18)

project(callclusterClang)

add_executable(callclusterClang main.c GatheredCallgraph.c CallgraphVisit.c Visitor.c serialization.c cJSON.c DefinitionData.c definition_analysis.c StructuralAnalysis.c complexity_analysis.c parameters.c ComplexityVisitor.c)

include_directories(/usr/lib/llvm-10/include)

target_link_libraries(callclusterClang /usr/lib/llvm-10/lib/libclang-10.0.0.so)

include_directories(${PROJECT_SOURCE_DIR})