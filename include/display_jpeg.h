/*************************************************
Copyright (C), 1988-1999
File name: display_jpeg.h
Author: ChangZheng Wei 
Version: 1.0
Date: 2019/08/21
Description: 将函数display_format_jpeg()声明为外部函数
Function List: 
1. display_format_jpeg() //显示jpg格式图片
History: 
1. Date:2019/08/21
Author: ChangZheng Wei 
Modification: 创建了此文件
*************************************************/
#ifndef __DISPLAY_JPEG_H
#define __DISPLAY_JPEG_H


extern int display_format_jpeg(const char *filename, unsigned int x_s, unsigned int y_s, unsigned int *lcd_buf_ptr);



#endif
