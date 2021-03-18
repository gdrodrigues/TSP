//
// Created by Gabriel Duarte on 16/03/21.
//

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

struct node {
    int data;
    struct node *next;
};

//display the list
void printList(struct node** head_ref) {
    struct node *ptr = (*head_ref);
    printf("\n[ ");

    //start from the beginning
    while(ptr != NULL) {
        printf("(%d) ",ptr->data);
        ptr = ptr->next;
    }

    printf(" ]\n");
}

//insert link at the first location
void insertFirst(struct node** head_ref, int data) {
    //create a link
    struct node *link = (struct node*) malloc(sizeof(struct node));

    link->data = data;

    //point it to old first node
    link->next = (*head_ref);

    //point first to new first node
    (*head_ref) = link;
}

//delete first item
struct node* deleteFirst(struct node** head_ref) {

    //save reference to first link
    struct node *tempLink = (*head_ref);

    //mark next to first link as first
    (*head_ref) = (*head_ref)->next;

    //return the deleted link
    return tempLink;
}

int length(struct node** head_ref) {
    int length = 0;
    struct node *current;

    for(current = (*head_ref); current != NULL; current = current->next) {
        length++;
    }

    return length;
}

//delete a link with given data
struct node* delete(struct node** head_ref, int data) {

    //start from the first link
    struct node* current = (*head_ref);
    struct node* previous = NULL;

    //if list is empty
    if((*head_ref) == NULL) {
        return NULL;
    }

    //navigate through list
    while(current->data != data) {

        //if it is last node
        if(current->next == NULL) {
            return NULL;
        } else {
            //store reference to current link
            previous = current;
            //move to next link
            current = current->next;
        }
    }

    //found a match, update the link
    if(current == (*head_ref)) {
        //change first to point to next link
        (*head_ref) = (*head_ref)->next;
    } else {
        //bypass the current link
        previous->next = current->next;
    }

    return current;
}