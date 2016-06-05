#include "EZ.h"
static void quit(EZ_Widget *w,  void *d) { EZ_Shutdown(); exit(0);}

main(int ac, char **av)
{
  EZ_Widget *frame, *btn;
  EZ_Pixmap *shapeMask;

  EZ_Initialize(ac, av, 0);

  /* make a shape mask */
  {
    EZ_Item   *fig;
    XPoint    xpoints[200];
    XGCValues  gcvalues;
    unsigned   long gcmask;
    GC         gc;
    int i; double x, dx = 0.0628;


    fig = EZ_CreateItem(EZ_FIG_ITEM, 
                        EZ_PADX, 0, EZ_PADY, 0,
                        EZ_BORDER_WIDTH, 0,
                        0);
    gcvalues.foreground = 1L;
    gcvalues.background = 0L;
    gcvalues.font = EZ_GetFontFromId(1)->fid;
    gc = EZ_GetGC( GCForeground|GCBackground|GCFont, &gcvalues);
    for(x=0.0, i = 0; i < 100; i++)
      {
	x += dx;
	xpoints[i].x = (int)(80.0 * cos(x) * sin(0.5*x));
	xpoints[i].y = (int)(60.0 * sin(x) * sin(0.5*x));
      }
    EZ_FigItemAddPolygon(fig, gc, xpoints, 100, CoordModeOrigin);
    EZ_FigItemAddString(fig,  gc, "A Shaped Window", 15, EZ_CENTER, -70, 50);
    shapeMask = EZ_CreateShapeMaskFromFigure(fig);
    EZ_DestroyItem(fig);
    EZ_FreeGC(gc);
  }

  frame  = EZ_CreateWidgetXrm(EZ_WIDGET_FRAME,    NULL,
                              "Frame",           "frame",
                              EZ_IPADX,           4,
                              EZ_SHAPE_PIXMAP,    shapeMask,
                              0);
  btn  =   EZ_CreateWidgetXrm(EZ_WIDGET_NORMAL_BUTTON,    frame,
                              "AButton",           "aButton",
                              EZ_LABEL_STRING,     "Done",
                              EZ_CALLBACK,  quit, NULL,
                              0);
  
  EZ_DisplayWidget(frame);
  EZ_EventMainLoop();
}

