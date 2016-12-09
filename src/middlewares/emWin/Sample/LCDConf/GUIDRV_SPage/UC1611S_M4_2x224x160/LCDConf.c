/*********************************************************************
*                SEGGER Microcontroller GmbH & Co. KG                *
*        Solutions for real time microcontroller applications        *
**********************************************************************
*                                                                    *
*        (c) 1996 - 2016  SEGGER Microcontroller GmbH & Co. KG       *
*                                                                    *
*        Internet: www.segger.com    Support:  support@segger.com    *
*                                                                    *
**********************************************************************

** emWin V5.36 - Graphical user interface for embedded applications **
All  Intellectual Property rights  in the Software belongs to  SEGGER.
emWin is protected by  international copyright laws.  Knowledge of the
source code may not be used to write a similar product.  This file may
only be used in accordance with the following terms:

The software has been licensed to  ARM LIMITED whose registered office
is situated at  110 Fulbourn Road,  Cambridge CB1 9NJ,  England solely
for  the  purposes  of  creating  libraries  for  ARM7, ARM9, Cortex-M
series,  and   Cortex-R4   processor-based  devices,  sublicensed  and
distributed as part of the  MDK-ARM  Professional  under the terms and
conditions  of  the   End  User  License  supplied  with  the  MDK-ARM
Professional. 
Full source code is available at: www.segger.com

We appreciate your understanding and fairness.
----------------------------------------------------------------------
Licensing information

Licensor:                 SEGGER Software GmbH
Licensed to:              ARM Ltd, 110 Fulbourn Road, CB1 9NJ Cambridge, UK
Licensed SEGGER software: emWin
License number:           GUI-00181
License model:            LES-SLA-20007, Agreement, effective since October 1st 2011 
Licensed product:         MDK-ARM Professional
Licensed platform:        ARM7/9, Cortex-M/R4
Licensed number of seats: -
----------------------------------------------------------------------
File        : LCDConf.c
Purpose     : Display controller configuration (single layer)

              The configuration sample shows how to use a display
              containing 2 LCD-controllers.
              
              It uses GUIDRV_DIST which can be used to configure
              different (sub)devices for each controller.
---------------------------END-OF-HEADER------------------------------
*/

#include "GUI.h"
#include "LCD_X_8080_8.h"
#include "GUIDRV_SPage.h"
#include "GUIDRV_Dist.h"

/*********************************************************************
*
*       Layer configuration
*
**********************************************************************
*/
//
// Physical display size
//
#define XSIZE_PHYS 224
#define YSIZE_PHYS 320

//
// Color conversion
//
#define COLOR_CONVERSION GUICC_4

//
// Display driver
//
#define DISPLAY_DRIVER GUIDRV_SPAGE_4C1

/*********************************************************************
*
*       Configuration checking
*
**********************************************************************
*/
#ifndef   VXSIZE_PHYS
  #define VXSIZE_PHYS XSIZE_PHYS
#endif
#ifndef   VYSIZE_PHYS
  #define VYSIZE_PHYS YSIZE_PHYS
#endif
#ifndef   XSIZE_PHYS
  #error Physical X size of display is not defined!
#endif
#ifndef   YSIZE_PHYS
  #error Physical Y size of display is not defined!
#endif
#ifndef   COLOR_CONVERSION
  #error Color conversion not defined!
#endif
#ifndef   DISPLAY_DRIVER
  #error No display driver defined!
#endif

/*********************************************************************
*
*       Static code
*
**********************************************************************
*/
/*********************************************************************
*
*       _InitController
*
* Purpose:
*   Initializes the display controller
*/
static void _InitController(void) {
#ifndef WIN32
  //
  // Initialize lines
  //
  LCD_X_8080_8_Init();
  GUI_X_Delay(50);
  //
  // Initialize controller 0
  //
  LCD_X_8080_8_Write00(0xeb); /* SET LCD BIAS RATIO: 12 */
  LCD_X_8080_8_Write00(0x81); /* SET VBIAS POTENTIOMETER AND GAIN */
  LCD_X_8080_8_Write00(0x90);
  LCD_X_8080_8_Write00(0xa6); /* SET NORMAL DISPLAY */
  LCD_X_8080_8_Write00(0xd0); /* SET GRAY SCALE MODE */
  LCD_X_8080_8_Write00(0xc0); /* SET LCD MAPPING CONTROL */
  LCD_X_8080_8_Write00(0x04);
  LCD_X_8080_8_Write00(0xa3); /* SET LINE RATE */
  LCD_X_8080_8_Write00(0xaf); /* SET DISPLAY ENABLE */
  //
  // Initialize controller 1
  //
  LCD_X_8080_8_Write10(0xeb); /* SET LCD BIAS RATIO: 12 */
  LCD_X_8080_8_Write10(0x81); /* SET VBIAS POTENTIOMETER AND GAIN */
  LCD_X_8080_8_Write10(0x90);
  LCD_X_8080_8_Write10(0xa6); /* SET NORMAL DISPLAY */
  LCD_X_8080_8_Write10(0xd0); /* SET GRAY SCALE MODE */
  LCD_X_8080_8_Write10(0xc0); /* SET LCD MAPPING CONTROL */
  LCD_X_8080_8_Write10(0x02);
  LCD_X_8080_8_Write10(0xa3); /* SET LINE RATE */
  LCD_X_8080_8_Write10(0xaf); /* SET DISPLAY ENABLE */
#endif // WIN32
}

/*********************************************************************
*
*       Public code
*
**********************************************************************
*/
/*********************************************************************
*
*       LCD_X_Config
*
* Purpose:
*   Called during the initialization process in order to set up the
*   display driver configuration.
*   
*/
void LCD_X_Config(void) {
  GUI_DEVICE * pDevice;
  GUI_RECT Rect0;
  GUI_RECT Rect1;
  CONFIG_SPAGE Config0 = {0};
  CONFIG_SPAGE Config1 = {0};
  GUI_DEVICE * pDevice0;
  GUI_DEVICE * pDevice1;
  GUI_PORT_API PortAPI0 = {0}; 
  GUI_PORT_API PortAPI1 = {0}; 

  //
  // Set display driver and color conversion for 1st layer
  //
  pDevice = GUI_DEVICE_CreateAndLink(GUIDRV_DIST, COLOR_CONVERSION, 0, 0);
  //
  // Display size configuration
  //
  if (LCD_GetSwapXY()) {
    LCD_SetSizeEx (0, YSIZE_PHYS,   XSIZE_PHYS);
    LCD_SetVSizeEx(0, VYSIZE_PHYS,  VXSIZE_PHYS);
  } else {
    LCD_SetSizeEx (0, XSIZE_PHYS,   YSIZE_PHYS);
    LCD_SetVSizeEx(0, VXSIZE_PHYS,  VYSIZE_PHYS);
  }
  //
  // Create first driver
  //
  pDevice0 = GUI_DEVICE_Create(DISPLAY_DRIVER, COLOR_CONVERSION, 0, -1);
  //
  // Configuration first driver
  //
  Config0.FirstSEG = 0;//256 - 224;
  GUIDRV_SPage_Config(pDevice0, &Config0);
  //
  // Configure hardware routines of first driver
  //
  PortAPI0.pfWrite8_A0  = LCD_X_8080_8_Write00;
  PortAPI0.pfWrite8_A1  = LCD_X_8080_8_Write01;
  PortAPI0.pfWriteM8_A0 = LCD_X_8080_8_WriteM00;
  PortAPI0.pfWriteM8_A1 = LCD_X_8080_8_WriteM01;
  PortAPI0.pfRead8_A0   = LCD_X_8080_8_Read00;
  PortAPI0.pfRead8_A1   = LCD_X_8080_8_Read01;
  PortAPI0.pfReadM8_A0  = LCD_X_8080_8_ReadM00;
  PortAPI0.pfReadM8_A1  = LCD_X_8080_8_ReadM01;
  GUIDRV_SPage_SetBus8(pDevice0, &PortAPI0);
  //
  // Controller configuration first driver
  //
  GUIDRV_SPage_SetUC1611(pDevice0);
  //
  // Create second driver
  //
  pDevice1 = GUI_DEVICE_Create(DISPLAY_DRIVER, COLOR_CONVERSION, 0, -1);
  //
  // Configuration second driver
  //
  Config1.FirstSEG = 256 - 224;
  GUIDRV_SPage_Config(pDevice1, &Config1);
  //
  // Configure hardware routines of second driver
  //
  PortAPI1.pfWrite8_A0  = LCD_X_8080_8_Write10;
  PortAPI1.pfWrite8_A1  = LCD_X_8080_8_Write11;
  PortAPI1.pfWriteM8_A0 = LCD_X_8080_8_WriteM10;
  PortAPI1.pfWriteM8_A1 = LCD_X_8080_8_WriteM11;
  PortAPI1.pfRead8_A0   = LCD_X_8080_8_Read10;
  PortAPI1.pfRead8_A1   = LCD_X_8080_8_Read11;
  PortAPI1.pfReadM8_A0  = LCD_X_8080_8_ReadM10;
  PortAPI1.pfReadM8_A1  = LCD_X_8080_8_ReadM11;
  GUIDRV_SPage_SetBus8(pDevice1, &PortAPI1);
  //
  // Controller configuration second driver
  //
  GUIDRV_SPage_SetUC1611(pDevice1);
  //
  // Add drivers to distribution driver
  //
  Rect0.x0 = 0;
  Rect0.y0 = 160;
  Rect0.x1 = 223;
  Rect0.y1 = 319;
  GUIDRV_Dist_AddDriver(pDevice, pDevice0, &Rect0);
  Rect1.x0 = 0;
  Rect1.y0 = 0;
  Rect1.x1 = 223;
  Rect1.y1 = 159;
  GUIDRV_Dist_AddDriver(pDevice, pDevice1, &Rect1);
}

/*********************************************************************
*
*       LCD_X_DisplayDriver
*
* Purpose:
*   This function is called by the display driver for several purposes.
*   To support the according task the routine needs to be adapted to
*   the display controller. Please note that the commands marked with
*   'optional' are not cogently required and should only be adapted if 
*   the display controller supports these features.
*
* Parameter:
*   LayerIndex - Index of layer to be configured
*   Cmd        - Please refer to the details in the switch statement below
*   pData      - Pointer to a LCD_X_DATA structure
*/
int LCD_X_DisplayDriver(unsigned LayerIndex, unsigned Cmd, void * pData) {
  int r;

  GUI_USE_PARA(LayerIndex);
  GUI_USE_PARA(pData);
  switch (Cmd) {
  //
  // Required
  //
  case LCD_X_INITCONTROLLER: {
    //
    // Called during the initialization process in order to set up the
    // display controller and put it into operation. If the display
    // controller is not initialized by any external routine this needs
    // to be adapted by the customer...
    //
    if (LayerIndex == 0) {
      _InitController();
    }
    return 0;
  }
  default:
    r = -1;
  }
  return r;
}

/*************************** End of file ****************************/
