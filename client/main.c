#include "head.h"

int main(int argc, char* argv[])
{
	IF_ARGS(3);
	int sfd = socket(AF_INET, SOCK_STREAM, 0);
	struct sockaddr_in ser;
	ser.sin_family = AF_INET;
	ser.sin_port = htons(atoi(argv[2]));
	ser.sin_addr.s_addr = inet_addr(argv[1]);
	connect(sfd, (struct sockaddr*)&ser, sizeof(struct sockaddr));
	
	Login(sfd);								//登录	
	char *cmd[7] = {"cd", "pwd", "ls", "mkdir", "rm", "gets", "puts"};
	char buf_cmd[16] = {0};
	char buf_path[16] = {0};
	char buf_fpath[64] = {0};
	char buf_ls[64] = {0};
	char buf_file[16] = {0};
	recv(sfd, buf_path, sizeof(buf_path), 0);
	printf("Curent path:%s\n", buf_path);
	//文件操作
	int flag_cmd;
	int i, ret;
	while(1)
	{
		bzero(buf_path, sizeof(buf_path));
		bzero(buf_ls, sizeof(buf_ls));
		bzero(buf_fpath, sizeof(buf_fpath));
		bzero(buf_cmd, sizeof(buf_cmd));
		bzero(buf_file, sizeof(buf_file));

		read(0, buf_cmd, sizeof(buf_cmd));		//标准输入读取命令传送给服务器
		send(sfd, buf_cmd, strlen(buf_cmd)-1, 0);
		strncpy(buf_file, buf_cmd, strlen(buf_cmd)-1);
		get_headChar(buf_cmd);					//获取命令类型
		//printf("%s\t%d\n", buf_file, strlen(buf_file));
		for(i = 0; i < 7; i++){
			if(strcmp(cmd[i], buf_cmd) == 0){
				flag_cmd = i;
				break;
			}
		}
		switch(flag_cmd)
		{
			case 0:
				//	printf("cd\n");
				ret = recv(sfd, buf_path, sizeof(buf_path), 0);
				if(0 == ret)
				{
					return -1;
				}
				printf("Curent path:%s\n", buf_path);
				recv(sfd, buf_ls, sizeof(buf_ls), 0);
				printf("%s\n", buf_ls);
				break;
			case 1:
				//printf("pwd\n");
				ret = recv(sfd, buf_fpath, sizeof(buf_fpath), 0);
				if(0 == ret)
				{
					return -1;
				}
				printf("%s\n", buf_fpath);
				break;
			case 2:
				//printf("ls\n");
				ret = recv(sfd, buf_ls, sizeof(buf_ls), 0);
				if(0 == ret)
				{
					return -1;
				}
				printf("%s\n", buf_ls);
				break;
			case 3:
				//printf("mkdir\n");
				break;
			case 4:
				//printf("rm\n");
				rm_file(sfd);
				break;
			case 5:
				//printf("gets\n");
				gets_file(sfd);
				break;
			case 6:
				//printf("puts\n");
				get_leftChar(buf_file, strlen(buf_file), 4);			//获取要上传的文件名
				puts_file(sfd, buf_file);
				break;
			default:
				default_file(sfd);
				break;
			}
	}
	return 0;
}
