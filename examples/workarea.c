/************************** WorkArea Items ***********************/
#include "EZ.h"
#include <string.h>

static EZ_Widget  *workarea;

static char stippleBits[] = {0x01, 0x04, 0x01, 0x04};

static void  eHandler   MY_ANSIARGS((EZ_Widget *w, void *data, int et, XEvent *xevent));

static void itemCB(EZ_Item *item, void *data)
{
  EZ_WorkAreaScaleItem(workarea, item, 0, 1.0, -1.0, 0, 0, 0);  
}


EZ_Widget *ruler, *fff;

int main(int argc, char **argv)
{
  EZ_Item    *items[20];
  EZ_Bitmap  *bitmap, *pixmap;
  Pixmap     stipple;
  XPoint     xpoints[200];
  XPoint pts[4];
  unsigned   long gcmask;
  XGCValues  gcvalues;
  unsigned   long red, green, blue, yellow, cyan, magenta;
  GC         gc;
  
  EZ_Initialize(argc,argv,0); 

  printf("===%s====\n",EZ_LibraryVersion());
  red     = EZ_AllocateColorFromName("red");
  green   = EZ_AllocateColorFromName("green");
  blue    = EZ_AllocateColorFromName("blue");
  yellow  = EZ_AllocateColorFromName("yellow");
  cyan    = EZ_AllocateColorFromName("cyan");
  magenta = EZ_AllocateColorFromName("magenta");
  pixmap  = EZ_CreateLabelPixmapFromImageFile("ez.gif");
  bitmap  = EZ_CreateLabelPixmapFromXBitmapFile("woman");
  stipple = XCreateBitmapFromData(EZ_GetDisplay(),
				  RootWindow(EZ_GetDisplay(), EZ_GetScreen()),
				  stippleBits, 4, 4);
  pts[0].x = -20, pts[0].y = 0;
  pts[1].x = 100, pts[1].y = 0;
  pts[2].x = 0,   pts[2].y = -20;
  pts[3].x = 0,   pts[3].y = 100;

  /* a FIG item, with a rectangle and a filled arc */
  gcvalues.foreground = red;
  gc = EZ_GetGC( GCForeground, &gcvalues);
  items[0] = EZ_CreateItem(EZ_FIG_ITEM,
			   EZ_CALLBACK,    itemCB, NULL,
                           EZ_BACKGROUND,  "white",
			   NULL);
  EZ_FigItemAddFilledRectangle(items[0], gc, 0,0,90, 80);
  gcvalues.foreground = green;
  gc = EZ_GetGC( GCForeground, &gcvalues);
  EZ_FigItemAddFilledArc(items[0], gc, 20, 20, 50, 50, 0, 300*64);
  EZ_FigItemAddLines(items[0], gc, pts, 4, CoordModeOrigin);
  /* a FIG item, a stippled polygon */
  xpoints[0].x = 20; xpoints[0].y = 0;
  xpoints[1].x = 60, xpoints[1].y = 0;
  xpoints[2].x = 80, xpoints[2].y = 60;
  xpoints[3].x = 0,  xpoints[3].y = 60;
  xpoints[4].x = 20, xpoints[4].y = 0;

  gcvalues.foreground = blue;
  gcvalues.background = yellow;
  gcvalues.stipple = stipple;
  gcvalues.fill_style = FillOpaqueStippled;
  items[1] =  EZ_CreateItem(EZ_FIG_ITEM, 
			    EZ_CALLBACK,    itemCB, NULL,
			    NULL);
  gcmask = GCForeground|GCBackground|GCStipple|GCFillStyle;
  gc = EZ_GetGC(gcmask, &gcvalues);
  EZ_FigItemAddPolygon(items[1], gc, xpoints, 5, CoordModeOrigin);

  /* FIG Item, lines */
  gcvalues.foreground = magenta;
  gcvalues.line_width = 8;
  items[2] =  EZ_CreateItem(EZ_FIG_ITEM, 
			    EZ_CALLBACK,    itemCB, NULL,
			    NULL);
  gcmask = GCForeground|GCLineWidth|GCFillStyle;
  gc = EZ_GetGC(gcmask, &gcvalues);
  EZ_FigItemAddLines(items[2], gc, xpoints, 5, CoordModePrevious);

  /* a LABEL item */
  items[3]= EZ_CreateItem(EZ_LABEL_ITEM,
			  EZ_LABEL_STRING, "Label Item",
			  EZ_PIXMAP_FILE, "twotails.xpm",
			  EZ_ORIENTATION, EZ_VERTICAL,
			  0);
  /* WIDGET Item */
  {
    EZ_Widget *slider = EZ_CreateWidget(EZ_WIDGET_VERTICAL_SLIDER, NULL,
					EZ_LABEL_STRING, "A Widget",
					EZ_IPADY, 10,
					EZ_SLIDER_WIDTH, 16,
					EZ_HEIGHT, 200,
					NULL);
    items[4]= EZ_CreateItem(EZ_WIDGET_ITEM,
			    EZ_ITEM_WIDGET_WIDGET, slider,
			    NULL);
  }
  /* FIG item, text */
  {
    static char *str = "This little piggy went to market\nThis little piggy stayed home";
    EZ_Pixmap *pix;
    EZ_FigPiece *tmp;
    gcvalues.foreground = magenta;
    gcvalues.font = EZ_GetFontFromName("-adobe-helvetica-bold-r-normal-*-*-180-*-*-*-*-*-*")->fid;
    gcmask = GCForeground|GCFont;
    items[5] =  EZ_CreateItem(EZ_FIG_ITEM, 
			      EZ_CALLBACK,    itemCB, NULL,
			      NULL);
    gc = EZ_GetGC(gcmask, &gcvalues);
    tmp = EZ_FigItemAddString(items[5], gc, str, strlen(str), EZ_CENTER, 10, 10);
    pix = EZ_CreateLabelPixmapFromXpmFile("button_pig.xpm");
    if(pix)
      {
	int x1,x2,y1,y2, w, h;
	EZ_GetFigPieceBoundingBox(tmp, &x1, &y1, &x2, &y2);
	EZ_GetLabelPixmapInfo(pix, NULL, NULL, &w, &h);
	EZ_FigItemAddPixmap(items[5], gc, pix, 0, 0, w, h, x2+1, y1 +(y2-y1-h)/2);
      }
  }
  gcvalues.foreground = 0;
  gc = EZ_GetGC(GCForeground, &gcvalues);
  items[6] =  EZ_CreateItem(EZ_FIG_ITEM, NULL);
  EZ_FigItemAddPixmap(items[6], gc, bitmap, 0, 0, 100, 100, 0, 0);
  
  /* FIG item, polygon */
  {
    int i; double x, dx = 0.1;
    items[7] =  EZ_CreateItem(EZ_FIG_ITEM, 
			      EZ_CALLBACK,    itemCB, NULL,
			      NULL);
    for(x=0.0, i = 0; i < 100; i++)
      {
	x += dx;
	xpoints[i].x = i+i;
	xpoints[i].y = (int)(60.0* sin(x));
      }
    gcvalues.foreground = green;
    gc = EZ_GetGC(gcmask, &gcvalues);
    EZ_FigItemAddPolygon(items[7], gc, xpoints, 100, CoordModeOrigin);
  }

  /* FIG item, pximap */
  items[8] =  EZ_CreateItem(EZ_FIG_ITEM, 
			    EZ_CALLBACK,    itemCB, NULL,
			    NULL);
  EZ_FigItemAddPixmap(items[8], gc, pixmap, 0, 0, 100, 100, 0, 0);

  fff = EZ_CreateWidgetXrm(EZ_WIDGET_FRAME, NULL,
                           "fff", "FFF",
                           EZ_PADX, 0, EZ_PADY, 0,
                           EZ_ORIENTATION, EZ_VERTICAL,
                           EZ_FILL_MODE, EZ_FILL_BOTH,
                           EZ_SIZE,          400, 400,
                           0);
  ruler = EZ_CreateWidget(EZ_WIDGET_HORIZONTAL_RULER, fff,
                          EZ_EXPAND, 1,
                          EZ_BORDER_WIDTH, 0,
                          0);

  workarea = EZ_CreateWidget(EZ_WIDGET_WORK_AREA, fff,
			     EZ_RUBBER_BAND,   1,
			     EZ_ITEM_MOVABLE,  True,
			     EZ_IPADX, 16, EZ_IPADY, 16,
                              EZ_IMAGE_FILE,  "guilin.gif",
                             EZ_EVENT_HANDLER, eHandler, NULL,
			     0);

  
  EZ_WorkAreaInsertItems(workarea, items, 9);
  EZ_DisplayWidget(fff);
  /* after the items are displayed, disable the default geom manager */
  EZ_SetWorkAreaGeometryManager(workarea, NULL, NULL); 
  EZ_EventMainLoop();
}
/********************************************************/

static void  eHandler(EZ_Widget *w, void *data, int et, XEvent *event)
{
  if(event->type == MotionNotify)
    {
      int x = event->xmotion.x;
      int y = event->xmotion.y;
      EZ_WidgetNotifyRuler(ruler, x, y, w);
    } 
}
/********************************************************/
