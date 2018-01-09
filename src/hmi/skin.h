#ifndef SKIN_H
#define	SKIN_H

typedef struct
{
    const GUI_BITMAP *BitmapConstChosen;
    const GUI_BITMAP *BitmapConstNotChosen;
}PageChosenData_struct;

int SKIN_checkbox(const WIDGET_ITEM_DRAW_INFO * pDrawItemInfo);

#endif
