#include "bmpdisplay.h"
#include "GUI.h"
#include "DIALOG.h"
#include "yaffsfs.h"
/*
*�������ܣ�����ͼƬ��Ϣ�ṹ��p_inf
*    ������ͼƬ·���ַ���picture_file_path
*  ����ֵ������ͼƬ��Ϣ��NULLʱΪ��ȡͼƬ��Ϣʧ��
*    ���ߣ��
*/
p_inf * readPicInf(char *pfilepath)
{
    int  fileDescriptor;//�ļ�����߳��ļ�������
    uint32_t readByteResult;//��Ϊf_read�����һ�����������ܶ����˶��ٸ��ֽ�
    char fOptResult;//�����ļ���������ֵ��Ҳ���Ƿ��ؽ����
    p_inf *infReturn;//ͼƬ��Ϣ�ṹ�壬�����Ϊ����ֵ
    struct yaffs_stat st;
   
    fileDescriptor = yaffs_open(pfilepath, O_RDONLY, 0);
    if (fileDescriptor < 0)
    {
        ThrowFSCode(fOptResult = yaffs_get_error(), pfilepath, "readPicInf()-open");
        return NULL;
    }
    yaffs_stat(pfilepath, &st);
    infReturn = (p_inf *)malloc(sizeof(p_inf));
    if (infReturn == NULL) 
    {
        goto INFRETURN_MALLOC_ERR;
    }
    infReturn->pfilesize = st.st_size;
    infReturn->pfilestring = (char *)malloc(infReturn->pfilesize);
    if (infReturn->pfilestring == NULL)
    {
        goto INFRETURN_PFILESTRING_MALLOC_ERR;
    }
 
    readByteResult = yaffs_read(fileDescriptor, infReturn->pfilestring, infReturn->pfilesize);
    if (readByteResult != infReturn->pfilesize)
    {
        ThrowFSCode(fOptResult = yaffs_get_error(), pfilepath, "readPicInf()-read");
        goto F_READ_ERR;
    }

    yaffs_close(fileDescriptor);
    return infReturn;
F_READ_ERR:
    free(infReturn->pfilestring);
INFRETURN_PFILESTRING_MALLOC_ERR:
    free(infReturn);
INFRETURN_MALLOC_ERR:
    yaffs_close(fileDescriptor);
    return NULL;
}


GUI_BITMAP readDtafile(char *pathfile)
{
    p_inf *tmp;
    GUI_LOGPALETTE Palette;
    GUI_BITMAP bitmaptmp;
    tmp = readPicInf(pathfile);
    GUI_CreateBitmapFromStream565(&bitmaptmp, &Palette, tmp->pfilestring);
    return bitmaptmp;
}

GUI_MEMDEV_Handle createMemdev(char *pfilepath)
{
    int  fileDescriptor;//�ļ�����߳��ļ�������
    int XSize, YSize;
    GUI_HMEM hMem;
    GUI_MEMDEV_Handle hMemBMP;
    uint32_t readByteResult;//��Ϊf_read�����һ�����������ܶ����˶��ٸ��ֽ�
    struct yaffs_stat st;    
    char *_acBuffer;
    GUI_LOGPALETTE Palette;
    GUI_BITMAP bitmaptmp;
    
    yaffs_stat(pfilepath, &st);
    fileDescriptor = yaffs_open(pfilepath, O_RDONLY, 0);
    if (fileDescriptor < 0)
    {
        ThrowFSCode(fileDescriptor = yaffs_get_error(), pfilepath, "readPicInf()-open");
        return -1;
    }
    /* ����һ���ڴ�ռ� ���ҽ������� */
    hMem = GUI_ALLOC_AllocZero(st.st_size);
    /* �����뵽�ڴ�ľ��ת����ָ������ */
    _acBuffer = GUI_ALLOC_h2p(hMem);
 
    readByteResult = yaffs_read(fileDescriptor, _acBuffer, st.st_size);
    if (readByteResult != st.st_size)
    {
        ThrowFSCode(readByteResult = yaffs_get_error(), pfilepath, "readPicInf()-read");
        return -1;
    }

    GUI_CreateBitmapFromStream565(&bitmaptmp, &Palette, _acBuffer);
    
    XSize = bitmaptmp.XSize;
    YSize = bitmaptmp.YSize;
    
    hMemBMP = GUI_MEMDEV_CreateEx(0, 0, XSize, YSize, GUI_MEMDEV_NOTRANS);
    GUI_MEMDEV_Select(hMemBMP);
    GUI_DrawBitmap(&bitmaptmp, 0, 0);
    GUI_MEMDEV_Select(0);
    /* �ͷŶ�̬�ڴ� hMem */
    GUI_ALLOC_Free(hMem);
    yaffs_close(fileDescriptor);
    return hMemBMP;
}




