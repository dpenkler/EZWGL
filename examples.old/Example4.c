/************************** Example 4 ***********************/
#include "EZ.h"                     /* the header file      */

static char *colors[] = {  "red", "green", "blue", "yellow",
			   "cyan", "magenta",  "white", NULL};

void buttonCallBack(EZ_Widget *widget, void *data) 
{                                  /* callback for button */
  if(widget && data)
    {
      EZ_Widget *menu = (EZ_Widget *)data;
      EZ_DoPopup(menu, EZ_BOTTOM);
    }
}

void menuCallBack(EZ_Widget *widget, void *data)
{
  if(widget)
    {
      int i = EZ_GetWidgetReturnedData(widget);
      if( i >= 0 && i <= 6)    /* to make sure */
	{
	  EZ_Widget *tmp = (EZ_Widget *)data;
	  if(tmp)
	    {
	      char str[64];
	      sprintf(str,"A %s Button", colors[i]);
	      EZ_ConfigureWidget(tmp, EZ_LABEL_STRING, str,
				 EZ_FOREGROUND, colors[i],0);
	    }
	}
    }
}

main(int argc, char **argv)
{
  EZ_Widget *menu, *button, *tmp;

  EZ_Initialize(argc, argv, 0);
  button = EZ_CreateButton(NULL,"An Optional Button", -1); /* a button */
  menu = EZ_CreatePopupMenu(NULL);

  EZ_AddWidgetCallBack(button, EZ_CALLBACK, 
		       buttonCallBack, menu, 0);

  EZ_AddWidgetCallBack(menu, EZ_CALLBACK, 
		       menuCallBack, button, 0);
  {
    char **ptr = colors;
    int i = 0;
    while(*ptr)
      {
	tmp = EZ_CreateMenuNormalButton(menu, *ptr, -1, i++);
	ptr++;
      }
  }
  
  EZ_DisplayWidget(button);
  EZ_EventMainLoop();
}
/************************** Example 4 ***********************/
