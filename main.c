#include <stdio.h>
#include <clang-c/Index.h>
#include <clang-c/CXCompilationDatabase.h>
#include <stdlib.h>


enum CXChildVisitResult cursor_visitor(CXCursor cursor, CXCursor parent, CXClientData client_data)
{
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

void analyze_command(CXCompileCommand command, CXIndex cxindex)
{
   CXString filename = clang_CompileCommand_getFilename(command);

   unsigned int num_args = clang_CompileCommand_getNumArgs(command);

   CXString* args = (CXString*) malloc(num_args * sizeof(CXString));
   for( unsigned int i = 0; i < num_args; i++ )
   {
      args[i] = clang_CompileCommand_getArg(command,i);
   }

   const char** cargs = malloc(num_args * sizeof(char*));
   for( unsigned int i = 0; i < num_args; i++ )
   {
      cargs[i] = clang_getCString(args[i]);
   }


   CXTranslationUnit tu;
   clang_parseTranslationUnit2FullArgv(//o talvez clang_parseTranslationUnit2FullArgv ?
      cxindex,
      NULL,
      cargs,num_args,
      0,0,
      CXTranslationUnit_None,
      &tu
   );

   CXCursor cursor = clang_getTranslationUnitCursor(tu);
   printf("cursor kind: %u",cursor.kind);
   clang_visitChildren(
      cursor,
      cursor_visitor,
      0
   );

   clang_disposeTranslationUnit(tu);

   clang_disposeString(filename);
   for(unsigned int i = 0; i < num_args; i++ )
   {
      clang_disposeString(args[i]);
   }
   free(args);
   free(cargs);
}

int main(int argc, char *argv[]) {
   if(argc < 2)
   {
      printf("Please pass the build folder where compile_commands.json is located.");
      exit(0);
   }

   CXCompilationDatabase_Error error;
   CXCompilationDatabase db = clang_CompilationDatabase_fromDirectory("./test-projects/simple/build", &error);
   if(error == CXCompilationDatabase_CanNotLoadDatabase)
   {
      printf("A compile_commands.json in that directory cannot be loaded.");
      clang_CompilationDatabase_dispose(db);
      exit(0);
   }

   CXCompileCommands commands = clang_CompilationDatabase_getAllCompileCommands(db);
   unsigned int commands_number = clang_CompileCommands_getSize(commands);
   CXIndex index = clang_createIndex(1, 0);//esto hay que compartirlo entre todos los parse!!
   clang_CXIndex_setInvocationEmissionPathOption(index,"clang.log");
   printf("Number of commands found: %u\n",commands_number);
   for(unsigned int c=0; c < commands_number; c++)
   {
      printf("Analyzing command: %u of %u\n", c, commands_number);
      CXCompileCommand command = clang_CompileCommands_getCommand(commands,c);
      analyze_command(command, index);
   }
   clang_disposeIndex(index);
   clang_CompileCommands_dispose(commands);
   clang_CompilationDatabase_dispose(db);

   return 0;
}
