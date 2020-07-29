#ifndef DEFINITION_DATA_H
#define DEFINITION_DATA_H

struct DefinitionData {
    char* Location;
    unsigned int NumberOfLines;
};

typedef struct DefinitionData DefinitionData;

DefinitionData* create_DefinitionData();

DefinitionData* create_DefinitionData_with_params(const char* location, unsigned int number_of_lines);

void dispose_DefinitionData(DefinitionData* d);

#endif /* DEFINITION_DATA_H */