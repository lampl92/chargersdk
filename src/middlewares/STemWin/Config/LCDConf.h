#ifndef _LCDCONF_H
#define _LCDCONF_H

#include "GUI.h"
#include "GUIDRV_Lin.h"

typedef struct
{
  int      address;
  volatile int pending_buffer;
  int      buffer_index;
  int      xSize;
  int      ySize;
  int      BytesPerPixel;
  LCD_API_COLOR_CONV   *pColorConvAPI;
}
LCD_LayerPropTypedef;

#endif

