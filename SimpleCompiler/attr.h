/**********************************************
        CS415  Project 2
        Spring  2015
        Student Version
**********************************************/

#ifndef ATTR_H
#define ATTR_H


typedef union {int num; char *str;} tokentype; // "token" in parse.y

typedef enum type_expression {TYPE_INT=0, TYPE_BOOL, TYPE_ERROR} Type_Expression;

typedef struct _Node {
    char *name;
    struct _Node *next;
} Node;

typedef struct {
        Type_Expression type;
        int targetRegister;
        int isArray;
        Node *head;
} regInfo; // "targetReg" in parse.y


Node *push(Node *head, Node *new_node);
void destroy(Node *head);

#endif


  
