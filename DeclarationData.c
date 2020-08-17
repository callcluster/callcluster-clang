#include "DeclarationData.h"
#include <stdlib.h>

DeclarationData* create_DeclarationData()
{
    DeclarationData* ret = malloc(sizeof(DeclarationData));
    ret->DisplayName=NULL;
    ret->Filename=NULL;
    ret->Location=NULL;
    return ret;
}

void dispose_DeclarationData(DeclarationData* d)
{
    if(d->DisplayName!=NULL){
        free(d->DisplayName);
    }
    if(d->Filename!=NULL){
        free(d->Filename);
    }
    if(d->Location!=NULL){
        free(d->Location);
    }
    free(d);
}