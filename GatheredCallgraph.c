#include "GatheredCallgraph.h"
#include <stdlib.h>

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


