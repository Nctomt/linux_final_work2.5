#include "head.h"
void gets_file(int sfd)
{
	char buf[1000] = {0};
	char fileName[32] = {0};
	int len, fd;
	off_t file_size;
	float before_size = 0;
	float real_size = 0;
	off_t value;
	char buf_flag[1];
	int flag;
	//接收要下载的文件名
	recv(sfd, buf_flag, sizeof(buf_flag), 0);
	flag = atoi(buf_flag);					
	if(0 == flag)
	{
		printf("No such file!\n");
		return;
	}else{
		//接收文件长度
		recv(sfd, &len, sizeof(int), 0);
		recv(sfd, buf, len, 0);			//文件名
		printf("Name:%s\n", buf);
		//接收文件大小
		recv(sfd, &len, sizeof(int), 0);
		recv(sfd, &file_size, len, 0);
		printf("size:%ld\n", file_size);
		fd = open(buf, O_RDWR|O_CREAT, 0666);
		value = file_size/100;
		while(1)
		{
			recv_n(sfd, (char*)&len, sizeof(int));
			if(len > 0)
			{
				recv_n(sfd, buf, len);
				write(fd, buf, len);
				real_size = real_size + len;
				if(real_size - before_size >= value)
				{
					printf("\r%5.2f%s", real_size/file_size*100, "%");
					fflush(stdout);
					before_size = real_size;
				}
			}else{
				printf("\r100.00%s\n", "%");
				printf("download success!\n");
				return;
			}
		}
	}
}
