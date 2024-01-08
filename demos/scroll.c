/***********************************************************************
 *
 * This file implements scrolled window, a composite widget
 *
 *  
 */
#include "EZ.h"

int  RegisterScrolledWidget(char *iname, char *cname);
EZ_Widget *getScrolledWidget MY_ANSIARGS((EZ_Widget *sww));

/***********************************************************************/
static EZ_Widget *createScrolledWindow MY_ANSIARGS((EZ_Widget *parent, char *i, char *c));
static void  motionCB   MY_ANSIARGS((EZ_Widget *w, void *data));
static void  vscrollCB  MY_ANSIARGS((EZ_Widget *w, void *data));
static void  hscrollCB  MY_ANSIARGS((EZ_Widget *w, void *data));
static void  destroyCB  MY_ANSIARGS((EZ_Widget *w, void *data));
static void  eHandler   MY_ANSIARGS((EZ_Widget *w, void *data, int et, XEvent *xevent));
static void  eHandlerF  MY_ANSIARGS((EZ_Widget *w, void *data, int et, XEvent *xevent));
/***********************************************************************
 *
 */
typedef struct {
  EZ_Widget *tframe,  *mframe0, *mframe;
  EZ_Widget *vscroll, *hscroll;
  EZ_Widget *freelabel;
} swwRecord;

/***********************************************************************/
EZ_Widget *createScrolledWindow(EZ_Widget *parent, char *iname, char *cname)
{
  EZ_Widget *frame, *maskFrame0, *maskFrame, *sww, *hscroll, *vscroll;
  swwRecord *record = (swwRecord *)malloc(sizeof(swwRecord));

  frame = EZ_CreateWidgetXrm(EZ_WIDGET_FRAME, parent, 
                             iname, cname,
                             EZ_PADX, 0, EZ_PADY, 0,
                             EZ_IPADX, 0, EZ_IPADY, 0,
                             EZ_GRID_CONSTRAINS, EZ_COLUMN, 0, 60,  1000,  0,  
                             EZ_GRID_CONSTRAINS, EZ_COLUMN, 1, 0,    0,   0,                      
                             EZ_GRID_CONSTRAINS, EZ_ROW,    0, 60,  1000,  0,  
                             EZ_GRID_CONSTRAINS, EZ_ROW,    1, 0,    0,   0,  
                             EZ_DESTROY_CALLBACK, destroyCB, record,
                             EZ_CLIENT_PTR_DATA,  record,
                             NULL);
  maskFrame0 = EZ_CreateWidgetXrm(EZ_WIDGET_FRAME, frame,
                                  "MaskFrame0", "maskFrame0",
                                  EZ_PADX, 0, EZ_PADY, 0,
                                  EZ_BORDER_WIDTH,     2,
                                  EZ_BORDER_TYPE,      EZ_BORDER_SUNKEN,
                                  EZ_GRID_CELL_GEOMETRY, 0, 0, 1, 1,
                                  EZ_GRID_CELL_PLACEMENT, EZ_FILL_BOTH, EZ_CENTER, 
                                  EZ_FILL_MODE, EZ_FILL_BOTH,
                                  EZ_PROPAGATE,     False,
                                  EZ_EXPAND, 1,
                                  0);
  maskFrame = EZ_CreateWidgetXrm(EZ_WIDGET_FRAME, maskFrame0,
                                 "MaskFrame", "maskFrame",
                                 EZ_PADX, 0, EZ_PADY, 0,
                                 EZ_BORDER_WIDTH,     0,
                                 EZ_PROPAGATE,     False,
                                 EZ_EVENT_HANDLER, eHandlerF, record,
                                 0);
  sww = EZ_CreateWidget(EZ_WIDGET_FREE_LABEL, maskFrame,
                        EZ_SIZE,     0, 0,
                        EZ_PADX, 0, EZ_PADY, 0,
                        EZ_BORDER_WIDTH, 0,
                        EZ_MOTION_CALLBACK, motionCB, record,
                        EZ_EVENT_HANDLER,   eHandler, record,
                        EZ_PROPAGATE,     False,
                        0);
  hscroll = EZ_CreateWidget(EZ_WIDGET_HORIZONTAL_SCROLLBAR, frame,
                            EZ_GRID_CELL_GEOMETRY, 0, 1, 1, 1,
                            EZ_GRID_CELL_PLACEMENT, EZ_FILL_BOTH, EZ_CENTER, 
                            EZ_CALLBACK, hscrollCB, record,
                            EZ_EXPAND,    True,
                            0);
  vscroll = EZ_CreateWidget(EZ_WIDGET_VERTICAL_SCROLLBAR, frame,
                            EZ_GRID_CELL_GEOMETRY, 1, 0, 1, 1,
                            EZ_GRID_CELL_PLACEMENT, EZ_FILL_BOTH, EZ_CENTER, 
                            EZ_CALLBACK, vscrollCB, record,
                            EZ_EXPAND,    True,
                            0);
  record->tframe = frame;
  record->mframe0 = maskFrame0;
  record->mframe = maskFrame;
  record->hscroll = hscroll;
  record->vscroll = vscroll;
  record->freelabel = sww;

  return(frame);
  return(NULL);
}
/*********************************************************************/
EZ_Widget *getScrolledWidget(EZ_Widget *swww)
{
  if(swww)
    {
      swwRecord *record = (swwRecord *)EZ_GetWidgetPtrData(swww);
      return(record->freelabel);
    }
  return(NULL);
}
/*********************************************************************/
static void destroyCB(EZ_Widget *widget, void *data)
{  (void) free((void *) data); }
/*********************************************************************/
static void  eHandler(EZ_Widget *w, void *data, int et, XEvent *event)
{
  if(event->type == MapNotify)
    { motionCB(NULL, data);}
}
static void  eHandlerF(EZ_Widget *w, void *data, int et, XEvent *event)
{
  if(event->type == ConfigureNotify)
    { motionCB(NULL, data); }
}

/*********************************************************************/

static void motionCB(EZ_Widget *widget, void *data)
{
  int fx,fy,fw,fh,w,h, reconfig = 0;
  swwRecord *record = (swwRecord *)data;

  EZ_GetWidgetDimension(record->mframe, &w, &h);
  EZ_GetWidgetGeometry(record->freelabel, &fx, &fy, &fw,&fh);
  if(fx < w - fw) { reconfig = 1; fx = w-fw;}
  if(fx > 0) {reconfig = 1; fx = 0; }
  if(fy < h - fh) { reconfig = 1; fy = h-fh;}
  if(fy > 0) {fy = 0; reconfig = 1; }

  EZ_UpdateScrollbar(record->hscroll, fw, w, fx);
  EZ_UpdateScrollbar(record->vscroll, fh, h, fy);
  if(reconfig && widget) 
    EZ_MoveWidgetWindow(widget,fx,fy);
}
/*********************************************************************/
static void vscrollCB(EZ_Widget *widget, void *data)
{
  swwRecord *record = (swwRecord *)data;
  int       w, h, start, maxsize, pagesize; 
  int       fx,fy,fw,fh; 

  EZ_GetWidgetDimension(record->mframe, &w, &h);
  EZ_GetWidgetGeometry(record->freelabel, &fx, &fy, &fw,&fh);
  EZ_GetScrollbarState(record->vscroll, &maxsize, &pagesize, &start);
  if(pagesize != h)
    {
      EZ_UpdateScrollbar(record->vscroll, fh, h, 0);
      EZ_GetScrollbarState(record->vscroll, &maxsize, &pagesize, &start);
    }
  EZ_MoveWidgetWindow(record->freelabel,fx,start);
}
/*********************************************************************/
static void hscrollCB(EZ_Widget *widget, void *data)
{
  swwRecord *record = (swwRecord *)data;
  int       w, h, start, maxsize, pagesize;
  int       fx,fy,fw,fh; 

  EZ_GetWidgetDimension(record->mframe, &w, &h);
  EZ_GetScrollbarState(record->hscroll, &maxsize, &pagesize, &start);
  EZ_GetWidgetGeometry(record->freelabel, &fx, &fy, &fw,&fh);
  if(pagesize != w)
    {
      EZ_UpdateScrollbar(record->hscroll, fw, w, 0);
      EZ_GetScrollbarState(record->hscroll, &maxsize, &pagesize, &start);
    }
  EZ_MoveWidgetWindow(record->freelabel,start, fy);
}
/*********************************************************************/
int  RegisterScrolledWidget(char *iname, char *cname)
{
  int type = EZ_RegisterCompositeWidget(iname?iname:"scrollWindow", 
					cname?cname:"ScrollWindow",
                                        createScrolledWindow);
  return(type);
}
/*********************************************************************/
int main(int ac, char **av)
{
  EZ_Widget *frame;
  EZ_Widget *scrollWin;
  int scrollWinType;

  EZ_Initialize(ac,av,0);
  scrollWinType =  RegisterScrolledWidget("scrollWin", "ScrollWin");
  frame =       EZ_CreateWidget(EZ_WIDGET_FRAME, NULL,
                                EZ_ORIENTATION, EZ_VERTICAL,
                                EZ_FILL_MODE, EZ_FILL_BOTH,
                                0);
  scrollWin =  EZ_CreateWidget(scrollWinType, frame, 0);
  {
    int i, j; char str[32];
    EZ_Widget  *tmp, *sww = getScrolledWidget(scrollWin);
    for(i = 0; i < 10; i ++)
      {
        for(j = 0; j < 10; j++)      
          {
            sprintf(str, "button %d %d", i, j);
            tmp = EZ_CreateWidget(EZ_WIDGET_NORMAL_BUTTON, sww,
                                  EZ_GRID_CELL_GEOMETRY, i, j, 1, 1,
                                  EZ_LABEL_STRING,  str,
                                  EZ_FOCUS_PAD,     2,
                                  0);
          }
      }
  }
  EZ_DisplayWidget(frame);
  EZ_EventMainLoop();
}
