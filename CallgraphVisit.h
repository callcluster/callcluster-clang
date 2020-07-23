#ifndef CALLGRAPH_VISIT_H
#define CALLGRAPH_VISIT_H
#include "GatheredCallgraph.h"

typedef void* CallgraphVisit;

CallgraphVisit createCallgraphVisit(GatheredCallgraph modified);

void disposeCallgraphVisit(CallgraphVisit);

#endif /* CALLGRAPH_VISIT_H */