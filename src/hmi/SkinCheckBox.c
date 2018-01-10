#include "DIALOG.h"
#include "skin.h"
#include "HMI_Start.h"

int SKIN_checkbox(const WIDGET_ITEM_DRAW_INFO * pDrawItemInfo)
{
//    PageChosenData_struct *Pic;
    WM_HWIN hWin;
    int x0, y0;  
    switch (pDrawItemInfo->Cmd)
    {
    case WIDGET_ITEM_CREATE:
        break; 
    case WIDGET_ITEM_DRAW_BUTTON:
        x0 =WM_GetWindowOrgX(pDrawItemInfo->hWin);
        y0 =WM_GetWindowOrgY(pDrawItemInfo->hWin);
//        hWin = WM_GetParent(pDrawItemInfo->hWin);
//        WM_GetUserData(hWin, &Pic, sizeof(Pic));
        GUI_DrawBitmap(&BitmapCheckboxChosen, x0, y0);//��ҳ��������ʾͼƬ,emWin���Լ����г�����ؼ���Χ��ͼƬ
//        WM_Paint(hWin);
        break;
    case WIDGET_ITEM_DRAW_BITMAP://ѡ���
        x0 =WM_GetWindowOrgX(pDrawItemInfo->hWin);
        y0 =WM_GetWindowOrgY(pDrawItemInfo->hWin);
//        hWin = WM_GetParent(pDrawItemInfo->hWin);
//        WM_GetUserData(hWin, &Pic, sizeof(Pic));
        GUI_DrawBitmap(&BitmapCheckboxNotChosen, x0, y0);
//        WM_Paint(hWin);
        break;
    case WIDGET_ITEM_DRAW_FOCUS:
        CHECKBOX_DrawSkinFlex(pDrawItemInfo);
        break;
    default: return CHECKBOX_DrawSkinFlex(pDrawItemInfo);//emWinĬ�Ͽؼ����ƺ���

    }
    return 0;
}