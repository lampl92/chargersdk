/**
* @file Touch_Disp_Module.c
* @brief 封装控件的显示函数
* @author share
* @version v1.0
* @date 2017-03-02
*/
#include "xbffontcreate.h"
#include "touchtimer.h"
#include "DIALOG.h"

/** framewin初始化
 *
 * pMsg:消息体 ,textid0:文本0 ,textid1:文本1, textid2：文本2，textid3:文本3
 * @param
 * @return
 *
 */
void FrameWin_Init(WM_MESSAGE *pMsg,uint16_t textid0,uint16_t textid1,uint16_t textid2,uint16_t textid3)
{
    FrameWin_Show(pMsg->hWin,GUI_TA_HCENTER | GUI_TA_VCENTER,40,&XBF24_Font,GUI_RED,"欢迎使用北京动力源交流充电桩");
    Text_Show(WM_GetDialogItem(pMsg->hWin, textid2),&XBF16_Font,GUI_RED,"信号：强");
    Text_Show(WM_GetDialogItem(pMsg->hWin, textid3),&XBF16_Font,GUI_BLACK,"感谢您为空气的清新奉献一份力量");
    Text_Show(WM_GetDialogItem(pMsg->hWin, textid0),&XBF16_Font,GUI_RED,"2017-02-28");
    Text_Show(WM_GetDialogItem(pMsg->hWin, textid1),&XBF16_Font,GUI_RED,"14:00:00");
    WM_SetFocus(pMsg->hWin);
}
/** window初始化
 *
 * pMsg:消息体 ,textid0:文本0 ,textid1:文本1, textid2：文本2，textid3:文本3
 * @param
 * @return
 *
 */
void Window_Init(WM_MESSAGE *pMsg,uint16_t textid0,uint16_t textid1,uint16_t textid2,uint16_t textid3)
{
    //FrameWin_Show(pMsg->hWin,GUI_TA_HCENTER | GUI_TA_VCENTER,40,&XBF24_Font,GUI_RED,"欢迎使用北京动力源交流充电桩");
    Text_Show(WM_GetDialogItem(pMsg->hWin, textid2),&XBF16_Font,GUI_RED,"信号：强");
    Text_Show(WM_GetDialogItem(pMsg->hWin, textid3),&XBF16_Font,GUI_BLACK,"感谢您为空气的清新奉献一份力量");
    Text_Show(WM_GetDialogItem(pMsg->hWin, textid0),&XBF16_Font,GUI_RED,"2017-02-28");
    Text_Show(WM_GetDialogItem(pMsg->hWin, textid1),&XBF16_Font,GUI_RED,"14:00:00");
    WM_SetFocus(pMsg->hWin);
}
/** framewin初始化
 *
 * pMsg:消息体 ,textid0:文本0 ,textid1:文本1, textid2：文本2，textid3:文本3
 * @param
 * @return
 *
 */
void Caculate_RTC_Show(WM_MESSAGE *pMsg,uint16_t textid0,uint16_t textid1)
{
    RTC_TimeTypeDef RTC_TimeStruct;
    RTC_DateTypeDef RTC_DateStruct;
    WM_HWIN hWin = pMsg->hWin;

    HAL_RTC_GetDate(&RTC_Handler, &RTC_DateStruct, RTC_FORMAT_BIN);
    xsprintf((char *)Date_buf, "20%02d-%02d-%02d", RTC_DateStruct.Year, RTC_DateStruct.Month, RTC_DateStruct.Date);
    TEXT_SetText(WM_GetDialogItem(hWin, textid0), Date_buf);

    HAL_RTC_GetTime(&RTC_Handler, &RTC_TimeStruct, RTC_FORMAT_BIN);
    xsprintf((char *)Time_buf, "%02d:%02d:%02d", RTC_TimeStruct.Hours, RTC_TimeStruct.Minutes, RTC_TimeStruct.Seconds);
    TEXT_SetText(WM_GetDialogItem(hWin, textid1), Time_buf);
}
/** framewin初始化
 *
 * WM_WIN:源窗口 ,align:对齐方式,heigh:窗口高度,font:字体类型,color:字体颜色,buf:显示的文字内容
 * @param
 * @return
 *
 */
void FrameWin_Show(WM_HWIN hItem,uint8_t aglin,uint8_t heigh,GUI_FONT *font,uint32_t color,uint8_t *buf)
{
    FRAMEWIN_SetTextAlign(hItem, aglin);
    FRAMEWIN_SetTitleHeight(hItem, heigh);
    FRAMEWIN_SetFont(hItem, font);
    FRAMEWIN_SetTextColor(hItem, color);
    FRAMEWIN_SetText(hItem, buf);
//    FRAMEWIN_AddMinButton(hItem,FRAMEWIN_BUTTON_RIGHT,0);
}
/** Text显示
 *
 * WM_WIN:源窗口,font:字体类型,color:字体颜色,buf:显示的文字内容
 * @param
 * @return
 *
 */
void Text_Show(WM_HWIN hItem,GUI_FONT *font,uint32_t color,uint8_t *buf)
{
    TEXT_SetFont(hItem, font);
    TEXT_SetTextColor(hItem, color);
    TEXT_SetText(hItem, buf);
}
/** Edit用作带外方框的显示
 *
 * WM_WIN:源窗口,font:字体类型,color:字体颜色,buf:显示的文字内容
 * @param
 * @return
 * note；此处并不做编辑处理,没有做颜色处理
 */
void Edit_Show(WM_HWIN hItem,GUI_FONT *font,uint8_t *buf)
{
    EDIT_SetFont(hItem, font);
    EDIT_SetText(hItem, buf);
}
/** Button显示
 *
 * WM_WIN:源窗口,aglin:文字对齐方式,font:字体类型,bkcolor:按钮背景颜色 ,color:字体颜色,buf:显示的文字内容
 * @param
 * @return
 *
 */
void Button_Show(WM_HWIN hItem,uint8_t aglin,GUI_FONT *font,uint8_t bk_style,uint32_t bkcolor,uint8_t text_style,uint32_t color,uint8_t *buf)
{
    BUTTON_SetFont(hItem, font);
    BUTTON_SetTextAlign(hItem,aglin);
    BUTTON_SetBkColor(hItem, bk_style, bkcolor);
    BUTTON_SetTextColor(hItem, text_style, color);
    BUTTON_SetText(hItem, buf);
}
/** 图片显示
 *
 * WM_WIN:源窗口,aglin:文字对齐方式,font:字体类型,bkcolor:按钮背景颜色 ,color:字体颜色,buf:显示的文字内容
 * @param
 * @return
 *Note:主要针对于二维码
 */
void Image_Show(WM_HWIN hItem,uint8_t imageid,U32 filesize)
{
    const void  *pData;

    //pData = _GetImageById(imageid, &filesize);
    IMAGE_SetBMP(hItem, pData, filesize);
}





