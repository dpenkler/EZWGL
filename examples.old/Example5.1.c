/************************** Example 5 ***********************/
#include "EZ.h"                     /* the header file      */

static char *colors[] = {  "red", "green", "blue", "yellow",
			   "cyan", "magenta",  "white", NULL};

void buttonEventHandler(EZ_Widget *widget, int eType, XEvent *xev) 
{                             /* a very simple event handler */
  if(widget)
    {
      if(eType == EZ_LEFT_BUTTON_PRESS)
	{
	  EZ_Widget *menu = (EZ_Widget *)EZ_GetWidgetPtrData(widget);
	  {
	    int x,y,w,h;
	    EZ_GetWidgetAbsoluteGeometry(widget,&x,&y,&w,&h);
	    EZ_DoPopupAt(menu, x, y+h);
	  }
          /* disable all other event handlers on this event,
	   * include the default one !! */
	  xev->type = 0;
	}
    }
}

void menuCallBack(EZ_Widget *widget)
{
  if(widget)
    {
      int i = EZ_GetWidgetReturnedData(widget);
      if( i >= 0 && i <= 6)    /* to make sure */
	{
	  EZ_Widget *tmp = (EZ_Widget *)EZ_GetWidgetPtrData(widget);
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

  EZ_ConfigureWidget(button, 
		     EZ_EVENT_HANDLER, buttonEventHandler,
		     EZ_CLIENT_PTR_DATA, menu,
		     0);
  EZ_ConfigureWidget(menu, 
		     EZ_CALLBACK, menuCallBack,
		     EZ_CLIENT_PTR_DATA, button, 
		     0);
  
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
/************************** Example 5 ***********************/
