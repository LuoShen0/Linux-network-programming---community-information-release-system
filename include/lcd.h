/*************************************************
Copyright (C), 1988-1999
File name: lcd.h
Author: ChangZheng Wei 
Version: 1.0
Date: 2019/08/19
Description: 包含了lcd.c文件用到的头文件和其中的所有函数声明
Function List: 
1. lcd_display_point()//像素点颜色显示
2.open_lcd_device()  //打开lcd
3. close_lcd_device()  //关闭LCD
History: 
1. Date:2019/08/19
Author: ChangZheng Wei 
Modification: 创建了此文件
*************************************************/
#ifndef __LCD_H
#define __LCD_H


#include "common.h"

void lcd_display_point(unsigned int x, unsigned int y, unsigned int color, unsigned int *lcd_fb_ptr);
int open_lcd_device(unsigned int **lcd_fb_ptr);
int close_lcd_device(int lcd_fd, unsigned int *lcd_fb_ptr);





#endif