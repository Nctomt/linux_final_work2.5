#include "factory.h"

void trans_file(int sfd, char* fileName)
{
	signal(SIGPIPE, SIG_IGN);
	train t;
	int ret;
	//发送文件名大小
	t.len = strlen(fileName);
	strcpy(t.buf, fileName);
	send(sfd, &t, 4+t.len, 0);

	//发送文件信息
	struct stat buf;
	stat(fileName, &buf);
	memcpy(t.buf, &buf.st_size, sizeof(off_t));
	t.len = sizeof(off_t);
	send_n(sfd, (char*)&t, 4+t.len);

	int fd = open(fileName, O_RDWR);
	//发送文件
	while((t.len = read(fd, t.buf, sizeof(t.buf))) > 0)
	{
		ret = send_n(sfd, (char*)&t, 4+t.len);
		if(-1 == ret)
		{
			return;
		}
	}
	send(sfd, &t, 4+t.len, 0);
	return;
}
void recv_file(int sfd)
{	
	char buf[1000] = {0};
	int len, fd;
	off_t file_size;
	float before_size = 0;
	float real_size = 0;
	off_t value;

	//接收文件长度
	recv(sfd, &len, sizeof(int), 0);
	recv(sfd, buf, len, 0);
	printf("Name:%s\n", buf);
	//接收文件的大小
	recv(sfd, &len, sizeof(int), 0);
	recv(sfd, &file_size, len, 0);
	printf("Size:%ld\n", file_size);
	fd = open(buf, O_RDWR|O_CREAT, 0666);
	value = file_size/100;
	while(1)
	{
		recv_n(sfd, (char*)&len, sizeof(int));
		if(len > 0)
		{
			recv_n(sfd, buf, len);
			write(fd, buf, len);
			real_size = real_size +len;
			if(real_size - before_size >= value)
			{
				printf("\r%5.2f%s", real_size/file_size*100, "%");
				fflush(stdout);
				before_size = real_size;
			}
		}else{
			printf("\r100.00%s\n", "%");
			printf("upload success!\n");
			return;
		}
	}
}


int send_n(int sfd, char*p, int len)
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
int recv_n(int sfd, char*p, int len)
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
