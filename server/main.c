#include "factory.h"

void* threadfunc(void* p)
{
	factory* pf = (factory*)p;
	pque_t pq = &pf->que;
	pnode_t pcur;
	while(1)
	{
		pthread_mutex_lock(&pq->que_mutex);
		if(!pq->que_size)
		{
			pthread_cond_wait(&pf->cond, &pq->que_mutex);
		}
		que_get(pq, &pcur);
		pthread_mutex_unlock(&pq->que_mutex);

 		//Login
 		//校验用户名 成功后发给客户端咸值
 		char usrname[20] = {0};
 		recv(pcur->new_fd, usrname, sizeof(usrname), 0);
 		struct spwd *sp;
 		bzero(&sp, sizeof(sp));	
 		char salt[32] = {0};			//咸值
 		MYSQL* conn;
 		MYSQL_RES *res;
 		MYSQL_ROW row;
 		const char* server = "localhost";
 		const char* user = "root";
 		const char* password = "199458";
 		const char* database = "UserInfo";
 		char query[300] = "select * from User where UserName='";
 		sprintf(query, "%s%s%s", query, usrname, "'");
 		int t;
 		conn = mysql_init(NULL);
 		if(!mysql_real_connect(conn, server, user, password, database, 0, NULL, 0))
 		{
 			 printf("Error connectint to database:%s\n", mysql_error(conn));
 			 pthread_exit(0);
 		}else{
 			printf("Connected...\n");
 		}
 		t = mysql_query(conn, query);
 		if(t)
 		{
 			printf("Error making query %s\n", mysql_error(conn));
 		}else{
 			res = mysql_use_result(conn);
 			row = mysql_fetch_row(res);
 			getsalt(salt, row[2]);
 			if(strcmp(row[1], usrname) == 0)
 			{	
 				printf("username passed\n");
 				send(pcur->new_fd, salt, strlen(salt), 0);
 			}else{
 				//用户名不存在未设计
				pthread_exit(0);
 			}
 			mysql_free_result(res);
 		}
 		//校验密码
 		char passwd[512] = {0};
 		recv(pcur->new_fd, passwd, sizeof(passwd), 0);//接收密文
 		t = mysql_query(conn, query);
 		if(t)
 		{
 			printf("Error making query %s\n", mysql_error(conn));
 		}else{
 			res = mysql_use_result(conn);
 			row = mysql_fetch_row(res);
 			if(strcmp(row[2], passwd) == 0)
 			{	
 				printf("passwd passed\n");
 				send(pcur->new_fd, "1", 1, 0);
 			}else{
 				send(pcur->new_fd, "0", 1, 0);
 				pthread_exit(0);
 			}
 			mysql_free_result(res);
 		}

		
		//登陆成功后 服务器 发送给客户端一个目录
		char root[16] = {0};				//根目录
		bzero(query, sizeof(query));
		strcpy(query,"select * from Dir where par_dir is NULL;");
		//查找该用户的父目录
		t = mysql_query(conn, query);
		if(t)
		{
			printf("Error making query: %s\n", mysql_error(conn));
		}else{
			res = mysql_use_result(conn);
 			row = mysql_fetch_row(res);
			strcpy(root, row[1]);		//根目录
			send(pcur->new_fd, root, strlen(root), 0);
			mysql_free_result(res);
		}
	
		int ret;
		int len_cmd, len_path, len_fpath;
		char buf_tmp[128] = {0};
		char cmd[30] = {0};				// cd ls pwd
		char ls_buf[64] = {0};			//目录数组
		char buf_cmd[30] = {0};			//接收到的完整命令
		char buf_path[30] = {0}; 		//当前路径
		char buf_fpath[64] = {0};		//完整路径
		char buf_md5[100] = {0};		//md5
		strcpy(buf_fpath, root);		//完整路径 起始也为根目录
		strcpy(buf_path, root);			//起始目录为根目录
		while(1)
		{
			//printf("I am here\n");
			bzero(query, sizeof(query));
			bzero(buf_cmd, sizeof(buf_cmd));
			bzero(ls_buf, sizeof(ls_buf));
			bzero(buf_md5, sizeof(buf_md5));
		    ret = recv(pcur->new_fd, buf_cmd, sizeof(buf_cmd), 0);
			if(0 == ret)
			{
				pthread_exit(0);
			}
			len_cmd = strlen(buf_cmd);		//客户端传来命令行的长度
			//printf("%d\n", ret);
			//printf("%s\n", buf_cmd);
			
			bzero(cmd, sizeof(cmd));
			if(strcmp("cd", strncpy(cmd, buf_cmd, 2)) == 0)
			{
				printf("I am cd\n");
				get_leftChar(buf_cmd, len_cmd, 2); 
				//输入命令数组  长度  截取长度
				if(strcmp("..", buf_cmd) == 0)
				{
					bzero(buf_tmp, sizeof(buf_tmp));
					strcpy(buf_tmp, buf_fpath);
					bzero(buf_fpath, sizeof(buf_fpath));
					strncpy(buf_fpath, buf_tmp, len_fpath-len_path-1);
					puts(buf_fpath);

					strcpy(query, "select * from Dir where pwdir='");
					sprintf(query, "%s%s%s", query, buf_path, "'");
					t = mysql_query(conn, query);
					if(t)
					{
						printf("Error making query: %s\n", mysql_error(conn));
					}else{
						res = mysql_use_result(conn);
		 				row = mysql_fetch_row(res);
						if(NULL == row[2])
						{//父目录为空
							strcat(root, "\n");
							send(pcur->new_fd, root, strlen(root), 0);
							send(pcur->new_fd,"This is root!\n", strlen("This is root!\n"), 0);
							strcpy(buf_path, root);
						    mysql_free_result(res);
							continue;
						}else{
							strcpy(buf_path, row[2]);	//存储下父目录 并发送
							send(pcur->new_fd, buf_path, strlen(buf_path),0);
						}
						mysql_free_result(res);
					}
					//发送cd后 子目录
					strcpy(query, "select * from Dir where pwdir='");
					sprintf(query, "%s%s%s", query, buf_path, "'");
					t = mysql_query(conn, query);
					if(t)
					{
						printf("Error making query: %s\n", mysql_error(conn));
					}else{
						res = mysql_use_result(conn);
						row = mysql_fetch_row(res);
						strcpy(ls_buf, row[3]);
						send(pcur->new_fd, ls_buf, strlen(ls_buf), 0);
						mysql_free_result(res);
					}						
					continue;
				}else{
					bzero(buf_path, sizeof(buf_path));
					strcpy(buf_path, buf_cmd);		//存储下当前目录
					len_path = strlen(buf_path);

					strcat(buf_fpath, "/");			//记录完整目录
					strcat(buf_fpath, buf_path);
					len_fpath = strlen(buf_fpath);
					puts(buf_fpath);
					
					//发送给客户端当前目录
					send(pcur->new_fd, buf_path, strlen(buf_path), 0);

					strcpy(query,"select * from Dir where pwdir='");
					sprintf(query, "%s%s%s", query, buf_cmd, "'");
					//printf("%s\n", query);
					t = mysql_query(conn, query);
					if(t)
					{
						printf("Error making query: %s\n", mysql_error(conn));
					}else{
						res = mysql_use_result(conn);
		 				row = mysql_fetch_row(res);
						strcpy(ls_buf, row[3]);
						//发送cd后的子目录
						send(pcur->new_fd, ls_buf, strlen(ls_buf), 0);
						mysql_free_result(res);
					}
					continue;
				}
			}
			if(strcmp("pwd", buf_cmd) == 0)
			{
				printf("I am pwd\n");
				send(pcur->new_fd, buf_fpath, strlen(buf_fpath), 0);	
				continue;
			}
			if(strcmp("ls", buf_cmd) == 0)
			{	
				printf("I am ls\n");
				bzero(ls_buf, sizeof(ls_buf));
				strcpy(query,"select * from Dir where pwdir='");
				sprintf(query, "%s%s%s", query, buf_path, "'");
				t = mysql_query(conn, query);
				if(t)
				{
					printf("Error making query: %s\n", mysql_error(conn));
				}else{
					res = mysql_use_result(conn);
	 				row = mysql_fetch_row(res);
					strcpy(ls_buf, row[3]);				
					send(pcur->new_fd, ls_buf, strlen(ls_buf), 0);
					mysql_free_result(res);
				}
				continue;
			}
			bzero(cmd, sizeof(cmd));
			if(strcmp("gets", strncpy(cmd, buf_cmd, 4)) == 0)
			{
				//printf("I am gets!\n");
				get_leftChar(buf_cmd, len_cmd, 4);
				strcpy(query, "select * from Dir where type='");
				sprintf(query, "%s%s%s", query, "f", "'");
				t = mysql_query(conn, query);
				if(t)
				{
					printf("Error making query:%s\n", mysql_error(conn));
				}else{
					res = mysql_use_result(conn);
					row = mysql_fetch_row(res);
					if(strcmp(row[1], buf_cmd) == 0)//后改为校验MD5
					{
						send(pcur->new_fd, "1", 1, 0);//校验成功发送1
						trans_file(pcur->new_fd, buf_cmd);//参数2：文件名
						mysql_free_result(res);
						continue;
					}else{
						send(pcur->new_fd, "0", 1, 0);//失败发送0
						mysql_free_result(res);
						continue;
					}
				}
			}
			bzero(cmd, sizeof(cmd));
			if(strcmp("puts", strncpy(cmd, buf_cmd, 4)) == 0)
			{
				printf("I am puts\n");
				get_leftChar(buf_cmd, len_cmd, 4);
				//接收MD5码
				recv(pcur->new_fd, buf_md5, sizeof(buf_md5), 0);
				puts(buf_md5);
				strcpy(query, "select * from Dir where type='f' AND MD5='");
				sprintf(query, "%s%s%s", query, buf_md5, "'");
				t = mysql_query(conn, query);
				if(t)
				{
					printf("Error making query:%s\n", mysql_error(conn));
				}else{
					res = mysql_use_result(conn);
					row = mysql_fetch_row(res);
					if(NULL == row)
					{
						send(pcur->new_fd, "0", 1, 0);
						mysql_free_result(res);
						//插入到数据库
						//插入文件栏
						bzero(query, sizeof(query));
						strcpy(query, "insert into Dir (pwdir,par_dir,MD5,usrID,type)value('");
						sprintf(query, "%s%s%s%s%s%s%s", query, buf_cmd, "','",buf_path, "','", buf_md5, "',2,'f')" );
						puts(query);
						if(mysql_query(conn, query)){
							printf("Error making query:%s\n", mysql_error(conn));
						}else{printf("insert ok\n");}
						//更新path_buf 的 子目录
						bzero(query, sizeof(query));
						strcpy(query, "select * from Dir where pwdir='");
						sprintf(query, "%s%s%s", query, buf_path, "'");
						mysql_query(conn, query);
						res = mysql_use_result(conn);
						row = mysql_fetch_row(res);
						strcpy(ls_buf, row[3]);
						strcat(ls_buf, "  ");
						strcat(ls_buf, buf_cmd);
						mysql_free_result(res);
						
						bzero(query, sizeof(query));
						strcpy(query, "update Dir set son_dir='");
						sprintf(query, "%s%s%s", query, ls_buf, "' ");
						strcat(query, "where pwdir='");
						sprintf(query, "%s%s%s", query, buf_path, "'");
						puts(query);
						mysql_query(conn, query);

						recv_file(pcur->new_fd);
						continue;
					}else{
						send(pcur->new_fd, "1", 1, 0);
					}
					mysql_free_result(res);
					continue;
				}
			}
			bzero(cmd, sizeof(cmd));
			if(strcmp("mkdir", strncpy(cmd, buf_cmd, 5)) == 0)
			{
				//printf("I am mkdir\n");
				get_leftChar(buf_cmd, len_cmd, 5);
				puts(buf_cmd);
				//插入目录信息
				bzero(query, sizeof(query));
				strcpy(query, "insert into Dir(pwdir,par_dir,usrID,type)value('");
				sprintf(query, "%s%s%s%s%s%s%s", query, buf_cmd, "','", 
						buf_path, "',", "2,", "'d')" );
				puts(query);
				if(mysql_query(conn, query)){
					printf("Error making query:%s\n", mysql_error(conn));
				}else{printf("insert ok\n");}
				//在buf_path下更新子目录信息
				bzero(query, sizeof(query));
				strcpy(query, "select * from Dir where pwdir='");
				sprintf(query, "%s%s%s", query, buf_path, "'");
				mysql_query(conn, query);
				res = mysql_use_result(conn);
				row = mysql_fetch_row(res);
				strcpy(ls_buf, row[3]);
				strcat(ls_buf, "  ");
				strcat(ls_buf, buf_cmd);
				mysql_free_result(res);

				bzero(query, sizeof(query));
				strcpy(query, "update Dir set son_dir='");
				sprintf(query, "%s%s%s", query, ls_buf, "' ");
				strcat(query, "where pwdir='");
				sprintf(query, "%s%s%s", query, buf_path, "'");
				puts(query);
				mysql_query(conn, query);
			}
			bzero(cmd, sizeof(cmd));
			if(strcmp("rm", strncpy(cmd, buf_cmd, 2)) == 0)
			{	
				printf("I am rm\n");
				get_leftChar(buf_cmd, len_cmd, 2);
				puts(buf_cmd);
				//判断：要删除的文件 子目录为空删除  非空不执行
				bzero(query, sizeof(query));
				strcpy(query, "select * from Dir where pwdir='");
				sprintf(query, "%s%s%s", query, buf_cmd, "';");
				puts(query);
				mysql_query(conn, query);
				res = mysql_use_result(conn);
				row = mysql_fetch_row(res);
				//printf("%s\n", row[3]);
				if(NULL ==  row[3])
				{	
					printf("son_dir is NULL!\n");
					mysql_free_result(res);
					//在数据库中删除掉pwdir为buf_cmd的行
					bzero(query, sizeof(query));
					strcpy(query, "delete from Dir where pwdir='");
					sprintf(query, "%s%s%s", query, buf_cmd, "'" );
					puts(query);
					if(mysql_query(conn, query)){
						printf("Error making query:%s\n", mysql_error(conn));
					}else{printf("delete ok\n");}
					//在buf_path的子目录下删除
					//寻找父目录为buf_path  ， 粘贴到ls_buf数组中 更新
					bzero(query, sizeof(query));
					strcpy(query, "select * from Dir where par_dir='");
					sprintf(query, "%s%s%s", query, buf_path, "'");
					mysql_query(conn, query);
					res = mysql_use_result(conn);
					if(res)
					{
						while((row = mysql_fetch_row(res)) != NULL)
						{
							strcat(ls_buf, row[1]);
							strcat(ls_buf, "  ");
						}
					}
					mysql_free_result(res);
					
					bzero(query, sizeof(query));
					strcpy(query, "update Dir set son_dir='");
					sprintf(query, "%s%s%s", query, ls_buf, "' ");
					strcat(query, "where pwdir='");
					sprintf(query, "%s%s%s", query, buf_path, "'");
					puts(query);
					mysql_query(conn, query);
					send(pcur->new_fd, "1", 1, 0);
					continue;
				}else{
					mysql_free_result(res);
					send(pcur->new_fd, "0", 1, 0);
					continue;
				}
			}
			else
			{	
				printf("I am here!\n");
				send(pcur->new_fd, "0", 1, 0);
				continue;
			}
		}

 		mysql_close(conn);
 		free(pcur);
	}
}
int main(int argc, char*argv[])
{
	IF_ARGS(5);
	int th_num = atoi(argv[3]);
	int capacity = atoi(argv[4]);

	factory f;
	factory_init(&f, th_num, capacity);
	factory_start(&f);

	int ret; 					
	int sfd = socket(AF_INET, SOCK_STREAM, 0);
	if(-1 == sfd)
	{
		perror("socket");
		return -1;
	}
	int reuse = 1;
	setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(int));
	struct sockaddr_in ser;
	bzero(&ser, sizeof(ser));
	ser.sin_family = AF_INET; 
	ser.sin_port = htons(atoi(argv[2]));
	ser.sin_addr.s_addr = inet_addr(argv[1]);
	ret = bind(sfd, (struct sockaddr*)&ser, sizeof(struct sockaddr));
	if(-1 == ret)
	{
		perror("bind");
		return -1;
	}
	listen(sfd, capacity);

	
	int new_fd;
	pnode_t pnew;
	pque_t pq = &f.que;
	
	while(1)
	{
		new_fd = accept(sfd, NULL, NULL);
		pnew = (pnode_t)calloc(1, sizeof(node_t));
		pnew->new_fd = new_fd;
		pthread_mutex_lock(&pq->que_mutex);
		que_insert(pq, pnew);
		pthread_mutex_unlock(&pq->que_mutex);
		pthread_cond_signal(&f.cond);
	}
	return 0;
}
