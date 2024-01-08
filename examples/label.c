#include "EZ.h"

static void quit(EZ_Widget *widget, void *data)
{
  if(getenv("SIMAGE"))
    {
      FILE *fp = fopen("label.ppm", "w");
      if(fp) 
        {
          EZ_SaveWidget2PPM(data, fp);
          fclose(fp);
        }
    }
  EZ_Shutdown();
  exit(0);
}

static char *twoLines = "Multi-line labels may be left, center or right justified. And it may be anchored at the center or the four corners ...";

int main(int ac, char **av)
{
  EZ_Widget *frame, *label1, *label2, *label3, *frame4, *btn;

  EZ_Initialize(ac, av, 0);
  frame  = EZ_CreateWidgetXrm(EZ_WIDGET_FRAME,    NULL,
                              "Labels",           "labels",
                              EZ_FILL_MODE,       EZ_FILL_BOTH,
                              EZ_IPADX,           4,
                              EZ_SIZE,            600, 100,
                              0);
  label1 = EZ_CreateWidgetXrm(EZ_WIDGET_LABEL,    frame,
                              "Label1",           "label1",
                              EZ_LABEL_STRING,    twoLines,
                              EZ_BACKGROUND,      "#cc00cc",
                              0);
  label2 = EZ_CreateWidgetXrm(EZ_WIDGET_LABEL,    frame,
                              "Label2",           "label2",
                              EZ_LABEL_STRING,    twoLines,
                              EZ_BACKGROUND,      "bisque",
                              EZ_JUSTIFICATION,   EZ_RIGHT,
                              EZ_LABEL_POSITION,  EZ_BOTTOM_RIGHT,
                              EZ_BORDER_TYPE,     EZ_BORDER_RIDGE,
                              EZ_BORDER_WIDTH,    2,
                              0);
  label3 = EZ_CreateWidgetXrm(EZ_WIDGET_LABEL,    frame,
                              "Label3",           "label3",
                              EZ_PIXMAP_FILE,     "twotails.xpm",
                              EZ_BORDER_TYPE,     EZ_BORDER_RAISED,
                              EZ_BORDER_WIDTH,    2,
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
