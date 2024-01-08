#include "EZ.h"

#include "EZ.h"

static void quit(EZ_Widget *widget, void *data)
{
  if(getenv("SIMAGE"))
    {
      FILE *fp = fopen("optentry1.ppm", "w");
      if(fp) 
	{
	  EZ_SaveWidget2PPM(data, fp);
	  fclose(fp);
	}
    }
  EZ_Shutdown();
  exit(0);
}

static char *colors[] = {
  "red",
  "green",
  "blue",
  "yellow",
  "cyan",
  "magenta",
  "white",
  "gainsboro   fd af d  f  d a   fd sa f  fdsa   fdsaf  fdsa ",
  "PapayaWhip",
  "PeachPuff",
};


int main(int ac, char **av)
{
  EZ_Widget *frame1, *oe1, *oe2, *frame5, *btn;

  EZ_Initialize(ac,av,0);
  frame1 = EZ_CreateWidget(EZ_WIDGET_FRAME, NULL,
                           EZ_FILL_MODE, EZ_FILL_BOTH,
                           0);
  
  oe2 =  EZ_CreateWidget(EZ_WIDGET_OPTIONAL_ENTRY, frame1,
                         EZ_ENTRY_STRING,  "OptionalEntry", 
                         EZ_FORGET_GEOMETRY, 1,
                         EZ_FILL_MODE, EZ_FILL_BOTH,
                         0);
  oe1 =  EZ_CreateWidget(EZ_WIDGET_OPTIONAL_ENTRY, frame1,
                         EZ_ENTRY_STRING,  "Colors", 
                         0);
  frame5 = EZ_CreateWidgetXrm(EZ_WIDGET_FRAME,    frame1,
                              "Frame5",           "frame5",
                              EZ_BG_IMAGE_FILE,   "marble",
                              EZ_DOCKABLE,        True,
                              EZ_BORDER_TYPE,     EZ_BORDER_RAISED,
                              EZ_BORDER_WIDTH,    2,
                              0);
  btn  =   EZ_CreateWidgetXrm(EZ_WIDGET_NORMAL_BUTTON,    frame5,
                              "AButton",           "aButton",
                              EZ_LABEL_STRING,     "Done",
                              EZ_CALLBACK,          quit, frame1,
                              0);
  EZ_SetOptionalEntryOptionalItems(oe2, colors, 10);
  EZ_DisplayWidget(frame1);
  EZ_EventMainLoop();
}
/*
End optentry.c
\end{verbatim}
*/






