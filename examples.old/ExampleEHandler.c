/************************************** ExampleEHandler *****************************/
#include "EZ.h"

static void frameEventHandler(EZ_Widget *widget, void *data, int etype, XEvent *xev)
{
  EZ_Widget *label = (EZ_Widget *)data;
  char str[256];

  switch(etype)
    {
    case EZ_BUTTON1_PRESS:
      sprintf(str, "Button1Press:  (x,y)=(%d, %d)",
	      EZ_PointerCoordinates[0],EZ_PointerCoordinates[1]);
      break;
    case EZ_BUTTON1_RELEASE:
      sprintf(str, "Button1Release:  (x,y)=(%d, %d)",
	      EZ_PointerCoordinates[0],EZ_PointerCoordinates[1]);
      break;
    case EZ_POINTER_MOTION:
      sprintf(str, "PointerMotion:  (x,y)=(%d, %d)",
	      EZ_PointerCoordinates[0],EZ_PointerCoordinates[1]);
      break;
    case EZ_KEY_PRESS:
      sprintf(str,"KeyPress: key=%c, keycode=%d", EZ_PressedKey,EZ_PressedKey);
      break;
    case EZ_LEAVE_WINDOW:
      sprintf(str, "LeaveWindow");
      break;
    case EZ_ENTER_WINDOW:
      sprintf(str, "EnterWindow");
      break;      
    case EZ_REDRAW:
      sprintf(str, "Redraw");
      break;
    default:
      str[0]=0;
      break;
    }
  if(str[0]) EZ_ConfigureWidget(label, EZ_LABEL_STRING, str, 0);
}
      
main(int ac, char **av)
{
  EZ_Widget *frame, *label;
  
  EZ_Initialize(ac,av,0);

  frame = EZ_CreateFrame(NULL, NULL);
  label = EZ_CreateLabel(frame, NULL);

  EZ_ConfigureWidget(frame,EZ_ORIENTATION, EZ_VERTICAL_BOTTOM,
		     EZ_WIDTH, 400, EZ_HEIGHT, 300, 
		     EZ_EVENT_HANDLER, frameEventHandler, label, 0);

  EZ_ConfigureWidget(label, EZ_EXPAND, True, EZ_PROPAGATE, False, 
		     EZ_BORDER_WIDTH, 2, EZ_BORDER_TYPE, EZ_BORDER_SUNKEN,
		     EZ_TEXT_LINE_LENGTH, 80, EZ_FOREGROUND, "red",
		     EZ_BACKGROUND, "bisque2",
		     0);

  EZ_DisplayWidget(frame);
  EZ_EventMainLoop();
}

/************************************** ExampleEHandler *****************************/
