#include "head.h"
void Login(int sfd)
{
	char usrname[20];
	char* passwd;
	char salt[20] = {0};
	char cnt_flag[1] = {0};
	char buf[256] = {0};
	printf("Please input the usrname\n");
	read(0, usrname, sizeof(usrname));
	send(sfd, usrname, strlen(usrname)-1, 0);
	recv(sfd, salt, sizeof(salt), 0);	 //收到salt
	passwd = getpass("Please input passwd\n");
	strcpy(buf, crypt(passwd, salt));
	send(sfd, buf, strlen(buf), 0);      //发送密文
	recv(sfd, cnt_flag, 1, 0);
	int psd = atoi(cnt_flag);
	if(psd == 0)
	{
		printf("error passwd\n");
		return;
	}else{
		printf("Landing success...\n");
		sleep(1);
		system("clear");
	}
	printf("User:%s\n", usrname);
}
