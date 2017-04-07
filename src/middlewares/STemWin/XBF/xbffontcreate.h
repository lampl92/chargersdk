#ifndef _XBFFONTCREATE_H
#define _XBFFONTCREATE_H
#include "GUI.h"
#include "includes.h"

extern FIL XBF12FontFile;
extern FIL XBF16FontFile;
extern FIL XBF24FontFile;
extern FIL XBF36FontFile;

extern GUI_FONT XBF12_Font;
extern GUI_FONT XBF16_Font;
extern GUI_FONT XBF24_Font;
extern GUI_FONT XBF36_Font;

uint8_t Date_buf[40];
uint8_t Time_buf[40];

uint8_t Create_XBF12(uint8_t *fxpath);
uint8_t Create_XBF16(uint8_t *fxpath);
uint8_t Create_XBF24(uint8_t *fxpath);
uint8_t Create_XBF36(uint8_t *fxpath);
#endif

