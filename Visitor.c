#include "Visitor.h"
#include "CallgraphVisit.h"

enum CXChildVisitResult call_visitor(CXCursor cursor, CXCursor parent, CXClientData client_data)
{
    CallgraphVisit visit = (CallgraphVisit) client_data;

    if(cursor.kind == CXCursor_DeclRefExpr){
        CXCursor referenced = clang_getCursorReferenced(cursor);
        if(referenced.kind == CXCursor_FunctionDecl){
            CallgraphVisit_addCall(visit, referenced);
        }
    }
    return CXChildVisit_Recurse;
}

enum CXChildVisitResult declaration_visitor(CXCursor cursor, CXCursor parent, CXClientData client_data)
{
    CallgraphVisit visit = (CallgraphVisit) client_data;

    if(cursor.kind == CXCursor_FunctionDecl){
        CallgraphVisit_addFunctionDeclaration(visit, cursor);
        if(clang_isCursorDefinition(cursor) != 0){
            CallgraphVisit_setCurrentFunctionDefinition(visit, cursor);
            clang_visitChildren(
                cursor,
                call_visitor,
                visit
            );
        }
    }
    
    return CXChildVisit_Continue;
}

void visit_translationUnit(CXTranslationUnit tu, GatheredCallgraph gathered_callgraph)
{
    CXCursor cursor = clang_getTranslationUnitCursor(tu);
    CallgraphVisit visit = CallgraphVisit_create(gathered_callgraph);
    clang_visitChildren(
        cursor,
        declaration_visitor,
        visit
    );
    CallgraphVisit_dispose(visit);
}