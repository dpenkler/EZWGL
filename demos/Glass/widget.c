/****************** Pixmap Server *************************************/
#include "EZ.h"

Atom  XROOTPMAP_ID;
Pixmap rootBgPixmap = None;
static EZ_LabelPixmap *bigPixmap = NULL;

static int  makeBgPixmap()
{
  Display   *dpy; 
  Window    rootWin;
  int       scrn,depth; 
  Visual    *vis; 
  Atom      aType;
  int       aFormat, ans;
  unsigned long bytesAfter, length;
  unsigned char *data;
  Pixmap   pixmap = None;

  EZ_GetDisplayInfo( &dpy, &scrn, &vis, &depth);
  rootWin = RootWindow(dpy, scrn);
  ans = XGetWindowProperty(dpy,
                           rootWin,
                           XROOTPMAP_ID,
                           0L, 256L,
                           False,
                           AnyPropertyType,
                           &aType,
                           &aFormat,
                           &length,
                           &bytesAfter,
                           &data);
  if((ans == Success) && (aType == XA_PIXMAP))
    pixmap = *((Pixmap *) data);
  if(pixmap != None && pixmap != rootBgPixmap)
    {
      int x, y, w=0, h, bw, depth;
      Window root;
      XGetGeometry(dpy, pixmap, &root, &x,&y,&w,&h, &bw, &depth);
      XGrabServer(dpy);
      if(w) bigPixmap =EZ_CreateBackgroundPixmapsFromXPixmapX(pixmap,0,0,w,h, 80,
							     100,200,200,70);
      XUngrabServer(dpy);
      rootBgPixmap = pixmap;
      return(bigPixmap != NULL);
    }
  return(0);
}

static void eventHandler(EZ_Widget *widget, void *data, int etype, XEvent *xev)
{
  if(xev->type == ConfigureNotify)
    {
      int x, y,w,h;
      EZ_GetWidgetAbsoluteGeometry(widget, &x,&y,&w,&h);
      EZ_ConfigureWidget(widget, 154, 1, -x, -y, 0);
    }
}


static void proptyNotifyHandler(EZ_Widget *widget, void *data, int etype, XEvent *xev)
{
  if(xev->type == PropertyNotify)
    {
      Display   *dpy; 
      Window    rootWin;
      int       scrn,depth; 
      Visual    *vis; 
      EZ_GetDisplayInfo( &dpy, &scrn, &vis, &depth);
      rootWin = RootWindow(dpy, scrn);
      if(xev->xany.window == rootWin)
        {
          makeBgPixmap();
          EZ_ConfigureWidget(data, EZ_BG_PIXMAP, bigPixmap,  0);
        }
    }
}

main(int ac, char **av)
{
  Display   *dpy; 
  Window    rootWin;
  int       scrn,depth; 
  Visual    *vis; 
  EZ_Widget *frame, *text, *tmp;

  EZ_Initialize(ac, av, 0);
  XROOTPMAP_ID = EZ_GetAtom("_XROOTPMAP_ID");
  makeBgPixmap();      

  EZ_GetDisplayInfo( &dpy, &scrn, &vis, &depth);
  rootWin = RootWindow(dpy, scrn);
  XSelectInput(dpy, rootWin, PropertyChangeMask);
  
  frame  = EZ_CreateWidgetXrm(EZ_WIDGET_FRAME,    NULL,
                              "server",           "Server",
                              EZ_FILL_MODE,       EZ_FILL_BOTH,
                              EZ_LABEL_STRING,    "Transparent Widget",
                              EZ_BG_PIXMAP,       bigPixmap,
                              EZ_IPADX,           4,
                              EZ_SIZE,            400, 400,
                              EZ_EVENT_HANDLER,   eventHandler, NULL,
                              0);

  text =  EZ_CreateWidgetXrm(EZ_WIDGET_TEXT,      frame,
                             "text",              "Text",
                             0);
  EZ_TextLoadFile(text, "widget.c");

  EZ_AddEventHandler(NULL, proptyNotifyHandler, frame, 0);
  EZ_DisplayWidget(frame);
  EZ_EventMainLoop();
}
/*************************************************************************/




