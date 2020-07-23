#ifndef GATHERED_CALLGRAPH_H
#define GATHERED_CALLGRAPH_H
typedef void* GatheredCallgraph;

GatheredCallgraph createGatheredCallgraph();

void disposeGatheredCallgraph(GatheredCallgraph cg);

#endif /* GATHERED_CALLGRAPH_H */