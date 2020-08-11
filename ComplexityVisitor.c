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

    new->CompoundCaseSeen = 0;
    new->CompoundDefaultSeen = 0;
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

unsigned int Visit_canReceiveExpressions(Visit* v)
{
    //not compound statement
    if(v->OpStack==NULL) return 1;
    if(v->OpStack->Kind != Op_CompoundStmt) return 1;

    //compound statement doesn't come after switch
    if(v->OpStack->Tail==NULL) return 1;
    if(v->OpStack->Tail->Kind!=Op_SwitchStmt) return 1;

    //compound statement that comes after switch fulfills at least one of
    if(v->OpStack->CompoundCaseSeen) return 1;
    if(v->OpStack->CompoundDefaultSeen) return 1;

    //none of the conditions are met
    return 0;
}
        
void Visit_enter(Visit* v, enum OperationKind k)
{
    if(!Visit_canReceiveExpressions(v)) return;

    print_flow_enter(spell_kind(k));

    if(k == Op_CompoundStmt){
        Node* last = Visit_getLast(v);
        Visit_push(v);
        v->OpStack->Kind = Op_CompoundStmt;

        Operation* previous = v->OpStack->Tail;
        if(previous!=NULL && previous->Kind==Op_SwitchStmt){
            v->OpStack->CompoundCaseOrigin = last;
        }else{
            v->OpStack->CompoundLast = last;
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

void Visit_addLooseEnd(Visit* v, Node* end)
{
    print_flow_end(end->NodeNumber);
    if(v->OpStack==NULL){
        Node_addEdge(end,v->ReturnNode);
    }else{
        if(v->OpStack->Kind==Op_CompoundStmt){
            v->OpStack->CompoundLast = end;
        }
        if(v->OpStack->Kind==Op_IfStmt){
            if(!v->OpStack->IfTrueBranchVisited){
                v->OpStack->IfTrueEnd = end;
                v->OpStack->IfTrueBranchVisited = 1;
            }else{
                v->OpStack->IfFalseEnd = end;
                v->OpStack->IfFalseBranchVisited = 1;
            }
        }
        if(v->OpStack->Kind==Op_SwitchStmt){
            Node_addEdge(end,v->OpStack->BreakNode);
        }
        if( v->OpStack->Kind==Op_ForStmt || v->OpStack->Kind==Op_WhileStmt ){
            Node*  intermediate = Node_create(v);
            Node_addEdge(end,intermediate);
            v->OpStack->CycleLastNode = intermediate;
        }
    }
    
}

void Visit_exit(Visit* v)
{
    if(v->OpStack != NULL){

        print_flow_exit(spell_kind(v->OpStack->Kind));

        if(v->OpStack->Kind==Op_CompoundStmt){
            Node* last = v->OpStack->CompoundLast;
            Visit_pop(v);
            if (last!=NULL) Visit_addLooseEnd(v, last);
            
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
            Visit_addLooseEnd(v,final);
        }else if(v->OpStack->Kind == Op_ForStmt || v->OpStack->Kind == Op_WhileStmt){
            Node_addEdge(v->OpStack->CycleLastNode, v->OpStack->CondPrevious);
            Node_addEdge(v->OpStack->CondPrevious, v->OpStack->BreakNode);
            Node* final = v->OpStack->BreakNode;
            Visit_pop(v);
            Visit_addLooseEnd(v,final);
        }else{
            Visit_pop(v);
        }
        
    }
}

void Visit_expression(Visit* v)
{
    if(!Visit_canReceiveExpressions(v)) return;
    
    if(v->OpStack->Kind == Op_CompoundStmt){
        Node* new = Node_create(v);
        if(v->OpStack->CompoundLast!=NULL){
            Node_addEdge(v->OpStack->CompoundLast, new);
        }
        v->OpStack->CompoundLast = new;
    } else if(v->OpStack->Kind == Op_IfStmt){
        if(v->OpStack->IfConditionVisited){
            Node* new = Node_create(v);
            Node_addEdge(v->OpStack->CondPrevious, new);
            if(v->OpStack->IfTrueEnd==NULL && !v->OpStack->IfTrueBranchVisited){
                v->OpStack->IfTrueEnd = new;
                v->OpStack->IfTrueBranchVisited = 1;
            }else if(v->OpStack->IfFalseEnd==NULL && !v->OpStack->IfFalseBranchVisited){
                v->OpStack->IfFalseEnd = new;
                v->OpStack->IfFalseBranchVisited=1;
            }
        }else{
            v->OpStack->IfConditionVisited = 1;
        }
    } else if(v->OpStack->Kind==Op_ForStmt){
        v->OpStack->ForVisitedExpressions++;
        if(v->OpStack->ForVisitedExpressions>4){//the body is an expression
            Node* new = Node_create(v);
            Node_addEdge(v->OpStack->CondPrevious, new);
            Visit_addLooseEnd(v,new);//this is the  only statement of the for block
        }
    }else if (v->OpStack->Kind==Op_WhileStmt){
        if(v->OpStack->WhileConditionVisited){
            Node* new = Node_create(v);
            Node_addEdge(v->OpStack->CondPrevious, new);
            Visit_addLooseEnd(v,new);//this is the  only statement of the for block
        }else{
            v->OpStack->WhileConditionVisited = 1;
        }
    }
}

void Operation_visit_case_default(Visit* v)
{
    Operation* op=v->OpStack;
    if(op->CompoundLast==NULL){
        /*
        the first case of the block OR the first case after a break.
        */
        Node* new = Node_create(v);
        Node_addEdge(op->CompoundCaseOrigin,new);
        op->CompoundLast = new;
    }else if(!Node_edgeExists(op->CompoundCaseOrigin,op->CompoundLast)){
        /*
        the case that comes after a statement or expression that is not a break
        case 0:
        q+=1
        case 1: <--
        case 2:
        case 3:
        */
        Node* new = Node_create(v);
        Node_addEdge(op->CompoundCaseOrigin,new);
        Node_addEdge(op->CompoundLast,new);
        op->CompoundLast = new;
    }
    /*
    we ignore the case after a case
    case 2:
    case 3: <---
    case 4:
    */
}

void Visit_case(Visit* v)
{
    print_flow_case();
    Operation* op = v->OpStack;
    op->CompoundCaseSeen = 1;
    Operation_visit_case_default(v);
}

void Visit_default(Visit* v)
{
    print_flow_default();
    Operation* op = v->OpStack;
    op->CompoundDefaultSeen = 1;
    Operation_visit_case_default(v);
}

void break_flow(Visit* v){
    Operation* op = v->OpStack;
    op->CompoundLast=Node_create(v);

    while(op != NULL && op->Kind!=Op_IfStmt){
        op = op->Tail;
    }
    if(op==NULL) return;

    if(op->IfTrueBranchVisited){
        op->IfFalseBranchVisited=1;
    }else{
        op->IfTrueBranchVisited=1;
    }
}

void Visit_break(Visit* v)
{
    Operation* op = v->OpStack;
    Node_addEdge(op->CompoundLast, op->BreakNode);
    break_flow(v);
    
}

void Visit_continue(Visit* v)
{
    Operation* op = v->OpStack;
    Node_addEdge(op->CompoundLast, op->ContinueNode);
    break_flow(v);
}

void Visit_return(Visit* v)
{
    Operation* op = v->OpStack;
    Node_addEdge(Visit_getLast(v),v->ReturnNode);
    Node* last_node=Node_create(v);
    Visit_addLooseEnd(v,last_node);
    break_flow(v);
}
void Visit_goto(Visit* v, char* label){
    Operation* op = v->OpStack;
    Node* gone_node=Node_create(v);
    Node_addEdge(Visit_getLast(v),gone_node);
    Visit_addLooseEnd(v,gone_node);
    gone_node->GotoLabel=label;
    print_flow_goto(gone_node->NodeNumber,gone_node->GotoLabel);
    break_flow(v);
}

void Visit_label(Visit* v, char* name)
{
    Visit_expression(v);
    v->OpStack->CompoundLast->Label = name;//assume we are in a CompoundStatement
    print_flow_labeled(v->OpStack->CompoundLast->NodeNumber, v->OpStack->CompoundLast->Label);
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

unsigned int Visit_get_complexity(Visit* v)
{
    return 2;
}

void Visit_dispose(Visit* v)
{
    free(v);
}