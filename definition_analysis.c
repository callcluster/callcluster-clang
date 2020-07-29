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

unsigned int number_of_statements(CXCursor c)
{
    return 3;
}

DefinitionData* analyze(CXCursor c){
    DefinitionData* ret = create_DefinitionData();

    ret->Location = create_location_string(c);
    ret->NumberOfLines = lines_of(c);
    ret->NumberOfStatements = number_of_statements(c);
    return ret;
}