#include "basili_complexity.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

typedef struct {
    unsigned int ifs;
    unsigned int whiles;
    unsigned int fors;
    unsigned int ands_plus_ors;
    unsigned int ternaries;
    unsigned int labels;
    unsigned int computed_gotos;
    unsigned int cases;
    unsigned int defaults;
} Counts;

unsigned get_ands_plus_ors_in(CXCursor c)
{
    CXTranslationUnit tu = clang_Cursor_getTranslationUnit(c);
    CXSourceRange range = clang_getCursorExtent(c);
    CXToken* tokens;
    unsigned num_tokens, ands=0, ors=0;
    clang_tokenize(tu, range, &tokens, &num_tokens);
    for(unsigned i=0;i<num_tokens;i++){
        CXString spelling = clang_getTokenSpelling(tu,tokens[i]);
        const char* cstring = clang_getCString(spelling);
        if(strcmp(cstring,"&&") == 0){
            ands++;
        }else if(strcmp(cstring,"||") == 0){
            ors++;
        }
        clang_disposeString(spelling);
    }
    clang_disposeTokens(tu,tokens,num_tokens);
    return ands+ors;
}

enum CXChildVisitResult feature_counter(CXCursor cursor, CXCursor parent, CXClientData client_data)
{
    
    Counts* c = (Counts*) client_data;
    CXString s = clang_getCursorKindSpelling(clang_getCursorKind(cursor));
    switch(clang_getCursorKind(cursor)){

        case CXCursor_BinaryOperator:
        c->ands_plus_ors+=get_ands_plus_ors_in(cursor);
        return CXChildVisit_Continue;


        case CXCursor_IfStmt: c->ifs++; 
        break;
        case CXCursor_WhileStmt: c->whiles++; 
        break;
        case CXCursor_ForStmt: c->fors++; 
        break;
        case CXCursor_IndirectGotoStmt: c->computed_gotos++;
        break;
        case CXCursor_LabelStmt: c->labels++;
        break;
        case CXCursor_ConditionalOperator: c->ternaries++;
        break;
        case CXCursor_CaseStmt: c->cases++;
        break;
        case CXCursor_DefaultStmt: c->defaults++;
        break;
        default:
        break;
    }
    return CXChildVisit_Recurse;
}

void initialize_counts(Counts * c)
{
    c->ifs=0;
    c->whiles=0;
    c->fors=0;
    c->ands_plus_ors=0;
    c->ternaries=0;
    c->labels=0;
    c->computed_gotos=0;
    c->cases=0;
    c->defaults=0;
}

unsigned int complexity(Counts counts)
{
    return 
    1 + 
    /*
    "logical if, if-then-else, block if, block if-then-else"
    logical statements = block statements for C
    if's and if-then-else's  are counted separately and added, this is equivalent to counting if keywords.
    */
    counts.ifs +
    /*
    "do loops, while loops"
    C only has while and for loops, so we use those istead
    */
    counts.whiles+
    counts.fors+
    /*
    "AND's, OR's, XOR's, EQV's, NEQV's"
    Theese are all of fortran's binary logical operators, C has only && and ||, the equivalent to XOR, and NEQV is != and the equivalent to  EQV is ==.
    We are going to count only && and ||, as == and != aren't so used for branching conditions, and they are actually used for non-logical checks.
    */
    counts.ands_plus_ors+
    /*
    "twice the number of arithmetic if's"
    this shouldn't be doubled for C, since fortran's  arithmetic IF's have 3 branches but C's have 2.
    */
    counts.ternaries+

    /*
    "n-1 decision counts for a computed go to with n statement labels"
    there is no equivalent for C, but C does have computed gotos for GCC, we just can't tell what labels we are 'gong to', so we must assume it's all of them.
    */
   ((counts.labels > 0)? (counts.computed_gotos*(counts.labels-1)):0) +

    /*
    "n decision counts for a case with n predicates"
    this is equivalent to counting default and case keywords.
    */
    counts.cases+
    counts.defaults;
}

unsigned int get_basili_complexity(CXCursor c)
{
    Counts counts;
    initialize_counts(&counts);
    clang_visitChildren(c,feature_counter, (CXClientData) &counts);
    return complexity(counts);
}