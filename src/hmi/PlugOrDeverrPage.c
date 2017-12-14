#include "DIALOG.h"

#define ID_WINDOW_0 (GUI_ID_USER + 0x00)
#define ID_IMAGE_0 (GUI_ID_USER + 0x01)
#define ID_IMAGE_1 (GUI_ID_USER + 0x02)
#define ID_TEXT_0 (GUI_ID_USER + 0x05)
#define ID_TEXT_1 (GUI_ID_USER + 0x06)
#define ID_IMAGE_2 (GUI_ID_USER + 0x07)
#define ID_IMAGE_3 (GUI_ID_USER + 0x08)

#define ID_IMAGE_0_IMAGE_0 0x00
#define ID_IMAGE_1_IMAGE_0 0x01
#define ID_IMAGE_2_IMAGE_0 0x02
#define ID_IMAGE_3_IMAGE_0 0x03


static const GUI_WIDGET_CREATE_INFO _aDialogCreate[] = {
  { WINDOW_CreateIndirect, "PlugOrDeverrPage", ID_WINDOW_0, 0, 0, 800, 480, 0, 0x0, 0 },
  { IMAGE_CreateIndirect, "POrDBackImage", ID_IMAGE_0, 0, 0, 800, 480, 0, 0, 0 },
  { IMAGE_CreateIndirect, "SignalImage", ID_IMAGE_1, 650, 5, 60, 30, 0, 0, 0 },
  { TEXT_CreateIndirect, "Date", ID_TEXT_0, 500, 70, 240, 24, 0, 0x0, 0 },
  { TEXT_CreateIndirect, "Time", ID_TEXT_1, 500, 120, 240, 24, 0, 0x0, 0 },
  { IMAGE_CreateIndirect, "POrDPicImage", ID_IMAGE_2, 160, 80, 300, 200, 0, 0, 0 },
  { IMAGE_CreateIndirect, "POrDPromptImage", ID_IMAGE_3, 160, 300, 300, 100, 0, 0, 0 },
};

static void _cbDialog(WM_MESSAGE * pMsg) {
  const void * pData;
  WM_HWIN      hItem;
  U32          FileSize;
  switch (pMsg->MsgId) {
  case WM_INIT_DIALOG:
    hItem = WM_GetDialogItem(pMsg->hWin, ID_IMAGE_0);
    IMAGE_SetBMP(hItem, pData, FileSize);
      
    hItem = WM_GetDialogItem(pMsg->hWin, ID_IMAGE_1);
    IMAGE_SetBMP(hItem, pData, FileSize);

    hItem = WM_GetDialogItem(pMsg->hWin, ID_IMAGE_2);
    IMAGE_SetBMP(hItem, pData, FileSize);

    hItem = WM_GetDialogItem(pMsg->hWin, ID_IMAGE_3);
    IMAGE_SetBMP(hItem, pData, FileSize);

    break;
  default:
    WM_DefaultProc(pMsg);
    break;
  }
}

/*
*  CreatePlugOrDeverrPage
*/
WM_HWIN CreatePlugOrDeverrPage(void);
WM_HWIN CreatePlugOrDeverrPage(void) {
  WM_HWIN hWin;

  hWin = GUI_CreateDialogBox(_aDialogCreate, GUI_COUNTOF(_aDialogCreate), _cbDialog, WM_HBKWIN, 0, 0);
  return hWin;
}


