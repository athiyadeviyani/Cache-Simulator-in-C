#include<stdio.h>
#include<stdlib.h>

typedef struct elem {
 int val;
 struct elem *nxt;
} elem;

elem *head;

// -> in c is like . in java
// if a name is assigned to object a 
// a.name() == a->name

// write the function insertq that adds an element to the end of the queue
void insertq(elem *qhead, int value) {
    // allocate memory of the size of the element
    elem *newElem = malloc(sizeof(elem));
    newElem->val = value;
    newElem->nxt = NULL; // initialize pointer

    if (qhead == NULL) {
        qhead = newElem; // when the linked list is empty
    } else {
        elem *p = qhead;
        while(p->nxt != NULL) {
            p = p->nxt;
        }
        p->nxt = newElem;
    }

}

// write the function removeq that removes an element from the end of the queue
void removeq(elem *qhead) {
    // if the next element is already empty, remove it
    if (qhead->nxt == NULL) {
        free(qhead);
    }

    elem *p = qhead;
    // but if it's not, continue until you find a NULL next value
    while(p->nxt->nxt != NULL) {
        p = p->nxt;
    }

    free(p->nxt);
    p->nxt = NULL;
}

// write the function sizeq that finds the number of elements in the queue
int sizeq(elem *qhead) {
    int count = 0;
    elem *p = qhead;
    // while not reached the end of the queue
    // keep updating the counter
    while (p->nxt != NULL) {
        p = p->nxt;
        count++;
    }
    return count;
}

// write the function searchq that searches through the queue for a value v and returns the reference to the first matching element
elem* searchq(elem *qhead, int value) {
    elem *p = qhead;
    while (p->nxt != NULL) {
        p = p->nxt;
        if(p->val == value){
            return p;
        }
    }
}

// additional function to check
void printList (elem *qhead){
    elem *p  = qhead;  
    while(p->nxt != NULL){
        p = p->nxt;
        printf("%d", p->val);
    }
}

int main() {
    // create the first element in the linked list using malloc and assign it to head
    elem *head = NULL;
    head = malloc(sizeof(elem));

    // testing 
    insertq(head, 5);
    insertq(head, 2);
    insertq(head, 6);
    insertq(head, 4);
    insertq(head, 6);
    printf("\n");
    printList(head);
    printf("\n");
    removeq(head);
    printList(head);
    printf("\n");
    int f = sizeq(head);
    printf("%d", f);
    printf("\n");
}