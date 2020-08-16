#ifndef COMPLEXITY_VISITOR_H
#define COMPLEXITY_VISITOR_H
enum OperationKind{
    Op_CompoundStmt,
    Op_IfStmt,
    Op_SwitchStmt,
    Op_ForStmt,
    Op_WhileStmt,
    Op_Function
};


typedef void* ComplexityNode;
typedef void* NodeClientDataType;

typedef ComplexityNode (*CreateNodeFunType)(NodeClientDataType client_data);
typedef void (*CreateEdgeFunType)(NodeClientDataType client_data, ComplexityNode from, ComplexityNode to);
typedef void (*LabelFunType)(NodeClientDataType client_data, ComplexityNode node, const char* label);

struct ComplexityParameters {
    CreateNodeFunType CreateNode;
    CreateEdgeFunType AddEdge;
    LabelFunType SetLabel;
    LabelFunType GotoLabel;
    NodeClientDataType NodeClientData;
};

typedef struct ComplexityParameters ComplexityParameters;
/*
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
*/
typedef struct Visit Visit;

struct Visit {
    Visit* Previous;
    ComplexityNode* Entry;
    ComplexityNode* ReturnNode;
    unsigned int * node_number;

    enum OperationKind Kind;

    ComplexityNode CompoundLast;//constantly moving end of a compound statement
    ComplexityNode CondPrevious;//Node previous to an if or switch statement 
    ComplexityNode IfTrueEnd;//Final Node in the truthful block of an if statement
    unsigned int IfTrueBranchVisited;//wether the true branch was visited
    ComplexityNode IfFalseEnd;//Final Node in the truthful block of an if statement
    unsigned int IfFalseBranchVisited;//wether the true branch was visited
    unsigned int IfConditionVisited;//boolean, true if the condition for the if statement was visited
    ComplexityNode BreakNode;//Node that will only not be NULL when the current operation can be broken
    ComplexityNode CompoundCaseOrigin;//CompoundLast will take this value when a case is seen
    ComplexityNode CycleLastNode;//CycleLastNode represents the last executable node in a cycle
    unsigned int ForVisitedExpressions;//counts expressions within a for statement
    ComplexityNode ContinueNode;//used for cycles, it's CondPrevious but falls through
    unsigned int WhileConditionVisited;//used for the while cycle 0 means the check expression for this while loop was not seen

    ComplexityParameters* Parameters; 
};


Visit* Visit_enter(Visit* v, enum OperationKind k);
void Visit_exit(Visit* v);
void Visit_case(Visit* v);
void Visit_default(Visit* v);
void Visit_break(Visit* v);
void Visit_continue(Visit* v);
void Visit_return(Visit* v);
void Visit_label(Visit* v, char* name);
void Visit_goto(Visit* v, char* label);
void Visit_expression(Visit* v);
Visit* Visit_create(ComplexityParameters*);
void Visit_dispose(Visit* v);

#endif