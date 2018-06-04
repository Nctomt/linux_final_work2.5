#include "head.h"
int recv_n(int sfd, char* p, int len)
{
	int ret;
	int total = 0;
	while(total < len)
	{
		ret = recv(sfd, p+total, len-total, 0);
		if(-1 == ret)
		{
			return -1;
		}
		total = total + ret;
	}
	return 0;
}
int send_n(int sfd, char* p, int len)
{
	int ret;
	int total = 0;
	while(total < len)
	{
		ret = send(sfd, p+total, len-total, 0);
		if(-1 == ret)
		{
			return -1;
		}
		total = total + ret;
	}
	return 0;
}
