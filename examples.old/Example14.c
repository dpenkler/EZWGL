/*************************** Example 14 ***************************/
#include "EZ.h"

void  rbuttonCallback(EZ_Widget *btn, void *data)
{
  char *ptr = (char *)EZ_GetWidgetPtrData(btn);
  EZ_SetDefaultLabelFont(ptr);
}

main(int ac, char **av)
{
  EZ_Widget *frame, *frame1, *menubar, *tmp, *mbtn;
  
  frame = EZ_CreateFrame(NULL, NULL);
  EZ_ConfigureWidget(frame, EZ_ORIENTATION,EZ_VERTICAL, 
                     EZ_FILL_MODE, EZ_FILL_VERTICALLY,
                     EZ_PADY, 0, 0);
  menubar = EZ_CreateFrame(frame, NULL);
  EZ_ConfigureWidget(menubar, EZ_EXPAND, True, /* strecth horizontally */
                     EZ_PADX, 0, EZ_PADY,0, 
                     EZ_IPADY, 0, EZ_HEIGHT, 0, /* keep its height mininum */
		     EZ_BORDER_TYPE, EZ_BORDER_UP,
                     EZ_BORDER_WIDTH, 2, 
                     EZ_ORIENTATION, EZ_HORIZONTAL_LEFT, 
                     0);
  mbtn = EZ_CreateMenuButton(menubar,"MenuButton 1", 0);
  tmp = EZ_CreateSimpleMenu("|||||");
  EZ_SetMenuButtonMenu(mbtn, tmp);
  mbtn = EZ_CreateMenuButton(menubar,"MenuButton 2", 1);
  tmp = EZ_CreateSimpleMenu("Menu 2 %t|||||");
  EZ_SetMenuButtonMenu(mbtn, tmp);
  mbtn = EZ_CreateMenuButton(menubar,"MenuButton 3", 2);
  tmp = EZ_CreateSimpleMenu("Menu 3 %t|||||");
  EZ_SetMenuButtonMenu(mbtn, tmp);

  /* put something there */
  frame1 = EZ_CreateFrame(frame, NULL);

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
/*************************** Example 14 ***************************/
