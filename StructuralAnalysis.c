#include "StructuralAnalysis.h"
#include <stdlib.h>
#include <string.h>
struct IndexLinkedList{
    unsigned int Index;
    struct IndexLinkedList* Next;
};

struct Node {
    char* Name;
    struct IndexLinkedList* Functions;
    struct NodeLinkedList* Children
};

struct NodeLinkedList {
    struct Node;
    struct NodeLinkedList* Next;
};

typedef struct Node Node;
typedef struct NodeLinkedList NodeLinkedList;
typedef struct IndexLinkedList IndexLinkedList;


Node* Node_addOrGetChild(Node* n, const char* token)
{

}

Node* Node_addFunction(Node* n, unsigned int f)
{

}

void visitor(const char * name, const char * usr, unsigned int index, DefinitionData* def_data, void* data)
{
    Node* current = (Node*) data;
    const char* path=def_data->Filename;
    const char* token = strtok(path,"/");
    while(token != NULL){
        current = Node_addOrGetChild(current,token);
        token = strtok(NULL,"/");
    }
    Node_addFunction(current, index);
}

StructuralAnalysis create_StructuralAnalysis(GatheredCallgraph c)
{
    Node* ret = malloc(sizeof(Node));
    GatheredCallgraph_visitFunctions(c,visitor,ret);
    return (StructuralAnalysis) ret;
}

void dispose_Node(Node* n)
{

}
void dispose_StructuralAnalysis(StructuralAnalysis sa)
{
    Node* n = (Node*) sa;
    dispose_Node(n);
}