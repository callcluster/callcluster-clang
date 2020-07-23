#include "CallgraphVisit.h"
#include <stdlib.h>

typedef struct {
    GatheredCallgraph Callgraph;
} CallgraphVisitImpl;

CallgraphVisit createCallgraphVisit(GatheredCallgraph modified)
{
    CallgraphVisitImpl* visit_impl = malloc(sizeof(CallgraphVisitImpl));
    visit_impl->Callgraph=modified;
    return visit_impl;
}

void disposeCallgraphVisit(CallgraphVisit v)
{
    free((CallgraphVisitImpl*) v);
}