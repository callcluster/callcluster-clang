#ifndef DECLARATION_DATA_H
#define DECLARATION_DATA_H

struct DeclarationData {
    char* Name;
    char* Location;
    char* Filename;
};

typedef struct DeclarationData DeclarationData;

DeclarationData* create_DeclarationData();

void dispose_DeclarationData(DeclarationData* d);

#endif