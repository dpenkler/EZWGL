#include "EZ.h"

static void quit(EZ_Widget *widget, void *data)
{
  if(getenv("SIMAGE"))
    {
      FILE *fp = fopen("locator.ppm", "w");
      if(fp) 
        {
          EZ_SaveWidget2PPM(data, fp);
          fclose(fp);
        }
    }
  EZ_Shutdown();
  exit(0);
}

int main(int ac, char **av)
{
  EZ_Widget *top, *ruler, *frame, *btn;

  EZ_Initialize(ac, av, 0);

  ruler  = EZ_CreateWidgetXrm(EZ_WIDGET_LOCATOR,    NULL,
                              "ruler",                       "ruler",
                              EZ_SIZE,                       200, 200,
                             EZ_BORDER_TYPE,     EZ_BORDER_RAISED,
                             EZ_BORDER_WIDTH,    2,
                              0);

  frame = EZ_CreateWidgetXrm(EZ_WIDGET_FRAME,    ruler,
                              "Frame",           "frame",
                             EZ_BG_IMAGE_FILE,   "marble",
                             EZ_DOCKABLE,        True,
                             EZ_BORDER_TYPE,     EZ_BORDER_RAISED,
                             EZ_BORDER_WIDTH,    2,
                             0);
  btn  =   EZ_CreateWidgetXrm(EZ_WIDGET_NORMAL_BUTTON,    frame,
                              "AButton",           "aButton",
                              EZ_LABEL_STRING,     "Done",
                              EZ_CALLBACK,          quit, ruler,
                              0);
  EZ_DisplayWidget(ruler);
  EZ_EventMainLoop();
}

