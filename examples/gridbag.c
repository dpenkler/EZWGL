/********************** Example GridBag *********************/
#include "EZ.h"
static void my_exit(void *a, void *b)
{   EZ_Shutdown();   exit(0); }

#define NCOLS     20  /* number of columns in our gridbag */

main(int ac, char **av)
{
  EZ_Widget *gb, *text, *mbtn, *menu, *tmp;

  EZ_Initialize(ac,av,0);

  /* a toplevel gridbag. Extra spaces go to ROW and COLUME 4 */
  gb = EZ_CreateWidget(EZ_WIDGET_FRAME, NULL, 
                       EZ_PADX,               0, 
                       EZ_PADY,               0,        /*idx     min weight pad */
                       EZ_GRID_CONSTRAINS,    EZ_ROW,      1,     300,  10,   0, 
                       EZ_GRID_CONSTRAINS,    EZ_COLUMN, NCOLS-2, 200,  10,   0, 
                       EZ_BACKGROUND,         "grey85",
                       0);
  /* the first row is a menu bar with 3 buttons.
   * The (NCOLS-2)th column which has a weight 10, 
   * this pushes the last menu button to the right edge.
   */
  mbtn = EZ_CreateWidget(EZ_WIDGET_MENU_BUTTON,    gb,
                                                /* X, Y, W, H */
                         EZ_GRID_CELL_GEOMETRY,    0, 0, 1, 1, 
                                                /*   fill_mode,   gravity */
                         EZ_GRID_CELL_PLACEMENT,   EZ_FILL_BOTH, EZ_CENTER, 
                         EZ_LABEL_STRING,          "MButton 1",
                         EZ_PADY,                  0,
                         0);
  menu = EZ_CreateSimpleMenu("Exit%f|||||",my_exit);
  EZ_SetMenuButtonMenu(mbtn, menu);

 
  mbtn = EZ_CreateWidget(EZ_WIDGET_MENU_BUTTON,   gb,
                         EZ_GRID_CELL_GEOMETRY,   1, 0, 1, 1,
                         EZ_GRID_CELL_PLACEMENT,  EZ_FILL_BOTH, EZ_CENTER,
                         EZ_LABEL_STRING,         "MButton 2",
                         EZ_PADY,                  0,
                         0);
  menu = EZ_CreateSimpleMenu("|||||");
  EZ_SetMenuButtonMenu(mbtn, menu);

  mbtn = EZ_CreateWidget(EZ_WIDGET_MENU_BUTTON,   gb,
                         EZ_GRID_CELL_GEOMETRY,   NCOLS-1, 0, 1, 1,
                         EZ_GRID_CELL_PLACEMENT,  EZ_FILL_BOTH, EZ_CENTER,
                         EZ_LABEL_STRING,         "Help",
                         EZ_PADY,                  0,
                         0);
  menu = EZ_CreateSimpleMenu("|||||");
  EZ_SetMenuButtonMenu(mbtn, menu);

  text = EZ_CreateWidget(EZ_WIDGET_TEXT,         gb,
                         EZ_GRID_CELL_GEOMETRY,  0, 1, NCOLS, 3,
                         EZ_EXPAND,              True,
			 EZ_PROPAGATE,           False,
                         EZ_TEXT_BACKGROUND,     "grey95",
                         EZ_SELECTION_BACKGROUND,"yellow",
                         0);
  EZ_DisplayWidget(gb);
  EZ_TextLoadFile(text, "gridbag.c");  
  EZ_EventMainLoop();
}
/********************** Example GridBag *********************/
