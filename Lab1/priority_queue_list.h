typedef struct pqueue pqueue;
struct pqueue {
	char *data;
	pqueue *next;
	pqueue *prev;
};

void qinsert(pqueue **phead, char *data);

void qlist(pqueue *head);

void qremove(pqueue **phead, int index);
