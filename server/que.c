#include "que.h"

void que_insert(pque_t pq, pnode_t pnew)
{
	if(!pq->que_head)
	{
		pq->que_head = pnew;
		pq->que_tail = pnew;
	}else{
		pq->que_tail->pnext = pnew;
		pq->que_tail = pnew;
	}
	pq->que_size++;
}

void que_get(pque_t pq, pnode_t* pcur)
{
	*pcur = pq->que_head;
	pq->que_head = pq->que_head->pnext;
	pq->que_size--;
}
