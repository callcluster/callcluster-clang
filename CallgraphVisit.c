#include "CallgraphVisit.h"
#include <stdlib.h>

typedef struct {
    GatheredCallgraph Callgraph;
} CallgraphVisitImpl;

CallgraphVisit CallgraphVisit_create(GatheredCallgraph modified)
{
    CallgraphVisitImpl* visit_impl = malloc(sizeof(CallgraphVisitImpl));
    visit_impl->Callgraph=modified;
    return visit_impl;
}

void CallgraphVisit_dispose(CallgraphVisit v)
{
    free((CallgraphVisitImpl*) v);
}

void CallgraphVisit_addCall(CallgraphVisit v, CXCursor called)
{
    CXString usr = clang_getCursorUSR(called);
    printf("called: %s \n",clang_getCString(usr));
    clang_disposeString(usr);
}

void CallgraphVisit_addFunctionDeclaration(CallgraphVisit v, CXCursor declared)
{
    CXString usr = clang_getCursorUSR(declared);
    printf("declared: %s \n",clang_getCString(usr));
    clang_disposeString(usr);
}

void CallgraphVisit_setCurrentFunctionDefinition(CallgraphVisit v, CXCursor defined)
{
    CXString usr = clang_getCursorUSR(defined);
    printf("defined: %s \n",clang_getCString(usr));
    clang_disposeString(usr);
}