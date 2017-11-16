#include "bmpdisplay.h"
#include "GUI.h"
#include "DIALOG.h"
#include "yaffsfs.h"

static int BMPFile;
int BMPFile_BCGROUND;
int BMPFile_ENCODE;
static int ScrSortFile;                 //屏幕截图文件
static char bmpBuffer[BMPPERLINESIZE];
char *bmpbuffer;
char *bmpBackGround;
/*******************************************************************
*
*       Static functions
*
********************************************************************
*/
/*********************************************************************
*
*       BmpGetData
*
* Function description
*   This routine is called by GUI_BMP_DrawEx(). The routine is responsible
*   for setting the data pointer to a valid data location with at least
*   one valid byte.
*
* Parameters:
*   p           - Pointer to application defined data.
*   NumBytesReq - Number of bytes requested.
*   ppData      - Pointer to data pointer. This pointer should be set to
*                 a valid location.
*   StartOfFile - If this flag is 1, the data pointer should be set to the
*                 beginning of the data stream.
*
* Return value:
*   Number of data bytes available.
*/
static int BmpGetData(void *p, const uint8_t ** ppData, unsigned NumBytesReq, unsigned long Off)
{
	static int readaddress=0;
	int * phFile;
	uint32_t NumBytesRead;

	phFile = (int *)p;

	if (NumBytesReq > sizeof(bmpBuffer))
	{
		NumBytesReq = sizeof(bmpBuffer);
	}

	//移动指针到应该读取的位置
	if(Off == 1) readaddress = 0;
	else readaddress=Off;

    taskENTER_CRITICAL();	//临界区

	yaffs_lseek(*phFile, readaddress, SEEK_SET);

	//读取数据到缓冲区中
    NumBytesRead = yaffs_read(*phFile, bmpBuffer, NumBytesReq);

    taskEXIT_CRITICAL();	//退出临界区

	*ppData = (uint8_t *)bmpBuffer;
	return NumBytesRead;//返回读取到的字节数
}

//在指定位置显示加载到RAM中的BMP图片
//BMPFileName:图片在SD卡或者其他存储设备中的路径(需文件系统支持！)
//mode:显示模式
//		0 在指定位置显示，由参数x,y确定显示位置
//		1 在LCD中间显示图片，当选择此模式的时候参数x,y无效。
//x:图片左上角在LCD中的x轴位置(当参数mode为1时，此参数无效)
//y:图片左上角在LCD中的y轴位置(当参数mode为1时，此参数无效)
//member:  缩放比例的分子项
//denom:缩放比例的分母项
//返回值:0 显示正常,其他 失败
#if 0
int dispbmp(uint8_t *BMPFileName,uint8_t mode,uint32_t x,uint32_t y,int member,int denom)
{
	uint16_t bread;
	uint16_t bred;
	uint16_t bre;
	char *bmpbuffer;
	char result;
	int XSize,YSize;
	float Xflag,Yflag;

	result = yaffs_open(&BMPFile,(const TCHAR*)BMPFileName,FA_READ);	//打开文件
	//文件打开错误或者文件大于BMPMEMORYSIZE
	if((result != FR_OK) || (BMPFile.obj.objsize>BMPMEMORYSIZE)) 	return 1;

	bmpbuffer = malloc(BMPFile.obj.objsize);//申请内存
	if(bmpbuffer == NULL) return 2;

    //taskENTER_CRITICAL();	//临界区

	result = f_read(&BMPFile,bmpbuffer,BMPFile.obj.objsize,(uint32_t *)&bread); //读取数据
	if(result != FR_OK) return 3;//        ！！！！内存泄漏风险！！！！！！！

    //taskEXIT_CRITICAL();//退出临界区

	switch(mode)
	{
		case 0:	//在指定位置显示图片
			if((member == 1) && (denom == 1)) //无需缩放，直接绘制
			{
				GUI_BMP_Draw(bmpbuffer,x,y);	//在指定位置显示BMP图片
			}else //否则图片需要缩放
			{
				GUI_BMP_DrawScaled(bmpbuffer,x,y,member,denom);
			}
			break;
		case 1:	//在LCD中间显示图片
			XSize = GUI_BMP_GetXSize(bmpbuffer);	//获取图片的X轴大小
			YSize = GUI_BMP_GetYSize(bmpbuffer);	//获取图片的Y轴大小
			if((member == 1) && (denom == 1)) //无需缩放，直接绘制
			{
				//在LCD中间显示图片
				//GUI_BMP_Draw(bmpbuffer,(lcddev.width-XSize)/2-1,(lcddev.height-YSize)/2-1);
			}else //否则图片需要缩放
			{
				//Xflag = (float)XSize*((float)member/(float)denom);
				//Yflag = (float)YSize*((float)member/(float)denom);
				//XSize = (lcddev.width-(int)Xflag)/2-1;
				//YSize = (lcddev.height-(int)Yflag)/2-1;
				//GUI_BMP_DrawScaled(bmpbuffer,XSize,YSize,member,denom);
			}
			break;
	}
	yaffs_close(BMPFile);				//关闭BMPFile文件
	free(bmpbuffer);		//释放内存
	return 0;
}
#endif

//在指定位置显示无需加载到RAM中的BMP图片(需文件系统支持！对于小RAM，推荐使用此方法！)
//BMPFileName:图片在SD卡或者其他存储设备中的路径
//mode:显示模式
//		0 在指定位置显示，有参数x,y确定显示位置
//		1 在LCD中间显示图片，当选择此模式的时候参数x,y无效。
//x:图片左上角在LCD中的x轴位置(当参数mode为1时，此参数无效)
//y:图片左上角在LCD中的y轴位置(当参数mode为1时，此参数无效)
//member:  缩放比例的分子项
//denom:缩放比例的分母项
//返回值:0 显示正常,其他 失败
int dispbmpex(uint8_t *BMPFileName,uint8_t mode,uint32_t x,uint32_t y,int member,int denom)
{
	char result;
	int XSize,YSize;
	float Xflag,Yflag;

    BMPFile = yaffs_open(BMPFileName, O_RDONLY, 0);	//打开文件
    result = yaffsfs_GetLastError();
	//文件打开错误
	if(result != 0) 	
    	return 1;

	switch(mode)
	{
		case 0:	//在指定位置显示图片
			if((member == 1) && (denom == 1)) //无需缩放，直接绘制
			{
				GUI_BMP_DrawEx(BmpGetData,&BMPFile,x,y);//在指定位置显示BMP图片
			}else //否则图片需要缩放
			{
				GUI_BMP_DrawScaledEx(BmpGetData,&BMPFile,x,y,member,denom);
			}
			break;
		case 1:	//在LCD中间显示图片
			XSize = GUI_BMP_GetXSizeEx(BmpGetData,&BMPFile);	//获取图片的X轴大小
			YSize = GUI_BMP_GetYSizeEx(BmpGetData,&BMPFile);	//获取图片的Y轴大小
			if((member == 1) && (denom == 1)) //无需缩放，直接绘制
			{
				//在LCD中间显示图片
				//GUI_BMP_DrawEx(BmpGetData,&BMPFile,(lcddev.width-XSize)/2-1,(lcddev.height-YSize)/2-1);
			}else //否则图片需要缩放
			{
				//Xflag = (float)XSize*((float)member/(float)denom);
				//Yflag = (float)YSize*((float)member/(float)denom);
				//XSize = (lcddev.width-(int)Xflag)/2-1;
				//YSize = (lcddev.height-(int)Yflag)/2-1;
				//GUI_BMP_DrawScaledEx(BmpGetData,&BMPFile,XSize,YSize,member,denom);
			}
			break;
	}
	yaffs_close(BMPFile);		//关闭BMPFile文件
	return 0;
}

//此函数被GUI_BMP_Serialize()调用，用来向文件写入字节
static void _WriteByte2File(uint8_t Data, void * p)
{
	uint32_t nWritten;
    nWritten = yaffs_write(*(int*)p, &Data, 1);
}

//得到一个文件名
//pname:得到的文件名
void emwinbmp_new_pathname(uint8_t *pname,uint8_t *ppath)
{
    uint8_t res;
    uint16_t index=0;
    while(index<0XFFFF)
    {
        sprintf((char*)pname,ppath,index);
        ScrSortFile = yaffs_open(pname, O_RDONLY, 0);//尝试打开这个文件
        res = yaffsfs_GetLastError();
        if(res!=0)
            break;   //改文件名不存在=正是我们需要的
        else if(res==0) 
            yaffs_close(ScrSortFile);//如果打开成功，就关闭掉
        index++;
    }
}
//生成一个BMP图片，保存到SD卡中,实现屏幕截图功能！
//*filepath:文件路径
//x0:创建BMP文件的X起始位置
//y0:创建BMP文件的Y起始位置
//Xsize:X的大小
//Ysize:Y的大小
void create_bmppicture(uint8_t *filename,int x0,int y0,int Xsize,int Ysize)
{
	static int hFile;
	//创建一个文件，路径为filename,如果文件原本就存在的话会被新建的文件覆盖掉！
    hFile = yaffs_open(filename, O_CREAT|O_TRUNC|O_RDWR,0);
	GUI_BMP_SerializeEx(_WriteByte2File,x0,y0,Xsize,Ysize,&hFile);
	yaffs_close(hFile);	//关闭文件
}

void bmpdisplay(uint8_t *ppath)
{
    //GUI_DispStringHCenterAt("在指定位置显示一张加载到RAM中的BMP图片",400,5);
    dispbmp(ppath,0,0,0,1,1);
    //GUI_Clear();
}

/** @brief
 *
 * @param
 * @param
 * @return
 *  **********注意在切换界面时要使用free释放掉图片的内存
 */
uint8_t dispbmpNOFree(uint8_t is_free,char *BMPFileName,uint8_t mode,uint32_t x,uint32_t y,int member,int denom,WM_HWIN hWin)
{
	uint16_t bread;
	uint16_t bred;
	uint16_t bre;
	char result;
	int XSize,YSize;
	float Xflag,Yflag;
	WM_HWIN      hItem;
	IMAGE_Handle imageHandle;
    struct yaffs_stat st;

//    if(is_free == 1)
//    {
//        free(bmpbuffer);
//        return 0;
//    }

    yaffs_stat(BMPFileName, &st);
    BMPFile = yaffs_open(BMPFileName, O_RDONLY, 0);	//打开文件
    result = yaffsfs_GetLastError();
	//文件打开错误或者文件大于BMPMEMORYSIZE
	if((result != 0) || (st.st_size>BMPMEMORYSIZE)) 	
    	return 1;

    bmpbuffer = malloc(st.st_size);//申请内存

	if(bmpbuffer == NULL)
	{
        return 2;//分配失败
	}

    //taskENTER_CRITICAL();	//临界区

    bread = yaffs_read(BMPFile, bmpbuffer, st.st_size); //读取数据
    result = yaffsfs_GetLastError();
	if(result != 0) 
    	return 3;

    //taskEXIT_CRITICAL();//退出临界区

	switch(mode)
	{
		case 0:	//在指定位置显示图片
			if((member == 1) && (denom == 1)) //无需缩放，直接绘制
			{
//			    hItem = WM_GetDialogItem(hWin, (GUI_ID_USER + 0x0A));
                XSize = GUI_BMP_GetXSize(bmpbuffer);	//获取图片的X轴大小
                YSize = GUI_BMP_GetYSize(bmpbuffer);	//获取图片的Y轴大小
                imageHandle = IMAGE_CreateEx(x,y,XSize,YSize,
                                     hWin,WM_CF_SHOW,IMAGE_CF_TILE,GUI_ID_IMAGE0);

				IMAGE_SetBMP(imageHandle, bmpbuffer, st.st_size);//GUI_BMP_Draw(bmpbuffer,x,y);	//在指定位置显示BMP图片
			}else //否则图片需要缩放
			{
				GUI_BMP_DrawScaled(bmpbuffer,x,y,member,denom);
			}
			break;
		case 1:	//在LCD中间显示图片
			XSize = GUI_BMP_GetXSize(bmpbuffer);	//获取图片的X轴大小
			YSize = GUI_BMP_GetYSize(bmpbuffer);	//获取图片的Y轴大小
			if((member == 1) && (denom == 1)) //无需缩放，直接绘制
			{
				//在LCD中间显示图片
				//GUI_BMP_Draw(bmpbuffer,(lcddev.width-XSize)/2-1,(lcddev.height-YSize)/2-1);
			}else //否则图片需要缩放
			{
				//Xflag = (float)XSize*((float)member/(float)denom);
				//Yflag = (float)YSize*((float)member/(float)denom);
				//XSize = (lcddev.width-(int)Xflag)/2-1;
				//YSize = (lcddev.height-(int)Yflag)/2-1;
				//GUI_BMP_DrawScaled(bmpbuffer,XSize,YSize,member,denom);
			}
			break;
	}
	yaffs_close(BMPFile);				//关闭BMPFile文件
	return 0;
}
/** @brief 显示背景图片
 *
 * @param 路径；左上角位置；句柄
 * @param
 * @return
 *
 */
uint8_t readBackGroundNOFREE(char *BMPFileName)
{
	uint16_t bread;
	char result;
    struct yaffs_stat st;

    yaffs_stat(BMPFileName, &st);
    BMPFile_BCGROUND = yaffs_open(BMPFileName, O_RDONLY, 0);	//打开文件
    result = yaffsfs_GetLastError();
	//文件打开错误或者文件大于BMPMEMORYSIZE
	if((result != 0) || (st.st_size>BMPMEMORYSIZE)) 	
    	return 1;

    bmpBackGround = malloc(st.st_size);//申请内存

	if(bmpBackGround == NULL)
	{
        return 2;//分配失败
	}
    //printf_safe("bmp_file = %d\n", BMPFile_BCGROUND.obj.objsize);
    bread = yaffs_read(BMPFile_BCGROUND, bmpBackGround, st.st_size); //读取数据
	if(result != 0) 
    	return 3;

	yaffs_close(BMPFile_BCGROUND);				//关闭BMPFile文件
	return 0;
}
