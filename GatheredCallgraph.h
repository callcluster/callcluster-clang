#ifndef GATHERED_CALLGRAPH_H
#define GATHERED_CALLGRAPH_H
typedef void* GatheredCallgraph;

GatheredCallgraph createGatheredCallgraph();

void disposeGatheredCallgraph(GatheredCallgraph cg);

void GatheredCallgraph_addCall(GatheredCallgraph, const char * from, const char * to);

void GatheredCallgraph_addDefinition(GatheredCallgraph, const char * def_usr, const char * def_display_name);

void GatheredCallgraph_addDeclaration(GatheredCallgraph, const char * declared);

typedef void (*CallsVisitor)(unsigned int from, unsigned int to, void* data);

void GatheredCallgraph_visitCalls(GatheredCallgraph, CallsVisitor, void* data);

typedef  void (*FunctionsVisitor)(const char * name, const char * usr, void* data);

void GatheredCallgraph_visitFunctions(GatheredCallgraph, FunctionsVisitor, void* data);

#endif /* GATHERED_CALLGRAPH_H */