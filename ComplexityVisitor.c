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
    }
}

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
    ret->NodeNumber = v->node_number;
    ret->Label = NULL;
    ret->GotoLabel = NULL;
    v->node_number++;
    return ret;
}


void Visit_set_last(Visit* v, Node* last)
{
    if(v->OpStack==NULL){
        Node_addEdge(last,v->ReturnNode);
        return;
    }

    if(v->OpStack->Kind == Op_CompoundStmt){
        v->OpStack->CompoundLast = last;
    } else if(v->OpStack->Kind == Op_IfStmt){
        if(v->OpStack->IfTrueEnd==NULL && !v->OpStack->IfTrueBranchVisited){
            v->OpStack->IfTrueEnd = last;
            v->OpStack->IfTrueBranchVisited = 1;
        }else if(v->OpStack->IfFalseEnd==NULL && !v->OpStack->IfFalseBranchVisited){
            v->OpStack->IfFalseEnd = last;
            v->OpStack->IfFalseBranchVisited=1;
        }
    } else if(v->OpStack->Kind==Op_SwitchStmt){
        Node_addEdge(last,v->OpStack->BreakNode);
    } else if( v->OpStack->Kind==Op_ForStmt || v->OpStack->Kind==Op_WhileStmt ){
        Node*  intermediate = Node_create(v);
        Node_addEdge(last,intermediate);
        v->OpStack->CycleLastNode = intermediate;
    }
}


void Visit_push(Visit* v){
    Operation* new = malloc(sizeof(Operation));
    new->Kind=0;
    
    new->IfTrueEnd = NULL;
    new->IfFalseEnd = NULL;
    new->IfTrueBranchVisited = 0;
    new->IfFalseBranchVisited = 0;
    new->IfConditionVisited = 0;
    new->CondPrevious = NULL;

    if(v->OpStack!=NULL){
        new->CompoundLast = v->OpStack->CompoundLast;
    }else{
        new->CompoundLast = NULL;
    }
    
    if(v->OpStack != NULL){
        new->BreakNode = v->OpStack->BreakNode;
    }else{
        new->BreakNode = NULL;
    }
    new->CompoundCaseOrigin = NULL;
    new->CycleLastNode = NULL;
    new->ForVisitedExpressions = 0;

    if(v->OpStack != NULL){
        new->ContinueNode = v->OpStack->ContinueNode;
    }else{
        new->ContinueNode = NULL;
    }

    new->WhileConditionVisited=0;

    new->Tail = v->OpStack;
    v->OpStack = new;
}

Node* Visit_getLast(Visit* v)
{
    if(v->OpStack==NULL){
        return v->Entry;
    }else{
        switch (v->OpStack->Kind)
        {
        case Op_CompoundStmt:
            return v->OpStack->CompoundLast;
        case Op_IfStmt:
        case Op_SwitchStmt:
        case Op_ForStmt:
        case Op_WhileStmt:
            return v->OpStack->CondPrevious;
        default:
            return NULL;
        }
    }
}

void Visit_break_flow(Visit* v, Node* target)
{    
    Operation* op = v->OpStack;
    Node_addEdge(Visit_getLast(v), target);
    Visit_set_last(v,Node_create(v));
}
        
void Visit_enter(Visit* v, enum OperationKind k)
{
    print_flow_enter(spell_kind(k));

    if(k == Op_CompoundStmt){
        Node* last = Visit_getLast(v);
        Visit_push(v);
        v->OpStack->Kind = Op_CompoundStmt;

        Operation* previous = v->OpStack->Tail;
        v->OpStack->CompoundLast = last;
        if(previous!=NULL && previous->Kind==Op_SwitchStmt){
            v->OpStack->CompoundCaseOrigin = last;
            Visit_set_last(v,Node_create(v));
        }
    }
    if(k==Op_IfStmt){
        Node* last = Visit_getLast(v);
        Visit_push(v);
        v->OpStack->Kind = Op_IfStmt;
        v->OpStack->CondPrevious = last;
    }
    if(k==Op_SwitchStmt){
        Node* last = Visit_getLast(v);
        Visit_push(v);
        v->OpStack->Kind = Op_SwitchStmt;
        v->OpStack->CondPrevious = last;
        v->OpStack->BreakNode = Node_create(v);
    }
    if( k == Op_ForStmt || k == Op_WhileStmt ){
        Node* last = Visit_getLast(v);
        Visit_push(v);
        v->OpStack->Kind = k;
        v->OpStack->CondPrevious = last;
        v->OpStack->ContinueNode = last;
        v->OpStack->BreakNode = Node_create(v);
    }
}

void Visit_pop(Visit* v){
    Operation* old = v->OpStack;
    v->OpStack = v->OpStack->Tail;
    free(old);
}


void Visit_exit(Visit* v)
{
    if(v->OpStack != NULL){

        print_flow_exit(spell_kind(v->OpStack->Kind));

        if(v->OpStack->Kind==Op_CompoundStmt){
            Node* last = v->OpStack->CompoundLast;
            Visit_pop(v);
            Visit_set_last(v, last);
            
        }else if(v->OpStack->Kind == Op_IfStmt){
            Node* final = Node_create(v);

            if(v->OpStack->IfTrueEnd!=NULL){
                Node_addEdge(v->OpStack->IfTrueEnd,final);
            }

            Node* falseEnd = v->OpStack->IfFalseEnd;
            if(falseEnd==NULL){
                if(!v->OpStack->IfFalseBranchVisited){
                    Node_addEdge(v->OpStack->CondPrevious,final);
                }
            }else{
                Node_addEdge(falseEnd,final);
            }
            
            Visit_pop(v);
            Visit_set_last(v,final);
        }else if(v->OpStack->Kind == Op_ForStmt || v->OpStack->Kind == Op_WhileStmt){
            Node_addEdge(v->OpStack->CycleLastNode, v->OpStack->CondPrevious);
            Node_addEdge(v->OpStack->CondPrevious, v->OpStack->BreakNode);
            Node* final = v->OpStack->BreakNode;
            Visit_pop(v);
            Visit_set_last(v,final);
        }else if(v->OpStack->Kind == Op_SwitchStmt) {
            Node* last = v->OpStack->BreakNode;
            Visit_pop(v);
            Visit_set_last(v,last);
        }else {
            Visit_pop(v);
        }
    }
}




void Visit_flow_to(Visit* v, Node* next)
{
    Node_addEdge(Visit_getLast(v), next);
}

void Visit_flow(Visit* v)
{
    Node* last = Node_create(v);
    Node_addEdge(Visit_getLast(v), last);
    Visit_set_last(v,last);
}

void Visit_expression(Visit* v)
{   
    if(v->OpStack->Kind == Op_CompoundStmt){
        Visit_flow(v);
    } else if(v->OpStack->Kind == Op_IfStmt){
        if(v->OpStack->IfConditionVisited){
            Visit_flow(v);
        }else{
            v->OpStack->IfConditionVisited = 1;
        }
    } else if(v->OpStack->Kind==Op_ForStmt){
        v->OpStack->ForVisitedExpressions++;
        if(v->OpStack->ForVisitedExpressions>4){//the body is an expression
            Visit_flow(v);
            //this is the  only statement of the for block
        }
    }else if (v->OpStack->Kind==Op_WhileStmt){
        if(v->OpStack->WhileConditionVisited){
            Visit_flow(v);
            //this is the  only statement of the for block
        }else{
            v->OpStack->WhileConditionVisited = 1;
        }
    }
}

void Visit_case_default(Visit* v)
{
    Operation* op = v->OpStack;
    Node* new = Node_create(v);
    Node_addEdge(op->CompoundCaseOrigin,new);
    Node_addEdge(op->CompoundLast,new);
    op->CompoundLast = new;
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

Visit* Visit_create()
{
    Visit* ret = malloc(sizeof(Visit));
    ret->OpStack = NULL;
    ret->node_number = 0;
    ret->Entry = Node_create(ret);
    ret->ReturnNode = Node_create(ret);
    return ret;
}


void Visit_dispose(Visit* v)
{
    free(v);
}



//// ---------------------- EXTERNAL INTERFACE -----------------////



void Visit_break(Visit* v)
{
    Visit_break_flow(v,v->OpStack->BreakNode);
}

void Visit_continue(Visit* v)
{
    Visit_break_flow(v,v->OpStack->ContinueNode);
}

void Visit_return(Visit* v)
{
    Visit_break_flow(v,v->ReturnNode);
}

void Visit_goto(Visit* v, char* label){
    Node* gone_node=Node_create(v);
    Visit_break_flow(v,gone_node);
    gone_node->GotoLabel=label;
    print_flow_goto(gone_node->NodeNumber,gone_node->GotoLabel);
}

void Visit_label(Visit* v, char* name)
{
    Visit_expression(v);
    Node* last = Visit_getLast(v);
    last->Label = name;//assume we are in a CompoundStatement
    print_flow_labeled(last->NodeNumber, name);
}

unsigned int Visit_get_complexity(Visit* v)
{
    return 2;
}