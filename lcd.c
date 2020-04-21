//************************************************************
//*Copyright (C), 1988-1999.
//*
//*FileName: lcd.c
//*
//*Author: ChangZheng Wei 
//*
//*Version : 1.0
//*
//*Date: 2019/08/19
//*
//*Description: 打开LCD，并且映射一块内存
//*
//*Function List: 
//1.lcd_display_point() //
//2.open_lcd_device()   //
//3. close_lcd_device() //
//*
//*History: 
//<author>       <time>     <version > <desc>
//ChangZheng Wei 2019/08/19  1.0      创建此文件
//*************************************************************
#include "common.h"


/*整个屏幕所有像素点的大小*/
#define FB_SIZE	800*480*4    


/*******************************************************************
Function: lcd_display_point()

Description:显示颜色 

Calls: 无

Called By:  display_format_jpeg()

Input: x和y为坐标，color为颜色，lcd_fb_ptr为指针

Return:返回头结点 
********************************************************************/
void lcd_display_point(unsigned int x, unsigned int y, unsigned int color, unsigned int *lcd_fb_ptr)
{
	*(lcd_fb_ptr+800*y+x) = color;
}


/*******************************************************************
Function: open_lcd_device()

Description: 打开LCD屏幕，并且映射一块内存

Calls:  (1.open() 2.perror() 3.mmap())

Called By: main()

Input: lcd_fb_ptr指向内存映射

Return:文件标识符
********************************************************************/
int open_lcd_device(unsigned int **lcd_fb_ptr)
{
	int lcd_fd;

	lcd_fd = open("/dev/fb0", O_RDWR);
	if(lcd_fd == -1)
	{
		perror("open lcd device failed\n");
		return -1;
	}

	*lcd_fb_ptr = mmap( NULL, FB_SIZE, PROT_READ|PROT_WRITE, MAP_SHARED, lcd_fd, 0);
	if(*lcd_fb_ptr == MAP_FAILED)
	{
		perror("map lcd_fb error\n");
		return -1;
	}


	return lcd_fd;
}


/*******************************************************************
Function: close_lcd_device()

Description: 关闭LCD屏幕

Calls: munmap()

Called By: main()

Return:关闭LCD 
********************************************************************/
int close_lcd_device(int lcd_fd, unsigned int *lcd_fb_ptr)
{
	munmap(lcd_fb_ptr, FB_SIZE);

	return close(lcd_fd);
}


