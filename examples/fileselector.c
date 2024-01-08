  
#include "EZ.h"

static void quit(EZ_Widget *widget, void *data)
{
  if(getenv("SIMAGE"))
    {
      FILE *fp = fopen("fileselector.ppm", "w");
      if(fp) 
	{
	  EZ_SaveWidget2PPM(data, fp);
	  fclose(fp);
	}
    }
  EZ_Shutdown();
  exit(0);
}


static void ftest( EZ_Widget *widget, void *t)
{
  char *str = EZ_GetFileSelectorSelection(widget);
  printf("Your choice was: \"%s\"\n", str);
}


int main(int ac, char **av)
{
  EZ_Widget *tmp, *frame5, *btn;
  EZ_Initialize(ac,av,0);

  tmp = EZ_CreateWidget(EZ_WIDGET_FILE_SELECTOR, NULL,
                        EZ_CALLBACK, ftest, NULL,
                        EZ_ROW_BG, 1, "white", "bisque",
                        EZ_BG_IMAGE_FILE, "bg1.gif",
                        EZ_SELECTION_BACKGROUND, "yellow",
                        0);

  frame5 = EZ_CreateWidgetXrm(EZ_WIDGET_FRAME,    tmp,
                              "Frame5",           "frame5",
                              EZ_BG_IMAGE_FILE,   "marble",
                              EZ_DOCKABLE,        True,
                              EZ_BORDER_TYPE,     EZ_BORDER_RAISED,
                              EZ_BORDER_WIDTH,    2,
                              0);
  btn  =   EZ_CreateWidgetXrm(EZ_WIDGET_NORMAL_BUTTON,   frame5,
                              "AButton",           "aButton",
                              EZ_LABEL_STRING,     "Done",
                              EZ_CALLBACK,          quit, tmp,
                              0);
  EZ_DisplayWidget(tmp);
  EZ_EventMainLoop();
}

