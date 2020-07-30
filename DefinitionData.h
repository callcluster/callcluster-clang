#ifndef DEFINITION_DATA_H
#define DEFINITION_DATA_H

struct DefinitionData {
    char* Location;
    char* Filename;
    unsigned int NumberOfLines;
    unsigned int NumberOfStatements;
};

typedef struct DefinitionData DefinitionData;

DefinitionData* create_DefinitionData();

void dispose_DefinitionData(DefinitionData* d);

#endif /* DEFINITION_DATA_H */