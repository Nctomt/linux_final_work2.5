#include "head.h"
void get_headChar(char* arr)
{
	char tmp[48];
	strcpy(tmp, arr);
	bzero(arr, sizeof(arr));
	int i = 0;
	while(tmp[i])
	{
		arr[i] = tmp[i];
		i++;
		if(tmp[i] == ' '|| tmp[i] == '\n' )
			break;

	}
}
void get_leftChar(char *arr, int len, int cutLen)
{
	int i;
	char buf[30] = {0};
	strcpy(buf, arr);
	bzero(arr, len);
	for(i = 0; i < len-cutLen-1; i++)
	{
		buf[i] = buf[i+cutLen+1];
	}
	strncpy(arr, buf, len-cutLen-1);
}
