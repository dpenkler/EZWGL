
#include "EZ.h"

static void quit(EZ_Widget *widget, void *data)
{
  if(getenv("SIMAGE"))
    {
      FILE *fp = fopen("scroll.ppm", "w");
      if(fp) 
	{
	  EZ_SaveWidget2PPM(data, fp);
	  fclose(fp);
	}
    }
  EZ_Shutdown();
  exit(0);
}
static void noop(EZ_Widget *widget, void *data){ }

int main(int ac, char **av)
{
  EZ_Widget *frame, *scrollbar1, *scrollbar2, *scrollbar12, *scrollbar21;
  EZ_Widget *scrollbar3, *scrollbar4, *frame5, *btn;

  EZ_Initialize(ac, av, 0);
  /*EZ_DisableHighlight();*/
  frame  = EZ_CreateWidgetXrm(EZ_WIDGET_FRAME,    NULL,
                              "Frames",           "frames",
                              EZ_FILL_MODE,       EZ_FILL_BOTH,
                              EZ_IPADX,           4,
                              EZ_GRID_CONSTRAINS, EZ_ROW,    0, 100,  100,   0, 
                              EZ_GRID_CONSTRAINS, EZ_COLUMN, 0, 200,  100,   0, 
                              0);
  scrollbar1= EZ_CreateWidgetXrm(EZ_WIDGET_HORIZONTAL_SCROLLBAR,  frame,
                                 "SCROLLBAR1",           "SCROLLBAR1",
                                 EZ_BACKGROUND,        "#cc00cc",
                                 EZ_GRID_CELL_GEOMETRY,   0, 0, 1, 1,
                                 EZ_GRID_CELL_PLACEMENT,  EZ_FILL_BOTH, EZ_BOTTOM,
                                 EZ_CALLBACK, noop, NULL,
                                 0);
  EZ_UpdateScrollbar(scrollbar1, 100, 10, 20);
  scrollbar2= EZ_CreateWidgetXrm(EZ_WIDGET_HORIZONTAL_SCROLLBAR,  frame,
                                 "SCROLLBAR2",           "SCROLLBAR2",
                                 EZ_SLIDER_STYLE,      1,
                                 EZ_GRID_CELL_GEOMETRY,   0, 1, 1, 1,
                                 EZ_GRID_CELL_PLACEMENT,  EZ_FILL_BOTH, EZ_CENTER,
                                 EZ_CALLBACK, noop, NULL,
                                 0);

  EZ_UpdateScrollbar(scrollbar2, 100, 30, 40);
  scrollbar3= EZ_CreateWidgetXrm(EZ_WIDGET_VERTICAL_SCROLLBAR,  frame,
                                 "SCROLLBAR3",           "SCROLLBAR3",
                                 EZ_GRID_CELL_GEOMETRY,   1, 0, 1, 4,
                                 EZ_GRID_CELL_PLACEMENT,  EZ_FILL_BOTH, EZ_CENTER,
                                 EZ_WIDTH, 0,
                                 EZ_CALLBACK, noop, NULL,
                                 0);

  EZ_UpdateScrollbar(scrollbar3, 60, 40, 20);
  scrollbar4= EZ_CreateWidgetXrm(EZ_WIDGET_VERTICAL_SCROLLBAR,  frame,
                                 "SCROLLBAR4",              "SCROLLBAR4",
                                 EZ_BACKGROUND,        "#00cccc",
                                 EZ_SLIDER_STYLE,          3,
                                 EZ_GRID_CELL_GEOMETRY,   2, 0, 1, 4,
                                 EZ_GRID_CELL_PLACEMENT,  EZ_FILL_BOTH, EZ_CENTER,
                                 EZ_CALLBACK, noop, NULL,
                                 0);

  frame5 = EZ_CreateWidgetXrm(EZ_WIDGET_FRAME,    frame,
                              "Frame5",           "frame5",
                              EZ_BG_IMAGE_FILE,   "marble",
                              EZ_DOCKABLE,        True,
                              EZ_BORDER_TYPE,     EZ_BORDER_RAISED,
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


