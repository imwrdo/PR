typedef struct pqueue pqueue;
struct pqueue {
	void *data;
	pqueue *next;
	pqueue *prev;
};

void qinsert(pqueue **phead, void *data);

void qlist(pqueue *head, void (*print_data)(void *));

void qremove(pqueue **phead, int index);
