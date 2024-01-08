/*************************************************************/
#include "EZ.h"
#include <string.h>

int encodeBG(EZ_Widget *, void *, char **, int *, int *);
int decodeBG(EZ_Widget *, void *, char *, int);

static char *colors[] = { "red", "green", "blue", "cyan", 
                          "magenta", "yellow", "red3","#cf00cf"};

Atom MY_BG_ATOM;

int main(int ac, char **av)
{
  EZ_Widget *frame, *tmp, *btn;
  Cursor cursor;
  int i;
  
  EZ_Initialize(ac,av,0);
  MY_BG_ATOM = EZ_GetAtom("MY_BG_ATOM");

  cursor = EZ_GetCursor(XC_box_spiral);
  frame = EZ_CreateWidget(EZ_WIDGET_FRAME, NULL,
                          EZ_ORIENTATION, EZ_VERTICAL,
                          EZ_LABEL_STRING, "Drag sources",  0);
  for(i = 0; i < 8; i++)
    {
      btn = EZ_CreateWidget(EZ_WIDGET_NORMAL_BUTTON, frame,
                            EZ_LABEL_STRING, colors[i], 
                            EZ_EXPAND, True,
			    EZ_BACKGROUND, colors[i], 
                            EZ_DND_BUBBLE_HELP, "DnD bubble help",
                            0);
      EZ_WidgetAddDnDDataEncoder(btn, MY_BG_ATOM, 0,
				 encodeBG, NULL,
				 NULL, NULL);
      EZ_WidgetAddDnDDataDecoder(btn, MY_BG_ATOM, 0,
				 decodeBG, NULL,
				 NULL, NULL);
    }
  
  EZ_DisplayWidget(frame);
  EZ_EventMainLoop();
}

int encodeBG(EZ_Widget *widget, void *data,
	     char **message, int *length, int *needFree)
{
  static char str[32];  
  if(widget)
    {
      unsigned long pv;
      int r,g,b;

      /* why not just use the label string? */
      EZ_GetBackgroundPV(widget, &pv, 0, 0);
      EZ_PixelValue2RGB(pv, &r, &g, &b);
      sprintf(str, "#%02x%02x%02x", r, g, b);
      
      *length = strlen(str);
      *message = str;
      *needFree = 0;

      return(EZ_DND_SUCCESS);
    }
  return(EZ_DND_FAILURE);
}

int decodeBG(EZ_Widget *widget, void *data,
	     char *message, int length)
{
  if(widget)
    {
      /* the message is a correct color specification */
      EZ_ConfigureWidget(widget, EZ_FOREGROUND, message, 0);
      return(EZ_DND_SUCCESS);
    }
  return(EZ_DND_FAILURE);
}
/*************************************************************/
     

