#include "definition_analysis.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "CabeComplexity/complexity_analysis.h"
#include "basili_complexity.h"
#include <limits.h>

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
    char* ret=malloc(sizeof(char)*PATH_MAX);
    realpath(clang_getCString(filename),ret);
    clang_disposeString(filename);
    return ret;
}

int lines_of(CXCursor c)
{
    CXSourceRange r = clang_getCursorExtent(c);
    CXSourceLocation start = clang_getRangeStart(r);
    int line_start;
    clang_getPresumedLocation(start,NULL,&line_start,NULL);

    CXSourceLocation end = clang_getRangeEnd(r);
    int line_end;
    clang_getPresumedLocation(end,NULL,&line_end,NULL);

    return line_end - line_start + 1;
}

// It doesn't count statements but the syntax rule called 'expression_statement' here: https://www.lysator.liu.se/c/ANSI-C-grammar-y.html#expression
enum CXChildVisitResult statement_counter(CXCursor cursor, CXCursor parent, CXClientData client_data)
{
    int* statements = (int*)client_data;
    enum CXCursorKind k = clang_getCursorKind(cursor);
    if(clang_isExpression(k) && clang_isStatement(clang_getCursorKind(parent))){
        (*statements) += 1;
    }
    return CXChildVisit_Recurse;
}

int number_of_statements(CXCursor c)
{
    int statements = 0;
    clang_visitChildren(c,statement_counter, (CXClientData) &statements);
    return statements;
}

DefinitionData* analyze(CXCursor c){
    DefinitionData* ret = create_DefinitionData();

    ret->Location = create_location_string(c);
    ret->linesOfCode = lines_of(c);
    ret->NumberOfStatements = number_of_statements(c);
    ret->Filename = create_filename_string(c);
    ret->CyclomaticComplexity = get_complexity(c);
    ret->BasiliComplexity = get_basili_complexity(c);

    return ret;
}

char* create_name_string(CXCursor c)
{
    CXString s = clang_getCursorDisplayName(c);
    char* ret = mallocopy_da(clang_getCString(s));
    clang_disposeString(s);
    return ret;
}

DeclarationData* analyze_declaration(CXCursor c)
{
    DeclarationData* ret = create_DeclarationData();
    ret->Name = create_name_string(c);
    ret->Filename=create_filename_string(c);
    ret->Location=create_location_string(c);
    return ret;
}