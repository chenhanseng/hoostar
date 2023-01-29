#include "net.h"

void do_register(int sockfd);
int do_login(int sockfd);
void word_search(int sockfd);
void history_search(int sockfd);

//程序使用说明函数
void usage(char *s)
{
	printf("Usage:\n\t%s [dns_name] [serv_port]\n",s);
	printf("\tdns_name: the dns_name or IP address of server. \n\tdefault is 192.168.20.152\n");
	printf("\t[serv_port]:the port value for server(serv_port > 5000) \n\tdefault is 5003\n");
}

void menu1(void)
{
	printf("\n");
	printf("--------------welcome to net dictionary-----------------\n\n");
	printf("    1:register    2:login    3:quit\n\n");
	printf("--------------------------------------------------------\n");
	fprintf(stderr,"Please select the required command:");
}

void menu2(void)
{
	printf("\n--------------------------------------------------------\n\n");
	printf("    1:Word_Search    2:Historical_Search    3:quit\n\n");
	printf("--------------------------------------------------------\n");
	fprintf(stderr,"Please select the required command:");
}



int main(int argc,char *argv[])
{
	
	int port = 0,ip = 0;
	port  = PORT;//默认端口号 5003
	ip = inet_addr(IP);//默认IP 192.168.20.152
	int sockfd = -1;
	struct hostent *host = NULL;//定义一个结构体指针 指向域名的结构体

	//<优化 将IP和端口号改为通过参数传输>
	if(argc != 1 && argc != 3)//使用错误 提示并退出
	{
		usage(argv[0]);
		exit(1);
	}
	
	if(argc == 3){

		//端口号处理
		int temp = -1;
		temp = atoi(argv[2]);//调用atoi函数将字符串转换为数字 即将我们自己指定的端口号进行转换
		if(temp < 5000){//如果输入端口号小于5000 判定为不合法 退出并提示用法
			usage(argv[0]);
			exit(1);
		}
		port = temp;
		
		/*服务器的域名解析 通过域名可以解析到对应IP*/
		host = gethostbyname(argv[1]);//调用此函数实现域名的解析 并将解析到的网络字节序IP存入结构体 hostent中
		if(!host)//解析出错则退出
		{
			herror("gethostbyname");//类似于perror 是netdb.h提供的错误信息打印函数
			exit(1);
		}
	}
	
	//1.创建socket文件套接字
	if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
	perror("socket");
	exit(1);
	}
	
	//2.连接服务器 connect()
	struct sockaddr_in sin;//定义一个网络通信结构体
	bzero(&sin,sizeof(sin));//清空结构体
	sin.sin_family = AF_INET;//通信协议的类型 IPV4
	sin.sin_port = htons(port);//端口号为PORT 同时将PORT转换为网络字节序
	if(argc == 1){//只有一个参数 即不指定域名或IP 则使用默认IP地址
		sin.sin_addr.s_addr = ip;//IP地址转换
		printf("Use default IP and PORT(%s:%d)\n",IP,PORT);
	}
	
	if(argc == 3){//指定了域名或IP 则使用域名解析的IP地址
		memcpy(&sin.sin_addr, host->h_addr, sizeof(sin.sin_addr));
		endhostent();//释放host内存
		host = NULL;//防止产生野指针
	}
	
	if(connect(sockfd, (struct sockaddr *)&sin ,sizeof(sin) ) < 0){
	perror("connect");
	exit(1);
	}
	printf("Client connect to server success!\n");
	
	
	while(1)//菜单1
	{
		menu1();
		char COMMAND[BUFSIZ];
		bzero(COMMAND,BUFSIZ);
		int ret;
		ret = read(0,COMMAND,BUFSIZ);//读取命令
		if(ret < 0){
			perror("read\n");
			exit(1);
		}
		printf("\n");
		if(ret > 2)//正常情况下键盘输入1 读取到的有两个字符‘1’和字符串结束表示符‘\0’
		{
			printf("请按提示信息输入：1  2  3\n");
			continue;
		}
		
		ret = write(sockfd,COMMAND,1);//发送命令
		if(ret < 0){
			perror("write\n");
			exit(1);
		}
		
		 switch(COMMAND[0])
		{
			case '1':
				do_register(sockfd);
				break;
			case '2':
			{
				if(do_login(sockfd) == 1)
					goto NEXT;
			}
				break;
			case '3':
				close(sockfd);
				return 0;
				break;
			default:
				printf("请按提示信息输入：1  2  3\n");
		}
		
	}
	
	NEXT:
	while(1)//菜单2
	{
		menu2();
		int ret = -1;
		char CHOOSE[BUFSIZ];
		bzero(CHOOSE,BUFSIZ);
		scanf("%s",CHOOSE);
		printf("\n");
		if(strlen(CHOOSE) > 1)//处理输入错误的情况 strlen 计算大小不包含‘\0’
		{
			printf("请按提示信息输入：1  2  3\n");
			continue;
		}
		
		ret = write(sockfd,CHOOSE,1);//将读取的命令发送到服务器
		if(ret < 0){
		perror("write\n");
		exit(1);
		}
	
		switch(CHOOSE[0])
		{
			case '1'://单词查询
				word_search(sockfd);
				break;
			
			case '2':
				history_search(sockfd);
				break;
			
			case '3':
			{
				close(sockfd);
				return 0;
			}
				break;
			default:
				printf("请按提示信息输入：1  2  3\n");
				
		}
	}
	
	close(sockfd);
	return 0;
}

//账号注册函数
void do_register(int sockfd)
{
	while(1){
	char name[100],key[100];
	bzero(name,100);
	bzero(key,100);
	int ret =-1;
	
	printf("请输入账号：\n");
	scanf("%s",name);
	ret = write(sockfd,name,100);
	if(ret < 0){
	perror("write\n");
	exit(1);
	}
	if(strlen(name) > 20)
	{
		printf("账号名长度需要小于20位！\n");
		continue;
	}
	
	printf("请输入密码：\n");
	scanf("%s",key);
	ret = write(sockfd,key,100);
	if(ret < 0){
	perror("write\n");
	exit(1);
	}
	if(strlen(key) > 20)
	{
		printf("密码长度需要小于20位！\n");
		continue;
	}
	printf("账号注册成功！\n");
	break;
	}
}

//账户登录函数
int do_login(int sockfd)
{
	char name[100],key[100];
	bzero(name,100);
	bzero(key,100);
	int ret = -1;
	
	//开始登录
	printf("请输入账号：\n");
	scanf("%s",name);
	ret = write(sockfd,name,100);
	if(ret < 0){
	perror("write\n");
	exit(1);
	}
	
	printf("请输入密码：\n");
	scanf("%s",key);
	ret = write(sockfd,key,100);
	if(ret < 0)
	{
		close(sockfd);
		perror("write\n");
		exit(1);
	}	
	
	char FLAG = '0';
	char *flag = &FLAG;
	ret = -1;
	ret = read(sockfd,flag,1);//等待服务器回应 若返回1则登录成功 若返回0则登录失败
	if(ret < 0)
	{
		close(sockfd);
		perror("raed\n");
		exit(1);
	}
	
	if(FLAG == '0')
	{
		printf("登录失败，未查找到用户！\n");
		return 0;		
	}
	
	if(FLAG == '1')
	{
		printf("登录成功！\n");
		return 1;
	}
}

//单词查找函数
void word_search(int sockfd)
{
	while(1)
	{
		char word[BUFSIZ];
		bzero(word,BUFSIZ);
		printf("\n请输入需要查询的单词(输入‘##’退出查询):");
		scanf("%s",word);
		int ret =-1;
		
		//将输入的单词上传到服务器查找
		ret = write(sockfd,word,BUFSIZ-1);//将读取的命令发送到服务器
		if(ret < 0){
		perror("write\n");
		exit(1);
		}
		if(!strncasecmp(word,"##",strlen("##"))){//输入“##”返回上一级菜单
			printf("已退出查询\n\n");
			break;
		}
		
		//等待服务器查询结果
		char explain[BUFSIZ];
		bzero(explain,BUFSIZ);
		ret = read(sockfd,explain,BUFSIZ-1);//将读取的命令发送到服务器
		if(ret < 0){
		perror("write\n");
		exit(1);
		}
		printf("%s\n",explain);
		
		
	}
}

//历史记录查询函数
void history_search(int sockfd)
{
	short count =  0;
	int ret = -1;
	ret = read(sockfd,&count,sizeof(&count));
	if(ret < 0)
	{
		perror("recv");
		close(sockfd);
		exit(1);
	}
	ntohs(count);
	printf("共查询到%d条记录：\n",count);
	if(count == 0)//记录为空则函数返回
		return;
	
	char history[500];
	bzero(history,500);
	ret = -1;
	while(1){//读取服务器返回的历史记录
	
	bzero(history,500);
	ret = read(sockfd,history,sizeof(history));
	printf("%s\n",history);
	if(ret < 0)
	{
		perror("recv");
		close(sockfd);
		exit(1);
	}
	else 
		break;
	}
}



