
#include "xbffontcreate.h"
#include "ff.h"

//定义字体
GUI_FONT XBF12_Font;
GUI_FONT XBF14_Font;
GUI_FONT XBF16_Font;
GUI_FONT XBF19_Font;
GUI_FONT XBF24_Font;
GUI_FONT XBF36_Font;

GUI_XBF_DATA	XBF12_Data;
GUI_XBF_DATA	XBF14_Data;
GUI_XBF_DATA	XBF16_Data;
GUI_XBF_DATA	XBF19_Data;
GUI_XBF_DATA	XBF24_Data;
GUI_XBF_DATA	XBF36_Data;

FIL XBF12FontFile;
FIL XBF14FontFile;
FIL XBF16FontFile;
FIL XBF19FontFile;
FIL XBF24FontFile;
FIL XBF36FontFile;

//回调函数，用来获取字体数据
//参数：Off:		在XBF中偏移(位置)
//		NumBytes:	要读取的字节数
//		pVoid:	要读取的文件
//		pBuff:	读取到的数据的缓冲区
//返回值:0 成功，1 失败
static int _cbGetData(volatile uint32_t Off, volatile uint16_t NumBytes, void * pVoid, void * pBuffer)
{
	int result;
	uint16_t bread;
	FIL *hFile;

	hFile = (FIL*)pVoid;

	//设置在文件中的偏移(位置)
	result = f_lseek(hFile,Off);
	if(result != FR_OK)	return 1; //返回错误

	result = f_read(hFile,pBuffer,NumBytes,(UINT *)&bread); //读取数据

	if(result != FR_OK) return 1; //返回错误
	return 0;
}

//创建XBF12字体，共EMWIN使用
//fxpath:XBF字体文件路径
//返回值:0，成功；1，失败
uint8_t Create_XBF12(char *fxpath)
{
	int result;
	result = f_open(&XBF12FontFile,(const TCHAR*)fxpath,FA_READ);	//打开字库文件

	if(result != FR_OK) return 1;
	//创建XBF16字体
	GUI_XBF_CreateFont(	&XBF12_Font,    //指向GUI_FONT结构
						&XBF12_Data, 	//指向GUI_XBF_DATA结构
						GUI_XBF_TYPE_PROP,//要创建的字体类型
						_cbGetData,   	//回调函数
						&XBF12FontFile);  //窗体给回调函数_cbGetData的参数
	return 0;
}
//创建XBF12字体，共EMWIN使用
//fxpath:XBF字体文件路径
//返回值:0，成功；1，失败
uint8_t Create_XBF14(char *fxpath)
{
	int result;
	result = f_open(&XBF14FontFile,(const TCHAR*)fxpath,FA_READ);	//打开字库文件

	if(result != FR_OK) return 1;
	//创建XBF16字体
	GUI_XBF_CreateFont(	&XBF14_Font,    //指向GUI_FONT结构
						&XBF14_Data, 	//指向GUI_XBF_DATA结构
						GUI_XBF_TYPE_PROP,//要创建的字体类型
						_cbGetData,   	//回调函数
						&XBF14FontFile);  //窗体给回调函数_cbGetData的参数
	return 0;
}

//创建XBF16字体，共EMWIN使用
//fxpath:XBF字体文件路径
//返回值:0，成功；1，失败
uint8_t Create_XBF16(char *fxpath)
{
	int result;
	result = f_open(&XBF16FontFile,(const TCHAR*)fxpath,FA_READ);	//打开字库文件

	if(result != FR_OK) return 1;
	//创建XBF16字体
	GUI_XBF_CreateFont(	&XBF16_Font,    //指向GUI_FONT结构
						&XBF16_Data, 	//指向GUI_XBF_DATA结构
						GUI_XBF_TYPE_PROP,//要创建的字体类型
						_cbGetData,   	//回调函数
						&XBF16FontFile);  //窗体给回调函数_cbGetData的参数
	return 0;
}
//创建XBF12字体，共EMWIN使用
//fxpath:XBF字体文件路径
//返回值:0，成功；1，失败
uint8_t Create_XBF19(char *fxpath)
{
	int result;
	result = f_open(&XBF19FontFile,(const TCHAR*)fxpath,FA_READ);	//打开字库文件

	if(result != FR_OK) return 1;
	//创建XBF16字体
	GUI_XBF_CreateFont(	&XBF19_Font,    //指向GUI_FONT结构
						&XBF19_Data, 	//指向GUI_XBF_DATA结构
						GUI_XBF_TYPE_PROP,//要创建的字体类型
						_cbGetData,   	//回调函数
						&XBF19FontFile);  //窗体给回调函数_cbGetData的参数
	return 0;
}

//创建XBF24字体，共EMWIN使用
//fxpath:XBF字体文件路径
//返回值:0，成功；1，失败
uint8_t Create_XBF24(char *fxpath)
{
	int result;
	result = f_open(&XBF24FontFile,(const TCHAR*)fxpath,FA_READ);	//打开字库文件
	if(result != FR_OK) return 1;
	//创建XBF16字体
	GUI_XBF_CreateFont(	&XBF24_Font,    //指向GUI_FONT结构
						&XBF24_Data, 	//指向GUI_XBF_DATA结构
						GUI_XBF_TYPE_PROP,//要创建的字体类型
						_cbGetData,   	//回调函数
						&XBF24FontFile);  //窗体给回调函数_cbGetData的参数
	return 0;
}

//创建XBF36字体，共EMWIN使用
//fxpath:XBF字体文件路径
//返回值:0，成功；1，失败
uint8_t Create_XBF36(char *fxpath)
{
	int result;
	result = f_open(&XBF36FontFile,(const TCHAR*)fxpath,FA_READ);	//打开字库文件
	if(result == FR_NOT_READY)
    {
        return 1;
    }
 	if(result == FR_TOO_MANY_OPEN_FILES)
    {
        return 1;
    }
    if(result == FR_INVALID_PARAMETER)
    {
        return 1;
    }
	if(result != FR_OK) return 1;
	//创建XBF16字体
	GUI_XBF_CreateFont(	&XBF36_Font,    //指向GUI_FONT结构
						&XBF36_Data, 	//指向GUI_XBF_DATA结构
						GUI_XBF_TYPE_PROP,//要创建的字体类型
						_cbGetData,   	//回调函数
						&XBF36FontFile);  //窗体给回调函数_cbGetData的参数
	return 0;
}

