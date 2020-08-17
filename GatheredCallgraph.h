#ifndef GATHERED_CALLGRAPH_H
#define GATHERED_CALLGRAPH_H
#include "DefinitionData.h"
#include "DeclarationData.h"

typedef void* GatheredCallgraph;

GatheredCallgraph createGatheredCallgraph();

void disposeGatheredCallgraph(GatheredCallgraph cg);

void GatheredCallgraph_addCall(GatheredCallgraph, const char * from, const char * to);

void GatheredCallgraph_addDefinition(GatheredCallgraph, const char * def_usr, DefinitionData* data);

void GatheredCallgraph_addDeclaration(GatheredCallgraph, const char * declared_usr, DeclarationData* data);

unsigned int GatheredCallgraph_wasDeclared(GatheredCallgraph, const char * declared_usr);

typedef void (*CallsVisitor)(unsigned int from, unsigned int to, void* data);

void GatheredCallgraph_visitCalls(GatheredCallgraph, CallsVisitor, void* data);

typedef  void (*FunctionsVisitor)(DeclarationData * name, const char * usr, unsigned int index, DefinitionData* def_data, void* data);

void GatheredCallgraph_visitFunctions(GatheredCallgraph, FunctionsVisitor, void* data);

#endif /* GATHERED_CALLGRAPH_H */