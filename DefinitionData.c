#include "DefinitionData.h"
#include <stdlib.h>
#include <string.h>

DefinitionData* create_DefinitionData()
{
    DefinitionData* ret = malloc(sizeof(DefinitionData));
    ret->Location=NULL;
    ret->NumberOfLines=0;
    ret->Filename=NULL;
    ret->CyclomaticComplexity=0;
    ret->BasiliComplexity=0;
    return ret;
}

void dispose_DefinitionData(DefinitionData* d)
{
    if(d->Location!=NULL)
    {
        free(d->Location);
    }
    if(d->Filename!=NULL)
    {
        free(d->Filename);
    }
    free(d);
}