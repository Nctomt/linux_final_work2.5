#include "head.h"
//上传    
void puts_file(int sfd, char* fileName)
{
	signal(SIGPIPE, SIG_IGN);
	train t;
	int ret, fd;
	char buf_start[4] = {0};
	//MD5码发送
	const char *file_path = fileName;
	char md5_str[MD5_STR_LEN + 1];
	ret = Compute_file_md5(file_path, md5_str);
	puts(md5_str);
	if(0 == ret)
	{
		send(sfd, md5_str, strlen(md5_str), 0);
	}

	recv(sfd, buf_start, sizeof(buf_start), 0);
	if(strcmp("1", buf_start) == 0)
	{
		printf("start uploading...\n");
		sleep(1);
		printf("upload succsee!\n");
		return;
	}else{
		printf("start uploading...\n");
	}
	fd = open(fileName, O_RDWR);
	//发送文件名大小
	t.len = strlen(fileName);
	strcpy(t.buf, fileName);
	ret = send(sfd, &t, 4+t.len, 0);
	//发送文件信息
	struct stat buf;
	stat(fileName , &buf);
	memcpy(t.buf, &buf.st_size, sizeof(off_t));
	t.len = sizeof(off_t);
	send_n(sfd, (char*)&t, 4+t.len);
	//发送文件
	//off_t value = buf.st_size/100;
	//float real_size = buf.st_size;
	//float before_size = buf.st_size;
	while((t.len = read(fd, t.buf, sizeof(t.buf))) > 0)
	{
		ret = send_n(sfd, (char*)&t, 4+t.len);
		if(-1 == ret)
		{
			printf("upload succsee!\n");
			return;
		}
	}	
	send(sfd, &t, 4+t.len, 0);
	return ;
}
//Md5

int Compute_file_md5(const char *file_path, char *md5_str)
{
	int i;
	int fd;
	int ret;
	unsigned char data[READ_DATA_SIZE];
	unsigned char md5_value[MD5_SIZE];
	MD5_CTX md5;

	fd = open(file_path, O_RDONLY);
	if (-1 == fd)
	{
		perror("open");
		return -1;
	}

	// init md5
	MD5Init(&md5);

	while (1)
	{
		ret = read(fd, data, READ_DATA_SIZE);
		if (-1 == ret)
		{
			perror("read");
			return -1;
		}

		MD5Update(&md5, data, ret);

		if (0 == ret || ret < READ_DATA_SIZE)
		{
			break;
		}
	}

	close(fd);

	MD5Final(&md5, md5_value);

	for(i = 0; i < MD5_SIZE; i++)
	{
		snprintf(md5_str + i*2, 2+1, "%02x", md5_value[i]);
	}
	md5_str[MD5_STR_LEN] = '\0'; // add end

	return 0;
}
