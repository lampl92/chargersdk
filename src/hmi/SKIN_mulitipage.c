#include "DIALOG.h"
#include "GUI.h"
#include "skin.h"
#include <stdlib.h>
#include "HMI_Start.h"
#include "touchtimer.h"
#include "GUI_backstage.h"

int SKIN_mulitipage(const WIDGET_ITEM_DRAW_INFO * pDrawItemInfo)
{
    WM_HWIN hWin;
    //int x0, y0;
    int x, y;
    GUI_RECT h;
    MULTIPAGE_SKIN_INFO *info;
    switch (pDrawItemInfo->Cmd)
    {
    case WIDGET_ITEM_CREATE:
    case WIDGET_ITEM_DRAW_FRAME:
    case WIDGET_ITEM_DRAW_BACKGROUND:
        info = pDrawItemInfo->p;
        if (info->Sel == pDrawItemInfo->ItemIndex)
        {
            GUI_SetColor(color_manager_in);
            if (pDrawItemInfo->ItemIndex == 0)
            {
                GUI_FillRect(pDrawItemInfo->x0 + 1, pDrawItemInfo->y0, pDrawItemInfo->x1, pDrawItemInfo->y1);
            }
            else
            {
                GUI_FillRect(pDrawItemInfo->x0, pDrawItemInfo->y0, pDrawItemInfo->x1, pDrawItemInfo->y1);                
            }
        }
        else
        {
            GUI_SetColor(color_manager_out);
            GUI_FillRect(pDrawItemInfo->x0, pDrawItemInfo->y0, pDrawItemInfo->x1, pDrawItemInfo->y1);
            GUI_SetColor(color_manager_in);
            GUI_SetPenSize(3);
            GUI_DrawRect(pDrawItemInfo->x0 - 3, pDrawItemInfo->y0, pDrawItemInfo->x1 + 3, pDrawItemInfo->y1);    
          
        }
        break;
    case WIDGET_ITEM_DRAW_TEXT:        
        if (pDrawItemInfo->ItemIndex == 0)
        {
            h.x0 = pDrawItemInfo->x0;
            h.x1 = pDrawItemInfo->x1;
            h.y0 = pDrawItemInfo->y0;
            h.y1 = pDrawItemInfo->y1;
            GUI_SetFont(&fontwryhcg30e);
            GUI_SetColor(GUI_WHITE);
            info = pDrawItemInfo->p;
            if (info->Sel == pDrawItemInfo->ItemIndex)
            {
                GUI_SetBkColor(color_manager_in);
            }
            else
            {
                GUI_SetBkColor(color_manager_out);
            }
            GUI_DispStringInRect("模拟量", &h, GUI_TA_VCENTER | GUI_TA_HCENTER);
        }
        break;
    default:
        break;
        //return MULTIPAGE_DrawSkinFlex(pDrawItemInfo);
    }
    return 0;
}


