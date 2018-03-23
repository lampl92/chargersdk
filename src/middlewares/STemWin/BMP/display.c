/*
*
*模块名称;创建存储设备
*
*文件名称：display.c
*
*说明：
*   此模块用来创建交互界面所有所需的所有GUI_MEMDEV_Handle（存储设备）
*
*作者：杨康
*
*email：y2603012723@163.com
*
*Copyright (C)，2018-2，北京动力源科技股份有限公司
*
*/

#include "display.h"
#include "GUI.h"
#include "DIALOG.h"
#include "yaffsfs.h"
#include "stringName.h"
#include "HMI_Start.h"
#include "interface.h"

#pragma region MyRegion
p_inf *HomeImage;
p_inf *SignalImage0;
p_inf *SignalImage1;
p_inf *SignalImage2;
p_inf *SignalImage3;
p_inf *SignalImage4;
p_inf *SignalImage5;

p_inf *CardInfoImage;
p_inf *GetCardInfoImage;
p_inf *CardUnregisteredImage;
p_inf *CardArrearsImage;
p_inf *PleaseConnectPlugImage;
p_inf *CardInfoVoidImage;

p_inf *ChargingImage;
p_inf *cartoonImage0;
p_inf *cartoonImage1;
p_inf *cartoonImage2;
p_inf *cartoonImage3;
p_inf *cartoonImage4;
p_inf *cartoonImage5;
p_inf *StopByCardImage;
p_inf *StopByQRImage;
p_inf *ChargingVoidImage;

p_inf *ChargeDoneImage;
p_inf *OrderUploadImage;
p_inf *NormalDoneImage;
p_inf *FullDoneImage;
p_inf *DevErrDoneImage;
p_inf *MoneyNotEnoughDoneImage;
p_inf *ChargeDoneVoidImage;

p_inf *AdvertisementImage;

p_inf *DtaFileCheckboxChosen;
p_inf *DtaFileCheckboxNotChosen;
p_inf *DtaFileBeijing;
p_inf *DtaFileCheckboxDisable;
GUI_BITMAP BitmapCheckboxChosen;
GUI_BITMAP BitmapCheckboxNotChosen;
GUI_BITMAP BitmapCheckboxDisable;
GUI_BITMAP BitmapBeijing;			  
#pragma endregion

#pragma region MyRegion
        //公用图片“退出”
        GUI_BITMAP BitmapQuit;
        GUI_BITMAP BitmapQuitPress;
        GUI_BITMAP BitmapbackQuit;
        GUI_BITMAP BitmapbackQuitPress;

        //选择页图片
        GUI_BITMAP BitmapSelectGunBack;
        GUI_BITMAP BitmapSelectGunAbottonNotpress;
        GUI_BITMAP BitmapSelectGunBbottonNotpress;
        GUI_BITMAP BitmapSelectGunAbottonPress;
        GUI_BITMAP BitmapSelectGunBbottonPress;

        //选择充电模式页图片
        GUI_BITMAP Bitmapselectpatternback;
        GUI_BITMAP Bitmapselectpatternelectricnumber;
        GUI_BITMAP Bitmapselectpatternfull;
        GUI_BITMAP Bitmapselectpatternmoneynumber;
        GUI_BITMAP Bitmapselectpatterntime;

        GUI_BITMAP Bitmapselectpatternelectricnumberpress;
        GUI_BITMAP Bitmapselectpatternfullpress;
        GUI_BITMAP Bitmapselectpatternmoneynumberpress;
        GUI_BITMAP Bitmapselectpatterntimepress;

        GUI_BITMAP Bitmapselectpatternunityuan;
        GUI_BITMAP Bitmapselectpatternunitdu;
        GUI_BITMAP Bitmapselectpatternunitfen;
        GUI_BITMAP Bitmapselectpatternunitno;

        GUI_BITMAP BitmapKeyboardback;
        GUI_BITMAP BitmapKeyboard0;
        GUI_BITMAP BitmapKeyboard1;
        GUI_BITMAP BitmapKeyboard2;
        GUI_BITMAP BitmapKeyboard3;
        GUI_BITMAP BitmapKeyboard4;
        GUI_BITMAP BitmapKeyboard5;
        GUI_BITMAP BitmapKeyboard6;
        GUI_BITMAP BitmapKeyboard7;
        GUI_BITMAP BitmapKeyboard8;
        GUI_BITMAP BitmapKeyboard9;
        GUI_BITMAP BitmapKeyboardBackspace;
        GUI_BITMAP BitmapKeyboardesc;
        GUI_BITMAP BitmapKeyboardok;
        GUI_BITMAP BitmapKeyboardpoint;

        GUI_BITMAP BitmapKeyboardbackpress;
        GUI_BITMAP BitmapKeyboard0press;
        GUI_BITMAP BitmapKeyboard1press;
        GUI_BITMAP BitmapKeyboard2press;
        GUI_BITMAP BitmapKeyboard3press;
        GUI_BITMAP BitmapKeyboard4press;
        GUI_BITMAP BitmapKeyboard5press;
        GUI_BITMAP BitmapKeyboard6press;
        GUI_BITMAP BitmapKeyboard7press;
        GUI_BITMAP BitmapKeyboard8press;
        GUI_BITMAP BitmapKeyboard9press;
        GUI_BITMAP BitmapKeyboardBackspacepress;
        GUI_BITMAP BitmapKeyboardescpress;
        GUI_BITMAP BitmapKeyboardokpress;
        GUI_BITMAP BitmapKeyboardpointpress;

        //卡信息页图片
        GUI_BITMAP Bitmapcardinfoarrears;
        GUI_BITMAP Bitmapcardinfoback;
GUI_BITMAP Bitmapcardinforeadycharging;
        GUI_BITMAP Bitmapcardinfoget;
        GUI_BITMAP Bitmapcardinfoplug;
        GUI_BITMAP Bitmapcardinfounregister;

        //主页图片
        GUI_BITMAP Bitmaphomeback;
        GUI_BITMAP BitmaphomegunAchargedone;
        GUI_BITMAP BitmaphomegunAcharging;
        GUI_BITMAP BitmaphomegunAerror;
        GUI_BITMAP BitmaphomegunAfree;
        GUI_BITMAP BitmaphomegunBchargedone;
        GUI_BITMAP BitmaphomegunBcharging;
        GUI_BITMAP BitmaphomegunBerror;
        GUI_BITMAP BitmaphomegunBfree;
        GUI_BITMAP Bitmaphomegunlookinfo;
        GUI_BITMAP Bitmaphomegunscancode;
        GUI_BITMAP Bitmaphomegunlookinfopress;
        GUI_BITMAP Bitmaphomegunscancodepress;
        GUI_BITMAP Bitmaphomesignal0;
        GUI_BITMAP Bitmaphomesignal1;
        GUI_BITMAP Bitmaphomesignal2;
        GUI_BITMAP Bitmaphomesignal3;

        //提示信息页图片
        GUI_BITMAP Bitmapchargedoneinfo;
        GUI_BITMAP Bitmapcharginginfo;				  
#pragma endregion

#pragma region MyRegion
//主页储存设备
GUI_MEMDEV_Handle Memdevhomeback;
GUI_MEMDEV_Handle MemdevhomegunAchargedone;
GUI_MEMDEV_Handle MemdevhomegunAcharging;
GUI_MEMDEV_Handle MemdevhomegunAerror;
GUI_MEMDEV_Handle MemdevhomegunAfree;
GUI_MEMDEV_Handle MemdevhomegunBchargedone;
GUI_MEMDEV_Handle MemdevhomegunBcharging;
GUI_MEMDEV_Handle MemdevhomegunBerror;
GUI_MEMDEV_Handle MemdevhomegunBfree;
GUI_MEMDEV_Handle Memdevhomegunlookinfo;
GUI_MEMDEV_Handle Memdevhomegunscancode;
GUI_MEMDEV_Handle Memdevhomegunlookinfopress;
GUI_MEMDEV_Handle Memdevhomegunscancodepress;
GUI_MEMDEV_Handle Memdevhomegunerror;
GUI_MEMDEV_Handle Memdevhomesignal0;
GUI_MEMDEV_Handle Memdevhomesignal1;
GUI_MEMDEV_Handle Memdevhomesignal2;
GUI_MEMDEV_Handle Memdevhomesignal3;
GUI_MEMDEV_Handle Memdevhomesignal4;
GUI_MEMDEV_Handle Memdevhomesignal5;
GUI_MEMDEV_Handle Memdevhomechargedoneinfo;
GUI_MEMDEV_Handle Memdevhomecharginginfo;
GUI_MEMDEV_Handle Memdevhomechargehelp;
GUI_MEMDEV_Handle Memdevhomechargehelppress;
GUI_MEMDEV_Handle Memdevhomehelpinfo;

//选枪页
GUI_MEMDEV_Handle MemdevSelectGunBack;
GUI_MEMDEV_Handle MemdevSelectGunAbottonNotpress;
GUI_MEMDEV_Handle MemdevSelectGunBbottonNotpress;
GUI_MEMDEV_Handle MemdevSelectGunAbottonPress;
GUI_MEMDEV_Handle MemdevSelectGunBbottonPress;		  

//选模式
GUI_MEMDEV_Handle Memdevselectpatternback;
GUI_MEMDEV_Handle Memdevselectpatternelectricnumber;
GUI_MEMDEV_Handle Memdevselectpatternfull;
GUI_MEMDEV_Handle Memdevselectpatternmoneynumber;
GUI_MEMDEV_Handle Memdevselectpatterntime;

GUI_MEMDEV_Handle Memdevselectpatternelectricnumberpress;
GUI_MEMDEV_Handle Memdevselectpatternfullpress;
GUI_MEMDEV_Handle Memdevselectpatternmoneynumberpress;
GUI_MEMDEV_Handle Memdevselectpatterntimepress;

GUI_MEMDEV_Handle Memdevselectpatternunityuan;
GUI_MEMDEV_Handle Memdevselectpatternunitdu;
GUI_MEMDEV_Handle Memdevselectpatternunitfen;
GUI_MEMDEV_Handle Memdevselectpatternunitno;

GUI_MEMDEV_Handle Memdevselectpatternkeyboard;
GUI_MEMDEV_Handle Memdevselectpatternkeyboardpress;

//提示信息页图片
GUI_MEMDEV_Handle Memdevchargedoneinfo;
GUI_MEMDEV_Handle Memdevcharginginfo;	

//公用图片“退出”
GUI_MEMDEV_Handle MemdevQuit;
GUI_MEMDEV_Handle MemdevQuitPress;
GUI_MEMDEV_Handle MemdevbackQuit;
GUI_MEMDEV_Handle MemdevbackQuitPress;

//卡信息页图片
GUI_MEMDEV_Handle Memdevcardinfoback;
GUI_MEMDEV_Handle Memdevcardinfocardconditionnotok;
GUI_MEMDEV_Handle Memdevcardinfochargingok;
GUI_MEMDEV_Handle Memdevcardinfonettimeout;
GUI_MEMDEV_Handle Memdevcardinfopleaseplug;
GUI_MEMDEV_Handle Memdevcardinfoplugtimeout;
GUI_MEMDEV_Handle Memdevcardinforeadystart;
//GUI_MEMDEV_Handle Memdevcardinfoarrears;
//GUI_MEMDEV_Handle MemdevcardinfoUnavailable;
//GUI_MEMDEV_Handle Memdevcardinfostartfail;
//GUI_MEMDEV_Handle Memdevcardinfoback;
//GUI_MEMDEV_Handle Memdevcardinforeadycharging;
//GUI_MEMDEV_Handle Memdevcardinfoget;
//GUI_MEMDEV_Handle Memdevcardinfoplug;
//GUI_MEMDEV_Handle Memdevcardinfounregister;

#pragma endregion

/*
*函数功能：返回图片信息结构体p_inf
*    参数：图片路径字符串picture_file_path
*  返回值：返回图片信息，NULL时为读取图片信息失败
*    作者：杨康
*/
p_inf * readPicInf(char *pfilepath)
{
    int  fileDescriptor;//文件体或者称文件描述符
    uint32_t readByteResult;//作为f_read的最后一个参数，接受读到了多少个字节
    char fOptResult;//接受文件操作返回值（也就是返回结果）
    p_inf *infReturn;//图片信息结构体，最后作为返回值
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
    int  fileDescriptor;//文件体或者称文件描述符
    int XSize, YSize;
    GUI_HMEM hMem;
    GUI_MEMDEV_Handle hMemBMP;
    uint32_t readByteResult;//作为f_read的最后一个参数，接受读到了多少个字节
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
    /* 申请一块内存空间 并且将其清零 */
    hMem = GUI_ALLOC_AllocZero(st.st_size);
    /* 将申请到内存的句柄转换成指针类型 */
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
    /* 释放动态内存 hMem */
    GUI_ALLOC_Free(hMem);
    yaffs_close(fileDescriptor);
    return hMemBMP;
}

void createGUI_BITMAP()
{
    GUI_LOGPALETTE Palette;	
    DtaFileCheckboxChosen = readPicInf(pathCheckboxDta);
    DtaFileCheckboxNotChosen = readPicInf(pathCheckboxNotDta);
    DtaFileCheckboxDisable = readPicInf(pathCheckboxDisable);
    DtaFileBeijing = readPicInf(pathBeijingDta);
    GUI_CreateBitmapFromStream565(&BitmapCheckboxChosen, &Palette, DtaFileCheckboxChosen->pfilestring);
    GUI_CreateBitmapFromStream565(&BitmapCheckboxNotChosen, &Palette, DtaFileCheckboxNotChosen->pfilestring);
    GUI_CreateBitmapFromStream565(&BitmapCheckboxDisable, &Palette, DtaFileCheckboxDisable->pfilestring);
    GUI_CreateBitmapFromStream565(&BitmapBeijing, &Palette, DtaFileBeijing->pfilestring);
    //公用图片读取
    BitmapQuit = readDtafile(pathQuit);
    BitmapQuitPress = readDtafile(pathQuitPress);
    BitmapbackQuit = readDtafile(pathbackquit);
    BitmapbackQuitPress = readDtafile(pathbackquitpress);
    //选择页图片读取
    BitmapSelectGunAbottonNotpress = readDtafile(pathSelectGunAbottonNotpress);
    BitmapSelectGunAbottonPress = readDtafile(pathSelectGunAbottonPress);
    BitmapSelectGunBack = readDtafile(pathSelectGunBack);
    BitmapSelectGunBbottonNotpress = readDtafile(pathSelectGunBbottonNotpress);
    BitmapSelectGunBbottonPress = readDtafile(pathSelectGunBbottonPress);
        
    //选择充电模式页图片读取
    Bitmapselectpatternback = readDtafile(pathselectpatternback);
    Bitmapselectpatternelectricnumber = readDtafile(pathselectpatternelectricnumber);
    Bitmapselectpatternfull = readDtafile(pathselectpatternfull);
    Bitmapselectpatternmoneynumber = readDtafile(pathselectpatternmoneynumber);
    Bitmapselectpatterntime = readDtafile(pathselectpatterntime);

    Bitmapselectpatternelectricnumberpress = readDtafile(pathselectpatternelectricnumberpress);
    Bitmapselectpatternfullpress = readDtafile(pathselectpatternfullpress);
    Bitmapselectpatternmoneynumberpress = readDtafile(pathselectpatternmoneynumberpress);
    Bitmapselectpatterntimepress = readDtafile(pathselectpatterntimepress);

    Bitmapselectpatternunityuan = readDtafile(pathselectpatternunityuan);
    Bitmapselectpatternunitdu = readDtafile(pathselectpatternunitdu);
    Bitmapselectpatternunitfen = readDtafile(pathselectpatternunitfen);
    Bitmapselectpatternunitno = readDtafile(pathselectpatternunitno);
        
    BitmapKeyboardback = readDtafile(pathKeyboardback);
        
    BitmapKeyboard0 = readDtafile(pathKeyboard0);
    BitmapKeyboard1 = readDtafile(pathKeyboard1);
    BitmapKeyboard2 = readDtafile(pathKeyboard2);
    BitmapKeyboard3 = readDtafile(pathKeyboard3);
    BitmapKeyboard4 = readDtafile(pathKeyboard4);
    BitmapKeyboard5 = readDtafile(pathKeyboard5);
    BitmapKeyboard6 = readDtafile(pathKeyboard6);
    BitmapKeyboard7 = readDtafile(pathKeyboard7);
    BitmapKeyboard8 = readDtafile(pathKeyboard8);
    BitmapKeyboard9 = readDtafile(pathKeyboard9);
    BitmapKeyboardBackspace = readDtafile(pathKeyboardBackspace);
    BitmapKeyboardesc = readDtafile(pathKeyboardesc);
    BitmapKeyboardok = readDtafile(pathKeyboardok);
    BitmapKeyboardpoint = readDtafile(pathKeyboardpoint);

    BitmapKeyboard0press = readDtafile(pathKeyboard0press);
    BitmapKeyboard1press = readDtafile(pathKeyboard1press);
    BitmapKeyboard2press = readDtafile(pathKeyboard2press);
    BitmapKeyboard3press = readDtafile(pathKeyboard3press);
    BitmapKeyboard4press = readDtafile(pathKeyboard4press);
    BitmapKeyboard5press = readDtafile(pathKeyboard5press);
    BitmapKeyboard6press = readDtafile(pathKeyboard6press);
    BitmapKeyboard7press = readDtafile(pathKeyboard7press);
    BitmapKeyboard8press = readDtafile(pathKeyboard8press);
    BitmapKeyboard9press = readDtafile(pathKeyboard9press);
    BitmapKeyboardBackspacepress = readDtafile(pathKeyboardBackspacepress);
    BitmapKeyboardescpress = readDtafile(pathKeyboardescpress);
    BitmapKeyboardokpress = readDtafile(pathKeyboardokpress);
    BitmapKeyboardpointpress = readDtafile(pathKeyboardpointpress);
        
    //卡信息页图片
    //Bitmapcardinfoarrears = readDtafile(pathcardinfoarrears);
//    Bitmapcardinfoback = readDtafile(pathcardinfoback);
//    Bitmapcardinforeadycharging = readDtafile(pathcardinforeadycharging);
//    Bitmapcardinfoget = readDtafile(pathcardinfoget);
//    Bitmapcardinfoplug = readDtafile(pathcardinfoplug);
    //Bitmapcardinfounregister = readDtafile(pathcardinfounregister);
        
    //主页图片读取
    Bitmaphomeback = readDtafile(pathhomeback);
    BitmaphomegunAchargedone = readDtafile(pathhomegunAchargedone);
    BitmaphomegunAcharging = readDtafile(pathhomegunAcharging);
    BitmaphomegunAerror = readDtafile(pathhomegunAerror);
    BitmaphomegunAfree = readDtafile(pathhomegunAfree);
    BitmaphomegunBchargedone = readDtafile(pathhomegunBchargedone);
    BitmaphomegunBcharging = readDtafile(pathhomegunBcharging);
    BitmaphomegunBerror = readDtafile(pathhomegunBerror);
    BitmaphomegunBfree = readDtafile(pathhomegunBfree);
    Bitmaphomegunlookinfo = readDtafile(pathhomegunlookinfo);
    Bitmaphomegunscancode = readDtafile(pathhomegunscancode);
    Bitmaphomegunlookinfopress = readDtafile(pathhomegunlookinfopress);
    Bitmaphomegunscancodepress = readDtafile(pathhomegunscancodepress);   
    Bitmaphomesignal0 = readDtafile(pathhomesignal0);
    Bitmaphomesignal1 = readDtafile(pathhomesignal1);
    Bitmaphomesignal2 = readDtafile(pathhomesignal2);
    Bitmaphomesignal3 = readDtafile(pathhomesignal3);
        
    Bitmapchargedoneinfo = readDtafile(pathchargedoneinfo);
    Bitmapcharginginfo = readDtafile(pathcharginginfo);		
}

//返回-1时出错
int createQRinMemdev(const char * pText, GUI_MEMDEV_Handle mem)
{
    int memx;
    int qrx;
    int qry;
    GUI_HMEM qr_hmem;
//int QR_Width;//NUmber of "Moudle"
//int QR_Size;//Size of Bitmap in pixels
    GUI_QR_INFO QR_info_struct;//仿真时看值
    qr_hmem = GUI_QR_Create(pText, 6, GUI_QR_ECLEVEL_L, 0);
    GUI_QR_GetInfo(qr_hmem, &QR_info_struct);
    GUI_MEMDEV_Select(mem);
    memx =  GUI_MEMDEV_GetXSize(mem);
    if (memx < QR_info_struct.Size)
    {
        return -1;
    }
    qrx = (memx - QR_info_struct.Size) / 2;
    qry = 39;
    GUI_QR_Draw(qr_hmem, qrx, qry);
    GUI_MEMDEV_Select(0);  
    return 0;
}

void creatememdev()
{   
    //主页存储
    CON_t *pCON;   
    MemdevhomegunAfree = createMemdev(pathhomegunAfree);
    pCON = CONGetHandle(0);
    createQRinMemdev(pCON->info.strQRCode, MemdevhomegunAfree);
    
    MemdevhomegunBfree = createMemdev(pathhomegunBfree);
    pCON = CONGetHandle(0);
    createQRinMemdev(pCON->info.strQRCode, MemdevhomegunBfree);
    
    Memdevhomeback = createMemdev(pathhomeback);
    MemdevhomegunAchargedone = createMemdev(pathhomegunAchargedone);
    MemdevhomegunAcharging = createMemdev(pathhomegunAcharging);
    MemdevhomegunAerror = createMemdev(pathhomegunAerror); 
    MemdevhomegunBchargedone = createMemdev(pathhomegunBchargedone);
    MemdevhomegunBcharging = createMemdev(pathhomegunBcharging);
    MemdevhomegunBerror = createMemdev(pathhomegunBerror);   
    Memdevhomegunlookinfo = createMemdev(pathhomegunlookinfo);
    Memdevhomegunscancode = createMemdev(pathhomegunscancode);
    Memdevhomegunlookinfopress = createMemdev(pathhomegunlookinfopress);
    //Memdevhomegunscancodepress = createMemdev(pathhomegunscancodepress);
    Memdevhomegunerror = createMemdev(pathhomegunerror);
    Memdevhomesignal0 = createMemdev(pathhomesignal0);
    Memdevhomesignal1 = createMemdev(pathhomesignal1);
    Memdevhomesignal2 = createMemdev(pathhomesignal2);
    Memdevhomesignal3 = createMemdev(pathhomesignal3);
    Memdevhomesignal3 = createMemdev(pathhomesignal4);
    Memdevhomesignal3 = createMemdev(pathhomesignal5);
    Memdevhomechargedoneinfo = createMemdev(pathhomechargedoneinfo);
    Memdevhomecharginginfo = createMemdev(pathhomecharginginfo);
    Memdevhomechargehelp = createMemdev(pathhomechargehelp);
    Memdevhomechargehelppress = createMemdev(pathhomechargehelppress);
    Memdevhomehelpinfo = createMemdev(pathhomehelpinfo);
        
    //选枪
    MemdevSelectGunBack = createMemdev(pathSelectGunBack);
    MemdevSelectGunAbottonNotpress = createMemdev(pathSelectGunAbottonNotpress);
    MemdevSelectGunBbottonNotpress = createMemdev(pathSelectGunBbottonNotpress);
    MemdevSelectGunAbottonPress = createMemdev(pathSelectGunAbottonPress);
    MemdevSelectGunBbottonPress = createMemdev(pathSelectGunBbottonPress);
    
    //选模式
    Memdevselectpatternback = createMemdev(pathselectpatternback);
    Memdevselectpatternelectricnumber = createMemdev(pathselectpatternelectricnumber);
    Memdevselectpatternfull = createMemdev(pathselectpatternfull);
    Memdevselectpatternmoneynumber = createMemdev(pathselectpatternmoneynumber);
    Memdevselectpatterntime = createMemdev(pathselectpatterntime);

    Memdevselectpatternelectricnumberpress = createMemdev(pathselectpatternelectricnumberpress);
    Memdevselectpatternfullpress = createMemdev(pathselectpatternfullpress);
    Memdevselectpatternmoneynumberpress = createMemdev(pathselectpatternmoneynumberpress);
    Memdevselectpatterntimepress = createMemdev(pathselectpatterntimepress);

    Memdevselectpatternunityuan = createMemdev(pathselectpatternunityuan);
    Memdevselectpatternunitdu = createMemdev(pathselectpatternunitdu);
    Memdevselectpatternunitfen = createMemdev(pathselectpatternunitfen);
    Memdevselectpatternunitno = createMemdev(pathselectpatternunitno);

    Memdevselectpatternkeyboard = createMemdev(pathselectpatternkeyboard);
    Memdevselectpatternkeyboardpress = createMemdev(pathselectpatternkeyboardpress);
    
    //提示信息页图片
    //Memdevchargedoneinfo = createMemdev(pathchargedoneinfo);
    //Memdevcharginginfo = createMemdev(pathcharginginfo);

    //公用图片“退出”
    MemdevQuit = createMemdev(pathQuit);
    MemdevQuitPress = createMemdev(pathQuitPress);
    MemdevbackQuit = createMemdev(pathbackquit);
    MemdevbackQuitPress = createMemdev(pathbackquitpress);

    //卡信息页图片
    Memdevcardinfoback = createMemdev(pathcardinfoback);
    Memdevcardinfocardconditionnotok = createMemdev(pathcardinfocardconditionnotok);
    Memdevcardinfochargingok = createMemdev(pathcardinfochargingok);
    Memdevcardinfonettimeout = createMemdev(pathcardinfonettimeout);
    Memdevcardinfopleaseplug = createMemdev(pathcardinfopleaseplug);
    Memdevcardinfoplugtimeout = createMemdev(pathcardinfoplugtimeout);
    Memdevcardinforeadystart = createMemdev(pathcardinforeadystart);
    //Memdevcardinfoarrears = createMemdev(pathcardinfoarrears);
//    MemdevcardinfoUnavailable = createMemdev(pathcardinfoUnavailable);
//    Memdevcardinfostartfail = createMemdev(pathcardinfostartfail);
//    Memdevcardinfoback = createMemdev(pathcardinfoback);
//    Memdevcardinforeadycharging = createMemdev(pathcardinforeadycharging);
//    Memdevcardinfoget = createMemdev(pathcardinfoget);
//    Memdevcardinfoplug = createMemdev(pathcardinfoplug);
    //Memdevcardinfounregister = createMemdev(pathcardinfounregister);
    
        
}




