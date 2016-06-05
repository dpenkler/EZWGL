/*********************************************************************
 *
 *         EZWGL, the EZ Widget and Graphics Library
 *
 * *             Copyright (C) 1996, 1997, 1998  Maorong Zou
 *  
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the Free
 * Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 **********************************************************************/
/*
 *  June 1996.  Beta Release.
 *  Sept 1996.  Release Version 1.0
 *  Dec 1996.  Release Version 1.1 Beta
 *  April 1997.  Release Version 1.2
 *  November 1997.  Release Version 1.3
 *  November 1998.  Release Version 1.4
 *  December 1999.  Release Version 1.50
 */
/*****************************************************************
 ***                                                           ***
 ***              Widget Led.                                  ***
 ***                                                           ***
 *****************************************************************/
#define _EZ_WIDGET_LED_C_
#include "EZ_Widget.h"

/*********************************************************************
 * 
 *  Functions implemented in this file:
 */
int   EZ_QueueLedAction MY_ANSIARGS((EZ_Widget *led, int type, int delay, int count,
				   char *msg,  char *clr));
void  EZ_RemoveLedAction MY_ANSIARGS((EZ_Widget *led, int action));
void  EZ_SetLedColorFunction MY_ANSIARGS((EZ_Widget *led, EZ_LedPixelColorF f));
void  EZ_SetLedString MY_ANSIARGS((EZ_Widget *parent, char *str, char *clr));
char  *EZ_GetLedString MY_ANSIARGS((EZ_Widget *led));
void  EZ_ClearLed  MY_ANSIARGS((EZ_Widget *lcd));
void  EZ_SetLedForeground  MY_ANSIARGS((EZ_Widget *lcd, char *clr));
void  EZ_SetLedBackground  MY_ANSIARGS((EZ_Widget *lcd, char *clr));
void  EZ_SetLedOffPixelColor  MY_ANSIARGS((EZ_Widget *lcd, char *clr));
void  EZ_SetLedFont  MY_ANSIARGS((EZ_Widget *lcd, char *font));
void  EZ_SetLedWidth  MY_ANSIARGS((EZ_Widget *lcd, int w));
void  EZ_SetLedHeight  MY_ANSIARGS((EZ_Widget *lcd, int h));
void  EZ_SetLedSize    MY_ANSIARGS((EZ_Widget *lcd, int w, int h));
void  EZ_SetLedPixelSize  MY_ANSIARGS((EZ_Widget *lcd, int s, int t));
void  EZ_GetLedGeometry  MY_ANSIARGS((EZ_Widget *led, int *w, int *h, int *pw, int *ph));
int   EZ_LedIsFree  MY_ANSIARGS((EZ_Widget *lcd));
void  EZ_SetLedPixmapColors MY_ANSIARGS((EZ_Widget *lcd, int on, int off));
void  EZ_OnOffLedPixel MY_ANSIARGS((EZ_Widget *led, int onff, int x, int y, char *clr));
void  EZ_LedLoadBitmap MY_ANSIARGS((EZ_Widget *wptr, unsigned char *data,
                                    int w, int h, int x0, int y0));

void  EZ_LedLoadPixelMatrix MY_ANSIARGS((EZ_Widget *wptr, unsigned long *data,
                                         int w, int h, int x0, int y0,int d));
void  EZ_LedLoadOnOffMatrix MY_ANSIARGS((EZ_Widget *wptr, char *data,
                                         int w, int h, int x0, int y0, int d));

int    EZ_GetLedSelection  MY_ANSIARGS((EZ_Widget *lcd, int *x, int *y));
void   EZ_LedWindow2Led    MY_ANSIARGS((EZ_Widget *lcd, int wx, int wy, int *lx, int *ly));
Pixmap EZ_MakeColorBall MY_ANSIARGS((unsigned long pv, int size));
void   EZ_DestroyColorBalls MY_ANSIARGS((void));
/*********************************************************************/
EZ_Widget  *EZ_CreateLed MY_ANSIARGS((EZ_Widget *parent));
void        EZ_DrawWidgetLed MY_ANSIARGS((EZ_Widget *widget));
void        EZ_ComputeWidgetLedSize MY_ANSIARGS((EZ_Widget *widget, int *w, int *h));
void        EZ_FreeWidgetLedData MY_ANSIARGS((EZ_Widget *widget));
void        EZ_LedEventHandle MY_ANSIARGS((EZ_Widget *widget, XEvent *event));

/*********************************************************************/
static void fixLedInitOffsets MY_ANSIARGS((EZ_Widget *led));
static void LedDrawPixel  MY_ANSIARGS((Drawable d, GC gc, int x, int y,int xs, int ys,
                                       unsigned long pv, int type));
static void LedDrawColumn MY_ANSIARGS((EZ_Widget *led, Drawable d, int idx));
static void LedDrawRow    MY_ANSIARGS((EZ_Widget *led, Drawable d, int idx));
static void LedCreateMask MY_ANSIARGS((EZ_Widget *led));
static void LedCreatePixmapC MY_ANSIARGS((EZ_Widget *led, int w, int h));
static void LedCreatePixmap  MY_ANSIARGS((EZ_Widget *led, int w, int h));
static void TimerSCallBack MY_ANSIARGS((EZ_Timer *timer, void *data));
static void TimerRCallBack MY_ANSIARGS((EZ_Timer *timer, void *data));

static void ledDestroyAction MY_ANSIARGS((EZ_LedAction *at));
static int  ledAddAction  MY_ANSIARGS((EZ_Widget *led, int type, int delay, int cnt, 
				       char *str, char *color));
extern int EZ_CounterUT;

/*********************************************************************/
typedef struct colorBall_ {
  unsigned long  pv;
  Pixmap         pixmap;
  struct colorBall_ *next;
} colorBall;

static colorBall *colorBalls[64];


/*********************************************************************
 * 
 *  Local Variables.
 */
static EZ_WidgetHandle EZ_LedHandle =
{ 
  EZ_ComputeWidgetLedSize,
  EZ_DrawWidgetLed,
  EZ_FreeWidgetLedData,
  EZ_LedEventHandle,    /* response only to Expose */
};

/*********************************************************************/

EZ_Widget  *EZ_CreateLed(parent)
     EZ_Widget  *parent;     /* parent widget    */
{
  EZ_Widget  *tmp;
  
  tmp = EZ_CreateNewWidget(parent);
  /*--------------------------------------------------
   * Register the handling functions for label.
   *  has to be done after  EZ_CreateNewWiget.
   *-------------------------------------------------*/
  EZ_WidgetHandlingFunctions[EZ_WIDGET_LED] = &EZ_LedHandle;

  EZ_SetWidgetTypeAndNames(tmp,EZ_WIDGET_LED);
  EZ_WidgetBorderStyle(tmp)  = EZ_BORDER_NONE;
  EZ_WidgetBorderWidth(tmp)  = 0;
  EZ_WidgetPadB(tmp)  = 0;
  EZ_WidgetFont(tmp) = EZ_GetFontFromId(0); /* 9x15 */
  EZ_WidgetLabelPosition(tmp) = EZ_CENTER;
  EZ_WidgetJustification(tmp) = EZ_CENTER;
  EZ_WidgetForeground(tmp) = EZ_ColorArray[10];

  EZ_LEDAction(tmp) = NULL;
  EZ_LEDBG(tmp)= EZ_ColorArray[0];
  EZ_LEDBGL(tmp)= EZ_ColorArray[2];
  EZ_LEDXSize(tmp) = 2;
  EZ_LEDYSize(tmp) = 2;
  EZ_LEDPixmap(tmp) = (Pixmap)NULL;
  EZ_LEDPixmapC(tmp) = (Pixmap)NULL;
  EZ_LEDSRows(tmp) = 10;
  EZ_LEDSCols(tmp) = 60;
  EZ_LEDOldW(tmp) = 0;
  EZ_LEDOldH(tmp) = 0;
  EZ_LEDAdvance(tmp) = 0;
  EZ_LEDDirty(tmp) = LED_ALL_DIRTY;
  EZ_LEDImage(tmp) = (unsigned char *)NULL;
  EZ_LEDStringLength(tmp) = 0;
  EZ_LEDHasBitmap(tmp) = 0;
  EZ_LEDString(tmp) = EZ_AllocCopyString(NULL);
  EZ_LEDNLines(tmp) = 1;
  EZ_LEDLineLength(tmp) = 6000;
  EZ_LEDTextWidth(tmp) = 1;
  EZ_LEDTextHeight(tmp) = 1;
  EZ_LEDMaskW(tmp) = 0;
  EZ_LEDMaskH(tmp) = 0;
  EZ_LEDXOffset(tmp) = 0;
  EZ_LEDYOffset(tmp) = 0;
  EZ_LEDColorF(tmp) = NULL;
  EZ_LEDStatus(tmp) = 0;
  EZ_LEDXYIdx(tmp) = 0;
  EZ_LEDOther(tmp) = 0;
  EZ_LEDXXX(tmp) = -1;

  (void) EZ_CreateTimer(1, 0, -1, (EZ_CallBack)TimerSCallBack, (void *)tmp, 0);

  return(tmp);
}


void EZ_FreeWidgetLedData(widget)
     EZ_Widget *widget;
{
  if(EZ_LEDString(widget) != (char *)NULL)
    my_free((char *)EZ_LEDString(widget));
  if(EZ_LEDPixmap(widget) != (Pixmap)NULL)
    XFreePixmap(EZ_Display, EZ_LEDPixmap(widget));
  if(EZ_LEDPixmapC(widget) != (Pixmap)NULL)
    XFreePixmap(EZ_Display, EZ_LEDPixmapC(widget));
  if(EZ_LEDImage(widget) != NULL) my_free((char *)EZ_LEDImage(widget));
  {
    EZ_LedAction *tmp, *head = EZ_LEDAction(widget);
    while(head) { tmp = head->next; ledDestroyAction(head); head = tmp; }
  }
}

 
/********************************************************************
 *
 *  Figure out the dimension of a lcd widget.
 */
void EZ_ComputeWidgetLedSize(widget, w, h)
    EZ_Widget *widget;
     int             *w, *h;
{
  int      length, height, cw, ch;
	    
  if(!(EZ_GetWidgetSizeComputedFlag(widget))){ EZ_SetWidgetSizeComputedFlag(widget);}
  
  length = EZ_LEDXSize(widget) * EZ_LEDSCols(widget);
  height = EZ_LEDYSize(widget) * EZ_LEDSRows(widget);

  EZ_WidgetPadX(widget) =0; /* padding does not make too much sense for a led */
  EZ_WidgetPadY(widget) =0; 
  EZ_WidgetPadB(widget) =0;
  cw = EZ_WidgetBorderWidth(widget);  
  ch = EZ_WidgetBorderWidth(widget);
  cw = cw + cw + length;
  ch = ch + ch + height;  

  *w = cw;
  *h = ch;
}

/*******************************************************************
 *
 *  Draw A Led Widget.
 */
void  EZ_DrawWidgetLed(wptr)
     EZ_Widget *wptr;
{
  int       w, h, bw, bw2, ww,hh, rows, cols, fixoff=0;
  int       xsize, ysize;
  Pixmap    pixb, pixmap;
  Window    win;

  win = EZ_WidgetWindow(wptr);
  w   = EZ_WidgetWidth(wptr);
  h   = EZ_WidgetHeight(wptr);
  bw = EZ_WidgetBorderWidth(wptr);
  bw2 = bw+bw;

  ww = w - bw2; hh = h - bw2;
  xsize = EZ_LEDXSize(wptr);
  ysize = EZ_LEDYSize(wptr);
  rows = (hh + ysize -1)/ysize;
  cols = (ww + xsize -1)/xsize; 
  EZ_LEDRows(wptr) = rows;
  EZ_LEDCols(wptr) = cols;

  if((EZ_LEDOldW(wptr) != ww) || (EZ_LEDOldH(wptr) != hh))
    {
      unsigned char *umask = EZ_LEDImage(wptr);

      EZ_LEDOldW(wptr) = ww;  EZ_LEDOldH(wptr) = hh;
      if(EZ_LEDPixmap(wptr) != (Pixmap)NULL) XFreePixmap(EZ_Display, EZ_LEDPixmap(wptr));
      if(EZ_LEDPixmapC(wptr) != (Pixmap)NULL)XFreePixmap(EZ_Display, EZ_LEDPixmapC(wptr));
      EZ_LEDPixmap(wptr) = XCreatePixmap(EZ_Display, win, ww, hh, EZ_Depth);       
      EZ_LEDPixmapC(wptr) = XCreatePixmap(EZ_Display, win, ww, hh, EZ_Depth);       
      EZ_LEDDirty(wptr) |= (LED_PIXMAP_DIRTY | LED_PIXMAPC_DIRTY);
      fixoff = 1;
      if(umask &&  (EZ_LEDDirty(wptr) & LED_MASK_DIRTY) == 0)
        {
          int nww = (cols +7)/8;
          int nhh = rows;
          int oww = EZ_LEDMaskW(wptr);
          int ohh = EZ_LEDMaskH(wptr);
          int scols = oww > nww ? oww : nww;
          int srows = ohh > nhh ? ohh : nhh;
          int i, size = scols * srows;
          unsigned char *src, *dest;
          EZ_LEDImage(wptr) = (unsigned char *)my_malloc(size * sizeof(char), _EZ_LED_DATA_);
          memset(EZ_LEDImage(wptr), 0, size);
          src = umask; dest = EZ_LEDImage(wptr);
          for(i = 0; i < ohh; i++)
            {
              memcpy(dest, src, oww);
              src += oww;
              dest += scols;
            }
          EZ_LEDMaskW(wptr) = scols;
          EZ_LEDMaskH(wptr) = srows;
          my_free(umask);
        }
    }
  if(EZ_LEDDirty(wptr) & LED_MASK_DIRTY)
    {
      fixoff = 1;
      LedCreateMask(wptr);
    }

  if(fixoff) fixLedInitOffsets(wptr);

  if(EZ_LEDDirty(wptr) & LED_PIXMAPC_DIRTY)
    LedCreatePixmapC(wptr, ww, hh);
  if(EZ_LEDDirty(wptr) & LED_PIXMAP_DIRTY)  
    LedCreatePixmap(wptr, ww, hh);
  
  pixb = EZ_LEDPixmap(wptr);  
  XCopyArea(EZ_Display,pixb,win, EZ_WRITABLEGC,0,0,ww,hh,bw,bw);       
  if(bw2 != 0)
    {
      pixmap = XCreatePixmap(EZ_Display, win, w, h, EZ_Depth); 
      EZ_DrawRectBorder(wptr, pixmap);     
      XCopyArea(EZ_Display,pixmap, win, EZ_WRITABLEGC, 0, 0, w, bw, 0, 0);
      XCopyArea(EZ_Display,pixmap, win, EZ_WRITABLEGC,0, h-bw, w, bw, 0, h-bw);
      XCopyArea(EZ_Display,pixmap, win, EZ_WRITABLEGC,0, bw, bw, hh, 0, bw);
      XCopyArea(EZ_Display,pixmap, win, EZ_WRITABLEGC,w-bw, bw, bw, hh, w-bw, bw);
      XFreePixmap(EZ_Display, pixmap);       
    }
}
static void fixLedInitOffsets(wptr)
     EZ_Widget *wptr;
{
  int cols = EZ_LEDCols(wptr);
  int rows = EZ_LEDRows(wptr);
  int xtmp, ytmp;

  xtmp = (EZ_LEDTextWidth(wptr) - cols );
  if(xtmp < 0)
    {
      switch(EZ_WidgetLabelPosition(wptr)) 
	{
	case EZ_CENTER: case EZ_BOTTOM: case EZ_TOP:
          EZ_LEDXOffset(wptr) = xtmp/2; break;
	case EZ_RIGHT:  case EZ_TOP_RIGHT: case EZ_BOTTOM_RIGHT:
	  EZ_LEDXOffset(wptr) = xtmp;   break;
	default:  EZ_LEDXOffset(wptr) = 0; break;
	}
    }
  else EZ_LEDXOffset(wptr)  = 0;
  ytmp = EZ_LEDTextHeight(wptr) - rows;
  if(ytmp < 0 )
    {
      switch(EZ_WidgetLabelPosition(wptr)) 
	{
	case EZ_CENTER:  case EZ_LEFT: case EZ_RIGHT:
          EZ_LEDYOffset(wptr) = ytmp/2; break;
	case EZ_BOTTOM: case EZ_BOTTOM_LEFT: case EZ_BOTTOM_RIGHT: 
	  EZ_LEDYOffset(wptr) = ytmp; break;
	default:  EZ_LEDYOffset(wptr) = 0; break;
	}
    }
  else EZ_LEDYOffset(wptr) = 0;
}
/***************************************************************/
void EZ_LedEventHandle(widget, event)
     EZ_Widget *widget;
     XEvent *event;
{
  if(widget)
    {
      if(event->type == Expose) EZ_DrawWidget(widget);      
      else if(event->type == ButtonPress)
	{
	  if(event->xbutton.button == EZ_Btn3)
	    {
	      /*
		#include "EZ_DnDHandler1.c"
		EZ_DnDEventHandler1(widget, event);
		*/
	      EZ_HandleDnDEvents(widget, event);
	    }
          else
            {
              int bw = EZ_WidgetBorderWidth(widget);
              int rows = EZ_LEDRows(widget);
              int cols = EZ_LEDCols(widget);
              int pxsize = EZ_LEDXSize(widget);
              int pysize = EZ_LEDYSize(widget);
              int off = bw;
              XEvent  xevent;
              int button = event->xbutton.button;
              int x = event->xbutton.x;
              int y = event->xbutton.y;
              int px, py, moved=0;

              px = (x - off)/pxsize;
              py = (y - off)/pysize;
              if(px < 0 || py < 0 || px >= cols || py >= rows) return;

              EZ_LEDXXX(widget) = (short) px;
              EZ_LEDYYY(widget) = (short) py;

              xevent.type =0;
              while(1)
                {
                  int haveE = 0;
                  while(XPending(EZ_Display))
                    {
                      if(xevent.type == ButtonRelease && xevent.xbutton.button == button) break;
                      XNextEvent(EZ_Display, &xevent);
                      if(EZ_FilterEvent(&xevent)) EZ_InvokePrivateEventHandler(&xevent);
                      if(xevent.type == Expose || xevent.type == FocusIn || xevent.type == FocusOut) 
                        EZ_WidgetDispatchEvent(&xevent); 
                      haveE=1;
                    }
                  if(!haveE)
                    {
                      EZ_CheckTimerEvents();
                      if(EZ_CheckAppInputs(1000) != 0)
                        EZ_SitForALittleBit(1000);
                    }
                  if(xevent.type == ButtonRelease && xevent.xbutton.button == button)
                    {
                      EZ_ExecuteWidgetCallBacks(widget);
                      break;
                    }
                  else 
                    {
                      int             rx,ry,x,y;
                      unsigned int    mask;
                      Window          root,win;
                      XQueryPointer(EZ_Display, EZ_WidgetWindow(widget),
                                    &root, &win, 
                                    &rx, &ry, 
                                    &x,&y, 
                                    &mask );
                      px = (x - off)/pxsize;
                      py = (y - off)/pysize;
                      if(px >= 0 || py >= 0 || px < cols || py < rows)
                        {
                          int oldx = EZ_LEDXXX(widget);
                          int oldy = EZ_LEDYYY(widget);
                          EZ_LEDXXX(widget) = (short) px;
                          EZ_LEDYYY(widget) = (short) py;

                          if(oldx != px || oldy != py || moved==0)
                            {
                              EZ_CallBackStruct *callbacks = EZ_WidgetMotionCallBack(widget);
                              if(callbacks) {EZ_HandleMotionCallBacks(widget,callbacks);}
                              else { EZ_ExecuteWidgetCallBacks(widget);}
                              moved=1;
                            }
                        }
                    }
                }   /* while */
	    }
	}
    }
}
/****************************************************************************/

static void LedDrawPixel(pixb,gc,x,y,xsize,ysize,pv,type)
     Drawable pixb;
     GC gc;
     int x,y,xsize,ysize,type;
     unsigned long pv;
{
  Pixmap pixmap = None;
  int j = x *xsize;
  int i = y *ysize;
  if(xsize == ysize)
    {
      int size = xsize;
      switch(size)
        {
        case 2:
          XDrawPoint(EZ_Display, pixb, gc,j,i);
          break;
        case 3:
          XFillRectangle(EZ_Display, pixb, gc, j,i,2,2);
          break;
        case 4:
          XDrawLine(EZ_Display, pixb,gc,j,i+1,j+2,i+1);		    
          XDrawLine(EZ_Display, pixb, gc,j+1,i,j+1,i+2);		    
          break;
        case 5:
        case 6:
          XFillRectangle(EZ_Display, pixb, gc, j+1,i,2,4);
          XFillRectangle(EZ_Display, pixb,gc, j,i+1,4,2);
          break;
        case 7:
          if(type && (pixmap = EZ_MakeColorBall(pv, size)) != None)
            XCopyArea(EZ_Display, pixmap,  pixb, gc, 0,0,size,size, j,i);
          else
            {
              XFillRectangle(EZ_Display, pixb, gc, j+2,i+1,3,5);
              XFillRectangle(EZ_Display, pixb,gc, j+1,i+2,5,3);
            }
          break;
        case 8:
          if(type && (pixmap = EZ_MakeColorBall(pv, size)) != None)
            {
              XCopyArea(EZ_Display, pixmap,  pixb, gc, 0,0,size,size, j,i);
              if(type > 1) XDrawRectangle(EZ_Display, pixb, EZ_DARKGC2, j,i, size, size);
            }
          else
            {
              XFillRectangle(EZ_Display, pixb, gc, j+2,i+1,4,6);
              XFillRectangle(EZ_Display, pixb,gc, j+1,i+2,6,4);
            }
          break;
        default:
          if(type && (pixmap = EZ_MakeColorBall(pv, size)) != None)
            {
              XCopyArea(EZ_Display, pixmap,  pixb, gc, 0,0,size,size, j,i);
              if(type > 1) XDrawRectangle(EZ_Display, pixb, EZ_DARKGC2, j,i, size, size);
            }
          else
            XFillArc(EZ_Display, pixb, gc, j+1, i+1, size-3, size-3, 0, 360 *64);
          break;
        }
    }
  else
    {
      XFillRectangle(EZ_Display, pixb, gc, j,i,xsize-1,ysize-1);      
    }
}

static void LedCreateMask(wptr)
     EZ_Widget *wptr;
{
  Pixmap pixb;
  GC  gc;
  XImage *image;
  int srows, scols, rows, cols=0, nlines=1, itmp, jtmp, ktmp, rtmp, height, ascent;
  XFontStruct *font_info = EZ_WidgetFont(wptr);

  if(EZ_LEDStringLength(wptr) > 0)
    EZ_FigureLabelExtent(font_info, EZ_LEDString(wptr),EZ_LEDStringLength(wptr),
                         (int)EZ_LEDLineLength(wptr),
                         &nlines, &cols);
  if(cols == 0)
    {
      cols = 1;
      rows = 1;
    }
  EZ_LEDNLines(wptr) = nlines;
  ascent = font_info->ascent;
  height = ascent + font_info->descent;
  rows = height * nlines;

  EZ_LEDTextWidth(wptr) =  cols;
  EZ_LEDTextHeight(wptr) = rows;  

  itmp = ((cols + 7)/8);
  ktmp = ((EZ_LEDCols(wptr) + 7)/8);
  scols = (itmp < ktmp ? ktmp : itmp);
  rtmp = EZ_LEDRows(wptr);
  srows = (rows < rtmp? rtmp : rows); 
  jtmp = srows * scols;
  
  if(EZ_LEDMaskW(wptr) < scols || EZ_LEDMaskH(wptr) < srows )
    {
      if(EZ_LEDImage(wptr) != NULL) my_free((void *)EZ_LEDImage(wptr));
      EZ_LEDImage(wptr) = (unsigned char *)my_malloc(jtmp * sizeof(char), _EZ_LED_DATA_);
    }
  memset(EZ_LEDImage(wptr), 0, jtmp);
  EZ_LEDMaskW(wptr) = scols;        EZ_LEDMaskH(wptr) = srows;      
  pixb = XCreatePixmap(EZ_Display, EZ_WidgetWindow(wptr), cols, rows, EZ_Depth); 
  gc =  EZ_WRITABLEGC;
  XSetForeground(EZ_Display, EZ_WRITABLEGC, 0);
  XFillRectangle(EZ_Display, pixb, gc, 0,0, cols,rows); 
  XSetForeground(EZ_Display, gc, 1);
  XSetFont(EZ_Display,  gc, EZ_WidgetFont(wptr)->fid);
  if(EZ_LEDStringLength(wptr)> 0)
    EZ_RenderLabelText(pixb,gc,0,ascent,height,EZ_LEDString(wptr),EZ_LEDStringLength(wptr),
		       (int)EZ_LEDLineLength(wptr), nlines,
		       cols, EZ_WidgetJustification(wptr), font_info, 0);
  image = XGetImage(EZ_Display, pixb, 0,0, cols,rows, 1, XYPixmap);
  {
    int x, y, advance, itmp, ktmp;
    unsigned int ch;
    unsigned char *row = EZ_LEDImage(wptr);
    unsigned long pixel;
    advance = EZ_LEDAdvance(wptr);
    
    for(y = 0; y < rows; y++, row += advance)
      {
	for(ch = 0, ktmp = 0, itmp = 0, x = 0; x < cols; x++)
	  {
	    pixel = XGetPixel(image,x,y);
	    if(pixel != 0) ch |= (1 << ktmp);
	    ktmp++;
	    if(ktmp == 8)
	      {
		row[itmp] = (unsigned char)ch;
		itmp++;
		ktmp = 0;
		ch = 0;
	      }
	  }
	if(ktmp != 0){ row[itmp] = (unsigned char)ch;}
      }
  }
  XDestroyImage(image);
  XFreePixmap(EZ_Display, pixb);
  EZ_LEDDirty(wptr) &= ~LED_MASK_DIRTY;
}

static void LedCreatePixmapC(wptr, ww, hh)
     EZ_Widget *wptr; int ww, hh;
{
  int  xx, yy, xsize, ysize, rows, cols;
  unsigned long pixel;
  Pixmap  pixb = EZ_LEDPixmapC(wptr);
  int itype = (int)EZ_LEDIndicator(wptr);

  GC   gc;
  pixel = EZ_LEDBG(wptr);	
  if(pixel != ~0)
    {
      gc = EZ_WRITABLEGC;
      XSetForeground(EZ_Display, gc, pixel);
    }
  else EZ_GetBackgroundGC(wptr, &gc, &pixel, 0, 0);
  XFillRectangle(EZ_Display, pixb, gc, 0,0, ww, hh); 

  pixel = EZ_LEDBGL(wptr);
  if(pixel != ~0)	
    {
      gc = EZ_WRITABLEGC;
      XSetForeground(EZ_Display, gc, pixel);
    }
  else
    {
      EZ_GetBrightBDpv(wptr, &pixel);
      EZ_GetBrightBDGC(wptr, &gc);
    }

  rows = EZ_LEDRows(wptr);
  cols = EZ_LEDCols(wptr);
  xsize = EZ_LEDXSize(wptr);
  ysize = EZ_LEDYSize(wptr);
  for(yy = 0; yy < rows; yy++)
    for(xx = 0; xx < cols; xx++)
      LedDrawPixel(pixb,gc,xx,yy,xsize, ysize, pixel, itype); 
  EZ_LEDDirty(wptr) &= ~LED_PIXMAPC_DIRTY;
}
/**************************************************************/

static void LedCreatePixmap(wptr, ww, hh)
     EZ_Widget *wptr; int ww, hh;
{
  Pixmap  pixb = EZ_LEDPixmap(wptr);      
  Pixmap  pixc = EZ_LEDPixmapC(wptr);
  unsigned char *masks = EZ_LEDImage(wptr);

  XCopyArea(EZ_Display, pixc, pixb, EZ_WRITABLEGC,0,0,ww,hh,0,0);       

  if( masks != NULL)      
    {
      int x, y,  cnt, rows, cols, xsize, ysize, xoff, yoff;
      int tw, th, px, py,ix,iy, ii, jj, kk, flag;
      int itype = (int)EZ_LEDIndicator(wptr);
      unsigned int mask;
      int advance = EZ_LEDAdvance(wptr);
      GC   gc = EZ_WRITABLEGC;
      unsigned long pixel, tpixel;
      EZ_LedPixelColorF colorf;

      pixel = EZ_WidgetForeground(wptr);
      XSetForeground(EZ_Display, gc, pixel);
      
      rows = EZ_LEDRows(wptr);
      cols = EZ_LEDCols(wptr);
      xsize = EZ_LEDXSize(wptr);
      ysize = EZ_LEDYSize(wptr);
      if(EZ_LEDStringLength(wptr)> 0 || EZ_LEDHasBitmap(wptr))
        {
          tw = EZ_LEDTextWidth(wptr);
          th = EZ_LEDTextHeight(wptr);
          xoff = EZ_LEDXOffset(wptr);
          yoff = EZ_LEDYOffset(wptr);
        }
      else
        {
          tw = cols;
          th = rows;
          xoff = EZ_LEDXOffset(wptr)=0;
          yoff = EZ_LEDYOffset(wptr)=0;
        }
      colorf = EZ_LEDColorF(wptr);
      
      if(xoff < 0) { px = -xoff; ix = 0;}
      else { px = 0; ix = xoff; }
      if(yoff < 0) { py = -yoff; iy = 0;}
      else { py = 0; iy = yoff; }

      for(y=py, jj=iy, masks +=(iy *advance); y<rows && jj<th;  y++, jj++, masks += advance)
	{
	  for(x = px, ii = ix; x < cols && ii < tw; x++, ii++)
	    {
	      if(colorf)
		{
		  tpixel = (colorf)(wptr,x,y);
		  if(tpixel != pixel)
		    {
		      pixel = tpixel;
		      XSetForeground(EZ_Display, gc, pixel);
		    }
		}
	      cnt = (ii >> 3);  kk = (ii & 0x7); flag = 1 << kk;
	      mask= (unsigned int)(masks[cnt]);
	      if(mask&flag)  LedDrawPixel(pixb,gc,x,y,xsize, ysize, pixel, itype);
	    }
	}
    }
  EZ_LEDDirty(wptr) &= ~LED_PIXMAP_DIRTY;
}
/***********************************************************************/
void EZ_SetLedColorFunction(widget, f)
     EZ_Widget *widget; EZ_LedPixelColorF f;
{
  if(widget && EZ_WidgetType(widget) == EZ_WIDGET_LED)
    {
      EZ_LEDColorF(widget) = f;
      if(EZ_WidgetIsViewable(widget)) EZ_DrawWidget(widget);
    }
}

void EZ_SetLedString(widget, str, clr)
     EZ_Widget *widget; char *str, *clr;
{
  if(widget && EZ_WidgetType(widget) == EZ_WIDGET_LED)
    {
      unsigned long  pixel_value;
      EZ_LEDStatus(widget) |= LED_BLOCKED;
      pixel_value = EZ_AllocateColorFromName(clr);
      if(pixel_value != 0 && EZ_WidgetForeground(widget) != pixel_value)
	EZ_WidgetForeground(widget) = pixel_value;
      EZ_SetWidgetLabelString(widget,str);
      if(EZ_WidgetIsViewable(widget)) EZ_DrawWidget(widget);
      EZ_LEDStatus(widget) &= ~LED_BLOCKED;
      EZ_LEDHasBitmap(widget) = 0;
    }
}

char  *EZ_GetLedString(led)
     EZ_Widget *led;
{
  if(led && EZ_WidgetType(led) == EZ_WIDGET_LED)
    return(EZ_LEDString(led));
  return(NULL);
}

void EZ_SetLedForeground(widget, str)
     EZ_Widget *widget; char *str;
{
  if(widget && EZ_WidgetType(widget) == EZ_WIDGET_LED)
    {
      unsigned long  pixel_value;
      pixel_value = EZ_AllocateColorFromName(str);
      if(pixel_value != 0 && EZ_WidgetForeground(widget) != pixel_value)
	{
	  EZ_WidgetForeground(widget) = pixel_value;
	  if(EZ_WidgetIsViewable(widget)) EZ_DrawWidget(widget);  
	}
    }
}
void EZ_SetLedBackground(widget, str)
     EZ_Widget *widget; char *str;
{
  if(widget && EZ_WidgetType(widget) == EZ_WIDGET_LED)
    {
      unsigned long  pixel_value;
      pixel_value = EZ_AllocateColorFromName(str);
      if(EZ_LEDBG(widget) != pixel_value)
	{
	  EZ_LEDBG(widget) = pixel_value;
	  EZ_LEDDirty(widget) |= (LED_PIXMAP_DIRTY | LED_PIXMAPC_DIRTY);
	  if(EZ_WidgetIsViewable(widget)) EZ_DrawWidget(widget);  
	}
    }
}
void EZ_SetLedOffPixelColor(widget, str)
     EZ_Widget *widget; char *str;
{
  if(widget && EZ_WidgetType(widget) == EZ_WIDGET_LED)
    {
      unsigned long  pixel_value;
      pixel_value = EZ_AllocateColorFromName(str);
      if(EZ_LEDBGL(widget) != pixel_value)
	{
	  EZ_LEDBGL(widget) = pixel_value;
	  EZ_LEDDirty(widget) |= (LED_PIXMAP_DIRTY | LED_PIXMAPC_DIRTY);
	  if(EZ_WidgetIsViewable(widget)) EZ_DrawWidget(widget);  
	}
    }
}
void EZ_SetLedFont(widget, name)
     EZ_Widget *widget; char *name;
{
  if(widget && EZ_WidgetType(widget) == EZ_WIDGET_LED)
    {  
      int font = EZ_LoadXFont(name);
      XFontStruct *fontstruct = EZ_GetFontFromId(font);
      if(EZ_WidgetFont(widget) != fontstruct)
	{
	  EZ_WidgetFont(widget) = fontstruct;
	  EZ_LEDDirty(widget) |= (LED_PIXMAP_DIRTY | LED_MASK_DIRTY);      
	  if(EZ_WidgetIsViewable(widget)) EZ_DrawWidget(widget);  
	}
    }
}

void EZ_SetLedWidth(widget, w)
     EZ_Widget *widget; int w;
{
  if(widget && EZ_WidgetType(widget) == EZ_WIDGET_LED && w > 0)
    {
      if(EZ_LEDSCols(widget) != w)
	{
	  EZ_LEDSCols(widget) = w;
	  EZ_LEDDirty(widget) |= (LED_PIXMAP_DIRTY | LED_PIXMAPC_DIRTY);
	  if(EZ_WidgetIsViewable(widget)) EZ_ReDisplayWidget(widget);
	}
    }
}

void EZ_SetLedHeight(widget, h)
     EZ_Widget *widget; int h;
{
  if(widget && EZ_WidgetType(widget) == EZ_WIDGET_LED && h > 0)
    {
      if(EZ_LEDSRows(widget) != h)
	{
	  EZ_LEDSRows(widget) = h;
	  EZ_LEDDirty(widget) |= (LED_PIXMAP_DIRTY | LED_PIXMAPC_DIRTY);
	  if(EZ_WidgetIsViewable(widget)) EZ_ReDisplayWidget(widget);
	}
    }
}

void EZ_SetLedSize(widget, w, h)
     EZ_Widget *widget; int w, h;
{
  if(widget && EZ_WidgetType(widget) == EZ_WIDGET_LED && w > 0 && h > 0)
    {
      EZ_LEDSCols(widget) = w;
      EZ_LEDSRows(widget) = h;
      EZ_LEDDirty(widget) |= (LED_PIXMAP_DIRTY | LED_PIXMAPC_DIRTY); 
      if(EZ_WidgetIsViewable(widget)) EZ_ReDisplayWidget(widget);
    }
}
void EZ_SetLedPixelSize(widget, s, t)
     EZ_Widget *widget; int s, t;
{
  if(widget && EZ_WidgetType(widget) == EZ_WIDGET_LED)
    {
      if(s <= 0) s = 1; else if(s>62) s = 62; s++;
      if(t <= 0) t = 1; else if(t>62) t = 62; t++;
      if(EZ_LEDXSize(widget) != s || EZ_LEDYSize(widget) != t)
	{
	  EZ_LEDXSize(widget) = s;
	  EZ_LEDYSize(widget) = t;
	  EZ_LEDDirty(widget) |= (LED_PIXMAP_DIRTY | LED_PIXMAPC_DIRTY); 
	  if(EZ_WidgetIsViewable(widget)) EZ_ReDisplayWidget(widget);
	}
    }
}

void EZ_GetLedGeometry(widget, w, h, pw, ph)
     EZ_Widget *widget; int *w, *h, *pw, *ph;
{
  if(widget && EZ_WidgetType(widget) == EZ_WIDGET_LED)
    {
      *w = EZ_LEDCols(widget);
      *h = EZ_LEDRows(widget);
      *pw = EZ_LEDXSize(widget);
      *ph = EZ_LEDYSize(widget);
    }  
}
/***********************************************************************/
void EZ_ClearLed(widget)
     EZ_Widget *widget;
{
  if(widget && EZ_WidgetType(widget) == EZ_WIDGET_LED)
    {
      EZ_LEDStatus(widget) |= LED_BLOCKED;
      if(EZ_LEDStringLength(widget) != 0)
	{
	  EZ_LEDStringLength(widget) = 0;
	  EZ_LEDDirty(widget) |= (LED_MASK_DIRTY | LED_PIXMAP_DIRTY);      
	  if(EZ_WidgetIsViewable(widget)) EZ_DrawWidget(widget);
	}
      EZ_LEDHasBitmap(widget) = 0;
      /* clear the action queue */
      {
	EZ_LedAction *tmp, *head = EZ_LEDAction(widget);
	while(head) { tmp = head->next; ledDestroyAction(head); head = tmp; }
	EZ_LEDAction(widget) = NULL;
      }      
      EZ_LEDStatus(widget) &= ~LED_BLOCKED;
    }
}
/***********************************************************************/
static void LedDrawColumn(widget, drawable, idx)
     EZ_Widget *widget; Drawable drawable; int idx;
{
  int xsize, ysize, nrows, ncols, x, y, advance, xoff, yoff;
  int px, py, ix, iy, ii, jj, kk, th, cnt, flag;
  int itype = (int)EZ_LEDIndicator(widget);
  unsigned int mask;
  unsigned char *masks;
  GC   gc = EZ_WRITABLEGC;
  unsigned long pixel, tpixel;
  EZ_LedPixelColorF colorf;

  nrows = EZ_LEDRows(widget);
  ncols = EZ_LEDCols(widget);
  xsize = EZ_LEDXSize(widget);
  ysize = EZ_LEDYSize(widget);
  xoff = EZ_LEDXOffset(widget);
  yoff = EZ_LEDYOffset(widget);

  if(xoff < 0) { px = -xoff; ix = 0;}
  else { px = 0; ix = xoff; }
  if(yoff < 0) { py = -yoff; iy = 0;}
  else { py = 0; iy = yoff; }

  ii = idx - px + ix;
  if(idx < px || idx > ncols || (ii) > EZ_LEDTextWidth(widget)) {return;}
  pixel = EZ_WidgetForeground(widget);
  XSetForeground(EZ_Display, gc, pixel);

  th = EZ_LEDTextHeight(widget);
  cnt = (ii >> 3); kk = (ii & 0x7); flag= 1<<kk;
  masks = EZ_LEDImage(widget);
  advance = EZ_LEDAdvance(widget);
  colorf = EZ_LEDColorF(widget);
  x = idx;
  for(y=py, jj=iy, masks +=(iy *advance); y<nrows && jj<th;  y++, jj++, masks += advance)
    {
      if(colorf)
	{
	  tpixel = (colorf)(widget,x,y);
	  if(tpixel != pixel)
	    {
	      pixel = tpixel;
	      XSetForeground(EZ_Display, gc, pixel);
	    }	  
	}
      mask= (unsigned int)(masks[cnt]);
      if(mask&flag) {  LedDrawPixel(drawable,gc,x,y,xsize,ysize, pixel,itype); }
    }
}

static void LedDrawRow(widget, drawable, idx)
     EZ_Widget *widget; Drawable drawable; int idx;
{
  int xsize, ysize, nrows, ncols, x, y, advance, xoff, yoff;
  int px, py, ix, iy, ii, kk, cnt, flag,tw;
  int itype = (int)EZ_LEDIndicator(widget);
  unsigned int mask;
  unsigned char *masks;
  GC   gc = EZ_WRITABLEGC;
  unsigned long pixel, tpixel;
  EZ_LedPixelColorF colorf;

  nrows = EZ_LEDRows(widget);
  ncols = EZ_LEDCols(widget);
  xsize = EZ_LEDXSize(widget);
  ysize = EZ_LEDYSize(widget);
  xoff = EZ_LEDXOffset(widget);
  yoff = EZ_LEDYOffset(widget);
  
  if(xoff < 0) { px = -xoff; ix = 0;}
  else { px = 0; ix = xoff; }
  if(yoff < 0) { py = -yoff; iy = 0;}
  else { py = 0; iy = yoff; }

  if(idx < py || idx >= nrows || (iy+idx-py) >= EZ_LEDTextHeight(widget)) return;
  pixel = EZ_WidgetForeground(widget);
  XSetForeground(EZ_Display, gc, pixel);

  tw = EZ_LEDTextWidth(widget);
  advance = EZ_LEDAdvance(widget);
  masks = EZ_LEDImage(widget) + ((iy + (idx -py) ) * advance);
  colorf = EZ_LEDColorF(widget);
  y = idx;
  for(x = px, ii = ix; x < ncols && ii < tw; x++, ii++)
    {
      if(colorf)
	{
	  tpixel = (colorf)(widget,x,y);
	  if(tpixel != pixel)
	    {
	      pixel = tpixel;
	      XSetForeground(EZ_Display, gc, pixel);
	    }	
	}
      cnt = (ii >> 3);  kk = (ii & 0x7); flag = 1 << kk;
      mask= (unsigned int)(masks[cnt]);
      if(mask&flag)  { LedDrawPixel(drawable,gc,x,y,xsize, ysize, pixel,itype); }
    }
}
/***********************************************************************/

static void TimerSCallBack(timer, data)
     EZ_Timer *timer; void *data;
{
  /* this is the action scheduler, wakes up once a second */
  EZ_Widget *led = (EZ_Widget *)data;
  if(EZ_WidgetExist(led) && EZ_WidgetType(led) == EZ_WIDGET_LED)
    {
      EZ_LedAction *at;
      int tw, th, rows, cols;
      if(EZ_LEDStatus(led) & LED_BLOCKED) return; 
      if(EZ_WidgetIsViewable(led) == 0) return;

      at = EZ_LEDAction(led);
      if(at == NULL || at->status != LED_AT_WAITING) return;

      at->status = LED_AT_RUNNING; 
      if(at->color != 0)
	{
	  at->colorf = EZ_LEDColorF(led);
	  EZ_LEDColorF(led) = NULL;
	  EZ_WidgetForeground(led) = at->color;
	}
      
      /* now do the set up */
      if(at->str != NULL)
	{
	  if( EZ_LEDString(led) == NULL ||  strcmp(at->str,EZ_LEDString(led)) != 0)
	    {
	      EZ_SetWidgetLabelString(led, at->str);
	      EZ_LEDStatus(led) &=  ~LED_BLOCKED;
	      /* make mask, set initial offset */
	      LedCreateMask(led);	
	      EZ_LEDDirty(led) &= ~LED_MASK_DIRTY;       
	    }
	}
      tw = EZ_LEDTextWidth(led);
      th = EZ_LEDTextHeight(led);
      rows = EZ_LEDRows(led);
      cols = EZ_LEDCols(led);
      fixLedInitOffsets(led);
      switch(at->type)
	{
	case LED_SB_LEFT:
	case LED_SC_LEFT:
	  EZ_LEDXOffset(led) = -cols;
	  break;
	case LED_SB_RIGHT:
	case LED_SC_RIGHT:
	  EZ_LEDXOffset(led) = tw;
	  break;
	case LED_SC_UP: 
	  EZ_LEDYOffset(led) = -rows;
	  break;
	case LED_SC_DOWN: 
	  EZ_LEDYOffset(led) = th;
	  break;
	case LED_SW_LEFT:
	  EZ_LEDXYIdx(led) = 0;
	  break;	  
	case LED_SW_RIGHT:
	  EZ_LEDXYIdx(led) = cols;
	  break;
	case LED_SW_UP:
	  EZ_LEDXYIdx(led) = rows;
	  break;
	case LED_SW_DOWN:
	  EZ_LEDXYIdx(led) = 0;
	  break;
	case LED_BLINK:
	  EZ_LEDXYIdx(led) = 0;
	  break;
	case LED_SC_CENTER:
	case LED_SC_CENTER_V:
	  EZ_LEDXYIdx(led) = 0;
	  break;
	case LED_SW_CENTER:
	case LED_SW_CENTER_V:
	  EZ_LEDXYIdx(led) = 0;
	  break;
	case LED_BOUNCE:
	  if(EZ_LEDDirty(led) & LED_PIXMAP_DIRTY)	  
	    LedCreatePixmap(led, EZ_LEDOldW(led),EZ_LEDOldH(led));
	  EZ_LEDXYIdx(led) = EZ_LEDXOffset(led);
	  EZ_LEDOther(led) = 0;
	  break;
	case LED_BOUNCE_V:
	  if(EZ_LEDDirty(led) & LED_PIXMAP_DIRTY)	  
	    LedCreatePixmap(led, EZ_LEDOldW(led),EZ_LEDOldH(led));
	  EZ_LEDXYIdx(led) = EZ_LEDYOffset(led);
	  EZ_LEDOther(led) = 0;
	  break;
	case LED_SLEEP:
	  if(EZ_LEDDirty(led) & LED_PIXMAP_DIRTY)	  
	    LedCreatePixmap(led, EZ_LEDOldW(led),EZ_LEDOldH(led));	  
	  break;
	default:
	  break;
	}
      (void) EZ_CreateTimer(0, (long)at->delay, -1, (EZ_CallBack)TimerRCallBack, 
			    (void *)led, at->serial);
    }
  else EZ_CancelTimer(timer);
}

/* execute led actions */
static void TimerRCallBack(timer, data) 
     EZ_Timer *timer; void *data;
{
  EZ_Widget *led = (EZ_Widget *)data;
  if(EZ_WidgetExist(led) && EZ_WidgetType(led) == EZ_WIDGET_LED)
    {
      EZ_LedAction *at;
      int idata;
      if(EZ_WidgetIsViewable(led) == 0) return;   /* later */
      if(EZ_LEDStatus(led) & LED_BLOCKED) return; 

      idata = EZ_GetTimerIntData(timer);      
      at = EZ_LEDAction(led);
      if(at && at->serial == idata)
	{
	  int type = at->type;
	  int finished = 0;
	  Window win = EZ_WidgetWindow(led);
	  int bw = EZ_WidgetBorderWidth(led);
	  int xsize = EZ_LEDXSize(led);
	  int ysize = EZ_LEDYSize(led);
	  Pixmap pixmap = EZ_LEDPixmap(led);
	  Pixmap pixtmp;
	  int tw = EZ_LEDTextWidth(led);
	  int th = EZ_LEDTextHeight(led);
	  int ww = EZ_LEDOldW(led);
	  int hh = EZ_LEDOldH(led);
	  int rows = EZ_LEDRows(led);
	  int cols = EZ_LEDCols(led);
	  switch(type)
	    {
	    case LED_BOUNCE:
	      if(tw < cols)
		{
		  if(EZ_LEDOther(led) < 3) /* going left */
		    {
		      if(EZ_LEDOther(led) == 0) /* going left */ 
			{
			  if(EZ_LEDXOffset(led) > 0) EZ_LEDOther(led) += 1;
			  else
			    {
			      EZ_LEDXOffset(led) += 1;
			      pixtmp = XCreatePixmap(EZ_Display, win, ww, hh, EZ_Depth);       
			      XCopyArea(EZ_Display, pixmap, pixtmp, EZ_WRITABLEGC,
					xsize,0,ww-xsize, hh, 0, 0);
			      XCopyArea(EZ_Display, EZ_LEDPixmapC(led),pixtmp,EZ_WRITABLEGC,
					ww-xsize,0, xsize, hh, ww-xsize, 0);
			      XCopyArea(EZ_Display, pixtmp,pixmap,EZ_WRITABLEGC,
					0,0, ww, hh, 0, 0);
			      XCopyArea(EZ_Display, pixmap,win,EZ_WRITABLEGC,
					0,0,ww,hh,bw,bw);
			      XFreePixmap(EZ_Display, pixtmp);
			    }
			}
		      else if(EZ_LEDOther(led) == 1) /* going right */
			{
			  if(EZ_LEDXOffset(led) <= (tw -cols)) EZ_LEDOther(led) += 1;
			  else
			    {
			      EZ_LEDXOffset(led) -= 1;
			      pixtmp = XCreatePixmap(EZ_Display, win, ww, hh, EZ_Depth);       
			      XCopyArea(EZ_Display, pixmap, pixtmp, EZ_WRITABLEGC,
					0,0,ww-xsize, hh, xsize, 0);
			      XCopyArea(EZ_Display, EZ_LEDPixmapC(led),pixtmp,EZ_WRITABLEGC,
					0,0, xsize, hh, 0, 0);
			      XCopyArea(EZ_Display, pixtmp,pixmap,EZ_WRITABLEGC,
					0,0, ww, hh, 0, 0);
			      XCopyArea(EZ_Display, pixmap,win,EZ_WRITABLEGC,
					0,0,ww,hh,bw,bw);
			      XFreePixmap(EZ_Display, pixtmp);
			    }
			}
		      else  /* coming back to its original location */
			{
			  int tmp = EZ_LEDXOffset(led) - EZ_LEDXYIdx(led);
			  if(tmp < 0)
			    {
			      EZ_LEDXOffset(led) += 1;
			      pixtmp = XCreatePixmap(EZ_Display, win, ww, hh, EZ_Depth);       
			      XCopyArea(EZ_Display, pixmap, pixtmp, EZ_WRITABLEGC,
					xsize,0,ww-xsize, hh, 0, 0);
			      XCopyArea(EZ_Display, EZ_LEDPixmapC(led),pixtmp,EZ_WRITABLEGC,
					ww-xsize,0, xsize, hh, ww-xsize, 0);
			      XCopyArea(EZ_Display, pixtmp,pixmap,EZ_WRITABLEGC,
					0,0, ww, hh, 0, 0);
			      XCopyArea(EZ_Display, pixmap,win,EZ_WRITABLEGC,
					0,0,ww,hh,bw,bw);
			      XFreePixmap(EZ_Display, pixtmp);
			    }
			  else  EZ_LEDOther(led) += 1;
			}
		    }
		  else  finished = 1;
		  if(finished && --(at->count) > 0) 
		    {
		      EZ_LEDXYIdx(led) = EZ_LEDXOffset(led);
		      finished = 0; EZ_LEDOther(led) = 0; 
		    }
		}
	      else finished = 1;
	      break;
	    case LED_BOUNCE_V:
	      if(th < rows)
		{
		  if(EZ_LEDOther(led) < 3) 
		    {
		      if(EZ_LEDOther(led) == 0) /* going up */ 
			{
			  if(EZ_LEDYOffset(led) > 0) EZ_LEDOther(led) += 1;
			  else
			    {
			      EZ_LEDYOffset(led) += 1;
			      pixtmp = XCreatePixmap(EZ_Display, win, ww, hh, EZ_Depth);       
			      XCopyArea(EZ_Display, pixmap, pixtmp, EZ_WRITABLEGC,
					0, ysize,ww, hh-ysize, 0, 0);
			      XCopyArea(EZ_Display, EZ_LEDPixmapC(led),pixtmp,EZ_WRITABLEGC,
					0,hh-ysize, ww, ysize, 0, hh-ysize);
			      XCopyArea(EZ_Display, pixtmp,pixmap,EZ_WRITABLEGC,
					0,0, ww, hh, 0, 0);
			      XCopyArea(EZ_Display, pixmap,win,EZ_WRITABLEGC,
					0,0,ww,hh,bw,bw);
			      XFreePixmap(EZ_Display, pixtmp);
			    }
			}
		      else if(EZ_LEDOther(led) == 1) /* going down */
			{
			  if(EZ_LEDYOffset(led) <= (th -rows)) EZ_LEDOther(led) += 1;
			  else
			    {
			      EZ_LEDYOffset(led) -= 1;
			      pixtmp = XCreatePixmap(EZ_Display, win, ww, hh, EZ_Depth);       
			      XCopyArea(EZ_Display, pixmap, pixtmp, EZ_WRITABLEGC,
					0,0,ww, hh-ysize, 0, ysize);
			      XCopyArea(EZ_Display, EZ_LEDPixmapC(led),pixtmp,EZ_WRITABLEGC,
					0,0, ww, ysize, 0, 0);
			      XCopyArea(EZ_Display, pixtmp,pixmap,EZ_WRITABLEGC,
					0,0, ww, hh, 0, 0);
			      XCopyArea(EZ_Display, pixmap,win,EZ_WRITABLEGC,
					0,0,ww,hh,bw,bw);
			      XFreePixmap(EZ_Display, pixtmp);
			    }
			}
		      else  /* coming back to its original location */
			{
			  int tmp = EZ_LEDYOffset(led) - EZ_LEDXYIdx(led);
			  if(tmp < 0)
			    {
			      EZ_LEDYOffset(led) += 1;
			      pixtmp = XCreatePixmap(EZ_Display, win, ww, hh, EZ_Depth);       
			      XCopyArea(EZ_Display, pixmap, pixtmp, EZ_WRITABLEGC,
					0, ysize,ww, hh-ysize, 0, 0);
			      XCopyArea(EZ_Display, EZ_LEDPixmapC(led),pixtmp,EZ_WRITABLEGC,
					0, hh-ysize, ww, ysize, 0, hh-ysize);
			      XCopyArea(EZ_Display, pixtmp,pixmap,EZ_WRITABLEGC,
					0,0, ww, hh, 0, 0);
			      XCopyArea(EZ_Display, pixmap,win,EZ_WRITABLEGC,
					0,0,ww,hh,bw,bw);
			      XFreePixmap(EZ_Display, pixtmp);
			    }
			  else  EZ_LEDOther(led) += 1;
			}
		    }
		  else finished = 1;
		  if(finished && --(at->count) > 0) 
		    {
		      EZ_LEDXYIdx(led) = EZ_LEDXOffset(led);
		      finished = 0; EZ_LEDOther(led) = 0; 
		    }
		}
	      else finished = 1;
	      break;
	    case LED_SHOW:
	      LedCreatePixmap(led, ww, hh);
	      XCopyArea(EZ_Display, pixmap,win,EZ_WRITABLEGC,
			0,0,ww,hh,bw,bw);	      
	      finished = 1;
	      break;
	    case LED_SC_LEFT:
	      {
		int itmp = (cols -2)*xsize;
		EZ_LEDXOffset(led) += 1;
		pixtmp = XCreatePixmap(EZ_Display, win, ww, hh, EZ_Depth);       
		XCopyArea(EZ_Display, pixmap, pixtmp, EZ_WRITABLEGC,
			  xsize,0,itmp, hh, 0, 0);
		XCopyArea(EZ_Display, EZ_LEDPixmapC(led),pixtmp,EZ_WRITABLEGC,
			  itmp,0, ww-itmp, hh, itmp, 0);
		LedDrawColumn(led, pixtmp, cols-2);
		XCopyArea(EZ_Display, pixtmp,pixmap,EZ_WRITABLEGC,
			  0,0, ww, hh, 0, 0);
		XCopyArea(EZ_Display, pixmap,win,EZ_WRITABLEGC,
			  0,0,ww,hh,bw,bw);
		XFreePixmap(EZ_Display, pixtmp);
		if(EZ_LEDXOffset(led) >= MAXV(0, tw -cols) ) finished = 1;
	      }
	    break;
	    case LED_SB_LEFT:
	      {
		int itmp = (cols -2)*xsize;
		EZ_LEDXOffset(led) += 1;
		if(EZ_LEDOther(led) >= 24) EZ_LEDOther(led) =0;
		pixtmp = XCreatePixmap(EZ_Display, win, ww, hh, EZ_Depth);       
		XCopyArea(EZ_Display, pixmap, pixtmp, EZ_WRITABLEGC,
			  xsize,0,itmp, hh, 0, 0);
		XCopyArea(EZ_Display, EZ_LEDPixmapC(led),pixtmp,EZ_WRITABLEGC,
			  itmp,0, ww-itmp, hh, itmp, 0);
		LedDrawColumn(led, pixtmp, cols-2);
		XCopyArea(EZ_Display, pixtmp,pixmap,EZ_WRITABLEGC,
			  0,0, ww, hh, 0, 0);
		EZ_LEDOther(led) += 1; 
		if(EZ_LEDXOffset(led) >= MAXV(0, tw -cols) ) finished = 1;
		if(EZ_LEDOther(led) < 16 || (finished && at->count <= 1)) 
		  XCopyArea(EZ_Display, pixmap,win,EZ_WRITABLEGC, 0,0,ww,hh,bw,bw);
		else 
		  XCopyArea(EZ_Display, EZ_LEDPixmapC(led), win,EZ_WRITABLEGC, 0,0,ww,hh,bw,bw);

		XFreePixmap(EZ_Display, pixtmp);
	      }
	    break;
	    case LED_SB_RIGHT:  
	      {
		EZ_LEDXOffset(led) -= 1;
		if(EZ_LEDOther(led) >= 30) EZ_LEDOther(led) =0;
		pixtmp = XCreatePixmap(EZ_Display, win, ww, hh, EZ_Depth);       
		XCopyArea(EZ_Display, pixmap,pixtmp,EZ_WRITABLEGC,
			  0,0,ww-xsize, hh, xsize, 0);
		XCopyArea(EZ_Display, EZ_LEDPixmapC(led),pixtmp,EZ_WRITABLEGC,
			  0,0, xsize, hh, 0, 0);
		LedDrawColumn(led, pixtmp, 0);
		XCopyArea(EZ_Display, pixtmp,pixmap,EZ_WRITABLEGC,
			  0,0, ww, hh, 0, 0);
		EZ_LEDOther(led) += 1; 
		if(EZ_LEDXOffset(led) <= MINV(0, tw -cols)) finished = 1;
		if(EZ_LEDOther(led) < 16 || (finished && at->count <= 1)) 
		  XCopyArea(EZ_Display, pixmap,win,EZ_WRITABLEGC, 0,0,ww,hh,bw,bw);
		else 
		  XCopyArea(EZ_Display, EZ_LEDPixmapC(led), win,EZ_WRITABLEGC, 0,0,ww,hh,bw,bw);
		XFreePixmap(EZ_Display, pixtmp);
	      }
	    break;
	    case LED_SC_RIGHT:  
	      {
		EZ_LEDXOffset(led) -= 1;
		pixtmp = XCreatePixmap(EZ_Display, win, ww, hh, EZ_Depth);       
		XCopyArea(EZ_Display, pixmap,pixtmp,EZ_WRITABLEGC,
			  0,0,ww-xsize, hh, xsize, 0);
		XCopyArea(EZ_Display, EZ_LEDPixmapC(led),pixtmp,EZ_WRITABLEGC,
			  0,0, xsize, hh, 0, 0);
		LedDrawColumn(led, pixtmp, 0);
		XCopyArea(EZ_Display, pixtmp,pixmap,EZ_WRITABLEGC,
			  0,0, ww, hh, 0, 0);
		XCopyArea(EZ_Display, pixmap,win,EZ_WRITABLEGC,
			  0,0,ww,hh,bw,bw);
		XFreePixmap(EZ_Display, pixtmp);
		if(EZ_LEDXOffset(led) <= MINV(0, tw -cols)) finished = 1;
	      }
	    break;
	    case LED_SC_UP:
	      {
		int itmp = ysize+ysize;
		EZ_LEDYOffset(led) += 1;
		pixtmp = XCreatePixmap(EZ_Display, win, ww, hh, EZ_Depth);       
		XCopyArea(EZ_Display, pixmap,pixtmp,EZ_WRITABLEGC,
			  0,ysize,ww, hh-itmp, 0, 0);
		XCopyArea(EZ_Display, EZ_LEDPixmapC(led),pixtmp,EZ_WRITABLEGC,
			  0,hh-itmp, ww, itmp, 0, hh-itmp);
		LedDrawRow(led, pixtmp, rows-2);
		XCopyArea(EZ_Display, pixtmp,pixmap,EZ_WRITABLEGC,
			  0,0, ww, hh, 0, 0);
		XCopyArea(EZ_Display, pixmap,win,EZ_WRITABLEGC,
			  0,0,ww,hh,bw,bw);
		XFreePixmap(EZ_Display, pixtmp);
		if(EZ_LEDYOffset(led)> MAXV(0, th -rows)) finished = 1;
	      }
	    break;
	    case LED_SC_DOWN:
	      {
		EZ_LEDYOffset(led) -= 1;
		pixtmp = XCreatePixmap(EZ_Display, win, ww, hh, EZ_Depth);       
		XCopyArea(EZ_Display, pixmap,pixtmp,EZ_WRITABLEGC,
			  0,0,ww, hh-ysize, 0, ysize);
		XCopyArea(EZ_Display, EZ_LEDPixmapC(led),pixtmp,EZ_WRITABLEGC,
			  0,0, ww, ysize, 0, 0);
		LedDrawRow(led, pixtmp, 0);
		XCopyArea(EZ_Display, pixtmp,pixmap,EZ_WRITABLEGC,
			  0,0, ww, hh, 0, 0);
		XCopyArea(EZ_Display, pixmap,win,EZ_WRITABLEGC,
			  0,0,ww,hh,bw,bw);
		XFreePixmap(EZ_Display, pixtmp);
		if(EZ_LEDYOffset(led) <= MINV(0, th - rows)) finished = 1;
	      }
	    break;
	    case LED_SC_CENTER:
	      {
		int bd, ktmp, jtmp, itmp = EZ_LEDXYIdx(led);
		if(itmp <= (tw)/2) 
		  {
		    pixtmp = XCreatePixmap(EZ_Display, win, ww, hh, EZ_Depth);  
		    bd = (cols+1)/2;
		    ktmp = bd*xsize ;
		    jtmp = bd;
		    XCopyArea(EZ_Display, pixmap, pixtmp, EZ_WRITABLEGC,
			      xsize, 0, ktmp-xsize, hh, 0, 0);
		    XCopyArea(EZ_Display, pixmap, pixtmp, EZ_WRITABLEGC,
			      ktmp, 0, ww - ktmp -2*xsize , hh, ktmp + xsize, 0);
		    XCopyArea(EZ_Display, EZ_LEDPixmapC(led), pixtmp, EZ_WRITABLEGC,
			      ktmp-xsize, 0, xsize+xsize , hh, ktmp-xsize, 0);
		    EZ_LEDXOffset(led) = itmp - jtmp;
		    LedDrawColumn(led, pixtmp, bd-1);
		    EZ_LEDXOffset(led) = tw - jtmp - itmp ;
		    LedDrawColumn(led, pixtmp, bd);
		    XCopyArea(EZ_Display, pixtmp, pixmap,EZ_WRITABLEGC,
			      0,0,ww,hh,0,0);
		    XCopyArea(EZ_Display, pixmap,win,EZ_WRITABLEGC,
			      0,0,ww,hh,bw,bw);
		    XFreePixmap(EZ_Display, pixtmp);
		    EZ_LEDXYIdx(led) = itmp+1;
		  }
		else 
		  {
		    if(itmp< (cols+1)/2)
		      {
			bd = (cols - tw +1)/2;
			ktmp = bd * xsize;
			
			XCopyArea(EZ_Display, EZ_LEDPixmapC(led), pixmap, EZ_WRITABLEGC,
				  0, 0, ktmp, hh, 0, 0);
			XCopyArea(EZ_Display, EZ_LEDPixmapC(led), pixmap, EZ_WRITABLEGC,
				  ww - ktmp, 0, ww-ktmp, hh, ww-ktmp, 0);
			XCopyArea(EZ_Display, pixmap,win,EZ_WRITABLEGC,
				  0,0,ww,hh,bw,bw);
		      }
		    finished = 1;
		  }
	      }
	      break;
	    case LED_SC_CENTER_V:
	      {
		int bd, ktmp, jtmp, itmp = EZ_LEDXYIdx(led);
		if(itmp <= (th)/2) 
		  {
		    pixtmp = XCreatePixmap(EZ_Display, win, ww, hh, EZ_Depth);  
		    bd = (rows+1)/2;
		    ktmp = bd*ysize ;
		    jtmp = bd;
		    XCopyArea(EZ_Display, pixmap, pixtmp, EZ_WRITABLEGC,
			      0, ysize, ww, ktmp-ysize, 0, 0);
		    XCopyArea(EZ_Display, pixmap, pixtmp, EZ_WRITABLEGC,
			      0, ktmp, ww, hh - ktmp -2*ysize , 0, ktmp + ysize);
		    XCopyArea(EZ_Display, EZ_LEDPixmapC(led), pixtmp, EZ_WRITABLEGC,
			      0, ktmp-ysize, ww, ysize+ysize, 0, ktmp-ysize);
		    EZ_LEDYOffset(led) = itmp - jtmp;
		    LedDrawRow(led, pixtmp, bd-1);
		    EZ_LEDYOffset(led) = th - jtmp - itmp ;
		    LedDrawRow(led, pixtmp, bd);
		    XCopyArea(EZ_Display, pixtmp, pixmap,EZ_WRITABLEGC,
			      0,0,ww,hh,0,0);
		    XCopyArea(EZ_Display, pixmap,win,EZ_WRITABLEGC,
			      0,0,ww,hh,bw,bw);
		    XFreePixmap(EZ_Display, pixtmp);
		    EZ_LEDXYIdx(led) = itmp+1;
		  }
		else 
		  {
		    if(itmp< (rows+1)/2)
		      {
			bd = (rows - th +1)/2;
			ktmp = bd * ysize;
			
			XCopyArea(EZ_Display, EZ_LEDPixmapC(led), pixmap, EZ_WRITABLEGC,
				  0, 0, ww, ktmp, 0, 0);
			XCopyArea(EZ_Display, EZ_LEDPixmapC(led), pixmap, EZ_WRITABLEGC,
				  0, hh - ktmp, ww, hh-ktmp,  0, hh-ktmp);
			XCopyArea(EZ_Display, pixmap,win,EZ_WRITABLEGC,
				  0,0,ww,hh,bw,bw);
		      }
		    finished = 1;
		  }
	      }
	    break;
	    case LED_SW_UP:
	      {
		int itmp = EZ_LEDXYIdx(led) -1;
		if(itmp >= rows) itmp = rows-1;
		if(itmp >= 0)
		  {
		    int jtmp = itmp * ysize;
		    EZ_LEDXYIdx(led) = itmp;
		    XCopyArea(EZ_Display, EZ_LEDPixmapC(led),pixmap,EZ_WRITABLEGC,
			      0,jtmp, ww, ysize, 0, jtmp);
		    LedDrawRow(led, pixmap, itmp);
		    XCopyArea(EZ_Display, pixmap,win,EZ_WRITABLEGC,
			      0,0,ww,hh,bw,bw);
		  }
		else finished = 1;
	      }
	    break;
	    case LED_SW_LEFT:
	      {
		int itmp = EZ_LEDXYIdx(led) +1;
		if(itmp < cols)
		  {
		    int jtmp = itmp * xsize;
		    EZ_LEDXYIdx(led) = itmp;
		    XCopyArea(EZ_Display, EZ_LEDPixmapC(led),pixmap,EZ_WRITABLEGC,
			      jtmp, 0, xsize, hh, jtmp, 0);
		    LedDrawColumn(led, pixmap, itmp);
		    XCopyArea(EZ_Display, pixmap,win,EZ_WRITABLEGC,
			      0,0,ww,hh,bw,bw);
		  }
		else finished = 1;
	      }
	    break;
	    case LED_SW_RIGHT:
	      {
		int itmp = EZ_LEDXYIdx(led) -1;
		if(itmp >= cols) itmp = cols -1;
		if(itmp >= 0)
		  {
		    int jtmp = itmp * xsize;
		    EZ_LEDXYIdx(led) = itmp;
		    XCopyArea(EZ_Display, EZ_LEDPixmapC(led),pixmap,EZ_WRITABLEGC,
			      jtmp, 0, xsize, hh, jtmp, 0);
		    LedDrawColumn(led, pixmap, itmp);
		    XCopyArea(EZ_Display, pixmap,win,EZ_WRITABLEGC,
			      0,0,ww,hh,bw,bw);
		  }
		else finished = 1;
	      }
	    break;
	    case LED_SW_DOWN:
	      {
		int itmp = EZ_LEDXYIdx(led) +1;
		if(itmp < rows) 
		  {
		    int jtmp = itmp * ysize;
		    EZ_LEDXYIdx(led) = itmp;
		    XCopyArea(EZ_Display, EZ_LEDPixmapC(led),pixmap,EZ_WRITABLEGC,
			      0,jtmp, ww, ysize, 0, jtmp);
		    LedDrawRow(led, pixmap, itmp);
		    XCopyArea(EZ_Display, pixmap,win,EZ_WRITABLEGC,
			      0,0,ww,hh,bw,bw);
		  }
		else finished = 1;
	      }
	    break;
	    case LED_SW_CENTER:
	      {
		int ktmp, jtmp, itmp = EZ_LEDXYIdx(led);
		int bd = (cols+1)/2;
		if(itmp <= bd) 
		  {
		    ktmp = bd - itmp;
		    jtmp = ktmp * xsize;
		    XCopyArea(EZ_Display, EZ_LEDPixmapC(led),pixmap,EZ_WRITABLEGC,
			      jtmp, 0,  xsize, hh, jtmp, 0);
		    LedDrawColumn(led, pixmap, ktmp);
		    ktmp = bd + itmp;
		    if(ktmp < cols)
		      {
			jtmp = ktmp * xsize;
			XCopyArea(EZ_Display, EZ_LEDPixmapC(led),pixmap,EZ_WRITABLEGC,
				  jtmp, 0,  xsize, hh, jtmp, 0);
			LedDrawColumn(led, pixmap, ktmp);
		      }
		    XCopyArea(EZ_Display, pixmap,win,EZ_WRITABLEGC,
			      0,0,ww,hh,bw,bw);
		    EZ_LEDXYIdx(led) = itmp+1;
		  }
		else finished = 1;
	      }
	      break;
	    case LED_SW_CENTER_V:
	      {
		int ktmp, jtmp, itmp = EZ_LEDXYIdx(led);
		int bd = (rows+1)/2;
		if(itmp <= bd) 
		  {
		    ktmp = bd - itmp;
		    jtmp = ktmp * ysize;
		    XCopyArea(EZ_Display, EZ_LEDPixmapC(led),pixmap,EZ_WRITABLEGC,
			      0, jtmp, ww, ysize, 0, jtmp);
		    LedDrawRow(led, pixmap, ktmp);
		    ktmp = bd + itmp;
		    if(ktmp < rows)
		      {
			jtmp = ktmp * ysize;
			XCopyArea(EZ_Display, EZ_LEDPixmapC(led),pixmap,EZ_WRITABLEGC,
				  0, jtmp, ww, ysize, 0, jtmp);
			LedDrawRow(led, pixmap, ktmp);
		      }
		    XCopyArea(EZ_Display, pixmap,win,EZ_WRITABLEGC,
			      0,0,ww,hh,bw,bw);
		    EZ_LEDXYIdx(led) = itmp+1;
		  }
		else finished = 1;
	      }
	      break;
	    case LED_BLINK:
	      {
		at->count--;
		if(EZ_LEDXYIdx(led) != 0 || at->count <= 0)
		  {
		    if( EZ_LEDDirty(led) & LED_PIXMAP_DIRTY)
		      LedCreatePixmap(led, ww, hh);
		    XCopyArea(EZ_Display, EZ_LEDPixmap(led), win,EZ_WRITABLEGC,
			      0,0,ww,hh,bw,bw);
		    XFlush(EZ_Display);
		    EZ_LEDXYIdx(led) = 0;
		  }
		else
		  {
		    XCopyArea(EZ_Display, EZ_LEDPixmapC(led), win,EZ_WRITABLEGC,
			      0,0,ww,hh,bw,bw);
		    EZ_LEDXYIdx(led) = 1;
		  }
		if(at->count <= 0) finished = 1;
	      }
	    break;
	    case LED_SLEEP:
	      XCopyArea(EZ_Display, EZ_LEDPixmap(led), win,EZ_WRITABLEGC,
			0,0,ww,hh,bw,bw);
	      if( --(at->count) <= 0) finished = 1;
	      break;
	    default:
	      finished = 1;
	      break;
	    }
	  if(finished == 0) return;
	  at->count--;
	  if(at->count > 0) /* set it up immediately */
	    { 
	      at->status = LED_AT_WAITING;
	      TimerSCallBack(NULL, led);
	    }
	  else
	    {
	      EZ_LEDAction(led) = at->next;
	      if(at->colorf != NULL) EZ_LEDColorF(led) = at->colorf;
	      ledDestroyAction(at);
	    }
	}
    }
  EZ_CancelTimer(timer);
}
/***********************************************************************/
static int ledAddAction(led, type, delay, cnt, str, color)
     EZ_Widget *led; int type, delay, cnt; char *str; char *color;     
{
  EZ_LedAction *at, *head;
  at = (EZ_LedAction *)my_malloc(sizeof(EZ_LedAction), _LED_AT_);
  at->led = led;
  at->type = type;
  if(str != NULL)  at->str = EZ_AllocCopyString(str);
  else  at->str = NULL;
  at->delay = (delay<1000? 1000: delay);
  at->color = (color == NULL? 0:
	       EZ_AllocateColorFromName(color));
  at->colorf = NULL;
  at->serial = EZ_CounterUT++;
  at->count = (cnt<0? 0: cnt);
  at->status = LED_AT_WAITING; /* new action */
  at->next = NULL;
  head = EZ_LEDAction(led);
  if(head == NULL) EZ_LEDAction(led) = at;
  else
    {
      while(head->next != NULL) head = head->next;
      head->next = at;
    }
  return(at->serial);
}

static void ledDestroyAction(at)
     EZ_LedAction *at;
{
  if(at)
    {
      if(at->str) (void)my_free((char *)at->str);
      (void)my_free((char *)at);
    }
}

void EZ_RemoveLedAction(led, action)
     EZ_Widget *led; int action;
{
  if(led && action > 0 && EZ_WidgetType(led) == EZ_WIDGET_LED )
    {
      EZ_LedAction *atptr = EZ_LEDAction(led);
      if(atptr != NULL)
	{
	  EZ_LedAction *found = NULL;
	  /* removing the current action is not allowed */
	  if(atptr->serial == action) return; 
	  while(atptr->next)
	    {
	      EZ_LedAction *tmp = atptr->next;
	      if( tmp->serial == action)
		{
		  atptr->next = tmp->next;
		  found = tmp;
		  break;
		}
	      atptr = atptr->next;
	    }
	  if(found) ledDestroyAction(found);
	}
    }
}

/***********************************************************************/
int EZ_QueueLedAction(led, type, delay, count, msg, color)
     EZ_Widget *led; int type; char *msg; int delay; int count;
     char *color;
{
  if(led && EZ_WidgetType(led) == EZ_WIDGET_LED)
    {
      if(count < 0 || delay < 0 || type <= 0 || type > LED_LAST_ACTION) return(-1);
      return(ledAddAction(led, type, delay, count, msg, color)); 
    }
  return(-1);
}
/***********************************************************************/
int  EZ_LedIsFree (led) EZ_Widget *led;
{
  if(led && EZ_WidgetType(led) == EZ_WIDGET_LED)
    return( EZ_LEDAction(led) == NULL);
  return(0);
}

#ifdef EZ_DEBUG 
void EZ_DumpLedActionTable(led) EZ_Widget *led;
{
  if(led && EZ_WidgetType(led) == EZ_WIDGET_LED)
    {
      EZ_LedAction *atptr = EZ_LEDAction(led);
      printf("==Led Action Table For %lx ==\n", (unsigned long)led);  
      while(atptr)
	{
	  fprintf(stderr, "[%d %s]\n", atptr->serial, atptr->str);
	  atptr = atptr->next;
	}
    }
}
#endif
/***********************************************************************/

void  EZ_OnOffLedPixel(wptr, onoff, x, y, clr)
     EZ_Widget *wptr; int onoff, x, y; char *clr;
{
  if(wptr && EZ_WidgetType(wptr) == EZ_WIDGET_LED && EZ_WidgetMapped(wptr)
     && EZ_LEDPixmap(wptr) != None)
    {
      int rows = EZ_LEDRows(wptr);
      int cols = EZ_LEDCols(wptr);
      int done = 0;
      if(x >= 0 && x < cols && y >= 0 && y < rows)
        {
          unsigned char *mask = EZ_LEDImage(wptr);          
          if(mask && EZ_LEDStringLength(wptr)== 0 && EZ_LEDHasBitmap(wptr)==0)
            {
              int adv = EZ_LEDMaskW(wptr);
              int rr = y * adv + (x >> 3);
              int pp = x & 0x7;
              unsigned char vv = (1 << pp);
              if(onoff) { if(onoff == 1 && (mask[rr] & vv)) done = 1; mask[rr] |= vv;}
              else { if( (mask[rr] & vv) == 0) done = 1;  else  mask[rr] &=  ~vv;}
            }
          if(!done)
            {
              int itype = (int)EZ_LEDIndicator(wptr);
              int xsize= EZ_LEDXSize(wptr);
              int ysize= EZ_LEDYSize(wptr);
              GC   gc = EZ_WRITABLEGC;
              unsigned long pixel;
              EZ_LedPixelColorF colorf = EZ_LEDColorF(wptr);
              if(onoff)
                {
                  if(clr) pixel = EZ_AllocateColorFromName(clr);
                  else if(colorf) pixel = (colorf)(wptr, x,y);
                  else pixel = EZ_WidgetForeground(wptr);
                  XSetForeground(EZ_Display, gc, pixel);      
                }
              else
                {
                  pixel = EZ_LEDBGL(wptr);
                  if(pixel != ~0)	
                    XSetForeground(EZ_Display, gc, pixel);
                  else 
                    {
                      EZ_GetBrightBDpv(wptr, &pixel);
                      EZ_GetBrightBDGC(wptr, &gc);
                    }
                }
              LedDrawPixel(EZ_LEDPixmap(wptr),gc,x,y,xsize, ysize,pixel,itype);
              if(EZ_WidgetIsViewable(wptr) && EZ_GetWidgetFreezedFlag(wptr)==0)
                EZ_DrawWidget(wptr);
            }
        }
    }
}
/***********************************************************************/
void  EZ_LedLoadBitmap(wptr, data, width, height, x0, y0)
     EZ_Widget *wptr; unsigned char *data; int width,height, x0, y0;
{
  if(wptr && EZ_WidgetType(wptr) == EZ_WIDGET_LED && EZ_WidgetMapped(wptr)  && data)
    {
      int size, adv = (width + 7) >> 3;
      int cols = (EZ_LEDCols(wptr) +7) >> 3;
      int rows = EZ_LEDRows(wptr);
      int srows, scols;

      EZ_LEDStringLength(wptr) = 0;
      EZ_LEDTextWidth(wptr) = width;
      EZ_LEDTextHeight(wptr) = height;
      EZ_LEDDirty(wptr) |= LED_PIXMAP_DIRTY;
      EZ_LEDStatus(wptr) |= LED_BLOCKED;

      scols = cols < adv ? adv : cols;
      srows = rows < height? height: rows;
      size = scols * srows;
      if(EZ_LEDImage(wptr) != NULL) my_free((void *)EZ_LEDImage(wptr));
      EZ_LEDImage(wptr) = (unsigned char *)my_malloc( size *sizeof(char), _EZ_LED_DATA_);
      memset(EZ_LEDImage(wptr), 0, size);
      EZ_LEDMaskW(wptr) = scols;        EZ_LEDMaskH(wptr) = srows;
      {
        unsigned char *src = data, *dest = EZ_LEDImage(wptr);
        int i, si = adv, di = scols;
        for(i = 0; i < height; i++)
          {
            memcpy(dest, src, adv);
            src += si;
            dest += di;
          }
      }
      EZ_LEDHasBitmap(wptr) = 1;
      EZ_LEDStatus(wptr) &= ~LED_BLOCKED;
      EZ_LEDDirty(wptr) &= ~LED_MASK_DIRTY;
      if(EZ_WidgetIsViewable(wptr)) EZ_DrawWidget(wptr);
    }
}

void  EZ_LedLoadPixelMatrix(wptr, pvals, width, height, x0, y0, coor)
     EZ_Widget *wptr; unsigned long *pvals; int width,height, x0, y0, coor;
{
  if(wptr && EZ_WidgetType(wptr) == EZ_WIDGET_LED && EZ_WidgetMapped(wptr) && pvals
     && EZ_LEDPixmap(wptr) != None)
    {
      int rows = EZ_LEDRows(wptr);
      int cols = EZ_LEDCols(wptr);
      int i, j, x, y;
      unsigned long *cptr;
      int itype = (int)EZ_LEDIndicator(wptr);
      int xsize= EZ_LEDXSize(wptr);
      int ysize= EZ_LEDYSize(wptr);
      GC   gc = EZ_WRITABLEGC;

      /* ignore current text or bitmap */
      /*
      EZ_LEDStringLength(wptr) = 0;
      EZ_LEDHasBitmap(wptr) =0;
      */
      for(y = y0, i=0; i< height; y++, i++)
        {
          if(y >= 0 && y < rows)
            {
              int yy = (coor ? rows -y : y);
              cptr = pvals + i * width;
              for(x=x0, j=0; j < width; x++, j++, cptr++)
                {
                  if(x >= 0 && x < cols)
                    {
                      unsigned long pixel = *cptr;
                      XSetForeground(EZ_Display, gc, pixel);      
                      LedDrawPixel(EZ_LEDPixmap(wptr), gc,x,yy, xsize, ysize,pixel,itype);
                    }
                }
            }
        }
      if(EZ_WidgetIsViewable(wptr) && EZ_GetWidgetFreezedFlag(wptr)==0)
        EZ_DrawWidget(wptr);
    }
}
                           
void  EZ_LedLoadOnOffMatrix (wptr, onoff, width, height, x0, y0, coor)
     EZ_Widget *wptr; char *onoff; int width,height, x0, y0, coor;
{
  if(wptr && EZ_WidgetType(wptr) == EZ_WIDGET_LED && EZ_WidgetMapped(wptr) && onoff
     && EZ_LEDPixmap(wptr) != None)
    {
      int rows = EZ_LEDRows(wptr);
      int cols = EZ_LEDCols(wptr);
      int i, j, x, y;
      char  *cptr;
      int itype = (int)EZ_LEDIndicator(wptr);
      int xsize= EZ_LEDXSize(wptr);
      int ysize= EZ_LEDYSize(wptr);
      GC  bggc,  gc = EZ_WRITABLEGC;
      unsigned long bgpixel;
      EZ_LedPixelColorF colorf = EZ_LEDColorF(wptr);
      EZ_GetBrightBDpv(wptr, &bgpixel);
      EZ_GetBrightBDGC(wptr, &bggc);
      
      /* ignore current text or bitmap */
      /*
      EZ_LEDStringLength(wptr) = 0;
      EZ_LEDHasBitmap(wptr) =0;
      */
      for(y = y0, i=0; i< height; y++, i++)
        {
          if( y >= 0 && y < rows)
            {
              int yy = (coor ? rows -y: y);
              cptr = onoff + i * width;
              for(x=x0, j=0; j < width; x++,j++, cptr++)
                {
                  if(x >= 0 && x < cols)
                    {
                      unsigned long pixel;
                      if(*cptr)
                        {
                          if(colorf) pixel = (colorf)(wptr, x,yy);
                          else pixel = EZ_WidgetForeground(wptr);
                          XSetForeground(EZ_Display, gc, pixel);      
                          LedDrawPixel(EZ_LEDPixmap(wptr), gc,x,yy, xsize, ysize,pixel,itype);
                        }
                      else
                        {
                          pixel = EZ_LEDBGL(wptr);
                          if(pixel != ~0) 
                            {
                              XSetForeground(EZ_Display, gc, pixel);
                              LedDrawPixel(EZ_LEDPixmap(wptr), gc,x,yy, xsize, ysize,pixel,itype);
                            }
                          else 
                            {
                              LedDrawPixel(EZ_LEDPixmap(wptr), bggc,x,yy, xsize, ysize,bgpixel,itype);
                            }
                        }
                    }
                }
            }
        }
      if(EZ_WidgetIsViewable(wptr) && EZ_GetWidgetFreezedFlag(wptr)==0)
        EZ_DrawWidget(wptr);
    }
}
/***********************************************************************/
int EZ_GetLedSelection(wptr, x, y)
     EZ_Widget *wptr; int *x; int *y;
{
  if(wptr && EZ_WidgetType(wptr) == EZ_WIDGET_LED)
    {
      int xx = (int)EZ_LEDXXX(wptr);
      int yy = (int)EZ_LEDYYY(wptr);
      if(xx >= 0 && xx < EZ_LEDCols(wptr) && yy >=0 && yy < EZ_LEDRows(wptr))
        {
          *x = xx;
          *y = yy;
          return(1);
        }
    }
  return(0);
}

void EZ_LedWindow2Led(widget, wx, wy, px,py)
     EZ_Widget *widget; int wx, wy; int *px, *py;
{
  if(widget && EZ_WidgetType(widget) == EZ_WIDGET_LED)
    {
      int bw = EZ_WidgetBorderWidth(widget);
      int rows = EZ_LEDRows(widget);
      int cols = EZ_LEDCols(widget);
      int pxsize = EZ_LEDXSize(widget); 
      int pysize = EZ_LEDYSize(widget); 
      int x, y;
      x = (wx - bw)/pxsize;
      y = (wy - bw)/pysize;
      if(x < 0) x = 0; else if(x >= cols) x=cols-1;
      if(y < 0) y = 0; else if(y >= rows) x=rows-1;
      *px = x; *py = y;
    }
}
/***********************************************************************/
Pixmap EZ_MakeColorBall(pv, size)
     unsigned long pv; int size;
{
  colorBall **ptr;
  Pixmap pix;
  
  if(size > 63) size = 63;
  ptr = &(colorBalls[size]);
  while(*ptr) {
    if((*ptr)->pv == pv) return( (*ptr)->pixmap);
    ptr = &( (*ptr)->next);
  }
  /* dont have it */
  {
    int ir, ig, ib, wsize = (size + 7) & (~3);
    float frgb[4], shininess = 32.0;
    float fsize = (float)size, fwsize = (float)wsize, ftmp;
    float fclr=0.0, light_position[4];
    EZ_Widget *canvas;
#define OneOver255 0.003921568627450980392156862745
    EZ_PixelValue2RGB(pv, &ir, &ig, &ib);
    frgb[0] = (float)ir * OneOver255;
    frgb[1] = (float)ig * OneOver255;
    frgb[2] = (float)ib * OneOver255;
    frgb[3] = 1.0;
    light_position[0]=light_position[1]=light_position[3]=0.0;
    light_position[2]=1.0;
    canvas = EZ_CreateWidget(EZ_WIDGET_3D_CANVAS, NULL,
                             EZ_SIZE,   wsize, wsize,
                             EZ_OVERRIDE_REDIRECT, 1,
                             EZ_LOCATION, (-wsize-10), 0,
                             0);

    EZ_DisplayWidget(canvas);         
    EZ_RGBMode();                 /* we are using RGB mode  */
    EZ_SetBackBuffer(EZ_PIXMAP);  /* use a pixmap backbuffer*/
    EZ_ShadeModel(EZ_SMOOTH);     /* turn on smooth shading */
    EZ_DrawBuffer(EZ_BACK);  
    EZ_Enable(EZ_CULL_FACE);
    EZ_Enable(EZ_DEPTH_TEST);
    EZ_Enable(EZ_LIGHTING);       /* enable lighting      */
    EZ_Enable(EZ_LIGHT0);         /* enable one light src */

    EZ_Lightfv(EZ_LIGHT0, EZ_POSITION, light_position);
    EZ_Materialfv(EZ_FRONT,EZ_DIFFUSE,frgb);   /* set material for front face */
    fclr =  frgb[0]+frgb[1]+frgb[2];
    if(fclr > 0.2){ frgb[0]=1.0; frgb[1]=1.0; frgb[2]=1.0;}
    EZ_Materialfv(EZ_FRONT,EZ_SPECULAR,frgb);   /* set material for front face */
    EZ_Materialfv(EZ_FRONT,EZ_SHININESS,&shininess);

    EZ_MatrixMode(EZ_PROJECTION);           /* setup a fixed projection matrix  */
    EZ_LoadIdentity();
    EZ_Ortho(0.0, fwsize, 0.0, fwsize, -fwsize, fwsize);
    EZ_MatrixMode(EZ_MODELVIEW);            /* set matrix mode to model view    */
    EZ_LoadIdentity();
    EZ_Clear(EZ_COLOR_BUFFER_BIT | EZ_DEPTH_BUFFER_BIT);
    ftmp = 0.5 * fsize;
    EZ_Sphere(EZ_SPHERE_QUAD, 4, ftmp,fwsize-ftmp,0.0,ftmp-1.0);
    pix = XCreatePixmap(EZ_Display, EZ_DummyWindow, size, size, EZ_Depth);
    XCopyArea(EZ_Display, EZ_Pixmap3, pix, EZ_WRITABLEGC, 0,0, size, size,0,0);
    EZ_DestroyWidget(canvas);   
    {
      colorBall *ptr = (colorBall *)my_malloc(sizeof(colorBall), _MISC_DATA_);
      ptr->pixmap = pix;
      ptr->pv = pv;
      ptr->next = colorBalls[size];
      colorBalls[size] = ptr;
    }
    return(pix);
  }
}
void EZ_DestroyColorBalls()
{
  int i;
  colorBall *ptr, *next;
  for(i = 0; i < 64; i++)
    {
      ptr = colorBalls[i];
      while(ptr)
        {
          XFreePixmap(EZ_Display, ptr->pixmap);
          next = ptr->next;
          my_free(ptr);
          ptr = next;
        }
      colorBalls[i] = NULL;
    }
}

/***********************************************************************/
#undef _EZ_WIDGET_LED_C_


