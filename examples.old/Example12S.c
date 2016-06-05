/************************* Example 14S *************************************/
#include "EZ.h"

int encodeBG(EZ_Widget *, void *, char **, int *, int *);

static char *colors[] = { "red", "green", "blue", "cyan", 
                          "magenta", "yellow", "red3","#cf00cf"};

Atom MY_BG_ATOM;

main(int ac, char **av)
{
  EZ_Widget *frame, *tmp, *btn;
  int i;
  
  EZ_Initialize(ac,av,0);
  MY_BG_ATOM = EZ_GetAtom("MY_BG_ATOM");

  frame = EZ_CreateFrame(NULL, "Drag sources");
  tmp = EZ_CreateFrame(frame, NULL);
  EZ_ConfigureWidget(tmp, EZ_ORIENTATION, EZ_VERTICAL,
		     EZ_FILL_MODE, EZ_FILL_BOTH, 0);
  for(i = 0; i < 8; i++)
    {
      btn = EZ_CreateButton(tmp, colors[i], -1);
      EZ_ConfigureWidget(btn, EZ_EXPAND, True,
			 EZ_BACKGROUND, colors[i], 0);
      EZ_WidgetAddDnDDataEncoder(btn, MY_BG_ATOM, 0,
				 encodeBG, NULL,
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
/************************* Example 14S *************************************/
