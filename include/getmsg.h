#ifndef __GET_PIC_H__
#define __GET_PIC_H__


void *get_pic();
struct LcdDevice *init_lcd(const char *device);
void *tianqi(void *arg);
int touchScreen();


#endif