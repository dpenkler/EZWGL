/************************** Example 3 ***********************/
#include "EZ.h"                     /* the header file      */
static void grid(EZ_Widget *w, void *d) {EZ_ShowGrid(d);}



int main(int argc, char **argv)
{
  EZ_Widget *frame, *tmp;
  int i, j, k, ncursors;
  Cursor cursors[200];

  EZ_InitializeXrm("Test", NULL, 0, &argc, argv, NULL, 0);

  frame = EZ_CreateWidgetXrm(EZ_WIDGET_FRAME, NULL,  /* a toplevel frame   */
			     "grid", "Grid",         /* name, className    */
			     EZ_LABEL_STRING, "Predefined Cursors",
			     EZ_FOREGROUND, "red",
			     /* EZ_BACKGROUND, "bisque2", */
			     EZ_IPADX,10, EZ_IPADY,10,
			     EZ_PADX,10, EZ_PADY,10,
			     EZ_RUBBER_BAND, 1,
			     EZ_FILL_MODE, EZ_FILL_BOTH, 0);

  for(i = 0, j = 0; i < 200 && j < 300 ; j++)
    {
      Cursor tmpc = EZ_GetCursor(j);
      if(tmpc != None && EZ_GetCursorName(tmpc) != NULL)
	cursors[i++] = tmpc;
    }
  ncursors = i;
  for(i = 0, j = 0, k = 0; i < ncursors; i++)
    {
      char *name = EZ_GetCursorName(cursors[i]);

      tmp = EZ_CreateWidgetXrm(EZ_WIDGET_NORMAL_BUTTON, frame,
			       name, NULL,
			       EZ_CURSOR, cursors[i],
			       EZ_LABEL_STRING, name,
			       EZ_PROPAGATE, 0,
			       EZ_GRID_CELL_GEOMETRY,  j, k, 1, 1,
			       EZ_GRID_CELL_PLACEMENT, EZ_FILL_BOTH, EZ_CENTER,
                               EZ_CALLBACK, grid, frame,
			       NULL);
      k++;
      if(k == 15) {k = 0; j++;}
    }
  EZ_DisplayWidget(frame);
  EZ_EventMainLoop();
}
/************************** Example 3 ***********************/
