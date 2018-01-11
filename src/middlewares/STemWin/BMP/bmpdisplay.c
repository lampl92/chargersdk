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



