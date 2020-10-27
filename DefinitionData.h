#ifndef DEFINITION_DATA_H
#define DEFINITION_DATA_H

struct DefinitionData {
    char* Location;
    char* Filename;
    int linesOfCode;
    int NumberOfStatements;
    int CyclomaticComplexity;
    int BasiliComplexity;
};

typedef struct DefinitionData DefinitionData;

DefinitionData* create_DefinitionData();

void dispose_DefinitionData(DefinitionData* d);

#endif /* DEFINITION_DATA_H */