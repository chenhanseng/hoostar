#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
typedef struct data{
	char cmd;
	char name[11];
	char password[7];
}datatype;

/*将字符串temp内容 复制到字符数组buf 第a到b的位置*/
void WriteInBuf(char* buf,char *temp,int a,int b)
{
	int i , j = 0;
	for(i = a; i <= b; i++){
		if(temp[j] == 0)break;
		buf[i] = temp[j];
		j++;
	}
}
void buf_prcs(char *buf,datatype DATA)
{
	buf[0]=DATA.cmd;
	printf("%c\n",buf[0]);
	WriteInBuf(buf,DATA.name,1,11);
	WriteInBuf(buf,DATA.password,12,18);
}
int main(void)
{
	datatype DATA;
	char buf[20] ;
	bzero(buf,sizeof(buf));
	DATA.cmd = 'A';

	char name[11] = "zhang san";
	char password[7] = "123456";
	
	strcpy(DATA.name,name);
	strcpy(DATA.password,password);
	
	buf_prcs(buf,DATA);
	printf("%s\n",buf);
	for(int i =  0; i < sizeof(buf); i++){
	printf("%c",buf[i]);
	}
	printf("\n");
	puts(buf);
	return 0;
}
