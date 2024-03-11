#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "priority_queue_list.h"

typedef struct process process;
struct process {
	char name[200];
};

void print_process(void *data) {
	process *pr = (process *)data;
	printf("%s", pr->name);
}

int main(int argc, char **argv) {
	
	pqueue *queue = NULL;

	int choice;
    do {
        printf("\n===== MENU =====\n");
        printf("1. Insert a data\n");
        printf("2. Remove a data\n");
        printf("3. List all datas\n");
        printf("4. Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                printf("Enter data: ");
                char name[200];
                scanf("%s", name);
                process *new_process = (process *)malloc(sizeof(process));
                if (new_process == NULL) {
                    fprintf(stderr, "Memory allocation failed.\n");
                    exit(EXIT_FAILURE);
                }
                strcpy(new_process->name, name);
                qinsert(&queue, new_process);
                free(new_process);
                break;
            case 2:
                printf("Enter index of the process to remove: ");
                int remove_index;
                scanf("%d", &remove_index);
                qremove(&queue, remove_index);
                break;
            case 3:
                printf("\n===== LIST OF DATA =====\n");
                qlist(queue, print_process);
                break;
            case 4:
                printf("Exiting...\n");
                break;
            default:
                printf("\nInvalid choice! Please enter a number between 1 and 4.\n");
        }
    } while (choice != 4);

    while (queue != NULL) {
        pqueue *temp = queue;
        queue = queue->next;
        free(temp->data);
        free(temp);
    }

    return 0;
}
