#ifndef DEFINITION_ANALYSIS_H
#define DEFINITION_ANALYSIS_H
#include "DefinitionData.h"
#include "DeclarationData.h"
#include <clang-c/Index.h>
DefinitionData* analyze(CXCursor);
DeclarationData* analyze_declaration(CXCursor);
#endif /* DEFINITION_ANALYSIS_H */