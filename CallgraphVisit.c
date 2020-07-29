#include "CallgraphVisit.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "DefinitionData.h"

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

    CXString display = clang_getCursorDisplayName(declared);

    CXString usr = clang_getCursorUSR(declared);
    GatheredCallgraph_addDeclaration(visit->Callgraph, clang_getCString(usr), clang_getCString(display));
    clang_disposeString(usr);

    clang_disposeString(display);
}

char* create_location_string(CXCursor c)
{
    CXSourceLocation loc = clang_getCursorLocation(c);
    CXString filename;
    unsigned int line;
    clang_getPresumedLocation(loc,&filename,&line,NULL);
    const char* c_filename = clang_getCString(filename);
    
    char* ret=malloc(sizeof(char)*(strlen(c_filename) + 50));
    sprintf(ret,"file: %s line: %d",c_filename,line);

    clang_disposeString(filename);
    return ret;
}

unsigned int lines_of(CXCursor c)
{
    CXSourceRange r = clang_getCursorExtent(c);
    CXSourceLocation start = clang_getRangeStart(r);
    unsigned int line_start;
    clang_getPresumedLocation(start,NULL,&line_start,NULL);

    CXSourceLocation end = clang_getRangeEnd(r);
    unsigned int line_end;
    clang_getPresumedLocation(end,NULL,&line_end,NULL);

    return line_end - line_start + 1;
}


void CallgraphVisit_setCurrentFunctionDefinition(CallgraphVisit v, CXCursor defined)
{
    CallgraphVisitImpl* visit = (CallgraphVisitImpl*) v;
    CXString usr = clang_getCursorUSR(defined);

    char* location = create_location_string(defined);

    GatheredCallgraph_addDefinition(
        visit->Callgraph, 
        clang_getCString(usr), 
        create_DefinitionData_with_params(location, lines_of(defined))
    );

    clang_disposeString(usr);
    visit->CurrentCaller=defined;
    
    free(location);
}