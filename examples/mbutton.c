
#include "EZ.h"

EZ_Widget *tmenu, *tmbtn;

static quit(EZ_Widget *widget, void *data)
{
  EZ_Shutdown();
  exit(0);
}
static btnCb(EZ_Widget *widget, void *data)
{
  fprintf(stderr, "You've pressed: %s\n", (char *)data);
}

main(int ac, char **av)
{
  EZ_Widget *frame, *mbtn1, *mbtn2, *mbtn3, *frame4, *btn;
  EZ_Widget *menu1, *menu2, *menu3;

  EZ_Initialize(ac, av, 0);

  frame  = EZ_CreateWidgetXrm(EZ_WIDGET_FRAME,    NULL,
                              "Buttons",          "buttons",
                              EZ_IPADX,           4,
                              EZ_SIDE,            EZ_TOP,
                              0);
  mbtn1   = EZ_CreateWidgetXrm(EZ_WIDGET_MENU_BUTTON,    frame,
                               "Btn1",             "btn1",
                               EZ_LABEL_STRING,    "Menu Button 1",
                               EZ_LABEL_POSITION,  EZ_LEFT,
                               EZ_BORDER_WIDTH,    1,
                               EZ_BORDER_TYPE,     EZ_BORDER_UP,
                               EZ_FOREGROUND,      "red",
                              0);
  menu1 = EZ_CreateSimpleMenu("%Tmenu1|something||||");
  EZ_SetMenuButtonMenu(mbtn1, menu1);
  mbtn2   = EZ_CreateWidgetXrm(EZ_WIDGET_MENU_BUTTON,    frame,
                              "Btn1",             "btn2",
                               EZ_LABEL_STRING,    "Another Menu Button",
                               EZ_FOREGROUND,      "blue",
                               EZ_LABEL_POSITION,  EZ_LEFT,
                               EZ_UNDERLINE,       0,
                               EZ_BORDER_WIDTH,    1,
                               EZ_BORDER_TYPE,     EZ_BORDER_UP,
                               EZ_INDICATOR_TYPE,  EZ_MENU_BUTTON_INDICATOR,
                               EZ_INDICATOR_COLOR, "green",
                               EZ_FOREGROUND,      "blue",
                               EZ_BG_IMAGE_FILE, "bg.gif",
                              0);
  tmenu = menu2 = EZ_CreateSimpleMenu("%Tmenu2|something||||");
  EZ_SetMenuButtonMenu(mbtn2, menu2);
  tmbtn = mbtn2;

  mbtn3   = EZ_CreateWidgetXrm(EZ_WIDGET_MENU_BUTTON,    frame,
                              "Btn1",             "btn3",
                               EZ_LABEL_STRING,    "One more menu button",
                               EZ_PIXMAP_FILE,     "folder.xpm",
                               EZ_LABEL_POSITION,  EZ_LEFT,
                               EZ_BORDER_WIDTH,    1,
                               EZ_BORDER_TYPE,     EZ_BORDER_UP,
                              0);
  menu3 = EZ_CreateSimpleMenu("|||||");
  EZ_SetMenuButtonMenu(mbtn3, menu3);

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

