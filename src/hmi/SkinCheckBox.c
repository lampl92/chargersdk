#include "DIALOG.h"
#include "skin.h"
#include "HMI_Start.h"

int SKIN_checkboxfull(const WIDGET_ITEM_DRAW_INFO * pDrawItemInfo)
{
//    PageChosenData_struct *Pic;
//    WM_HWIN hWin;
//    int x0, y0;  
    switch (pDrawItemInfo->Cmd)
    {
    case WIDGET_ITEM_CREATE:
        break; 
    case WIDGET_ITEM_DRAW_BUTTON:
//        x0 = WM_GetWindowOrgX(pDrawItemInfo->hWin);
//        y0 = WM_GetWindowOrgY(pDrawItemInfo->hWin);
//        GUI_DrawBitmap(&Bitmapselectpatternfull, 0, 0);
        GUI_MEMDEV_WriteAt(Memdevselectpatternfull, Checkboxfullx, Checkboxfully);
        break;
    case WIDGET_ITEM_DRAW_BITMAP://选择打勾
//        x0 = WM_GetWindowOrgX(pDrawItemInfo->hWin);
//        y0 = WM_GetWindowOrgY(pDrawItemInfo->hWin);
//        GUI_DrawBitmap(&Bitmapselectpatternfullpress, 0, 0);
        GUI_MEMDEV_WriteAt(Memdevselectpatternfullpress, Checkboxfullx, Checkboxfully);
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
//    WM_HWIN hWin;
//    int x0, y0;  
    switch (pDrawItemInfo->Cmd)
    {
    case WIDGET_ITEM_CREATE:
        break; 
    case WIDGET_ITEM_DRAW_BUTTON:
//        x0 = WM_GetWindowOrgX(pDrawItemInfo->hWin);
//        y0 = WM_GetWindowOrgY(pDrawItemInfo->hWin);
//        GUI_DrawBitmap(&Bitmapselectpatterntime, 0, 0);
        GUI_MEMDEV_WriteAt(Memdevselectpatterntime, Checkboxtimex, Checkboxtimey);
        break;
    case WIDGET_ITEM_DRAW_BITMAP://选择打勾
//        x0 = WM_GetWindowOrgX(pDrawItemInfo->hWin);
//        y0 = WM_GetWindowOrgY(pDrawItemInfo->hWin);
//        GUI_DrawBitmap(&Bitmapselectpatterntimepress, 0, 0);
        GUI_MEMDEV_WriteAt(Memdevselectpatterntimepress, Checkboxtimex, Checkboxtimey);
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
//        x0 = WM_GetWindowOrgX(pDrawItemInfo->hWin);
//        y0 = WM_GetWindowOrgY(pDrawItemInfo->hWin);
//        GUI_DrawBitmap(&Bitmapselectpatternmoneynumber, 0, 0);
        GUI_MEMDEV_WriteAt(Memdevselectpatternmoneynumber, Checkboxmonex, Checkboxmoney);
        break;
    case WIDGET_ITEM_DRAW_BITMAP://选择打勾
//        x0 = WM_GetWindowOrgX(pDrawItemInfo->hWin);
//        y0 = WM_GetWindowOrgY(pDrawItemInfo->hWin);
//        GUI_DrawBitmap(&Bitmapselectpatternmoneynumberpress, 0, 0);
        GUI_MEMDEV_WriteAt(Memdevselectpatternmoneynumberpress, Checkboxmonex, Checkboxmoney);
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
//        x0 = WM_GetWindowOrgX(pDrawItemInfo->hWin);
//        y0 = WM_GetWindowOrgY(pDrawItemInfo->hWin);
//        GUI_DrawBitmap(&Bitmapselectpatternelectricnumber, 0, 0);
        GUI_MEMDEV_WriteAt(Memdevselectpatternelectricnumber, Checkboxelectricx, Checkboxelectricy);
        break;
    case WIDGET_ITEM_DRAW_BITMAP://选择打勾
//        x0 = WM_GetWindowOrgX(pDrawItemInfo->hWin);
//        y0 = WM_GetWindowOrgY(pDrawItemInfo->hWin);
//        GUI_DrawBitmap(&Bitmapselectpatternelectricnumberpress, 0, 0);
        GUI_MEMDEV_WriteAt(Memdevselectpatternelectricnumberpress, Checkboxelectricx, Checkboxelectricy);
        break;
    case WIDGET_ITEM_DRAW_FOCUS:
        CHECKBOX_DrawSkinFlex(pDrawItemInfo);
        break;
    default: return CHECKBOX_DrawSkinFlex(pDrawItemInfo);//emWin默认控件绘制函数

    }
    return 0;
}

int SKIN_checkboxStateInfo(const WIDGET_ITEM_DRAW_INFO * pDrawItemInfo)
{
    WM_HWIN hWin;
    GUI_RECT h;
    int x0, y0;  
    char c[10];
    switch (pDrawItemInfo->Cmd)
    {
    case WIDGET_ITEM_CREATE:
        break; 
    case WIDGET_ITEM_DRAW_BUTTON:
        GUI_GetClientRect(&h);
        GUI_SetColor(0xAAAAAA);
        GUI_FillRoundedRect(pDrawItemInfo->x0, pDrawItemInfo->y0, pDrawItemInfo->x1, pDrawItemInfo->y1, 3);        
        GUI_SetBkColor(0xAAAAAA);
        GUI_SetFont(&fontwryhcg30e);
        GUI_SetColor(GUI_BLACK);
        CHECKBOX_GetUserData(pDrawItemInfo->hWin, c, 10);
        if (strcmp("pile", c) == 0)
        {
            GUI_DispStringInRect("充电桩", &h, GUI_TA_VCENTER | GUI_TA_HCENTER); 
        }
        if (strcmp("gunA", c) == 0)
        {
            GUI_DispStringInRect("充电枪A", &h, GUI_TA_VCENTER | GUI_TA_HCENTER); 
        }
        if (strcmp("gunB", c) == 0)
        {
            GUI_DispStringInRect("充电枪B", &h, GUI_TA_VCENTER | GUI_TA_HCENTER); 
        }
        if (strcmp("gun", c) == 0)
        {
            GUI_DispStringInRect("充电枪", &h, GUI_TA_VCENTER | GUI_TA_HCENTER); 
        }
        break;
    case WIDGET_ITEM_DRAW_BITMAP://选择打勾
        GUI_GetClientRect(&h);
        GUI_SetColor(GUI_RED);
        GUI_FillRoundedRect(pDrawItemInfo->x0, pDrawItemInfo->y0, pDrawItemInfo->x1, pDrawItemInfo->y1, 3);        
        GUI_SetBkColor(GUI_RED);
        GUI_SetFont(&fontwryhcg30e);
        GUI_SetColor(GUI_BLACK);
        CHECKBOX_GetUserData(pDrawItemInfo->hWin, c, 10);
        if (strcmp("pile", c) == 0)
        {
            GUI_DispStringInRect("充电桩", &h, GUI_TA_VCENTER | GUI_TA_HCENTER); 
        }
        if (strcmp("gunA", c) == 0)
        {
            GUI_DispStringInRect("充电枪A", &h, GUI_TA_VCENTER | GUI_TA_HCENTER); 
        }
        if (strcmp("gunB", c) == 0)
        {
            GUI_DispStringInRect("充电枪B", &h, GUI_TA_VCENTER | GUI_TA_HCENTER); 
        }
        if (strcmp("gun", c) == 0)
        {
            GUI_DispStringInRect("充电枪", &h, GUI_TA_VCENTER | GUI_TA_HCENTER); 
        }
        break;
    case WIDGET_ITEM_DRAW_FOCUS:
        CHECKBOX_DrawSkinFlex(pDrawItemInfo);
        break;
    default: return CHECKBOX_DrawSkinFlex(pDrawItemInfo);//emWin默认控件绘制函数

    }
    return 0;
}
