#include "net.h"

// 定义通信双方的信息结构体
typedef struct {
	int type;
	char name[N];
	char data[256];
}MSG;


int do_register(int sockfd, MSG *msg);
int do_login(int sockfd, MSG *msg);
int do_login_select(int sockfd, MSG *msg);
int do_query(int sockfd, MSG *msg);
int do_history(int sockfd, MSG *msg);


int main(int argc, const char *argv[])
{

	int sockfd;
	int n;
	MSG  msg;

	if(argc != 3)
	{
		printf("Usage:%s 服务器设置的ip 端口号\n", argv[0]);
		return -1;
	}

	if((sockfd = socket(AF_INET, SOCK_STREAM,0)) < 0)
	{
		perror("socket");
	}

	struct sockaddr_in  serveraddr;
	memset(&serveraddr,0, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = inet_addr(argv[1]);
	serveraddr.sin_port = htons(atoi(argv[2]));

	if(connect(sockfd, (struct sockaddr *)&serveraddr, sizeof(serveraddr)) < 0)
	{
		perror("connect");
		return -1;
	}

	char buf[100]={};
	while(1)
	{
		printf("*****************************************************************\n");
		printf("* 1 -> 注册          2 -> 登陆              3 -> 退出           *\n");
		printf("*****************************************************************\n");
		printf("请输入上面操作对应的数字！\n");
//如果输入的不是数字 

        if(1 != scanf("%d",&n))
        {
            printf("请输入阿拉伯数字\n");
            getchar();
        }


        switch(n)
		{
			case 1:
				do_register(sockfd, &msg);
				break;
			case 2:
				if(do_login(sockfd, &msg) == 1)
				{
					if(do_login_select(sockfd, &msg) == 1)
						puts("用户注销");
					else
						puts("未知错误");
				}
				break;
			case 3:
				close(sockfd);
				return 0;
				break;
			default:
				printf("请输入：1 2 3\n");
		}
	}
	
	return 0;
}

//注册

int  do_register(int sockfd, MSG *msg)
{
	msg->type = R;

	printf("请输入注册名：\n");
	scanf("%s", msg->name);
	getchar();

	printf("请输入密码：\n");
	scanf("%s", msg->data);
	getchar();

	if(send(sockfd, msg, sizeof(MSG),0) < 0)
	{
		printf("fail to send.\n");
		return -1;
	}

	if(recv(sockfd, msg, sizeof(MSG), 0) < 0)
	{
		printf("fail to recv.\n");
		return -1;
	}

	//判断注册成功还是用户名被占用
	printf("%s\n", msg->data);

	return 0;
}

//登陆
int do_login(int sockfd, MSG *msg)
{
	msg->type = L;

	printf("请输入登陆名：\n");
	scanf("%s", msg->name);
	getchar();

	printf("请输入登录密码：\n");
	scanf("%s", msg->data);
	getchar();

	if(send(sockfd, msg, sizeof(MSG),0) < 0)
	{
		printf("fail to send.\n");
		return -1;
	}

	if(recv(sockfd, msg, sizeof(MSG), 0) < 0)
	{
		printf("fail to recv.\n");
		return -1;
	}

	if(strncmp(msg->data, "OK", 3) == 0)
	{
		printf("登陆成功!\n");
		return 1;
	}
	else 
	{
		printf("%s\n", msg->data);
	}

	return 0;
}

//登录选项
int do_login_select(int sockfd, MSG *msg)
{
	int n = -1;
	while(1)
	{
		printf("***********************************************************\n");
		printf("* 1 -> 查询单词          2 -> 查询历史          3 -> 退出 *\n");
		printf("***********************************************************\n");
		printf("请输入上面操作对应的数字！\n");

        if(1 != scanf("%d",&n))
        {
            printf("请输入阿拉伯数字\n");
            getchar();
        }

		switch(n)
		{
			case 1:
				do_query(sockfd, msg);
				break;
			case 2:
				do_history(sockfd, msg);
				break;
			case 3:
				return 1;
				break;
			default :
				printf("请输入：1 2 3\n");
		}

	}
	return 0;
}

//查询
int do_query(int sockfd, MSG *msg)
{
	msg->type = Q;

	while(1)
	{
	    printf("----------------------------------------\n");
		printf("请输入要查询的单词(输入\"#\"返回上一级):\n");
	    printf("----------------------------------------\n");
        puts("");
		scanf("%s", msg->data);
		getchar();
        
		//客户端输入#号，返回到上一级菜单
		if(strncmp(msg->data, "#", 1) == 0)
			break;

		//将要查询的单词发送给服务器
		if(send(sockfd,msg, sizeof(MSG), 0) < 0)
		{
			printf("fail to send.\n");
			return -1;
		}

		// 等待接受服务器传递回来的单词的信息
		if(recv(sockfd, msg,sizeof(MSG), 0) < 0)
		{
			printf("fail to recv.\n");
			return -1;
		}
		printf("%s\n", msg->data);
	}
	return 0;
}

//历史
int do_history(int sockfd, MSG *msg)
{

	msg->type = H;

	send(sockfd, msg, sizeof(MSG), 0);
	
	// 接收服务器，传递回来的历史记录信息
	while(1)
	{
		recv(sockfd, msg, sizeof(MSG), 0);

		if(msg->data[0] == '\0')
			break;

		//输出历史记录信息
		printf("%s\n", msg->data);
	}

	return 0;
}
