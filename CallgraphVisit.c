#include "CallgraphVisit.h"
#include <stdlib.h>
#include <string.h>

typedef struct {
    GatheredCallgraph Callgraph;
    CXCursor CurrentCaller;
} CallgraphVisitImpl;


char* copied_usr(CXCursor c){
    CXString usr = clang_getCursorUSR(c);
    const char* cstr = clang_getCString(usr);
    char* copied = malloc( sizeof(char) * ( strlen(cstr) + 1 ) );
    strcpy(copied,cstr);
    clang_disposeString(usr);
    return copied;
}

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
    CallgraphVisitImpl* visit = (CallgraphVisitImpl*) v;
    GatheredCallgraph_addCall(visit->Callgraph, copied_usr(visit->CurrentCaller), copied_usr(called));
}

void CallgraphVisit_addFunctionDeclaration(CallgraphVisit v, CXCursor declared)
{
    CallgraphVisitImpl* visit = (CallgraphVisitImpl*) v;
    GatheredCallgraph_addDeclaration(visit->Callgraph, copied_usr(declared));
}

void CallgraphVisit_setCurrentFunctionDefinition(CallgraphVisit v, CXCursor defined)
{
    CallgraphVisitImpl* visit = (CallgraphVisitImpl*) v;
    GatheredCallgraph_addDefinition(visit, copied_usr(defined));
    visit->CurrentCaller=defined;
}