#include "serialization.h"
#include "cJSON.h"
#include <stdio.h>
#include <string.h>

cJSON* createCallsArray(GatheredCallgraph gathered_callgraph){
    return cJSON_CreateArray();
}

cJSON* createFunctionsArray(GatheredCallgraph gathered_callgraph){
    return cJSON_CreateArray();
}

cJSON* createCommunityObject(GatheredCallgraph gathered_callgraph){
    return cJSON_CreateObject();
}

void save(GatheredCallgraph gathered_callgraph, const char * filename){
    cJSON* o = cJSON_CreateObject();
    cJSON_AddItemToObject(o, "calls", createCallsArray(gathered_callgraph));
    cJSON_AddItemToObject(o, "functions", createFunctionsArray(gathered_callgraph));
    cJSON_AddItemToObject(o, "community", createCommunityObject(gathered_callgraph));
    char * json = cJSON_Print(o);
    FILE* f = fopen(filename,"w");
    fwrite(json,sizeof(char),strlen(json),f);
    fclose(f);
}