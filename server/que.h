#ifndef __QUE_H__
#define __QUE_H__

#include "head.h"

typedef struct node{
	int new_fd;
	struct node *pnext;
}node_t, *pnode_t;

typedef struct{
	pnode_t que_head, que_tail;
	int que_capacity;
	int que_size;
	pthread_mutex_t que_mutex;
}que_t, *pque_t;

void que_insert(pque_t, pnode_t);
void que_get(pque_t, pnode_t*);


#endif

