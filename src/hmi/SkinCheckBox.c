#include "DIALOG.h"
#include "skin.h"
#include "HMI_Start.h"

int SKIN_checkboxfull(const WIDGET_ITEM_DRAW_INFO * pDrawItemInfo)
{
//    PageChosenData_struct *Pic;
    WM_HWIN hWin;
    int x0, y0;  
    switch (pDrawItemInfo->Cmd)
    {
    case WIDGET_ITEM_CREATE:
        break; 
    case WIDGET_ITEM_DRAW_BUTTON:
        x0 = WM_GetWindowOrgX(pDrawItemInfo->hWin);
        y0 = WM_GetWindowOrgY(pDrawItemInfo->hWin);
        GUI_DrawBitmap(&Bitmapselectpatternfull, 0, 0);
        break;
    case WIDGET_ITEM_DRAW_BITMAP://选择打勾
        x0 = WM_GetWindowOrgX(pDrawItemInfo->hWin);
        y0 = WM_GetWindowOrgY(pDrawItemInfo->hWin);
        GUI_DrawBitmap(&Bitmapselectpatternfullpress, 0, 0);
        break;
    case WIDGET_ITEM_DRAW_FOCUS:
        CHECKBOX_DrawSkinFlex(pDrawItemInfo);
        break;
    default: return CHECKBOX_DrawSkinFlex(pDrawItemInfo);//emWin默认控件绘制函数

    }
    return 0;
}

int SKIN_checkboxtime(const WIDGET_ITEM_DRAW_INFO * pDrawItemInfo)
{
    WM_HWIN hWin;
    int x0, y0;  
    switch (pDrawItemInfo->Cmd)
    {
    case WIDGET_ITEM_CREATE:
        break; 
    case WIDGET_ITEM_DRAW_BUTTON:
        x0 = WM_GetWindowOrgX(pDrawItemInfo->hWin);
        y0 = WM_GetWindowOrgY(pDrawItemInfo->hWin);
        GUI_DrawBitmap(&Bitmapselectpatterntime, 0, 0);
        break;
    case WIDGET_ITEM_DRAW_BITMAP://选择打勾
        x0 = WM_GetWindowOrgX(pDrawItemInfo->hWin);
        y0 = WM_GetWindowOrgY(pDrawItemInfo->hWin);
        GUI_DrawBitmap(&Bitmapselectpatterntimepress, 0, 0);
        break;
    case WIDGET_ITEM_DRAW_FOCUS:
        CHECKBOX_DrawSkinFlex(pDrawItemInfo);
        break;
    default: return CHECKBOX_DrawSkinFlex(pDrawItemInfo);//emWin默认控件绘制函数

    }
    return 0;
}

int SKIN_checkboxmoney(const WIDGET_ITEM_DRAW_INFO * pDrawItemInfo)
{
    WM_HWIN hWin;
    int x0, y0;  
    switch (pDrawItemInfo->Cmd)
    {
    case WIDGET_ITEM_CREATE:
        break; 
    case WIDGET_ITEM_DRAW_BUTTON:
        x0 = WM_GetWindowOrgX(pDrawItemInfo->hWin);
        y0 = WM_GetWindowOrgY(pDrawItemInfo->hWin);
        GUI_DrawBitmap(&Bitmapselectpatternmoneynumber, 0, 0);
        break;
    case WIDGET_ITEM_DRAW_BITMAP://选择打勾
        x0 = WM_GetWindowOrgX(pDrawItemInfo->hWin);
        y0 = WM_GetWindowOrgY(pDrawItemInfo->hWin);
        GUI_DrawBitmap(&Bitmapselectpatternmoneynumberpress, 0, 0);
        break;
    case WIDGET_ITEM_DRAW_FOCUS:
        CHECKBOX_DrawSkinFlex(pDrawItemInfo);
        break;
    default: return CHECKBOX_DrawSkinFlex(pDrawItemInfo);//emWin默认控件绘制函数

    }
    return 0;
}

int SKIN_checkboxelectric(const WIDGET_ITEM_DRAW_INFO * pDrawItemInfo)
{
    WM_HWIN hWin;
    int x0, y0;  
    switch (pDrawItemInfo->Cmd)
    {
    case WIDGET_ITEM_CREATE:
        break; 
    case WIDGET_ITEM_DRAW_BUTTON:
        x0 = WM_GetWindowOrgX(pDrawItemInfo->hWin);
        y0 = WM_GetWindowOrgY(pDrawItemInfo->hWin);
        GUI_DrawBitmap(&Bitmapselectpatternelectricnumber, 0, 0);
        break;
    case WIDGET_ITEM_DRAW_BITMAP://选择打勾
        x0 = WM_GetWindowOrgX(pDrawItemInfo->hWin);
        y0 = WM_GetWindowOrgY(pDrawItemInfo->hWin);
        GUI_DrawBitmap(&Bitmapselectpatternelectricnumberpress, 0, 0);
        break;
    case WIDGET_ITEM_DRAW_FOCUS:
        CHECKBOX_DrawSkinFlex(pDrawItemInfo);
        break;
    default: return CHECKBOX_DrawSkinFlex(pDrawItemInfo);//emWin默认控件绘制函数

    }
    return 0;
}
