#ifndef _XBFFONTCREATE_H
#define _XBFFONTCREATE_H
#include "GUI.h"
#include "includes.h"

extern FIL XBF12FontFile;
extern FIL XBF14FontFile;
extern FIL XBF16FontFile;
extern FIL XBF19FontFile;
extern FIL XBF24FontFile;
extern FIL XBF36FontFile;

extern GUI_FONT XBF12_Font;
extern GUI_FONT XBF14_Font;
extern GUI_FONT XBF16_Font;
extern GUI_FONT XBF19_Font;
extern GUI_FONT XBF24_Font;
extern GUI_FONT XBF36_Font;

uint8_t Date_buf[40];
uint8_t Time_buf[40];

uint8_t Create_XBF12(char *fxpath);
uint8_t Create_XBF14(char *fxpath);
uint8_t Create_XBF16(char *fxpath);
uint8_t Create_XBF19(char *fxpath);
uint8_t Create_XBF24(char *fxpath);
uint8_t Create_XBF36(char *fxpath);
#endif

