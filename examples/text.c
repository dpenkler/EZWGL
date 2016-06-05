#include "EZ.h"

static quit(EZ_Widget *widget, void *data)
{
  if(getenv("SIMAGE"))
    {
      FILE *fp = fopen("text.ppm", "w");
      if(fp) 
	{
	  EZ_SaveWidget2PPM(data, fp);
	  fclose(fp);
	}
    }
  EZ_Shutdown();
  exit(0);
}

/**************************************************************************/

main(int ac, char **av)
{
  EZ_Widget *frame, *text,*frame5, *btn;
  
  EZ_Initialize(ac,av,0);
  EZ_SetKbdHighlightColor("red");

  frame = EZ_CreateWidget(EZ_WIDGET_FRAME,  NULL,
                          EZ_LABEL_STRING,  "Text Widget",
                          EZ_ORIENTATION,   EZ_VERTICAL,
                          EZ_FILL_MODE,     EZ_FILL_BOTH,
                          0);

  text  = EZ_CreateWidget(EZ_WIDGET_TEXT, frame,
                          EZ_SELECTION_BACKGROUND, "yellow",
                          0);
  frame5 = EZ_CreateWidgetXrm(EZ_WIDGET_FRAME,    frame,
                              "Frame5",           "frame5",
                              EZ_BG_IMAGE_FILE,   "marble",
                              EZ_DOCKABLE,        True,
                              EZ_BORDER_TYPE,     EZ_BORDER_RAISED,
                              EZ_BORDER_WIDTH,    2,
                              0);
  btn  =   EZ_CreateWidgetXrm(EZ_WIDGET_NORMAL_BUTTON,    frame5,
                              "AButton",           "aButton",
                              EZ_LABEL_STRING,     "Done",
                              EZ_CALLBACK,          quit, frame,
                              0);
  EZ_TextLoadFile(text, "text.txt");
  EZ_DisplayWidget(frame);
  EZ_EventMainLoop();
}
/************************************************************************/
