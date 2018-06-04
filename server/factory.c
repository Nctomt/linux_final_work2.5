#include "factory.h"
void factory_init(factory *p, int th_num,int capacity)
{
	bzero(p, sizeof(factory));
	p->pth = (pthread_t*)calloc(th_num, sizeof(pthread_t));
	pthread_cond_init(&p->cond, NULL);
	pthread_mutex_init(&p->que.que_mutex, NULL);
	p->pthread_num = th_num;
	p->que.que_capacity = capacity;
}

void factory_start(factory *p)
{
	int i;
	if(!p->start_flag)
	{
		for(i = 0; i < p->pthread_num; i++)
		{
			pthread_create(p->pth+i, NULL, threadfunc, p);
		}
		p->start_flag = 1;
	}
}

	
