#ifndef VISITOR_H
#define VISITOR_H
#include <clang-c/Index.h>
#include "GatheredCallgraph.h"
void visit_translationUnit(CXTranslationUnit tu, GatheredCallgraph gathered_callgraph);
#endif /* VISITOR_H */