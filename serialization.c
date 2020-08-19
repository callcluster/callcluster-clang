#include "serialization.h"
#include "cJSON.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "GatheredCallgraph.h"
#include "StructuralAnalysis.h"

void calls_visitor(unsigned int from, unsigned int to, void* array)
{
    cJSON* a = (cJSON*) array;
    cJSON* o = cJSON_CreateObject();
    cJSON_AddNumberToObject(o,"from",from);
    cJSON_AddNumberToObject(o,"to",to);
    cJSON_AddItemToArray(a,o);
}

cJSON* createCallsArray(GatheredCallgraph gathered_callgraph)
{
    cJSON* a = cJSON_CreateArray();
    GatheredCallgraph_visitCalls(gathered_callgraph,calls_visitor,a);
    return a;
}

void functions_visitor(DeclarationData* decl_data, const char * usr, unsigned int index, DefinitionData* data, void* array)
{
    cJSON* a = (cJSON*) array;
    cJSON* f = cJSON_CreateObject();
    cJSON_AddStringToObject(f,"name",decl_data->DisplayName);
    cJSON_AddStringToObject(f,"usr",usr);
    if(data==NULL){
        cJSON_AddStringToObject(f,"location",decl_data->Location);
        cJSON_AddStringToObject(f,"filename",decl_data->Filename);
    }else{
        cJSON_AddStringToObject(f,"location",data->Location);
        cJSON_AddNumberToObject(f,"numberOfLines",data->NumberOfLines);
        cJSON_AddNumberToObject(f,"numberOfStatements",data->NumberOfStatements);
        cJSON_AddStringToObject(f,"filename",data->Filename);
        if(data->CyclomaticComplexity!=0){
            cJSON_AddNumberToObject(f,"cyclomaticComplexity",data->CyclomaticComplexity);
        }
        cJSON_AddNumberToObject(f,"basiliComplexity",data->BasiliComplexity);
    }
    cJSON_AddItemToArray(a,f);
}

cJSON* createFunctionsArray(GatheredCallgraph gathered_callgraph)
{
    cJSON* a = cJSON_CreateArray();
    GatheredCallgraph_visitFunctions(gathered_callgraph,functions_visitor,a);
    return a;
}

void children_visitor(Community community, char* name, unsigned int functions[], unsigned int functions_size, void* children_array);

void add_community_to_json_object(Community c, cJSON* o, char* name, unsigned int functions[], unsigned int functions_size)
{
    cJSON_AddStringToObject(o,"name",name);
    cJSON_AddItemToObject(o,"functions",cJSON_CreateIntArray(functions,functions_size));
    cJSON* children = cJSON_CreateArray();
    Community_visitChildren(c,children_visitor,children);
    cJSON_AddItemToObject(o,"communities",children);
}

void children_visitor(Community community, char* name, unsigned int functions[], unsigned int functions_size, void* children_array)
{
    cJSON* children = (cJSON*) children_array;
    cJSON* o = cJSON_CreateObject();
    add_community_to_json_object(community,o,name,functions,functions_size);
    cJSON_AddItemToArray(children_array,o);
}

void root_visitor(Community community, char* name, unsigned int functions[], unsigned int functions_size, void* root_object)
{
    cJSON* o = (cJSON*) root_object;
    add_community_to_json_object(community,o,name,functions,functions_size);
}

cJSON* createCommunityObject(GatheredCallgraph gathered_callgraph)
{
    cJSON* o = cJSON_CreateObject();
    StructuralAnalysis a = create_StructuralAnalysis(gathered_callgraph);
    StructuralAnalysis_visitRoot(a,root_visitor,o);
    dispose_StructuralAnalysis(a);
    return o;
}

void save(GatheredCallgraph gathered_callgraph, const char * filename)
{
    cJSON* o = cJSON_CreateObject();
    cJSON_AddItemToObject(o, "functions", createFunctionsArray(gathered_callgraph));
    cJSON_AddItemToObject(o, "calls", createCallsArray(gathered_callgraph));
    cJSON_AddItemToObject(o, "community", createCommunityObject(gathered_callgraph));
    char * json = cJSON_Print(o);
    FILE* f = fopen(filename,"w");
    fwrite(json, sizeof(char), strlen(json), f);
    fclose(f);
    free(json);
    cJSON_Delete(o);
}