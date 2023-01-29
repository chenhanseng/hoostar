#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <string.h>
 
//打印错误新的宏函数
#define ERR_MSG(msg)  do{\
    fprintf(stderr, " __%d__ ", __LINE__);\
    perror(msg);\
}while(0)
//发送接收结构体
struct message        
{
	char head;
	char msg[128];
	char name[50];
};
 
//指定要连接的服务器的地址信息
#define PORT 8888
#define IP  "192.168.20.132"
 
void menu(int n);
int sign_up(int sfd);
int sign_in(int sfd);
int word(int sfd,char* user);
int history(int sfd,char* user);
int cil_search(int sfd,char* user);
int log_out(int sfd,char* user);
 
int main(int argc, const char *argv[])
{

	system("clear");
	//创建流式套接字
	int sfd = socket(AF_INET, SOCK_STREAM, 0);
	if(sfd < 0)
	{
		ERR_MSG("socket");
		return -1;
	}
 
	//填充要连接的服务器的地址信息结构体
	struct sockaddr_in sin;
	sin.sin_family 		= AF_INET;
	sin.sin_port 		= htons(PORT);
	sin.sin_addr.s_addr = inet_addr(IP);
 
	//连接服务器 connect
	if(connect(sfd, (struct sockaddr*)&sin, sizeof(sin)) < 0)
	{
		ERR_MSG("connect");
		return -1;
	}	
	printf("连接服务器成功\n");
	struct message buf;
	int choose;
	int num;
	while(1)
	{
		menu(1);
		scanf("%d",&choose);
		while(getchar() != 10);
		switch(choose)
		{
		case 1:
			//注册
			sign_up(sfd);
			break;
		case 2:
			//登录
			num = sign_in(sfd);
			break;
		case 3:
			//退出
			system("clear");
			exit(0);
		default:
			printf("输入错误\n");
		}
	}
	return 0;
}
//菜单函数
void menu(int n)
{
	if(1 == n)
	{
		printf("----------------\n");
		printf("-----1.注册-----\n");
		printf("-----2.登录-----\n");
		printf("-----3.退出-----\n");
		printf("----------------\n");
	}
	else if(2 == n)
	{
		printf("-----------------\n");
		printf("---1.查单词------\n");
		printf("---2.历史记录----\n");
		printf("---3.返回上一级--\n");
		printf("-----------------\n");
 
	}
}
//注册用户
int sign_up(int sfd)
{
	system("clear");
	struct message buf;
	buf.head = 'Z';
	char user[50] = "";
	char password[50] = "";
 
	printf("请输入要注册的用户名user>>>");
	scanf("%s",user);
	printf("请输入密码password>>>");
	scanf("%s",password);
 
	sprintf(buf.msg,"%s%c%s",user,0,password);
 
	if(send(sfd,&buf,sizeof(buf),0) <0)
	{
		ERR_MSG("send");
		return -1;
	}
	if(recv(sfd,&buf,sizeof(buf),0) < 0)//接收服务器的信息
	{
		ERR_MSG("recv");
		return -1;
	}
	if(buf.head == 'Z')//'Z'注册成功
	{
		printf("%s",buf.msg);
		printf("您的用户名:%s\n",user);
		printf("您的密码:%s\n",password);
	}
	else if(buf.head == 'F')//'F'没有注册成功
	{
		printf("%s",buf.msg);
	}
	return 0;
}
//用户登录
int sign_in(int sfd)
{
	system("clear");
	struct message buf;
	buf.head = 'D';
	char user[50] = "";
	char password[50] = "";
	printf("请输入用户名user>>>");
	scanf("%s",user);
	printf("请输入密码password>>>");
 
	scanf("%s",password);
	sprintf(buf.msg,"%s%c%s",user,0,password);
	if(send(sfd,&buf,sizeof(buf),0) <0)
	{
		ERR_MSG("send");
		return -1;
	}
	if(recv(sfd,&buf,sizeof(buf),0) < 0)
	{
		ERR_MSG("recv");
		return -1;
	}
	if(buf.head == 'D')//'D'登录成功
	{
		printf("%s",buf.msg);
	}
	else if(buf.head == 'F')//'F'登录失败
	{
		printf("%s",buf.msg);
		return -1;//密码错误,用户名或者用户已在线不存在
	}
	word(sfd,user);//调用查词函数
 
	return 0;
}
//查词历史记录
int word(int sfd,char* user)
{
	int choose;
	while(1)
	{
		menu(2);
		scanf("%d",&choose);
		while(getchar()!=10);
		switch(choose)
		{
		case 1:
			cil_search(sfd,user);//查词
			break;
		case 2:
			history(sfd,user);//查看历史记录
			break;
		case 3:
			log_out(sfd,user);//回到上一级
			return 0;
		default:
			printf("输入错误\n");
		}
	}
}
//查询单词
int cil_search(int sfd,char* user)
{
	system("clear");
	struct message buf;
	char str[128] = "";
	buf.head = 'C';
 
	while(1)
	{
		strcpy(buf.name,user);
		buf.head = 'C';
		printf("请输入单词(输入#结束查词)>>>");
		fgets(str,sizeof(str),stdin);
		str[strlen(str)-1] = 0;
		if(str[0] == '#')//退出查词
		{
			system("clear");
			break;
		}
		strcpy(buf.msg,str);
		if(send(sfd,&buf,sizeof(buf),0) < 0)
		{
			ERR_MSG("send");
			return -1;
		}
		if(recv(sfd,&buf,sizeof(buf),0) < 0)
		{
			ERR_MSG("recv");
			return -1;
		}
		if(buf.head == 'F')//F查词失败
		{
			printf("单词[%s]%s",str,buf.msg);
		}
		else if(buf.head == 'C')//C查词成功
		{
			printf(" 单词:[%s]\t 解释:[%s]\n",str,buf.msg);
		}
		else
		{
			printf("服务器异常");
			break;
		}
	}
}
//历史记录
int history(int sfd,char* user)
{
	system("clear");
	int n = 0;
	struct message buf;
	char* q = buf.msg;
	strcpy(buf.name,user);
 
	buf.head = 'H';
	if(send(sfd,&buf,sizeof(buf),0) < 0)
	{
		ERR_MSG("send");
		return -1;
	}
	while(1)
	{
		strcpy(buf.name,user);
		q = buf.msg;
		if(recv(sfd,&buf,sizeof(buf),0) < 0)//接服务器的信息
		{
			ERR_MSG("recv");
			return -1;
		}
		if(buf.head == 'F')//接收完毕
		{
			if(n == 1)//n被修改为1,查过词
			{
				printf("查询完毕\n");
			}
			else 
				printf("你还没有查过词\n");
			break;
		}
		else
		{
			while(*q != '\0')//一个一个字符打印
			{
				if(*q != '|')
				{
					printf("%c",*q);
				}
				else
				{
					printf("\t\t");//遇到|打印\t
				}
				q++;
				n = 1;
			}
			printf("\n");
		}
		if(send(sfd,&buf,sizeof(buf),0) < 0)//发送ASK验证
		{
			ERR_MSG("send");
			return -1;
		}
	}
}
//退出登录
int log_out(int sfd,char* user)
{
	struct message buf;
	strcpy(buf.msg,user);
	//向服务器发送退出登录信息
	buf.head = 'Q';
	if(send(sfd,&buf,sizeof(buf),0) < 0)
	{
		ERR_MSG("send");
		return -1;
	}
	
	system("clear");
	return 0;
}
