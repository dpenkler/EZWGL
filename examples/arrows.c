#include "EZ.h"
  
int main(int argc, char **argv)
{
  EZ_Widget *tmp, *tmpa, *frame;
  int i;

  EZ_Initialize(argc,argv,0);


  frame = EZ_CreateWidget(EZ_WIDGET_FRAME, NULL,
                          EZ_LABEL_STRING, "Arrows",
                          EZ_FOREGROUND, "red",
                          EZ_BG_IMAGE_FILE, "bg.gif",
                          EZ_FILL_MODE, EZ_FILL_BOTH,
                          EZ_GRID_CONSTRAINS,    EZ_ALL_ROWS,      5,     10,  10,   0, 
                          EZ_GRID_CONSTRAINS,    EZ_ALL_COLUMNS,   5,     10,  10,   0, 
                          0);

  for(i = 1; i <= 36; i++)
    {
      char str[10];
      tmp = EZ_CreateWidget(EZ_WIDGET_NW_LABEL, frame,
                            EZ_ORIENTATION,     EZ_VERTICAL,
                            EZ_FILL_MODE,       EZ_FILL_BOTH,
                            EZ_PADX, 8,         EZ_PADY, 8, 
                            EZ_GRID_CELL_GEOMETRY, ((i-1)/6), ((i-1)%6), 1, 1,
                            EZ_GRID_CELL_PLACEMENT, EZ_FILL_BOTH, EZ_CENTER,
                            EZ_BORDER_WIDTH, 1, EZ_BORDER_TYPE, EZ_BORDER_DOWN,
                            0);
      
      tmpa = EZ_CreateWidget(EZ_WIDGET_NW_LABEL, tmp,
                             EZ_ARROW_LABEL,     i,
                             0);
      sprintf(str, "%2d", i);
      tmpa = EZ_CreateWidget(EZ_WIDGET_NW_LABEL, tmp,
                             EZ_LABEL_STRING, str,
                             EZ_SIZE,         0, 0,
                             0);
    }

  EZ_DisplayWidget(frame);
  EZ_EventMainLoop();
}
