#include "complexity_analysis.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ComplexityVisitor.h"

char* mallocopy_ca(const char* copied)
{
    char* ret = malloc( sizeof(char) * (strlen(copied) + 1) );
    strcpy(ret,copied);
    return ret;
}

enum OperationKind equivalent_operation(enum CXCursorKind k)
{
    switch(k){
        case CXCursor_CompoundStmt: return Op_CompoundStmt;
        case CXCursor_IfStmt: return Op_IfStmt;
        case CXCursor_SwitchStmt: return Op_SwitchStmt;
        case CXCursor_ForStmt: return Op_ForStmt;
        case CXCursor_WhileStmt: return Op_WhileStmt;
        default: return Op_CompoundStmt;
    }
}

enum CXChildVisitResult general_visitor (CXCursor cursor, CXCursor parent, CXClientData client_data)
{
    Visit* visit = (Visit*) client_data;
    enum CXCursorKind kind = clang_getCursorKind(cursor);
    switch(kind){

        case CXCursor_CompoundStmt:
        case CXCursor_IfStmt:
        case CXCursor_SwitchStmt:
        case CXCursor_ForStmt:
        case CXCursor_WhileStmt:
        {
            Visit* new_visit = Visit_enter(visit,equivalent_operation(kind));
            clang_visitChildren(cursor, general_visitor, (CXClientData) new_visit);
            Visit_exit(new_visit);
            Visit_dispose(new_visit);
        }
        return CXChildVisit_Continue;

        case CXCursor_CaseStmt:
        Visit_case(visit);
        clang_visitChildren(cursor, general_visitor, (CXClientData) visit);
        return CXChildVisit_Continue;

        case CXCursor_DefaultStmt:
        Visit_default(visit);
        clang_visitChildren(cursor, general_visitor, (CXClientData) visit);
        return CXChildVisit_Continue;

        case CXCursor_BreakStmt:
        Visit_break(visit);
        return CXChildVisit_Continue;

        case CXCursor_ContinueStmt:
        Visit_continue(visit);
        return CXChildVisit_Continue;

        case CXCursor_ReturnStmt:
        Visit_return(visit);
        return CXChildVisit_Continue;

        case CXCursor_LabelStmt:
        {
            CXString d = clang_getCursorDisplayName(cursor);
            char* copied = mallocopy_ca(clang_getCString(d));
            Visit_label(visit,copied);
            clang_disposeString(d);
            free(copied);
            clang_visitChildren(cursor, general_visitor, (CXClientData) visit);
            return CXChildVisit_Continue;
        }
        case CXCursor_GotoStmt:
        {
            clang_visitChildren(cursor, general_visitor, (CXClientData) visit);
            return CXChildVisit_Continue;
        }
        

        default:
        if(clang_getCursorKind(cursor) == CXCursor_LabelRef){
            CXString d = clang_getCursorDisplayName(cursor);
            char* copied = mallocopy_ca(clang_getCString(d));
            Visit_goto(visit,copied);
            free(copied);
            clang_disposeString(d);
        }else{
            if(clang_isStatement(clang_getCursorKind(parent))) Visit_expression(visit);
        }
        return CXChildVisit_Recurse;
    }
}

typedef void (*LabelFunType)(NodeClientDataType client_data, ComplexityNode node, const char* label);


#include "parameters.h"
typedef struct GraphNode GraphNode;
struct GraphNode{
    unsigned int NodeNumber;
    GraphNode* Previous;
};
struct GraphVisit{
    unsigned int vertices_count;
    unsigned int edges_count;
    unsigned int current_node_number;
    struct GraphNode* LastNode;
};
void add_edge(NodeClientDataType client_data, ComplexityNode from, ComplexityNode to)
{
    struct GraphVisit* data = ((struct GraphVisit*) client_data);
    (data->edges_count)++;
    if(to!=NULL && from!=NULL)
    print_flow_edge(
        ((GraphNode*)from)->NodeNumber,
        ((GraphNode*)to)->NodeNumber
    );
}

ComplexityNode create_node(NodeClientDataType client_data)
{
    GraphNode* new = malloc(sizeof(GraphNode));
    struct GraphVisit* data = ((struct GraphVisit*) client_data);
    new->Previous=data->LastNode;
    data->LastNode=new;
    new->NodeNumber=data->current_node_number;
    (data->current_node_number)++;
    (data->vertices_count)++;
    return (ComplexityNode) new;
}

void goto_label(NodeClientDataType client_data, ComplexityNode node, const char* label)
{
    struct GraphVisit* data = ((struct GraphVisit*) client_data);
    GraphNode* n = (GraphNode*)node;
    (data->vertices_count)++;
    (data->edges_count)++;
    print_flow_goto(n->NodeNumber,label);
}

void set_label(NodeClientDataType client_data, ComplexityNode node, const char* label)
{
    struct GraphVisit* data = ((struct GraphVisit*) client_data);
    GraphNode* n = (GraphNode*)node;
    (data->vertices_count)++;
    (data->edges_count)++;
    print_flow_labeled(n->NodeNumber,label);
}

void free_nodes(struct GraphNode* n)
{
    if(n!=NULL){
        struct GraphNode* next=n->Previous;
        free(n);
        free_nodes(next);
    }
    
}

unsigned int get_complexity(CXCursor c)
{
    if(!calculate_mccabe_complexity()){
        return 0;
    }
    ComplexityParameters parameters;
    parameters.AddEdge=add_edge;
    parameters.CreateNode=create_node;
    parameters.GotoLabel=goto_label;
    parameters.SetLabel=set_label;
    struct GraphVisit visit;
    visit.edges_count=0;
    visit.vertices_count=0;
    visit.current_node_number=0;
    visit.LastNode=NULL;
    parameters.NodeClientData=(NodeClientDataType) &visit;
    Visit* v = Visit_create(&parameters);
    clang_visitChildren(c, general_visitor, (CXClientData) v);
    free_nodes(visit.LastNode);
    unsigned int complexity = visit.edges_count-visit.vertices_count+2;
    Visit_dispose(v);
    return complexity;
}