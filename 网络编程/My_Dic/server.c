#include "net.h"

//函数声明
int do_login(char *name,int sockfd,int newfd,sqlite3* db);
void do_register(int sockfd,int newfd,sqlite3* db);
void word_search(char *name,int sockfd,int newfd,sqlite3* db);
void history_search(int sockfd,int newfd,char *name,sqlite3* db);

//程序使用说明函数
void usage(char *s)
{
	printf("Usage:\n\t%s [serv_port]\n",s);
	printf("\t[serv_port]:the port value for server(serv_port > 5000) \n\tdefault is 5003\n");
}

//回调函数 用于历史记录查询时向客户端发送查询结果
int history_callback(void* arg,int f_num,char** f_value,char** f_name)
{
 // record  , name  , date  , word 
 int newfd;
 int ret;
 char history[500];
 bzero(history,500);

 newfd = *((int *)arg);

 sprintf(history, "%s,%s\n", f_value[1], f_value[2]);

 ret = write(newfd, history, strlen(history));
 if(ret < 0)
 {
	 perror("write");
	 exit(1);
 }

 return 0;
}

//获取当前时间函数
void time_get(char * time_value)
{
	time_t ls = time(NULL);
	struct tm *lt = localtime(&ls);//定义一个结构体指针接收转化为本地时间
	sprintf(time_value,"%d-%02d-%02d %02d:%02d:%02d\n",lt->tm_year+1900,lt->tm_mon+1,lt->tm_mday,lt->tm_hour,lt->tm_min,lt->tm_sec);
	
}

int main(int argc,char *argv[])
{
	int port = PORT;
	//<优化2 将端口号改为通过参数传输>
	if(argc != 1 && argc != 2)
	{
		usage(argv[0]);//程序使用格式错误 提示用法
		exit(1);
	}
	if(argc == 2)
	{
		int temp = -1;
		temp = atoi(argv[1]);//调用atoi函数将字符串转换为数字 即将我们自己指定的端口号进行转换
		if(temp < 5000){//如果输入端口号小于5000 判定为不合法 退出并提示用法
			usage(argv[0]);
			exit(1);
		}
		port = temp;
	}
	/*end <优化2> */
	int sockfd = -1;

	//1.创建socket文件套接字
	if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
	perror("socket");
	exit(1);
	}
	
	
	/*对SIGCHLD信号做处理，使得fork()时，父进程不需要wait()/waitpid() */
        struct sigaction act;
        act.sa_handler = SIG_DFL;
        sigemptyset(&act.sa_mask);
        act.sa_flags = SA_RESTART|SA_NOCLDWAIT;  /* If signum is SIGCHLD, do not transform children into zombies when they terminate.*/
        sigaction(SIGCHLD, &act,NULL);
	
	
	//<优化3 允许本地的地址和端口快速重用（设置在socket()之后bind()之前）>
	//掉电之后可以立刻重启服务器 保证通信安全
   int reuse_on = 1;
   setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &reuse_on, sizeof(int));
	/* end <优化3> */
	
	//2.绑定IP地址和端口号
	struct sockaddr_in sin;//定义一个网络通信结构体
	bzero(&sin,sizeof(sin));//清空结构体
	sin.sin_family = AF_INET;//通信协议的类型 IPV4
	sin.sin_port = htons(port);//端口号为port 同时将port转换为网络字节序
	
	//<优化1>IP地址绑定为本地任意IP
	sin.sin_addr.s_addr = htonl(INADDR_ANY);//IP地址转换

	if(bind(sockfd, (struct sockaddr *)&sin ,sizeof(sin) ) < 0){
	perror("bind");
	exit(1);
	}
	
	//3.把sockfd用主动变为被动 同时允许2*backlog+1路客户端进行链接
	if(listen(sockfd,BACKLOG) < 0){
		perror("listen!\n");
		exit(1);
	}
	printf("Server staring....ok!\n");
	
	//创建数据库
	sqlite3* db = NULL;
	int ret = -1;
	ret = sqlite3_open("./Dictionary.db",&db);
	if(ret != SQLITE_OK)
	{
		fprintf(stderr,"open databases filed!\n");
		exit(0);
	}
	
   	//创建账户表 和 历史记录表
	char* errmsg = NULL;
	ret = sqlite3_exec(db,"create table if not exists account_info(key char[100],name char[100]);",NULL,NULL,&errmsg);	
	if(ret != SQLITE_OK)
	{
		fprintf(stderr,"create table account_info failed  errno:%d, erronr reason :%s\n",ret,errmsg);
		exit(0);
	}

	ret = sqlite3_exec(db,"create table if not exists history_info(name char[100],word char[100],time char[50]);",NULL,NULL,&errmsg);	
	if(ret != SQLITE_OK)
	{
		fprintf(stderr,"create table history_info failed  errno:%d, erronr reason :%s\n",ret,errmsg);
		exit(0);
	}
	
	struct sockaddr_in cin;//定义一个网络通信结构体 保存链接过来的客户端信息 IP 端口号等
	socklen_t addrlen = sizeof(cin);//结构体的长度
	int newfd = -1;
	int pid = -1;
	
	while(1){
	newfd = accept(sockfd,(struct sockaddr *)&cin,&addrlen);//阻塞等待客户端链接 获取新的文件描述符
	if(newfd < 0)
		continue;
	else
	{
		//当有新的客户端连接成功 则创建一个子进程 由子进程充当服务器进行通信
		if(pid < 0)//创建子进程失败
		{
			perror("fork");
			exit(1);
		}
		
		if(pid == 0)
		{
		printf("子进程：%d\n",getpid());//子进程打印自己的PID 子进程执行后面的语句 父进程阻塞等待新的客户端连接
		break;
		}
		
		if(pid > 0)
		{
			close(newfd);
		}
	}
	}
	
	
	/*子进程代码段*/
	//客户端信息处理 获取客户端IP、端口
	char cli_ip[16];
	bzero(cli_ip,16);
	inet_ntop(AF_INET,(void *)&cin.sin_addr,cli_ip,sizeof(cin));//IP号转换
	
	int cli_port;
	cli_port = ntohs(cin.sin_port);//端口号转换
	
	printf("client(%s:%d)connect success!\n\n",cli_ip,cli_port);

	char buf[BUFSIZ], name[100];
	
	while(1){//菜单一
		bzero(buf,BUFSIZ);//清空数组
		bzero(name,100);
		do{
		ret = read(newfd,buf,1);//阻塞等待客户端发送命令 1，注册 2，登录 3，退出
		
		}while(ret < 0 && EINTR == errno);
	
		if(!ret)
			break;//客户端已退出
	
		if(ret < 0)
		{
			perror("server:read()");
			exit(1);
		}
	
		switch(buf[0])
		{
			case '1':
				do_register(sockfd,newfd,db);
				break;
			
			case '2':
			{
				if(do_login(name,sockfd,newfd,db) == 1)//登录函数 登陆成功返回1 失败返回0
					goto NEXT;
			}
				break;
			
			case '3':
				{
				
				sqlite3_close(db);
				close(sockfd);
				close(newfd);
				return 0;
				}
				break;
		}
	
	}
	
	NEXT:
	while(1)//菜单二
	{
		int RET = -1;
		//登录成功后从这里执行 等待客户端发送新的命令
		char CHOOSE = '0';
		char *choose = &CHOOSE;
		do{
		RET = read(newfd,choose,1);//阻塞等待客户端发送命令 1 2 3 
		}while(RET < 0 && EINTR == errno);
		if(RET < 0)
		{
			perror("server:read()");
			exit(1);
		}
		hoostar_dbg("Client(%s:%d) said:%s\n",cli_ip,cli_port,choose);//服务器打印读取的命令
		
		switch(CHOOSE)//根据接收到的命令 执行不同case
		{
			case '1':
				word_search(name,sockfd,newfd,db);
				break;
			
			case '2':
				history_search(sockfd,newfd,name,db);
				break;
				
			case '3':
			{
				sqlite3_close(db);
				close(sockfd);
				close(newfd);
				return 0;
			}
				break;
		}
	}
	
	close(sockfd);
	close(newfd);
	return 0;
}


//账号注册函数
void do_register(int sockfd,int newfd,sqlite3* db)
{
	while(1){
	char name[100],key[100];
	bzero(name,100);
	bzero(key,100);
	int ret = -1;
	char *errmsg;
	
	ret = read(newfd,name,100);
	if(ret < 0){
	perror("read\n");
	sqlite3_close(db);
	close(sockfd);
	close(newfd);
	exit(1);
	}
	hoostar_dbg("%s\n",name);//调试信息
	if(strlen(name) > 20)
	{
		hoostar_dbg("名字太长\n");
		continue;
	}
	
	ret = read(newfd,key,100);
	if(ret < 0){
	perror("write\n");
	sqlite3_close(db);
	close(sockfd);
	close(newfd);
	exit(1);
	}
	hoostar_dbg("%s\n",key);
	if(strlen(key) > 20)
	{
		hoostar_dbg("密码太长\n");
		continue;
	}

	//将用户信息插入用户表中
	char sql[100]={0};
	sprintf(sql,"insert into account_info values(%s,'%s');",key,name);
	ret = sqlite3_exec(db,sql,NULL,NULL,&errmsg);
	if(ret != SQLITE_OK)
	{
		fprintf(stderr,"insert table account_info falied  errno:%d, errnor reason :%s\n",ret,errmsg);
		perror("write\n");
		sqlite3_close(db);
		close(sockfd);
		close(newfd);
		exit(1);
	}else
		hoostar_dbg("插入数据成功！\n");
		
	break;
	}
}

//账户登录函数
int do_login(char *name,int sockfd,int newfd,sqlite3* db)
{
	int RET = -1;
	char key[100];
	bzero(name,100);
	bzero(key,100);
	char *errmsg;
	
	RET = read(newfd,name,100);
	if(RET < 0){
	perror("read\n");
	sqlite3_close(db);
	close(sockfd);
	close(newfd);
	exit(1);
	}
	hoostar_dbg("%s\n",name);//调试信息
	
	RET = read(newfd,key,100);
	if(RET < 0){
	perror("write\n");
	sqlite3_close(db);
	close(sockfd);
	close(newfd);
	exit(1);
	}
	hoostar_dbg("%s\n",key);
	
	//在账户表中查询账户信息
	char str[100];
	bzero(str,100);
	RET = -1;
	int row = 0;//表示查询到的记录条数
	int column;
	char ** result = NULL;
	
	sprintf(str,"select * from account_info where name='%s' and key='%s';",name,key);
	RET = sqlite3_get_table(db,str,&result,&row,&column,&errmsg);//判断查询到的结果 如果row为1则表示查询成功
	hoostar_dbg("row:%d\n",row);
	
	if(RET != SQLITE_OK)
	{
		
		exit(0);
			
	}
	
	if(row == 0)//登录失败
	{
	char FLAG_F = '0';
	hoostar_dbg("No user found！\n");
	RET = write(newfd,&FLAG_F,1);
	if(RET < 0){
	perror("write\n");
	sqlite3_close(db);
	close(sockfd);
	close(newfd);
	RET = -1;
	exit(1);
		}
	return 0;
	}
	
	if(row > 0)//登陆成功
	{
		hoostar_dbg("success!\n");
		char FLAG_S = '1';
		RET = write(newfd,&FLAG_S,1);
		if(RET < 0){
		perror("write\n");
		sqlite3_close(db);
		close(sockfd);
		close(newfd);
		RET = -1;
		exit(1);
		}
		return 1;
	}
}

//单词查找函数
void word_search(char *name,int sockfd,int newfd,sqlite3* db)
{
	char word[BUFSIZ];
	bzero(word,BUFSIZ);
	char record[BUFSIZ];
	bzero(record,BUFSIZ);
	char time_value[50];
	bzero(time_value,50);
	char sql[100]={0};
	int FOUND_FLAG = 0;
	char *f = NULL;
	int RET = -1;
	int ret = -1;
	char *errmsg;

	while(1){//接收客户端传来的单词
		RET = read(newfd,word,BUFSIZ-1);//从客户端读取单词到 word数组
		if(RET < 0){
		perror("write\n");
		exit(1);
		}
		hoostar_dbg("word:%s\n",word);
		if(!strncasecmp(word,"##",strlen("##"))){//读取到“##”返回上一级菜单
			break;
		}
		
		//将查询记录插入到历史记录表中
		bzero(time_value,50);
		time_get(time_value);//获取时间 函数定义在mian前
		bzero(sql,100);
		sprintf(sql,"insert into history_info values('%s','%s','%s');",name,word,time_value);
		ret = sqlite3_exec(db,sql,NULL,NULL,&errmsg);//插入history_info表中
		if(ret != SQLITE_OK)
		{
		fprintf(stderr," insert table failed! errno:%d,erronr reson :%s\n",ret,errmsg);
		sqlite3_close(db);
		close(sockfd);
		close(newfd);
		exit(0);
		}
		
		FILE* fr = fopen("./dict.txt","r");//打开单词表文件
		if(fr == NULL)//打开文件失败
		{
			perror("fopen");
			sqlite3_close(db);
			close(sockfd);
			close(newfd);
			exit(1);
		}
		
		while(!feof(fr) && !ferror(fr))//文件未到达末尾 且文件未出错
		{	
			FOUND_FLAG = 0;//FOUND_FLAG为0表示未查询到单词 为1表示查询到单词
			bzero(record,BUFSIZ);
			f = fgets(record,BUFSIZ-1,fr);//按行循环读取数据到 record数组
			
			if(!strncmp(word,record,strlen(word)))//比较输入的单词与单词表当前行是否匹配
			{
				//为防止出现word = int  record = intter....时 把当前行错误的当成匹配行
				//所以匹配完strlen(word)长度的字符后 再判断record(strlen(word))后一个字符是否为空格
				//若为空格表示当前行单词是匹配的跳出循环 否则表示不匹配 继续读取下一行
															
				if(record[strlen(word)+1] == ' ')
				{
					FOUND_FLAG = 1;
					break;
				}else 
					FOUND_FLAG = 0;
			}
		
		}
		
		if(FOUND_FLAG == 1)//查询到输入的单词
			{
				ret = write(newfd,record,BUFSIZ-1);//将匹配到的词条发送到客户端
				if(ret < 0){
				fclose(fr);		
				sqlite3_close(db);
				close(sockfd);
				close(newfd);
				perror("write\n");
				exit(1);
				}
			}
			else
			{
				ret = write(newfd,"NO SEARCHED！",BUFSIZ-1);//发送错误提示
				if(ret < 0){
				fclose(fr);
				sqlite3_close(db);
				close(sockfd);
				close(newfd);
				perror("write\n");
				exit(1);
				}
			}
		fclose(fr);// 关闭文件
	}//while 接收单词
}

//历史记录查询函数
void history_search(int sockfd,int newfd,char *name,sqlite3* db)
{
	char str[100];
	bzero(str,100);
	int ret = -1;
	int row = 0;//表示查询到的记录条数
	int column;
	char ** result = NULL;
	char *errmsg = NULL;

	sprintf(str,"select * from history_info where name='%s';",name);
	ret = sqlite3_get_table(db,str,&result,&row,&column,&errmsg);//获取查询结果 row为查询到的记录条数
	hoostar_dbg("row:%d\n",row);
	htons((uint16_t)row);								
	ret = write(newfd,&row,sizeof(&row));
	if(ret < 0)
	{
		perror("write");
		exit(1);
	}
	if(row == 0)//记录为空则函数返回
		return;
	
	char sql[100];
	bzero(sql,100);
	sprintf(sql,"select * from history_info where name = '%s';",name);//查询语句以及条件
	ret = sqlite3_exec(db,sql,history_callback,&newfd,&errmsg);//查询调用回调函数 到回调函数中返回历史记录到客户端
	if(ret != SQLITE_OK){
		fprintf(stderr," select failed! errno:%d,erronr reson :%s\n",ret,errmsg);
		sqlite3_close(db);
		close(sockfd);
		close(newfd);
		exit(0);
	}
}














