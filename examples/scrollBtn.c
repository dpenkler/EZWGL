#include "EZ.h"

static quit(EZ_Widget *widget, void *data)
{
  if(getenv("SIMAGE"))
    {
      FILE *fp = fopen("scrollBtn.ppm", "w");
      if(fp) 
        {
          EZ_SaveWidget2PPM(data, fp);
          fclose(fp);
        }
    }
  EZ_Shutdown();
  exit(0);
}
static void cb(EZ_Widget *w, void *d)
{
  char str[20];
  int c= EZ_GetScrollButtonScrollCount(w);
  sprintf(str, "%d",c);
  EZ_SetEntryString(d, str);
}

main(int ac, char **av)
{
  EZ_Widget *entry, *frame,*frame0, *tmp, *frameA, *btn;

  EZ_Initialize(ac, av, 0);

  frame  = EZ_CreateWidgetXrm(EZ_WIDGET_FRAME, NULL,
                              "frame",                       "frame",
                              EZ_SIZE,                       300, 200,
                              EZ_ORIENTATION,     EZ_VERTICAL,
                              EZ_BORDER_TYPE,     EZ_BORDER_RAISED,
                              EZ_BORDER_WIDTH,    2,
                              0);
  entry = EZ_CreateWidget(EZ_WIDGET_ENTRY, frame, EZ_PROPAGATE, 0, 
                          EZ_TEXT_BACKGROUND, "white", 0);
  
  frame0  = EZ_CreateWidgetXrm(EZ_WIDGET_FRAME,  frame,
                              "frame",                       "frame",
                               EZ_BORDER_TYPE,     EZ_BORDER_RAISED,
                               EZ_BORDER_WIDTH,    2,
                               0);

  tmp  = EZ_CreateWidget(EZ_WIDGET_SCROLL_BUTTON, frame0,
                         EZ_SCROLL_TYPE, EZ_UP,
                         EZ_CALLBACK, cb, entry,
                         0);
  tmp  = EZ_CreateWidget(EZ_WIDGET_SCROLL_BUTTON, frame0,
                         EZ_SCROLL_TYPE, EZ_DOWN,
                         EZ_CALLBACK, cb, entry,
                         0);
  
  tmp  = EZ_CreateWidget(EZ_WIDGET_SCROLL_BUTTON, frame0,
                         EZ_SCROLL_TYPE, EZ_UP_DOWN,
                         EZ_CALLBACK, cb, entry,
                         0);

  tmp  = EZ_CreateWidget(EZ_WIDGET_SCROLL_BUTTON, frame0,
                         EZ_SCROLL_TYPE, EZ_LEFT_RIGHT,
                         EZ_CALLBACK, cb, entry,
                         0);

  tmp  = EZ_CreateWidget(EZ_WIDGET_SCROLL_BUTTON, frame0,
                         EZ_SCROLL_TYPE, EZ_LEFT,
                         EZ_CALLBACK, cb, entry,
                         0);
  tmp  = EZ_CreateWidget(EZ_WIDGET_SCROLL_BUTTON, frame0,
                         EZ_SCROLL_TYPE, EZ_RIGHT,
                         EZ_CALLBACK, cb, entry,
                         0);

  frameA = EZ_CreateWidgetXrm(EZ_WIDGET_FRAME,    frame,
                              "Frame",           "frame",
                             EZ_BG_IMAGE_FILE,   "marble",
                             EZ_DOCKABLE,        True,
                             EZ_BORDER_TYPE,     EZ_BORDER_RAISED,
                             EZ_BORDER_WIDTH,    2,
                             0);
  btn  =   EZ_CreateWidgetXrm(EZ_WIDGET_NORMAL_BUTTON,    frameA,
                              "AButton",           "aButton",
                              EZ_LABEL_STRING,     "Done",
                              EZ_CALLBACK,          quit, frame,
                              0);
  EZ_DisplayWidget(frame);
  EZ_EventMainLoop();
}

