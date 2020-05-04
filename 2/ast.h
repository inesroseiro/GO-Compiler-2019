#include <stdlib.h>
#include <stdio.h>
#include <string.h>

typedef struct node{
    struct node* brother;
    struct node* child;
    char *name; //label of the node
    char *value; //value of the node, if it has any
} node;

node *create_node(char *name, char *value);
void add_child(node *dad, node * child);
void add_brother(node *aux_brother, node *new_brother);
void printAST(node *current, int n);
void check_brothers(node *aux, char *val);
int count_brothers(node *start_node);



