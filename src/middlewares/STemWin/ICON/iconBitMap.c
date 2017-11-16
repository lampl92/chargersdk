#include <GUI.h>
#include "iconBitMap.h"

GUI_BITMAP 	*pBitmapHome;
GUI_BITMAP 	*pBitmapRotate;//翻转
GUI_BITMAP 	*pBitmapLog;//历史记录
GUI_BITMAP 	*pBitmapNet;//网络信息;
GUI_BITMAP 	*pBitmapEvseInfo;//桩信息，包括费率等
GUI_BITMAP 	*pBitmapBaseInfo;//基本信息，包括模拟量状态量
GUI_BITMAP 	*pBitmapSys;//系统设置信息

uint8_t iconRead(uint8_t *fPath,const uint8_t *iconPaddr)
{
    uint32_t bread;
    uint8_t result;
    FIL iconFile;
    const char *iconBmp = iconPaddr;
    
    result = f_open(&iconFile, (const TCHAR*)fPath, FA_READ);	//打开文件
    //文件打开错误或者文件大于BMPMEMORYSIZE
    if ((result != FR_OK) || (iconFile.obj.objsize > ICONMEMORYSIZE)) 	return 1;

    iconBmp = malloc(iconFile.obj.objsize);//申请内存

    if (iconBmp == NULL)
    {
        return 2;//分配失败
    }
  
    result = f_read(&iconFile, iconBmp, iconFile.obj.objsize, (UINT *)&bread); //读取数据
    if (result != FR_OK) 
    {
        free(iconBmp);
        return 3;
    }
    
    f_close(&iconFile);				//关闭BMPFile文件
    return 0;  
}

void createIcon(GUI_BITMAP *bitmap, uint16_t xsize, uint16_t ysize,uint16_t xper,uint16_t yper, const uint8_t *paddr)
{
    bitmap->XSize = xsize;
    bitmap->YSize = ysize;
    bitmap->BytesPerLine = xper;
    bitmap->BitsPerPixel = yper;
    bitmap->pData = &paddr;
    bitmap->pPal = NULL;
    bitmap->pMethods = GUI_DRAW_BMP8888;
}

