#include "parameters.h"
#include "ComplexityVisitor.h"
#include <stdlib.h>


const char* spell_kind(enum OperationKind k){
    switch(k){
        case Op_CompoundStmt: return "CompoundStmt";
        case Op_IfStmt: return "IfStmt";
        case Op_SwitchStmt: return "SwitchStmt";
        case Op_ForStmt: return "ForStmt";
        case Op_WhileStmt: return "WhileStmt";
        case Op_Function: return "Function";
    }
}
ComplexityNode Node_create(Visit* v)
{
    return v->Parameters->CreateNode(v->Parameters->NodeClientData);
}
void Node_addEdge(Visit* v, ComplexityNode from, ComplexityNode to)
{
    v->Parameters->AddEdge(v->Parameters->NodeClientData, from, to);
}
/*
unsigned int Node_edgeExists(Node* from, Node* to)
{
    NodeList* current=from->Next;
    while (current!=NULL)
    {
        if( current->Head == to ) return 1;
        current = current->Tail;
    }
    return 0;
}

void Node_addEdge(Node* from, Node* to)
{
    if(Node_edgeExists(from,to)) return;
    print_flow_edge(from->NodeNumber,to->NodeNumber);
    NodeList* prev = from->Next;
    from->Next = malloc(sizeof(NodeList));
    from->Next->Head = to;
    from->Next->Tail = prev;
}


// ----------------------------------------------------------------------- //


Node* Node_create(Visit* v)
{
    Node* ret = malloc(sizeof(Node));
    ret->Next = NULL;
    ret->NodeNumber = *(v->node_number);
    ret->Label = NULL;
    ret->GotoLabel = NULL;
    (*(v->node_number))++;
    return ret;
}
*/

void Visit_set_last(Visit* v, ComplexityNode last)
{
    switch(v->Kind){

        case Op_Function:
        Node_addEdge(v,last,v->ReturnNode);
        return;

        case Op_CompoundStmt:
        v->CompoundLast = last;
        return;

        case Op_IfStmt:
        if(v->IfTrueEnd==NULL && !v->IfTrueBranchVisited){
            v->IfTrueEnd = last;
            v->IfTrueBranchVisited = 1;
        }else if(v->IfFalseEnd==NULL && !v->IfFalseBranchVisited){
            v->IfFalseEnd = last;
            v->IfFalseBranchVisited=1;
        }
        return;

        case Op_SwitchStmt:
        Node_addEdge(v,last,v->BreakNode);
        return;

        case Op_ForStmt:
        case Op_WhileStmt:
        {
            ComplexityNode  intermediate = Node_create(v);
            Node_addEdge(v,last,intermediate);
            v->CycleLastNode = intermediate;
        }
        return;
    }
}

Visit* Visit_copy(Visit* v, enum OperationKind kind){
    Visit* new = malloc(sizeof(Visit));
    new->Previous = v;
    new->Entry=v->Entry;
    new->ReturnNode=v->ReturnNode;
    new->node_number=v->node_number;
    new->Kind=kind;

    new->IfTrueEnd = NULL;
    new->IfFalseEnd = NULL;
    new->IfTrueBranchVisited = 0;
    new->IfFalseBranchVisited = 0;
    new->IfConditionVisited = 0;
    new->CondPrevious = NULL;

    new->CompoundLast = v->CompoundLast;
    
    new->BreakNode = v->BreakNode;
    new->CompoundCaseOrigin = NULL;
    new->CycleLastNode = NULL;
    new->ForVisitedExpressions = 0;

    new->ContinueNode = v->ContinueNode;
    new->WhileConditionVisited=0;

    new->Parameters = v->Parameters;

    return new;
}

ComplexityNode Visit_getLast(Visit* v)
{
    switch (v->Kind)
    {
        case Op_Function:
            return v->Entry;
        case Op_CompoundStmt:
            return v->CompoundLast;
        case Op_IfStmt:
        case Op_SwitchStmt:
        case Op_ForStmt:
        case Op_WhileStmt:
            return v->CondPrevious;
    }
}

void Visit_break_flow(Visit* v, ComplexityNode target)
{    
    Node_addEdge(v,Visit_getLast(v), target);
    Visit_set_last(v,Node_create(v));
}
        
Visit* Visit_enter(Visit* v, enum OperationKind k)
{
    print_flow_enter(spell_kind(k));
    Visit* new = Visit_copy(v, k);

    switch (k)
    {
        case Op_CompoundStmt:
        new->CompoundLast=Visit_getLast(v);
        if(v->Kind==Op_SwitchStmt){
            new->CompoundCaseOrigin = Visit_getLast(v);
            Visit_set_last(new,Node_create(new));
        }
        return new;

        case Op_IfStmt:
        new->CondPrevious = Visit_getLast(v);
        return new;

        case Op_SwitchStmt:
        new->CondPrevious = Visit_getLast(v);
        new->BreakNode = Node_create(new);
        return new;

        case Op_ForStmt:
        case Op_WhileStmt:
        new->CondPrevious = Visit_getLast(v);
        new->ContinueNode = Visit_getLast(v);
        new->BreakNode = Node_create(new);
        return new;

        case Op_Function:
        return new;
    }
}


void Visit_exit(Visit* v)
{
    print_flow_exit(spell_kind(v->Kind));
    switch (v->Kind)
    {
        case Op_CompoundStmt:
        Visit_set_last(v->Previous, v->CompoundLast);
        return;

        case Op_IfStmt:
        {
            ComplexityNode final = Node_create(v);

            if(v->IfTrueEnd!=NULL){
                Node_addEdge(v,v->IfTrueEnd,final);
            }

            ComplexityNode falseEnd = v->IfFalseEnd;
            if(falseEnd==NULL){
                if(!v->IfFalseBranchVisited){
                    Node_addEdge(v,v->CondPrevious,final);
                }
            }else{
                Node_addEdge(v,falseEnd,final);
            }
            
            Visit_set_last(v->Previous,final);
        }
        return;

        case Op_ForStmt:
        case Op_WhileStmt:
        {
            Node_addEdge(v,v->CycleLastNode, v->CondPrevious);
            Node_addEdge(v,v->CondPrevious, v->BreakNode);
            Visit_set_last(v->Previous,v->BreakNode);
        }
        return;

        case Op_SwitchStmt:
        Visit_set_last(v->Previous,v->BreakNode);
        return;
    
        default: return;
    }
}

void Visit_flow_to(Visit* v, ComplexityNode next)
{
    Node_addEdge(v,Visit_getLast(v), next);
}

void Visit_flow(Visit* v)
{
    ComplexityNode last = Node_create(v);
    Node_addEdge(v,Visit_getLast(v), last);
    Visit_set_last(v,last);
}

void Visit_expression(Visit* v)
{
    switch(v->Kind){
        case Op_CompoundStmt: Visit_flow(v); return;

        case Op_IfStmt:
        if(v->IfConditionVisited){
            Visit_flow(v);
        }else{
            v->IfConditionVisited = 1;
        }
        return;

        case Op_ForStmt:
        v->ForVisitedExpressions++;
        if(v->ForVisitedExpressions>4){//the body is an expression
            Visit_flow(v);
            //this is the  only statement of the for block
        }
        return;

        case Op_WhileStmt:
        if(v->WhileConditionVisited){
            Visit_flow(v);
            //this is the  only statement of the for block
        }else{
            v->WhileConditionVisited = 1;
        }
        return;

        default: return;
    }
}

Visit* Visit_create(ComplexityParameters* parameters)
{
    Visit* new = malloc(sizeof(Visit));

    new->Parameters = parameters;
    new->Previous = NULL;
    new->node_number=malloc(sizeof(unsigned int));
    * (new->node_number) = 0;
    new->Entry=Node_create(new);
    new->ReturnNode=Node_create(new);
    new->Kind=Op_Function;

    new->IfTrueEnd = NULL;
    new->IfFalseEnd = NULL;
    new->IfTrueBranchVisited = 0;
    new->IfFalseBranchVisited = 0;
    new->IfConditionVisited = 0;
    new->CondPrevious = NULL;
    new->CompoundLast = NULL;
    new->BreakNode = NULL;
    new->CompoundCaseOrigin = NULL;
    new->CycleLastNode = NULL;
    new->ForVisitedExpressions = 0;
    new->ContinueNode = NULL;
    new->WhileConditionVisited=0;

    return new;
}


void Visit_dispose(Visit* v)
{
    if(v->Previous==NULL){
        free(v->node_number);
    }
    free(v);
}



//// ---------------------- EXTERNAL INTERFACE -----------------////

void Visit_case_default(Visit* v)
{
    ComplexityNode new = Node_create(v);
    Node_addEdge(v,v->CompoundCaseOrigin,new);
    Node_addEdge(v,Visit_getLast(v),new);
    Visit_set_last(v,new);
}

void Visit_case(Visit* v)
{
    print_flow_case();
    Visit_case_default(v);
}

void Visit_default(Visit* v)
{
    print_flow_default();
    Visit_case_default(v);
}

void Visit_break(Visit* v)
{
    Visit_break_flow(v,v->BreakNode);
}

void Visit_continue(Visit* v)
{
    Visit_break_flow(v,v->ContinueNode);
}

void Visit_return(Visit* v)
{
    Visit_break_flow(v,v->ReturnNode);
}

void Visit_goto(Visit* v, char* label){
    ComplexityNode gone_node=Node_create(v);
    Visit_break_flow(v,gone_node);
    v->Parameters->GotoLabel(v->Parameters->NodeClientData,gone_node,label);
}

void Visit_label(Visit* v, char* name)
{
    Visit_expression(v);
    ComplexityNode last = Visit_getLast(v);
    v->Parameters->SetLabel(v->Parameters->NodeClientData,last,name);
}