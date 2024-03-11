#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "priority_queue_list.h"

void qlist(pqueue *head, void (*print_data)(void *)) {
	pqueue *p;
	
	for (p = head; p != NULL; p = p->next) {
		printf("%d: ", p->k);
		print_data(p->data);
		printf("\n");
	}
	
}

void qinsert(pqueue **phead, void *data, int k) {
    pqueue *new_node = malloc(sizeof(pqueue));

    if (new_node == NULL) {
        fprintf(stderr, "Memory allocation failed.\n");
        exit(EXIT_FAILURE);
    }

    new_node->data = strdup(data);
    if (new_node->data == NULL) {
        fprintf(stderr, "Memory allocation failed.\n");
        free(new_node);
        exit(EXIT_FAILURE);
    }
    new_node->k = k;

    if (*phead == NULL) {
        *phead = new_node;
        new_node->next = NULL;
        new_node->prev = NULL;
    } else {
        pqueue *current = *phead;
        while (current->next != NULL && strcmp(data, current->data) < 0) {
            current = current->next;
        }
        if (strcmp(data, current->data) >= 0) {
            if (current->prev == NULL) {
                *phead = new_node;
            } else {
                current->prev->next = new_node;
            }
            new_node->prev = current->prev;
            new_node->next = current;
            current->prev = new_node;
        } else {
            current->next = new_node;
            new_node->prev = current;
            new_node->next = NULL;
        }
    }
}



void qremove(pqueue **phead, int index) {
	if (*phead == NULL) {
        return;
    }

    pqueue *current = *phead;
    int i = 0;

    while (current != NULL && i < index) {
        current = current->next;
        i++;
    }

    if (current == NULL) {
        fprintf(stderr, "Key out of range.\n\n");
        return;
    }

    if (current->prev != NULL) {
        current->prev->next = current->next;
    } else {
        *phead = current->next;
    }

    if (current->next != NULL) {
        current->next->prev = current->prev;
    }

    free(current->data); 
    free(current);
}


