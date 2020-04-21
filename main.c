#include <stdio.h>
#include <sys/types.h>         
#include <sys/socket.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <pthread.h>
#include "font.h"
#include "getmsg.h"
#include "lcd.h"
#include "display_jpeg.h"


unsigned int  *lcd_fb_addr;
int lcd_fd;
int buton=0; 
struct LcdDevice* lcd;

void *talk(void *arg)
{
	int tcp_socket=*((int*)arg);
	char g[1024]={0};   			//请求好友列表
	printf("请输入need向服务器请求好友列表\n");
	scanf("%s",g);
	write(tcp_socket,g,strlen(g));
	
}

int main()
{
	

	//获取天气，并且显示 
	pthread_t pid1;
	pthread_create(&pid1,NULL,tianqi,NULL);

	//1.创建网络通信对象 
 	int tcp_socket = socket(AF_INET, SOCK_STREAM, 0);
		

	//2.连接服务器 
	struct sockaddr_in   addr; 
    addr.sin_family  = AF_INET;
    addr.sin_port    =  htons(9292); 
    addr.sin_addr.s_addr = inet_addr("192.168.95.8");	 
	int  ret=connect(tcp_socket,(struct sockaddr *)&addr,sizeof(addr));
		
	//获取局域网服务器消息，并且显示图片或者消息通知 
	pthread_t pid2;
	pthread_create(&pid2,NULL,get_pic,(void*)&tcp_socket);

	while(1)
	{

		printf("输入：1.向服务器请求好友列表 2.给好友发送消息聊天\n");
		int i=0;
		scanf("%d",&i);
		if(i==1)
		{
			//发送好友列表请求
			pthread_t pid3;
			pthread_create(&pid3,NULL,talk,(void*)&tcp_socket);
		}
		if(i==2)
		{

			//给局域网中的好友发送信息
			char d[1024]={0};//消息内容
			char e[1024]={0};//好友ip
			char f[4096]={0};//拼接上面两个
			printf("请分别输入给好友发送的消息内容和好友ip：\n");
			scanf("%s%s",d,e);

			sprintf(f,"%s %s",d,e);
			write(tcp_socket,f,strlen(f));
			
		}
	
		buton = touchScreen();
		if(buton == 1)
		{
			
			buton = 0;
			printf("%d\n", 1);
			//初始化Lcd  必须要初始化
			struct LcdDevice* lcd = init_lcd("/dev/fb0");
			//1.打开字体
			font *f = fontLoad("/usr/share/fonts/DroidSansFallback.ttf");

			//2.字体大小的设置
			fontSetSize(f,24);

			//3.创建一个画板（点阵图）
			bitmap *bm4 = createBitmapWithInit(400,50,4,getColor(0,0,0,0)); //取消通知

			//4.将字体写到点阵图上
			char qk[]="1";
			fontPrint(f,bm4,0,0,qk,getColor(0,0,0,0),0);

			//5.把字体框输出到LCD屏幕上
			show_font_to_lcd(lcd->mp,0,0,bm4);
		}
	}
		
	

	return 0;
	
}