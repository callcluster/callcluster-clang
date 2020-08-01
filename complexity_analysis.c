#include "complexity_analysis.h"
#include <stdio.h>

typedef struct {
    int size;
} ControlFlowGraphVisit;

/*
void visit_statement(CXCursor cursor,CXClientData client_data)
{
    enum CXCursorKind kind = clang_getCursorKind(cursor);
    switch(kind){
        case CXCursor_CompoundStmt: visit_compound(cursor,client_data); break;
        case CXCursor_IfStmt: visit_if(cursor,client_data); break;
        case CXCursor_SwitchStmt:  visit_switch(cursor,client_data); break;
        case CXCursor_WhileStmt: visit_while(cursor,client_data); break;
        case CXCursor_DoStmt: visit_do(cursor,client_data); break;
        case CXCursor_ForStmt: visit_for(cursor,client_data); break;
    }
}
*/
enum CXChildVisitResult general_visitor (CXCursor cursor, CXCursor parent, CXClientData client_data)
{
    enum CXCursorKind kind = clang_getCursorKind(cursor);
    if(clang_isStatement(kind)){
        CXString s = clang_getCursorKindSpelling(kind);
        printf("kind: %s\n",clang_getCString(s));
        clang_disposeString(s);
        return CXVisit_Continue;
    }else{
        return CXChildVisit_Recurse;
    }
}

unsigned int get_complexity(CXCursor c)
{
    ControlFlowGraphVisit g;
    clang_visitChildren(c, general_visitor, (CXClientData) &g);
    return 2;
}