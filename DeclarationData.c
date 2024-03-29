#include "DeclarationData.h"
#include <stdlib.h>

DeclarationData* create_DeclarationData()
{
    DeclarationData* ret = malloc(sizeof(DeclarationData));
    ret->Name=NULL;
    ret->Filename=NULL;
    ret->Location=NULL;
    return ret;
}

void dispose_DeclarationData(DeclarationData* d)
{
    if(d->Name!=NULL){
        free(d->Name);
    }
    if(d->Filename!=NULL){
        free(d->Filename);
    }
    if(d->Location!=NULL){
        free(d->Location);
    }
    free(d);
}