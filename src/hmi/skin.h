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


#endif
