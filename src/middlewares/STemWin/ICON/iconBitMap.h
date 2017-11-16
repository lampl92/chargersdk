#ifndef _ICONBITMAP_H
#define _ICONBITMAP_H

#include "ff.h"
#include "GUI.h"
#include "includes.h"

#define ICONMEMORYSIZE	(1*1024*1024)	//图片大小不大于1M

extern GUI_BITMAP 	pBitmapHome;
extern GUI_BITMAP 	pBitmapRotate;//翻转
extern GUI_BITMAP 	pBitmapLog;//历史记录
extern GUI_BITMAP 	pBitmapNet;//网络信息;
extern GUI_BITMAP 	pBitmapEvseInfo;//桩信息，包括费率等
extern GUI_BITMAP 	pBitmapBaseInfo;//基本信息，包括模拟量状态量
extern GUI_BITMAP 	pBitmapSys;//系统设置信息

extern uint8_t iconRead(uint8_t *fPath, GUI_BITMAP *bitmap);
extern void createIcon(GUI_BITMAP *bitmap, uint16_t xsize, uint16_t ysize, uint16_t xper, uint16_t yper);

#endif
