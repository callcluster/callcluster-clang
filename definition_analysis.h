#ifndef DEFINITION_ANALYSIS_H
#define DEFINITION_ANALYSIS_H
#include "DefinitionData.h"
#include <clang-c/Index.h>
DefinitionData* analyze(CXCursor);
#endif /* DEFINITION_ANALYSIS_H */