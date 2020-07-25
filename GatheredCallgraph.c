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
    struct CallsSet* Next;
};

typedef struct CallsSet CallsSet;

typedef struct  {
    FunctionList* LastFunction;
    unsigned int FunctonsSize;
    CallsSet* Calls;
} GatheredCallgraphImpl;


int functionId(GatheredCallgraphImpl* cg, const char * name)
{
    FunctionList* current = cg -> LastFunction;
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
    cg->LastFunction = NULL;
    cg->FunctonsSize = 0;
    return cg;
}

void disposeGatheredCallgraph(GatheredCallgraph callgraph)
{
    GatheredCallgraphImpl* cg = (GatheredCallgraphImpl*) callgraph;

    {
        FunctionList* current = cg->LastFunction;
        while(current != NULL){
            FunctionList* prev = current->Previous;
            free(current->FunctionName);
            free(current);
            current = prev;
        }
    }
    {
        CallsSet* current = cg->Calls;
        while(current!=NULL){
            CallsSet* Next = current->Next;
            free(current);
            current = Next;
        }
    }

    free(cg);
}


void GatheredCallgraph_addCall(GatheredCallgraph callgraph, const char * from, const char * to)
{
    GatheredCallgraphImpl* cg = (GatheredCallgraphImpl*) callgraph;
    
    int from_id = functionId(cg, from);
    int to_id = functionId(cg, to);

    CallsSet* new_set = malloc(sizeof(CallsSet));
    new_set->from = from_id;
    new_set->to = to_id;
    new_set->Next = cg->Calls;

    printf("%d -> %d\n", from_id, to_id);

    cg->Calls = new_set;
}

FunctionList* createFunctionList(const char* name, FunctionList* previous){
    FunctionList* ret = malloc(sizeof(FunctionList));
    ret->FunctionName = malloc( sizeof(name) * (strlen(name) + 1) );
    strcpy(ret->FunctionName,name);
    ret->Previous = previous;
    return ret;
}

void addFunction(GatheredCallgraphImpl* cg, const char * name)
{
    if(cg->LastFunction == NULL){
        cg->LastFunction = createFunctionList(name,NULL);
        cg->FunctonsSize = 1;
    }else{
        
        FunctionList* previous = cg->LastFunction;
        cg->LastFunction = createFunctionList(name,previous);
        cg->FunctonsSize += 1;
    }
}

void GatheredCallgraph_addDefinition(GatheredCallgraph callgraph, const char * def_usr)
{
    GatheredCallgraphImpl* cg = (GatheredCallgraphImpl*) callgraph;
    int id = functionId(cg,def_usr);
    if(id < 0){
        addFunction(cg,def_usr);
    }
}

void GatheredCallgraph_addDeclaration(GatheredCallgraph callgraph, const  char * declared)
{
    GatheredCallgraph_addDefinition(callgraph, declared);
}

void GatheredCallgraph_visitCalls(GatheredCallgraph gathered_callgraph, CallsVisitor visitor, void* data)
{
    GatheredCallgraphImpl* cg = (GatheredCallgraphImpl*) gathered_callgraph;
    CallsSet* current = cg->Calls;
    while(current!=NULL){
        visitor(current->from, current->to, data);
        current = current->Next;
    }
}
void GatheredCallgraph_visitFunctions(GatheredCallgraph gathered_callgraph, FunctionsVisitor visitor, void* data)
{
    GatheredCallgraphImpl* cg = (GatheredCallgraphImpl*) gathered_callgraph;
    FunctionList** functions = malloc(sizeof(FunctionList*)*(cg->FunctonsSize));
    {
        FunctionList* current = cg->LastFunction;
        unsigned int current_index = (cg->FunctonsSize) - 1;
        while(current!=NULL){
            functions[current_index] = current;
            current = current->Previous; current_index--;
        }
    }
    for(unsigned int i = 0; i < (cg->FunctonsSize); i++){
        visitor(functions[i]->FunctionName,data);
    }
    free(functions);
}
