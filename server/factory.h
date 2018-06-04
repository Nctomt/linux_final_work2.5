#ifndef __FACTORY_H__
#define __FACTORY_H__
#include "head.h"
#include "que.h"

typedef struct{
	pthread_t *pth;
	pthread_cond_t cond;
	que_t que;
	short pthread_num;
	short start_flag; //0 -N  1 -Y
}factory;

typedef struct{
	int len;
	char buf[1000];
}train;

void factory_init(factory*, int ,int);
void factory_start(factory*);
void *threadfunc(void*);
void getsalt(char *, char*);
void get_leftChar(char*, int, int);
void trans_file(int, char*);
void recv_file(int);
int send_n(int , char*, int);
int recv_n(int , char*, int);
#endif
