
#include "EZ.h"

static quit(EZ_Widget *widget, void *data)
{
  if(getenv("SIMAGE"))
    {
      FILE *fp = fopen("cbutton.ppm", "w");
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
  EZ_Widget *frame, *cbtn1, *cbtn2, *cbtn3, *frame4, *btn;

  EZ_Initialize(ac, av, 0);
  frame  = EZ_CreateWidgetXrm(EZ_WIDGET_FRAME,    NULL,
                              "Buttons",          "buttons",
                              EZ_IPADX,           4,
                              EZ_ORIENTATION,     EZ_VERTICAL,       
                              EZ_FILL_MODE,       EZ_FILL_HORIZONTALLY,
                              0);
  cbtn1   = EZ_CreateWidgetXrm(EZ_WIDGET_CHECK_BUTTON,    frame,
                              "Btn1",             "btn1",
                               EZ_LABEL_STRING,    "Check Button 1",
                               EZ_LABEL_POSITION,  EZ_LEFT,
                               EZ_BORDER_WIDTH,    1,
                               EZ_BORDER_TYPE,     EZ_BORDER_UP,
                               EZ_FOREGROUND,      "red",
                              0);

  cbtn2   = EZ_CreateWidgetXrm(EZ_WIDGET_CHECK_BUTTON,    frame,
                              "Btn1",             "btn2",
                               EZ_LABEL_STRING,    "Another Button",
                               EZ_LABEL_POSITION,  EZ_LEFT,
                               EZ_BORDER_WIDTH,    1,
                               EZ_BORDER_TYPE,     EZ_BORDER_UP,
                               EZ_INDICATOR_TYPE,  EZ_CIRCLE_INDICATOR,
                               EZ_INDICATOR_COLOR, "green",
                               EZ_FOREGROUND,      "red",
                              0);
  cbtn3   = EZ_CreateWidgetXrm(EZ_WIDGET_CHECK_BUTTON,    frame,
                              "Btn1",             "btn3",
                               EZ_LABEL_STRING,    "One more check button",
                               EZ_LABEL_POSITION,  EZ_LEFT,
                               EZ_BORDER_WIDTH,    1,
                               EZ_BORDER_TYPE,     EZ_BORDER_UP,
                               EZ_INDICATOR_TYPE,  EZ_TICK_INDICATOR,
                               EZ_FOREGROUND,      "blue",
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


