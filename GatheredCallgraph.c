#include "GatheredCallgraph.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

struct FunctionList {
    char* FunctionUsr;
    char* Location;
    char* DisplayName;
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


void find_function(GatheredCallgraphImpl* cg, const char * name, int* id_out, FunctionList** function_out)
{
    FunctionList* current = cg -> LastFunction;
    int i = cg->FunctonsSize - 1;
    while(current != NULL){
        if(strcmp(current->FunctionUsr,name)==0){
            if(id_out!=NULL){
                *id_out = i;
            }
            if(function_out!=NULL){
                *function_out = current;
            }
            return;
        }
        current = current -> Previous; i--;
    }
    if(id_out!=NULL){
        *id_out = -1;
    }
    if(function_out!=NULL){
        *function_out = NULL;
    }
    return;
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
            free(current->FunctionUsr);

            if(current->Location!=NULL){
                free(current->Location);
            }

            if(current->DisplayName!=NULL){
                free(current->DisplayName);
            }
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
    
    int from_id = -1;
    int to_id = -1;

    find_function(cg,from,&from_id,NULL);
    find_function(cg,to,&to_id,NULL);

    CallsSet* new_set = malloc(sizeof(CallsSet));
    new_set->from = from_id;
    new_set->to = to_id;
    new_set->Next = cg->Calls;

    printf("%d -> %d\n", from_id, to_id);

    cg->Calls = new_set;
}

char* mallocopy(const char* copied)
{
    char* ret = malloc( sizeof(char) * (strlen(copied) + 1) );
    strcpy(ret,copied);
    return ret;
}

FunctionList* createFunctionList(const char* name, FunctionList* previous)
{
    FunctionList* ret = malloc(sizeof(FunctionList));
    ret->FunctionUsr = mallocopy(name);
    ret->Previous = previous;
    ret->DisplayName = NULL;
    ret->Location = NULL;
    return ret;
}

FunctionList* addFunction(GatheredCallgraphImpl* cg, const char * name)
{
    if(cg->LastFunction == NULL){
        cg->LastFunction = createFunctionList(name,NULL);
        cg->FunctonsSize = 1;
    }else{
        FunctionList* previous = cg->LastFunction;
        cg->LastFunction = createFunctionList(name,previous);
        cg->FunctonsSize += 1;
    }
    return cg->LastFunction;
}

FunctionList* get_or_add_function(GatheredCallgraphImpl* cg, const char * usr)
{
    FunctionList* ret;
    find_function(cg,usr,NULL,&ret);
    if(ret==NULL){
        ret = addFunction(cg,usr);
    }
    return ret;
}

void GatheredCallgraph_addDefinition(GatheredCallgraph callgraph, const char * def_usr, const char * location)
{
    GatheredCallgraphImpl* cg = (GatheredCallgraphImpl*) callgraph;
    FunctionList* fun = get_or_add_function(cg,def_usr);
    if(fun->Location==NULL){
        fun->Location=mallocopy(location);
    }
}

void GatheredCallgraph_addDeclaration(GatheredCallgraph callgraph, const char * declared_usr, const char * def_display_name)
{
    GatheredCallgraphImpl* cg = (GatheredCallgraphImpl*) callgraph;
    FunctionList* fun = get_or_add_function(cg,declared_usr);
    if(fun->DisplayName==NULL){
        fun->DisplayName = mallocopy(def_display_name);
    }
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
        visitor(functions[i]->DisplayName,functions[i]->FunctionUsr,functions[i]->Location,data);
    }
    free(functions);
}
