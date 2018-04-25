#include "DIALOG.h"
#include "GUI.h"
#include "skin.h"
#include <stdlib.h>
#include "HMI_Start.h"
#include "touchtimer.h"
#include "GUI_backstage.h"

#define testfactor 6
//static const GUI_POINT _aNeedle[] = { { -3*testfactor, 0 }, { -3*testfactor , -70*testfactor }, { 0, -80*testfactor  }, { 3*testfactor , -70*testfactor  }, { 3*testfactor , 0 } };
static const GUI_POINT _aNeedle[] = { { -5*testfactor, 0 }, { -3*testfactor, -40*testfactor }, { 0, -70*testfactor }, { 3*testfactor, -40*testfactor }, { 5*testfactor, 0 } };

static const GUI_POINT _aNeedle2[] = { { -5*testfactor, 0 }, { 0*testfactor, -65*testfactor }, { 5*testfactor, 0 } };

static const GUI_POINT _aNeedle1[] = { { 0, 0 }, { 0, 219  }, { 219, 219  }, { 219 , 0  } };

//ProgbarMeter�ؼ�ָ����ƺ���
static void PROGBARMETER_DispNeedle(const WIDGET_ITEM_DRAW_INFO * pDrawItemInfo, short angleBEG, short angleEND, short valMIN, short valMAX)
{
	GUI_POINT aPoints[GUI_COUNTOF(_aNeedle)];
	GUI_RECT r;
	float Angle;
    char  OldPenShape;
    int x;
    int y;
    int xPrev;
    int yPrev;
    GUI_AA_EnableHiRes();
    GUI_AA_SetFactor(testfactor);
	short Value; char i, Text[8], *p = (char *)((PROGBAR_SKINFLEX_INFO *)pDrawItemInfo->p)->pText;
	for(i=0; i<7; i++){Text[i] = *p++; Text[i+1] = 0; if(*p == '%') break;}
	Value = atoi(Text); if(Value<valMIN) Value = valMIN; if(Value>valMAX) Value = valMAX;
	Angle = angleBEG - (abs(angleEND-angleBEG)/(valMAX-valMIN))*Value;
	//PROGBAR_GetMinMax(pDrawItemInfo->hWin, &valMIN, &valMAX);//emwin v5.30
	//Angle = angleBEG - (abs(angleEND-angleBEG)/(valMAX-valMIN))*PROGBAR_GetValue(pDrawItemInfo->hWin);//emwin v5.30

	Angle *= 3.1415926f / 180;
	GUI_SetColor(0x35bf5f);
	GUI_RotatePolygon(aPoints, _aNeedle, GUI_COUNTOF(_aNeedle), Angle);
	WM_GetWindowRectEx(pDrawItemInfo->hWin, &r);
//    OldPenShape = GUI_SetPenShape(15);
//    GUI_SetColor(GUI_BLUE);
//    xPrev = testfactor*(r.x1 - r.x0) / 2 + (*(aPoints + GUI_COUNTOF(aPoints) - 1)).x;
//    yPrev = testfactor*(r.y1 - r.y0) / 2 + (*(aPoints + GUI_COUNTOF(aPoints) - 1)).y;
//    for (i = 0; i < GUI_COUNTOF(aPoints); i++)
//    {
//        x = testfactor*(r.x1 - r.x0) / 2 + (*(aPoints + i)).x;
//        y = testfactor*(r.y1 - r.y0) / 2 + (*(aPoints + i)).y;
//        GUI_AA_DrawLine(xPrev, yPrev, x, y);
//        xPrev = x;
//        yPrev = y;
//    }
 //   GUI_SetColor(0x006666);//��ɫ������
    //GUI_SetColor(0xF9F900);//��ɫ����
    //006600��ɫ����
    GUI_SetColor(0x3366CC);
   // GUI_AA_FillPolygon(aPoints, GUI_COUNTOF(aPoints), (r.x1 - r.x0) / 2*testfactor, (r.y1 - r.y0) / 2*testfactor);
    //227-112 = 115,228-79=149,572-456=116
    GUI_AA_FillPolygon(aPoints, GUI_COUNTOF(aPoints), 115*testfactor, 149*testfactor);
    //GUI_SetColor(0xF9F900);
    GUI_AA_FillCircle(115*testfactor, 149*testfactor, 11*testfactor);
    GUI_SetColor(0xF9F900);//��ɫ����
    GUI_AA_FillCircle(115*testfactor, 149*testfactor, 8*testfactor);
    GUI_SetColor(0x006666);//��ɫ������
    GUI_AA_FillCircle(115*testfactor, 149*testfactor, 4*testfactor);
    GUI_AA_DisableHiRes();
}

//ProgbarMeter�ؼ����Զ�����ƺ���
int SKIN_progbarmeter(const WIDGET_ITEM_DRAW_INFO * pDrawItemInfo)
{
	WM_HWIN hWin;
	//int x0, y0;
    int x, y;
	switch (pDrawItemInfo->Cmd)
	{
	case WIDGET_ITEM_CREATE:
	case WIDGET_ITEM_DRAW_FRAME:
	case WIDGET_ITEM_DRAW_BACKGROUND:
//    	GUI_SetPenShape(3);
    	x = WM_GetWindowOrgX(WM_GetParent(pDrawItemInfo->hWin));
    	y = WM_GetWindowOrgY(WM_GetParent(pDrawItemInfo->hWin));
    	if (x == 112)
    	{
        	GUI_MEMDEV_WriteAt(MemdevhomegunAcharging, x, y);
    	}
    	else if (x == 456)
    	{
        	GUI_MEMDEV_WriteAt(MemdevhomegunBcharging, x, y);
    	}
//    	GUI_SetColor(GUI_BLACK);  	
//    	GUI_FillPolygon(_aNeedle1, GUI_COUNTOF(_aNeedle1), 0, 0);
//    	GUI_SetColor(GUI_GREEN);
//    	GUI_DrawLine(0,0,219,20);
//    	GUI_AA_EnableHiRes();
//    	GUI_AA_SetFactor(testfactor);
//    	GUI_AA_DrawLine(0,20*testfactor,219/2,40/2);
//    	GUI_AA_DisableHiRes();
//		hWin = (WM_GET_PARENT[C-1])(pDrawItemInfo->hWin);//λ�ڵڼ����޸�����(WM_GetParent��=����-1)
//		WM_GetUserData(hWin, &winBMP, sizeof(winBMP));//���û�����������"ҳ��ͼƬ�ṹ��"ָ��
//		//��ȡ�˿ؼ��������ҳ��(����:WINDOW)λ��ƫ������
//		x0 = WM_GetWindowOrgX(hWin) - WM_GetWindowOrgX(pDrawItemInfo->hWin);
//		y0 = WM_GetWindowOrgY(hWin) -WM_GetWindowOrgY(pDrawItemInfo->hWin);
//		PROGBARMETER_SetUserClip(pDrawItemInfo);//����1���û�������
//		if(((PROGBAR_SKINFLEX_INFO *)pDrawItemInfo->p)->Index == PROGBAR_SKINFLEX_L)
//			GUI_DrawBitmap(winBMP->normal, x0, y0);//��ҳ��������ʾͼƬ,emWin���Լ����г�����ؼ���Χ��ͼƬ
//		else if(((PROGBAR_SKINFLEX_INFO *)pDrawItemInfo->p)->Index == PROGBAR_SKINFLEX_R)
//			GUI_DrawBitmap(winBMP->normal, x0, y0);
//		else BUTTON_DrawSkinFlex(pDrawItemInfo);
//		WM_SetUserClipRect(0);
		break;
  case WIDGET_ITEM_DRAW_TEXT:
			PROGBARMETER_DispNeedle(pDrawItemInfo, 119, -119, 0, 100);
    	break;
	default: return PROGBAR_DrawSkinFlex(pDrawItemInfo);//emWinĬ�Ͽؼ����ƺ���
	}
	return 0;
}


