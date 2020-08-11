#include "complexity_analysis.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parameters.h"
struct Node;

struct NodeList {
    struct Node* Head;
    struct NodeList* Tail;
};

struct Node {
    struct NodeList* Next;
    unsigned int NodeNumber;
    char* Label;
    char* GotoLabel;
};
typedef struct Node Node;
typedef struct NodeList NodeList;

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

struct Operation {
    enum CXCursorKind Kind;
    struct Operation* Tail;
    Node* CompoundLast;//constantly moving end of a compound statement
    Node* CondPrevious;//Node previous to an if or switch statement 
    Node* IfTrueEnd;//Final Node in the truthful block of an if statement
    unsigned int IfTrueBranchVisited;//wether the true branch was visited
    Node* IfFalseEnd;//Final Node in the truthful block of an if statement
    unsigned int IfFalseBranchVisited;//wether the true branch was visited
    unsigned int IfConditionVisited;//boolean, true if the condition for the if statement was visited
    struct Node* BreakNode;//Node that will only not be NULL when the current operation can be broken
    unsigned int CompoundCaseSeen;//Node that indicates wether the current Compound has seen a Case statement
    unsigned int CompoundDefaultSeen;//Node that indicates wether the current Compound has seen a Case statement
    Node* CompoundCaseOrigin;//CompoundLast will take this value when a case is seen
    Node* CycleLastNode;//CycleLastNode represents the last executable node in a cycle
    unsigned int ForVisitedExpressions;//counts expressions within a for statement
    Node* ContinueNode;//used for cycles, it's CondPrevious but falls through
    unsigned int WhileConditionVisited;//used for the while cycle 0 means the check expression for this while loop was not seen
};
typedef struct Operation Operation;

typedef struct {
    Operation* OpStack;
    Node* Entry;
    Node* ReturnNode;
    unsigned int node_number;
} Visit;

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
        case CXCursor_CompoundStmt:
            return v->OpStack->CompoundLast;
        case CXCursor_IfStmt:
        case CXCursor_SwitchStmt:
        case CXCursor_ForStmt:
        case CXCursor_WhileStmt:
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
    if(v->OpStack->Kind != CXCursor_CompoundStmt) return 1;

    //compound statement doesn't come after switch
    if(v->OpStack->Tail==NULL) return 1;
    if(v->OpStack->Tail->Kind!=CXCursor_SwitchStmt) return 1;

    //compound statement that comes after switch fulfills at least one of
    if(v->OpStack->CompoundCaseSeen) return 1;
    if(v->OpStack->CompoundDefaultSeen) return 1;

    //none of the conditions are met
    return 0;
}
        
void Visit_enter(Visit* v, enum CXCursorKind k)
{
    if(!Visit_canReceiveExpressions(v)) return;

    // print operation type
    CXString s = clang_getCursorKindSpelling(k);
    print_flow_enter(clang_getCString(s));
    clang_disposeString(s);

    if(k == CXCursor_CompoundStmt){
        Node* last = Visit_getLast(v);
        Visit_push(v);
        v->OpStack->Kind = CXCursor_CompoundStmt;

        Operation* previous = v->OpStack->Tail;
        if(previous!=NULL && previous->Kind==CXCursor_SwitchStmt){
            v->OpStack->CompoundCaseOrigin = last;
        }else{
            v->OpStack->CompoundLast = last;
        }
        
    }
    if(k==CXCursor_IfStmt){
        Node* last = Visit_getLast(v);
        Visit_push(v);
        v->OpStack->Kind = CXCursor_IfStmt;
        v->OpStack->CondPrevious = last;
    }
    if(k==CXCursor_SwitchStmt){
        Node* last = Visit_getLast(v);
        Visit_push(v);
        v->OpStack->Kind = CXCursor_SwitchStmt;
        v->OpStack->CondPrevious = last;
        v->OpStack->BreakNode = Node_create(v);
    }
    if( k == CXCursor_ForStmt || k == CXCursor_WhileStmt ){
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
        if(v->OpStack->Kind==CXCursor_CompoundStmt){
            v->OpStack->CompoundLast = end;
        }
        if(v->OpStack->Kind==CXCursor_IfStmt){
            if(!v->OpStack->IfTrueBranchVisited){
                v->OpStack->IfTrueEnd = end;
                v->OpStack->IfTrueBranchVisited = 1;
            }else{
                v->OpStack->IfFalseEnd = end;
                v->OpStack->IfFalseBranchVisited = 1;
            }
        }
        if(v->OpStack->Kind==CXCursor_SwitchStmt){
            Node_addEdge(end,v->OpStack->BreakNode);
        }
        if( v->OpStack->Kind==CXCursor_ForStmt || v->OpStack->Kind==CXCursor_WhileStmt ){
            Node*  intermediate = Node_create(v);
            Node_addEdge(end,intermediate);
            v->OpStack->CycleLastNode = intermediate;
        }
    }
    
}

void Visit_exit(Visit* v)
{
    if(v->OpStack != NULL){

        CXString s = clang_getCursorKindSpelling(v->OpStack->Kind);
        print_flow_exit(clang_getCString(s));
        clang_disposeString(s);

        if(v->OpStack->Kind==CXCursor_CompoundStmt){
            Node* last = v->OpStack->CompoundLast;
            Visit_pop(v);
            if (last!=NULL) Visit_addLooseEnd(v, last);
            
        }else if(v->OpStack->Kind == CXCursor_IfStmt){
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
        }else if(v->OpStack->Kind == CXCursor_CaseStmt){
            Node* final = v->OpStack->BreakNode;
            Visit_pop(v);
            Visit_addLooseEnd(v,final);
        }else if(v->OpStack->Kind == CXCursor_ForStmt || v->OpStack->Kind == CXCursor_WhileStmt){
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
    
    if(v->OpStack->Kind == CXCursor_CompoundStmt){
        Node* new = Node_create(v);
        if(v->OpStack->CompoundLast!=NULL){
            Node_addEdge(v->OpStack->CompoundLast, new);
        }
        v->OpStack->CompoundLast = new;
    } else if(v->OpStack->Kind == CXCursor_IfStmt){
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
    } else if(v->OpStack->Kind==CXCursor_ForStmt){
        v->OpStack->ForVisitedExpressions++;
        if(v->OpStack->ForVisitedExpressions>4){//the body is an expression
            Node* new = Node_create(v);
            Node_addEdge(v->OpStack->CondPrevious, new);
            Visit_addLooseEnd(v,new);//this is the  only statement of the for block
        }
    }else if (v->OpStack->Kind==CXCursor_WhileStmt){
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

void Visit_break(Visit* v)//PENSADO SOLAMENTE PARA CASE
{
    Operation* op = v->OpStack;
    Node_addEdge(op->CompoundLast, op->BreakNode);
    op->CompoundLast=Node_create(v);

    while(op!=NULL && op->Kind!=CXCursor_IfStmt){
        op = op->Tail;
    }
    if(op==NULL) return;

    if(op->IfTrueBranchVisited){
        op->IfFalseBranchVisited=1;
    }else{
        op->IfTrueBranchVisited=1;
    }
}

void Visit_continue(Visit* v)
{
    Operation* op = v->OpStack;
    Node_addEdge(op->CompoundLast, op->ContinueNode);
    op->CompoundLast=Node_create(v);

    while(op != NULL && op->Kind!=CXCursor_IfStmt){
        op = op->Tail;
    }
    if(op==NULL) return;

    if(op->IfTrueBranchVisited){
        op->IfFalseBranchVisited=1;
    }else{
        op->IfTrueBranchVisited=1;
    }
}

void Visit_return(Visit* v)
{
    Operation* op = v->OpStack;
    Node_addEdge(Visit_getLast(v),v->ReturnNode);
    Node* last_node=Node_create(v);
    Visit_addLooseEnd(v,last_node);
    op->CompoundLast=Node_create(v);

    while(op != NULL && op->Kind!=CXCursor_IfStmt){
        op = op->Tail;
    }
    if(op==NULL) return;

    if(op->IfTrueBranchVisited){
        op->IfFalseBranchVisited=1;
    }else{
        op->IfTrueBranchVisited=1;
    }
}
void Visit_goto(Visit* v, char* label){
    Operation* op = v->OpStack;
    Node* gone_node=Node_create(v);
    Node_addEdge(Visit_getLast(v),gone_node);
    Visit_addLooseEnd(v,gone_node);
    gone_node->GotoLabel=label;
    print_flow_goto(gone_node->NodeNumber,gone_node->GotoLabel);
    op->CompoundLast=Node_create(v);

    while(op != NULL && op->Kind!=CXCursor_IfStmt){
        op = op->Tail;
    }
    if(op==NULL) return;

    if(op->IfTrueBranchVisited){
        op->IfFalseBranchVisited=1;
    }else{
        op->IfTrueBranchVisited=1;
    }
}
void Visit_label(Visit* v, char* name)
{
    Visit_expression(v);
    v->OpStack->CompoundLast->Label = name;//assume we are in a CompoundStatement
    print_flow_labeled(v->OpStack->CompoundLast->NodeNumber, v->OpStack->CompoundLast->Label);
}

char* mallocopy_ca(const char* copied)
{
    char* ret = malloc( sizeof(char) * (strlen(copied) + 1) );
    strcpy(ret,copied);
    return ret;
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
        Visit_enter(visit,kind);
        clang_visitChildren(cursor, general_visitor, (CXClientData) visit);
        Visit_exit(visit);
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
            Visit_label(visit,mallocopy_ca(clang_getCString(d)));
            clang_disposeString(d);
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
            Visit_goto(visit,mallocopy_ca(clang_getCString(d)));
            clang_disposeString(d);
        }else{
            if(clang_isStatement(clang_getCursorKind(parent))) Visit_expression(visit);
        }
        return CXChildVisit_Recurse;
    }
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

unsigned int get_complexity(CXCursor c)
{
    Visit* v = Visit_create();
    clang_visitChildren(c, general_visitor, (CXClientData) v);
    unsigned int complexity = Visit_get_complexity(v);
    Visit_dispose(v);
    return complexity;
}