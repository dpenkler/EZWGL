/**************************************************************/
#include "EZ.h"

int decodeBG(EZ_Widget *, void *, char *, int);

Atom MY_BG_ATOM;

int main(int ac, char **av)
{
  EZ_Widget *frame, *label;
  
  EZ_Initialize(ac,av,0);
  MY_BG_ATOM = EZ_GetAtom("MY_BG_ATOM");

  frame = EZ_CreateWidget(EZ_WIDGET_FRAME,NULL,
                          EZ_LABEL_STRING, "Drag target", 0); 
  label = EZ_CreateWidget(EZ_WIDGET_LABEL, frame, 
                          EZ_LABEL_STRING, "Drag and drop a color button to change my FG color",
                          EZ_DND_BUBBLE_HELP, "DnD bubble help", 0);
  EZ_WidgetAddDnDDataDecoder(label, MY_BG_ATOM, 0,
			     decodeBG, NULL,
			     NULL, NULL);
  EZ_DisplayWidget(frame);
  EZ_EventMainLoop();
}

int decodeBG(EZ_Widget *widget, void *data,
	     char *message, int length)
{
  if(widget)
    {
      /* the message is a correct color specification */
      EZ_ConfigureWidget(widget, EZ_BACKGROUND, message, 0);
      return(EZ_DND_SUCCESS);
    }
  return(EZ_DND_FAILURE);
}

/**************************************************************/
