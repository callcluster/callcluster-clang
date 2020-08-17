#define _GNU_SOURCE
#include <stdio.h>
#include <clang-c/Index.h>
#include <clang-c/CXCompilationDatabase.h>
#include <stdlib.h>
#include "serialization.h"
#include "GatheredCallgraph.h"
#include "Visitor.h"
#include <parameters.h>
#include <string.h>
#include <unistd.h>

char* mallocopy_mn(CXString s)
{
   const char* c_str = clang_getCString(s);
   char* ret = malloc((strlen(c_str) + 1) * sizeof(char));
   strcpy(ret,c_str);
   return ret;
}

char* get_filename(CXCompileCommand command)
{
   CXString filename = clang_CompileCommand_getFilename(command);
   CXString directory = clang_CompileCommand_getDirectory(command);
   char* ret = malloc(sizeof(char)*(strlen(clang_getCString(filename)) + strlen(clang_getCString(directory)) + 10));
   sprintf(ret,"%s/%s",clang_getCString(directory),clang_getCString(filename));
   return ret;
}

void get_arguments(CXCompileCommand command, char*** argv_list_out, unsigned* argc_out)
{
   unsigned filename_found=0;
   unsigned argc = clang_CompileCommand_getNumArgs(command);
   CXString filename = clang_CompileCommand_getFilename(command);

   char** argv = malloc((argc-1) * sizeof(char*));
   int argv_i=0;
   for(int i=0;i<argc;i++){
      CXString arg = clang_CompileCommand_getArg(command,i);
      if(strcmp(clang_getCString(arg),clang_getCString(filename)) == 0){
         filename_found=1;
      }else{
         argv[argv_i]=mallocopy_mn(arg);
         argv_i++;
      }
      clang_disposeString(arg);
   }

   *argv_list_out = argv;
   *argc_out = argv_i;

   clang_disposeString(filename);
}

void analyze_command(CXCompileCommand command, CXIndex cxindex, GatheredCallgraph gathered_callgraph)
{
   char** argv;
   unsigned argc;
   get_arguments(command,&argv,&argc);
   //char* filename=get_filename(command);
   CXString filename = clang_CompileCommand_getFilename(command);
   CXString directory = clang_CompileCommand_getDirectory(command);

   CXTranslationUnit tu;
   chdir(clang_getCString(directory));//necessary to better simulate the compiler
   enum CXErrorCode err = clang_parseTranslationUnit2(
      cxindex,
      clang_getCString(filename),
      argv,argc,
      0,0,
      CXTranslationUnit_None,
      &tu
   );
   if(err != CXError_Success){
      printf("Error %d loading %s at %s\n",err,clang_getCString(filename), clang_getCString(directory));
   }
   visit_translationUnit(tu,gathered_callgraph);
   clang_disposeTranslationUnit(tu);

   for(unsigned int i = 0; i < argc; i++ )
   {
      free(argv[i]);
   }
   free(argv);
   clang_disposeString(filename);
   clang_disposeString(directory);
}

int main(int argc, char *argv[]) {
   clang_enableStackTraces();
   if(argc < 2)
   {
      printf("Please pass the build folder where compile_commands.json is located.");
      exit(0);
   }
   CXCompilationDatabase_Error error;
   CXCompilationDatabase db = clang_CompilationDatabase_fromDirectory(argv[1], &error);
   if(error == CXCompilationDatabase_CanNotLoadDatabase)
   {
      printf("A compile_commands.json in that directory cannot be loaded.");
      clang_CompilationDatabase_dispose(db);
      exit(0);
   }

   set_parameters(argc,argv);

   char* initial_work_dir = get_current_dir_name();

   CXCompileCommands commands = clang_CompilationDatabase_getAllCompileCommands(db);
   unsigned int commands_number = clang_CompileCommands_getSize(commands);
   CXIndex index = clang_createIndex(1, 0);
   GatheredCallgraph gathered_callgraph = createGatheredCallgraph();
   clang_CXIndex_setInvocationEmissionPathOption(index,"./clang.log");
   print_progress_total(commands_number);
   for(unsigned int c=0; c < commands_number; c++)
   {
      print_progress_current(c,commands_number);
      CXCompileCommand command = clang_CompileCommands_getCommand(commands,c);
      analyze_command(command, index, gathered_callgraph);
   }
   chdir(initial_work_dir);
   save(gathered_callgraph,"analysis.json");
   disposeGatheredCallgraph(gathered_callgraph);
   clang_disposeIndex(index);
   clang_CompileCommands_dispose(commands);
   clang_CompilationDatabase_dispose(db);
   free(initial_work_dir);

   return 0;
}
