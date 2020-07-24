#ifndef CALLGRAPH_VISIT_H
#define CALLGRAPH_VISIT_H
#include "GatheredCallgraph.h"
#include <clang-c/Index.h>

typedef void* CallgraphVisit;

CallgraphVisit CallgraphVisit_create(GatheredCallgraph modified);

void CallgraphVisit_dispose(CallgraphVisit);

void CallgraphVisit_addCall(CallgraphVisit, CXCursor called);

void CallgraphVisit_addFunctionDeclaration(CallgraphVisit, CXCursor declared);

void CallgraphVisit_setCurrentFunctionDefinition(CallgraphVisit, CXCursor defined);

#endif /* CALLGRAPH_VISIT_H */