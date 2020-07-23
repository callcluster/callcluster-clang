#include "Visitor.h"
#include "CallgraphVisit.h"
#include <stdio.h>

enum CXChildVisitResult cursor_visitor(CXCursor cursor, CXCursor parent, CXClientData client_data)
{
   //CallgraphVisit visit = (CallgraphVisit) client_data;
   
   //if( cursor.kind == CXCursor_FunctionDecl)//decl
   if( cursor.kind == CXCursor_DeclRefExpr){
      CXCursor def = clang_getCursorReferenced(cursor);
      CXString usr = clang_getCursorUSR(def);
      printf("A call to: %s \n",clang_getCString(usr));
      clang_disposeString(usr);
   }
   
   CXString usr = clang_getCursorUSR(cursor);
   printf("thing: %s \n",clang_getCString(usr));
   printf("kind: %u \n",cursor.kind);
   clang_disposeString(usr);
   


   return CXChildVisit_Recurse;
}

void visit_translationUnit(CXTranslationUnit tu, GatheredCallgraph gathered_callgraph)
{
    CXCursor cursor = clang_getTranslationUnitCursor(tu);
    printf("cursor kind: %u",cursor.kind);
    CallgraphVisit visit = createCallgraphVisit(gathered_callgraph);
    clang_visitChildren(
        cursor,
        cursor_visitor,
        visit
    );
    disposeCallgraphVisit(visit);
}