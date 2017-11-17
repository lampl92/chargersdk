#include <GUI.h>
#include "iconBitMap.h"

GUI_BITMAP 	pBitmapHome;
GUI_BITMAP 	pBitmapRotate;//翻转
GUI_BITMAP 	pBitmapLog;//历史记录
GUI_BITMAP 	pBitmapNet;//网络信息;
GUI_BITMAP 	pBitmapEvseInfo;//桩信息，包括费率等
GUI_BITMAP 	pBitmapBaseInfo;//基本信息，包括模拟量状态量
GUI_BITMAP 	pBitmapSys;//系统设置信息
static GUI_CONST_STORAGE unsigned char _acbaseinfo[] = {
};
static GUI_CONST_STORAGE unsigned char _achome[] = {

};

uint8_t iconRead(uint8_t *fPath, GUI_BITMAP *bitmap)
{
    uint32_t bread;
    uint32_t bwrite;
    uint8_t result;
    FIL iconFile,testFile;
    GUI_BITMAPSTREAM_INFO *pInfo;
    char *iconBmp;
    
    result = f_open(&iconFile, (const TCHAR*)fPath, FA_READ);	//打开文件
    //文件打开错误或者文件大于BMPMEMORYSIZE
    if ((result != FR_OK) || (iconFile.obj.objsize > ICONMEMORYSIZE)) 	return 1;

    bitmap->pData = malloc(iconFile.obj.objsize);//申请内存

    if (bitmap->pData == NULL)
    {
        return 2;//分配失败
    }
  
    iconBmp = bitmap->pData;
    taskENTER_CRITICAL();
    
    result = f_read(&iconFile, (void *)bitmap->pData, iconFile.obj.objsize, (UINT *)&bread); //读取数据
    if (result != FR_OK) 
    {
        free(bitmap->pData);
        return 3;
    }
    bitmap->XSize = 61;//GUI_BMP_GetXSize(bitmap->pData);
    bitmap->YSize = 58;//GUI_BMP_GetYSize(bitmap->pData);
    bitmap->BytesPerLine = 183;//xper;//一行位图数据含有的字节数
    bitmap->BitsPerPixel = 24;//pInfo->BitsPerPixel;    
    bitmap->pPal = NULL;
    bitmap->pMethods = GUI_DRAW_BMP24;
    //bitmap->pData = bitmap->pData;//(unsigned char *)_acbaseinfo;
    taskEXIT_CRITICAL();
    
//    f_open(&testFile, "system/test.c", FA_CREATE_NEW | FA_READ | FA_WRITE);
//    f_write(&testFile, bitmap->pData, iconFile.obj.objsize, (UINT *)&bwrite);
//    f_close(&testFile);
    f_close(&iconFile);				//关闭BMPFile文件
    return 0;  
}

void createIcon(GUI_BITMAP *bitmap, uint16_t xsize, uint16_t ysize,uint16_t xper,uint16_t yper)
{
//    bitmap->XSize = xsize;
//    bitmap->YSize = ysize;
//    bitmap->BytesPerLine = 183;//xper;//一行位图数据含有的字节数
//    bitmap->BitsPerPixel = yper;//每像素的位数
//    bitmap->pData = &paddr;
//    bitmap->pPal = NULL;
//    bitmap->pMethods = GUI_DRAW_BMP24;
}

void iconBitmapInit()
{
    pBitmapHome.XSize = 61;//GUI_BMP_GetXSize(pBitmapHome.pData);
    pBitmapHome.YSize = 58;//GUI_BMP_GetYSize(pBitmapHome.pData);
    pBitmapHome.BytesPerLine = 183;//xper;//一行位图数据含有的字节数
    pBitmapHome.BitsPerPixel = 24;//pInfo->BitsPerPixel;    
    pBitmapHome.pPal = NULL;
    pBitmapHome.pMethods = GUI_DRAW_BMP24;
    pBitmapHome.pData = (unsigned char *)_achome;

    pBitmapBaseInfo.XSize = 61;//GUI_BMP_GetXSize(pBitmapBaseInfo.pData);
    pBitmapBaseInfo.YSize = 58;//GUI_BMP_GetYSize(pBitmapBaseInfo.pData);
    pBitmapBaseInfo.BytesPerLine = 183;//xper;//一行位图数据含有的字节数
    pBitmapBaseInfo.BitsPerPixel = 24;//pInfo->BitsPerPixel;    
    pBitmapBaseInfo.pPal = NULL;
    pBitmapBaseInfo.pMethods = GUI_DRAW_BMP24;
    pBitmapBaseInfo.pData = (unsigned char *)_acbaseinfo;
}