#ifndef _SIFFONTCREATE_H
#define _SIFFONTCREATE_H

#include "GUI.h"
#include "includes.h"

extern GUI_FONT SIF12_Font;
extern GUI_FONT SIF16_Font;
extern GUI_FONT SIF24_Font;
extern GUI_FONT SIF36_Font;

void Create_SIF12(uint8_t *fxpath);
void Create_SIF16(uint8_t *fxpath);
void Create_SIF24(uint8_t *fxpath);
void Create_SIF36(uint8_t *fxpath);
#endif
