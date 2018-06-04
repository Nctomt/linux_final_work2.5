#include "head.h"
void get_leftChar(char *arr, int len, int cutLen)
{
	int i;
	char buf[10] = {0};
	strcpy(buf, arr);
	bzero(arr, len);
	for(i = 0; i < len-cutLen-1; i++)
	{
		buf[i] = buf[i+cutLen+1];
	}
	strncpy(arr, buf, len-cutLen-1);
}
