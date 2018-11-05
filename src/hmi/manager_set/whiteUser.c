#include "touchtimer.h"
#include "DIALOG.h"
#include "includes.h"
#include "order.h"
#include "interface.h"
#include "utils.h"
#include "cJSON.h"
#include "evse_config.h"
#include "cfg_parse.h"
#include "stringName.h"
#include "errorcode.h"
#include <string.h>

#define ID_WINDOW_0     (GUI_ID_USER + 0x00)
#define ID_LISTVIEW_0   (GUI_ID_USER + 0x01)

#define ID_BUTTON_0  (GUI_ID_USER + 0x02)//添加白名单
#define ID_BUTTON_1  (GUI_ID_USER + 0x03)//清空白名单
#define ID_BUTTON_2  (GUI_ID_USER + 0x04)//退出

WM_HWIN _hWinManagerWhiteUser;

static const GUI_WIDGET_CREATE_INFO _aDialogCreate[] =
{
	{ WINDOW_CreateIndirect, "WhiteUserSet", ID_WINDOW_0, 0, 0, 800, 480, 0, 0, 0 },
	{ LISTVIEW_CreateIndirect, "Listview", ID_LISTVIEW_0, 150, 170, 500, 280, 0, 0x0, 0 },
	{ BUTTON_CreateIndirect, "add", ID_BUTTON_0, 230, 60, 135, 70, 0, 0x0, 0 },
	{ BUTTON_CreateIndirect, "clearAll", ID_BUTTON_1, 435, 60, 135, 70, 0, 0x0, 0 },
	{ BUTTON_CreateIndirect, "X", ID_BUTTON_2, 715, 15, 70, 70, 0, 0x0, 0 },
};

static void _cbDialog(WM_MESSAGE * pMsg) {
	int NCode;
	int Id;
	WM_HWIN      hItem;
	SCROLLBAR_Handle wScroll;
	HEADER_Handle hHeader;
	char buf[10] = { 0 };
	int i;
	
	switch (pMsg->MsgId) {
	case WM_INIT_DIALOG:
		BUTTON_SetFont(WM_GetDialogItem(pMsg->hWin, ID_BUTTON_0), &fontwryhcg36e);
		BUTTON_SetFont(WM_GetDialogItem(pMsg->hWin, ID_BUTTON_1), &fontwryhcg36e);
		BUTTON_SetFont(WM_GetDialogItem(pMsg->hWin, ID_BUTTON_2), &fontwryhcg36e);
		hItem = WM_GetDialogItem(pMsg->hWin, ID_LISTVIEW_0);
		LISTVIEW_SetRowHeight(hItem, 30);
		
		/* 设置列表控件中header控件的所显示文本的字体 */
		hHeader = LISTVIEW_GetHeader(hItem);
		HEADER_SetFont(hHeader, &fontwryhcg36e);
		HEADER_SetHeight(hHeader, 40);

		wScroll = SCROLLBAR_CreateAttached(hItem, SCROLLBAR_CF_VERTICAL); //垂直滑轮
		SCROLLBAR_SetNumItems(wScroll, 30 * 15);
		SCROLLBAR_SetWidth(wScroll, 60);
		/*end*/

		//SCROLLBAR_SetDefaultWidth(15);
		/* 设置列表控件选项中所显示文本的字体 */
		LISTVIEW_SetFont(hItem, &SIF24_Font);
		/* 设置列表控件表格可见 */
		LISTVIEW_SetGridVis(hItem, 1);
		
		LISTVIEW_AddColumn(WM_GetDialogItem(pMsg->hWin, ID_LISTVIEW_0), 500, "WhiteUserList", GUI_TA_HCENTER | GUI_TA_VCENTER);
		
		break;
	case WM_NOTIFY_PARENT:
		Id    = WM_GetId(pMsg->hWinSrc);
		NCode = pMsg->Data.v;
		switch (Id) {
		case ID_BUTTON_0: //'gun1infobutton'
		    switch(NCode) {
			case WM_NOTIFICATION_CLICKED:
    		    Keypad_GetValue(WHITE_LIST, " ");
//			    hItem = WM_GetDialogItem(pMsg->hWin, ID_LISTVIEW_0);
//			    for (i = 0; i < 15; i++)
//			    {
//				    LISTVIEW_AddRow(hItem, NULL);
//
//				    sprintf((char *)buf, "%d", i + 1);
//				    LISTVIEW_SetItemText(hItem, 0, i, buf);
//			    }
				break;
			case WM_NOTIFICATION_RELEASED:

				break;
			case WM_NOTIFICATION_MOVED_OUT:

				break;
			}
			break;  
		case ID_BUTTON_1: //'help'
		    switch(NCode) {
			case WM_NOTIFICATION_CLICKED:
            
				break;
			case WM_NOTIFICATION_RELEASED:

				break;
			case WM_NOTIFICATION_MOVED_OUT:

				break;
			}
			break;
		}
		break;
	case WM_PAINT:
		break;
	case WM_TIMER:
		break;
	case MSG_JUMPHOME:
		break;
	default:
		WM_DefaultProc(pMsg);
		break;
	}
}

WM_HWIN CreateManagerWhiteUser()
//WM_HWIN CreateManagerWhiteUser(WM_HWIN srcHwin)
{
	_hWinManagerWhiteUser = GUI_CreateDialogBox(_aDialogCreate, GUI_COUNTOF(_aDialogCreate), _cbDialog, WM_HBKWIN, 0, 0);
	//_hWinManagerWhiteUser = GUI_CreateDialogBox(_aDialogCreate, GUI_COUNTOF(_aDialogCreate), _cbDialog, WM_GetClientWindow(srcHwin), 0, 0);
	return _hWinManagerWhiteUser;
}


