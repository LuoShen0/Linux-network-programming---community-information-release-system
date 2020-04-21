#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h> 
#include <sys/types.h>         
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <sys/time.h>
#include <strings.h>
#include <sys/types.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include "lcd.h"
#include "display_jpeg.h"
#include "common.h"
#include "font.h"
#include "getmsg.h"

#define one x>600 && x < 800 && y > 0 && y < 150    //确认按钮
#define two x>600 && x < 800 && y > 280 && y < 430   //聊天按钮

unsigned int  *lcd_fb_addr;
int lcd_fd; 
struct LcdDevice* lcd;
int but=0;

//触摸屏函数
int touchScreen()
{
	struct input_event ts;
	int x,y;
	
	int fs = open("/dev/input/event0", O_RDONLY);
	if(fs == -1)
	{
		printf("open ts failed\n");
	}
	while(1)
	{
		read(fs,&ts,sizeof(ts));
	
		if(ts.type == EV_ABS  && ts.code == ABS_X){
			x=ts.value;
		}
		
		if(ts.type == EV_ABS  && ts.code == ABS_Y){
			y=ts.value;
		}
		
		if( ts.type == EV_KEY && ts.code == BTN_TOUCH && ts.value == 0 )
		{
			/*手指离开触摸屏时刻 x，y的数值*/
			if(one)
			{
				//printf("%d\n", 1);
				return 1;
			}
			if(two)
			{
				//printf("%d\n", 2);
				return 2;
			}	
		}
	}
	close(fs);
}

//初始化Lcd函数
struct LcdDevice *init_lcd(const char *device)
{
	//申请空间
	struct LcdDevice* lcd = malloc(sizeof(struct LcdDevice));
	if(lcd == NULL)
	{
		return NULL;
	} 

	//1打开设备
	lcd->fd = open(device, O_RDWR);
	if(lcd->fd < 0)
	{
		perror("open lcd fail");
		free(lcd);
		return NULL;
	}
	
	//映射
	lcd->mp = mmap(NULL,800*480*4,PROT_READ|PROT_WRITE,MAP_SHARED,lcd->fd,0);

	return lcd;
}



//获取天气
void *tianqi(void *arg)
{

	//1.创建网络通信对象 
	int tcp_socket = socket(AF_INET, SOCK_STREAM, 0);
		if(tcp_socket < 0)
		{
			perror("");
			//return -1;
		}
		else
		{
			//printf("tcp_socket 创建成功\n");
	    }

	//2.连接服务器   
	struct sockaddr_in   addr; 
    addr.sin_family  = AF_INET;
    addr.sin_port    =  htons(80); 
    addr.sin_addr.s_addr = inet_addr("47.107.120.234");	   //获取广州天气服务器的IP地址

	int  ret=connect(tcp_socket,(struct sockaddr *)&addr,sizeof(addr));
		if(ret == 0)
		{
			//printf("连接外网服务器成功\n");
		}
		else
		{
			perror("");
			//return -1;
		}

	//获取外网免费api，显示当地天气
	//http://api.qingyunke.com/api.php?key=free&appid=0&msg=%E5%A4%A9%E6%B0%94%E5%B9%BF%E5%B7%9E
	char *http = "GET /api.php?key=free&appid=0&msg=%E5%A4%A9%E6%B0%94%E5%B9%BF%E5%B7%9E HTTP/1.1\r\nHost:api.qingyunke.com\r\n\r\n";


	//发送要求给服务器 
    ret=write(tcp_socket,http,strlen(http));
    printf("等待服务器响应\n");
	printf("ret=%d\n",ret);
	
	
	//读取去头后的数据
	char buf[4096]={0};
	
	int size=read(tcp_socket,buf,4096);
	char *s=strstr(buf,"}");
	memset(s-1,0,2);			//去除后面多余字符串

	char *p=strstr(buf,"[");  //去除前面多余字符串
	printf("p=%s\n",p);
		
	//初始化Lcd  必须要初始化
	struct LcdDevice* lcd = init_lcd("/dev/fb0");
	//1.打开字体	
	font *f = fontLoad("/usr/share/fonts/DroidSansFallback.ttf");
	  
	//2.字体大小的设置
	fontSetSize(f,24);
	
	//3.创建一个画板（点阵图）
	bitmap *bm = createBitmapWithInit(800,25,4,getColor(0,0,0,0)); 		  //天气框（底部）
	bitmap *bm1 = createBitmapWithInit(200,150,4,getColor(0,255,255,0));  //聊天框（左下）
	bitmap *bm2 = createBitmapWithInit(200,150,4,getColor(0,255,255,0));  //确认框（左上）
	
	char buf1[] = "进入聊天";
	char buf2[] = "确认消息";

	//4.将字体写到点阵图上
	fontPrint(f,bm,0,0,p,getColor(0,0,0,255),0);
	fontPrint(f,bm1,0,0,buf1,getColor(0,0,0,255),0);
	fontPrint(f,bm2,0,0,buf2,getColor(0,0,0,255),0);
	

	//5.把字体框输出到LCD屏幕上
	show_font_to_lcd(lcd->mp,0,455,bm);   //天气框（底部）
	show_font_to_lcd(lcd->mp,600,280,bm1);//聊天框（左下）
	show_font_to_lcd(lcd->mp,600,0,bm2);  //确认框（左上）

	while(1);
	
}


//从局域网服务器接收消息，判断消息类型，选择显示通知还是图片
void *get_pic(void *arg)
{

	int tcp_socket=*((int*)arg);
	int xx=0,yy=120;
	while(1)
	{
		
		//接收局域网服务器的消息
		char a[1024]={0};//消息内容
		char b[1024]={0};//消息类型:txt为通知消息，pic为图片
		char c[4096]={0};//拼接上面两个

		int ret=read(tcp_socket,c,4096);
		printf("c里面的东西：%s\n", c);
		sscanf(c,"%s %s",a,b);   //切割出来
		printf("切割之后的c里面的东西：%s\n", c);

		printf("a的内容：%s\n", a);
		printf("b的内容：%s\n", b);
		
		if(strstr(b,"pic")!=NULL)      //判断图片类型的消息
		{
			int clientfd = socket(AF_INET, SOCK_STREAM, 0);
			if(clientfd == -1)
			{
				perror("Create socket failed");
				//return -1;
			}

			struct sockaddr_in  clientaddr;
			clientaddr.sin_family = AF_INET;
			clientaddr.sin_addr.s_addr = inet_addr("192.168.95.8");    //局域网服务器ip
			clientaddr.sin_port = htons(80);

			int ret = connect(clientfd, (struct sockaddr *)&clientaddr, sizeof(clientaddr));
			if(ret >= 0)
			{
				//printf("连接成功\n");
			}
			if(ret < 0)
			{
				perror("connect failed");
				close(clientfd);
				//return -1;
			}

			char http[1024]={0};
			sprintf(http,"GET /pic/%s HTTP/1.1\r\nHost:192.168.95.8\r\n\r\n",a);   //获取图片请求
			
			ret = write(clientfd, http, strlen(http));
			char i[1024]={0};
			sprintf(i,"./%s",a);

			int fd = open(i, O_RDWR|O_CREAT|O_TRUNC, 0777);
			if(fd == -1)
			{
				perror("open file failed");
				//return -1;
			}

			//找到头数据
			char buf[4096] = {0};
			int headlen = 0, file = 0, sum = 0;
			int size = read(clientfd, buf, 4096);

			//找到头数据结束符
			char *p = strstr(buf, "\r\n\r\n");
			if(p != NULL)
			{
				//printf("找到头数据的结束符\n");
				headlen = (int)((p+4)-buf);
				printf("%d\n", headlen );
			}

			//找到文件大小
			char *filelen = strstr(buf, "Content-Length:");
			if(filelen != NULL)
			{
				//将文件大小字符串转为整型
				file = atoi(filelen+16);
				printf("已经找到文件大小=%d\n", file);
			}

			//将图片数据写到本地文件中
			write(fd, p+4, size - headlen);
			sum += size - headlen;

			//传剩下的数据 如果sum等于文件大小 传完就走
			while(1)
			{	
				bzero(buf, 4096);
				size = read(clientfd, buf, 4096);
				write(fd, buf, size);
				sum += size;
				if(sum == file)
				{
					printf("文件传输完毕\n");
					break;
				}
			}

			lcd_fd = open_lcd_device(&lcd_fb_addr);   

			display_format_jpeg(a, 200, 120, lcd_fb_addr);//显示图片
			close_lcd_device(lcd_fd, lcd_fb_addr);
				
		}

		if(strstr(b,"txt")!=NULL)       //判断通知消息类型
		{

			struct LcdDevice* lcd = init_lcd("/dev/fb0");
			//打开字体	
			font *f = fontLoad("/usr/share/fonts/DroidSansFallback.ttf");
			  
			//字体大小的设置
			fontSetSize(f,24);

			printf("通知消息为：%s\n",a);

			//创建一个画板（点阵图）
			bitmap *bm = createBitmapWithInit(400,50,4,getColor(0,0,0,0)); //通知消息框
		
			//将字体写到点阵图上
			fontPrint(f,bm,0,0,a,getColor(0,0,0,255),0);

			//把字体框输出到LCD屏幕上
			show_font_to_lcd(lcd->mp,0,0,bm);
		}


		if(strstr(c,"--")!=NULL)      //判断好友列表类型的消息
		{
			printf("ip为：%s\n", c);
			but = touchScreen();

			if(but == 2)
			{
				but = 0;
				printf("%d\n", 2);
				
	
				//初始化Lcd  必须要初始化
				struct LcdDevice* lcd = init_lcd("/dev/fb0");
				//1.打开字体
				font *f = fontLoad("/usr/share/fonts/DroidSansFallback.ttf");

				//2.字体大小的设置
				fontSetSize(f,24);

				//3.创建一个画板（点阵图）
				bitmap *bm5 = createBitmapWithInit(200,50,4,getColor(0,0,0,0)); //好友列表框

				//4.将字体写到点阵图上
				
				fontPrint(f,bm5,0,0,c,getColor(0,0,0,255),0);

				//5.把字体框输出到LCD屏幕上
				show_font_to_lcd(lcd->mp,xx,yy,bm5);
				yy+=50;
				
			}
			
		}

		if(ret<=0)
		{
			break;
		}
		
	}
}	