#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sqlite3.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <arpa/inet.h>
#include <signal.h>
#include <time.h>
#define PORT 8888
#define IP 	"192.168.20.132"
//打印错误新的宏函数
#define ERR_MSG(msg)  do{\
	fprintf(stderr, "__%d__", __LINE__);\
	perror(msg);\
}while(0)
//发送接收结构体
struct message
{
	char head;
	char msg[128];
	char name[50];
};
typedef void (*sighandler_t)(int);
void handler(int sig);
void word_exist();
int tcp_con();
int rcv_cli_msg(int newfd, struct sockaddr_in cin);
int sign_up(int newfd,struct sockaddr_in cin,char *msg,sqlite3* db_user);
int sign_in(int newfd,struct sockaddr_in cin,char *msg,sqlite3* db_user);
int search(int newfd,struct sockaddr_in cin,struct message cil_buf,sqlite3* db_word,sqlite3* db_user);
int history(int newfd,struct sockaddr_in cin,struct message cil_buf,sqlite3* db_user);
int do_select(sqlite3* db,char* table,char* name,char* buf);
int do_insert(sqlite3* db,char* buf);
int word_into();
int isonline(char* msg,sqlite3* db_user);
int log_out(char* name,sqlite3* db_user);
 
int main(int argc, const char *argv[])
{
	//捕获17号信号 SIGCHLD
	sighandler_t s = signal(17, handler);
	if(SIG_ERR == s)
	{
		ERR_MSG("signal");
		return -1;
	}
	//检查是否有单词数据库,创建用户数据库
	word_exist();
	printf("初始化完成\n");
	//TCP连接
	int sfd = tcp_con();
	//等待客户端连接
	printf("等待客户端连接\n");
	int newfd = 0;
	pid_t pid = 0;
	struct sockaddr_in cin;
	socklen_t addrlen = sizeof(cin);
	while(1)
	{
		//父进程只负责连接
		newfd = accept(sfd, (struct sockaddr*)&cin, &addrlen);
		if(newfd < 0)
		{
			ERR_MSG("accept");
			return -1;
		}
 
		printf("[%s : %d] newfd = %d 连接成功\n", inet_ntoa(cin.sin_addr), ntohs(cin.sin_port),newfd);
 
		//创建子进程
		pid = fork();
		if(pid > 0)
		{
			close(newfd);
		}
		else if(0 == pid)
		{
			close(sfd);
			rcv_cli_msg(newfd,cin);
 
			close(newfd);
			exit(0);
		}
		else
		{
			ERR_MSG("fork");
			return -1;
		}
	}
	close(sfd);
	return 0;
}
void handler(int sig)
{
	//回收僵尸进程
	while(waitpid(-1, NULL, WNOHANG) > 0);
}
//初始化判断
void word_exist()
{
	//导入单词到数据库
	if(access("./word.db",F_OK) ==  0)
	{
		printf("单词数据库已存在\n");
	}
	else
	{
		if(access("./dict.txt",F_OK) == -1)
		{
			printf("存储单词的文本不存在\n");
			exit(0);
		}
		else
		{
			printf("正在导入单词\n");
			word_into();
			printf("单词数据库导入成功\n");
		}
	}
	//创建用户数据库
	if(access("./user.db",F_OK) == 0)//数据库已经存在
	{
		return;
	}
	//打开数据库
	printf("正在创建用户数据库\n");
 
	sqlite3* db_user = NULL;
	if(sqlite3_open("./user.db", &db_user) != SQLITE_OK)
	{
		printf("errmsg:%s\n", sqlite3_errmsg(db_user));
		fprintf(stderr, "__%d__ sqlite3_open failed\n", __LINE__);
		exit(0);
	}
	//创建一个用户表格
	char* sql = "create table if not exists user (name char primary key, password char,online char);" ;
	char* errmsg = NULL;
 
	if(sqlite3_exec(db_user, sql, NULL, NULL, &errmsg) != SQLITE_OK)
	{
		fprintf(stderr, "__%d__ sqlite3_exec:%s\n", __LINE__, errmsg);
		exit(0);
	}
	//关闭数据库
	sqlite3_close(db_user);
 
	printf("用户数据库创建成功\n");
	return;
}
int word_into()
{
	char buf[1000];
	//打开文件
	FILE*fd = fopen("./dict.txt","r");
	if(NULL == fd)
	{
		perror("fopen");
		return -1;
	}
	//打开数据库
	sqlite3* db_word = NULL;
	if(sqlite3_open("./word.db",&db_word) != SQLITE_OK)
	{
		printf("errmsg:%s\n",sqlite3_errmsg(db_word));
		fprintf(stderr,"__%d__ sqlite3_open failed\n",__LINE__);
		return -1;
	}
	//创建一个表格
	char sql[128] = "create table if not exists word (word char,mean txt)";
	char* errmsg = NULL;
	if(sqlite3_exec(db_word,sql,NULL,NULL,&errmsg) != SQLITE_OK)
	{
		fprintf(stderr,"__%d__ sqlite3_exec:%s\n",__LINE__,errmsg);
		return -1;
	}
	//读取文件内容存储到表格中
	while(1)
	{
		if(NULL == fgets(buf,sizeof(buf),fd))//获取一行的内容
			break;
		do_insert(db_word,buf);
	}
	sqlite3_close(db_word);
	fclose(fd);
	return 0;	
}
//将单词插入到数据库中
int do_insert(sqlite3* db,char* buf)
{
	char word[50] = "",mean[500] = "";
	char* q = buf;
	int num = 0;
	char* errmsg = NULL;
	char sql[128] = "";
	//获取一行中的单词,遇到二个空格单个单词结束
	while(1)
	{
		if(*q == ' ' /*&& *(q+1) == ' '*/)
		{
			while(1)
			{
				if(*q != ' ')//将q指向下个不是空格的位置,即词性的开头
					break;
				q++;
			}
			break;
		}
		word[num] = *q;
		num++;
		q++;
	}
	//q指向后面内容就是单词词性和中文,直接拷贝到mean字符串中
	q[strlen(q)-1] = 0;
	strcpy(mean,q);
	sprintf(sql,"insert into word(word,mean) values(\"%s\",\"%s\")",word,mean);//存入到表中
	if(sqlite3_exec(db,sql,NULL,NULL,&errmsg) != SQLITE_OK)
	{
		fprintf(stderr,"__%d__ sqlite3_exec:%s\n",__LINE__,errmsg);
		return -1;
		//exit(1);
	}
	return 0;
}
//TCP连接
int tcp_con()
{
	int sfd = socket(AF_INET,SOCK_STREAM,0);
	if(sfd < 0)
	{
		ERR_MSG("socket");
		exit(0);
	}
	//允许端口快速重用
	int reuse = 1;
	if(setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) < 0)
	{
		ERR_MSG("setsockopt");
		exit(0);
	}
 
	//填充地址信息结构体
	struct sockaddr_in sin;
	sin.sin_family 		= AF_INET;
	sin.sin_port 		= htons(PORT); 	
	sin.sin_addr.s_addr = inet_addr(IP);
 
	//将地址信息结构体绑定到套接字上
	if(bind(sfd, (struct sockaddr*)&sin, sizeof(sin)) < 0)
	{
		ERR_MSG("bind");
		exit(0);
	}
 
	//将套接字设置为被动监听状态，让内核去监听是否有客户端连接;
	if(listen(sfd, 10) < 0)
	{
		ERR_MSG("listen");
		exit(0);
	}
	//打开数据库
	sqlite3* db_user = NULL;
	if(sqlite3_open("./user.db",&db_user) != SQLITE_OK)
	{
		printf("errmsg:%s\n",sqlite3_errmsg(db_user));
		fprintf(stderr,"__%d__ sqlite3_open failed\n",__LINE__);
		return -1;
	}
	//修改所有用户为非在线状态
	char sql[128] = "update user set online=\"N\"";
	char* errmsg = NULL;
	if(sqlite3_exec(db_user, sql, NULL, NULL, &errmsg) != SQLITE_OK)
	{
		fprintf(stderr, "__%d__ sqlite3_exec:%s\n", __LINE__, errmsg);
		return -1;
	}
	//关闭数据库
	sqlite3_close(db_user);
 
	return sfd;
 
}
//接收客户端信息
int rcv_cli_msg(int newfd, struct sockaddr_in cin)
{		
	//打开存储用户信息的数据库
	sqlite3* db_user = NULL;
	if(sqlite3_open("./user.db",&db_user) != SQLITE_OK)
	{
		printf("errmsg:%s\n",sqlite3_errmsg(db_user));
		fprintf(stderr,"__%d__ sqlite3_open failed\n",__LINE__);
		return -1;
	}		
	//打开存储单词的数据库
	sqlite3* db_word = NULL;
	if(sqlite3_open("./word.db",&db_word) != SQLITE_OK)
	{
		printf("errmsg:%s\n",sqlite3_errmsg(db_word));
		fprintf(stderr,"__%d__ sqlite3_open failed\n",__LINE__);
		return -1;
	}
	struct message buf;
	ssize_t res = 0;
	char name[50] = "";
	while(1)
	{
		memset(&buf,0, sizeof(buf));
		//循环接收 
		res = recv(newfd, &buf, sizeof(buf), 0);
		if(res < 0)
		{
			ERR_MSG("recv");
			return -1;
		}
		else if(0 == res)
		{
			printf("[%s : %d] newfd = %d 断开连接\n", inet_ntoa(cin.sin_addr), ntohs(cin.sin_port),newfd);
			//修改用户为非在线状态
			char sql[128] = "";
			sprintf(sql, "update user set online=\"N\" where name=\"%s\";", name);
			char* errmsg = NULL;
			if(sqlite3_exec(db_user, sql, NULL, NULL, &errmsg) != SQLITE_OK)
			{
				fprintf(stderr, "__%d__ sqlite3_exec:%s\n", __LINE__, errmsg);
				return -1;
			}
			//关闭数据库
			sqlite3_close(db_user);
			sqlite3_close(db_word);
			return 0;
		}
		switch (buf.head)
		{
		case 'Z':
			//用户注册
			sign_up(newfd,cin,buf.msg,db_user);
			break;
		case 'D':
			//用户登录
			sign_in(newfd,cin,buf.msg,db_user);
			strcpy(name,buf.msg);
			break;
		case 'C':
			//查询单词
			search(newfd,cin,buf,db_word,db_user);
			break;
		case 'H':
			//查历史记录
			history(newfd,cin,buf,db_user);
			break;
		case 'Q':
			//退出登录
			log_out(name,db_user);
			break;
		default:
			printf("buf.head = %c \t客户端错误\n",buf.head);
		}
	}
	return 0;
}
//用户注册
int sign_up(int newfd,struct sockaddr_in cin,char *msg,sqlite3* db_user)
{
	printf("用户请求注册\n");
	struct message buf;
	buf.head = 'Z';
	strcpy(buf.msg,"注册成功\n");
 
	char sql[128] = "";
	sprintf(sql, "insert into user values(\"%s\", \"%s\",\"%s\");",msg,(msg+strlen(msg)+1),"N");
 
	char* errmsg = NULL;
	if(sqlite3_exec(db_user, sql, NULL, NULL, &errmsg) != SQLITE_OK)
	{
		if(sqlite3_errcode(db_user) == 19)
		{
			printf("用户名[%s]重复注册\n",msg);
			//重复注册
			buf.head = 'F';
			strcpy(buf.msg,"用户名重复\n");
			//发送给客户端
			if(send(newfd,&buf,sizeof(buf),0) < 0)
			{
				ERR_MSG("send");
				return -1;
			}
			return -1;
		}
		fprintf(stderr, "__%d__ sqlite3_exec:%s\n", __LINE__, errmsg);
		
	}
	//创建一张表存储历史记录
	sprintf(sql,"create table if not exists %s (word char,mean char,time char);",msg);	
 
	if(sqlite3_exec(db_user, sql, NULL, NULL, &errmsg) != SQLITE_OK)
	{
		fprintf(stderr, "__%d__ sqlite3_exec:%s\n", __LINE__, errmsg);
		return -1;
	}
	printf("用户名[%s]注册成功\n",msg);
 
	//发送给客户端
	if(send(newfd,&buf,sizeof(buf),0) < 0)
	{
		ERR_MSG("send");
		return -1;
	}
	return 0;
}
//用户登录
int sign_in(int newfd,struct sockaddr_in cin,char *msg,sqlite3* db_user)
{
	char name[50] = "";
	char password[50] = "";
	sprintf(name,"name='%s'",msg);
	struct message buf;
	printf("用户请求登录\n");
	buf.head = 'D';
	strcpy(buf.msg,"登录成功\n");
 
	//查询用户名密码是否正确
	if(do_select(db_user,"user",name,password) == 0)
	{
		if(isonline(msg,db_user) != 0)
		{
			printf("用户名[%s]重复登录\n",msg);
			buf.head = 'F';
			strcpy(buf.msg,"重复登录\n");
			//发送给客户端
			if(send(newfd,&buf,sizeof(buf),0) < 0)
			{
				ERR_MSG("send");
				return -1;
			}	
			return 0;
		}
	}
 
	if(do_select(db_user,"user",name,password) == -1)
	{
		//用户名错误
		buf.head = 'F';
		strcpy(buf.msg,"用户名不存在\n");
		printf("用户名[%s]不存在\n",msg);
	}
	else if(strcmp(password,msg+strlen(msg)+1) != 0)
	{
		//密码错误
		buf.head = 'F';
		strcpy(buf.msg,"密码错误\n");
		printf("用户名[%s]密码错误\n",msg);
	}
	else
	{
		printf("用户名[%s]登录成功\n",msg);
		//修改用户为在线状态
		char sql[128] = "";
		sprintf(sql, "update user set online=\"Y\" where name=\"%s\";", msg);
 
		char* errmsg = NULL;
 
		if(sqlite3_exec(db_user, sql, NULL, NULL, &errmsg) != SQLITE_OK)
		{
			fprintf(stderr, "__%d__ sqlite3_exec:%s\n", __LINE__, errmsg);
			return -1;
		}
 
	}
	//发送给客户端
	if(send(newfd,&buf,sizeof(buf),0) < 0)
	{
		ERR_MSG("send");
		return -1;
	}	
}
//判断用户是否在线
int isonline(char* msg,sqlite3* db_user)
{
	//判断是否在线,N不在线,Y在线
    char sql[128] = "";
    char ** pres = NULL;    //存储查询结果的首地址
    int row, column;        //查询结果的行列数
    char* errmsg = NULL;
 
	sprintf(sql,"select * from user where name=\"%s\"",msg);
 
    if(sqlite3_get_table(db_user, sql, &pres, &row, &column, &errmsg) != SQLITE_OK)
    {
        fprintf(stderr, "__%d__ sqlite3_get_table:%s\n", __LINE__, errmsg);
        return -1;
    }
    //能运行到当前位置则说明，查询函数运行成功
	if(strcmp(pres[5],"Y") == 0)
	{
		//释放获取到的空间
		sqlite3_free_table(pres);
		pres = NULL;
		return -1;
	}
    //释放获取到的空间
    sqlite3_free_table(pres);
    pres = NULL;
    return 0;	
	
}
 
//查询数据库的单词意思和用户名
int do_select(sqlite3* db,char* table,char* name,char* buf)
{
    char sql[128] = "";
    char ** pres = NULL;    //存储查询结果的首地址
    int row, column;        //查询结果的行列数
    char* errmsg = NULL;
	sprintf(sql,"select * from %s where %s;",table,name);
 
    if(sqlite3_get_table(db, sql, &pres, &row, &column, &errmsg) != SQLITE_OK)
    {
        fprintf(stderr, "__%d__ sqlite3_get_table:%s\n", __LINE__, errmsg);
        return -1;
    }
	
	if(column == 0)//没有相关的单词或者用户名
	{
		printf("没有查询到\n");
		return -1;
	}
	strcpy(buf,pres[column+1]);
    //释放获取到的空间
    sqlite3_free_table(pres);
    pres = NULL;
    return 0;
}
//用户请求查词
int search(int newfd,struct sockaddr_in cin,struct message cil_buf,sqlite3* db_word,sqlite3* db_user)
{
	printf("用户[%s]请求查词\n",cil_buf.name);
	struct message buf;
	buf.head = 'C';
 
	//将单词和意思保存
	char word[150] = "";
	char mean[150] = "";
	sprintf(word,"word='%s'",cil_buf.msg);
	//调用查找函数查找单词
	if(do_select(db_word,"word",word,mean) == -1)
	{
		//单词不存在
		buf.head = 'F';
		strcpy(buf.msg,"单词不存在\n");
		printf("单词[%s]不存在\n",cil_buf.msg);
	}
	else
	{
		printf("单词[%s]查询成功,解释[%s]\n",cil_buf.msg,mean);
		strcpy(buf.msg,mean);
	}
 
	//发送给客户端
	if(send(newfd,&buf,sizeof(buf),0) < 0)
	{
		ERR_MSG("send");
		return -1;
	}
	//存储单词查询的记录
	if(buf.head == 'C')
	{
		char ti[30] = "";
		time_t t = time(NULL);
		struct tm *info = NULL;
        info = localtime(&t);
		//拼接时间
		sprintf(ti,"%d-%02d-%02d %02d:%02d:%02d",info->tm_year+1900, info->tm_mon+1, info->tm_mday,info->tm_hour, info->tm_min, info->tm_sec);
 
		char sql[256] = "";
		sprintf(sql, "insert into %s values(\"%s\", \"%s\",\" %s\");",cil_buf.name,cil_buf.msg,mean,ti);
		char* errmsg = NULL;
 
		if(sqlite3_exec(db_user, sql, NULL, NULL, &errmsg) != SQLITE_OK)
		{
			fprintf(stderr, "__%d__ sqlite3_exec:%s\n", __LINE__, errmsg);
			return -1;
		}
		printf("查词[%s]记录存储成功\n",cil_buf.msg);
	}
}
//查询历史记录
int history(int newfd,struct sockaddr_in cin,struct message cil_buf,sqlite3* db_user)
{
	printf("用户[%s]请求查词历史记录\n",cil_buf.name);
	struct message buf;
	char his[128] = "";
	buf.head = 'H';
	//查找数据的内容
	char sql[128] = "select *  from stu";
	char ** pres = NULL;    //存储查询结果的首地址
	int row, column;        //查询结果的行列数
	char* errmsg = NULL;
	
	sprintf(sql,"select * from %s",cil_buf.name);
	
    if(sqlite3_get_table(db_user, sql, &pres, &row, &column, &errmsg) != SQLITE_OK)
    {
        fprintf(stderr, "__%d__ sqlite3_get_table:%s\n", __LINE__, errmsg);
        return -1;
    }
    //能运行到当前位置则说明，查询函数运行成功
 
    int i = 0;
	ssize_t res;
    for(i=0; i<(row+1)*column; i++)
    {
		strcat(his,pres[i]);
		strcat(his,"|");
        if(i%column == (column-1))
		{
			strcpy(buf.msg,his);
			if(send(newfd,&buf,sizeof(buf),0) < 0)
			{
				ERR_MSG("send");
				return -1;
			}
			bzero(his,sizeof(his));
			res = recv(newfd,&buf,sizeof(buf),0);
			if(res < 0)
			{
				ERR_MSG("recv");
				return -1;
			}
			else if(res == 0)
			{
				printf("客户端退出\n");
				return 0;
			}
			if(buf.head != 'H')
			{
				printf("客户端异常\n");
				return -1;
			}
		}
    }
	buf.head = 'F';
	if(send(newfd,&buf,sizeof(buf),0) < 0)
	{
		ERR_MSG("send");
		return -1;
	}
    //释放获取到的空间
    sqlite3_free_table(pres);
    pres = NULL;
    return 0;
}
//退出登录,修改数据库信息为N
int log_out(char* name,sqlite3* db_user)
{
	printf("用户[%s]退出登录\n",name);
	//修改用户为非在线状态
	char sql[128] = "";
	sprintf(sql, "update user set online=\"N\" where name=\"%s\";",name);
	char* errmsg = NULL;
	if(sqlite3_exec(db_user, sql, NULL, NULL, &errmsg) != SQLITE_OK)
	{
		fprintf(stderr, "__%d__ sqlite3_exec:%s\n", __LINE__, errmsg);
		return -1;
	}
	return 0;
}
