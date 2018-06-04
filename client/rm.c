#include "head.h"
void rm_file(int sfd)
{
	int flag;
	char buf[4] = {0};
	recv(sfd, buf, sizeof(buf), 0);
	flag = atoi(buf);
	if(0 == flag)
	{
		printf("rm filed!\n");
		return;
	}else{
		printf("rm success!\n");
		return ;
	}
}
