#ifndef STRUCTURAL_ANALYSIS_H
#define STRUCTURAL_ANALYSIS_H
#include "GatheredCallgraph.h"

typedef void* StructuralAnalysis;
typedef void* Community;

typedef void (*CommunityVisitor) (
    Community community,
    char* name, 
    unsigned int functions[], 
    unsigned int functions_size,
    void* data
);

StructuralAnalysis create_StructuralAnalysis(GatheredCallgraph);
void dispose_StructuralAnalysis(StructuralAnalysis);
void StructuralAnalysis_visitRoot(StructuralAnalysis, CommunityVisitor visitor, void* data);
void Community_visitChildren(Community, CommunityVisitor visitor, void* data);


#endif /* STRUCTURAL_ANALYSIS_H */