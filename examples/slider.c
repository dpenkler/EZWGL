#include "EZ.h"

int main(int ac, char **av)
{
  EZ_Widget *frame, *slider, *frame5, *btn;

  EZ_Initialize(ac, av, 0);
  frame  = EZ_CreateWidget(EZ_WIDGET_FRAME,    NULL,
                           EZ_LABEL_STRING,    "Sliders ...",
                           EZ_IPADY,           4,
                           EZ_GRID_CONSTRAINS, EZ_COLUMN, 0, 200,  100,  0, 

                           0);
  slider= EZ_CreateWidget(EZ_WIDGET_HORIZONTAL_SLIDER,  frame,
                          EZ_BORDER_TYPE,       EZ_BORDER_RAISED,
                          EZ_BORDER_WIDTH,      2,
                          EZ_BACKGROUND,        "#00cccc",
                          EZ_GRID_CELL_GEOMETRY,   0, 0, 1, 1,
                          EZ_GRID_CELL_PLACEMENT,  EZ_FILL_BOTH, EZ_CENTER,
                          EZ_SLIDER_STYLE,         0,
                          0);
  slider= EZ_CreateWidget(EZ_WIDGET_HORIZONTAL_SLIDER,  frame,
                          EZ_BORDER_TYPE,       EZ_BORDER_RAISED,
                          EZ_BORDER_WIDTH,      2,
                          EZ_GRID_CELL_GEOMETRY,   0, 1, 1, 1,
                          EZ_GRID_CELL_PLACEMENT,  EZ_FILL_BOTH, EZ_CENTER,
                          EZ_SLIDER_STYLE,      1,
                          EZ_SLIDER_RANGE,     0.0, 10.0,
                          EZ_SLIDER_RESOLUTION, 0.1,
                          EZ_SHOW_TICK, 10, 10,
                          EZ_SLIDER_DISPLAY_VALUE,  10,
                          0);
  slider=EZ_CreateWidget(EZ_WIDGET_HORIZONTAL_SLIDER,  frame,
                         EZ_BORDER_TYPE,       EZ_BORDER_RAISED,
                         EZ_BORDER_WIDTH,      2,
                         EZ_LABEL_STRING,     "Another Slider",                         
                         EZ_SLIDER_STYLE,      2,
                         EZ_GRID_CELL_GEOMETRY,  0, 2, 1, 1,
                         EZ_GRID_CELL_PLACEMENT,  EZ_FILL_BOTH, EZ_CENTER,
                         0);
  slider=EZ_CreateWidget(EZ_WIDGET_HORIZONTAL_SLIDER,  frame,
                         EZ_BORDER_TYPE,       EZ_BORDER_RAISED,
                         EZ_BORDER_WIDTH,      2,
                         EZ_SLIDER_STYLE,      4,
                         EZ_GRID_CELL_GEOMETRY,  0, 3, 1, 1,
                         EZ_GRID_CELL_PLACEMENT,  EZ_FILL_BOTH, EZ_CENTER,
                         EZ_SHOW_TICK, 4, 10,
                         0);

  slider=EZ_CreateWidget(EZ_WIDGET_HORIZONTAL_SLIDER,  frame,
                         EZ_BORDER_TYPE,       EZ_BORDER_RAISED,
                         EZ_BORDER_WIDTH,      2,
                         EZ_SLIDER_STYLE,      5,
                         EZ_GRID_CELL_GEOMETRY,  0, 3, 1, 1,
                         EZ_GRID_CELL_PLACEMENT,  EZ_FILL_BOTH, EZ_CENTER,
                         EZ_SHOW_TICK, 4, 10,
                         EZ_SLIDER_DISPLAY_VALUE,  4,
                         0);

  slider=EZ_CreateWidget(EZ_WIDGET_HORIZONTAL_SLIDER,  frame,
                          EZ_BACKGROUND,        "#88cccc",
                         EZ_BORDER_TYPE,       EZ_BORDER_RAISED,
                         EZ_BORDER_WIDTH,      2,
                         EZ_SLIDER_STYLE,      5,
                         EZ_GRID_CELL_GEOMETRY,  0, 4, 1, 1,
                         EZ_GRID_CELL_PLACEMENT,  EZ_FILL_BOTH, EZ_CENTER,
                         EZ_SLIDER_DISPLAY_VALUE,  0,
                         0);


  EZ_DisplayWidget(frame);
  EZ_EventMainLoop();
}
