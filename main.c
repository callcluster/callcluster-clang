#include <stdio.h>
#include <clang-c/Index.h>
#include <clang-c/CXCompilationDatabase.h>
#include <stdlib.h>

void analyzeCommand(CXCompileCommand command)
{
   CXString filename = clang_CompileCommand_getFilename(command);

   unsigned int num_args = clang_CompileCommand_getNumArgs(command);

   CXString args[] = malloc(num_args * sizeof(CXString));
   for( unsigned int i = 0; i < num_args; i++ )
   {
      args[i] = clang_CompileCommand_getArg(command,i);
   }

   char* cargs[] = malloc(num_args * sizeof(char*));
   for( unsigned int i = 0; i < num_args; i++ )
   {
      cargs[i] = clang_getCString(args[i]);
   }

   CXIndex cxindex = clang_createIndex(1, 0);//esto hay que compartirlo entre todos los parse!!
   CXTranslationUnit tu;
   clang_parseTranslationUnit2(//o talvez clang_parseTranslationUnit2FullArgv ?
      cxindex,
      clang_getCString(filename),
      cargs,num_args,
      0,0,
      CXTranslationUnit_None,
      &tu
   );

   clang_visitChildren(
      clang_getTranslationUnitCursor(tu),

   );

   clang_disposeTranslationUnit(CXTranslationUnit);//noo, no disponer!! crear todas, almacenarlas, recorrerlas todas con clang_visitchildren, y sólo entonces hacer dispose!!!!

   clang_disposeString(filename);
   for(unsigned int i = 0; i < num_args; i++ )
   {
      clang_disposeString(args[i]);
   }
   free(args);
}

int main(int argc, char *argv[]) {
   if(argc < 2)
   {
      printf("Please pass the build folder where compile_commands.json is located.");
      exit();
   }

   CXCompilationDatabase_Error error;
   CXCompilationDatabase db = clang_CompilationDatabase_fromDirectory("./test-projects/simple/build", &error);
   if(error == CXCompilationDatabase_CanNotLoadDatabase)
   {
      printf("A compile_commands.json in that directory cannot be loaded.");
      clang_CompilationDatabase_dispose(db);
      exit();
   }

   CXCompileCommands commands = clang_CompilationDatabase_getAllCompileCommands(db);
   unsigned int commands_number = clang_CompileCommands_getSize(commands);
   printf("Number of commands found: %u\n",commands_number);
   for(unsigned int c=0; c < commands_number; c++)
   {
      printf("Analyzing command: %u of %u\n", c, commands_number);
      CXCompileCommand command = clang_CompileCommands_getCommand(commands,c);

      analyzeCommand(command);

      clang_CompileCommand_dispose(command);
   }
   clang_CompileCommands_dispose(commands);
   clang_CompilationDatabase_dispose(db);

   return 0;
}
