#include "serialization.h"
#include "cJSON.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "GatheredCallgraph.h"

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

void functions_visitor(const char * name, const char * usr, const char * location, unsigned int number_of_lines, void* array)
{
    cJSON* a = (cJSON*) array;
    cJSON* f = cJSON_CreateObject();
    cJSON_AddStringToObject(f,"name",name);
    cJSON_AddStringToObject(f,"usr",usr);
    cJSON_AddStringToObject(f,"location",location);
    cJSON_AddNumberToObject(f,"numberOfLines",number_of_lines);
    cJSON_AddItemToArray(a,f);
}

cJSON* createFunctionsArray(GatheredCallgraph gathered_callgraph)
{
    cJSON* a = cJSON_CreateArray();
    GatheredCallgraph_visitFunctions(gathered_callgraph,functions_visitor,a);
    return a;
}

cJSON* createCommunityObject(GatheredCallgraph gathered_callgraph)
{
    return cJSON_CreateObject();
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