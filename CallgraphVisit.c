#include "CallgraphVisit.h"
#include <stdlib.h>
#include "DefinitionData.h"
#include "definition_analysis.h"
#include "parameters.h"

typedef struct {
    GatheredCallgraph Callgraph;
    CXCursor CurrentCaller;
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
    CallgraphVisitImpl* visit = (CallgraphVisitImpl*) v;

    CXString current_usr = clang_getCursorUSR(visit->CurrentCaller);
    CXString called_usr = clang_getCursorUSR(called);

    GatheredCallgraph_addCall(
        visit->Callgraph, 
        clang_getCString(current_usr), 
        clang_getCString(called_usr)
    );

    clang_disposeString(current_usr);
    clang_disposeString(called_usr);
}

void CallgraphVisit_addFunctionDeclaration(CallgraphVisit v, CXCursor declared)
{
    CallgraphVisitImpl* visit = (CallgraphVisitImpl*) v;

    CXString usr = clang_getCursorUSR(declared);

    if(!GatheredCallgraph_wasDeclared(visit->Callgraph, clang_getCString(usr))){
        GatheredCallgraph_addDeclaration(visit->Callgraph, clang_getCString(usr), analyze_declaration(declared));
    }

    
    clang_disposeString(usr);
}

void CallgraphVisit_setCurrentFunctionDefinition(CallgraphVisit v, CXCursor defined)
{
    CallgraphVisitImpl* visit = (CallgraphVisitImpl*) v;
    CXString usr = clang_getCursorUSR(defined);
    print_function_usr(clang_getCString(usr));
    set_current_analyzed_function(clang_getCString(usr));
    GatheredCallgraph_addDefinition(
        visit->Callgraph, 
        clang_getCString(usr), 
        analyze(defined)
    );
    set_current_analyzed_function(NULL);
    clang_disposeString(usr);
    visit->CurrentCaller = defined;
}