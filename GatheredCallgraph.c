#include "GatheredCallgraph.h"
#include <stdlib.h>
#include <stdio.h>

typedef struct {
    char* FunctionName;
    struct FunctionList* next;
} FunctionList;

typedef struct {
    unsigned int from;
    unsigned int to;
    struct CallsSet* next;
} CallsSet;

typedef struct {
    FunctionList* Functions;
    CallsSet* Calls;
} GatheredCallgraphImpl;


GatheredCallgraph createGatheredCallgraph()
{
    return malloc(sizeof(GatheredCallgraphImpl));
}

void disposeGatheredCallgraph(GatheredCallgraph cg)
{
    return free((GatheredCallgraphImpl*) cg);
}


void GatheredCallgraph_addCall(GatheredCallgraph callgraph, char * from,  char * to)
{
    printf("%s -> %s \n",from,to);
    free(from);
    free(to);
}

void GatheredCallgraph_addDefinition(GatheredCallgraph callgraph,  char * def_usr)
{
    printf("def: %s\n",def_usr);
    free(def_usr);
}

void GatheredCallgraph_addDeclaration(GatheredCallgraph callgraph,  char * declared)
{
    printf("decl: %s\n",declared);
    free(declared);
}