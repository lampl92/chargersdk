#include "DIALOG.h"
#include "GUI.h"
#include "skin.h"
#include "HMI_Start.h"

extern int keyboardx;
extern int keyboardy;

#define ID_WINDOW_0  (GUI_ID_USER + 0x00)
#define ID_BUTTON_0  (GUI_ID_USER + 0x02)
#define ID_BUTTON_1  (GUI_ID_USER + 0x03)
#define ID_BUTTON_2  (GUI_ID_USER + 0x04)
#define ID_BUTTON_3  (GUI_ID_USER + 0x05)
#define ID_BUTTON_4  (GUI_ID_USER + 0x06)
#define ID_BUTTON_5  (GUI_ID_USER + 0x07)
#define ID_BUTTON_6  (GUI_ID_USER + 0x08)
#define ID_BUTTON_7  (GUI_ID_USER + 0x09)
#define ID_BUTTON_8  (GUI_ID_USER + 0x0A)
#define ID_BUTTON_9  (GUI_ID_USER + 0x0B)
#define ID_BUTTON_10  (GUI_ID_USER + 0x0C)
#define ID_BUTTON_11  (GUI_ID_USER + 0x0D)
#define ID_BUTTON_12  (GUI_ID_USER + 0x0E)
#define ID_BUTTON_13  (GUI_ID_USER + 0x0F)
#define ID_BUTTON_14  (GUI_ID_USER + 0x10)
#define ID_BUTTON_15  (GUI_ID_USER + 0x11)
#define ID_BUTTON_16  (GUI_ID_USER + 0x12)
#define ID_BUTTON_17  (GUI_ID_USER + 0x13)
#define ID_BUTTON_18  (GUI_ID_USER + 0x14)
#define ID_BUTTON_19  (GUI_ID_USER + 0x15)

static const GUI_WIDGET_CREATE_INFO _aDialogCreate[] = {
    { BUTTON_CreateIndirect, "Button1", ID_BUTTON_0, 15, 15, 50, 50, 0, 0x0, 0 },
    { BUTTON_CreateIndirect, "Button2", ID_BUTTON_1, 75, 15, 50, 50, 0, 0x0, 0 },
    { BUTTON_CreateIndirect, "Button3", ID_BUTTON_2, 135, 15, 50, 50, 0, 0x0, 0 },
    { BUTTON_CreateIndirect, "Button-b", ID_BUTTON_3, 195, 15, 50, 50, 0, 0x0, 0 },
    { BUTTON_CreateIndirect, "Button4", ID_BUTTON_4, 15, 75, 50, 50, 0, 0x0, 0 },
    { BUTTON_CreateIndirect, "Button5", ID_BUTTON_5, 75, 75, 50, 50, 0, 0x0, 0 },
    { BUTTON_CreateIndirect, "Button6", ID_BUTTON_6, 135, 75, 50, 50, 0, 0x0, 0 },
    { BUTTON_CreateIndirect, "Button-cancel", ID_BUTTON_7, 195, 75, 50, 50, 0, 0x0, 0 },
    { BUTTON_CreateIndirect, "Button7", ID_BUTTON_8, 15, 135, 50, 50, 0, 0x0, 0 },
    { BUTTON_CreateIndirect, "Button8", ID_BUTTON_9, 75, 135, 50, 50, 0, 0x0, 0 },
    { BUTTON_CreateIndirect, "Button9", ID_BUTTON_10, 135, 135, 50, 50, 0, 0x0, 0 },
    { BUTTON_CreateIndirect, "Button-ok", ID_BUTTON_11, 195, 135, 50, 110, 0, 0x0, 0 },
    { BUTTON_CreateIndirect, "Button0", ID_BUTTON_12, 15, 195, 110, 50, 0, 0x0, 0 },
    { BUTTON_CreateIndirect, "Button-point", ID_BUTTON_13, 135, 195, 50, 50, 0, 0x0, 0 },
};

static int BUTTONSKB_GetPressKey(const WIDGET_ITEM_DRAW_INFO * pDrawItemInfo, I16 *pid, GUI_RECT *pr)
{
	I16 i;
	GUI_PID_STATE statePID;
	WIDGET_ITEM_DRAW_INFO DrawItemInfoL;
	GUI_PID_GetState(&statePID);
	statePID.x -= WM_GetWindowOrgX(pDrawItemInfo->hWin);
	statePID.y -= WM_GetWindowOrgY(pDrawItemInfo->hWin);
	*pid = 0;
	for(i=0; i<GUI_COUNTOF(_aDialogCreate); i++)
	{
		DrawItemInfoL.x0 = _aDialogCreate[i].x0;
		DrawItemInfoL.y0 = _aDialogCreate[i].y0;
		DrawItemInfoL.x1 = _aDialogCreate[i].x0 + _aDialogCreate[i].xSize;
		DrawItemInfoL.y1 = _aDialogCreate[i].y0 + _aDialogCreate[i].ySize;
		if((statePID.x >= DrawItemInfoL.x0) && (statePID.x <= DrawItemInfoL.x1)
		&& (statePID.y >= DrawItemInfoL.y0) && (statePID.y <= DrawItemInfoL.y1))
		{
			*pid = i;
			pr->x0 = DrawItemInfoL.x0; pr->y0 = DrawItemInfoL.y0;
			pr->x1 = DrawItemInfoL.x1; pr->y1 = DrawItemInfoL.y1;
			return *pid;
		}
	}
	return -1;
}

//Button控件的自定义绘制函数
int SKIN_buttonSKB(const WIDGET_ITEM_DRAW_INFO * pDrawItemInfo)
{
	WM_HWIN hWin;
	int x0, y0;
	int res; I16 Id; GUI_RECT r;
	switch (pDrawItemInfo->Cmd)
	{
	case WIDGET_ITEM_CREATE:
	case WIDGET_ITEM_DRAW_BITMAP:
		break;
	default: return BUTTON_DrawSkinFlex(pDrawItemInfo);//emWin默认控件绘制函数

	case WIDGET_ITEM_DRAW_BACKGROUND:
		if(pDrawItemInfo->ItemIndex == BUTTON_SKINFLEX_PI_ENABLED)
		{
    		GUI_MEMDEV_WriteAt(Memdevselectpatternkeyboard, keyboardx, keyboardy);
		}
		else if(pDrawItemInfo->ItemIndex == BUTTON_SKINFLEX_PI_PRESSED)
		{
    		GUI_MEMDEV_WriteAt(Memdevselectpatternkeyboard, keyboardx, keyboardy);
			BUTTONSKB_GetPressKey(pDrawItemInfo, &Id, &r);
			BUTTON_SetUserData(pDrawItemInfo->hWin, &Id, 2);
			if(Id == -1) break;
            WM_SetUserClipRect(&r);
    		GUI_MEMDEV_WriteAt(Memdevselectpatternkeyboardpress, keyboardx, keyboardy);
			WM_SetUserClipRect(0);
		}
		else if(pDrawItemInfo->ItemIndex == BUTTON_SKINFLEX_PI_FOCUSSED)
		{
    		GUI_MEMDEV_WriteAt(Memdevselectpatternkeyboard, keyboardx, keyboardy);
		}
		else if(pDrawItemInfo->ItemIndex == BUTTON_SKINFLEX_PI_DISABLED)
		{
    		GUI_MEMDEV_WriteAt(Memdevselectpatternkeyboard, keyboardx, keyboardy);
		}
		break;
	case WIDGET_ITEM_DRAW_TEXT:
			//if(!BUTTONSKB_UBMP) BUTTONSKB_DrawAllText();
			break;
	}
	return 0;
}


