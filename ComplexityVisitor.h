#include <clang-c/Index.h>

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


void Visit_enter(Visit* v, enum CXCursorKind k);
void Visit_exit(Visit* v);
void Visit_case(Visit* v);
void Visit_default(Visit* v);
void Visit_break(Visit* v);
void Visit_continue(Visit* v);
void Visit_return(Visit* v);
void Visit_label(Visit* v, char* name);
void Visit_goto(Visit* v, char* label);
void Visit_expression(Visit* v);
Visit* Visit_create();
unsigned int Visit_get_complexity(Visit* v);
void Visit_dispose(Visit* v);