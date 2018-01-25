#ifndef SKIN_H
#define	SKIN_H

typedef struct
{
    const GUI_BITMAP *BitmapConstChosen;
    const GUI_BITMAP *BitmapConstNotChosen;
}PageChosenData_struct;

int SKIN_checkboxfull(const WIDGET_ITEM_DRAW_INFO * pDrawItemInfo);
int SKIN_checkboxtime(const WIDGET_ITEM_DRAW_INFO * pDrawItemInfo);
int SKIN_checkboxmoney(const WIDGET_ITEM_DRAW_INFO * pDrawItemInfo);
int SKIN_checkboxelectric(const WIDGET_ITEM_DRAW_INFO * pDrawItemInfo);

int SKIN_buttontest(const WIDGET_ITEM_DRAW_INFO * pDrawItemInfo);
int SKIN_buttongunAlookinfo(const WIDGET_ITEM_DRAW_INFO * pDrawItemInfo);
int SKIN_buttongunAscancode(const WIDGET_ITEM_DRAW_INFO * pDrawItemInfo);
int SKIN_buttongunAerror(const WIDGET_ITEM_DRAW_INFO * pDrawItemInfo);

int SKIN_buttongunBlookinfo(const WIDGET_ITEM_DRAW_INFO * pDrawItemInfo);
int SKIN_buttongunBscancode(const WIDGET_ITEM_DRAW_INFO * pDrawItemInfo);
int SKIN_buttongunBerror(const WIDGET_ITEM_DRAW_INFO * pDrawItemInfo);

#endif
