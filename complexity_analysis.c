#include "complexity_analysis.h"
#include <stdio.h>
#include <stdlib.h>

struct Node {
    int holis;
};
typedef struct Node Node;

struct Operation {
    enum CXCursorKind Head;
    struct Operation* Tail;
};
typedef struct Operation Operation;

typedef struct {
    Operation* OpStack;
} Visit;
        
void Visit_enter(Visit* v, enum CXCursorKind k)
{
    CXString s = clang_getCursorKindSpelling(k);
    printf("enter: %s\n",clang_getCString(s));
    clang_disposeString(s);

    Operation* new = malloc(sizeof(Operation));
    new->Head = k;
    new->Tail = v->OpStack;
    v->OpStack = new;
}

void Visit_exit(Visit* v)
{
    if(v->OpStack != NULL){
        Operation* old = v->OpStack;
        v->OpStack = v->OpStack->Tail;

        CXString s = clang_getCursorKindSpelling(old->Head);
        printf("exit: %s\n",clang_getCString(s));
        clang_disposeString(s);

        free(old);
    }
}

void Visit_expression(Visit* v)
{
    printf("e\n");
}


enum CXChildVisitResult general_visitor (CXCursor cursor, CXCursor parent, CXClientData client_data)
{
    Visit* visit = (Visit*) client_data;
    enum CXCursorKind kind = clang_getCursorKind(cursor);
    if(clang_isStatement(kind)){

        Visit_enter(visit,kind);
        clang_visitChildren(cursor, general_visitor, (CXClientData) visit);
        Visit_exit(visit);


        return CXChildVisit_Continue;
    }else{
        if(clang_isStatement(clang_getCursorKind(parent))){
            Visit_expression(visit);
        }

        return CXChildVisit_Recurse;
    }
}

Visit* Visit_create()
{
    Visit* ret = malloc(sizeof(Visit));
    ret->OpStack = NULL;
    return ret;
}

unsigned int Visit_get_complexity(Visit* v)
{
    return 2;
}

void Visit_dispose(Visit* v)
{
    free(v);
}

unsigned int get_complexity(CXCursor c)
{
    printf("-----\n");
    Visit* v = Visit_create();
    clang_visitChildren(c, general_visitor, (CXClientData) v);
    unsigned int complexity = Visit_get_complexity(v);
    Visit_dispose(v);
    return complexity;
}