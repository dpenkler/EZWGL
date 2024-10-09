/*************************** Example 15 ***************************/
#include "EZ.h"

void  rbuttonCallback(EZ_Widget *btn, void *data)
{
  char *ptr = (char *)EZ_GetWidgetPtrData(btn);
  EZ_SetDefaultLabelFont(ptr); 
}

int main(int ac, char **av)
{
  EZ_Widget *frame, *frame1, *menubar, *tmp, *mbtn;
  
  frame = EZ_CreateFrame(NULL, NULL);
  EZ_ConfigureWidget(frame, EZ_WIDTH, 400, EZ_HEIGHT, 200, 0);
  menubar = EZ_CreateFrame(frame, NULL);
  EZ_ConfigureWidget(menubar, EZ_X, 0, EZ_Y, 0, 
		     EZ_WIDTH, 400, EZ_HEIGHT, 26,
		     EZ_BORDER_TYPE, EZ_BORDER_UP,
                     EZ_BORDER_WIDTH, 2, 0);
  mbtn = EZ_CreateMenuButton(menubar,"MenuButton 1", 0);
  EZ_ConfigureWidget(mbtn, EZ_X, 5, EZ_Y, 3, EZ_HEIGHT, 20, EZ_WIDTH, 90, 0);
  tmp = EZ_CreateSimpleMenu("|||||");
  EZ_SetMenuButtonMenu(mbtn, tmp);
  mbtn = EZ_CreateMenuButton(menubar,"MenuButton 2", 1);
  EZ_ConfigureWidget(mbtn, EZ_X, 100, EZ_Y, 3, EZ_HEIGHT, 20, EZ_WIDTH, 90, 0);
  tmp = EZ_CreateSimpleMenu("Menu 2 %t|||||");
  EZ_SetMenuButtonMenu(mbtn, tmp);
  mbtn = EZ_CreateMenuButton(menubar,"MenuButton 3", 2);
  EZ_ConfigureWidget(mbtn, EZ_X, 195, EZ_Y, 3, EZ_HEIGHT, 20, EZ_WIDTH, 90, 0);
  tmp = EZ_CreateSimpleMenu("Menu 3 %t|||||");
  EZ_SetMenuButtonMenu(mbtn, tmp);

  /* put something there */
  frame1 = EZ_CreateFrame(frame, NULL);
  EZ_ConfigureWidget(frame1, EZ_X,100, EZ_Y, 100, 0, EZ_WIDTH, 200,
		     EZ_HEIGHT, 200, 0);
  tmp = EZ_CreateRadioButton(frame1, "Normal Font", 0, 0, 0);
  EZ_ConfigureWidget(tmp, EZ_CLIENT_PTR_DATA,
		     "-Adobe-Helvetica-Bold-R-Normal--*-120-*-*-*-*-*-*",
		     EZ_CALLBACK, rbuttonCallback, NULL, 0);
  tmp = EZ_CreateRadioButton(frame1, "Large Font",  0,  0, 1);
  EZ_ConfigureWidget(tmp, EZ_CLIENT_PTR_DATA,
		     "-Adobe-Helvetica-Medium-R-Normal--*-180-*-*-*-*-*-*",
		     EZ_CALLBACK, rbuttonCallback, NULL, 0);

  EZ_DisplayWidget(frame);
  EZ_EventMainLoop();
}
/*************************** Example 15 ***************************/
