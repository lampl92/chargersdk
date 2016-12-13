#include "GUI.h"

void MainTask(void)
{
	GUI_CURSOR_Show();
	CreateEVSE(); 
 
	while(1) 
	{ 
		GUI_Delay(10); 
	} 
}
