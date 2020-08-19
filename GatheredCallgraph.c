#define _GNU_SOURCE
#include "GatheredCallgraph.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <search.h>

struct FunctionBucket {
    struct hsearch_data * htab;
    struct FunctionBucket* previous;
};

typedef struct FunctionBucket FunctionBucket;
typedef struct FullFunctionData FullFunctionData;

struct FunctionList {
    char* FunctionUsr;
    DefinitionData* data;
    DeclarationData* declaration_data;
    struct FunctionList* Previous;
    unsigned int id;
};

typedef struct FunctionList FunctionList;

struct CallsSet {
    unsigned int from;
    unsigned int to;
    struct CallsSet* Next;
};

typedef struct CallsSet CallsSet;

typedef struct  {
    FunctionBucket* LastBucket;
    FunctionList* LastFunction;
    unsigned int FunctonsSize;
    CallsSet* Calls;
} GatheredCallgraphImpl;


void find_function(GatheredCallgraphImpl* cg, const char * name, int* id_out, FunctionList** function_out)
{
    FunctionBucket* current = cg->LastBucket;
    ENTRY entry;
    entry.key=name;
    ENTRY* ret_entry=NULL;
    while(ret_entry == NULL && current!=NULL){
        hsearch_r(entry, FIND, &ret_entry, current->htab);
        current = current->previous;
    }

    if(ret_entry==NULL){
        if(id_out!=NULL){
            *id_out = -1;
        }
        if(function_out!=NULL){
            *function_out = NULL;
        }
    }else{
        if(id_out!=NULL){
            *id_out = ((FunctionList*)ret_entry->data)->id;
        }
        if(function_out!=NULL){
            *function_out = (FunctionList*)ret_entry->data;
        }
    }
}

GatheredCallgraph createGatheredCallgraph()
{
    GatheredCallgraphImpl* cg = malloc(sizeof(GatheredCallgraphImpl));
    cg->Calls = NULL;
    cg->LastFunction = NULL;
    cg->FunctonsSize = 0;
    cg->LastBucket = NULL;
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
            if(current->data!=NULL){
                dispose_DefinitionData(current->data);
            }
            if(current->declaration_data!=NULL){
                dispose_DeclarationData(current->declaration_data);
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
    {
        FunctionBucket* current = cg->LastBucket;
        while(current!=NULL){
            FunctionBucket* previous = current->previous;
            hdestroy_r(current->htab);
            free(current->htab);
            free(current);
            current = previous;
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
    ret->declaration_data = NULL;
    ret->data = NULL;
    return ret;
}

FunctionBucket* createEmptyBucket(FunctionBucket* previous)
{
    FunctionBucket* ret = malloc(sizeof(FunctionBucket));
    ret->previous = previous;
    ret->htab=calloc(1,sizeof(struct hsearch_data));
    hcreate_r(1000,ret->htab);
    return ret;
}

unsigned int addFunctionToBucket(FunctionBucket* bucket, FunctionList* added)
{
    ENTRY item;
    ENTRY* retitem;
    item.data=added;
    item.key=added->FunctionUsr;
    FunctionBucket* current_bucket=bucket;
    return hsearch_r(item,ENTER,&retitem,current_bucket->htab);
}

FunctionList* addFunction(GatheredCallgraphImpl* cg, const char * name)
{
    if(cg->LastFunction == NULL){
        cg->LastFunction = createFunctionList(name,NULL);
        cg->LastFunction->id = 0;
        cg->FunctonsSize = 1;
    }else{
        FunctionList* previous = cg->LastFunction;
        cg->LastFunction = createFunctionList(name,previous);
        cg->LastFunction->id = cg->FunctonsSize;
        cg->FunctonsSize += 1;
    }

    if(cg->LastBucket==NULL){
        cg->LastBucket=createEmptyBucket(NULL);
    }
    unsigned int success = addFunctionToBucket(cg->LastBucket,cg->LastFunction);
    if(!success){
        cg->LastBucket = createEmptyBucket(cg->LastBucket);
        addFunctionToBucket(cg->LastBucket, cg->LastFunction);
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

void GatheredCallgraph_addDefinition(GatheredCallgraph callgraph, const char * def_usr, DefinitionData* data)
{
    GatheredCallgraphImpl* cg = (GatheredCallgraphImpl*) callgraph;
    FunctionList* fun = get_or_add_function(cg,def_usr);

    if(fun->data==NULL){
        fun->data=data;
    }else{
        dispose_DefinitionData(data);
    }
}

unsigned int GatheredCallgraph_wasDeclared(GatheredCallgraph callgraph, const char * declared_usr)
{
    GatheredCallgraphImpl* cg = (GatheredCallgraphImpl*) callgraph;
    FunctionList* fun = get_or_add_function(cg,declared_usr);
    return fun->declaration_data!=NULL;
}

void GatheredCallgraph_addDeclaration(GatheredCallgraph callgraph, const char * declared_usr, DeclarationData* data)
{
    GatheredCallgraphImpl* cg = (GatheredCallgraphImpl*) callgraph;
    FunctionList* fun = get_or_add_function(cg,declared_usr);
    if(fun->declaration_data==NULL){
        fun->declaration_data = data;
    }else{
        dispose_DeclarationData(data);
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
        visitor(
            functions[i]->declaration_data,
            functions[i]->FunctionUsr,
            i,
            functions[i]->data,
            data
        );
    }
    free(functions);
}
