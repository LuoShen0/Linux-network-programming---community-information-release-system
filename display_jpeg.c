//************************************************************
//*Copyright (C), 1988-1999.
//*
//*FileName: display_jpeg.c
//*
//*Author: ChangZheng Wei 
//*
//*Version : 1.0
//*
//*Date: 2019/08/21
//*
//*Description: 显示jpg格式图片
//
//*Function List: display_format_jpeg()
//*
//*History: 
//<author>       <time>     <version > <desc>
//ChangZheng Wei 2019/08/21  1.0       创建此文件
//*************************************************************
#include <stdio.h>
#include <setjmp.h>
#include <stdlib.h>
#include <jpeglib.h>
#include <jerror.h>
#include "lcd.h"


/*指向R、G、B阶数据的大数组*/
extern JSAMPLE * image_buffer;	

/*图像中的行数*/
extern int image_height;

/*图像中的列数 */	
extern int image_width;		

struct my_error_mgr {
  struct jpeg_error_mgr pub;   

  jmp_buf setjmp_buffer;      
};

typedef struct my_error_mgr * my_error_ptr;


METHODDEF(void)
my_error_exit (j_common_ptr cinfo)
{
  /* cinfo->err really points to a my_error_mgr struct, so coerce pointer */
  my_error_ptr myerr = (my_error_ptr) cinfo->err;

  /* Always display the message. */
  /* We could postpone this until after returning, if we chose. */
  (*cinfo->err->output_message) (cinfo);

  /* Return control to the setjmp point */
  longjmp(myerr->setjmp_buffer, 1);
}


/*
 * Sample routine for JPEG decompression.  We assume that the source file name
 * is passed in.  We want to return 1 on success, 0 on error.
 */


int display_format_jpeg(const char *filename, unsigned int x_s, unsigned int y_s, unsigned int *lcd_buf_ptr)
{
  
	struct jpeg_decompress_struct cinfo;
  
	struct my_error_mgr jerr;

	/* source file */
	FILE * infile;

	/* physical row width in output buffer */		
	int row_stride;		
 
	if((infile = fopen(filename, "rb")) == NULL) 
	{
		fprintf(stderr, "can't open %s\n", filename);
		return 0;
	}

	cinfo.err = jpeg_std_error(&jerr.pub);
	jerr.pub.error_exit = my_error_exit;
  
	if(setjmp(jerr.setjmp_buffer)) 
	{    
		jpeg_destroy_decompress(&cinfo);
		fclose(infile);
		return 0;
	}

	jpeg_create_decompress(&cinfo);

	jpeg_stdio_src(&cinfo, infile);
	
	jpeg_read_header(&cinfo, TRUE);
    
	jpeg_start_decompress(&cinfo);
  
	row_stride = cinfo.output_width * cinfo.output_components;

	/* Output row buffer */
	char *buffer;		
  
	buffer =(char *)(*cinfo.mem->alloc_sarray)
		((j_common_ptr) &cinfo, JPOOL_IMAGE, row_stride, 1);

	unsigned int x, y = y_s;
	unsigned int color;
	char *buf_save = buffer;

	while(cinfo.output_scanline < cinfo.output_height) 
	{    
		buffer = buf_save;

		jpeg_read_scanlines(&cinfo, (JSAMPARRAY)&buffer, 1);
		
		for(x=x_s; x<x_s+cinfo.output_width; x++)
		{
			color = buffer[0]<<16 | buffer[1]<<8 |buffer[2];

			lcd_display_point(x, y, color, lcd_buf_ptr);

			buffer+=3;
		}

		y++;
	}

 

	jpeg_finish_decompress(&cinfo);
 
	jpeg_destroy_decompress(&cinfo);

 
	fclose(infile);

	return 1;
}


