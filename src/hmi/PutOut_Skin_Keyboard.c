#include <stddef.h>
#include "DIALOG.h"
#include "keyboard.h"
#include "string.h"
#include "stdlib.h"
#include "pyinput.h"
#include "stringName.h"
#include "WM.h"
#include "BUTTON.h"
#include "cfg_parse.h"
#include "touchtimer.h"
#define lcd_height 480
#define lcd_width 800

WM_HWIN hMulti=0;       //多行文本
WM_HWIN htmpChild;      //用于存放传递过来的子窗口句柄
WM_HWIN htmpBK;         //用于存放传递过来的背景窗口句柄
static uint8_t htmpID;  //用于存放传递过来的设置句柄ID
BUTTON_Handle _aahButtonOk;
BUTTON_Handle _aahButtonCancel;
EDIT_Handle _aahEditVar;
EDIT_Handle _aahEditEg;//示例
KEYPADStructTypeDef keypad_dev;

uint8_t ManagerSetOptions = 0;
uint8_t *passwd = "888888";
static int _DrawSkinFlex_BUTTON(const WIDGET_ITEM_DRAW_INFO * pDrawItemInfo);
static int _DrawChineseSkin_BUTTON(const WIDGET_ITEM_DRAW_INFO * pDrawItemInfo);

typedef struct
{
	int          xPos;              //按钮X位置
	int          yPos;              //按钮Y位置
	int          xSize;             //按钮宽度
	int          ySize;             //按钮高度
	const char * acLabel;           //按钮所代表的字符
	void (* pfDraw)(WM_HWIN hWin);  //按钮的绘制函数
    const char * ButString;         //按钮上显示的字符
} BUTTON_DATA;


//在按钮上绘制位图，比如回车键，删除键等
static void _DrawCentered(WM_HWIN hWin, const GUI_BITMAP * pBM)
{
	int xSizeWin;
	int ySizeWin;
	int xSizeBMP;
	int ySizeBMP;
	int xPos;
	int yPos;

	xSizeWin = WM_GetWindowSizeX(hWin);
	ySizeWin = WM_GetWindowSizeY(hWin);
	xSizeBMP = pBM->XSize;
	ySizeBMP = pBM->YSize;
	xPos = (xSizeWin - xSizeBMP) >> 1;
	yPos = (ySizeWin - ySizeBMP) >> 1;
	GUI_DrawBitmap(pBM, xPos, yPos);
}

//static const unsigned char _aahEyeOn[] = {
//
//
//
//
//
//};
//static const unsigned char _aahEyeOff[] = {
//
//
//
//
//
//};
//static const GUI_COLOR Colors[] = { 0x000000, 0xFFFFFF };
//static const GUI_LOGPALETTE Palette = { 2, 1, Colors };
//static const GUI_BITMAP bm_bpp_on = { 5, 5, 4, 1, _aahEyeOn, &Palette};
//static const GUI_BITMAP bm_bpp_off = { 5, 5, 4, 1, _aahEyeOff, &Palette};

static void _DrawBkSpc (WM_HWIN hWin) {  _DrawCentered(hWin, &bmBackSpace); }	//绘制退格键(删除键)
static void _DrawEnter (WM_HWIN hWin) {  _DrawCentered(hWin, &bmEnter); }		//绘制回车键
static void _DrawShift (WM_HWIN hWin) {  _DrawCentered(hWin, &bmShift); }		//绘制shift键
static void _DrawRetern(WM_HWIN hWin) {  _DrawCentered(hWin, &bmReturn); }		//绘制返回键


//英文键盘位置
static  BUTTON_DATA _aEngButtonData[] = {
	{ (ENG_BUTTON_XPAC*1+ENG_BUTTON_WIDTH*0+ENG_BUTTONLINE1_XOFFSET),  ENG_BUTTON_LINE1YPOS, ENG_BUTTON_WIDTH,ENG_BUTTON_HEIGHT, "Q",0,"Q" },
	{ (ENG_BUTTON_XPAC*2+ENG_BUTTON_WIDTH*1+ENG_BUTTONLINE1_XOFFSET),  ENG_BUTTON_LINE1YPOS, ENG_BUTTON_WIDTH,ENG_BUTTON_HEIGHT, "W",0,"W" },
	{ (ENG_BUTTON_XPAC*3+ENG_BUTTON_WIDTH*2+ENG_BUTTONLINE1_XOFFSET),  ENG_BUTTON_LINE1YPOS, ENG_BUTTON_WIDTH,ENG_BUTTON_HEIGHT, "E",0,"E" },
	{ (ENG_BUTTON_XPAC*4+ENG_BUTTON_WIDTH*3+ENG_BUTTONLINE1_XOFFSET),  ENG_BUTTON_LINE1YPOS, ENG_BUTTON_WIDTH,ENG_BUTTON_HEIGHT, "R",0,"R" },
	{ (ENG_BUTTON_XPAC*5+ENG_BUTTON_WIDTH*4+ENG_BUTTONLINE1_XOFFSET),  ENG_BUTTON_LINE1YPOS, ENG_BUTTON_WIDTH,ENG_BUTTON_HEIGHT, "T",0,"T" },
	{ (ENG_BUTTON_XPAC*6+ENG_BUTTON_WIDTH*5+ENG_BUTTONLINE1_XOFFSET),  ENG_BUTTON_LINE1YPOS, ENG_BUTTON_WIDTH,ENG_BUTTON_HEIGHT, "Y",0,"Y" },
	{ (ENG_BUTTON_XPAC*7+ENG_BUTTON_WIDTH*6+ENG_BUTTONLINE1_XOFFSET),  ENG_BUTTON_LINE1YPOS, ENG_BUTTON_WIDTH,ENG_BUTTON_HEIGHT, "U",0,"U" },
	{ (ENG_BUTTON_XPAC*8+ENG_BUTTON_WIDTH*7+ENG_BUTTONLINE1_XOFFSET),  ENG_BUTTON_LINE1YPOS, ENG_BUTTON_WIDTH,ENG_BUTTON_HEIGHT, "I",0,"I" },
	{ (ENG_BUTTON_XPAC*9+ENG_BUTTON_WIDTH*8+ENG_BUTTONLINE1_XOFFSET),  ENG_BUTTON_LINE1YPOS, ENG_BUTTON_WIDTH,ENG_BUTTON_HEIGHT, "O",0,"O" },
	{ (ENG_BUTTON_XPAC*10+ENG_BUTTON_WIDTH*9+ENG_BUTTONLINE1_XOFFSET), ENG_BUTTON_LINE1YPOS, ENG_BUTTON_WIDTH,ENG_BUTTON_HEIGHT, "P",0,"P" },

	{ (ENG_BUTTON_XPAC*1+ENG_BUTTON_WIDTH*0+ENG_BUTTONLINE2_XOFFSET),  ENG_BUTTON_LINE2YPOS, ENG_BUTTON_WIDTH,ENG_BUTTON_HEIGHT, "A",0,"A" },
	{ (ENG_BUTTON_XPAC*2+ENG_BUTTON_WIDTH*1+ENG_BUTTONLINE2_XOFFSET),  ENG_BUTTON_LINE2YPOS, ENG_BUTTON_WIDTH,ENG_BUTTON_HEIGHT, "S",0,"S" },
	{ (ENG_BUTTON_XPAC*3+ENG_BUTTON_WIDTH*2+ENG_BUTTONLINE2_XOFFSET),  ENG_BUTTON_LINE2YPOS, ENG_BUTTON_WIDTH,ENG_BUTTON_HEIGHT, "D",0,"D" },
	{ (ENG_BUTTON_XPAC*4+ENG_BUTTON_WIDTH*3+ENG_BUTTONLINE2_XOFFSET),  ENG_BUTTON_LINE2YPOS, ENG_BUTTON_WIDTH,ENG_BUTTON_HEIGHT, "F",0,"F" },
	{ (ENG_BUTTON_XPAC*5+ENG_BUTTON_WIDTH*4+ENG_BUTTONLINE2_XOFFSET),  ENG_BUTTON_LINE2YPOS, ENG_BUTTON_WIDTH,ENG_BUTTON_HEIGHT, "G",0,"G" },
	{ (ENG_BUTTON_XPAC*6+ENG_BUTTON_WIDTH*5+ENG_BUTTONLINE2_XOFFSET),  ENG_BUTTON_LINE2YPOS, ENG_BUTTON_WIDTH,ENG_BUTTON_HEIGHT, "H",0,"H" },
	{ (ENG_BUTTON_XPAC*7+ENG_BUTTON_WIDTH*6+ENG_BUTTONLINE2_XOFFSET),  ENG_BUTTON_LINE2YPOS, ENG_BUTTON_WIDTH,ENG_BUTTON_HEIGHT, "J",0,"J" },
	{ (ENG_BUTTON_XPAC*8+ENG_BUTTON_WIDTH*7+ENG_BUTTONLINE2_XOFFSET),  ENG_BUTTON_LINE2YPOS, ENG_BUTTON_WIDTH,ENG_BUTTON_HEIGHT, "K",0,"K" },
	{ (ENG_BUTTON_XPAC*9+ENG_BUTTON_WIDTH*8+ENG_BUTTONLINE2_XOFFSET),  ENG_BUTTON_LINE2YPOS, ENG_BUTTON_WIDTH,ENG_BUTTON_HEIGHT, "L",0,"L" },

	{ (ENG_BUTTON_XPAC*1+ENG_BUTTON_WIDTH*0+ENG_BUTTONLINE3_XOFFSET),  ENG_BUTTON_LINE3YPOS, ENG_BUTTON_WIDTH+ENG_BUTTONSHIFT_XINC,ENG_BUTTON_HEIGHT, "\x19", _DrawShift },	//shift的键值为0X19
	{ (ENG_BUTTON_XPAC*2+ENG_BUTTON_WIDTH*1+ENG_BUTTONLINE3_XOFFSET+ENG_BUTTONSHIFT_XINC),  ENG_BUTTON_LINE3YPOS, ENG_BUTTON_WIDTH,ENG_BUTTON_HEIGHT, "Z",0,"Z" },
	{ (ENG_BUTTON_XPAC*3+ENG_BUTTON_WIDTH*2+ENG_BUTTONLINE3_XOFFSET+ENG_BUTTONSHIFT_XINC),  ENG_BUTTON_LINE3YPOS, ENG_BUTTON_WIDTH,ENG_BUTTON_HEIGHT, "X",0,"X" },
	{ (ENG_BUTTON_XPAC*4+ENG_BUTTON_WIDTH*3+ENG_BUTTONLINE3_XOFFSET+ENG_BUTTONSHIFT_XINC),  ENG_BUTTON_LINE3YPOS, ENG_BUTTON_WIDTH,ENG_BUTTON_HEIGHT, "C",0,"C" },
	{ (ENG_BUTTON_XPAC*5+ENG_BUTTON_WIDTH*4+ENG_BUTTONLINE3_XOFFSET+ENG_BUTTONSHIFT_XINC),  ENG_BUTTON_LINE3YPOS, ENG_BUTTON_WIDTH,ENG_BUTTON_HEIGHT, "V",0,"V" },
	{ (ENG_BUTTON_XPAC*6+ENG_BUTTON_WIDTH*5+ENG_BUTTONLINE3_XOFFSET+ENG_BUTTONSHIFT_XINC),  ENG_BUTTON_LINE3YPOS, ENG_BUTTON_WIDTH,ENG_BUTTON_HEIGHT, "B",0,"B" },
	{ (ENG_BUTTON_XPAC*7+ENG_BUTTON_WIDTH*6+ENG_BUTTONLINE3_XOFFSET+ENG_BUTTONSHIFT_XINC),  ENG_BUTTON_LINE3YPOS, ENG_BUTTON_WIDTH,ENG_BUTTON_HEIGHT, "N",0,"N" },
	{ (ENG_BUTTON_XPAC*8+ENG_BUTTON_WIDTH*7+ENG_BUTTONLINE3_XOFFSET+ENG_BUTTONSHIFT_XINC),  ENG_BUTTON_LINE3YPOS, ENG_BUTTON_WIDTH,ENG_BUTTON_HEIGHT, "M",0,"M" },
	{ (ENG_BUTTON_XPAC*9+ENG_BUTTON_WIDTH*8+ENG_BUTTONLINE3_XOFFSET+ENG_BUTTONSHIFT_XINC),  ENG_BUTTON_LINE3YPOS, ENG_BUTTON_WIDTH+ENG_BUTTONENTER_XINC,ENG_BUTTON_HEIGHT, "\x08",_DrawBkSpc},

	{ (ENG_BUTTON_XPAC*1+ENG_BUTTON_WIDTH*0+ENG_BUTTONLINE4_XOFFSET),  ENG_BUTTON_LINE4YPOS, ENG_BUTTON_WIDTH+ENG_BUTTONFIGURE_XINC,ENG_BUTTON_HEIGHT, "\x02",0,"数"},						    //切换数组键盘
	{ (ENG_BUTTON_XPAC*2+ENG_BUTTON_WIDTH*1+ENG_BUTTONLINE4_XOFFSET+ENG_BUTTONFIGURE_XINC),  ENG_BUTTON_LINE4YPOS, ENG_BUTTON_WIDTH+ENG_BUTTONSYMBOL_XINC,ENG_BUTTON_HEIGHT,  "\x03",0,"符"},	//切换符号键盘
	{ (ENG_BUTTON_XPAC*3+ENG_BUTTON_WIDTH*2+ENG_BUTTONLINE4_XOFFSET+ENG_BUTTONFIGURE_XINC+ENG_BUTTONSYMBOL_XINC),  ENG_BUTTON_LINE4YPOS, ENG_BUTTON_WIDTH,ENG_BUTTON_HEIGHT,  ",",0,"," },
	{ (ENG_BUTTON_XPAC*4+ENG_BUTTON_WIDTH*3+ENG_BUTTONLINE4_XOFFSET+ENG_BUTTONFIGURE_XINC+ENG_BUTTONSYMBOL_XINC),  ENG_BUTTON_LINE4YPOS, ENG_BUTTON_WIDTH+ENG_BUTTONSPACE_XINC,ENG_BUTTON_HEIGHT,  " ",0,"└┘"},
	{ (ENG_BUTTON_XPAC*5+ENG_BUTTON_WIDTH*4+ENG_BUTTONLINE4_XOFFSET+ENG_BUTTONFIGURE_XINC+ENG_BUTTONSYMBOL_XINC+ENG_BUTTONSPACE_XINC),  ENG_BUTTON_LINE4YPOS, ENG_BUTTON_WIDTH,ENG_BUTTON_HEIGHT, "." ,0,"."},
	{ (ENG_BUTTON_XPAC*6+ENG_BUTTON_WIDTH*5+ENG_BUTTONLINE4_XOFFSET+ENG_BUTTONFIGURE_XINC+ENG_BUTTONSYMBOL_XINC+ENG_BUTTONSPACE_XINC),  ENG_BUTTON_LINE4YPOS, ENG_BUTTON_WIDTH+ENG_BUTTONCE_XINC,ENG_BUTTON_HEIGHT, "\x0B",0,"中"},	//切换到中文键盘
	{ (ENG_BUTTON_XPAC*7+ENG_BUTTON_WIDTH*6+ENG_BUTTONLINE4_XOFFSET+ENG_BUTTONFIGURE_XINC+ENG_BUTTONSYMBOL_XINC+ENG_BUTTONSPACE_XINC+ENG_BUTTONCE_XINC),  ENG_BUTTON_LINE4YPOS, ENG_BUTTON_WIDTH+ENG_BUTTONBACK_XINC,ENG_BUTTON_HEIGHT, "\x0d",_DrawEnter},
};

//数字键盘位置
static  BUTTON_DATA _aNumButtonData[] = {
	{ (NUM_BUTTON_XPAC*1+NUM_BUTTON_WIDTH*0+NUM_BUTTON_XOFFSET),  NUM_BUTTON_LINE1YPOS,(NUM_BUTTON_WIDTH-NUM_BUTTONSING_XINC),NUM_BUTTON_HEIGHT, "+",0,"+" },
	{ (NUM_BUTTON_XPAC*2+NUM_BUTTON_WIDTH*1+NUM_BUTTON_XOFFSET-NUM_BUTTONSING_XINC),  NUM_BUTTON_LINE1YPOS, NUM_BUTTON_WIDTH,NUM_BUTTON_HEIGHT, "1" ,0,"1"},
	{ (NUM_BUTTON_XPAC*3+NUM_BUTTON_WIDTH*2+NUM_BUTTON_XOFFSET-NUM_BUTTONSING_XINC),  NUM_BUTTON_LINE1YPOS, NUM_BUTTON_WIDTH,NUM_BUTTON_HEIGHT, "2" ,0,"2"},
	{ (NUM_BUTTON_XPAC*4+NUM_BUTTON_WIDTH*3+NUM_BUTTON_XOFFSET-NUM_BUTTONSING_XINC),  NUM_BUTTON_LINE1YPOS, NUM_BUTTON_WIDTH,NUM_BUTTON_HEIGHT, "3" ,0,"3"},
	{ (NUM_BUTTON_XPAC*5+NUM_BUTTON_WIDTH*4+NUM_BUTTON_XOFFSET-NUM_BUTTONSING_XINC),  NUM_BUTTON_LINE1YPOS, (NUM_BUTTON_WIDTH-NUM_BUTTONBACKSPACE_XINC),NUM_BUTTON_HEIGHT, "\x08",_DrawBkSpc},	//删除键

	{ (NUM_BUTTON_XPAC*1+NUM_BUTTON_WIDTH*0+NUM_BUTTON_XOFFSET),  NUM_BUTTON_LINE2YPOS,(NUM_BUTTON_WIDTH-NUM_BUTTONSING_XINC),NUM_BUTTON_HEIGHT, "-",0,"-" },
	{ (NUM_BUTTON_XPAC*2+NUM_BUTTON_WIDTH*1+NUM_BUTTON_XOFFSET-NUM_BUTTONSING_XINC),  NUM_BUTTON_LINE2YPOS, NUM_BUTTON_WIDTH,NUM_BUTTON_HEIGHT, "4" ,0,"4"},
	{ (NUM_BUTTON_XPAC*3+NUM_BUTTON_WIDTH*2+NUM_BUTTON_XOFFSET-NUM_BUTTONSING_XINC),  NUM_BUTTON_LINE2YPOS, NUM_BUTTON_WIDTH,NUM_BUTTON_HEIGHT, "5" ,0,"5"},
	{ (NUM_BUTTON_XPAC*4+NUM_BUTTON_WIDTH*3+NUM_BUTTON_XOFFSET-NUM_BUTTONSING_XINC),  NUM_BUTTON_LINE2YPOS, NUM_BUTTON_WIDTH,NUM_BUTTON_HEIGHT, "6" ,0,"6"},
	{ (NUM_BUTTON_XPAC*5+NUM_BUTTON_WIDTH*4+NUM_BUTTON_XOFFSET-NUM_BUTTONSING_XINC),  NUM_BUTTON_LINE2YPOS, (NUM_BUTTON_WIDTH-NUM_BUTTONSPACE_XINC),NUM_BUTTON_HEIGHT,  " ",0,"└┘" },	//空格键

	{ (NUM_BUTTON_XPAC*1+NUM_BUTTON_WIDTH*0+NUM_BUTTON_XOFFSET),  NUM_BUTTON_LINE3YPOS,(NUM_BUTTON_WIDTH-NUM_BUTTONSING_XINC),NUM_BUTTON_HEIGHT, "/",0,"/" },
	{ (NUM_BUTTON_XPAC*2+NUM_BUTTON_WIDTH*1+NUM_BUTTON_XOFFSET-NUM_BUTTONSING_XINC),  NUM_BUTTON_LINE3YPOS, NUM_BUTTON_WIDTH,NUM_BUTTON_HEIGHT, "7" ,0,"7"},
	{ (NUM_BUTTON_XPAC*3+NUM_BUTTON_WIDTH*2+NUM_BUTTON_XOFFSET-NUM_BUTTONSING_XINC),  NUM_BUTTON_LINE3YPOS, NUM_BUTTON_WIDTH,NUM_BUTTON_HEIGHT, "8" ,0,"8"},
	{ (NUM_BUTTON_XPAC*4+NUM_BUTTON_WIDTH*3+NUM_BUTTON_XOFFSET-NUM_BUTTONSING_XINC),  NUM_BUTTON_LINE3YPOS, NUM_BUTTON_WIDTH,NUM_BUTTON_HEIGHT, "9" ,0,"9"},
	{ (NUM_BUTTON_XPAC*5+NUM_BUTTON_WIDTH*4+NUM_BUTTON_XOFFSET-NUM_BUTTONSING_XINC),  NUM_BUTTON_LINE3YPOS, (NUM_BUTTON_WIDTH-NUM_BUTTONENTER_XINC),NUM_BUTTON_HEIGHT, "\x0d",_DrawEnter},	//回车键

	{ (NUM_BUTTON_XPAC*1+NUM_BUTTON_WIDTH*0+NUM_BUTTON_XOFFSET),  NUM_BUTTON_LINE4YPOS,(NUM_BUTTON_WIDTH-NUM_BUTTONSING_XINC),NUM_BUTTON_HEIGHT, "@",0,"@" },
	{ (NUM_BUTTON_XPAC*2+NUM_BUTTON_WIDTH*1+NUM_BUTTON_XOFFSET-NUM_BUTTONSING_XINC),  NUM_BUTTON_LINE4YPOS, NUM_BUTTON_WIDTH,NUM_BUTTON_HEIGHT, "," ,0,","},
	{ (NUM_BUTTON_XPAC*3+NUM_BUTTON_WIDTH*2+NUM_BUTTON_XOFFSET-NUM_BUTTONSING_XINC),  NUM_BUTTON_LINE4YPOS, NUM_BUTTON_WIDTH,NUM_BUTTON_HEIGHT, "0" ,0,"0"},
	{ (NUM_BUTTON_XPAC*4+NUM_BUTTON_WIDTH*3+NUM_BUTTON_XOFFSET-NUM_BUTTONSING_XINC),  NUM_BUTTON_LINE4YPOS, NUM_BUTTON_WIDTH,NUM_BUTTON_HEIGHT, "." ,0,"."},
	{ (NUM_BUTTON_XPAC*5+NUM_BUTTON_WIDTH*4+NUM_BUTTON_XOFFSET-NUM_BUTTONSING_XINC),  NUM_BUTTON_LINE4YPOS, (NUM_BUTTON_WIDTH-NUM_BUTTONRETURN_XINC),NUM_BUTTON_HEIGHT, "\x04",_DrawRetern },//返回键
};

//符号键盘参数
static  BUTTON_DATA _aSinButtonData[][20] = {
	{
		{ (SIGN_BUTTON_XPAC*1+SING_BUTTON_WIDTH*0+SIGN_BUTTON_XOFFSET),  SIGN_BUTTON_LINE1YPOS, SING_BUTTON_WIDTH,SIGN_BUTTON_HEIGHT, "~" ,0,"~"},
		{ (SIGN_BUTTON_XPAC*2+SING_BUTTON_WIDTH*1+SIGN_BUTTON_XOFFSET),  SIGN_BUTTON_LINE1YPOS, SING_BUTTON_WIDTH,SIGN_BUTTON_HEIGHT, "-" ,0,"-"},
		{ (SIGN_BUTTON_XPAC*3+SING_BUTTON_WIDTH*2+SIGN_BUTTON_XOFFSET),  SIGN_BUTTON_LINE1YPOS, SING_BUTTON_WIDTH,SIGN_BUTTON_HEIGHT, ":" ,0,":"},
		{ (SIGN_BUTTON_XPAC*4+SING_BUTTON_WIDTH*3+SIGN_BUTTON_XOFFSET),  SIGN_BUTTON_LINE1YPOS, SING_BUTTON_WIDTH,SIGN_BUTTON_HEIGHT, "_" ,0,"_"},
		{ (SIGN_BUTTON_XPAC*5+SING_BUTTON_WIDTH*4+SIGN_BUTTON_XOFFSET),  SIGN_BUTTON_LINE1YPOS, (SING_BUTTON_WIDTH-SIGN_BUTTONCONTR_XINC),SIGN_BUTTON_HEIGHT, "\x08",_DrawBkSpc},	//删除键

		{ (SIGN_BUTTON_XPAC*1+SING_BUTTON_WIDTH*0+SIGN_BUTTON_XOFFSET),  SIGN_BUTTON_LINE2YPOS, SING_BUTTON_WIDTH,SIGN_BUTTON_HEIGHT, "\"",0,"\"" },
		{ (SIGN_BUTTON_XPAC*2+SING_BUTTON_WIDTH*1+SIGN_BUTTON_XOFFSET),  SIGN_BUTTON_LINE2YPOS, SING_BUTTON_WIDTH,SIGN_BUTTON_HEIGHT, "," ,0,","},
		{ (SIGN_BUTTON_XPAC*3+SING_BUTTON_WIDTH*2+SIGN_BUTTON_XOFFSET),  SIGN_BUTTON_LINE2YPOS, SING_BUTTON_WIDTH,SIGN_BUTTON_HEIGHT, "<" ,0,"<"},
		{ (SIGN_BUTTON_XPAC*4+SING_BUTTON_WIDTH*3+SIGN_BUTTON_XOFFSET),  SIGN_BUTTON_LINE2YPOS, SING_BUTTON_WIDTH,SIGN_BUTTON_HEIGHT, ">" ,0,">"},
		{ (SIGN_BUTTON_XPAC*5+SING_BUTTON_WIDTH*4+SIGN_BUTTON_XOFFSET),  SIGN_BUTTON_LINE2YPOS, (SING_BUTTON_WIDTH-SIGN_BUTTONCONTR_XINC),SIGN_BUTTON_HEIGHT,  "\x05",0,"∧"},	//上一页

		{ (SIGN_BUTTON_XPAC*1+SING_BUTTON_WIDTH*0+SIGN_BUTTON_XOFFSET),  SIGN_BUTTON_LINE3YPOS, SING_BUTTON_WIDTH,NUM_BUTTON_HEIGHT, "*",0,"*" },
		{ (SIGN_BUTTON_XPAC*2+SING_BUTTON_WIDTH*1+SIGN_BUTTON_XOFFSET),  SIGN_BUTTON_LINE3YPOS, SING_BUTTON_WIDTH,NUM_BUTTON_HEIGHT, "+",0,"+" },
		{ (SIGN_BUTTON_XPAC*3+SING_BUTTON_WIDTH*2+SIGN_BUTTON_XOFFSET),  SIGN_BUTTON_LINE3YPOS, SING_BUTTON_WIDTH,NUM_BUTTON_HEIGHT, ".",0,"." },
		{ (SIGN_BUTTON_XPAC*4+SING_BUTTON_WIDTH*3+SIGN_BUTTON_XOFFSET),  SIGN_BUTTON_LINE3YPOS, SING_BUTTON_WIDTH,NUM_BUTTON_HEIGHT, "#",0,"#" },
		{ (SIGN_BUTTON_XPAC*5+SING_BUTTON_WIDTH*4+SIGN_BUTTON_XOFFSET),  SIGN_BUTTON_LINE3YPOS, (SING_BUTTON_WIDTH-SIGN_BUTTONCONTR_XINC),SIGN_BUTTON_HEIGHT, "\x06",0,"∨"},	//下一页

		{ (SIGN_BUTTON_XPAC*1+SING_BUTTON_WIDTH*0+SIGN_BUTTON_XOFFSET),  SIGN_BUTTON_LINE4YPOS, SING_BUTTON_WIDTH,NUM_BUTTON_HEIGHT, "%",0,"%"},
		{ (SIGN_BUTTON_XPAC*2+SING_BUTTON_WIDTH*1+SIGN_BUTTON_XOFFSET),  SIGN_BUTTON_LINE4YPOS, SING_BUTTON_WIDTH,NUM_BUTTON_HEIGHT, "@",0,"@" },
		{ (SIGN_BUTTON_XPAC*3+SING_BUTTON_WIDTH*2+SIGN_BUTTON_XOFFSET),  SIGN_BUTTON_LINE4YPOS, SING_BUTTON_WIDTH,NUM_BUTTON_HEIGHT, "!",0,"!" },
		{ (SIGN_BUTTON_XPAC*4+SING_BUTTON_WIDTH*3+SIGN_BUTTON_XOFFSET),  SIGN_BUTTON_LINE4YPOS, SING_BUTTON_WIDTH,NUM_BUTTON_HEIGHT, "/",0,"/" },
		{ (SIGN_BUTTON_XPAC*5+SING_BUTTON_WIDTH*4+SIGN_BUTTON_XOFFSET),  SIGN_BUTTON_LINE4YPOS, (SING_BUTTON_WIDTH-SIGN_BUTTONCONTR_XINC),SIGN_BUTTON_HEIGHT, "\x04",_DrawRetern },//返回键
	},

	{
		{ (SIGN_BUTTON_XPAC*1+SING_BUTTON_WIDTH*0+SIGN_BUTTON_XOFFSET),  SIGN_BUTTON_LINE1YPOS, SING_BUTTON_WIDTH,SIGN_BUTTON_HEIGHT, "?",0,"?" },
		{ (SIGN_BUTTON_XPAC*2+SING_BUTTON_WIDTH*1+SIGN_BUTTON_XOFFSET),  SIGN_BUTTON_LINE1YPOS, SING_BUTTON_WIDTH,SIGN_BUTTON_HEIGHT, ";",0,";" },
		{ (SIGN_BUTTON_XPAC*3+SING_BUTTON_WIDTH*2+SIGN_BUTTON_XOFFSET),  SIGN_BUTTON_LINE1YPOS, SING_BUTTON_WIDTH,SIGN_BUTTON_HEIGHT, "`",0,"`" },
		{ (SIGN_BUTTON_XPAC*4+SING_BUTTON_WIDTH*3+SIGN_BUTTON_XOFFSET),  SIGN_BUTTON_LINE1YPOS, SING_BUTTON_WIDTH,SIGN_BUTTON_HEIGHT, "$",0,"$" },
		{ (SIGN_BUTTON_XPAC*5+SING_BUTTON_WIDTH*4+SIGN_BUTTON_XOFFSET),  SIGN_BUTTON_LINE1YPOS, (SING_BUTTON_WIDTH-SIGN_BUTTONCONTR_XINC),SIGN_BUTTON_HEIGHT, "\x08",_DrawBkSpc},	//删除键

		{ (SIGN_BUTTON_XPAC*1+SING_BUTTON_WIDTH*0+SIGN_BUTTON_XOFFSET),  SIGN_BUTTON_LINE2YPOS, SING_BUTTON_WIDTH,SIGN_BUTTON_HEIGHT, "^" ,0,"^"},
		{ (SIGN_BUTTON_XPAC*2+SING_BUTTON_WIDTH*1+SIGN_BUTTON_XOFFSET),  SIGN_BUTTON_LINE2YPOS, SING_BUTTON_WIDTH,SIGN_BUTTON_HEIGHT, "\\",0,"\\" },
		{ (SIGN_BUTTON_XPAC*3+SING_BUTTON_WIDTH*2+SIGN_BUTTON_XOFFSET),  SIGN_BUTTON_LINE2YPOS, SING_BUTTON_WIDTH,SIGN_BUTTON_HEIGHT, "(" ,0,"("},
		{ (SIGN_BUTTON_XPAC*4+SING_BUTTON_WIDTH*3+SIGN_BUTTON_XOFFSET),  SIGN_BUTTON_LINE2YPOS, SING_BUTTON_WIDTH,SIGN_BUTTON_HEIGHT, ")" ,0,")"},
		{ (SIGN_BUTTON_XPAC*5+SING_BUTTON_WIDTH*4+SIGN_BUTTON_XOFFSET),  SIGN_BUTTON_LINE2YPOS, (SING_BUTTON_WIDTH-SIGN_BUTTONCONTR_XINC),SIGN_BUTTON_HEIGHT,  "\x05",0,"∧" },	//上一页

		{ (SIGN_BUTTON_XPAC*1+SING_BUTTON_WIDTH*0+SIGN_BUTTON_XOFFSET),  SIGN_BUTTON_LINE3YPOS, SING_BUTTON_WIDTH,NUM_BUTTON_HEIGHT, "&",0,"&" },
		{ (SIGN_BUTTON_XPAC*2+SING_BUTTON_WIDTH*1+SIGN_BUTTON_XOFFSET),  SIGN_BUTTON_LINE3YPOS, SING_BUTTON_WIDTH,NUM_BUTTON_HEIGHT, "=",0,"=" },
		{ (SIGN_BUTTON_XPAC*3+SING_BUTTON_WIDTH*2+SIGN_BUTTON_XOFFSET),  SIGN_BUTTON_LINE3YPOS, SING_BUTTON_WIDTH,NUM_BUTTON_HEIGHT, "'",0,"'" },
		{ (SIGN_BUTTON_XPAC*4+SING_BUTTON_WIDTH*3+SIGN_BUTTON_XOFFSET),  SIGN_BUTTON_LINE3YPOS, SING_BUTTON_WIDTH,NUM_BUTTON_HEIGHT, "|",0,"|" },
		{ (SIGN_BUTTON_XPAC*5+SING_BUTTON_WIDTH*4+SIGN_BUTTON_XOFFSET),  SIGN_BUTTON_LINE3YPOS, (SING_BUTTON_WIDTH-SIGN_BUTTONCONTR_XINC),SIGN_BUTTON_HEIGHT, "\x06",0,"∨"},	//下一页

		{ (SIGN_BUTTON_XPAC*1+SING_BUTTON_WIDTH*0+SIGN_BUTTON_XOFFSET),  SIGN_BUTTON_LINE4YPOS, SING_BUTTON_WIDTH,NUM_BUTTON_HEIGHT, "[",0,"[" },
		{ (SIGN_BUTTON_XPAC*2+SING_BUTTON_WIDTH*1+SIGN_BUTTON_XOFFSET),  SIGN_BUTTON_LINE4YPOS, SING_BUTTON_WIDTH,NUM_BUTTON_HEIGHT, "]",0,"]" },
		{ (SIGN_BUTTON_XPAC*3+SING_BUTTON_WIDTH*2+SIGN_BUTTON_XOFFSET),  SIGN_BUTTON_LINE4YPOS, SING_BUTTON_WIDTH,NUM_BUTTON_HEIGHT, "{",0,"{" },
		{ (SIGN_BUTTON_XPAC*4+SING_BUTTON_WIDTH*3+SIGN_BUTTON_XOFFSET),  SIGN_BUTTON_LINE4YPOS, SING_BUTTON_WIDTH,NUM_BUTTON_HEIGHT, "}",0,"}" },
		{ (SIGN_BUTTON_XPAC*5+SING_BUTTON_WIDTH*4+SIGN_BUTTON_XOFFSET),  SIGN_BUTTON_LINE4YPOS, (SING_BUTTON_WIDTH-SIGN_BUTTONCONTR_XINC),SIGN_BUTTON_HEIGHT, "\x04",_DrawRetern },//返回键
	},
};

//中文键盘1位置
static  BUTTON_DATA _aChineseButtonData1[] = {
    //中文键盘第一部分
	{ (CHIN1_BUTTON_XPAC*1+CHIN1_BUTTON_WIDTH*0+CHIN1_BUTTON_XOFFSET),  CHIN1_BUTTON_LINE1YPOS, CHIN1_BUTTON_WIDTH,CHIN1_BUTTON_HEIGHT, ";" ,0,";" },
	{ (CHIN1_BUTTON_XPAC*2+CHIN1_BUTTON_WIDTH*1+CHIN1_BUTTON_XOFFSET),  CHIN1_BUTTON_LINE1YPOS, CHIN1_BUTTON_WIDTH,CHIN1_BUTTON_HEIGHT, "2" ,0,"ABC"},
	{ (CHIN1_BUTTON_XPAC*3+CHIN1_BUTTON_WIDTH*2+CHIN1_BUTTON_XOFFSET),  CHIN1_BUTTON_LINE1YPOS, CHIN1_BUTTON_WIDTH,CHIN1_BUTTON_HEIGHT, "3" ,0,"DEF"},
	{ (CHIN1_BUTTON_XPAC*4+CHIN1_BUTTON_WIDTH*3+CHIN1_BUTTON_XOFFSET),  CHIN1_BUTTON_LINE1YPOS, (CHIN1_BUTTON_WIDTH-CHIN1_BUTTONCONTR_XINC),CHIN1_BUTTON_HEIGHT, "\x08",_DrawBkSpc},	//删除键

	{ (CHIN1_BUTTON_XPAC*1+CHIN1_BUTTON_WIDTH*0+CHIN1_BUTTON_XOFFSET),  CHIN1_BUTTON_LINE2YPOS, CHIN1_BUTTON_WIDTH,CHIN1_BUTTON_HEIGHT, "4" ,0,"GHI"},
	{ (CHIN1_BUTTON_XPAC*2+CHIN1_BUTTON_WIDTH*1+CHIN1_BUTTON_XOFFSET),  CHIN1_BUTTON_LINE2YPOS, CHIN1_BUTTON_WIDTH,CHIN1_BUTTON_HEIGHT, "5" ,0,"JKL"},
	{ (CHIN1_BUTTON_XPAC*3+CHIN1_BUTTON_WIDTH*2+CHIN1_BUTTON_XOFFSET),  CHIN1_BUTTON_LINE2YPOS, CHIN1_BUTTON_WIDTH,CHIN1_BUTTON_HEIGHT, "6" ,0,"MNO"},
	{ (CHIN1_BUTTON_XPAC*4+CHIN1_BUTTON_WIDTH*3+CHIN1_BUTTON_XOFFSET),  CHIN1_BUTTON_LINE2YPOS, (CHIN1_BUTTON_WIDTH-CHIN1_BUTTONCONTR_XINC),CHIN1_BUTTON_HEIGHT,  " ",0,"└┘" },	//空格键

	{ (CHIN1_BUTTON_XPAC*1+CHIN1_BUTTON_WIDTH*0+CHIN1_BUTTON_XOFFSET),  CHIN1_BUTTON_LINE3YPOS, CHIN1_BUTTON_WIDTH,CHIN1_BUTTON_HEIGHT, "7",0,"PQRS" },
	{ (CHIN1_BUTTON_XPAC*2+CHIN1_BUTTON_WIDTH*1+CHIN1_BUTTON_XOFFSET),  CHIN1_BUTTON_LINE3YPOS, CHIN1_BUTTON_WIDTH,CHIN1_BUTTON_HEIGHT, "8",0,"TUV" },
	{ (CHIN1_BUTTON_XPAC*3+CHIN1_BUTTON_WIDTH*2+CHIN1_BUTTON_XOFFSET),  CHIN1_BUTTON_LINE3YPOS, CHIN1_BUTTON_WIDTH,CHIN1_BUTTON_HEIGHT, "9",0,"WXYZ"},
	{ (CHIN1_BUTTON_XPAC*4+CHIN1_BUTTON_WIDTH*3+CHIN1_BUTTON_XOFFSET),  CHIN1_BUTTON_LINE3YPOS, (CHIN1_BUTTON_WIDTH-CHIN1_BUTTONCONTR_XINC),CHIN1_BUTTON_HEIGHT, "\x04",_DrawRetern },//返回键
};
//中文键盘2位置
static BUTTON_DATA _aChineseButtonData2[] = {
    { (CHIN2_BUTTON_XPAC*1+CHIN2_BUTTON_WIDTH*0+CHIN2_BUTTON_XOFFSET),  CHIN2_BUTTON_LINE1YPOS, CHIN2_BUTTON_WIDTH,CHIN2_BUTTON_HEIGHT, "",0,"＜"},             //向左
	{ (CHIN2_BUTTON_XPAC*2+CHIN2_BUTTON_WIDTH*1+CHIN2_BUTTON_XOFFSET),  CHIN2_BUTTON_LINE1YPOS, CHIN2_BUTTON_WIDTH,CHIN2_BUTTON_HEIGHT, "" },
	{ (CHIN2_BUTTON_XPAC*3+CHIN2_BUTTON_WIDTH*2+CHIN2_BUTTON_XOFFSET),  CHIN2_BUTTON_LINE1YPOS, CHIN2_BUTTON_WIDTH,CHIN2_BUTTON_HEIGHT, "" },
	{ (CHIN2_BUTTON_XPAC*4+CHIN2_BUTTON_WIDTH*3+CHIN2_BUTTON_XOFFSET),  CHIN2_BUTTON_LINE1YPOS, CHIN2_BUTTON_WIDTH,CHIN2_BUTTON_HEIGHT, "" },
    { (CHIN2_BUTTON_XPAC*5+CHIN2_BUTTON_WIDTH*4+CHIN2_BUTTON_XOFFSET),  CHIN2_BUTTON_LINE1YPOS, CHIN2_BUTTON_WIDTH,CHIN2_BUTTON_HEIGHT, "" },
    { (CHIN2_BUTTON_XPAC*6+CHIN2_BUTTON_WIDTH*5+CHIN2_BUTTON_XOFFSET),  CHIN2_BUTTON_LINE1YPOS, CHIN2_BUTTON_WIDTH,CHIN2_BUTTON_HEIGHT, "" },
    { (CHIN2_BUTTON_XPAC*7+CHIN2_BUTTON_WIDTH*6+CHIN2_BUTTON_XOFFSET),  CHIN2_BUTTON_LINE1YPOS, CHIN2_BUTTON_WIDTH,CHIN2_BUTTON_HEIGHT, "" },
    { (CHIN2_BUTTON_XPAC*8+CHIN2_BUTTON_WIDTH*7+CHIN2_BUTTON_XOFFSET),  CHIN2_BUTTON_LINE1YPOS, CHIN2_BUTTON_WIDTH,CHIN2_BUTTON_HEIGHT, "",0,"＞"},             //向右

    { (CHIN2_BUTTON_XPAC*1+CHIN2_BUTTON_WIDTH*0+CHIN2_BUTTON_XOFFSET),  CHIN2_BUTTON_LINE2YPOS, (CHIN2_BUTTON_WIDTH+CHIN2_BUTTONPY_XINC),(CHIN2_BUTTON_HEIGHT+CHIN2_BUTTONPY_YINC), "\x05",0,"∧"},//上一页
    { (CHIN2_BUTTON_XPAC*1+CHIN2_BUTTON_WIDTH*0+CHIN2_BUTTON_XOFFSET),  CHIN2_BUTTON_LINE3YPOS, (CHIN2_BUTTON_WIDTH+CHIN2_BUTTONPY_XINC),(CHIN2_BUTTON_HEIGHT+CHIN2_BUTTONPY_YINC), " " },
    { (CHIN2_BUTTON_XPAC*1+CHIN2_BUTTON_WIDTH*0+CHIN2_BUTTON_XOFFSET),  CHIN2_BUTTON_LINE4YPOS, (CHIN2_BUTTON_WIDTH+CHIN2_BUTTONPY_XINC),(CHIN2_BUTTON_HEIGHT+CHIN2_BUTTONPY_YINC), " " },
    { (CHIN2_BUTTON_XPAC*1+CHIN2_BUTTON_WIDTH*0+CHIN2_BUTTON_XOFFSET),  CHIN2_BUTTON_LINE5YPOS, (CHIN2_BUTTON_WIDTH+CHIN2_BUTTONPY_XINC),(CHIN2_BUTTON_HEIGHT+CHIN2_BUTTONPY_YINC), " " },
    { (CHIN2_BUTTON_XPAC*1+CHIN2_BUTTON_WIDTH*0+CHIN2_BUTTON_XOFFSET),  CHIN2_BUTTON_LINE6YPOS, (CHIN2_BUTTON_WIDTH+CHIN2_BUTTONPY_XINC),(CHIN2_BUTTON_HEIGHT+CHIN2_BUTTONPY_YINC), "\x06",0,"∨" },//下一页

};

//大小写切换处理
void changecase(u8 change_flag,BUTTON_DATA *buttondata)
{
	u16 i;
	char	   	NumToStr;
	char buffer[5];
	if(keypad_dev.LowerCase)		//小写,按钮修改为小写
	{
		//更改按钮的字符
		for (i = 0; i < 27; i++)
		{
			if(i==19) continue;
			NumToStr=(char)((int)*(buttondata[i].acLabel)+0x20);
            sprintf(buffer,"%c",NumToStr);              //转换为字符
			BUTTON_SetFont(keypad_dev.EngButton_Handle[i],&SIF24_Font);
			BUTTON_SetText(keypad_dev.EngButton_Handle[i],buffer);
			BUTTON_SetFocussable(keypad_dev.EngButton_Handle[i], 0);
        }
    }
	else 	//大写，按钮上的字符改为大写
	{
		//更改按钮的字符
		for (i = 0; i < 27; i++)
		{
			if(i==19) continue;
			BUTTON_SetFont(keypad_dev.EngButton_Handle[i],&SIF24_Font);
			BUTTON_SetText(keypad_dev.EngButton_Handle[i], buttondata[i].acLabel);
			BUTTON_SetFocussable(keypad_dev.EngButton_Handle[i], 0);
		}
	}
}

//绘制英文键盘
//hWin:父窗口
void drawenglish_pad(WM_HWIN hWin)
{
	u16 	i;

	//创建键盘按钮
	//BUTTON_SetDefaultSkin(_DrawSkinFlex_BUTTON); //设置BUTTON的皮肤
	for (i = 0; i < GUI_COUNTOF(_aEngButtonData); i++)
	{
		//创建按钮
		keypad_dev.EngButton_Handle[i] = BUTTON_CreateEx(_aEngButtonData[i].xPos, _aEngButtonData[i].yPos, _aEngButtonData[i].xSize, _aEngButtonData[i].ySize,
                             hWin, WM_CF_SHOW | WM_CF_HASTRANS, 0, ID_BUTTON + i);
		BUTTON_SetFont(keypad_dev.EngButton_Handle[i],&SIF24_Font);
        BUTTON_SetText(keypad_dev.EngButton_Handle[i], _aEngButtonData[i].ButString);
		BUTTON_SetFocussable(keypad_dev.EngButton_Handle[i], 0);
        BUTTON_SetSkin(keypad_dev.EngButton_Handle[i], _DrawSkinFlex_BUTTON);
	}
	changecase(keypad_dev.LowerCase,_aEngButtonData);
}

//绘制数字键盘
void drawnumber_pad(WM_HWIN hWin)
{
	u16 i;
	//BUTTON_SetDefaultSkin(_DrawSkinFlex_BUTTON); //设置BUTTON的皮肤
	for(i=0;i<GUI_COUNTOF(_aNumButtonData);i++)								//创建数字键盘
	{
		//创建按钮
		keypad_dev.NumButton_Handle[i] = BUTTON_CreateEx(_aNumButtonData[i].xPos, _aNumButtonData[i].yPos, _aNumButtonData[i].xSize, _aNumButtonData[i].ySize,
                             hWin, WM_CF_SHOW | WM_CF_HASTRANS, 0, ID_BUTTON + i);
		BUTTON_SetFont(keypad_dev.NumButton_Handle[i],&SIF24_Font);
		BUTTON_SetText(keypad_dev.NumButton_Handle[i], _aNumButtonData[i].ButString);
		BUTTON_SetFocussable(keypad_dev.NumButton_Handle[i], 0);
        BUTTON_SetSkin(keypad_dev.NumButton_Handle[i], _DrawSkinFlex_BUTTON);
	}
}

//绘制符号键盘
void drawsign_pad(WM_HWIN hWin)
{
	u16 i;
	//BUTTON_SetDefaultSkin(_DrawSkinFlex_BUTTON); //设置BUTTON的皮肤
	for(i=0;i<GUI_COUNTOF(_aSinButtonData[keypad_dev.signpad_flag]);i++)					//创建符号键盘
	{
		//创建按钮
		keypad_dev.SignButton_Handle[i] = BUTTON_CreateEx(_aSinButtonData[keypad_dev.signpad_flag][i].xPos, _aSinButtonData[keypad_dev.signpad_flag][i].yPos,
											_aSinButtonData[keypad_dev.signpad_flag][i].xSize,_aSinButtonData[keypad_dev.signpad_flag][i].ySize,
											hWin, WM_CF_SHOW | WM_CF_HASTRANS, 0, ID_BUTTON + i);
		BUTTON_SetFont(keypad_dev.SignButton_Handle[i],&SIF24_Font);
		BUTTON_SetText(keypad_dev.SignButton_Handle[i],_aSinButtonData[keypad_dev.signpad_flag][i].ButString);
		BUTTON_SetFocussable(keypad_dev.SignButton_Handle[i], 0);
        BUTTON_SetSkin(keypad_dev.SignButton_Handle[i], _DrawSkinFlex_BUTTON);
	}
}

//绘制中文键盘1
void drawchinese1_pad(WM_HWIN hWin)
{
	u16 i;
	//BUTTON_SetDefaultSkin(_DrawSkinFlex_BUTTON); //设置BUTTON的皮肤
	for(i=0;i<GUI_COUNTOF(_aChineseButtonData1);i++)					//创建中文键盘1
	{
		//创建按钮
		keypad_dev.ChineseButton_Handle[i] = BUTTON_CreateEx(_aChineseButtonData1[i].xPos, _aChineseButtonData1[i].yPos, _aChineseButtonData1[i].xSize, _aChineseButtonData1[i].ySize,
                             hWin, WM_CF_SHOW | WM_CF_HASTRANS, 0, ID_BUTTON + i);
		BUTTON_SetFont(keypad_dev.ChineseButton_Handle[i],&SIF24_Font);
		BUTTON_SetText(keypad_dev.ChineseButton_Handle[i], _aChineseButtonData1[i].ButString);
		BUTTON_SetFocussable(keypad_dev.ChineseButton_Handle[i], 0);
		BUTTON_SetSkin(keypad_dev.ChineseButton_Handle[i], _DrawSkinFlex_BUTTON);
	}
}

void drawchinese2_pad(WM_HWIN hWin)
{
    u16 i;
    //BUTTON_SetDefaultSkin(_DrawChineseSkin_BUTTON); //设置BUTTON的皮肤
    for(i=0;i<GUI_COUNTOF(_aChineseButtonData2);i++)					//创建中文键盘2
	{
		//创建按钮
		keypad_dev.ChineseButton_Handle[i+12] = BUTTON_CreateEx(_aChineseButtonData2[i].xPos, _aChineseButtonData2[i].yPos, _aChineseButtonData2[i].xSize, _aChineseButtonData2[i].ySize,
                             hWin, WM_CF_SHOW | WM_CF_HASTRANS, 0, ID_BUTTON + i+12);
		BUTTON_SetFont(keypad_dev.ChineseButton_Handle[i+12],&SIF24_Font);
		BUTTON_SetText(keypad_dev.ChineseButton_Handle[i+12], _aChineseButtonData2[i].ButString);
		BUTTON_SetFocussable(keypad_dev.ChineseButton_Handle[i+12], 0);
		BUTTON_SetSkin(keypad_dev.ChineseButton_Handle[i+12], _DrawSkinFlex_BUTTON);
	}
}

//英文键盘处理函数
//buttondata:英文键盘按钮参数
//Id:按钮ID
//*pMsg:指向消息
void engkeypad_process(BUTTON_DATA *buttondata,int Id,WM_MESSAGE *pMsg)
{
	char 		c;
	WM_MESSAGE 	Msg;
    u8          KeyID;

    KeyID=Id-ID_BUTTON;
	if (buttondata[Id - ID_BUTTON].acLabel)
	{
		c = buttondata[Id - ID_BUTTON].acLabel[0];
        if(KeyID == 19)                             //大小写切换
        {
            keypad_dev.LowerCase ^= 1;		        //对LowerCase进行异或操作
			changecase(keypad_dev.LowerCase,buttondata);
        }
        else  if(KeyID == 28)                       //切换到数字键盘
        {
            keypad_dev.padtype=NUMBER_KEYPAD;		//数字键盘
			Msg.MsgId=CHANGE_ENGLISHPADTO_NUMPAD;
			Msg.hWinSrc=pMsg->hWin;
			WM_SendMessage(keypad_dev.hKeypad,&Msg);
        }
        else if(KeyID == 29)                        //切换到符号键盘
        {
            keypad_dev.padtype=SIGN_KEYPAD;
			Msg.MsgId=CHANGE_ENGLISHPADTO_SIGNPAD1;
			Msg.hWinSrc=pMsg->hWin;
			WM_SendMessage(keypad_dev.hKeypad,&Msg);
        }
        else if(KeyID == 33)                        //切换到中文键盘
        {
            keypad_dev.padtype=CHINESE_KEYPAD;
			Msg.MsgId=CHANGE_ENGLISHPADTO_CHINESEPAD;
			Msg.hWinSrc=pMsg->hWin;
			WM_SendMessage(keypad_dev.hKeypad,&Msg);
        }
        else
        {
            if (keypad_dev.LowerCase && (c>=0X41) && (c<=0X5A)) //小写
			{
				c += 0x20;
			}
			GUI_StoreKeyMsg(c, 1);	//把消息存进键盘缓冲器，按下状态
			GUI_StoreKeyMsg(c, 0);	//把消息存进键盘缓冲器，松开状态
        }
	}
}

//数字键盘处理函数
//buttondata:数字键盘按钮参数
//Id:按钮ID
//*pMsg:指向消息
void numkeypad_process(BUTTON_DATA *buttondata,int Id,WM_MESSAGE *pMsg)
{
	char 		c;
	WM_MESSAGE 	Msg;
    u8          KeyID;

    KeyID=Id-ID_BUTTON;
	if (buttondata[Id - ID_BUTTON].acLabel)
	{
		c = buttondata[Id - ID_BUTTON].acLabel[0];
        if(KeyID == 19)                             //返回英文键盘
        {
            keypad_dev.padtype=ENGLISH_KEYPAD;		//英文键盘
			Msg.MsgId=CHANGE_NUMPADTO_ENGLISHPAD;
			Msg.hWinSrc=pMsg->hWin;
			WM_SendMessage(keypad_dev.hKeypad,&Msg);
        }
        else
        {
            GUI_StoreKeyMsg(c, 1);				//把消息存进键盘缓冲器，按下状态
			GUI_StoreKeyMsg(c, 0);				//把消息存进键盘缓冲器，松开状态
        }
	}
}
//符号键盘处理函数
//buttondata:数字键盘按钮参数
//Id:按钮ID
//*pMsg:指向消息
void signkeypad_process(BUTTON_DATA *buttondata,int Id,WM_MESSAGE *pMsg)
{
	char 		c;
	WM_MESSAGE 	Msg;
    u8          KeyID;

    KeyID=Id-ID_BUTTON;
	if (buttondata[Id - ID_BUTTON].acLabel)
	{
		c = buttondata[Id - ID_BUTTON].acLabel[0];
        if(KeyID == 19)                             //返回到英文键盘
        {
            keypad_dev.padtype=ENGLISH_KEYPAD;		//英文键盘
			Msg.MsgId=CHANGE_SIGNPADTO_ENGLISHPAD;
			Msg.hWinSrc=pMsg->hWin;
			WM_SendMessage(keypad_dev.hKeypad,&Msg);
        }
        else if(KeyID == 14)                        //下一个符号键盘
        {
            if(keypad_dev.signpad_flag<(keypad_dev.signpad_num-1))
			{
				keypad_dev.signpad_flag++;	        //符号键盘索引加一
				Msg.MsgId=CHANGE_SIGNPAD;
				Msg.hWinSrc=pMsg->hWin;
				WM_SendMessage(keypad_dev.hKeypad,&Msg);
			}
        }
        else if(KeyID == 9)                         //上一个符号键盘
        {
            if(keypad_dev.signpad_flag>0)
			{
				keypad_dev.signpad_flag--;          //符号键盘索引减1
				Msg.MsgId=CHANGE_SIGNPAD;
				Msg.hWinSrc=pMsg->hWin;
				WM_SendMessage(keypad_dev.hKeypad,&Msg);
			}
        }
        else
        {
            GUI_StoreKeyMsg(c, 1);				//把消息存进键盘缓冲器，按下状态
			GUI_StoreKeyMsg(c, 0);				//把消息存进键盘缓冲器，松开状态
        }
	}
}


//中文处理函数
//buttondata:数字键盘按钮参数
//Id:按钮ID
//*pMsg:指向消息
void chinesekeypad_process(BUTTON_DATA *buttondata,int Id,WM_MESSAGE *pMsg)
{
	char 		c;
	WM_MESSAGE 	Msg;
    u8          key;
    u8          KeyID;
    u8          TempID;
    u8          i=0;

    KeyID=Id-ID_BUTTON;
	if (buttondata[Id - ID_BUTTON].acLabel)
	{
		c = buttondata[Id - ID_BUTTON].acLabel[0];
        if(KeyID == 3)                             //删除键
        {
            keypad_dev.cur_index=1;                 //当删除输入的拼音的时候，强制让索引为第一个
            if(keypad_dev.inputlen)
            {
                keypad_dev.inputlen--;
                keypad_dev.inputstr[keypad_dev.inputlen]='\0';  //添加结束符
            }
            else       //没有字符串输入
            {
                GUI_StoreKeyMsg(c, 1);				//把消息存进键盘缓冲器，按下状态
                GUI_StoreKeyMsg(c, 0);				//把消息存进键盘缓冲器，松开状态
            }
        }else if(KeyID == 11)                       //英文键盘
        {
            keypad_dev.padtype=ENGLISH_KEYPAD;		//英文键盘
			Msg.MsgId=CHANGE_CHINESEPADTO_ENGLISHPAD;
			Msg.hWinSrc=pMsg->hWin;
			WM_SendMessage(keypad_dev.hKeypad,&Msg);
        }else if(KeyID == 20)                        //拼音上一页
        {
            if(keypad_dev.pypagenum>0)
            {
                if(keypad_dev.pynowpage>0)keypad_dev.pynowpage--;
                else keypad_dev.pynowpage=0;
            }
        }else  if(KeyID == 24)                      //拼音下一页
        {
            if(keypad_dev.pypagenum>0)
            {
                if(keypad_dev.pynowpage<(keypad_dev.pypagenum-1)) keypad_dev.pynowpage++;
                else keypad_dev.pynowpage=keypad_dev.pypagenum-1;
            }
        }else if(KeyID == 12)                        //汉字上一页
        {
            if(keypad_dev.dispagenum>0)
            {
                if(keypad_dev.dispagecur>0) keypad_dev.dispagecur--;
                else keypad_dev.dispagecur=0;
            }
        }else if(KeyID == 19)                       //汉字下一页
        {
            if(keypad_dev.dispagenum>0)
            {
                if(keypad_dev.dispagecur<(keypad_dev.dispagenum-1)) keypad_dev.dispagecur++;
                else keypad_dev.dispagecur=keypad_dev.dispagenum-1;
            }
        }else if((KeyID > 12) && (KeyID < 19))      //横排
        {
            TempID=KeyID-12;
            TempID=TempID+6*keypad_dev.dispagecur;  //得到当前的汉字索引
            if(TempID<=keypad_dev.hznum)
            {
                keypad_dev.cur_hz=keypad_dev.butname[(KeyID-12)-1]; //获得选中的汉字
                if(keypad_dev.cur_hz != NULL)
                {
                    MULTIEDIT_AddText(hMulti,(const char*)keypad_dev.cur_hz);        //在多行文本中显示选中的汉字
                }
                keypad_dev.result_num=0;
                keypad_dev.dispagecur=0;
                keypad_dev.dispagenum=0;
                keypad_dev.cur_index=0;
                keypad_dev.pynowpage=0;
                keypad_dev.pypagenum=0;
                keypad_dev.inputlen=0;
                for(i=0;i<7;i++)    keypad_dev.inputstr[i]=0;  //输入清零
            }

        }else if((KeyID > 20) && (KeyID <24))       //竖排
        {
            TempID=KeyID-20;
            TempID=TempID+3*keypad_dev.pynowpage; //得到当前的拼音索引
            if(TempID<=keypad_dev.result_num)
            {
                keypad_dev.cur_index=TempID;
                keypad_dev.dispagecur=0;
                keypad_dev.dispagenum=0;
            }
        }
        else
        {
            if((c == ';')||(c == ' '))
            {
                GUI_StoreKeyMsg(c, 1);				    //把消息存进键盘缓冲器，按下状态
                GUI_StoreKeyMsg(c, 0);				    //把消息存进键盘缓冲器，松开状态
            }else
            {
                keypad_dev.inputstr[keypad_dev.inputlen]=c+'\0';   //输入字符
                if(keypad_dev.inputlen<6)keypad_dev.inputlen++;
            }

        }
/// TODO (zshare#1#): ///NULL -- '\0'

        if((keypad_dev.inputstr[0])!='\0')
        {
            key=t9.getpymb(keypad_dev.inputstr);        //得到匹配的结果数
            if(key)                                     //有部分匹配/完全匹配的结果
            {
                keypad_dev.result_num=key&0X7F;         //总匹配结果
                if((keypad_dev.sta&0X80)==0)
                {
                    keypad_dev.sta|=0X80;
                    keypad_dev.cur_index=1;             //默认第一个索引
                }
                if(key&0x80)                            //部分匹配
                {
                    keypad_dev.inputlen = key&0x7f;     //有效匹配位数
                    keypad_dev.inputstr[keypad_dev.inputlen]='\0';  //不匹配的位数去掉
                    if(keypad_dev.inputlen>1)keypad_dev.result_num=t9.getpymb(keypad_dev.inputstr);//重新获取完全匹配字符数
                }
            }else
            {
                keypad_dev.inputlen--;
                keypad_dev.inputstr[keypad_dev.inputlen]='\0';
            }
        }else
        {
            keypad_dev.sta&=0x7f;
            keypad_dev.cur_index=0;             //清零
            keypad_dev.result_num=0;
            keypad_dev.dispagecur=0;
            keypad_dev.dispagenum=0;
            keypad_dev.pynowpage=0;
            keypad_dev.pypagenum=0;
        }
        if(keypad_dev.result_num&0x80) keypad_dev.result_num=1;
		Msg.MsgId=CHANGE_CHINESE_PY;
		Msg.hWinSrc=pMsg->hWin;
		WM_SendMessage(keypad_dev.hKeypad,&Msg);
	}
}


//设置BUTTON要使用的皮肤
static int _DrawSkinFlex_BUTTON(const WIDGET_ITEM_DRAW_INFO * pDrawItemInfo)
{
	static GUI_MEMDEV_Handle ahMemEdges[4];
	GUI_MEMDEV_Handle        hMem;
	WM_HWIN                  hWin;
	unsigned                 i;
	int                      xPos;
	int                      yPos;
	int                      xSize;
	int                      ySize;
	int                      x;
	int                      y;
	int                      Id;
	int                      IsPressed;
    int                      butheigh;
	void (* pfDraw)(WM_HWIN hWin);

	x = 0;
	y = 0;
	switch (pDrawItemInfo->Cmd)
	{
		case WIDGET_ITEM_CREATE:
			if (ahMemEdges[0] == 0)
			{
				//创建一个小的帮助窗口，但是不可见
				hWin = WM_CreateWindowAsChild(0, 0, 16, 16, WM_HBKWIN, WM_CF_SHOW | WM_CF_STAYONTOP, NULL, 0);
				WM_SelectWindow(hWin);
				//为button的每个边缘创建存储设备
				for (i = 0; i < GUI_COUNTOF(ahMemEdges); i++)
				{
					switch (i)
					{
						case 0:
							x = 0;
							y = 0;
							break;
						case 1:
							x = -16;
							y = 0;
							break;
						case 2:
							x = -16;
							y = -16;
							break;
						case 3:
							x = 0;
							y = -16;
							break;
					}
					//创建放大设备
					hMem = GUI_MEMDEV_CreateFixed(0, 0, 16, 16, GUI_MEMDEV_NOTRANS, GUI_MEMDEV_APILIST_32, GUI_COLOR_CONV_8888);
					GUI_MEMDEV_Select(hMem);
					GUI_SetBkColor(GUI_TRANSPARENT);
					GUI_Clear();
					GUI_SetColor(COLOR_BORDER);
					GUI_DrawRoundedFrame(x, y, x + 31, y + 31, 16, 4);	//绘制换肤后的按钮四角的圆弧

					//收缩到指定的大小
					ahMemEdges[i] = GUI_MEMDEV_CreateFixed(0, 0, 4, 4, GUI_MEMDEV_NOTRANS, GUI_MEMDEV_APILIST_32, GUI_COLOR_CONV_8888);
					GUI_MEMDEV_Select(ahMemEdges[i]);
					GUI_SetBkColor(GUI_TRANSPARENT);
					GUI_Clear();
					GUI_MEMDEV_RotateHQ(hMem, ahMemEdges[i], -6, -6, 0, 250);
					GUI_MEMDEV_Delete(hMem);
					GUI_MEMDEV_Select(0);
				}
				WM_DeleteWindow(hWin);
			}
			break;
		case WIDGET_ITEM_DRAW_TEXT:
			//在按钮上显示自定义或者默认的颜色或者文本
			hWin = pDrawItemInfo->hWin;
			Id   = WM_GetId(hWin);
			i    = Id - ID_BUTTON;
			if(keypad_dev.padtype==ENGLISH_KEYPAD) pfDraw = _aEngButtonData[i].pfDraw;
			else if(keypad_dev.padtype==NUMBER_KEYPAD) pfDraw = _aNumButtonData[i].pfDraw;
			else if(keypad_dev.padtype==SIGN_KEYPAD) pfDraw = _aSinButtonData[keypad_dev.signpad_flag][i].pfDraw;
            else if(keypad_dev.padtype==CHINESE_KEYPAD) pfDraw = _aChineseButtonData1[i].pfDraw;

			if (pfDraw) pfDraw(hWin);
			else BUTTON_DrawSkinFlex(pDrawItemInfo);
			break;
		case WIDGET_ITEM_DRAW_BACKGROUND:
			//绘制按钮的背景
			IsPressed = BUTTON_IsPressed(pDrawItemInfo->hWin); 	//判断是否按钮是否被按下
			xPos      = WM_GetWindowOrgX(pDrawItemInfo->hWin);	//获取按钮小工具的X方向原点位置
			yPos      = WM_GetWindowOrgY(pDrawItemInfo->hWin);	//获取按钮小工具的Y方向原点位置
			xSize     = WM_GetWindowSizeX(pDrawItemInfo->hWin);	//获取按钮小工具的X大小
			ySize     = WM_GetWindowSizeY(pDrawItemInfo->hWin);	//获取按钮小工具的Y大小
			if (IsPressed) 										//如果按钮被按下
			{
				GUI_DrawGradientRoundedV(0, 0, xSize - 1, ySize - 1, 4, BUTTON_COLOR2, BUTTON_COLOR3);//绘制垂直颜色梯度填充的圆角矩形
			}
			else
			{
				GUI_DrawGradientRoundedV(0, 0, xSize - 1, ySize - 1, 4, BUTTON_COLOR0, BUTTON_COLOR1);//绘制垂直颜色梯度填充的圆角矩形
			}
			GUI_SetColor(COLOR_BORDER);
			//绘制按钮的四条边界
			GUI_DrawHLine(        0, 4, xSize - 5);
			GUI_DrawHLine(ySize - 1, 4, xSize - 5);
			GUI_DrawVLine(        0, 4, ySize - 5);
			GUI_DrawVLine(xSize - 1, 4, ySize - 5);
			//将给定存储设备的内容写入当前选定存储设备的制定位置
            if(keypad_dev.padtype==CHINESE_KEYPAD) butheigh = 77;
            else butheigh = 68;
			GUI_MEMDEV_WriteAt(ahMemEdges[0], xPos +  0, yPos +  0);			    //按钮左上角圆弧
			GUI_MEMDEV_WriteAt(ahMemEdges[1], xPos + xSize - 4, yPos +  0);		    //按钮右上角圆弧
			GUI_MEMDEV_WriteAt(ahMemEdges[2], xPos + xSize - 4, yPos + butheigh);   //按钮右下角圆弧
			GUI_MEMDEV_WriteAt(ahMemEdges[3], xPos +  0, yPos + butheigh);			//按钮左下角圆弧
			break;
		default:
			return BUTTON_DrawSkinFlex(pDrawItemInfo);//其他命令就使用默认的皮肤
	}
	return 0;
}

//设置中文键盘要使用的皮肤
static int _DrawChineseSkin_BUTTON(const WIDGET_ITEM_DRAW_INFO * pDrawItemInfo)
{
	WM_HWIN                  hWin;
	unsigned                 i;
	int                      xSize;
	int                      ySize;
	int                      Id;
	int                      IsPressed;
	void (* pfDraw)(WM_HWIN hWin);

	switch (pDrawItemInfo->Cmd)
	{
		case WIDGET_ITEM_DRAW_TEXT:
			//在按钮上显示自定义或者默认的颜色或者文本
			hWin = pDrawItemInfo->hWin;
			Id   = WM_GetId(hWin);
			i    = Id - ID_BUTTON;
            pfDraw = _aChineseButtonData1[i].pfDraw;

			if (pfDraw) pfDraw(hWin);
			else BUTTON_DrawSkinFlex(pDrawItemInfo);
			break;
		case WIDGET_ITEM_DRAW_BACKGROUND:
			//绘制按钮的背景
			IsPressed = BUTTON_IsPressed(pDrawItemInfo->hWin); 	//判断是否按钮是否被按下
			xSize     = WM_GetWindowSizeX(pDrawItemInfo->hWin);	//获取按钮小工具的X大小
			ySize     = WM_GetWindowSizeY(pDrawItemInfo->hWin);	//获取按钮小工具的Y大小
			if (IsPressed) 										//如果按钮被按下
			{
                GUI_DrawGradientV(0,0,xSize-1,ySize-1,BUTTON_COLOR2,BUTTON_COLOR3);
			}
			else
			{
                GUI_DrawGradientV(0,0,xSize-1,ySize-1,GUI_WHITE,GUI_WHITE);
			}
			GUI_SetColor(COLOR_BORDER);
			//绘制按钮的四条边界
			GUI_DrawHLine(        0, 0, xSize - 1);
			GUI_DrawHLine(ySize - 1, 0, xSize - 1);
			GUI_DrawVLine(        0, 0, ySize - 1);
			GUI_DrawVLine(xSize - 1, 0, ySize - 1);
			break;
		default:
			return BUTTON_DrawSkinFlex(pDrawItemInfo);//其他命令就使用默认的皮肤
	}
	return 0;
}

//更新横向的6个按钮
void keypad_hbtn_updc(u8* str)
{
    vu8 i=0;
    if((keypad_dev.dispagecur>=keypad_dev.dispagenum)&&keypad_dev.dispagenum)return;  //当前页等于或者大于总页数
    for(i=0;i<6;i++) keypad_dev.butname[i]=NULL;
    i=0;
    for(i=0;i<6;i++)
    {
        if((i+6*(keypad_dev.dispagecur))>(keypad_dev.hznum-1)) //超出相应拼音的汉字字数
        {
            keypad_dev.disbuf[i][0]=' ';
            keypad_dev.disbuf[i][1]=' ';
            keypad_dev.disbuf[i][2]='\0';
            keypad_dev.butname[i]=keypad_dev.disbuf[i];
        }else
        {
            keypad_dev.disbuf[i][0]=*str++;
            keypad_dev.disbuf[i][1]=*str++;
            keypad_dev.disbuf[i][2]='\0';
            keypad_dev.butname[i]=keypad_dev.disbuf[i];
        }
    }
}

//绘制横向的6个按钮
void keypad_draw_hbtn(WM_HWIN *handle)
{
    u8 i;
    for(i=0;i<6;i++)
    {
        BUTTON_SetFont(handle[13+i],&SIF24_Font);
        BUTTON_SetText(handle[13+i],(char*)keypad_dev.butname[i]);
    }

}
//keypad窗口调用的回调函数
static void _cbKeyPad(WM_MESSAGE * pMsg)
{
	WM_HWIN    hWin;
	int        Id;
	int        NCode;
	int        xSize;
	int        ySize;
    u16        slen;
	unsigned   i;

	hWin = pMsg->hWin;
	switch (pMsg->MsgId)
	{
		case WM_CREATE:				//此消息在窗口创建后立即发送，是窗口可以初始化并创建任何子窗口
			drawenglish_pad(hWin);	//创建英文键盘
			break;
		case WM_PAINT:
			//绘制背景
			xSize = WM_GetWindowSizeX(hWin);
			ySize = WM_GetWindowSizeY(hWin);
			GUI_SetColor(COLOR_BORDER);
			GUI_DrawRect(0, 0, xSize - 1, ySize - 1);
			GUI_DrawGradientV(1, 1, xSize - 2, ySize - 2, COLOR_KEYPAD0, COLOR_KEYPAD1);
			break;
		case WM_NOTIFY_PARENT:	//告诉父窗口，子窗口发生了改变
			//想当前焦点窗口发送按键消息
			Id    = WM_GetId(pMsg->hWinSrc);
			NCode = pMsg->Data.v;
			switch (NCode)
			{
			case WM_NOTIFICATION_RELEASED://WM_NOTIFICATION_CLICKED:				//按钮被释放
					if(keypad_dev.padtype==ENGLISH_KEYPAD)	//英文键盘
					{
						engkeypad_process(_aEngButtonData,Id,pMsg);		//处理英文键盘
					}
					else if(keypad_dev.padtype==NUMBER_KEYPAD)			//数字键盘
					{
						numkeypad_process(_aNumButtonData,Id,pMsg);		//处理数字键盘
					}
					else if(keypad_dev.padtype==SIGN_KEYPAD)
					{
						signkeypad_process(_aSinButtonData[keypad_dev.signpad_flag],Id,pMsg);
					}
                    else if(keypad_dev.padtype==CHINESE_KEYPAD)
                    {
                        chinesekeypad_process(_aChineseButtonData1,Id,pMsg);
                    }
					break;
			}
			break;
		case CHANGE_ENGLISHPADTO_NUMPAD:			//从英文键盘切换到数字键盘
			for(i=0;i<GUI_COUNTOF(_aEngButtonData);i++) WM_DeleteWindow(keypad_dev.EngButton_Handle[i]);
            for(i=0;i<GUI_COUNTOF(_aEngButtonData);i++) keypad_dev.EngButton_Handle[i]=0;
			drawnumber_pad(hWin);
			break;
		case CHANGE_NUMPADTO_ENGLISHPAD:			//从数字键盘切换到英文键盘
			for(i=0;i<GUI_COUNTOF(_aNumButtonData);i++) WM_DeleteWindow(keypad_dev.NumButton_Handle[i]);
            for(i=0;i<GUI_COUNTOF(_aEngButtonData);i++) keypad_dev.NumButton_Handle[i]=0;
			drawenglish_pad(hWin);
			break;
		case CHANGE_ENGLISHPADTO_SIGNPAD1:			//从英文键盘切换到符号键盘
			for(i=0;i<GUI_COUNTOF(_aEngButtonData);i++) WM_DeleteWindow(keypad_dev.EngButton_Handle[i]);
            for(i=0;i<GUI_COUNTOF(_aEngButtonData);i++) keypad_dev.SignButton_Handle[i]=0;
		    drawsign_pad(hWin);
			break;
		case CHANGE_SIGNPADTO_ENGLISHPAD:			//从符号键盘切换到英文键盘
			for(i=0;i<GUI_COUNTOF(_aSinButtonData[keypad_dev.signpad_flag]);i++) WM_DeleteWindow(keypad_dev.SignButton_Handle[i]);
            for(i=0;i<GUI_COUNTOF(_aSinButtonData[keypad_dev.signpad_flag]);i++) keypad_dev.SignButton_Handle[i]=0;
			drawenglish_pad(hWin);
			break;
		case CHANGE_SIGNPAD:						//切换符号键盘
			for(i=0;i<GUI_COUNTOF(_aSinButtonData[keypad_dev.signpad_flag]);i++) WM_DeleteWindow(keypad_dev.SignButton_Handle[i]);
            for(i=0;i<GUI_COUNTOF(_aSinButtonData[keypad_dev.signpad_flag]);i++) keypad_dev.SignButton_Handle[i]=0;
		    drawsign_pad(hWin);
			break;
        case CHANGE_ENGLISHPADTO_CHINESEPAD:        //从英文键盘切换到中文键盘
            for(i=0;i<GUI_COUNTOF(_aEngButtonData);i++) WM_DeleteWindow(keypad_dev.EngButton_Handle[i]);
            for(i=0;i<GUI_COUNTOF(_aEngButtonData);i++) keypad_dev.EngButton_Handle[i]=0;
            drawchinese1_pad(hWin);
            drawchinese2_pad(hWin);
            break;
        case CHANGE_CHINESEPADTO_ENGLISHPAD:        //从中文键盘切换到英文键盘
            for(i=0;i<25;i++) WM_DeleteWindow(keypad_dev.ChineseButton_Handle[i]);
            for(i=0;i<25;i++) keypad_dev.ChineseButton_Handle[i]=0;
            memset(keypad_dev.inputstr,0,7);    //清零
            keypad_dev.cur_index=0;
            keypad_dev.inputlen=0;
            keypad_dev.result_num=0;
            keypad_dev.pypagenum=0;
            keypad_dev.pynowpage=0;
            keypad_dev.dispagecur=0;
            keypad_dev.dispagenum=0;
            keypad_dev.sta=0;
            drawenglish_pad(hWin);
            break;
        case CHANGE_CHINESE_PY:                                                             //显示九宫格输入的拼音选项
            if(keypad_dev.cur_index>0)                                                      //keypad_dev.curindex>0说明有拼音选中
            {
                keypad_dev.disstr=t9.pymb[keypad_dev.cur_index-1]->pymb;                    //获得当前码表
                slen=strlen((const char*)keypad_dev.disstr);                                //得到当前选中的拼音对应的汉字字符串的长度
                keypad_dev.hznum=slen/2;                                                    //因为是中文，每个字视为2个字符。
            }else                                                                           //没有拼音选中
            {
                keypad_dev.disstr=NULL;
                slen=0;
                keypad_dev.hznum=0;
            }
            keypad_dev.dispagenum=keypad_dev.hznum/6+(keypad_dev.hznum%6?1:0);              //得到汉字字符串的总页数
            keypad_dev.pypagenum=keypad_dev.result_num/3+(keypad_dev.result_num%3?1:0);     //得到拼音页数
/// TODO (zshare#1#): ///NULL -- '\0'

            if(keypad_dev.inputstr[0]!='\0')
            {
                for(i=0;i<3;i++)
                {
                    BUTTON_SetFont(keypad_dev.ChineseButton_Handle[21+i],&SIF24_Font);
                    if((i+3*(keypad_dev.pynowpage))>(keypad_dev.result_num-1))
                    {
                        BUTTON_SetText(keypad_dev.ChineseButton_Handle[21+i]," ");
                    }else
                    {
                        BUTTON_SetText(keypad_dev.ChineseButton_Handle[21+i],(const char*)t9.pymb[i+3*(keypad_dev.pynowpage)]->py);
                    }
                }

                keypad_hbtn_updc(keypad_dev.disstr+(keypad_dev.dispagecur*6*2));        //更新横向按钮名字
                keypad_draw_hbtn(keypad_dev.ChineseButton_Handle);                      //绘制横向按钮

            }else                                                                       //没有输入任何拼音
            {
                for(i=0;i<3;i++)
                {
                    BUTTON_SetFont(keypad_dev.ChineseButton_Handle[21+i],&SIF24_Font);
                    BUTTON_SetText(keypad_dev.ChineseButton_Handle[21+i]," ");
                }

                for(i=0;i<6;i++)
                {
                    keypad_dev.disbuf[i][0]=' ';
                    keypad_dev.disbuf[i][1]=' ';
                    keypad_dev.disbuf[i][2]='\0';
                    keypad_dev.butname[i]=keypad_dev.disbuf[i];
                }
                keypad_draw_hbtn(keypad_dev.ChineseButton_Handle);                  //绘制横向按钮
            }
            break;
	}
}


//背景窗口回调函数
static void _cbBk(WM_MESSAGE * pMsg)
{
	switch (pMsg->MsgId)
	{
		case WM_PAINT:
			GUI_DrawGradientV(0, 0,lcd_width,lcd_height, COLOR_BACK0, COLOR_BACK1);
			break;
	}
}

/** @brief
 *对输入的值检测合法性，合法进行设置动作，保存;非法返回
 * @param
 * @param
 * @return
 *  0：数据合法，并且保存成功;1：数据非法;2：保存不成功;
 */

static uint8_t Value_Check()
{
    uint8_t result_input[0x100];
    uint8_t tmpU8;
    uint16_t tmpU16;
    uint32_t tmpU32;
    double tmpDouble;
    uint16_t i = 0;
    CON_t *pCon;

    pCon = CONGetHandle(0);

    memset(result_input,'\0',sizeof(result_input));
    MULTIEDIT_GetText(hMulti,result_input,MULTIEDIT_GetTextSize(hMulti));

    switch(ManagerSetOptions)
    {
    case LOGIN_PASSWD:
        if(strlen(result_input) == 0)
        {
            BUTTON_SetTextColor(_aahButtonOk, BUTTON_CI_UNPRESSED, GUI_BLACK);
            BUTTON_SetText(_aahButtonOk, "确定");
        }
        else if(strcmp(result_input,passwd) == 0)
        {
            return VALUE_OK_SAV;
        }
        else
        {
            BUTTON_SetTextColor(_aahButtonOk, BUTTON_CI_UNPRESSED, GUI_RED);
            BUTTON_SetText(_aahButtonOk, "密码错误");
            for(i = 0;i < MULTIEDIT_GetTextSize(hMulti);i++)
            {
                GUI_StoreKeyMsg('\b', 1);
                GUI_StoreKeyMsg('\b', 0);
            }
            return VALUE_ERROR;
        }
        break;
    case SYSSET_VALUE:
        switch(htmpID)
        {
            case 20:
                pEVSE->info.SetEVSECfg(pEVSE, jnEVSESN, result_input, ParamTypeString);
                memset(pEVSE->info.strSN,'\0',strlen(pEVSE->info.strSN));
                strcpy(pEVSE->info.strSN,result_input);
                WM_SendMessageNoPara(htmpChild, MSG_MANAGERSETID0);
                break;
            case 21:
                pEVSE->info.SetEVSECfg(pEVSE, jnEVSEID, result_input, ParamTypeString);
                memset(pEVSE->info.strID,'\0',strlen(pEVSE->info.strID));
                strcpy(pEVSE->info.strID,result_input);
                WM_SendMessageNoPara(htmpChild, MSG_MANAGERSETID1);
                break;
            case 22:
                pechProto->info.SetProtoCfg(jnProtoServerIP, ParamTypeString, NULL, 0, result_input);
                memset(pechProto->info.strServerIP,'\0',strlen(pechProto->info.strServerIP));
                strcpy(pechProto->info.strServerIP,result_input);
                WM_SendMessageNoPara(htmpChild, MSG_MANAGERSETID2);
                break;
            case 23://
                tmpU16 = (uint16_t)atoi(result_input);
                pechProto->info.SetProtoCfg(jnProtoServerPort, ParamTypeU16, NULL, 0, &tmpU16);
                pechProto->info.usServerPort = tmpU16;
                WM_SendMessageNoPara(htmpChild, MSG_MANAGERSETID3);
                break;
            case 24://user name
                pechProto->info.SetProtoCfg(jnProtoUserName, ParamTypeString, NULL, 0, result_input);
                memset(pechProto->info.strUserName,'\0',strlen(pechProto->info.strUserName));
                strcpy(pechProto->info.strUserName,result_input);
                WM_SendMessageNoPara(htmpChild, MSG_MANAGERSETID4);
                break;
            case 25://user passwd
                pechProto->info.SetProtoCfg(jnProtoUserPwd, ParamTypeString ,NULL ,0,result_input);
                memset(pechProto->info.strUserPwd,'\0',strlen(pechProto->info.strUserPwd));
                strcpy(pechProto->info.strUserPwd,result_input);
                WM_SendMessageNoPara(htmpChild, MSG_MANAGERSETID5);
                break;
            case 26://屏保时间
                tmpU32 = atoi(result_input);
                xSysconf.SetSysCfg(jnSysDispSleepTime, (void *)&tmpU32, ParamTypeU32);
                xSysconf.ulDispSleepTime_s = tmpU32;
                WM_SendMessageNoPara(htmpChild, MSG_MANAGERSETID6);
                break;
        }
        break;
    case CONSET_VALUE:
        switch(htmpID)
        {
            case 20:
                pCon->info.SetCONCfg(pCon, jnQRCode, result_input, ParamTypeString);
                memset(pCon->info.strQRCode,'\0',sizeof(pCon->info.strQRCode));
                strcpy(pCon->info.strQRCode,result_input);
                WM_SendMessageNoPara(htmpChild, MSG_MANAGERSETID0);
                break;
            case 21:
                pCon->info.SetCONCfg(pCon, jnSocketType, result_input, ParamTypeString);
                memset(pCon->info.ucSocketType,'\0',sizeof(pCon->info.ucSocketType));
                strcpy(pCon->info.ucSocketType,result_input);
                WM_SendMessageNoPara(htmpChild, MSG_MANAGERSETID1);
                break;
            case 22:
                tmpDouble = atof(result_input);
                pCon->info.dVolatageUpperLimits = tmpDouble;
                pCon->info.SetCONCfg(pCon, jnVolatageUpperLimits, &tmpDouble, ParamTypeDouble);
                WM_SendMessageNoPara(htmpChild, MSG_MANAGERSETID2);
                break;
            case 23:
                tmpDouble = atof(result_input);
                pCon->info.dVolatageLowerLimits = tmpDouble;
                pCon->info.SetCONCfg(pCon, jnVolatageLowerLimits, &tmpDouble, ParamTypeDouble);
                WM_SendMessageNoPara(htmpChild, MSG_MANAGERSETID3);
                break;
            case 24://电流上限
                //tmpDouble = atof(result_input);
                //pCon->info.SetCONCfg(pCon, jnVolatageLowerLimits, &tmpDouble, ParamTypeDouble);
                WM_SendMessageNoPara(htmpChild, MSG_MANAGERSETID4);
                break;
            case 25://交流输入端子温度上限
                tmpDouble = atof(result_input);
                pCon->info.dACTempUpperLimits = tmpDouble;
                pCon->info.SetCONCfg(pCon, jnACTempUpperLimits, &tmpDouble, ParamTypeDouble);
                WM_SendMessageNoPara(htmpChild, MSG_MANAGERSETID5);
                break;
            case 26:
                tmpDouble = atof(result_input);
                pCon->info.dACTempLowerLimits = tmpDouble;
                pCon->info.SetCONCfg(pCon, jnACTempLowerLimits, &tmpDouble, ParamTypeDouble);
                WM_SendMessageNoPara(htmpChild, MSG_MANAGERSETID6);
                break;
            case 27://交流输出端子温度上限
                tmpDouble = atof(result_input);
                pCon->info.dSocketTempUpperLimits = tmpDouble;
                pCon->info.SetCONCfg(pCon, jnSocketTempUpperLimits, &tmpDouble, ParamTypeDouble);
                WM_SendMessageNoPara(htmpChild, MSG_MANAGERSETID7);
                break;
            case 28:
                tmpDouble = atof(result_input);
                pCon->info.dSocketTempLowerLimits = tmpDouble;
                pCon->info.SetCONCfg(pCon, jnSocketTempLowerLimits, &tmpDouble, ParamTypeDouble);
                WM_SendMessageNoPara(htmpChild, MSG_MANAGERSETID8);
                break;
            case 29:
                tmpDouble = atof(result_input);
                pCon->info.dRatedCurrent = tmpDouble;
                pCon->info.SetCONCfg(pCon, jnRatedCurrent, &tmpDouble, ParamTypeDouble);
                WM_SendMessageNoPara(htmpChild, MSG_MANAGERSETID9);
                break;
            case 30:
                tmpDouble = atof(result_input);
                pCon->info.dRatedPower = tmpDouble;
                pCon->info.SetCONCfg(pCon, jnRatedPower, &tmpDouble, ParamTypeDouble);
                WM_SendMessageNoPara(htmpChild, MSG_MANAGERSETIDA);
                break;
        }
        break;
    }
}

static void Jump_Screen(WM_HWIN hWin,uint8_t IS_jump)
{
    uint8_t i = 0;

    for (i = 0; i < GUI_COUNTOF(_aEngButtonData); i++)
	{
		WM_DeleteWindow(keypad_dev.EngButton_Handle[i]);
        keypad_dev.EngButton_Handle[i] = 0;
	}
    for (i = 0; i < GUI_COUNTOF(_aNumButtonData); i++)
	{
		WM_DeleteWindow(keypad_dev.NumButton_Handle[i]);
		keypad_dev.NumButton_Handle[i] = 0;
	}
    for (i = 0; i < GUI_COUNTOF(_aSinButtonData); i++)
	{
		WM_DeleteWindow(keypad_dev.SignButton_Handle[i]);
		keypad_dev.SignButton_Handle[i] = 0;
	}
    for (i = 0; i < GUI_COUNTOF(_aChineseButtonData1); i++)
	{
		WM_DeleteWindow(keypad_dev.ChineseButton_Handle[i]);
		keypad_dev.ChineseButton_Handle[i] = 0;
	}
    for (i = 0; i < GUI_COUNTOF(_aChineseButtonData2); i++)
	{
		WM_DeleteWindow(keypad_dev.ChineseButton_Handle[i+12]);
		keypad_dev.ChineseButton_Handle[i+12] = 0;
	}

    WM_DeleteWindow(hWin);
    WM_DeleteWindow(keypad_dev.hKeypad);
    WM_DeleteWindow(hMulti);
    //跳页操作
    switch(ManagerSetOptions)
    {
    case LOGIN_PASSWD:
        bitclr(winCreateFlag,2);
        (IS_jump == 0) ? (CreateManagerCommon()):(CreateHomePage());
    break;
    /**< 添加跳页到设置页 , */
    case SYSSET_VALUE:
        bitclr(winCreateFlag, 2);
        WM_ShowWindow(htmpBK);
        WM_ShowWindow(htmpChild);
    break;
    /**< 添加跳页到设置页 , */
    case CONSET_VALUE:
        bitclr(winCreateFlag, 2);
        WM_ShowWindow(htmpBK);
        WM_ShowWindow(htmpChild);
    break;
    default:
    break;
    }
}

/** @brief
 *frame回调函数，标示ok，cancle的动作
 * @param
 * @param
 * @return
 *
 */

static void _cbFrame(WM_MESSAGE * pMsg)
{
    WM_HWIN      hItem;
    int          NCode;
    int          Id;

    switch (pMsg->MsgId)
    {
    case WM_INIT_DIALOG:
        break;
    case WM_NOTIFY_PARENT:
        Id    = WM_GetId(pMsg->hWinSrc);
        NCode = pMsg->Data.v;
        switch(Id)
        {
        case GUI_ID_BUTTON0://确定
            switch(NCode)
            {
            case WM_NOTIFICATION_CLICKED:
                /**< 进入密码、设置值操作 */
                if(Value_Check() == VALUE_OK_SAV)
                {
                    //跳页操作
                    Jump_Screen(pMsg->hWin,0);
                }
                break;
            }
            break;
        case GUI_ID_BUTTON1://取消
            switch(NCode)
            {
            case WM_NOTIFICATION_CLICKED:
                Jump_Screen(pMsg->hWin,1);
                break;
            }
            break;
        }
        break;
    default:
        WM_DefaultProc(pMsg);
        break;
    }
}

void Keypad_GetValue(uint8_t optios,char *varname)
{
	WM_HWIN hFrame;

    ManagerSetOptions = optios;
	WM_SetCallback(WM_HBKWIN, _cbBk);		        //是指背景窗口回调函数
	keypad_dev.xpos=10;
	keypad_dev.ypos=150;
	keypad_dev.width=780;
	keypad_dev.height=320;
	keypad_dev.padtype=ENGLISH_KEYPAD;				//默认为英文键盘
	keypad_dev.signpad_flag=0;
	keypad_dev.signpad_num=2;
    keypad_dev.inputlen=0;
    keypad_dev.pynowpage=0;
    keypad_dev.cur_index=0;
    keypad_dev.sta=0;

	//设置keypad所使用的按钮的皮肤
	BUTTON_SetDefaultSkin(_DrawSkinFlex_BUTTON); 	//设置BUTTON的皮肤
	//创建keypad
	keypad_dev.hKeypad = WM_CreateWindowAsChild(keypad_dev.xpos,keypad_dev.ypos,keypad_dev.width,keypad_dev.height, WM_HBKWIN, WM_CF_SHOW | WM_CF_STAYONTOP, _cbKeyPad, 0);

	//设置notepad属性
	BUTTON_SetDefaultSkin(BUTTON_SKIN_FLEX);
	FRAMEWIN_SetDefaultSkin(FRAMEWIN_SKIN_FLEX);
	FRAMEWIN_SetDefaultTextAlign(GUI_TA_HCENTER);

	//创建FRAME窗口
	hFrame = FRAMEWIN_CreateEx(0, 0, 800, 480, WM_HBKWIN, WM_CF_SHOW, 0, 0, "北京动力源科技股份有限公司", _cbFrame);
	FRAMEWIN_SetTextColor(hFrame, GUI_RED);
	FRAMEWIN_SetFont(hFrame, &SIF24_Font);
	FRAMEWIN_SetClientColor(hFrame, GUI_WHITE);

	//创建一个multi edit(多行文本小工具)小工具
	hMulti = MULTIEDIT_CreateEx(170, 10, 400, 100, WM_GetClientWindow(hFrame), WM_CF_SHOW, 0, GUI_ID_MULTIEDIT0, 100, NULL);
	MULTIEDIT_EnableBlink(hMulti,500,1);//开启光标,周期500ms
    MULTIEDIT_SetInsertMode(hMulti,1);  //开启插入模式
	MULTIEDIT_SetFont(hMulti, &SIF24_Font);
	WM_SetFocus(hMulti);

    _aahButtonOk = BUTTON_CreateEx(600,5,100,50,WM_GetClientWindow(hFrame),WM_CF_SHOW,0,GUI_ID_BUTTON0);
    BUTTON_SetFont(_aahButtonOk, &SIF24_Font);
    BUTTON_SetTextAlign(_aahButtonOk,GUI_TA_HCENTER | GUI_TA_VCENTER);
    BUTTON_SetBkColor(_aahButtonOk, BUTTON_CI_UNPRESSED, GUI_GRAY);
    BUTTON_SetTextColor(_aahButtonOk, BUTTON_CI_UNPRESSED, GUI_BLACK);
    BUTTON_SetText(_aahButtonOk, "确定");

    _aahButtonCancel = BUTTON_CreateEx(600,60,100,50,WM_GetClientWindow(hFrame),WM_CF_SHOW,0,GUI_ID_BUTTON1);
    BUTTON_SetFont(_aahButtonCancel, &SIF24_Font);
    BUTTON_SetTextAlign(_aahButtonCancel,GUI_TA_HCENTER | GUI_TA_VCENTER);
    BUTTON_SetBkColor(_aahButtonCancel, BUTTON_CI_UNPRESSED, GUI_GRAY);
    BUTTON_SetTextColor(_aahButtonCancel, BUTTON_CI_UNPRESSED, GUI_BLACK);
    BUTTON_SetText(_aahButtonCancel, "取消");

    /// TODO (zshare#1#): ///添加密码是否显示眼睛位图
    //    hButton = BUTTON_CreateEx(580,45,20,20,WM_GetClientWindow(hFrame),WM_CF_SHOW,0,GUI_ID_BUTTON1);
    //    BUTTON_SetBitmapEx(hButton, 0, &bm_bpp_off, 0, 0);

    switch(ManagerSetOptions)
    {
    case LOGIN_PASSWD:
        _aahEditVar = TEXT_CreateEx(30, 45, 140, 25,WM_GetClientWindow(hFrame),WM_CF_SHOW,0,13,"登录密码:");
        TEXT_SetFont(_aahEditVar, &SIF24_Font);
        TEXT_SetTextColor(_aahEditVar, GUI_BLACK);
        MULTIEDIT_SetPasswordMode(hMulti,1);//是否启用密码模式
    break;

    case SYSSET_VALUE:
        _aahEditVar = TEXT_CreateEx(30, 45, 140, 25,WM_GetClientWindow(hFrame),WM_CF_SHOW,0,13,"交流桩序列号");
        TEXT_SetFont(_aahEditVar, &SIF16_Font);
        TEXT_SetTextColor(_aahEditVar, GUI_BLACK);
        _aahEditEg = TEXT_CreateEx(15, 70, 160, 25,WM_GetClientWindow(hFrame),WM_CF_SHOW,0,13,"交流桩序列号");
        TEXT_SetFont(_aahEditEg, &SIF16_Font);
        TEXT_SetTextColor(_aahEditEg, GUI_BLACK);
        TEXT_SetText(_aahEditEg,"eg,1122334455667788");
//        MULTIEDIT_SetText(hMulti,"eg,1122334455667788");
    break;

    default:
    break;
    }
    bitset(winCreateFlag,2);
    /// TODO (zshare#1#): ///增加键盘值返回函数
    //MULTIEDIT_GetText(hMulti,传参数组,个数);
}
void Keypad_GetValueTest(uint8_t optios,uint8_t id,WM_HWIN hwin,WM_HWIN _hbkWin,uint8_t *name_p,uint8_t *eg_p)
{
	WM_HWIN hFrame;

    ManagerSetOptions = optios;
	WM_SetCallback(WM_HBKWIN, _cbBk);		        //是指背景窗口回调函数
    htmpBK = _hbkWin;
	htmpChild = hwin;
    htmpID = id;
	keypad_dev.xpos=10;
	keypad_dev.ypos=150;
	keypad_dev.width=780;
	keypad_dev.height=320;
	keypad_dev.padtype=ENGLISH_KEYPAD;				//默认为英文键盘
	keypad_dev.signpad_flag=0;
	keypad_dev.signpad_num=2;
    keypad_dev.inputlen=0;
    keypad_dev.pynowpage=0;
    keypad_dev.cur_index=0;
    keypad_dev.sta=0;

	//设置keypad所使用的按钮的皮肤
	BUTTON_SetDefaultSkin(_DrawSkinFlex_BUTTON); 	//设置BUTTON的皮肤
	//创建keypad
	keypad_dev.hKeypad = WM_CreateWindowAsChild(keypad_dev.xpos,keypad_dev.ypos,keypad_dev.width,keypad_dev.height, WM_HBKWIN, WM_CF_SHOW | WM_CF_STAYONTOP, _cbKeyPad, 0);

	//设置notepad属性
	BUTTON_SetDefaultSkin(BUTTON_SKIN_FLEX);
	FRAMEWIN_SetDefaultSkin(FRAMEWIN_SKIN_FLEX);
	FRAMEWIN_SetDefaultTextAlign(GUI_TA_HCENTER);

	//创建FRAME窗口
	hFrame = FRAMEWIN_CreateEx(0, 0, 800, 480, WM_HBKWIN, WM_CF_SHOW, 0, 0, "北京动力源科技股份有限公司", _cbFrame);
	FRAMEWIN_SetTextColor(hFrame, GUI_RED);
	FRAMEWIN_SetFont(hFrame, &SIF24_Font);
	FRAMEWIN_SetClientColor(hFrame, GUI_WHITE);

	//创建一个multi edit(多行文本小工具)小工具
	hMulti = MULTIEDIT_CreateEx(170, 10, 400, 100, WM_GetClientWindow(hFrame), WM_CF_SHOW, 0, GUI_ID_MULTIEDIT0, 100, NULL);
	MULTIEDIT_EnableBlink(hMulti,500,1);//开启光标,周期500ms
    MULTIEDIT_SetInsertMode(hMulti,1);  //开启插入模式
	MULTIEDIT_SetFont(hMulti, &SIF24_Font);
	WM_SetFocus(hMulti);

    _aahButtonOk = BUTTON_CreateEx(600,5,100,50,WM_GetClientWindow(hFrame),WM_CF_SHOW,0,GUI_ID_BUTTON0);
    BUTTON_SetFont(_aahButtonOk, &SIF24_Font);
    BUTTON_SetTextAlign(_aahButtonOk,GUI_TA_HCENTER | GUI_TA_VCENTER);
    BUTTON_SetBkColor(_aahButtonOk, BUTTON_CI_UNPRESSED, GUI_GRAY);
    BUTTON_SetTextColor(_aahButtonOk, BUTTON_CI_UNPRESSED, GUI_BLACK);
    BUTTON_SetText(_aahButtonOk, "确定");

    _aahButtonCancel = BUTTON_CreateEx(600,60,100,50,WM_GetClientWindow(hFrame),WM_CF_SHOW,0,GUI_ID_BUTTON1);
    BUTTON_SetFont(_aahButtonCancel, &SIF24_Font);
    BUTTON_SetTextAlign(_aahButtonCancel,GUI_TA_HCENTER | GUI_TA_VCENTER);
    BUTTON_SetBkColor(_aahButtonCancel, BUTTON_CI_UNPRESSED, GUI_GRAY);
    BUTTON_SetTextColor(_aahButtonCancel, BUTTON_CI_UNPRESSED, GUI_BLACK);
    BUTTON_SetText(_aahButtonCancel, "取消");

    /// TODO (zshare#1#): ///添加密码是否显示眼睛位图
    //    hButton = BUTTON_CreateEx(580,45,20,20,WM_GetClientWindow(hFrame),WM_CF_SHOW,0,GUI_ID_BUTTON1);
    //    BUTTON_SetBitmapEx(hButton, 0, &bm_bpp_off, 0, 0);

    switch(ManagerSetOptions)
    {
    case LOGIN_PASSWD:
        _aahEditVar = TEXT_CreateEx(30, 45, 140, 25,WM_GetClientWindow(hFrame),WM_CF_SHOW,0,13,"登录密码:");
        TEXT_SetFont(_aahEditVar, &SIF24_Font);
        TEXT_SetTextColor(_aahEditVar, GUI_BLACK);
        MULTIEDIT_SetPasswordMode(hMulti,1);//是否启用密码模式
    break;

    case SYSSET_VALUE:
        _aahEditVar = TEXT_CreateEx(30, 45, 140, 25,WM_GetClientWindow(hFrame),WM_CF_SHOW,0,13,name_p);
        TEXT_SetFont(_aahEditVar, &SIF16_Font);
        TEXT_SetTextColor(_aahEditVar, GUI_BLACK);
        _aahEditEg = TEXT_CreateEx(15, 70, 160, 25,WM_GetClientWindow(hFrame),WM_CF_SHOW,0,13,name_p);
        TEXT_SetFont(_aahEditEg, &SIF16_Font);
        TEXT_SetTextColor(_aahEditEg, GUI_BLACK);
        TEXT_SetText(_aahEditEg,eg_p);
//        MULTIEDIT_SetText(hMulti,"eg,1122334455667788");
    break;

    case CONSET_VALUE:
        _aahEditVar = TEXT_CreateEx(30, 45, 140, 25,WM_GetClientWindow(hFrame),WM_CF_SHOW,0,13,name_p);
        TEXT_SetFont(_aahEditVar, &SIF16_Font);
        TEXT_SetTextColor(_aahEditVar, GUI_BLACK);
        _aahEditEg = TEXT_CreateEx(15, 70, 160, 25,WM_GetClientWindow(hFrame),WM_CF_SHOW,0,13,name_p);
        TEXT_SetFont(_aahEditEg, &SIF16_Font);
        TEXT_SetTextColor(_aahEditEg, GUI_BLACK);
        TEXT_SetText(_aahEditEg,eg_p);
//        MULTIEDIT_SetText(hMulti,"eg,1122334455667788");
    break;

    default:
    break;
    }
    bitset(winCreateFlag,2);
}




