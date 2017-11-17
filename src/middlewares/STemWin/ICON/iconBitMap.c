#include <GUI.h>
#include "iconBitMap.h"

GUI_BITMAP 	pBitmapHome;
GUI_BITMAP 	pBitmapRotate;//��ת
GUI_BITMAP 	pBitmapLog;//��ʷ��¼
GUI_BITMAP 	pBitmapNet;//������Ϣ;
GUI_BITMAP 	pBitmapEvseInfo;//׮��Ϣ���������ʵ�
GUI_BITMAP 	pBitmapBaseInfo;//������Ϣ������ģ����״̬��
GUI_BITMAP 	pBitmapSys;//ϵͳ������Ϣ
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
    
    result = f_open(&iconFile, (const TCHAR*)fPath, FA_READ);	//���ļ�
    //�ļ��򿪴�������ļ�����BMPMEMORYSIZE
    if ((result != FR_OK) || (iconFile.obj.objsize > ICONMEMORYSIZE)) 	return 1;

    bitmap->pData = malloc(iconFile.obj.objsize);//�����ڴ�

    if (bitmap->pData == NULL)
    {
        return 2;//����ʧ��
    }
  
    iconBmp = bitmap->pData;
    taskENTER_CRITICAL();
    
    result = f_read(&iconFile, (void *)bitmap->pData, iconFile.obj.objsize, (UINT *)&bread); //��ȡ����
    if (result != FR_OK) 
    {
        free(bitmap->pData);
        return 3;
    }
    bitmap->XSize = 61;//GUI_BMP_GetXSize(bitmap->pData);
    bitmap->YSize = 58;//GUI_BMP_GetYSize(bitmap->pData);
    bitmap->BytesPerLine = 183;//xper;//һ��λͼ���ݺ��е��ֽ���
    bitmap->BitsPerPixel = 24;//pInfo->BitsPerPixel;    
    bitmap->pPal = NULL;
    bitmap->pMethods = GUI_DRAW_BMP24;
    //bitmap->pData = bitmap->pData;//(unsigned char *)_acbaseinfo;
    taskEXIT_CRITICAL();
    
//    f_open(&testFile, "system/test.c", FA_CREATE_NEW | FA_READ | FA_WRITE);
//    f_write(&testFile, bitmap->pData, iconFile.obj.objsize, (UINT *)&bwrite);
//    f_close(&testFile);
    f_close(&iconFile);				//�ر�BMPFile�ļ�
    return 0;  
}

void createIcon(GUI_BITMAP *bitmap, uint16_t xsize, uint16_t ysize,uint16_t xper,uint16_t yper)
{
//    bitmap->XSize = xsize;
//    bitmap->YSize = ysize;
//    bitmap->BytesPerLine = 183;//xper;//һ��λͼ���ݺ��е��ֽ���
//    bitmap->BitsPerPixel = yper;//ÿ���ص�λ��
//    bitmap->pData = &paddr;
//    bitmap->pPal = NULL;
//    bitmap->pMethods = GUI_DRAW_BMP24;
}

void iconBitmapInit()
{
    pBitmapHome.XSize = 61;//GUI_BMP_GetXSize(pBitmapHome.pData);
    pBitmapHome.YSize = 58;//GUI_BMP_GetYSize(pBitmapHome.pData);
    pBitmapHome.BytesPerLine = 183;//xper;//һ��λͼ���ݺ��е��ֽ���
    pBitmapHome.BitsPerPixel = 24;//pInfo->BitsPerPixel;    
    pBitmapHome.pPal = NULL;
    pBitmapHome.pMethods = GUI_DRAW_BMP24;
    pBitmapHome.pData = (unsigned char *)_achome;

    pBitmapBaseInfo.XSize = 61;//GUI_BMP_GetXSize(pBitmapBaseInfo.pData);
    pBitmapBaseInfo.YSize = 58;//GUI_BMP_GetYSize(pBitmapBaseInfo.pData);
    pBitmapBaseInfo.BytesPerLine = 183;//xper;//һ��λͼ���ݺ��е��ֽ���
    pBitmapBaseInfo.BitsPerPixel = 24;//pInfo->BitsPerPixel;    
    pBitmapBaseInfo.pPal = NULL;
    pBitmapBaseInfo.pMethods = GUI_DRAW_BMP24;
    pBitmapBaseInfo.pData = (unsigned char *)_acbaseinfo;
}