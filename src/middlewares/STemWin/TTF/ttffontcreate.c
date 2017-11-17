#include "ttffontcreate.h"
#include "ff.h"

static GUI_TTF_CS Cs0, Cs1, Cs2, Cs3;

FIL TTFFontFile;

GUI_FONT TTF12_Font;
GUI_FONT TTF18_Font;
GUI_FONT TTF24_Font;
GUI_FONT TTF36_Font;

GUI_TTF_DATA TTFData;

//创建TTF字体
//返回0 成功
uint8_t Create_TTFFont(uint8_t *fxpath)
{
    uint8_t result;
    uint32_t bread;
    char *TtfBuffer;

    result = f_open(&TTFFontFile, (const TCHAR*)fxpath, FA_READ);	//打开字库文件
    if (result != FR_OK) 
        return 1;
	
    TtfBuffer = malloc(TTFFontFile.obj.objsize);//申请内存
    if (TtfBuffer == NULL) 
        return 3;//内存申请失败
			
    result = f_read(&TTFFontFile, TtfBuffer, TTFFontFile.obj.objsize, (UINT *)&bread); //读取数据
    if (result != FR_OK) 
    {
        free(TtfBuffer);
        return 4;	//文件打开失败，跳出        
    }
	
    f_close(&TTFFontFile);	//关闭TTFFointFile文件
	
    TTFData.pData = TtfBuffer;	//指向文件地址
    TTFData.NumBytes = TTFFontFile.obj.objsize; //文件大小
	
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
	
//    result = GUI_TTF_CreateFont(&TTF12_Font, &Cs0);	//创建字体
//    if (result) 
//    {
//        free(TtfBuffer);
//        return 5;	//字体创建失败        
//    }
//    result = GUI_TTF_CreateFont(&TTF18_Font, &Cs1);	//创建字体
//    if (result) 
//    {
//        free(TtfBuffer);
//        return 6;	//字体创建失败
//    }
//    result = GUI_TTF_CreateFont(&TTF24_Font, &Cs2);	//创建字体
//    if (result) 
//    {
//        free(TtfBuffer);
//        return 7;	//字体创建失败
//    }
//    result = GUI_TTF_CreateFont(&TTF36_Font, &Cs3);	//创建字体
//    if (result) 
//    {
//        free(TtfBuffer);
//        return 8;	//字体创建失败        
//    }
    return 0;
}





