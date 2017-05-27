#ifndef _BMPDISPLAY_H
#define _BMPDISPLAY_H
#include "includes.h"

#define BMPMEMORYSIZE	(8*1024*1024)	//图片大小不大于8M

//绘制无需加载到RAM中的BMP图片时，图片每行的字节数
#define BMPPERLINESIZE	2*1024

int dispbmp(uint8_t *BMPFileName,uint8_t mode,uint32_t x,uint32_t y,int member,int denom);
int dispbmpex(uint8_t *BMPFileName,uint8_t mode,uint32_t x,uint32_t y,int member,int denom);
void emwinbmp_new_pathname(uint8_t *pname,uint8_t *ppath);
void create_bmppicture(uint8_t *filename,int x0,int y0,int Xsize,int Ysize);
void bmpdisplay(uint8_t *ppath);
int dispbmpNOFree(uint8_t is_free,uint8_t *BMPFileName,uint8_t mode,uint32_t x,uint32_t y,int member,int denom,WM_HWIN hWin);
#endif

