#include "definition_analysis.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

char* create_location_string(CXCursor c)
{
    CXSourceLocation loc = clang_getCursorLocation(c);
    CXString filename;
    unsigned int line;
    clang_getPresumedLocation(loc,&filename,&line,NULL);
    const char* c_filename = clang_getCString(filename);
    
    char* ret=malloc(sizeof(char)*(strlen(c_filename) + 50));
    sprintf(ret,"file: %s line: %d",c_filename,line);

    clang_disposeString(filename);
    return ret;
}

char* mallocopy_da(const char* copied)
{
    char* ret = malloc( sizeof(char) * (strlen(copied) + 1) );
    strcpy(ret,copied);
    return ret;
}

char* create_filename_string(CXCursor c)
{
    CXSourceLocation loc = clang_getCursorLocation(c);
    CXString filename;
    clang_getPresumedLocation(loc,&filename,NULL,NULL);
    const char* c_filename = clang_getCString(filename);
    char* ret=mallocopy_da(c_filename);
    clang_disposeString(filename);
    return ret;
}

unsigned int lines_of(CXCursor c)
{
    CXSourceRange r = clang_getCursorExtent(c);
    CXSourceLocation start = clang_getRangeStart(r);
    unsigned int line_start;
    clang_getPresumedLocation(start,NULL,&line_start,NULL);

    CXSourceLocation end = clang_getRangeEnd(r);
    unsigned int line_end;
    clang_getPresumedLocation(end,NULL,&line_end,NULL);

    return line_end - line_start + 1;
}

// It doesn't count statements but the syntax rule called 'expression_statement' here: https://www.lysator.liu.se/c/ANSI-C-grammar-y.html#expression
enum CXChildVisitResult statement_counter(CXCursor cursor, CXCursor parent, CXClientData client_data)
{
    unsigned int* statements = (unsigned int*)client_data;
    enum CXCursorKind k = clang_getCursorKind(cursor);
    if(clang_isExpression(k) && clang_isStatement(clang_getCursorKind(parent))){
        (*statements) += 1;
    }
    return CXChildVisit_Recurse;
}

unsigned int number_of_statements(CXCursor c)
{
    unsigned int statements = 0;
    clang_visitChildren(c,statement_counter, (CXClientData) &statements);
    return statements;
}

DefinitionData* analyze(CXCursor c){
    DefinitionData* ret = create_DefinitionData();

    ret->Location = create_location_string(c);
    ret->NumberOfLines = lines_of(c);
    ret->NumberOfStatements = number_of_statements(c);
    ret->Filename = create_filename_string(c);
    return ret;
}