/**********************************************
        CS415  Project 2
        Spring  2018
        Student Version
**********************************************/

#include <stdlib.h>
#include "attr.h" 

Node *push(Node *head, Node *new_node)
{
    Node *ptr = NULL;

    ptr = head;
    if (!ptr) {
        return new_node;
    }
    while (ptr->next) {
        ptr = ptr->next;
    }
    ptr->next = new_node;
    return head;
}

void destroy(Node *head)
{
    Node *ptr = NULL, *prev = NULL;
    
    ptr = head;
    if (!ptr) {
        return;
    }
    while (ptr) {
        prev = ptr;
        ptr = ptr->next;
        free(prev);
    }
}
