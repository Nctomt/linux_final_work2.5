#include "md5.h"
#include <errno.h>
#include <shadow.h>
#include <mysql/mysql.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/epoll.h>
#include <sys/uio.h>
#include <crypt.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <pthread.h>
#include <sys/time.h>
#include <signal.h>
#include <sys/msg.h>
#include <sys/sem.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include <fcntl.h>
#include <dirent.h>
#include <time.h>
#include <grp.h>
#include <pwd.h>
#include <stdlib.h>
#define IF_ARGS(a) { if(argc!=a){printf("error args\n");return -1;}}
#define READ_DATA_SIZE  1024
#define MD5_SIZE        16
#define MD5_STR_LEN     (MD5_SIZE * 2)

int Compute_file_md5(const char *file_path, char *value);
void Login(int);
void get_headChar(char*);
int recv_n(int , char*, int);
int send_n(int , char*, int);
void get_leftChar(char*, int, int);
void puts_file(int, char*);
void gets_file(int);
void rm_file(int);
void default_file(int );
typedef struct{
	int len;
	char buf[1000];
}train;
