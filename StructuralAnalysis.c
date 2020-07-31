#include "StructuralAnalysis.h"
#include <stdlib.h>
#include <string.h>
struct IndexLinkedList{
    unsigned int Index;
    struct IndexLinkedList* Previous;
};

struct Node {
    char* Name;
    struct IndexLinkedList* LastIndex;
    struct NodeLinkedList* LastChild;
};

struct NodeLinkedList {
    struct Node* Node;
    struct NodeLinkedList* Previous;
};

typedef struct Node Node;
typedef struct NodeLinkedList NodeLinkedList;
typedef struct IndexLinkedList IndexLinkedList;

char* mallocopy_sa(const char* copied)
{
    char* ret = malloc( sizeof(char) * (strlen(copied) + 1) );
    strcpy(ret,copied);
    return ret;
}


Node* Node_addOrGetChild(Node* n, const char* name)
{
    NodeLinkedList* current = n->LastChild;
    while(current!=NULL && strcmp(current->Node->Name,name)!=0){
        current=current->Previous;
    }
    if(current==NULL){
        // new list
        NodeLinkedList* new_list=malloc(sizeof(NodeLinkedList));
        new_list->Previous = n->LastChild;
        n->LastChild = new_list;
        //new Node for the list
        Node* new_node = malloc(sizeof(Node));
        new_node->LastChild=NULL;
        new_node->LastIndex=NULL;
        new_node->Name=mallocopy_sa(name);
        //assign new Node to the list
        new_list->Node=new_node;
        return new_node;
    }else{
        return current->Node;
    }
}

void Node_addFunction(Node* n, unsigned int f)
{
    IndexLinkedList* new_list = malloc(sizeof(IndexLinkedList));
    new_list->Previous = n->LastIndex;
    new_list->Index = f;
    n->LastIndex = new_list;
}


void visitor(const char * name, const char * usr, unsigned int index, DefinitionData* def_data, void* data)
{
    if(def_data!=NULL)
    {
        Node* current = (Node*) data;
        char* path=mallocopy_sa(def_data->Filename);
        const char* token = strtok(path,"/");
        while(token != NULL){
            current = Node_addOrGetChild(current,token);
            token = strtok(NULL,"/");
        }
        Node_addFunction(current, index);
        free(path);
    }
}

StructuralAnalysis create_StructuralAnalysis(GatheredCallgraph c)
{
    Node* ret = malloc(sizeof(Node));
    GatheredCallgraph_visitFunctions(c,visitor,ret);
    return (StructuralAnalysis) ret;
}

unsigned int functions_array(Node* n, unsigned int** functions)
{
    {
        unsigned int size = 0;
        IndexLinkedList* current = n->LastIndex;
        while(current!=NULL){
            size += 1;
            current=current->Previous;
        }
        (*functions)=malloc(sizeof(unsigned int)*size);
    }
    {
        IndexLinkedList* current = n->LastIndex;
        unsigned int i = 0;
        while(current!=NULL){
            (*functions)[i] = current->Index;
            current=current->Previous; i++;
        }
        return i;
    }
}

void visit_node(Node* n, CommunityVisitor visitor, void* data)
{
    unsigned int* functions;
    unsigned int  functions_size = functions_array(n, &functions);
    visitor((Community) n, n->Name, functions, functions_size, data);
    free(functions);
}

void StructuralAnalysis_visitRoot(StructuralAnalysis structural_analysis, CommunityVisitor visitor, void* data)
{
    Node* root = (Node*) structural_analysis;
    visit_node(root,visitor,data);
}

void Community_visitChildren(Community c, CommunityVisitor visitor, void* data)
{
    Node* parent = (Node*) c;
    NodeLinkedList* current = parent->LastChild;
    while(current!=NULL){
        visit_node(current->Node,visitor,data);
        current=current->Previous;
    }
}


void dispose_Node(Node* n)
{

}
void dispose_StructuralAnalysis(StructuralAnalysis sa)
{
    Node* n = (Node*) sa;
    dispose_Node(n);
}