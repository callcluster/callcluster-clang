#include "parameters.h"
#include <stdio.h>
#include <string.h>
#define SHOW_FLOW 1
#define SHOW_FUNCTIONS 2
#define SHOW_PROGRESS 4
unsigned int __verbosity__ = 0;

unsigned int __mccabe__ = 0;

unsigned int calculate_mccabe_complexity(){
    return __mccabe__;
}

void set_parameters(int argc, char *argv[]){
    if(argc>=3){
        if(strstr(argv[2],"functions")!=NULL){
            __verbosity__ |= SHOW_FUNCTIONS;
        }
        if(strstr(argv[2],"flow")!=NULL){
            __verbosity__ |= SHOW_FLOW;
        }
        if(strstr(argv[2],"progress")!=NULL){
            __verbosity__ |= SHOW_PROGRESS;
        }
        if(strstr(argv[2],"mccabe")!=NULL){
            __mccabe__ = 1;
        }
    }

    if(argc>=4){
        __analyzed_funtion__ = argv[3];
    }
}

///// ---------- SHOW_FUNCTIONS ---------- //////

void print_function_usr(const char* usr){
    if(__verbosity__ & SHOW_FUNCTIONS){
        printf("function: %s\n",usr);
    }
}


//// ------- SHOW_FLOW ------////

const char* __analyzed_funtion__ = NULL;
const char* __current_analyzed_function__ = NULL;

unsigned int can_show_flow(){
    return (__verbosity__ & SHOW_FLOW) && (
        __analyzed_funtion__==NULL 
        || 
        0==strcmp(__analyzed_funtion__,__current_analyzed_function__)
    );
}

void set_current_analyzed_function(const char* usr){
    __current_analyzed_function__ = usr;
}



void print_flow_edge(unsigned int from, unsigned int to){
    if(!can_show_flow())return;
    printf("%d -> %d\n",from,to);
}

void print_flow_enter(const char* type_of_statement){
    if(!can_show_flow())return;
    printf("#enter:%s\n",type_of_statement);
}

void print_flow_exit(const char* type_of_statement){
    if(!can_show_flow())return;
    printf("#exit:%s\n",type_of_statement);
}

void print_flow_goto(unsigned int num, const char* label){
    if(!can_show_flow())return;
    printf("%d -> %s\n", num, label);
}

void print_flow_labeled(unsigned int num, const char* label){
    if(!can_show_flow())return;
    printf("%s -> %d\n", label, num);
}

void print_flow_case(){
    if(!can_show_flow())return;
    printf("#case\n");
}

void print_flow_default(){
    if(!can_show_flow())return;
    printf("#default\n");
}

void print_flow_end(unsigned int num){
    if(!can_show_flow()) return;
    printf("#end:%d\n",num);
}
/// ---------------- SHOW_PROGRESS ------------- /////

void print_progress_total(unsigned int total){
    if(__verbosity__ & SHOW_PROGRESS){
        printf("Number of commands found: %u\n", total);
    }
}

void print_progress_current(unsigned int current, unsigned int total){
    if(__verbosity__ & SHOW_PROGRESS){
        printf("Analyzing command: %u of %u\n", current, total);
    }
}