#include "GatheredCallgraph.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

struct FunctionList {
    char* FunctionName;
    struct FunctionList* Previous;
};

typedef struct FunctionList FunctionList;

typedef struct {
    unsigned int from;
    unsigned int to;
    struct CallsSet* next;
} CallsSet;

typedef struct {
    FunctionList* Last;
    unsigned int FunctonsSize;
    CallsSet* Calls;
} GatheredCallgraphImpl;


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


void GatheredCallgraph_addCall(GatheredCallgraph callgraph, char * from,  char * to)
{
    printf("%s -> %s \n",from,to);
    free(from);
    free(to);
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

int functionId(GatheredCallgraphImpl* cg, char * name)
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

void GatheredCallgraph_addDefinition(GatheredCallgraph callgraph,  char * def_usr)
{
    GatheredCallgraphImpl* cg = (GatheredCallgraphImpl*) callgraph;
    int id = functionId(cg,def_usr);

    printf("def %d: %s\n", id, def_usr);

    if(id < 0){
        addFunction(cg,def_usr);
    }
}

void GatheredCallgraph_addDeclaration(GatheredCallgraph callgraph,  char * declared)
{
    printf("decl: %s\n",declared);
    GatheredCallgraph_addDefinition(callgraph, declared);

}