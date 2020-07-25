#include "GatheredCallgraph.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

struct FunctionList {
    char* FunctionName;
    struct FunctionList* Previous;
};

typedef struct FunctionList FunctionList;

struct CallsSet {
    unsigned int from;
    unsigned int to;
    struct CallsSet* next;
};

typedef struct CallsSet CallsSet;

typedef struct  {
    FunctionList* Last;
    unsigned int FunctonsSize;
    CallsSet* Calls;
} GatheredCallgraphImpl;


int functionId(GatheredCallgraphImpl* cg, const char * name)
{
    FunctionList* current = cg -> Last;
    int i = cg->FunctonsSize - 1;
    while(current != NULL){
        if(strcmp(current->FunctionName,name)==0){
            return i;
        }
        current = current -> Previous; i--;
    }
    return i;
}

GatheredCallgraph createGatheredCallgraph()
{
    GatheredCallgraphImpl* cg = malloc(sizeof(GatheredCallgraphImpl));
    cg->Calls = NULL;
    cg->Last = NULL;
    cg->FunctonsSize = 0;
    return cg;
}

void disposeGatheredCallgraph(GatheredCallgraph cg)
{
    //free de todos los char*
    free((GatheredCallgraphImpl*) cg);
}


void GatheredCallgraph_addCall(GatheredCallgraph callgraph, const char * from, const char * to)
{
    GatheredCallgraphImpl* cg = (GatheredCallgraphImpl*) callgraph;
    
    int from_id = functionId(cg, from);
    int to_id = functionId(cg, to);

    CallsSet* new_set = malloc(sizeof(CallsSet));
    new_set->from = from_id;
    new_set->to = to_id;
    new_set->next = cg->Calls;

    printf("%d -> %d\n", from_id, to_id);

    cg->Calls = new_set;
}

FunctionList* createFunctionList(char* name, FunctionList* previous){
    FunctionList* ret = malloc(sizeof(FunctionList));
    ret->FunctionName = name;
    ret->Previous = previous;
    return ret;
}

void addFunction(GatheredCallgraphImpl* cg, char * name)
{
    if(cg->Last == NULL){
        cg->Last = createFunctionList(name,NULL);
        cg->FunctonsSize = 1;
    }else{
        
        FunctionList* previous = cg->Last;
        cg->Last = createFunctionList(name,previous);
        cg->FunctonsSize += 1;
    }
}

void GatheredCallgraph_addDefinition(GatheredCallgraph callgraph,  char * def_usr)
{
    GatheredCallgraphImpl* cg = (GatheredCallgraphImpl*) callgraph;
    int id = functionId(cg,def_usr);
    if(id < 0){
        addFunction(cg,def_usr);
    }
}

void GatheredCallgraph_addDeclaration(GatheredCallgraph callgraph,  char * declared)
{
    GatheredCallgraph_addDefinition(callgraph, declared);
}