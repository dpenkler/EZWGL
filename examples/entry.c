#include "EZ.h"

static quit(EZ_Widget *widget, void *data)
{
  if(getenv("SIMAGE"))
    {
      FILE *fp = fopen("entry.ppm", "w");
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
  EZ_Widget *frame, *entry1, *entry2, *btn,*frame5 , *tmp;

  EZ_Initialize(ac, av, 0);
  frame  = EZ_CreateWidgetXrm(EZ_WIDGET_FRAME,    NULL,
                              "Entries",           "entries",
                              0);
  tmp =   EZ_CreateWidget(EZ_WIDGET_LABEL, frame,
                          EZ_LABEL_STRING, "User Name:",
                          EZ_GRID_CELL_GEOMETRY, 0,0,1,1,
                          EZ_GRID_CELL_PLACEMENT, EZ_FILL_NONE, EZ_RIGHT,
                          0);
  tmp =   EZ_CreateWidget(EZ_WIDGET_LABEL, frame,
                          EZ_LABEL_STRING, "Password:",
                          EZ_GRID_CELL_GEOMETRY, 0,1,1,1,
                          EZ_GRID_CELL_PLACEMENT, EZ_FILL_NONE, EZ_RIGHT,
                          0);
  entry1= EZ_CreateWidgetXrm(EZ_WIDGET_ENTRY,        frame,
                             "nameEntry",           "NameEntry",
                             EZ_GRID_CELL_GEOMETRY, 1,0,1,1, 
                             EZ_GRID_CELL_PLACEMENT, EZ_FILL_BOTH, EZ_LEFT,
                             EZ_FONT_NAME, "-sony-fixed-medium-r-normal--24-170-100-100-c-120-jisx0201.1976-0",
                             0);
  entry2= EZ_CreateWidgetXrm(EZ_WIDGET_ENTRY,        frame,
                             "passwdEntry",        "PasswdEntry",
                             EZ_GRID_CELL_GEOMETRY, 1,1,1,1, 
                             EZ_GRID_CELL_PLACEMENT, EZ_FILL_BOTH, EZ_LEFT,
                             EZ_FONT_NAME, "-sony-fixed-medium-r-normal--24-170-100-100-c-120-jisx0201.1976-0",
                             0);
  EZ_OnOffSecretEntry(entry2, True);  

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
