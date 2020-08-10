#include "parameters.h"
#include <stdio.h>
#include <string.h>
#define SHOW_FLOW 1
#define SHOW_FUNCTIONS 2

enum Verbosity __verbosity__ = Nothing;
const char* __analyzed_funtion__ = NULL;
void print_function_usr(const char* usr){
    if(__verbosity__ & SHOW_FUNCTIONS){
        printf("function: %s\n",usr);
    }
}

void print_flow_edge(unsigned int from, unsigned int to){
    if(__verbosity__ & SHOW_FLOW){
        printf("%d -> %d\n",from,to);
    }
}

void print_flow_enter(const char* type_of_statement){
    if(__verbosity__ & SHOW_FLOW){
        printf("#enter:%s\n",type_of_statement);
    }
}

void print_flow_exit(const char* type_of_statement){
    if(__verbosity__ & SHOW_FLOW){
        printf("#exit:%s\n",type_of_statement);
    }
}

void print_flow_goto(unsigned int num, const char* label){
    if(__verbosity__ & SHOW_FLOW){
        printf("%d -> %s\n", num, label);
    }
}

void print_flow_labeled(unsigned int num, const char* label){
    if(__verbosity__ & SHOW_FLOW){
        printf("%s -> %d\n", label, num);
    }
}

void print_flow_case(){
    if(__verbosity__ & SHOW_FLOW){
        printf("#case\n");
    }
}

void print_flow_default(){
    if(__verbosity__ & SHOW_FLOW){
        printf("#default\n");
    }
}

void print_flow_end(unsigned int num){
    if(__verbosity__ & SHOW_FLOW){
        printf("#end:%d\n",num);
    }
}

void set_parameters(int argc, char *argv[]){
    if(argc>=3){
        if(strstr(argv[2],"functions")!=NULL){
            __verbosity__ |= SHOW_FUNCTIONS;
        }
        if(strstr(argv[2],"flow")!=NULL){
            __verbosity__ |= SHOW_FLOW;
        }
    }

    if(argc>=4){
        __analyzed_funtion__ = argv[3];
    }
}

