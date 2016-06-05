
#include "EZ.h"

static quit(EZ_Widget *widget, void *data)
{
  if(getenv("SIMAGE"))
    {
      FILE *fp = fopen("nbutton.ppm", "w");
      if(fp) 
        {
          EZ_SaveWidget2PPM(data, fp);
          fclose(fp);
        }
    }
  EZ_Shutdown();
  exit(0);
}
static btnCb(EZ_Widget *widget, void *data)
{
  fprintf(stderr, "You've pressed: %s\n", (char *)data);
}

main(int ac, char **av)
{
  EZ_Widget *frame, *btn1, *btn2, *btn3, *frame4, *btn;

  EZ_Initialize(ac, av, 0);
  EZ_DisableHighlight();
  frame  = EZ_CreateWidgetXrm(EZ_WIDGET_FRAME,    NULL,
                              "Buttons",          "buttons",
                              EZ_IPADX,           4,
                              0);
  btn1    = EZ_CreateWidgetXrm(EZ_WIDGET_NORMAL_BUTTON,    frame,
                              "Btn1",             "btn1",
                               EZ_LABEL_STRING,    "A Button",
                               EZ_FOREGROUND,      "red",
                               EZ_CALLBACK,         btnCb, "Button 1",
                              0);
  btn2   = EZ_CreateWidgetXrm(EZ_WIDGET_NORMAL_BUTTON,    frame,
                              "Btn2",             "btn2",
                              EZ_PIXMAP_FILE,     "twotails.xpm",
                              EZ_BORDER_WIDTH,    2,
                              EZ_CALLBACK,         btnCb, "Button 2",
                              0);

  frame4 = EZ_CreateWidgetXrm(EZ_WIDGET_FRAME,    frame,
                              "Frame4",           "frame4",
                              EZ_LABEL_STRING,    " ",     
                              EZ_BACKGROUND,      "#00cccc",
                              EZ_DOCKABLE,        True,
                              0);
  btn  =   EZ_CreateWidgetXrm(EZ_WIDGET_NORMAL_BUTTON,    frame4,
                              "AButton",           "aButton",
                              EZ_LABEL_STRING,     "Done",
                              EZ_CALLBACK,          quit, frame,
                              0);
  EZ_DisplayWidget(frame);
  EZ_EventMainLoop();
}


