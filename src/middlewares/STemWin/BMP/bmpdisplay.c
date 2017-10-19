#include "bmpdisplay.h"
#include "GUI.h"
#include "DIALOG.h"

static FIL BMPFile;
FIL BMPFile_BCGROUND;
FIL BMPFile_ENCODE;
static FIL ScrSortFile;                 //��Ļ��ͼ�ļ�
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
static int BmpGetData(void * p, const uint8_t ** ppData, unsigned NumBytesReq, uint32_t Off)
{
	static int readaddress=0;
	FIL * phFile;
	UINT NumBytesRead;

	phFile = (FIL *)p;

	if (NumBytesReq > sizeof(bmpBuffer))
	{
		NumBytesReq = sizeof(bmpBuffer);
	}

	//�ƶ�ָ�뵽Ӧ�ö�ȡ��λ��
	if(Off == 1) readaddress = 0;
	else readaddress=Off;

    taskENTER_CRITICAL();	//�ٽ���

	f_lseek(phFile,readaddress);

	//��ȡ���ݵ���������
	f_read(phFile,bmpBuffer,NumBytesReq,&NumBytesRead);

    taskEXIT_CRITICAL();	//�˳��ٽ���

	*ppData = (uint8_t *)bmpBuffer;
	return NumBytesRead;//���ض�ȡ�����ֽ���
}

//��ָ��λ����ʾ���ص�RAM�е�BMPͼƬ
//BMPFileName:ͼƬ��SD�����������洢�豸�е�·��(���ļ�ϵͳ֧�֣�)
//mode:��ʾģʽ
//		0 ��ָ��λ����ʾ���ɲ���x,yȷ����ʾλ��
//		1 ��LCD�м���ʾͼƬ����ѡ���ģʽ��ʱ�����x,y��Ч��
//x:ͼƬ���Ͻ���LCD�е�x��λ��(������modeΪ1ʱ���˲�����Ч)
//y:ͼƬ���Ͻ���LCD�е�y��λ��(������modeΪ1ʱ���˲�����Ч)
//member:  ���ű����ķ�����
//denom:���ű����ķ�ĸ��
//����ֵ:0 ��ʾ����,���� ʧ��
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

	result = f_open(&BMPFile,(const TCHAR*)BMPFileName,FA_READ);	//���ļ�
	//�ļ��򿪴�������ļ�����BMPMEMORYSIZE
	if((result != FR_OK) || (BMPFile.obj.objsize>BMPMEMORYSIZE)) 	return 1;

	bmpbuffer = malloc(BMPFile.obj.objsize);//�����ڴ�
	if(bmpbuffer == NULL) return 2;

    //taskENTER_CRITICAL();	//�ٽ���

	result = f_read(&BMPFile,bmpbuffer,BMPFile.obj.objsize,(UINT *)&bread); //��ȡ����
	if(result != FR_OK) return 3;//        ���������ڴ�й©���գ�������������

    //taskEXIT_CRITICAL();//�˳��ٽ���

	switch(mode)
	{
		case 0:	//��ָ��λ����ʾͼƬ
			if((member == 1) && (denom == 1)) //�������ţ�ֱ�ӻ���
			{
				GUI_BMP_Draw(bmpbuffer,x,y);	//��ָ��λ����ʾBMPͼƬ
			}else //����ͼƬ��Ҫ����
			{
				GUI_BMP_DrawScaled(bmpbuffer,x,y,member,denom);
			}
			break;
		case 1:	//��LCD�м���ʾͼƬ
			XSize = GUI_BMP_GetXSize(bmpbuffer);	//��ȡͼƬ��X���С
			YSize = GUI_BMP_GetYSize(bmpbuffer);	//��ȡͼƬ��Y���С
			if((member == 1) && (denom == 1)) //�������ţ�ֱ�ӻ���
			{
				//��LCD�м���ʾͼƬ
				//GUI_BMP_Draw(bmpbuffer,(lcddev.width-XSize)/2-1,(lcddev.height-YSize)/2-1);
			}else //����ͼƬ��Ҫ����
			{
				//Xflag = (float)XSize*((float)member/(float)denom);
				//Yflag = (float)YSize*((float)member/(float)denom);
				//XSize = (lcddev.width-(int)Xflag)/2-1;
				//YSize = (lcddev.height-(int)Yflag)/2-1;
				//GUI_BMP_DrawScaled(bmpbuffer,XSize,YSize,member,denom);
			}
			break;
	}
	f_close(&BMPFile);				//�ر�BMPFile�ļ�
	free(bmpbuffer);		//�ͷ��ڴ�
	return 0;
}
#endif

//��ָ��λ����ʾ������ص�RAM�е�BMPͼƬ(���ļ�ϵͳ֧�֣�����СRAM���Ƽ�ʹ�ô˷�����)
//BMPFileName:ͼƬ��SD�����������洢�豸�е�·��
//mode:��ʾģʽ
//		0 ��ָ��λ����ʾ���в���x,yȷ����ʾλ��
//		1 ��LCD�м���ʾͼƬ����ѡ���ģʽ��ʱ�����x,y��Ч��
//x:ͼƬ���Ͻ���LCD�е�x��λ��(������modeΪ1ʱ���˲�����Ч)
//y:ͼƬ���Ͻ���LCD�е�y��λ��(������modeΪ1ʱ���˲�����Ч)
//member:  ���ű����ķ�����
//denom:���ű����ķ�ĸ��
//����ֵ:0 ��ʾ����,���� ʧ��
int dispbmpex(uint8_t *BMPFileName,uint8_t mode,uint32_t x,uint32_t y,int member,int denom)
{
	char result;
	int XSize,YSize;
	float Xflag,Yflag;

	result = f_open(&BMPFile,(const TCHAR*)BMPFileName,FA_READ);	//���ļ�
	//�ļ��򿪴���
	if(result != FR_OK) 	return 1;

	switch(mode)
	{
		case 0:	//��ָ��λ����ʾͼƬ
			if((member == 1) && (denom == 1)) //�������ţ�ֱ�ӻ���
			{
				GUI_BMP_DrawEx(BmpGetData,&BMPFile,x,y);//��ָ��λ����ʾBMPͼƬ
			}else //����ͼƬ��Ҫ����
			{
				GUI_BMP_DrawScaledEx(BmpGetData,&BMPFile,x,y,member,denom);
			}
			break;
		case 1:	//��LCD�м���ʾͼƬ
			XSize = GUI_BMP_GetXSizeEx(BmpGetData,&BMPFile);	//��ȡͼƬ��X���С
			YSize = GUI_BMP_GetYSizeEx(BmpGetData,&BMPFile);	//��ȡͼƬ��Y���С
			if((member == 1) && (denom == 1)) //�������ţ�ֱ�ӻ���
			{
				//��LCD�м���ʾͼƬ
				//GUI_BMP_DrawEx(BmpGetData,&BMPFile,(lcddev.width-XSize)/2-1,(lcddev.height-YSize)/2-1);
			}else //����ͼƬ��Ҫ����
			{
				//Xflag = (float)XSize*((float)member/(float)denom);
				//Yflag = (float)YSize*((float)member/(float)denom);
				//XSize = (lcddev.width-(int)Xflag)/2-1;
				//YSize = (lcddev.height-(int)Yflag)/2-1;
				//GUI_BMP_DrawScaledEx(BmpGetData,&BMPFile,XSize,YSize,member,denom);
			}
			break;
	}
	f_close(&BMPFile);		//�ر�BMPFile�ļ�
	return 0;
}

//�˺�����GUI_BMP_Serialize()���ã��������ļ�д���ֽ�
static void _WriteByte2File(uint8_t Data, void * p)
{
	UINT nWritten;
	f_write((FIL*)p,&Data,1,&nWritten);
}

//�õ�һ���ļ���
//pname:�õ����ļ���
void emwinbmp_new_pathname(uint8_t *pname,uint8_t *ppath)
{
    uint8_t res;
    uint16_t index=0;
    while(index<0XFFFF)
    {
        sprintf((char*)pname,ppath,index);
        res=f_open(&ScrSortFile,(const TCHAR*)pname,FA_READ);//���Դ�����ļ�
        if(res==FR_NO_FILE)break;   //���ļ���������=����������Ҫ��
        else if(res==FR_OK) f_close(&ScrSortFile);//����򿪳ɹ����͹رյ�
        index++;
    }
}
//����һ��BMPͼƬ�����浽SD����,ʵ����Ļ��ͼ���ܣ�
//*filepath:�ļ�·��
//x0:����BMP�ļ���X��ʼλ��
//y0:����BMP�ļ���Y��ʼλ��
//Xsize:X�Ĵ�С
//Ysize:Y�Ĵ�С
void create_bmppicture(uint8_t *filename,int x0,int y0,int Xsize,int Ysize)
{
	static FIL hFile;
	//����һ���ļ���·��Ϊfilename,����ļ�ԭ���ʹ��ڵĻ��ᱻ�½����ļ����ǵ���
	f_open(&hFile,(const TCHAR*)filename,FA_READ|FA_WRITE|FA_CREATE_ALWAYS);
	GUI_BMP_SerializeEx(_WriteByte2File,x0,y0,Xsize,Ysize,&hFile);
	f_close(&hFile);	//�ر��ļ�
}

void bmpdisplay(uint8_t *ppath)
{
    //GUI_DispStringHCenterAt("��ָ��λ����ʾһ�ż��ص�RAM�е�BMPͼƬ",400,5);
    dispbmp(ppath,0,0,0,1,1);
    //GUI_Clear();
}

/** @brief
 *
 * @param
 * @param
 * @return
 *  **********ע�����л�����ʱҪʹ��free�ͷŵ�ͼƬ���ڴ�
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

//    if(is_free == 1)
//    {
//        free(bmpbuffer);
//        return 0;
//    }

	result = f_open(&BMPFile,(const TCHAR*)BMPFileName,FA_READ);	//���ļ�
	//�ļ��򿪴�������ļ�����BMPMEMORYSIZE
	if((result != FR_OK) || (BMPFile.obj.objsize>BMPMEMORYSIZE)) 	return 1;

	bmpbuffer = malloc(BMPFile.obj.objsize);//�����ڴ�

	if(bmpbuffer == NULL)
	{
        return 2;//����ʧ��
	}

    //taskENTER_CRITICAL();	//�ٽ���

	result = f_read(&BMPFile,bmpbuffer,BMPFile.obj.objsize,(UINT *)&bread); //��ȡ����
	if(result != FR_OK) return 3;

    //taskEXIT_CRITICAL();//�˳��ٽ���

	switch(mode)
	{
		case 0:	//��ָ��λ����ʾͼƬ
			if((member == 1) && (denom == 1)) //�������ţ�ֱ�ӻ���
			{
//			    hItem = WM_GetDialogItem(hWin, (GUI_ID_USER + 0x0A));
                XSize = GUI_BMP_GetXSize(bmpbuffer);	//��ȡͼƬ��X���С
                YSize = GUI_BMP_GetYSize(bmpbuffer);	//��ȡͼƬ��Y���С
                imageHandle = IMAGE_CreateEx(x,y,XSize,YSize,
                                     hWin,WM_CF_SHOW,IMAGE_CF_TILE,GUI_ID_IMAGE0);

				IMAGE_SetBMP(imageHandle, bmpbuffer, BMPFile.obj.objsize);//GUI_BMP_Draw(bmpbuffer,x,y);	//��ָ��λ����ʾBMPͼƬ
			}else //����ͼƬ��Ҫ����
			{
				GUI_BMP_DrawScaled(bmpbuffer,x,y,member,denom);
			}
			break;
		case 1:	//��LCD�м���ʾͼƬ
			XSize = GUI_BMP_GetXSize(bmpbuffer);	//��ȡͼƬ��X���С
			YSize = GUI_BMP_GetYSize(bmpbuffer);	//��ȡͼƬ��Y���С
			if((member == 1) && (denom == 1)) //�������ţ�ֱ�ӻ���
			{
				//��LCD�м���ʾͼƬ
				//GUI_BMP_Draw(bmpbuffer,(lcddev.width-XSize)/2-1,(lcddev.height-YSize)/2-1);
			}else //����ͼƬ��Ҫ����
			{
				//Xflag = (float)XSize*((float)member/(float)denom);
				//Yflag = (float)YSize*((float)member/(float)denom);
				//XSize = (lcddev.width-(int)Xflag)/2-1;
				//YSize = (lcddev.height-(int)Yflag)/2-1;
				//GUI_BMP_DrawScaled(bmpbuffer,XSize,YSize,member,denom);
			}
			break;
	}
	f_close(&BMPFile);				//�ر�BMPFile�ļ�
	return 0;
}
/** @brief ��ʾ����ͼƬ
 *
 * @param ·�������Ͻ�λ�ã����
 * @param
 * @return
 *
 */
uint8_t readBackGroundNOFREE(char *BMPFileName)
{
	uint16_t bread;
	char result;

	result = f_open(&BMPFile_BCGROUND,(const TCHAR*)BMPFileName,FA_READ);	//���ļ�
	//�ļ��򿪴�������ļ�����BMPMEMORYSIZE
	if((result != FR_OK) || (BMPFile_BCGROUND.obj.objsize>BMPMEMORYSIZE)) 	return 1;

	bmpBackGround = malloc(BMPFile_BCGROUND.obj.objsize);//�����ڴ�

	if(bmpBackGround == NULL)
	{
        return 2;//����ʧ��
	}

	result = f_read(&BMPFile_BCGROUND,bmpBackGround,BMPFile_BCGROUND.obj.objsize,(UINT *)&bread); //��ȡ����
	if(result != FR_OK) return 3;

	f_close(&BMPFile_BCGROUND);				//�ر�BMPFile�ļ�
	return 0;
}
