#include "EZ.h"

static quit(EZ_Widget *widget, void *data)
{
  if(getenv("SIMAGE"))
    {
      FILE *fp = fopen("rbutton.ppm", "w");
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
  EZ_Widget *frame, *rbtn1, *rbtn2, *rbtn3, *frame4, *btn;

  EZ_Initialize(ac, av, 0);
  frame  = EZ_CreateWidgetXrm(EZ_WIDGET_FRAME,    NULL,
                              "Buttons",          "buttons",
                              EZ_IPADX,           4,
                              EZ_ORIENTATION,     EZ_VERTICAL,       
                              EZ_FILL_MODE,       EZ_FILL_HORIZONTALLY,
                              0);
  rbtn1   = EZ_CreateWidgetXrm(EZ_WIDGET_RADIO_BUTTON,    frame,
                              "Btn1",             "btn1",
                               EZ_LABEL_STRING,    "Radio Button 1",
                               EZ_LABEL_POSITION,  EZ_LEFT,
                               EZ_BORDER_WIDTH,    1,
                               EZ_BORDER_TYPE,     EZ_BORDER_UP,
                               EZ_RADIO_BUTTON_GROUP, 10,
                               EZ_RADIO_BUTTON_VALUE, 1,
                               EZ_FOREGROUND,      "red",
				EZ_BACKGROUND, "black",
                              0);

  rbtn2   = EZ_CreateWidgetXrm(EZ_WIDGET_RADIO_BUTTON,    frame,
                              "Btn1",             "btn2",
                               EZ_LABEL_STRING,    "Another Radio Button",
                               EZ_FOREGROUND,      "blue",
                               EZ_LABEL_POSITION,  EZ_LEFT,
                               EZ_UNDERLINE,       0,
                               EZ_BORDER_WIDTH,    1,
                               EZ_BORDER_TYPE,     EZ_BORDER_UP,
                               EZ_RADIO_BUTTON_GROUP, 10,
                               EZ_RADIO_BUTTON_VALUE, 2,
                               EZ_INDICATOR_COLOR, "green",
                               EZ_FOREGROUND,      "red",
                              0);
  rbtn3   = EZ_CreateWidgetXrm(EZ_WIDGET_RADIO_BUTTON,    frame,
                              "Btn1",             "btn3",
                               EZ_LABEL_STRING,    "One more radio button",
                               EZ_PIXMAP_FILE,     "folder.xpm",
                               EZ_LABEL_POSITION,  EZ_LEFT,
                               EZ_BORDER_WIDTH,    1,
                               EZ_BORDER_TYPE,     EZ_BORDER_UP,
                               EZ_RADIO_BUTTON_GROUP, 10,
                               EZ_RADIO_BUTTON_VALUE, 3,
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


