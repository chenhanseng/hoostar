#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>          
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <sqlite3.h>
#include <signal.h>
#include <time.h>
#include <sys/wait.h>
#include <errno.h>

#include <unistd.h>

#define   N  32

#define  R  1   // user - register
#define  L  2   // user - login
#define  Q  3   // user - query
#define  H  4   // user - history

#define KU  "my.db"

// 定义通信双方的信息结构体
typedef struct {
	int type;
	char name[N];
	char data[256];
}MSG;




int do_client(int acceptfd, sqlite3 *db);
void do_register(int acceptfd, MSG *msg, sqlite3 *db);
int do_login(int acceptfd, MSG *msg, sqlite3 *db);
int do_query(int acceptfd, MSG *msg, sqlite3 *db);
int do_history(int acceptfd, MSG *msg, sqlite3 *db);
int history_callback(void* arg,int f_num,char** f_value,char** f_name);
int do_searchword(int acceptfd, MSG *msg, char word[]);
int get_date(char *date);


void sig_child_handle(int signo)
{
	if(SIGCHLD == signo)
		waitpid(-1, NULL,  WNOHANG);
}

//主函数
int main(int argc, const char *argv[])
{

	int sockfd;
	int n;
	MSG  msg;
	sqlite3 *db;
	int acceptfd;
	pid_t pid;
	char *errmsg;

	if(argc != 3)
	{
		printf("Usage:%s 服务器ip 端口号\n", argv[0]);
		return -1;
	}
	//打开数据库
	if(sqlite3_open(KU, &db) != SQLITE_OK)
	{
		printf("%s\n", sqlite3_errmsg(db));
		return -1;
	}
	else
        printf("数据库打开成功\n");

	if(sqlite3_exec(db, 
				"create table if not exists usr(name text primary key,pass text);",
				//primary key主关键字只能有一个
				NULL, NULL, &errmsg) != SQLITE_OK)
	{
		printf("%s\n", errmsg);
	}
	else
        printf("用户数据库创建成功\n");

	if(sqlite3_exec(db, 
				"create table if not exists record(name text ,data text,word text);",
				NULL, NULL, &errmsg) != SQLITE_OK)
	{
		printf("%s\n", errmsg);
	}
	else
        printf("历史记录启动成功\n");

	if((sockfd = socket(AF_INET, SOCK_STREAM,0)) < 0)
	{
		perror("socket");
		return -1;
	}

	struct sockaddr_in  serveraddr;
	bzero(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = inet_addr(argv[1]);
	serveraddr.sin_port = htons(atoi(argv[2]));

	if(bind(sockfd, (struct sockaddr *)&serveraddr, sizeof(serveraddr)) < 0)
	{
		perror("bind");
		return -1;
	}

	// 套接字
	if(listen(sockfd, 5) < 0)
	{
		perror("listen");
		return -1;
	}

	//处理僵尸进程
	signal(SIGCHLD,sig_child_handle);

	while(1)
	{
		if((acceptfd = accept(sockfd, NULL, NULL)) < 0)
		{
			perror("accept");
			return -1;
		}

		if((pid = fork()) < 0)
		{
			perror("fork");
			return -1;
		}
		else if(pid == 0)  // 儿子进程
		{
			//处理客户端具体的消息
			close(sockfd);
			do_client(acceptfd, db);

		}
		else  // 父亲进程,用来接受客户端的请求的
		{
			close(acceptfd);
		}
	}

	return 0;
}


//1登陆
int do_client(int acceptfd, sqlite3 *db)
{
	MSG msg;
	while(recv(acceptfd, &msg, sizeof(msg), 0) > 0)
	{
		printf("type:%d\n", msg.type);
		switch(msg.type)
		{
		case R:
			do_register(acceptfd, &msg, db);
			break;
		case L:
			do_login(acceptfd, &msg, db);
			break;
		case Q:
			do_query(acceptfd, &msg, db);
			break;
		case H:
			do_history(acceptfd, &msg, db);
			break;
		default:
            printf("客户端输入错误\n");
		}

	}

    printf("客户端退出\n");
	close(acceptfd);
	exit(0);

	return 0;
}

//2注册
void do_register(int acceptfd, MSG *msg, sqlite3 *db)
{
	char * errmsg;
	char sql[500]={0};
	int ret =-1;

	sprintf(sql, "insert into usr values('%s', %s);", msg->name, msg->data);
    printf("注册信息已经保存至用户数据库\n");
	printf("%s\n", sql);
    puts("");

	if(sqlite3_exec(db,sql, NULL, NULL, &errmsg) != SQLITE_OK)
	{
		printf("%s\n", errmsg);
        strcpy(msg->data, "用户名已经存在！");
	}
	else
	{
        printf("客户端注册成功！\n");
		strcpy(msg->data, "注册成功！");
	}

	do{
		ret = send(acceptfd,msg,sizeof(MSG),0);
	}while(ret <0 && EINTR==errno);
	if(ret < 0)
	{
		perror("send");
		return ;
	}

	return ;
}

//3登陆
int do_login(int acceptfd, MSG *msg , sqlite3 *db)
{
	char sql[500] = {0};
	char *errmsg;
	int nrow;
	int ncloumn;
	char **resultp;

	sprintf(sql, "select * from usr where name = '%s' and pass = '%s';", msg->name, msg->data);
    printf("客户端登陆信息：\n");
	printf("%s\n", sql);
    puts("");

	if(sqlite3_get_table(db, sql, &resultp, &nrow, &ncloumn, &errmsg)!= SQLITE_OK)
	{
		printf("%s\n", errmsg);
		return -1;
	}
	else
		printf("客户端登陆成功!\n");

	// 查询成功，数据库中拥有此用户
	if(nrow == 1)
	{
		strcpy(msg->data, "OK");//OK为进入查询单词的关键字
		send(acceptfd, msg, sizeof(MSG), 0);
		return 1;
	}

	if(nrow == 0) // 密码或者用户名错误
	{
		strcpy(msg->data,"用户名或密码错误！");
		send(acceptfd, msg, sizeof(MSG), 0);
	}

	return 0;
}

//3查询
int do_query(int acceptfd, MSG *msg , sqlite3 *db)
{
	char word[64];
	int found = 0;
	char date[128] = {};
	char sql[500] = {};
	char *errmsg;

	//拿出msg结构体中，要查询的单词
	strcpy(word, msg->data);

	found = do_searchword(acceptfd, msg, word);
	printf("查询一个单词完毕\n");

	// 表示找到了单词，那么此时应该将 用户名，时间，单词，插入到历史记录表中去。
	if(found == 1)
	{
		//需要获取系统时间
		get_date(date);

		sprintf(sql, "insert into record values('%s','%s','%s')",msg->name,date, word);

		if(sqlite3_exec(db, sql, NULL, NULL, &errmsg) != SQLITE_OK)
		{
			printf("%s\n", errmsg);
			return -1;
		}
		else
		{
			printf("历史查询已经记录\n");
		}

	}
	else  //表示没有找到
		strcpy(msg->data, "没有找到这个单词！");

	// 将查询的结果，发送给客户端
	send(acceptfd, msg, sizeof(MSG), 0);

	return 0;
}


//3.1打开文件查词
int do_searchword(int acceptfd, MSG *msg, char word[])
{
	FILE * fp;
	int len = 0;
	char temp[512] = {};
	int result;
	char *p;
	//打开存储单词的文件
	if((fp = fopen("dict.txt", "r")) == NULL)
	{
		perror("fopen");
		strcpy(msg->data, "单词表未打开");
		send(acceptfd, msg, sizeof(MSG), 0);
		return -1;
	}

	//打印出，客户端要查询的单词
	len = strlen(word);
	printf("%s , len = %d\n", word, len);

	//读文件，来查询单词
	while(fgets(temp,512, fp) != NULL)
	{
		//比较文件读到的单词前len位
		// abandon  ab
		result = strncmp(temp,word,len);
		if(result < 0)
			continue;
		//如果客户端输入的与查询到的一样
		//进一步判断避免出现查询到多个结果
		//例如查询abandon，结果出现abandonment这样结果
		if(result > 0 || ((result == 0) && (temp[len]!=' ')))
			break;
		//如果执行到这里说明
		// 找到了要查询的单词
		// 接下来就是要把单词后面的解释发送出去
		p = temp + len; //  abandon   v.akdsf dafsjkj 
		//让指针移动到解释的开头位置
		while(*p == ' ')
			p++;
		//将解释填入到消息结构体中
		strcpy(msg->data, p);
		printf("查询的单词:%s\n", msg->data);

		// 注释拷贝完毕之后，应该关闭文件
		fclose(fp);
		return 1;
	}
	fclose(fp);
	return 0;
}

//3.2获得日期
int get_date(char *date)
{
	time_t t;
	struct tm *tp;

	time(&t);

	//进行时间格式转换
	tp = localtime(&t);

	sprintf(date, "%d-%d-%d %d:%d:%d", tp->tm_year + 1900, tp->tm_mon+1, tp->tm_mday, 
			tp->tm_hour, tp->tm_min , tp->tm_sec);
	printf("get date:%s\n", date);

	return 0;
}



//4.查询历史
int do_history(int acceptfd, MSG *msg, sqlite3 *db)
{
	char sql[128] = {};
	char *errmsg;

	sprintf(sql, "select * from record where name = '%s'", msg->name);
 
	//查询数据库
	if(sqlite3_exec(db, sql, history_callback,(void *)&acceptfd, &errmsg)!= SQLITE_OK)
		printf("%s\n", errmsg);
	else
		printf("历史查询完成！\n");

	// 所有的记录查询发送完毕之后，给客户端发出一个结束信息
	msg->data[0] = '\0';

	send(acceptfd, msg, sizeof(MSG), 0);

	return 0;
}


//4.1历史结果--回调函数
// 得到查询结果，并且需要将历史记录发送给客户端
int history_callback(void* arg,int f_num,char** f_value,char** f_name)
{
	// record  , name  , date  , word 
	int acceptfd;
	MSG msg;

	acceptfd = *((int *)arg);

	sprintf(msg.data, "%s , %s", f_value[1], f_value[2]);

	send(acceptfd, &msg, sizeof(MSG), 0);

	return 0;
}


