#ifndef PARAMETERS_H
#define PARAMETERS_H
const char* __analyzed_funtion__;
enum Verbosity{
    Nothing,
    ListFunctions,
    ShowFlowGraph
};
void print_function_usr(const char* usr);
void print_flow_enter(const char* type_of_statement);
void print_flow_exit(const char* type_of_statement);
void print_flow_goto(unsigned int num, const char* label);
void print_flow_labeled(unsigned int num, const char* label);
void print_flow_case();
void print_flow_default();
void print_flow_end(unsigned int num);
void set_parameters(int argc, char *argv[]);
#endif