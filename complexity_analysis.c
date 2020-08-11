#include "complexity_analysis.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ComplexityVisitor.h"

char* mallocopy_ca(const char* copied)
{
    char* ret = malloc( sizeof(char) * (strlen(copied) + 1) );
    strcpy(ret,copied);
    return ret;
}

enum OperationKind equivalent_operation(enum CXCursorKind k)
{
    switch(k){
        case CXCursor_CompoundStmt: return Op_CompoundStmt;
        case CXCursor_IfStmt: return Op_IfStmt;
        case CXCursor_SwitchStmt: return Op_SwitchStmt;
        case CXCursor_ForStmt: return Op_ForStmt;
        case CXCursor_WhileStmt: return Op_WhileStmt;
        default: return Op_CompoundStmt;
    }
}

enum CXChildVisitResult general_visitor (CXCursor cursor, CXCursor parent, CXClientData client_data)
{
    Visit* visit = (Visit*) client_data;
    enum CXCursorKind kind = clang_getCursorKind(cursor);
    switch(kind){

        case CXCursor_CompoundStmt:
        case CXCursor_IfStmt:
        case CXCursor_SwitchStmt:
        case CXCursor_ForStmt:
        case CXCursor_WhileStmt:
        {
            Visit* new_visit = Visit_enter(visit,equivalent_operation(kind));
            clang_visitChildren(cursor, general_visitor, (CXClientData) new_visit);
            Visit_exit(new_visit);
        }
        
        return CXChildVisit_Continue;

        case CXCursor_CaseStmt:
        Visit_case(visit);
        clang_visitChildren(cursor, general_visitor, (CXClientData) visit);
        return CXChildVisit_Continue;

        case CXCursor_DefaultStmt:
        Visit_default(visit);
        clang_visitChildren(cursor, general_visitor, (CXClientData) visit);
        return CXChildVisit_Continue;

        case CXCursor_BreakStmt:
        Visit_break(visit);
        return CXChildVisit_Continue;

        case CXCursor_ContinueStmt:
        Visit_continue(visit);
        return CXChildVisit_Continue;

        case CXCursor_ReturnStmt:
        Visit_return(visit);
        return CXChildVisit_Continue;

        case CXCursor_LabelStmt:
        {
            CXString d = clang_getCursorDisplayName(cursor);
            Visit_label(visit,mallocopy_ca(clang_getCString(d)));
            clang_disposeString(d);
            clang_visitChildren(cursor, general_visitor, (CXClientData) visit);
            return CXChildVisit_Continue;
        }
        case CXCursor_GotoStmt:
        {
            clang_visitChildren(cursor, general_visitor, (CXClientData) visit);
            return CXChildVisit_Continue;
        }
        

        default:
        if(clang_getCursorKind(cursor) == CXCursor_LabelRef){
            CXString d = clang_getCursorDisplayName(cursor);
            Visit_goto(visit,mallocopy_ca(clang_getCString(d)));
            clang_disposeString(d);
        }else{
            if(clang_isStatement(clang_getCursorKind(parent))) Visit_expression(visit);
        }
        return CXChildVisit_Recurse;
    }
}

unsigned int get_complexity(CXCursor c)
{
    Visit* v = Visit_create();
    clang_visitChildren(c, general_visitor, (CXClientData) v);
    unsigned int complexity = Visit_get_complexity(v);
    Visit_dispose(v);
    return complexity;
}