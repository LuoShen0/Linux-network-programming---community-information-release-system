/*************************************************
Copyright (C), 1988-1999
File name: link.h
Author:  ChangZheng Wei  
Version: 1.0
Date: 2019/08/18
Description: 包含了link.c文件用到的头文件和其中的所有函数声明
Function List:
1.request_link_list_head_node()  // 申请链表头结点
2.new_link_list_node() //申请一个新结点
3.insert_node_to_link_list() //新结点插入到头结点后面
4.is_empty()  // 判断链表是否为空
5.display_link_list_node()  //遍历链表的内容
6.remove_link_list()  //销毁链表
History: 
1. Date:2019/08/18
Author: ChangZheng Wei  
Modification:创建了此文件
*************************************************/
#ifndef __LINK_H
#define __LINK_H

#include "common.h"

struct link_list_node
{
    char pic_name[4096];
    size_t pic_size;
    char format[5];
    struct link_list_node *next,*prev;

};


struct link_list_node *request_link_list_head_node(void);
struct link_list_node *new_link_list_node(char *pic_name, size_t pic_size,  char *format);
void insert_node_to_link_list(struct link_list_node *list_head, struct link_list_node *new_node);
bool is_empty(struct link_list_node *list_head);
void display_link_list_node(struct link_list_node *list_head);
void remove_link_list(struct link_list_node * list_head);


#endif


