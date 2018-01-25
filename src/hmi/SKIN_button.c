#include "DIALOG.h"
#include "GUI.h"
#include "skin.h"
#include "HMI_Start.h"

extern int gunbuttonax;
extern int gunbuttonay;

extern int gunbuttonbx;
extern int gunbuttonby;

//Button控件的自定义绘制函数
int SKIN_buttontest(const WIDGET_ITEM_DRAW_INFO * pDrawItemInfo)
{
	WM_HWIN hWin;
    switch (pDrawItemInfo->Cmd)
    {
    case WIDGET_ITEM_CREATE:
    //case WIDGET_ITEM_DRAW_BACKGROUND:
    case WIDGET_ITEM_DRAW_BITMAP:
    //case WIDGET_ITEM_DRAW_TEXT:
        break;
    default: return BUTTON_DrawSkinFlex(pDrawItemInfo);//emWin默认控件绘制函数

    case WIDGET_ITEM_DRAW_BACKGROUND:

        if (pDrawItemInfo->ItemIndex == BUTTON_SKINFLEX_PI_ENABLED)
        {
        	
        }
        else if (pDrawItemInfo->ItemIndex == BUTTON_SKINFLEX_PI_PRESSED)
        {
        	
        }
        else if (pDrawItemInfo->ItemIndex == BUTTON_SKINFLEX_PI_FOCUSSED)
        {
        	
        }
        else if (pDrawItemInfo->ItemIndex == BUTTON_SKINFLEX_PI_DISABLED)
        {
            
        }
        else
        {
            BUTTON_DrawSkinFlex(pDrawItemInfo); 
        }
		break;
	case WIDGET_ITEM_DRAW_TEXT:
//		return BUTTON_DrawSkinFlex(pDrawItemInfo);
        return 0;
	}
	return 0;
}

int SKIN_buttongunAlookinfo(const WIDGET_ITEM_DRAW_INFO * pDrawItemInfo)
{
    WM_HWIN hWin;
    switch (pDrawItemInfo->Cmd)
    {
    case WIDGET_ITEM_CREATE:
    //case WIDGET_ITEM_DRAW_BACKGROUND:
    case WIDGET_ITEM_DRAW_BITMAP:
    //case WIDGET_ITEM_DRAW_TEXT:
        break;
    default: return BUTTON_DrawSkinFlex(pDrawItemInfo);//emWin默认控件绘制函数

    case WIDGET_ITEM_DRAW_BACKGROUND:

        if (pDrawItemInfo->ItemIndex == BUTTON_SKINFLEX_PI_ENABLED)
        {
            GUI_MEMDEV_WriteAt(Memdevhomegunlookinfo,gunbuttonax,gunbuttonay);
        }
        else if (pDrawItemInfo->ItemIndex == BUTTON_SKINFLEX_PI_PRESSED)
        {
            GUI_MEMDEV_WriteAt(Memdevhomegunlookinfopress, gunbuttonax, gunbuttonay);
        }
        else if (pDrawItemInfo->ItemIndex == BUTTON_SKINFLEX_PI_FOCUSSED)
        {
            GUI_MEMDEV_WriteAt(Memdevhomegunlookinfo, gunbuttonax, gunbuttonay);
        }
        else if (pDrawItemInfo->ItemIndex == BUTTON_SKINFLEX_PI_DISABLED)
        {
            GUI_MEMDEV_WriteAt(Memdevhomegunlookinfo, gunbuttonax, gunbuttonay);
        }
        else
        {
            GUI_MEMDEV_WriteAt(Memdevhomegunlookinfo, gunbuttonax, gunbuttonay);
            //BUTTON_DrawSkinFlex(pDrawItemInfo); 
        }
        break;
    case WIDGET_ITEM_DRAW_TEXT:
    //		return BUTTON_DrawSkinFlex(pDrawItemInfo);
        return 0;
    }
    return 0;
}

int SKIN_buttongunAscancode(const WIDGET_ITEM_DRAW_INFO * pDrawItemInfo)
{
    WM_HWIN hWin;
    switch (pDrawItemInfo->Cmd)
    {
    case WIDGET_ITEM_CREATE:
    //case WIDGET_ITEM_DRAW_BACKGROUND:
    case WIDGET_ITEM_DRAW_BITMAP:
    //case WIDGET_ITEM_DRAW_TEXT:
        break;
    default: return BUTTON_DrawSkinFlex(pDrawItemInfo);//emWin默认控件绘制函数

    case WIDGET_ITEM_DRAW_BACKGROUND:

        if (pDrawItemInfo->ItemIndex == BUTTON_SKINFLEX_PI_ENABLED)
        {
            GUI_MEMDEV_WriteAt(Memdevhomegunscancode, gunbuttonax, gunbuttonay);
        }
        else if (pDrawItemInfo->ItemIndex == BUTTON_SKINFLEX_PI_PRESSED)
        {
            GUI_MEMDEV_WriteAt(Memdevhomegunscancode, gunbuttonax, gunbuttonay);
        }
        else if (pDrawItemInfo->ItemIndex == BUTTON_SKINFLEX_PI_FOCUSSED)
        {
            GUI_MEMDEV_WriteAt(Memdevhomegunscancode, gunbuttonax, gunbuttonay);
        }
        else if (pDrawItemInfo->ItemIndex == BUTTON_SKINFLEX_PI_DISABLED)
        {
            GUI_MEMDEV_WriteAt(Memdevhomegunscancode, gunbuttonax, gunbuttonay);
        }
        else
        {
            BUTTON_DrawSkinFlex(pDrawItemInfo); 
        }
        break;
    case WIDGET_ITEM_DRAW_TEXT:
    //		return BUTTON_DrawSkinFlex(pDrawItemInfo);
        return 0;
    }
    return 0;
}

int SKIN_buttongunAerror(const WIDGET_ITEM_DRAW_INFO * pDrawItemInfo)
{
    WM_HWIN hWin;
    switch (pDrawItemInfo->Cmd)
    {
    case WIDGET_ITEM_CREATE:
    //case WIDGET_ITEM_DRAW_BACKGROUND:
    case WIDGET_ITEM_DRAW_BITMAP:
    //case WIDGET_ITEM_DRAW_TEXT:
        break;
    default: return BUTTON_DrawSkinFlex(pDrawItemInfo);//emWin默认控件绘制函数

    case WIDGET_ITEM_DRAW_BACKGROUND:

        if (pDrawItemInfo->ItemIndex == BUTTON_SKINFLEX_PI_ENABLED)
        {
            GUI_MEMDEV_WriteAt(Memdevhomegunerror, gunbuttonax, gunbuttonay);
        }
        else if (pDrawItemInfo->ItemIndex == BUTTON_SKINFLEX_PI_PRESSED)
        {
            GUI_MEMDEV_WriteAt(Memdevhomegunerror, gunbuttonax, gunbuttonay);
        }
        else if (pDrawItemInfo->ItemIndex == BUTTON_SKINFLEX_PI_FOCUSSED)
        {
            GUI_MEMDEV_WriteAt(Memdevhomegunerror, gunbuttonax, gunbuttonay);
        }
        else if (pDrawItemInfo->ItemIndex == BUTTON_SKINFLEX_PI_DISABLED)
        {
            GUI_MEMDEV_WriteAt(Memdevhomegunerror, gunbuttonax, gunbuttonay);
        }
        else
        {
            BUTTON_DrawSkinFlex(pDrawItemInfo); 
        }
        break;
    case WIDGET_ITEM_DRAW_TEXT:
    //		return BUTTON_DrawSkinFlex(pDrawItemInfo);
        return 0;
    }
    return 0;
}

int SKIN_buttongunBlookinfo(const WIDGET_ITEM_DRAW_INFO * pDrawItemInfo)
{
    WM_HWIN hWin;
    switch (pDrawItemInfo->Cmd)
    {
    case WIDGET_ITEM_CREATE:
    //case WIDGET_ITEM_DRAW_BACKGROUND:
    case WIDGET_ITEM_DRAW_BITMAP:
    //case WIDGET_ITEM_DRAW_TEXT:
        break;
    default: return BUTTON_DrawSkinFlex(pDrawItemInfo);//emWin默认控件绘制函数

    case WIDGET_ITEM_DRAW_BACKGROUND:

        if (pDrawItemInfo->ItemIndex == BUTTON_SKINFLEX_PI_ENABLED)
        {
            GUI_MEMDEV_WriteAt(Memdevhomegunlookinfo, gunbuttonbx, gunbuttonby);
        }
        else if (pDrawItemInfo->ItemIndex == BUTTON_SKINFLEX_PI_PRESSED)
        {
            GUI_MEMDEV_WriteAt(Memdevhomegunlookinfopress, gunbuttonbx, gunbuttonby);
        }
        else if (pDrawItemInfo->ItemIndex == BUTTON_SKINFLEX_PI_FOCUSSED)
        {
            GUI_MEMDEV_WriteAt(Memdevhomegunlookinfo, gunbuttonbx, gunbuttonby);
        }
        else if (pDrawItemInfo->ItemIndex == BUTTON_SKINFLEX_PI_DISABLED)
        {
            GUI_MEMDEV_WriteAt(Memdevhomegunlookinfo, gunbuttonbx, gunbuttonby);
        }
        else
        {
            BUTTON_DrawSkinFlex(pDrawItemInfo); 
        }
        break;
    case WIDGET_ITEM_DRAW_TEXT:
    //		return BUTTON_DrawSkinFlex(pDrawItemInfo);
        return 0;
    }
    return 0;
}

int SKIN_buttongunBscancode(const WIDGET_ITEM_DRAW_INFO * pDrawItemInfo)
{
    WM_HWIN hWin;
    switch (pDrawItemInfo->Cmd)
    {
    case WIDGET_ITEM_CREATE:
    //case WIDGET_ITEM_DRAW_BACKGROUND:
    case WIDGET_ITEM_DRAW_BITMAP:
    //case WIDGET_ITEM_DRAW_TEXT:
        break;
    default: return BUTTON_DrawSkinFlex(pDrawItemInfo);//emWin默认控件绘制函数

    case WIDGET_ITEM_DRAW_BACKGROUND:

        if (pDrawItemInfo->ItemIndex == BUTTON_SKINFLEX_PI_ENABLED)
        {
            GUI_MEMDEV_WriteAt(Memdevhomegunscancode, gunbuttonbx, gunbuttonby);
        }
        else if (pDrawItemInfo->ItemIndex == BUTTON_SKINFLEX_PI_PRESSED)
        {
            GUI_MEMDEV_WriteAt(Memdevhomegunscancode, gunbuttonbx, gunbuttonby);
        }
        else if (pDrawItemInfo->ItemIndex == BUTTON_SKINFLEX_PI_FOCUSSED)
        {
            GUI_MEMDEV_WriteAt(Memdevhomegunscancode, gunbuttonbx, gunbuttonby);
        }
        else if (pDrawItemInfo->ItemIndex == BUTTON_SKINFLEX_PI_DISABLED)
        {
            GUI_MEMDEV_WriteAt(Memdevhomegunscancode, gunbuttonbx, gunbuttonby);
        }
        else
        {
            BUTTON_DrawSkinFlex(pDrawItemInfo); 
        }
        break;
    case WIDGET_ITEM_DRAW_TEXT:
    //		return BUTTON_DrawSkinFlex(pDrawItemInfo);
        return 0;
    }
    return 0;
}

int SKIN_buttongunBerror(const WIDGET_ITEM_DRAW_INFO * pDrawItemInfo)
{
    WM_HWIN hWin;
    switch (pDrawItemInfo->Cmd)
    {
    case WIDGET_ITEM_CREATE:
    //case WIDGET_ITEM_DRAW_BACKGROUND:
    case WIDGET_ITEM_DRAW_BITMAP:
    //case WIDGET_ITEM_DRAW_TEXT:
        break;
    default: return BUTTON_DrawSkinFlex(pDrawItemInfo);//emWin默认控件绘制函数

    case WIDGET_ITEM_DRAW_BACKGROUND:

        if (pDrawItemInfo->ItemIndex == BUTTON_SKINFLEX_PI_ENABLED)
        {
            GUI_MEMDEV_WriteAt(Memdevhomegunerror, gunbuttonbx, gunbuttonby);
        }
        else if (pDrawItemInfo->ItemIndex == BUTTON_SKINFLEX_PI_PRESSED)
        {
            GUI_MEMDEV_WriteAt(Memdevhomegunerror, gunbuttonbx, gunbuttonby);
        }
        else if (pDrawItemInfo->ItemIndex == BUTTON_SKINFLEX_PI_FOCUSSED)
        {
            GUI_MEMDEV_WriteAt(Memdevhomegunerror, gunbuttonbx, gunbuttonby);
        }
        else if (pDrawItemInfo->ItemIndex == BUTTON_SKINFLEX_PI_DISABLED)
        {
            GUI_MEMDEV_WriteAt(Memdevhomegunerror, gunbuttonbx, gunbuttonby);
        }
        else
        {
            BUTTON_DrawSkinFlex(pDrawItemInfo); 
        }
        break;
    case WIDGET_ITEM_DRAW_TEXT:
    //		return BUTTON_DrawSkinFlex(pDrawItemInfo);
        return 0;
    }
    return 0;
}

