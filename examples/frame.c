#include "EZ.h"

static quit(EZ_Widget *widget, void *data)
{
  if(getenv("SIMAGE"))
    {
      FILE *fp = fopen("frame.ppm", "w");
      if(fp) 
        {
          EZ_SaveWidget2PPM(data, fp);
          fclose(fp);
        }
    }
  EZ_Shutdown();
  exit(0);
}
main(int ac, char **av)
{
  EZ_Widget *frame, *frame1, *frame2, *frame3, *frame4, *frame5, *btn;

  EZ_Initialize(ac, av, 0);
  frame  = EZ_CreateWidgetXrm(EZ_WIDGET_FRAME,    NULL,
                              "Frames",           "frames",
                              EZ_FILL_MODE,       EZ_FILL_BOTH,
                              EZ_IPADX,           4,
                              EZ_SIZE,            600, 100,
                              0);
  frame1 = EZ_CreateWidgetXrm(EZ_WIDGET_FRAME,    frame,
                              "Frame1",           "frame1",
                              EZ_LABEL_STRING,    "A Labeled Frame",
                              EZ_BACKGROUND,      "#cc00cc",
                              0);
  frame2 = EZ_CreateWidgetXrm(EZ_WIDGET_FRAME,    frame,
                              "Frame2",           "frame2",
                              EZ_BACKGROUND,      "bisque",
                              EZ_BORDER_TYPE,     EZ_BORDER_SUNKEN,
                              EZ_BORDER_WIDTH,    2,
                              0);
  frame3 = EZ_CreateWidgetXrm(EZ_WIDGET_FRAME,    frame,
                              "Frame3",           "frame3",
                              EZ_LABEL_STRING,    " ",     
                              EZ_PIXMAP_FILE,     "folder.xpm",
                              EZ_LABEL_POSITION,  EZ_LEFT,
                              EZ_BACKGROUND,      "#cccc00",
                              EZ_INTERIOR_BORDER, 4, EZ_BORDER_SHADOW,
                              0);
  frame4 = EZ_CreateWidgetXrm(EZ_WIDGET_FRAME,    frame,
                              "Frame4",           "frame4",
                              EZ_LABEL_STRING,    " ",     
                              EZ_BACKGROUND,      "#00cccc",
                              EZ_INTERIOR_BORDER, 3, EZ_BORDER_EMBOSSED,
                              0);
  frame5 = EZ_CreateWidgetXrm(EZ_WIDGET_FRAME,    frame,
                              "Frame5",           "frame5",
                              EZ_BG_IMAGE_FILE,   "marble",
                              EZ_DOCKABLE,        True,
                              EZ_BORDER_TYPE,     EZ_BORDER_RAISED,
                              EZ_FILL_MODE, EZ_FILL_BOTH,
                              EZ_BORDER_WIDTH,    2,
                              0);
  btn  =   EZ_CreateWidgetXrm(EZ_WIDGET_NORMAL_BUTTON,    frame5,
                              "AButton",           "aButton",
                              EZ_LABEL_STRING,     "Done",
                              EZ_CALLBACK,          quit, frame,
                              0);
  EZ_DisplayWidget(frame);
  EZ_EventMainLoop();
}

