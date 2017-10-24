#include "siffontcreate.h"
#include "ff.h"
#include "bsp_define.h"

//���嶨��
GUI_FONT SIF12_Font;
GUI_FONT SIF16_Font;
GUI_FONT SIF24_Font;
GUI_FONT SIF36_Font;

//�����ļ�
FIL SIF12FontFile;
FIL SIF16FontFile;
FIL SIF24FontFile;
FIL SIF36FontFile;

//�ֿ��ļ�������
u8 *SIF12FontBuff;
u8 *SIF16FontBuff;
u8 *SIF24FontBuff;
u8 *SIF36FontBuff;

//SD����ȡ�ֿ��ļ�
//fxpath:�ֿ�·��
//buffer:�ֿ��ļ�������
//files:�ļ�
//����ֵ:0��ʧ�ܣ��������õ����ֿ��ļ�
u8 *GetFont_Info(u8 *fxpath,FIL files)
{
    int result;
	u16 bread;
	u8 *FontBuffer;

	result=f_open(&files,(const TCHAR*)fxpath,FA_READ);	//���ֿ��ļ�
	FontBuffer=malloc(files.obj.objsize); //���ⲿSDRAM�����ڴ�

    if(FontBuffer==NULL)
	{
		printf("�ڴ�����ʧ��\r\n");
		FontBuffer=NULL;	//�ڴ�����ʧ��
	}

	result = f_read(&files,FontBuffer,files.obj.objsize,(UINT *)&bread); //��ȡ����
	if(result != FR_OK) FontBuffer=NULL;

    return FontBuffer;
}

//����SIF12���壬��EMWINʹ��
void Create_SIF12(u8 *fxpath)
{
    SIF12FontBuff=GetFont_Info(fxpath,SIF12FontFile);
    if(SIF12FontBuff!=NULL)
    {
        //����SIF��ʽ����
        GUI_SIF_CreateFont(SIF12FontBuff,&SIF12_Font,GUI_SIF_TYPE_PROP);
    }
}

//����SIF16���壬��EMWINʹ��
void Create_SIF16(u8 *fxpath)
{
    SIF16FontBuff=GetFont_Info(fxpath,SIF16FontFile);
    if(SIF16FontBuff!=NULL)
    {
        //����SIF��ʽ����
        GUI_SIF_CreateFont(SIF16FontBuff,&SIF16_Font,GUI_SIF_TYPE_PROP);
    }
}

//����SIF24���壬��EMWINʹ��
void Create_SIF24(u8 *fxpath)
{
    SIF24FontBuff=GetFont_Info(fxpath,SIF24FontFile);
    if(SIF24FontBuff!=NULL)
    {
        //����SIF��ʽ����
        GUI_SIF_CreateFont(SIF24FontBuff,&SIF24_Font,GUI_SIF_TYPE_PROP);
    }
}

//����SIF36���壬��EMWINʹ��
void Create_SIF36(u8 *fxpath)
{
    SIF36FontBuff=GetFont_Info(fxpath,SIF36FontFile);
    if(SIF36FontBuff!=NULL)
    {
        //����SIF��ʽ����
        GUI_SIF_CreateFont(SIF36FontBuff,&SIF36_Font,GUI_SIF_TYPE_PROP);
    }
}