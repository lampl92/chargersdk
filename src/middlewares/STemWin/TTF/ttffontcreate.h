#ifndef _TTFFONTCREATE_H
#define _TTFFONTCREATE_H
#include "includes.h"
#include "GUI.h"

extern GUI_FONT TTF12_Font;
extern GUI_FONT TTF18_Font;
extern GUI_FONT TTF24_Font;
extern GUI_FONT TTF36_Font;

uint8_t Create_TTFFont(uint8_t *fxpath);
#endif
