#include "head.h"
void default_file(int sfd)
{
	int flag;
	char buf[4] = {0};
	recv(sfd, buf, sizeof(buf), 0);
	flag = atoi(buf);
	if(0 == flag)
	{
		printf("error input!\n");
		return;
	}
	return;
}
