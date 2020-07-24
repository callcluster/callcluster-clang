#ifndef GATHERED_CALLGRAPH_H
#define GATHERED_CALLGRAPH_H
typedef void* GatheredCallgraph;

GatheredCallgraph createGatheredCallgraph();

void disposeGatheredCallgraph(GatheredCallgraph cg);

void GatheredCallgraph_addCall(GatheredCallgraph, char * from,  char * to);

void GatheredCallgraph_addDefinition(GatheredCallgraph,  char * def_usr);

void GatheredCallgraph_addDeclaration(GatheredCallgraph,  char * declared);

#endif /* GATHERED_CALLGRAPH_H */