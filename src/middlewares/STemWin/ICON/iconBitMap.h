#ifndef _ICONBITMAP_H
#define _ICONBITMAP_H

#include "ff.h"
#include "GUI.h"
#include "includes.h"

#define ICONMEMORYSIZE	(1*1024*1024)	//ͼƬ��С������1M

extern GUI_BITMAP 	pBitmapHome;
extern GUI_BITMAP 	pBitmapRotate;//��ת
extern GUI_BITMAP 	pBitmapLog;//��ʷ��¼
extern GUI_BITMAP 	pBitmapNet;//������Ϣ;
extern GUI_BITMAP 	pBitmapEvseInfo;//׮��Ϣ���������ʵ�
extern GUI_BITMAP 	pBitmapBaseInfo;//������Ϣ������ģ����״̬��
extern GUI_BITMAP 	pBitmapSys;//ϵͳ������Ϣ

extern uint8_t iconRead(uint8_t *fPath, GUI_BITMAP *bitmap);
extern void createIcon(GUI_BITMAP *bitmap, uint16_t xsize, uint16_t ysize, uint16_t xper, uint16_t yper);

#endif
