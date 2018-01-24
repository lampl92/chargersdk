#include "siffontcreate.h"
#include "yaffsfs.h"
#include "sys_types.h"
#include "stringName.h"

//字体定义
GUI_FONT SIF12_Font;
GUI_FONT SIF16_Font;
GUI_FONT SIF24_Font;
GUI_FONT SIF36_Font;
GUI_FONT fontwryhct12aa4;
GUI_FONT fontwryhct16aa4;
GUI_FONT fontwryhct24aa4;
GUI_FONT fontwryhct36aa4;

//字库文件缓冲区
//u8 *SIF12FontBuff;
//u8 *SIF16FontBuff;
//u8 *SIF24FontBuff;
//u8 *SIF36FontBuff;

//SD卡获取字库文件
//fxpath:字库路径
//buffer:字库文件缓冲区
//files:文件
//返回值:0，失败；其他，得到的字库文件
u8 *GetFont_Info(u8 *fxpath)
{
    int fd;
    int result;
	u32 bread;
    int fsize;
    struct yaffs_stat st;
	u8 *FontBuffer;

	fd=yaffs_open(fxpath, O_RDONLY, 0);	//打开字库文件
    yaffs_stat(fxpath, &st);
    fsize = st.st_size;
    FontBuffer = malloc(fsize); //从外部SDRAM申请内存

    if(FontBuffer==NULL)
	{
		printf("内存申请失败\r\n");
    	return FontBuffer;
	}

    bread = yaffs_read(fd, FontBuffer, fsize); //读取数据
    if (bread != fsize)
    {
        ThrowFSCode(result = yaffs_get_error(), fxpath, "GetFont_Info()-read");
        free(FontBuffer);
    	FontBuffer=NULL;
    }
    yaffs_close(fd);
    return FontBuffer;
}


void create_sif_font(u8 *fxpath, GUI_FONT * pFont, const GUI_SIF_TYPE * pFontType)
{
    GUI_SIF_CreateFont(GetFont_Info(fxpath), pFont, pFontType);
}

void createfont()
{
    create_sif_font(pathstSIF12, &SIF12_Font, GUI_SIF_TYPE_PROP);
    create_sif_font(pathstSIF16, &SIF16_Font, GUI_SIF_TYPE_PROP);
    create_sif_font(pathstSIF24, &SIF24_Font, GUI_SIF_TYPE_PROP);
    create_sif_font(pathstSIF36, &SIF36_Font, GUI_SIF_TYPE_PROP);
    create_sif_font(pathfontwryhct12aa4, &fontwryhct12aa4, GUI_SIF_TYPE_PROP_AA4);
    create_sif_font(pathfontwryhct16aa4, &fontwryhct16aa4, GUI_SIF_TYPE_PROP_AA4);
    create_sif_font(pathfontwryhct24aa4, &fontwryhct24aa4, GUI_SIF_TYPE_PROP_AA4);
    create_sif_font(pathfontwryhct36aa4, &fontwryhct36aa4, GUI_SIF_TYPE_PROP_AA4);
}



////创建SIF12字体，供EMWIN使用
//void Create_SIF12(u8 *fxpath)
//{
//    u8 *SIF12FontBuff;
//    SIF12FontBuff=GetFont_Info(fxpath);
//    if(SIF12FontBuff!=NULL)
//    {
//        //创建SIF格式字体
//        GUI_SIF_CreateFont(SIF12FontBuff,&SIF12_Font,GUI_SIF_TYPE_PROP);                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                        
//    }
//}
//
////创建SIF16字体，供EMWIN使用
//void Create_SIF16(u8 *fxpath)
//{
//    u8 *SIF16FontBuff;
//    SIF16FontBuff=GetFont_Info(fxpath);
//    if(SIF16FontBuff!=NULL)
//    {
//        //创建SIF格式字体
//        GUI_SIF_CreateFont(SIF16FontBuff,&SIF16_Font,GUI_SIF_TYPE_PROP);
//    }
//}
//
////创建SIF24字体，供EMWIN使用
//void Create_SIF24(u8 *fxpath)
//{
//    u8 *SIF24FontBuff;
//    SIF24FontBuff=GetFont_Info(fxpath);
//    if(SIF24FontBuff!=NULL)
//    {
//        //创建SIF格式字体
//        GUI_SIF_CreateFont(SIF24FontBuff,&SIF24_Font,GUI_SIF_TYPE_PROP);
//    }
//}
//
////创建SIF36字体，供EMWIN使用
//void Create_SIF36(u8 *fxpath)
//{
//    u8 *SIF36FontBuff;
//    SIF36FontBuff=GetFont_Info(fxpath);
//    if(SIF36FontBuff!=NULL)
//    {
//        //创建SIF格式字体
//        GUI_SIF_CreateFont(SIF36FontBuff,&SIF36_Font,GUI_SIF_TYPE_PROP);
//    }
//}
