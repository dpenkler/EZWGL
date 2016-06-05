/***********************************************************/
#include "EZ.h"

static Display         *theDisplay;            /* display            */
static Window          theRootWindow;          /* root window        */
static int             theScreen, theDepth;    /* scrn # and depth   */
static Visual          *theVisual;             /* the visual         */
static GC              theGC;                  /* a private GC       */
static Atom            XROOTPMAP_ID;           /* the magic Atom     */
static Pixmap          rootBgPixmap = None;    /* root bg PixmapID   */
static Pixmap          spixmap;                /* for quick updating */
static EZ_Pixmap       *bigPixmap = NULL;      /* the EZ pixmap      */
static EZ_Pixmap       *smallPixmap = NULL;    /* another EZ pximap  */
static EZ_Widget       *dialogue, *button;

/*
 * this variable defines the rule to mix bg pixmap with a 
 * specified color      [r g b percentage1, percentage2]
 */
static float  rgbp[6] = { 100.0, 70.0, 50.0, 60.0, 100.0, 0.0}; 
/************************************************************
 *  
 *  Forward declarations 
 */
static void setBtnBg(int);

/************************************************************
 *
 *  This procedure read the root bg pixmap and returns
 *  the pixmap ID or None. The root bg pixmap is registered
 *  as the _XROOTPMAP_ID property of the root window.
 */
  
static Pixmap readRootBgPixmap()
{
  Atom            aType;
  int             aFormat, ans;
  unsigned long   bytesAfter, length;
  unsigned char   *data;
  Pixmap          pixmap = None;

  ans = XGetWindowProperty(theDisplay,
                           theRootWindow,
                           XROOTPMAP_ID,
                           0L, 1L,
                           False,
                           AnyPropertyType,
                           &aType,
                           &aFormat,
                           &length,
                           &bytesAfter,
                           &data);
  if((ans == Success) && (aType == XA_PIXMAP))  pixmap = *((Pixmap *) data);
  return(pixmap);
}

/************************************************************
 *
 *  This procedure create a EZWGL internal Pixmap to
 *  be used for bg tiling 
 */
static EZ_Pixmap *makeBgPixmap(Pixmap pixmap, int force)
{
  EZ_Pixmap       *ezpixmap = NULL;
  if(pixmap != None && (pixmap != rootBgPixmap || force))
    {
      int x, y; unsigned int bw, depth,w,h;
      Window root;
      XGetGeometry(theDisplay, pixmap, &root, &x,&y,&w,&h, &bw, &depth);
      XGrabServer(theDisplay);
      if(w)  ezpixmap =EZ_CreateBackgroundPixmapsFromXPixmapX(pixmap,0,0,w,h, (int)rgbp[3],
                                                              (int)rgbp[0],(int)rgbp[1],
                                                              (int)rgbp[2],(int)rgbp[4]);
      XUngrabServer(theDisplay);
      rootBgPixmap = pixmap;
      return(ezpixmap);
    }
  return(NULL);
}

/********************************************************************************
 * Registered to toplevel widgets. When the window moves, reset tile origin 
 */
static void eventHandler(EZ_Widget *widget, void *data, int etype, XEvent *xev)
{
  if(xev->type == ConfigureNotify)
    {
      int x, y, w, h, tox, toy;
      EZ_GetWidgetAbsoluteGeometry(widget, &x,&y,&w,&h);
      EZ_GetWidgetTileOrigin(widget, &tox, &toy);
      if(tox != -x || toy != -y)
        {
          EZ_ConfigureWidget(widget, EZ_TILE_ORIGIN, 1, -x, -y, 0);
          if((int)data == 0x1)  setBtnBg(1);
        }
    }
}
/********************************************************************************/
static void setBtnBg(int force)
{
  static int  btnX, btnY;  /* absolute position */
  int         x,y,w,h, dirty = 0;
  EZ_Pixmap   *tmp;
  
  EZ_GetWidgetAbsoluteGeometry(button, &x, &y, &w, &h);
  if(bigPixmap != NULL && (x != btnX || y != btnY) )
    {
      /* first update spixmap */
      XSetTile(theDisplay, theGC, rootBgPixmap);
      XSetTSOrigin(theDisplay, theGC, -x, -y);
      if(spixmap == None) spixmap = XCreatePixmap(theDisplay,rootBgPixmap, w, h, theDepth);
      XFillRectangle(theDisplay, spixmap, theGC, 0,0,w,h);
      dirty = 1;
    }
  if(spixmap && (dirty || force))
    {
      /* then update the internal tiling pixmap */
      tmp = EZ_CreateBackgroundPixmapsFromXPixmapX(spixmap,0,0,w,h, (int)rgbp[3],
                                                   (int)rgbp[0],(int)rgbp[1],
                                                   (int)rgbp[2],(int)rgbp[4]);
      /* and finally update the bg tile of button */
      if(smallPixmap == NULL) 
        smallPixmap = tmp;
      else 
        {
          EZ_CopyLabelPixmaps(smallPixmap, tmp);
          EZ_FreeLabelPixmap(tmp);
        }
      btnX = x;  btnY = y;
      EZ_ConfigureWidget(button, EZ_TILE_ORIGIN, 1, 0, 0,
                         EZ_BG_PIXMAP, smallPixmap, 0);
    }
}
/********************************************************************************
 *
 *  Registered to NULL (events not associated to EZ widgets)
 *  We select PropertyNotify events for the root window.
 *  Whenever the root bg pixmap changes, we update our toplevel widgets.
 */
static void redisplayWidget(EZ_Widget *widget, void *data)
{
  if(EZ_GetParentWidget(widget) == NULL && EZ_WidgetIsViewable(widget))
    EZ_DisplayWidget(widget);
}
static void proptyNotifyHandler(EZ_Widget *widget, void *data, int etype, XEvent *xev)
{
  if(xev->type == PropertyNotify)
    {
      if( (xev->xproperty.window == theRootWindow) && (xev->xproperty.atom == XROOTPMAP_ID) )
        {
          EZ_Pixmap *tmp;
          if((tmp = makeBgPixmap(readRootBgPixmap(), 0)) != NULL)
            {
              EZ_CopyLabelPixmaps(bigPixmap,tmp);
              EZ_FreeLabelPixmap(tmp);
              EZ_TraverseAllWidgets(redisplayWidget, NULL);
              setBtnBg(1);
            }
        }
    }
}
/********************************************************************************
 *
 * slider callback.
 */
static void sliderCB(EZ_Widget *slider, void *data)
{
  int          x,y,w,h;
  char         str[16];
  float        f = EZ_GetSliderValue(slider);
  int          i = EZ_GetWidgetIntData(slider);

  rgbp[i] = f;
  if(i < 3)
    {
      sprintf(str, "%03d", (int)f);
      EZ_ConfigureWidget(slider, EZ_LABEL_STRING, str, 0);
      sprintf(str, "#%02x%02x%02x",(int)rgbp[0], (int)rgbp[1], (int)rgbp[2]);
      EZ_ConfigureWidget(data, EZ_FOREGROUND, str, 0);
    }
  setBtnBg(1);
  rgbp[5] = 1.0;
}
/********************************************************************************
 *
 * Button callback.
 */
static void btnCB(EZ_Widget *slider, void *data)
{
  EZ_Pixmap *tmp;
  if(rgbp[5] == 0.0) return;
  if((tmp = makeBgPixmap(rootBgPixmap, 1)) != NULL)
    {
      rgbp[5] = 0.0;
      EZ_CopyLabelPixmaps(bigPixmap,tmp);
      EZ_FreeLabelPixmap(tmp);
      EZ_TraverseAllWidgets(redisplayWidget, NULL);
      setBtnBg(1);

    }  
}
/********************************************************************************/
static void closeCB(EZ_Widget *widget, void *data) { EZ_HideWidget(data);}
/********************************************************************************/

void makeDialogue()
{
  EZ_Widget     *slider, *glass, *btn;
  
  dialogue =    EZ_CreateWidgetXrm(EZ_WIDGET_FRAME,       NULL,
                                   "dialogue",           "Dialogue",
                                   EZ_LABEL_STRING,      "Glass ...",
                                   EZ_EVENT_HANDLER,      eventHandler, (void *) 0x1,
                                   EZ_SIZE,               400, 300,
                                   EZ_IPADX,              10,
                                   EZ_IPADY,              10,
                                   EZ_BG_PIXMAP,          bigPixmap,
				   EZ_GRID_CONSTRAINS,    EZ_COLUMN, 0, 100, 100, 0,
				   EZ_GRID_CONSTRAINS,    EZ_COLUMN, 1, 100, 100, 0,
				   EZ_GRID_CONSTRAINS,    EZ_ROW,    1, 100, 100, 0,
				   EZ_GRID_CONSTRAINS,    EZ_ROW,    2, 10, 0,   0,
				   EZ_GRID_CONSTRAINS,    EZ_ROW,    3, 10, 0,   0,
                                   0);

  button =      EZ_CreateWidget(EZ_WIDGET_NORMAL_BUTTON,  dialogue,
                                EZ_SIZE,                  128, 128,
                                EZ_BORDER_WIDTH,          4,
                                EZ_PROPAGATE,             0,
                                EZ_LABEL_STRING,          "Apply",
                                EZ_FOREGROUND,            "white",
                                EZ_FONT_NAME,             "-adobe-helvetica-bold-r-normal-*-*-240-*-*-*-*-*-*",
                                EZ_CALLBACK,              btnCB, dialogue,
                                EZ_GRID_CELL_GEOMETRY,    0, 1, 1, 1,
                                0);
  
  btn =         EZ_CreateWidget(EZ_WIDGET_NORMAL_BUTTON,  dialogue,
                                EZ_PROPAGATE,             0,
                                EZ_LABEL_STRING,          "Close",
                                EZ_UNDERLINE,              0,
                                EZ_FOREGROUND,            "red",
                                EZ_CALLBACK,              closeCB, dialogue,
                                EZ_GRID_CELL_GEOMETRY,    0, 3, 2, 1,
                                0);

  glass =       EZ_CreateWidget(EZ_WIDGET_FRAME,          dialogue,
                                EZ_LABEL_STRING,          "Glass color",
                                EZ_FILL_MODE,             EZ_FILL_BOTH,
                                EZ_GRID_CELL_GEOMETRY,    1, 0, 1, 2,
                                EZ_GRID_CELL_PLACEMENT,   EZ_FILL_BOTH, EZ_CENTER, 
                                0);

  slider = EZ_CreateWidget(EZ_WIDGET_VERTICAL_SLIDER,  glass,
                           EZ_SLIDER_RESOLUTION,       1.0,
                           EZ_PADX,                    0,
                           EZ_PADY,                    0,
                           EZ_FONT_NAME,               "-*-lucidatypewriter-bold-r-*-*-12-*-*-*-*-*-*-*",
                           EZ_SLIDER_RANGE,            0.0, 255.0,
                           EZ_SLIDER_INIT_VALUE,       rgbp[0],
                           EZ_FOREGROUND,              "red",
                           EZ_SLIDER_DISPLAY_VALUE,    0,
                           EZ_LABEL_STRING,            "red",
                           EZ_CALLBACK,                sliderCB, glass,
                           EZ_CLIENT_INT_DATA,         0,
                           EZ_WIDTH,                   0,
                           0);

  slider = EZ_CreateWidget(EZ_WIDGET_VERTICAL_SLIDER, glass,
                           EZ_SLIDER_RESOLUTION,      1.0,
                           EZ_PADX,                   0,
                           EZ_PADY,                   0,
                           EZ_FONT_NAME,              "-*-lucidatypewriter-bold-r-*-*-12-*-*-*-*-*-*-*",
                           EZ_SLIDER_RANGE,           0.0, 255.0,
                           EZ_SLIDER_INIT_VALUE,      rgbp[1],
                           EZ_FOREGROUND,             "green",
                           EZ_LABEL_STRING,           "grn",
                           EZ_SLIDER_DISPLAY_VALUE,   0,
                           EZ_CLIENT_INT_DATA,        1,
                           EZ_CALLBACK,               sliderCB, glass,
                           EZ_WIDTH,                  0,
                           0);

  slider = EZ_CreateWidget(EZ_WIDGET_VERTICAL_SLIDER, glass,
                           EZ_SLIDER_RESOLUTION,      1.0,
                           EZ_PADX,                   0,
                           EZ_PADY,                   0,
                           EZ_FONT_NAME,              "-*-lucidatypewriter-bold-r-*-*-12-*-*-*-*-*-*-*",
                           EZ_SLIDER_RANGE,           0.0, 255.0,
                           EZ_SLIDER_INIT_VALUE,      rgbp[2],
                           EZ_FOREGROUND,             "blue",
                           EZ_LABEL_STRING,           "blu",
                           EZ_SLIDER_DISPLAY_VALUE,   0,
                           EZ_CLIENT_INT_DATA,        2,
                           EZ_CALLBACK,               sliderCB, glass,
                           EZ_WIDTH,                  0,
                           0);

  
  slider = EZ_CreateWidget(EZ_WIDGET_HORIZONTAL_SLIDER, dialogue,
                           EZ_SLIDER_RESOLUTION,      1.0,
                           EZ_PADX,                   0,
                           EZ_PADY,                   0,
                           EZ_FONT_NAME,              "-*-lucidatypewriter-bold-r-*-*-12-*-*-*-*-*-*-*",
                           EZ_SLIDER_RANGE,           0.0, 100.0,
                           EZ_SLIDER_INIT_VALUE,      rgbp[3],
                           EZ_SLIDER_DISPLAY_VALUE,   0,
                           EZ_LABEL_STRING,           "Transparency",
                           EZ_CLIENT_INT_DATA,         3,
                           EZ_CALLBACK,                sliderCB, NULL,
                           EZ_GRID_CELL_GEOMETRY,      0, 2, 1, 1,
                           EZ_GRID_CELL_PLACEMENT,     EZ_FILL_BOTH, EZ_CENTER, 
                           0);
  slider = EZ_CreateWidget(EZ_WIDGET_HORIZONTAL_SLIDER, dialogue,
                           EZ_SLIDER_RESOLUTION,      1.0,
                           EZ_PADX,                   0,
                           EZ_PADY,                   0,
                           EZ_FONT_NAME,              "-*-lucidatypewriter-bold-r-*-*-12-*-*-*-*-*-*-*",
                           EZ_SLIDER_RANGE,           0.0, 100.0,
                           EZ_SLIDER_INIT_VALUE,      rgbp[4],
                           EZ_SLIDER_DISPLAY_VALUE,   0,
                           EZ_LABEL_STRING,           "Darken Factor",
                           EZ_CLIENT_INT_DATA,         4,
                           EZ_CALLBACK,                sliderCB, NULL,
                           EZ_GRID_CELL_GEOMETRY,      1, 2, 1, 1,
                           EZ_GRID_CELL_PLACEMENT,     EZ_FILL_BOTH, EZ_CENTER, 
                           0);
}

/********************************************************************************
 * test
 */
static char *transparentResource = "*tileOrigin: 2 0 0";

main(int ac, char **av)
{
  EZ_Widget *frame, *text, *tmp;
  unsigned long valuemask; 
  XGCValues     values;  
  XrmDatabase   dbt;

  EZ_InitializeXrm("transparent",        /* class name            */
		   NULL,                 /* cmd line options      */
		   0,                    /* # of cmd line options */
		   &ac,                  /* arg count, in and out */
		   av,                   /* cmd args, in and out  */
		   transparentResource,  /* fallback resources    */
		   0);                   /* useless junk          */

  XROOTPMAP_ID = EZ_GetAtom("_XROOTPMAP_ID");
  EZ_GetDisplayInfo( &theDisplay, &theScreen, &theVisual, &theDepth);
  theRootWindow = RootWindow(theDisplay, theScreen);
  valuemask = GCFillStyle;
  values.fill_style = FillTiled;
  theGC =  XCreateGC(theDisplay, theRootWindow, valuemask, &values);  
  XSelectInput(theDisplay, theRootWindow, PropertyChangeMask);
  bigPixmap = makeBgPixmap(readRootBgPixmap(), 0); 

  dbt = XrmGetStringDatabase(transparentResource);
  XrmCombineDatabase(dbt, EZ_GetResourceDatabase(), 1);      

  makeDialogue();
  
  EZ_AddEventHandler(NULL, proptyNotifyHandler, NULL, 0);
  EZ_DisplayWidget(dialogue); 
  EZ_EventMainLoop();
}
/*************************************************************************/


