/*************************************************
Copyright (C), 1988-1999
File name: display_bmp.h
Author: ChangZheng Wei 
Version: 1.0
Date: 2019/08/20
Description: 包含了display_bmp.h文件用到的头文件和其中函数的声明
Function List: 
1. get_file_size() //获取图片的大小
2. display_format_bmp() //显示bmp格式文件
History: 
1. Date:2019/08/20
Author: ChangZheng Wei
Modification:创建了此文件
*************************************************/
#ifndef __DISPLAY_BMP_H
#define __DISPLAY_BMP_H

#include "common.h"

off_t get_file_size(const char *pathname);
int display_format_bmp(unsigned int x_s, unsigned int y_s, const char *pathname, unsigned int *lcd_ptr);



#endif
