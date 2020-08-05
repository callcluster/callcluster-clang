#include "complexity_analysis.h"
#include <stdio.h>
#include <stdlib.h>

struct Node;

struct NodeList {
    struct Node* Head;
    struct NodeList* Tail;
};

struct Node {
    struct NodeList* Next;
    unsigned int NodeNumber;
};
typedef struct Node Node;
typedef struct NodeList NodeList;

unsigned int node_number=0;

Node* Node_create()
{
    Node* ret = malloc(sizeof(Node));
    ret->Next=NULL;
    ret->NodeNumber=node_number;
    node_number++;
    return ret;
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
    printf("from %d to %d\n",from->NodeNumber,to->NodeNumber);
    if(Node_edgeExists(from,to)) return;

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
    Node* IfFalseEnd;//Final Node in the truthful block of an if statement
    unsigned int IfConditionVisited;//boolean, true if the condition for the if statement was visited
    struct Node* BreakNode;//Node that will only not be NULL when the current operation can be broken
    unsigned int CompoundCaseSeen;//Node that indicates wether the current Compound has seen a Case statement
    unsigned int CompoundDefaultSeen;//Node that indicates wether the current Compound has seen a Case statement
    Node* CompoundCaseOrigin;//CompoundLast will take this value when a case is seen
};
typedef struct Operation Operation;

typedef struct {
    Operation* OpStack;
    Node* Entry;
    Node* Exit;
} Visit;



void Visit_push(Visit* v){
    Operation* new = malloc(sizeof(Operation));
    new->Kind=0;
    new->CompoundLast = NULL;
    new->CondPrevious = NULL;
    new->IfTrueEnd = NULL;
    new->IfFalseEnd = NULL;
    new->IfConditionVisited = 0;
    new->BreakNode = NULL;
    if(v->OpStack != NULL){
        new->BreakNode = v->OpStack->BreakNode;
    }
    new->CompoundCaseSeen = 0;
    new->CompoundDefaultSeen = 0;
    new->CompoundCaseOrigin = NULL;
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
    printf("enter: %s\n",clang_getCString(s));
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
        v->OpStack->BreakNode = Node_create();
    }
    
}

void Visit_pop(Visit* v){
    Operation* old = v->OpStack;
    v->OpStack = v->OpStack->Tail;
    free(old);
}

void Visit_addLooseEnd(Visit* v, Node* end)
{
    printf("ADDING LOOSE END:%d\n", end->NodeNumber);
    if(v->OpStack!=NULL){
        if(v->OpStack->Kind==CXCursor_CompoundStmt){
            v->OpStack->CompoundLast = end;
        }
        if(v->OpStack->Kind==CXCursor_IfStmt){
            if(v->OpStack->IfTrueEnd==NULL){
                v->OpStack->IfTrueEnd = end;
            }else{
                v->OpStack->IfFalseEnd = end;
            }
        }
        if(v->OpStack->Kind==CXCursor_SwitchStmt){
            Node_addEdge(end,v->OpStack->BreakNode);
        }

    }
    
}

void Visit_exit(Visit* v)
{
    if(v->OpStack != NULL){

        CXString s = clang_getCursorKindSpelling(v->OpStack->Kind);
        printf("exit: %s\n",clang_getCString(s));
        clang_disposeString(s);

        if(v->OpStack->Kind==CXCursor_CompoundStmt){
            Node* last = v->OpStack->CompoundLast;
            Visit_pop(v);
            Visit_addLooseEnd(v, last);
            
        }else if(v->OpStack->Kind == CXCursor_IfStmt){
            Node* final = Node_create();

            Node_addEdge(v->OpStack->IfTrueEnd,final);

            Node* falseEnd = v->OpStack->IfFalseEnd;
            if(falseEnd==NULL){
                Node_addEdge(v->OpStack->CondPrevious,final);
            }else{
                Node_addEdge(falseEnd,final);
            }
            
            Visit_pop(v);
            Visit_addLooseEnd(v,final);
        }else if(v->OpStack->Kind == CXCursor_CaseStmt){
            Node* final = v->OpStack->BreakNode;
            Visit_pop(v);
            Visit_addLooseEnd(v,final);
        } else {
            Visit_pop(v);
        }
        
    }
}

void Visit_expression(Visit* v)
{
    if(!Visit_canReceiveExpressions(v)) return;

    printf("e\n");
    if(v->OpStack->Kind == CXCursor_CompoundStmt){
        Node* new = Node_create();
        Node_addEdge(v->OpStack->CompoundLast, new);
        v->OpStack->CompoundLast = new;
    } else if(v->OpStack->Kind == CXCursor_IfStmt){
        if(v->OpStack->IfConditionVisited){
            Node* new = Node_create();
            Node_addEdge(v->OpStack->CondPrevious, new);
            if(v->OpStack->IfTrueEnd==NULL){
                v->OpStack->IfTrueEnd = new;
            }else{
                v->OpStack->IfFalseEnd = new;
            }
        }else{
            v->OpStack->IfConditionVisited = 1;
        }
    }
}

void Operation_visit_case_default(Operation* op)
{
    if(op->CompoundLast==NULL){
        /*
        the first case of the block OR the first case after a break.
        */
        Node* new = Node_create();
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
        Node* new = Node_create();
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
    printf("Case:");
    Operation* op = v->OpStack;
    op->CompoundCaseSeen = 1;
    Operation_visit_case_default(op);
}

void Visit_default(Visit* v)
{
    printf("Default:");
    Operation* op = v->OpStack;
    op->CompoundDefaultSeen = 1;
    Operation_visit_case_default(op);
}

void Visit_break(Visit* v)//PENSADO SOLAMENTE PARA CASE
{
    Operation* op = v->OpStack;
    Node_addEdge(op->CompoundLast, op->BreakNode);
    op->CompoundLast=NULL;
}

void Visit_return(Visit* v)
{
    printf("return:");
}

enum CXChildVisitResult general_visitor (CXCursor cursor, CXCursor parent, CXClientData client_data)
{
    Visit* visit = (Visit*) client_data;
    enum CXCursorKind kind = clang_getCursorKind(cursor);
    switch(kind){

        case CXCursor_CompoundStmt:
        case CXCursor_IfStmt:
        case CXCursor_SwitchStmt:
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
        clang_visitChildren(cursor, general_visitor, (CXClientData) visit);
        return CXChildVisit_Continue;

        case CXCursor_ReturnStmt:
        Visit_return(visit);
        clang_visitChildren(cursor, general_visitor, (CXClientData) visit);
        return CXChildVisit_Continue;

        default:
        if(clang_isStatement(clang_getCursorKind(parent))) Visit_expression(visit);
        return CXChildVisit_Recurse;
    }
}

Visit* Visit_create()
{
    Visit* ret = malloc(sizeof(Visit));
    ret->OpStack = NULL;
    ret->Entry = Node_create();
    ret->Exit = Node_create();
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
    printf("-----\n");
    Visit* v = Visit_create();
    clang_visitChildren(c, general_visitor, (CXClientData) v);
    unsigned int complexity = Visit_get_complexity(v);
    Visit_dispose(v);
    return complexity;
}