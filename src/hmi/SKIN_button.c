#include "DIALOG.h"
#include "GUI.h"
#include "skin.h"
#include "HMI_Start.h"

//测试按键
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

//主页上的两个按键
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

int SKIN_buttonhelp(const WIDGET_ITEM_DRAW_INFO * pDrawItemInfo)
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
            GUI_MEMDEV_WriteAt(Memdevhomechargehelp, helpbuttonx, helpbuttony);
        }
        else if (pDrawItemInfo->ItemIndex == BUTTON_SKINFLEX_PI_PRESSED)
        {
            GUI_MEMDEV_WriteAt(Memdevhomechargehelppress, helpbuttonx, helpbuttony);
        }
        else if (pDrawItemInfo->ItemIndex == BUTTON_SKINFLEX_PI_FOCUSSED)
        {
            GUI_MEMDEV_WriteAt(Memdevhomechargehelp, helpbuttonx, helpbuttony);
        }
        else if (pDrawItemInfo->ItemIndex == BUTTON_SKINFLEX_PI_DISABLED)
        {
            GUI_MEMDEV_WriteAt(Memdevhomechargehelp, helpbuttonx, helpbuttony);
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

//选枪页上的枪A枪B按钮
int SKIN_selectbutton(const WIDGET_ITEM_DRAW_INFO * pDrawItemInfo)
{
    WM_HWIN hWin;
    char s[10];
    switch (pDrawItemInfo->Cmd)
    {
    case WIDGET_ITEM_CREATE:
    //case WIDGET_ITEM_DRAW_BACKGROUND:
    case WIDGET_ITEM_DRAW_BITMAP:
    //case WIDGET_ITEM_DRAW_TEXT:
        break;
    default: return BUTTON_DrawSkinFlex(pDrawItemInfo);//emWin默认控件绘制函数

    case WIDGET_ITEM_DRAW_BACKGROUND:
        if (pDrawItemInfo->hWin == WM_GetDialogItem(WM_GetParent(pDrawItemInfo->hWin), GUI_ID_USER + 0x01))
        {
            BUTTON_GetUserData(pDrawItemInfo->hWin, s, 10);
            if (strcmp("disable",s) == 0)
            {
                GUI_MEMDEV_WriteAt(MemdevSelectGunAbottonDisable, selectgunax, selectgunay);
            }
            else
            {
                if (pDrawItemInfo->ItemIndex == BUTTON_SKINFLEX_PI_ENABLED)
                {
                    GUI_MEMDEV_WriteAt(MemdevSelectGunAbottonNotpress, selectgunax, selectgunay);
                }
                else if (pDrawItemInfo->ItemIndex == BUTTON_SKINFLEX_PI_PRESSED)
                {
                    GUI_MEMDEV_WriteAt(MemdevSelectGunAbottonPress, selectgunax, selectgunay);
                }
                else if (pDrawItemInfo->ItemIndex == BUTTON_SKINFLEX_PI_FOCUSSED)
                {
                    GUI_MEMDEV_WriteAt(MemdevSelectGunAbottonNotpress, selectgunax, selectgunay);
                }
                else if (pDrawItemInfo->ItemIndex == BUTTON_SKINFLEX_PI_DISABLED)
                {
                    GUI_MEMDEV_WriteAt(MemdevSelectGunAbottonNotpress, selectgunax, selectgunay);
                }
                else
                {
                    BUTTON_DrawSkinFlex(pDrawItemInfo); 
                }
            }
           
        }
        else if (pDrawItemInfo->hWin == WM_GetDialogItem(WM_GetParent(pDrawItemInfo->hWin), GUI_ID_USER + 0x03))
        {
            BUTTON_GetUserData(pDrawItemInfo->hWin, s, 10);
            if (strcmp("disable",s) == 0)
            {
                GUI_MEMDEV_WriteAt(MemdevSelectGunBbottonDisable, selectgunbx, selectgunby);
            }
            else
            {
                if (pDrawItemInfo->ItemIndex == BUTTON_SKINFLEX_PI_ENABLED)
                {
                    GUI_MEMDEV_WriteAt(MemdevSelectGunBbottonNotpress, selectgunbx, selectgunby);
                }
                else if (pDrawItemInfo->ItemIndex == BUTTON_SKINFLEX_PI_PRESSED)
                {
                    GUI_MEMDEV_WriteAt(MemdevSelectGunBbottonPress, selectgunbx, selectgunby);
                }
                else if (pDrawItemInfo->ItemIndex == BUTTON_SKINFLEX_PI_FOCUSSED)
                {
                    GUI_MEMDEV_WriteAt(MemdevSelectGunBbottonNotpress, selectgunbx, selectgunby);
                }
                else if (pDrawItemInfo->ItemIndex == BUTTON_SKINFLEX_PI_DISABLED)
                {
                    GUI_MEMDEV_WriteAt(MemdevSelectGunBbottonNotpress, selectgunbx, selectgunby);
                }
                else
                {
                    BUTTON_DrawSkinFlex(pDrawItemInfo); 
                }
            }
        }
        break;
    case WIDGET_ITEM_DRAW_TEXT:
    //		return BUTTON_DrawSkinFlex(pDrawItemInfo);
        return 0;
    }
    return 0;
}

//共有按钮退出和后退
int SKIN_buttonquit(const WIDGET_ITEM_DRAW_INFO * pDrawItemInfo)
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
            GUI_MEMDEV_WriteAt(MemdevQuit, quitx, quity);
        }
        else if (pDrawItemInfo->ItemIndex == BUTTON_SKINFLEX_PI_PRESSED)
        {
            GUI_MEMDEV_WriteAt(MemdevQuitPress, quitx, quity);
        }
        else if (pDrawItemInfo->ItemIndex == BUTTON_SKINFLEX_PI_FOCUSSED)
        {
            GUI_MEMDEV_WriteAt(MemdevQuit, quitx, quity);
        }
        else if (pDrawItemInfo->ItemIndex == BUTTON_SKINFLEX_PI_DISABLED)
        {
            GUI_MEMDEV_WriteAt(MemdevQuit, quitx, quity);
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

int SKIN_buttonbackquit(const WIDGET_ITEM_DRAW_INFO * pDrawItemInfo)
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
            GUI_MEMDEV_WriteAt(MemdevbackQuit, backquitx, backquity);
        }
        else if (pDrawItemInfo->ItemIndex == BUTTON_SKINFLEX_PI_PRESSED)
        {
            GUI_MEMDEV_WriteAt(MemdevbackQuitPress, backquitx, backquity);
        }
        else if (pDrawItemInfo->ItemIndex == BUTTON_SKINFLEX_PI_FOCUSSED)
        {
            GUI_MEMDEV_WriteAt(MemdevbackQuit, backquitx, backquity);
        }
        else if (pDrawItemInfo->ItemIndex == BUTTON_SKINFLEX_PI_DISABLED)
        {
            GUI_MEMDEV_WriteAt(MemdevbackQuit, backquitx, backquity);
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

int SKIN_buttonPleasePlugQuit(const WIDGET_ITEM_DRAW_INFO * pDrawItemInfo)
{
    WM_HWIN hWin;
    char s[10];
    switch (pDrawItemInfo->Cmd)
    {
    case WIDGET_ITEM_CREATE:
    //case WIDGET_ITEM_DRAW_BACKGROUND:
    case WIDGET_ITEM_DRAW_BITMAP:
    //case WIDGET_ITEM_DRAW_TEXT:
        break;
    default: return BUTTON_DrawSkinFlex(pDrawItemInfo);//emWin默认控件绘制函数

    case WIDGET_ITEM_DRAW_BACKGROUND:
        BUTTON_GetUserData(pDrawItemInfo->hWin, s, 10);
        if (strcmp("plug", s) == 0)
        {
            ;
        }
        else
        {
            if (pDrawItemInfo->ItemIndex == BUTTON_SKINFLEX_PI_ENABLED)
            {
                GUI_MEMDEV_WriteAt(MemdevcardinfoQuit, quitPleasePlugx, quitPleasePlugy);
            }
            else if (pDrawItemInfo->ItemIndex == BUTTON_SKINFLEX_PI_PRESSED)
            {
                GUI_MEMDEV_WriteAt(MemdevcardinfoQuitPress, quitPleasePlugx, quitPleasePlugy);
            }
            else if (pDrawItemInfo->ItemIndex == BUTTON_SKINFLEX_PI_FOCUSSED)
            {
                GUI_MEMDEV_WriteAt(MemdevcardinfoQuit, quitPleasePlugx, quitPleasePlugy);
            }
            else if (pDrawItemInfo->ItemIndex == BUTTON_SKINFLEX_PI_DISABLED)
            {
                GUI_MEMDEV_WriteAt(MemdevcardinfoQuit, quitPleasePlugx, quitPleasePlugy);
            }
            else
            {
                BUTTON_DrawSkinFlex(pDrawItemInfo); 
            }
        }
        break;
    case WIDGET_ITEM_DRAW_TEXT:
    //		return BUTTON_DrawSkinFlex(pDrawItemInfo);
        return 0;
    }
    return 0;
}

