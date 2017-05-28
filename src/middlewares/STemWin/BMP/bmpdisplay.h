#ifndef _BMPDISPLAY_H
#define _BMPDISPLAY_H
#include "includes.h"

#define BMPMEMORYSIZE	(8*1024*1024)	//ͼƬ��С������8M

//����������ص�RAM�е�BMPͼƬʱ��ͼƬÿ�е��ֽ���
#define BMPPERLINESIZE	2*1024

int dispbmp(uint8_t *BMPFileName,uint8_t mode,uint32_t x,uint32_t y,int member,int denom);
int dispbmpex(uint8_t *BMPFileName,uint8_t mode,uint32_t x,uint32_t y,int member,int denom);
void emwinbmp_new_pathname(uint8_t *pname,uint8_t *ppath);
void create_bmppicture(uint8_t *filename,int x0,int y0,int Xsize,int Ysize);
void bmpdisplay(uint8_t *ppath);
uint8_t dispbmpNOFree(uint8_t is_free,uint8_t *BMPFileName,uint8_t mode,uint32_t x,uint32_t y,int member,int denom,WM_HWIN hWin);
uint8_t readBackGroundNOFREE(uint8_t *BMPFileName);
#endif

