#include "DefinitionData.h"
#include <stdlib.h>
#include <string.h>

DefinitionData* create_DefinitionData()
{
    DefinitionData* ret = malloc(sizeof(DefinitionData));
    ret->Location=NULL;
    ret->NumberOfLines=0;
    return ret;
}

void dispose_DefinitionData(DefinitionData* d)
{
    if(d->Location!=NULL)
    {
        free(d->Location);
    }
    free(d);
}

char* dd_mallocopy(const char* copied)
{
    char* ret = malloc( sizeof(char) * (strlen(copied) + 1) );
    strcpy(ret,copied);
    return ret;
}

DefinitionData* create_DefinitionData_with_params(const char* location, unsigned int number_of_lines)
{
    DefinitionData* ret = create_DefinitionData();
    ret->Location = dd_mallocopy(location);
    ret->NumberOfLines = number_of_lines;
    return ret;
}