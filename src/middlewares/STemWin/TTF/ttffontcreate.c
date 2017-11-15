#include "ttffontcreate.h"
#include "ff.h"

static GUI_TTF_CS Cs0, Cs1, Cs2, Cs3;

FIL TTFFontFile;

GUI_FONT TTF12_Font;
GUI_FONT TTF18_Font;
GUI_FONT TTF24_Font;
GUI_FONT TTF36_Font;

GUI_TTF_DATA TTFData;

//����TTF����
//����0 �ɹ�
uint8_t Create_TTFFont(uint8_t *fxpath)
{
    uint8_t result;
    uint32_t bread;
    char *TtfBuffer;

    result = f_open(&TTFFontFile, (const TCHAR*)fxpath, FA_READ);	//���ֿ��ļ�
    if (result != FR_OK) 
        return 1;
	
    TtfBuffer = malloc(TTFFontFile.obj.objsize);//�����ڴ�
    if (TtfBuffer == NULL) 
        return 3;//�ڴ�����ʧ��
			
    result = f_read(&TTFFontFile, TtfBuffer, TTFFontFile.obj.objsize, (UINT *)&bread); //��ȡ����
    if (result != FR_OK) 
    {
        free(TtfBuffer);
        return 4;	//�ļ���ʧ�ܣ�����        
    }
	
    f_close(&TTFFontFile);	//�ر�TTFFointFile�ļ�
	
    TTFData.pData = TtfBuffer;	//ָ���ļ���ַ
    TTFData.NumBytes = TTFFontFile.obj.objsize; //�ļ���С
	
    Cs0.pTTF		= &TTFData;	
    Cs0.PixelHeight	= 12;
    Cs0.FaceIndex	= 0;
	
    Cs1.pTTF		= &TTFData;
    Cs1.PixelHeight	= 18;
    Cs1.FaceIndex	= 0;
	
    Cs2.pTTF		= &TTFData;
    Cs2.PixelHeight	= 24;
    Cs2.FaceIndex	= 0;
	
    Cs3.pTTF		= &TTFData;
    Cs3.PixelHeight	= 36;
    Cs3.FaceIndex	= 0;
	
    result = GUI_TTF_CreateFont(&TTF12_Font, &Cs0);	//��������
    if (result) 
    {
        free(TtfBuffer);
        return 5;	//���崴��ʧ��        
    }
    result = GUI_TTF_CreateFont(&TTF18_Font, &Cs1);	//��������
    if (result) 
    {
        free(TtfBuffer);
        return 6;	//���崴��ʧ��
    }
    result = GUI_TTF_CreateFont(&TTF24_Font, &Cs2);	//��������
    if (result) 
    {
        free(TtfBuffer);
        return 7;	//���崴��ʧ��
    }
    result = GUI_TTF_CreateFont(&TTF36_Font, &Cs3);	//��������
    if (result) 
    {
        free(TtfBuffer);
        return 8;	//���崴��ʧ��        
    }
    return 0;
}





