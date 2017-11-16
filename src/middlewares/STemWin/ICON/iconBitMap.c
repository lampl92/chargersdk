#include <GUI.h>
#include "iconBitMap.h"

GUI_BITMAP 	*pBitmapHome;
GUI_BITMAP 	*pBitmapRotate;//��ת
GUI_BITMAP 	*pBitmapLog;//��ʷ��¼
GUI_BITMAP 	*pBitmapNet;//������Ϣ;
GUI_BITMAP 	*pBitmapEvseInfo;//׮��Ϣ���������ʵ�
GUI_BITMAP 	*pBitmapBaseInfo;//������Ϣ������ģ����״̬��
GUI_BITMAP 	*pBitmapSys;//ϵͳ������Ϣ

uint8_t iconRead(uint8_t *fPath,const uint8_t *iconPaddr)
{
    uint32_t bread;
    uint8_t result;
    FIL iconFile;
    const char *iconBmp = iconPaddr;
    
    result = f_open(&iconFile, (const TCHAR*)fPath, FA_READ);	//���ļ�
    //�ļ��򿪴�������ļ�����BMPMEMORYSIZE
    if ((result != FR_OK) || (iconFile.obj.objsize > ICONMEMORYSIZE)) 	return 1;

    iconBmp = malloc(iconFile.obj.objsize);//�����ڴ�

    if (iconBmp == NULL)
    {
        return 2;//����ʧ��
    }
  
    result = f_read(&iconFile, iconBmp, iconFile.obj.objsize, (UINT *)&bread); //��ȡ����
    if (result != FR_OK) 
    {
        free(iconBmp);
        return 3;
    }
    
    f_close(&iconFile);				//�ر�BMPFile�ļ�
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

