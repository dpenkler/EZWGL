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
 ***              Widget Working Area                          ***
 ***                                                           ***
 *****************************************************************/
#define _EZ_WIDGET_IWORK_AREA_C_
#include "EZ_Widget.h"

#define _UP       1
#define _LEFT     2
#define _DOWN	  3
#define _RIGHT    4
/*********************************************************************
 * 
 *  Functions implemented in this file:
 */
EZ_Widget        *EZ_CreateIWorkArea MY_ANSIARGS((EZ_Widget *parent));
void             EZ_DrawWidgetIWorkArea MY_ANSIARGS((EZ_Widget *widget));
void             EZ_ComputeWidgetIWorkAreaSize MY_ANSIARGS((EZ_Widget *widget, int *w, int *h));
void             EZ_FreeWidgetIWorkAreaData MY_ANSIARGS((EZ_Widget *widget));
void             EZ_IWorkAreaEventHandle MY_ANSIARGS((EZ_Widget *widget, XEvent *event));

void             EZ_GrowIWorkAreaDataSpace MY_ANSIARGS((EZ_Widget *widget, int increment));

void              EZ_IWorkAreaInsertItem MY_ANSIARGS((EZ_Widget *widget, EZ_Item *item));
void              EZ_IWorkAreaInsertItems MY_ANSIARGS((EZ_Widget *widget, EZ_Item **item, int n));
void              EZ_IWorkAreaDeleteItem MY_ANSIARGS((EZ_Widget *widget, EZ_Item *item));
void              EZ_IWorkAreaDeleteItemO MY_ANSIARGS((EZ_Widget *widget, EZ_Item *item, int dp));
void              EZ_IWorkAreaDeleteAllItems MY_ANSIARGS((EZ_Widget *widget));

void              EZ_IWorkAreaInsertItemBeforeAfter MY_ANSIARGS((EZ_Widget *widget, EZ_Item *item,
								 EZ_Item *newitem, int BefAft));
void              EZ_IWorkAreaInsertItemUsingIdx MY_ANSIARGS((EZ_Widget *widget, int idx, EZ_Item *item));
void              EZ_IWorkAreaDeleteItemUsingIdxO MY_ANSIARGS((EZ_Widget *widget, int idx, int dp));
void              EZ_IWorkAreaDeleteSomeItemsUsingIndices  MY_ANSIARGS((EZ_Widget *widget, int *ids, int cnt));
EZ_Item           *EZ_FindPointerOnItemIWA MY_ANSIARGS((EZ_Item **data, int count, EZ_Item *guess,
						      int x, int y));

void              EZ_IWorkAreaSelectItem MY_ANSIARGS((EZ_Widget *widget, EZ_Item *item, int *location));
void              EZ_IWorkAreaSelectItemUsingIdx MY_ANSIARGS((EZ_Widget *widget, int idx, int *location));

EZ_Item           **EZ_IWorkAreaGetHighlightedItemsA MY_ANSIARGS((EZ_Widget *widget, int *n_ret));
int               EZ_IWorkAreaGetHighlightedItems MY_ANSIARGS((EZ_Widget *widget,  EZ_Item ** a, int siz));
void EZ_MakeNItemOutlines MY_ANSIARGS((EZ_Item **items, int nitems, int xof, int yof));
void EZ_MoveNItemOutlines MY_ANSIARGS((int x, int y, int flag, int dcnt, int dflag));
void EZ_WorkAreaRaiseItem MY_ANSIARGS((EZ_Widget *widget, EZ_Item *item));
void EZ_WorkAreaInsertAndDrawItemAt MY_ANSIARGS((EZ_Widget *widget, EZ_Item *item, int *location, int disp));
void EZ_WorkAreaDrawItemAt MY_ANSIARGS((EZ_Widget *widget, EZ_Item *item, int x, int y));
void EZ_WorkAreaScaleItem MY_ANSIARGS((EZ_Widget *widget, EZ_Item *item, int mode,
				       float sx, float sy, int tx, int ty, int disp));
void EZ_IWorkAreaMoveOneItem MY_ANSIARGS((EZ_Widget *widget, EZ_Item *item, 
						 int x, int y, int bx,int by,int bw,int bh));
/*********************************************************************
 * 
 * Local functions.
 */
static void   update_iwa_scrollbars MY_ANSIARGS((EZ_Widget *widget));
static void   IWATimerCallBack MY_ANSIARGS((EZ_Timer *timer, void *data));
static void   MoveIWorkAreaItem MY_ANSIARGS((EZ_Widget *widget, EZ_Item *item,
					     int nx, int ny, int ax, int ay,
					     int Rx, int Ry, int Rw, int Rh));
static int    EZ_FindItemInIWA MY_ANSIARGS((EZ_Widget *widget, EZ_Item *item));
static void   EZ_MakeIWASelectionVisible MY_ANSIARGS((EZ_Widget *widget, EZ_Item *sitem, EZ_Item *nitem));
static EZ_Item *EZ_FindClosestItem MY_ANSIARGS((EZ_Item **data,int nitems,EZ_Item *ositem, int dir));
static void   EZ_IWorkAreaClearHighlight  MY_ANSIARGS((EZ_Widget *widget, int bx,int by,int bw,int bh));
static void   EZ_HighlightItemsInRectangle MY_ANSIARGS((EZ_Widget *widget, int bx,int by,int bw,int bh));

static int EZ_IWorkAreaCountHighlightItems MY_ANSIARGS((EZ_Widget *widget));
static void EZ_IWorkAreaMoveNItems MY_ANSIARGS((EZ_Widget *widget, int nitems, EZ_Item *sitem,
						   int x, int y, int bx,int by,int bw,int bh));
static void EZ_IWorkAreaMakeNItemOutlines MY_ANSIARGS((EZ_Widget *widget, int nitems, int xof, int yof));

static void  EZ_IWorkAreaDnDOneItem MY_ANSIARGS((EZ_Widget *widget, EZ_Item *item, int kind,
						 XEvent *event, int Rx, int Ry, int Rw, int Rh));
static void EZ_IWorkAreaDnDNItems MY_ANSIARGS((EZ_Widget *widget, int hitems,
						 XEvent *event, int Rx, int Ry, int Rw, int Rh));
static void EZ_IWorkAreaSelectOneItem MY_ANSIARGS((EZ_Widget *widget, EZ_Item *sitem, 
						   int Rx, int Ry, int Rw, int Rh));
/*********************************************************************
 * 
 *  Local Variables.
 */
static EZ_WidgetHandle EZ_IWorkAreaHandle =
{
  EZ_ComputeWidgetIWorkAreaSize,
  EZ_DrawWidgetIWorkArea,
  EZ_FreeWidgetIWorkAreaData,
  EZ_IWorkAreaEventHandle,
};

/*********************************************************************/

EZ_Widget  *EZ_CreateIWorkArea(parent)
     EZ_Widget  *parent;     /* parent widget    */
{
  EZ_Widget  *wptr;

  wptr = EZ_CreateNewWidget(parent);
  EZ_WidgetHandlingFunctions[EZ_WIDGET_IWORK_AREA] = &EZ_IWorkAreaHandle;

  EZ_SetWidgetTypeAndNames(wptr,EZ_WIDGET_IWORK_AREA);
  EZ_WidgetBorderStyle(wptr)  = EZ_BORDER_DOWN;
  EZ_WidgetBorderWidth(wptr)  = 2;
  EZ_WidgetPadX(wptr) = 1;
  EZ_WidgetPadY(wptr) = 1;
  EZ_WidgetPadB(wptr) = 2;

  EZ_IWorkAreaSelection(wptr) = (EZ_Item *)NULL;
  EZ_IWorkAreaNItems(wptr)    = 0;

  EZ_IWorkAreaHighlightBG(wptr) = ~0;
  EZ_IWorkAreaHighlightFG(wptr) = ~0;
  EZ_IWorkAreaTextBG(wptr)      = ~0;

  EZ_IWorkAreaDirty(wptr)       = 1;

  EZ_IWorkAreaYSize(wptr)       = 1;   
  EZ_IWorkAreaYPageSize(wptr)   = 1;   
  EZ_IWorkAreaYStart(wptr)      = 0;   

  EZ_IWorkAreaXSize(wptr)       = 1;   
  EZ_IWorkAreaXPageSize(wptr)   = 1;   
  EZ_IWorkAreaXStart(wptr)      = 0;   

  EZ_IWorkAreaHScroll(wptr)     = (EZ_Widget *)NULL;   
  EZ_IWorkAreaVScroll(wptr)     = (EZ_Widget *)NULL;   

  EZ_IWorkAreaWorkArea(wptr)    = (EZ_Widget *)NULL;      

  EZ_IWorkAreaDisplayed(wptr) = 0;
  EZ_IWorkAreaItemMovable(wptr) = 1;
  EZ_IWorkAreaDrawn(wptr) = 0;

  EZ_IWorkAreaGeometryManager(wptr) = 
    (EZ_WorkAreaGeometryManager) EZ_WorkAreaGeometryManagerDefault;
  EZ_IWorkAreaGMData(wptr)  = (void *)NULL;

  EZ_IWorkAreaData(wptr)  = (EZ_Item **)NULL;
  EZ_IWorkAreaDataSpace(wptr)  = 0;

  EZ_GrowIWorkAreaDataSpace(wptr, 64);

  EZ_CreateTimer(0,310000,-1, (EZ_CallBack)IWATimerCallBack, wptr, 0);

  EZ_SetWidgetFocusableFlag(wptr);  
  
  EZ_Insert2DnDWidgetList(wptr);
  EZ_WidgetDnDHandler(wptr) = (EZ_DnDMsgHandler)EZ_IWorkAreaDnDHandle;

  return(wptr);
}
/************************************************************************************************/
void  EZ_GrowIWorkAreaDataSpace(wptr, incr)
     EZ_Widget *wptr;
     int incr;
{
  int aval = EZ_IWorkAreaDataSpace(wptr);
  int nitems = EZ_IWorkAreaNItems(wptr);
  EZ_Item **oldData = EZ_IWorkAreaData(wptr);

  if(nitems + incr + 4 > aval)
    {
      int i;
      if(oldData) 
	oldData = (EZ_Item **)my_realloc(oldData, (nitems+incr+64) * sizeof(EZ_Item *), _IWORK_AREA_DATA_);
      else 
	oldData = (EZ_Item **)  my_malloc( (nitems+incr+64) * sizeof(EZ_Item *), _IWORK_AREA_DATA_);
      if( oldData == NULL)  EZ_OutOfMemory("EZ_CreateIWorkArea");
      EZ_IWorkAreaData(wptr)  = oldData;
      EZ_IWorkAreaDataSpace(wptr)  = nitems+incr+64;

      for(i = aval; i < nitems+incr+64; i++) oldData[i] = (EZ_Item *)NULL;
    }
}
/************************************************************************************************/
void   EZ_ComputeWidgetIWorkAreaSize(widget, w, h)
     EZ_Widget *widget;
     int             *w, *h;
{
  int cw, ch;

  if(!(EZ_GetWidgetSizeComputedFlag(widget)))
    { 
      EZ_SetWidgetSizeComputedFlag(widget);
    }
  cw = EZ_WidgetPadX(widget) + EZ_WidgetBorderWidth(widget) + EZ_WidgetPadB(widget);
  ch = EZ_WidgetPadY(widget) + EZ_WidgetBorderWidth(widget) + EZ_WidgetPadB(widget);
  cw = cw + cw + 30;
  ch = ch + ch + 30;

  EZ_IWorkAreaXPageSize(widget)     = 30; 
  EZ_IWorkAreaYPageSize(widget)     = 30; 

  EZ_IWorkAreaDirty(widget) = 1;

  *w = cw;
  *h = ch;
}

/************************************************************************************************/
void  EZ_FreeWidgetIWorkAreaData(widget)
     EZ_Widget *widget;
{
  int i, nitems = EZ_IWorkAreaNItems(widget);
  EZ_Item **oldData = EZ_IWorkAreaData(widget);

  for(i = 0; i < nitems; i++)
    EZ_DestroyItem(oldData[i]);
  (void)my_free( (char *)oldData);
}
/************************************************************************************************/

void EZ_DrawWidgetIWorkArea(wptr)
     EZ_Widget *wptr;
{
  int             w, h, bw, padb, padb2;
  Pixmap          pixmap;
  Window          win;
  GC              gc;
  unsigned long   bgpv;

  win = EZ_WidgetWindow(wptr);
  w   = EZ_WidgetWidth(wptr);
  h   = EZ_WidgetHeight(wptr);
  bw = EZ_WidgetBorderWidth(wptr);
  padb = EZ_WidgetPadB(wptr);
  padb2 = padb + padb;

  /*-----------------------------------------------------------
   *  Create a pixmap, draw into this pixmap.
   *----------------------------------------------------------*/
  pixmap = XCreatePixmap(EZ_Display, win, w, h, EZ_Depth);    
  {
    if(EZ_IWorkAreaTextBG(wptr) != ~0)
      {
	XSetForeground(EZ_Display, EZ_WRITABLEGC, EZ_IWorkAreaTextBG(wptr));	
	gc = EZ_WRITABLEGC;
      }
    else
      {
	if(EZ_GetBackgroundGC(wptr, &gc, &bgpv, 0, 0))
	  {
	    int ox, oy;
	    EZ_GetWidgetTileOrigin(wptr, &ox, &oy);      
	    XSetTSOrigin(EZ_Display, gc, ox, oy);
	  }
      }
  }
  XFillRectangle(EZ_DisplayForWidgets, pixmap, gc, padb,padb, w -padb2, h-padb2); 
  /* display the lists */
  if(EZ_IWorkAreaNItems(wptr) > 0)
    {
      EZ_Item **data = EZ_IWorkAreaData(wptr);
      int nitems = EZ_IWorkAreaNItems(wptr); 
      int Wsize, Hsize;
      int xoff = EZ_WidgetBorderWidth(wptr) + EZ_WidgetPadX(wptr) + padb;
      int yoff = EZ_WidgetBorderWidth(wptr) + EZ_WidgetPadY(wptr) + padb;
      int hspacing = EZ_WidgetSepX(wptr);
      int vspacing = EZ_WidgetSepY(wptr);
      
      int x, y, i;
      int Rx = bw+padb;
      int Ry = bw+padb;
      int Rw = w -(Rx << 1);
      int Rh = h -(Rx << 1);

      EZ_IWorkAreaXPageSize(wptr) = w;
      EZ_IWorkAreaYPageSize(wptr) = h;

      if(EZ_IWorkAreaDirty(wptr))
	{
	  /* if there is a geom manager, call it whenever Dirty is set */
	  if(EZ_IWorkAreaGeometryManager(wptr))
	    {
	      (EZ_IWorkAreaGeometryManager(wptr))(EZ_IWorkAreaGMData(wptr),
						  data, nitems,
						  Rx,Ry,(w - (xoff << 1)), (h - (yoff << 1)),
						  hspacing, vspacing,  &Wsize, &Hsize);
	    }
	  else  /* otherwise, compute the geomtry of items */
	    {
	      EZ_Item *item; int  i, wx = 0, wy = 0, tw, th;	      
	      
	      for(i = 0; i < nitems; i++)
		{
		  if( (item = data[i]) != NULL )
		    {
		      EZ_ComputeItemSize(item, &th, &tw);
		      tw += EZ_ItemOriginX(item);
		      th += EZ_ItemOriginY(item);
		      if(tw > wx) wx = tw;
		      if(th > wy) wy = th;
		    }
		}
	      Wsize = tw; Hsize = th;
	    }
	  EZ_IWorkAreaXSize(wptr) = Wsize + (xoff << 1);
	  EZ_IWorkAreaYSize(wptr) = Hsize + (yoff << 1);
	  EZ_IWorkAreaDirty(wptr)= 0;
	  /* fix XStart and YStart */
	  if(EZ_IWorkAreaYStart(wptr) > 0) EZ_IWorkAreaYStart(wptr) = 0;
	  else
	    {
	      y = EZ_WidgetHeight(wptr) - EZ_IWorkAreaYSize(wptr);
	      if(y < 0 && EZ_IWorkAreaYStart(wptr) < y) EZ_IWorkAreaYStart(wptr) = y;
	      else if(y > 0)  EZ_IWorkAreaYStart(wptr) = 0;
	    }

	  if(EZ_IWorkAreaXStart(wptr) > 0) EZ_IWorkAreaXStart(wptr) = 0;
	  else
	    {
	      x = EZ_WidgetWidth(wptr) - EZ_IWorkAreaXSize(wptr);
	      if(x < 0 && EZ_IWorkAreaXStart(wptr) < x) EZ_IWorkAreaXStart(wptr) = x;
	      else if(x > 0) EZ_IWorkAreaXStart(wptr) = 0;
	    }
	}

      x = EZ_IWorkAreaXStart(wptr) + xoff;
      y = EZ_IWorkAreaYStart(wptr) + yoff;

      /* draw all items */
      {
	unsigned long fg = EZ_IWorkAreaHighlightFG(wptr);
	unsigned long bg = EZ_IWorkAreaHighlightBG(wptr);
	int hmode = EZ_IWorkAreaHighlightMode(wptr);
	for(i = 0; i < nitems; i++)
	  {
	    EZ_Item *item = data[i];
	    if(item)
	      {
		int x0 = EZ_ItemAOriginX(item);
		int y0 = EZ_ItemAOriginY(item);
		EZ_DisplayOneItem(wptr, pixmap, x+x0, y+y0, item, Rx, Ry, Rw,Rh);
	      }
	  }

      /* highlight selection */

	for(i = 0; i < nitems; i++)
	  {
	    EZ_Item *item = data[i];
	    if(EZ_ItemHighlighted(item))
	      EZ_HighlightOneItem(wptr, pixmap, item, fg, bg, Rx,Ry,Rw,Rh, hmode);
	  }
      }
    }
  EZ_FixFocusHighlightBorder(wptr, pixmap, w, h, padb);

  EZ_DrawRectBorder(wptr, pixmap);
  XCopyArea(EZ_Display,pixmap,win, EZ_WRITABLEGC,0,0,w,h,0,0); 
  XFreePixmap(EZ_DisplayForWidgets, pixmap); 

  update_iwa_scrollbars(wptr);

  if(EZ_IWorkAreaDrawn(wptr) == 0)
    {
      EZ_Item *nitem = EZ_IWorkAreaSelection(wptr);
      EZ_IWorkAreaDrawn(wptr) = 1;
      if(nitem) EZ_MakeIWASelectionVisible(wptr, NULL, nitem);
    }
}

static void  update_iwa_scrollbars(wptr)
     EZ_Widget *wptr;
{
  if(EZ_LookupWidgetFromAllHT(EZ_IWorkAreaHScroll(wptr)) != (EZ_Widget *)NULL)
    {
      EZ_UpdateScrollbarI(EZ_IWorkAreaHScroll(wptr),
			  EZ_IWorkAreaXSize(wptr),
			  EZ_IWorkAreaXPageSize(wptr),
			  EZ_IWorkAreaXStart(wptr));
    }
  else
    {
      EZ_Widget *p = EZ_IWorkAreaWorkArea(wptr);
      EZ_IWorkAreaHScroll(wptr) = NULL;
      if(p) EZ_WorkAreaHScroll(p) = NULL;
    }


  if(EZ_LookupWidgetFromAllHT(EZ_IWorkAreaVScroll(wptr)) != (EZ_Widget *)NULL)
    {
      EZ_UpdateScrollbarI(EZ_IWorkAreaVScroll(wptr),
			  EZ_IWorkAreaYSize(wptr),
			  EZ_IWorkAreaYPageSize(wptr),
			  -EZ_IWorkAreaYStart(wptr));
    }
  else
    {
      EZ_Widget *p = EZ_IWorkAreaWorkArea(wptr);
      EZ_IWorkAreaVScroll(wptr) = NULL;
      if(p) EZ_WorkAreaVScroll(p) = NULL;
    }
}
/*****************************************************************************/
void EZ_IWorkAreaEventHandle(widget, event)
     EZ_Widget *widget;
     XEvent          *event;
{
  XEvent    xevent;
  int       x, y, yoffset, xoffset, done, padb;
  int       bw, Rx,Ry,Rw,Rh, ww, hh;
  int       hmode, rubberband;

  if(event->type == Expose && event->xexpose.count == 0)   EZ_DrawWidget(widget);      
  if( EZ_GetWidgetDisabledFlag(widget) )  return;

  switch(event->type)
    {
    case SelectionRequest:
      EZ_HandleSelectionRequest(widget,event);
      break;
    case ButtonPress:
      if(event->xbutton.button == EZ_Btn1)  /* get keyboard focus */
	{ 
	  Window fw; int rt;
	  EZ_Widget *tmp;
	  XGetInputFocus(EZ_Display, &fw, &rt);
	  tmp = EZ_LookupWidgetFromMappedHT(fw);
	  if(tmp != widget) EZ_SetFocusTo(widget);
	}
      if(EZ_IWorkAreaNItems(widget) <= 0) {return;}

      rubberband = EZ_IWorkAreaRubberBand(widget);
      hmode = EZ_IWorkAreaHighlightMode(widget);	
      
      if(event->xbutton.button == EZ_Btn1)
	{ /* button 1 */
	  Time  time_stamp = event->xbutton.time;   
	  int   elapsedtime = EZ_LastButton1ClickTime - time_stamp; 
	  EZ_Item *ositem, *nsitem;
	  EZ_Item **data = EZ_IWorkAreaData(widget);
	  int nitems = EZ_IWorkAreaNItems(widget);
	  
	  EZ_LastButton1ClickTime = time_stamp;
	  
	  padb = EZ_WidgetPadB(widget);
	  bw = EZ_WidgetBorderWidth(widget) + padb;
	  xoffset = bw + EZ_WidgetPadX(widget);
	  yoffset = bw + EZ_WidgetPadY(widget);

	  Rx = bw;
	  Ry = bw;
	  ww = EZ_WidgetWidth(widget);
	  Rw = ww - (bw << 1);
	  hh = EZ_WidgetHeight(widget);
	  Rh = hh -(bw << 1);

	  x = event->xbutton.x;
	  y = event->xbutton.y;

	  ositem = EZ_IWorkAreaSelection(widget);
	  nsitem = EZ_FindPointerOnItemIWA(data, nitems, ositem, x,y);

          if((event->xbutton.state&ShiftMask) && nsitem)
            {
              int tx = xoffset + EZ_IWorkAreaXStart(widget);
              int ty = yoffset + EZ_IWorkAreaYStart(widget);
              int x0 = tx + EZ_ItemAOriginX(nsitem);
              int y0 = ty + EZ_ItemAOriginY(nsitem);
              int w = EZ_ItemAWidth(nsitem);
              int h = EZ_ItemAHeight(nsitem);
              int corner;
              tx=x-x0; ty=y-y0;
              if(tx < w/2) { if(ty < h/2) corner = XC_top_left_corner; else corner = XC_bottom_left_corner;}
              else { if(ty < h/2) corner = XC_top_right_corner; else corner = XC_bottom_right_corner;}
              EZ_IWorkAreaDirty(widget) = 1;
              EZ_DoScaleItem(nsitem, corner, x, y, x0,y0,w,h);
              EZ_DrawWidget(widget);
              return;
            }
	  if(rubberband == 0)  /* this is the default */
	    {
	      if(nsitem != NULL)   /* click is  on an item */
		{
		  if(nsitem != ositem)
		    {
		      /* clear has to be invoked before setting the new selection */
		      EZ_IWorkAreaClearHighlight(widget, Rx,Ry,Rw, Rh);
		      EZ_IWorkAreaSelection(widget) = nsitem;
		      EZ_ItemHighlighted(nsitem) = 1 ;
		      {
			unsigned long fg = EZ_IWorkAreaHighlightFG(widget);
			unsigned long bg = EZ_IWorkAreaHighlightBG(widget);
			EZ_HighlightOneItem(widget, EZ_WidgetWindow(widget),
					    nsitem, fg, bg, Rx,Ry,Rw,Rh, hmode);
		      }
		      EZ_HandleMotionCallBacks(widget,EZ_WidgetMotionCallBack(widget));
		    }
		  /*
		   * a double click cause the execuation of the call back function
		   */
		  if(ABSV(elapsedtime) < DOUBLE_CLICK_TIME_INTERVAL)
		    { 
		      EZ_HandleItemCallBacks(nsitem);
		      EZ_ExecuteWidgetCallBacks(widget);
		      return;
		    }
		  /* move an item */
		  if(EZ_IWorkAreaItemMovable(widget))
		    EZ_IWorkAreaMoveOneItem(widget, nsitem, x,y,Rx,Ry,Rw,Rh);
		  else
		    EZ_IWorkAreaSelectOneItem(widget, nsitem, Rx,Ry,Rw,Rh);
		} /* nsitem != NULL */
	      else /* click is on the background, do scroll */
		{
		  done = 0;
		  xevent.type = 0;
		  while(!done)
		    {
		      if(XPending(EZ_Display))
			{
			  while(XPending(EZ_Display))
			    {
			      if(xevent.type == MotionNotify)
				{
				  x = xevent.xmotion.x;
				  y = xevent.xmotion.y;
				}
			      else if(xevent.type == ButtonRelease)
				{
				  if( xevent.xbutton.button == EZ_Btn1) break;
				}
			      else if(xevent.type == Expose || xevent.type == FocusIn || xevent.type == FocusOut) 
				EZ_WidgetDispatchEvent(&xevent);
			      XNextEvent(EZ_Display, &xevent);
			      if(EZ_FilterEvent(&xevent))
				EZ_InvokePrivateEventHandler(&xevent);
			    }
			}
		      else { EZ_SitForALittleBit(50000); }
		      if(xevent.type == ButtonRelease && xevent.xbutton.button == EZ_Btn1) done = 1;	      
		  
		      if( x < xoffset || x > ww - xoffset ||
			  y <= yoffset  || y >hh - yoffset)
			{
			  if(y <= yoffset)
			    {
			      /* scroll down */
			      if(EZ_IWorkAreaYStart(widget) < 0)
				{
				  int tmp = EZ_IWorkAreaYStart(widget) + 10;
				  EZ_IWorkAreaYStart(widget) = (tmp < 0 ? tmp: 0);
				  EZ_DrawWidget(widget);
				}
			    }
			  else if( y > EZ_WidgetHeight(widget) - yoffset)
			    {
			      /* scroll up */ 
			      int  th = EZ_IWorkAreaYSize(widget);
			      if(th > EZ_WidgetHeight(widget))
				{
				  int dif = EZ_WidgetHeight(widget) - th;
				  int tmp = EZ_IWorkAreaYStart(widget) - 10;
				  int nOff =  (tmp > dif ? tmp: dif);
				  if(EZ_IWorkAreaYStart(widget) != nOff)
				    {
				      EZ_IWorkAreaYStart(widget) = nOff;
				      EZ_DrawWidget(widget);
				    }	  
				}
			    }
			  else if(x <= xoffset)
			    {
			      /* scroll right */
			      if(EZ_IWorkAreaXStart(widget) < 0)
				{
				  int tmp = EZ_IWorkAreaXStart(widget) + 10;
				  EZ_IWorkAreaXStart(widget) = (tmp < 0 ? tmp: 0);
				  EZ_DrawWidget(widget);
				}
			    }
			  else if( x > EZ_WidgetWidth(widget) - xoffset)
			    {
			      /* scroll left */ 
			      int  tw = EZ_IWorkAreaXSize(widget);
			      if(tw > EZ_WidgetWidth(widget))
				{
				  int dif = EZ_WidgetWidth(widget) - tw;
				  int tmp = EZ_IWorkAreaXStart(widget) - 10;
				  int nOff = (tmp > dif ? tmp: dif);
				  if(EZ_IWorkAreaXStart(widget) != nOff)
				    {
				      EZ_IWorkAreaXStart(widget) = nOff;
				      EZ_DrawWidget(widget); 
				    }
				}
			    }
			}
		    }
		}
	    }
	  else /* if rubberband is enabled ... */
	    {
	      if(nsitem != NULL) /* click on an item */
		{
		  if(EZ_ItemHighlighted(nsitem) == 0) /* on a new item */
		    {
		      EZ_IWorkAreaClearHighlight(widget, Rx,Ry,Rw, Rh);
		      EZ_IWorkAreaSelection(widget) = nsitem;
		      EZ_ItemHighlighted(nsitem) = 1 ;
		      {
			unsigned long fg = EZ_IWorkAreaHighlightFG(widget);
			unsigned long bg = EZ_IWorkAreaHighlightBG(widget);
			EZ_HighlightOneItem(widget, EZ_WidgetWindow(widget),
					    nsitem, fg, bg, Rx,Ry,Rw,Rh, hmode);
		      }
		      EZ_HandleMotionCallBacks(widget,EZ_WidgetMotionCallBack(widget));	

		      if(EZ_IWorkAreaItemMovable(widget))
			EZ_IWorkAreaMoveOneItem(widget, nsitem, x,y,Rx,Ry,Rw,Rh);
		      else if(rubberband == 1)
			EZ_IWorkAreaSelectOneItem(widget, nsitem, Rx,Ry,Rw,Rh);
		      else
			{
			  EZ_Item *newitem, *guess = nsitem;
			  unsigned long fg = EZ_IWorkAreaHighlightFG(widget);
			  unsigned long bg = EZ_IWorkAreaHighlightBG(widget);
			  while(1)
			    {
			      XNextEvent(EZ_Display, &xevent);
			      if(EZ_FilterEvent(&xevent))  EZ_InvokePrivateEventHandler(&xevent);
			      if(xevent.type == MotionNotify)
				{
				  x = xevent.xmotion.x;
				  y = xevent.xmotion.y;
				  newitem = EZ_FindPointerOnItemIWA(data, nitems, guess, x,y);
				  if(newitem)
				    {
				      guess = newitem;
				      if(EZ_ItemHighlighted(newitem) == 0) 
					{
					  EZ_ItemHighlighted(newitem) = 1 ;
					  EZ_HighlightOneItem(widget, EZ_WidgetWindow(widget),
							      newitem, fg, bg, Rx,Ry,Rw,Rh, hmode);
					}
				    }
				  while (XCheckTypedEvent(EZ_Display, MotionNotify, &xevent));
				}
			      else if(xevent.type == ButtonRelease && xevent.xbutton.button == EZ_Btn1)
				break;
			      else if(xevent.type == Expose || xevent.type == FocusIn || xevent.type == FocusOut)
				EZ_WidgetDispatchEvent(&xevent);			      
			    }
			}
		    }
		  else  /* click on a highlighted item */
		    {
		      if(ositem == nsitem && ABSV(elapsedtime) < DOUBLE_CLICK_TIME_INTERVAL)
			{ 
			  EZ_HandleItemCallBacks(nsitem);
			  EZ_ExecuteWidgetCallBacks(widget);
			  return;
			}
		      if(EZ_IWorkAreaItemMovable(widget))
			{
			  int nhitems = EZ_IWorkAreaCountHighlightItems(widget);
			  if(nhitems == 1)
			    EZ_IWorkAreaMoveOneItem(widget, nsitem, x,y,Rx,Ry,Rw,Rh);
			  else
			    EZ_IWorkAreaMoveNItems(widget, nhitems, nsitem, x,y,Rx,Ry,Rw,Rh);
			}
		      else /* not movable */
			{
			  EZ_IWorkAreaClearHighlight(widget, Rx,Ry,Rw, Rh);
			  EZ_IWorkAreaSelection(widget) = nsitem;
			  EZ_ItemHighlighted(nsitem) = 1 ;
			  {
			    unsigned long fg = EZ_IWorkAreaHighlightFG(widget);
			    unsigned long bg = EZ_IWorkAreaHighlightBG(widget);
			    EZ_HighlightOneItem(widget, EZ_WidgetWindow(widget),
						nsitem, fg, bg, Rx,Ry,Rw,Rh, hmode);
			  }
			  if(ositem != nsitem)
			    EZ_HandleMotionCallBacks(widget,EZ_WidgetMotionCallBack(widget));	
			}
		    }
		}
	      else /* click on bg, do rubberband */
		{
		  int x0, y0, x1, y1, rx, ry, rw,rh, dflag = 0, moved = 0;
		  Window win = EZ_WidgetWindow(widget);
		  GC gc = EZ_XORGC;
		  x0 = x;      y0 = y;
		  rx = ry = rw = rh = 0;
		  
		  while(1)
		    {
		      XNextEvent(EZ_Display, &xevent);
		      if(EZ_FilterEvent(&xevent))  EZ_InvokePrivateEventHandler(&xevent);
		      if(xevent.type == MotionNotify)
			{
			  if(moved == 0)
			    {
			      moved = 1;
			      EZ_IWorkAreaClearHighlight(widget, Rx,Ry,Rw, Rh);
			      EZ_IWorkAreaSelection(widget) = NULL;
			    }
			  if(dflag) 
			    {
			      XDrawRectangle(EZ_Display, win, gc, rx,ry,rw, rh);
			      dflag = 0;
			    }
			  while (XCheckTypedEvent(EZ_Display, MotionNotify, &xevent));
			  x1 = xevent.xmotion.x;
			  y1 = xevent.xmotion.y;
			  rw = x1 - x0; if(rw<0) rw = -rw;
			  rh = y1 - y0; if(rh<0) rh = -rh;
			  rx = x0 < x1 ? x0: x1;
			  ry = y0 < y1 ? y0: y1;
			  if(moved) EZ_HighlightItemsInRectangle(widget, rx,ry,rw,rh);
			  XDrawRectangle(EZ_Display, win, gc, rx,ry,rw, rh);
			  dflag = 1;
			}
		      else if(xevent.type == ButtonRelease &&  xevent.xbutton.button == EZ_Btn1)
			{
			  if(dflag) 
			    XDrawRectangle(EZ_Display, win, gc, rx,ry,rw, rh);		      
			  break;
			}
		      else if(xevent.type == Expose || xevent.type == FocusIn || xevent.type == FocusOut)
			EZ_WidgetDispatchEvent(&xevent);
		    }
		  if(moved) /* select an item */
		    {
		      EZ_Item **data = EZ_IWorkAreaData(widget);
		      int i, nitems = EZ_IWorkAreaNItems(widget);
		      for(i = 0; i < nitems; i++)
			{
			  EZ_Item *item = data[i];
			  if(EZ_ItemHighlighted(item))
			    {
			      EZ_IWorkAreaSelection(widget) = item;
			      EZ_HandleMotionCallBacks(widget,EZ_WidgetMotionCallBack(widget));
			      break;
			    }
			}
		    }
		}
	    }
	}
      else if(event->xbutton.button == EZ_Btn3)
	{ /* button 3 */
	  EZ_Item *ositem, *nsitem;
	  EZ_Item **data = EZ_IWorkAreaData(widget);
	  int nitems = EZ_IWorkAreaNItems(widget);
	  
	  bw = EZ_WidgetBorderWidth(widget) + EZ_WidgetPadB(widget);
	  xoffset = bw + EZ_WidgetPadX(widget);
	  yoffset = bw + EZ_WidgetPadY(widget);

	  Rx = bw;
	  Ry = bw;
	  ww = EZ_WidgetWidth(widget);
	  Rw = ww - (bw << 1);
	  hh = EZ_WidgetHeight(widget);
	  Rh = hh -(bw << 1);

	  x = event->xbutton.x;
	  y = event->xbutton.y;

	  ositem = EZ_IWorkAreaSelection(widget);
	  nsitem = EZ_FindPointerOnItemIWA(data, nitems, ositem, x,y);

	  if(rubberband == 0)
	    {
	      if( nsitem != NULL) /* click is  on an item */
		{
		  if(nsitem != ositem)
		    {
		      EZ_IWorkAreaClearHighlight(widget, Rx,Ry,Rw, Rh);
		      EZ_IWorkAreaSelection(widget) = nsitem;
		      EZ_ItemHighlighted(nsitem) = 1 ;
		      if(nsitem)
			{
			  unsigned long fg = EZ_IWorkAreaHighlightFG(widget);
			  unsigned long bg = EZ_IWorkAreaHighlightBG(widget);
			  int  hmode = EZ_IWorkAreaHighlightMode(widget);
			  EZ_HighlightOneItem(widget, EZ_WidgetWindow(widget),
					      nsitem, fg, bg, Rx,Ry,Rw,Rh,hmode);
			}
		      EZ_HandleMotionCallBacks(widget,EZ_WidgetMotionCallBack(widget));
		    }
		  /* do DnD on one item */
		  if(nsitem) 
		    {
		      if(EZ_ItemIsDnDSrc(nsitem))
			EZ_IWorkAreaDnDOneItem(widget, nsitem, 0, event, Rx,Ry,Rw,Rh);
		      else if(EZ_WidgetIsDnDSrc(widget))
			EZ_IWorkAreaDnDOneItem(widget, nsitem, 1, event, Rx,Ry,Rw,Rh);
		    }
		} /* nsitem != NULL */
	    }
	  else  /* rubberband is enabled */
	    {
	      if(nsitem != NULL) /* click on an item */
		{
		  if(EZ_ItemHighlighted(nsitem) == 0) /* on a new item */
		    {
		      EZ_IWorkAreaClearHighlight(widget, Rx,Ry,Rw, Rh);
		      EZ_IWorkAreaSelection(widget) = nsitem;
		      EZ_ItemHighlighted(nsitem) = 1 ;
		      {
			unsigned long fg = EZ_IWorkAreaHighlightFG(widget);
			unsigned long bg = EZ_IWorkAreaHighlightBG(widget);
			EZ_HighlightOneItem(widget, EZ_WidgetWindow(widget),
					    nsitem, fg, bg, Rx,Ry,Rw,Rh, hmode);
		      }
		      EZ_HandleMotionCallBacks(widget,EZ_WidgetMotionCallBack(widget));	
		      
		      /* drag a new item */
		      if(nsitem)
			{
			  if(EZ_ItemIsDnDSrc(nsitem))
			    EZ_IWorkAreaDnDOneItem(widget, nsitem, 0, event, Rx,Ry,Rw,Rh);
			  else if(EZ_WidgetIsDnDSrc(widget))
			    EZ_IWorkAreaDnDOneItem(widget, nsitem, 1, event, Rx,Ry,Rw,Rh);
			}
		    }
		  else  /* click on a highlighted item */
		    {
		      int nhitems = EZ_IWorkAreaCountHighlightItems(widget);
		      if(nhitems == 1)
			{
			  if(nsitem) 
			    {
			      if(EZ_ItemIsDnDSrc(nsitem))
				EZ_IWorkAreaDnDOneItem(widget, nsitem, 0, event, Rx,Ry,Rw,Rh);
			      else if(EZ_WidgetIsDnDSrc(widget))
				EZ_IWorkAreaDnDOneItem(widget, nsitem, 1, event, Rx,Ry,Rw,Rh);   
			    }
			}
		      else if(EZ_WidgetIsDnDSrc(widget)) /* move n items */
			{
			  EZ_IWorkAreaDnDNItems(widget, nhitems, event, Rx,Ry,Rw,Rh);
			}
		    }
		}
	      else /* click on bg, do rubberband */
		{
		  int x0, y0, x1, y1, rx, ry, rw,rh, dflag = 0, moved = 0;
		  Window win = EZ_WidgetWindow(widget);
		  GC gc = EZ_XORGC;
		  x0 = x;      y0 = y;
		  rx = ry = rw = rh = 0;
		  
		  while(1)
		    {
		      XNextEvent(EZ_Display, &xevent);
		      if(EZ_FilterEvent(&xevent))  EZ_InvokePrivateEventHandler(&xevent);
		      if(xevent.type == MotionNotify)
			{
			  if(moved == 0)
			    {
			      moved = 1;
			      EZ_IWorkAreaClearHighlight(widget, Rx,Ry,Rw, Rh);
			      EZ_IWorkAreaSelection(widget) = NULL;
			    }
			  if(dflag) 
			    {
			      XDrawRectangle(EZ_Display, win, gc, rx,ry,rw, rh);
			      dflag = 0;
			    }
			  while (XCheckTypedEvent(EZ_Display, MotionNotify, &xevent));
			  x1 = xevent.xmotion.x;
			  y1 = xevent.xmotion.y;
			  rw = x1 - x0; if(rw<0) rw = -rw;
			  rh = y1 - y0; if(rh<0) rh = -rh;
			  rx = x0 < x1 ? x0: x1;
			  ry = y0 < y1 ? y0: y1;
			  if(moved) EZ_HighlightItemsInRectangle(widget, rx,ry,rw,rh);
			  XDrawRectangle(EZ_Display, win, gc, rx,ry,rw, rh);
			  dflag = 1;
			}
		      else if(xevent.type == ButtonRelease &&  xevent.xbutton.button == EZ_Btn3)
			{
			  if(dflag) 
			    XDrawRectangle(EZ_Display, win, gc, rx,ry,rw, rh);		      
			  break;
			}
		      else if(xevent.type == Expose || xevent.type == FocusIn || xevent.type == FocusOut)
			EZ_WidgetDispatchEvent(&xevent);
		    }
		  if(moved) /* select an item */
		    {
		      EZ_Item **data = EZ_IWorkAreaData(widget);
		      int i, nitems = EZ_IWorkAreaNItems(widget);
		      for(i = 0; i < nitems; i++)
			{
			  EZ_Item *item = data[i];
			  if(EZ_ItemHighlighted(item))
			    {
			      EZ_IWorkAreaSelection(widget) = item;
			      EZ_HandleMotionCallBacks(widget,EZ_WidgetMotionCallBack(widget));
			      break;
			    }
			}
		    }
		}
	    }	      
	}
      break;
    case EnterNotify:
      break;
    case LeaveNotify:
      break;
    case KeyPress:
      if(EZ_IWorkAreaNItems(widget) <= 0) {return;}
      {
	EZ_Item **data = EZ_IWorkAreaData(widget);
	int nitems = EZ_IWorkAreaNItems(widget);
	int               count;
	KeySym            keysym;
	XComposeStatus    compose; 
	char              tmpbuffer[32];
	int               dir = 0, buffersize = 32;
	count = XLookupString(&(event->xkey), tmpbuffer, buffersize, &keysym, &compose);
	tmpbuffer[count] = '\0'; 

	switch(keysym)
	  {
	  case XK_Up: case XK_k:  case XK_p: case XK_K:  case XK_P: 
#ifdef XK_KP_Up
	  case XK_KP_Up:
#endif
	    dir =  _UP;
	    break;
	  case XK_Down: case XK_n: case XK_j: case XK_N: case XK_J:  
#ifdef XK_KP_Down
	  case XK_KP_Down:
#endif
	    dir = _DOWN;
	    break;
	  case XK_Left: case XK_b:  case XK_h: case XK_B:  case XK_H:  
#ifdef XK_KP_Left
	  case XK_KP_Left:
#endif
	    dir = _LEFT;
	    break;
	  case XK_Right: case XK_f: case XK_l: case XK_F: case XK_L:  
#ifdef XK_KP_Right
	  case XK_KP_Right: 
#endif
	    dir = _RIGHT;
	    break;
	  case XK_Return: case XK_Linefeed: case XK_space: 
#ifdef XK_KP_Enter
	  case XK_KP_Enter:
#endif
	    {
	      EZ_Item *ositem = EZ_IWorkAreaSelection(widget);
	      if(ositem)
		{
		  EZ_HandleItemCallBacks(ositem);
		  EZ_ExecuteWidgetCallBacks(widget);
		}
	    }
	    break;
	  default:
	    break;
	  }
	if(dir != 0)
	  {
	    EZ_Item *ositem = EZ_IWorkAreaSelection(widget);
	    EZ_Item *tmp = EZ_FindClosestItem(data,nitems,ositem, dir);
	    if(tmp && tmp != ositem) 
	      {
		bw = EZ_WidgetBorderWidth(widget) + EZ_WidgetPadB(widget);
		xoffset = bw + EZ_WidgetPadX(widget);
		yoffset = bw + EZ_WidgetPadY(widget);

		Rx = bw;
		Ry = bw;
		ww = EZ_WidgetWidth(widget);
		Rw = ww - (bw << 1);
		hh = EZ_WidgetHeight(widget);
		Rh = hh -(bw << 1);

		EZ_IWorkAreaClearHighlight(widget, Rx,Ry,Rw, Rh);
		EZ_IWorkAreaSelection(widget) = tmp;
		EZ_ItemHighlighted(tmp) = 1 ;
		EZ_MakeIWASelectionVisible(widget, ositem, tmp);
		EZ_HandleMotionCallBacks(widget,EZ_WidgetMotionCallBack(widget));
	      }
	  }
      }
      { 
	XEvent tmpevent; 
	while(XCheckTypedEvent(EZ_Display, KeyPress, &tmpevent))
	  if(EZ_FilterEvent(&tmpevent))
	    EZ_InvokePrivateEventHandler(&tmpevent);
      } 
      break;
    default:
      break;
    }
}
/******************************************************************************************/
EZ_Item   *EZ_FindPointerOnItemIWA(data, nitems, guess, x,y)
     EZ_Item **data, *guess;
     int nitems, x, y;
{
  EZ_Item *item;
  int i;

  if(guess && EZ_PickAnItem(guess,x,y)) return(guess);
  for(i = nitems-1; i >= 0; i--)
    {
      item = data[i];
      if(EZ_PickAnItem(item,x,y)) return(item);
    }
  return(NULL);
}
/******************************************************************************************/
void  EZ_IWorkAreaInsertItem(widget, item)
     EZ_Widget *widget;
     EZ_Item *item;
{
  if(widget && item)
    {
      int nitems;
      EZ_Item **data;

      EZ_GrowIWorkAreaDataSpace(widget, 1);
      
      nitems = EZ_IWorkAreaNItems(widget);
      data = EZ_IWorkAreaData(widget);
      data[nitems] = item;
      EZ_IWorkAreaNItems(widget) += 1;
      EZ_IWorkAreaDirty(widget) = 1;
      if(EZ_WidgetMapped(widget)) EZ_DrawWidget(widget);
    }
}

void  EZ_IWorkAreaInsertItems(widget, items, n)
     EZ_Widget *widget;
     EZ_Item **items; int n;
{
  if(widget && n > 0)
    {
      int nitems, i;
      EZ_Item **data;

      EZ_GrowIWorkAreaDataSpace(widget, n+1);
      
      nitems = EZ_IWorkAreaNItems(widget);
      data = EZ_IWorkAreaData(widget);
      for(i = 0; i < n; i++) data[nitems+i] = items[i];
      EZ_IWorkAreaNItems(widget) += n;
      EZ_IWorkAreaDirty(widget) = 1;
      if(EZ_WidgetMapped(widget)) EZ_DrawWidget(widget);
    }
}

/**********************************************************************************/      
void EZ_IWorkAreaDeleteItemO(widget, item, disp)
     EZ_Widget *widget;
     EZ_Item *item;
     int disp;
{
  if(widget && item)
    {
      int nitems = EZ_IWorkAreaNItems(widget);
      EZ_Item **data = EZ_IWorkAreaData(widget);
      int i,j;
      
      for(i = 0; i < nitems; i++)
	{
	  if(data[i] == item) break;
	}
      if(i < nitems)
	{
	  int x0,y0,aw,ah;
	  if(item == EZ_IWorkAreaSelection(widget))
	    EZ_IWorkAreaSelection(widget) = (EZ_Item *)NULL;
	  EZ_IWorkAreaDirty(widget) = 1;
	  x0 = EZ_ItemOriginX(item);
	  y0 = EZ_ItemOriginY(item);
	  aw = EZ_ItemAWidth(item);
	  ah = EZ_ItemAHeight(item);
	  EZ_DestroyItem(item);
	  for(j = i; j < nitems; j++)
	    data[j] = data[j+1];        /* data always contains > 1 extra slot */
	  if(EZ_IWorkAreaNItems(widget) > 0)
	    EZ_IWorkAreaNItems(widget) -= 1;
	  if(EZ_WidgetMapped(widget))
	    {
	      if(disp) EZ_DrawWidget(widget);
	      else
		{
		  int i, nitems = EZ_IWorkAreaNItems(widget); 
		  EZ_Item **data = EZ_IWorkAreaData(widget);    
		  int bw = EZ_WidgetBorderWidth(widget) + EZ_WidgetPadB(widget);
		  int Rx = bw;
		  int Ry = bw;
		  int Rw = EZ_WidgetWidth(widget) - (Rx+Rx);
		  int Rh = EZ_WidgetHeight(widget) -(Rx+Rx);
		  for(i = 0; i < nitems; i++)
		    {
		      EZ_Item *sitem = data[i];
		      if(sitem)
			{
			  int xx0 = EZ_ItemOriginX(sitem);
			  int yy0 = EZ_ItemOriginY(sitem);
			  int w0 = EZ_ItemAWidth(sitem);
			  int h0 = EZ_ItemAHeight(sitem);
			  if( xx0 >= x0 + aw || yy0 >= y0 + ah || xx0 + w0 <= x0  || yy0 + h0 < y0) ;
			  else { EZ_DisplayOneItem(widget, EZ_WidgetWindow(widget), xx0, yy0, sitem, Rx, Ry, Rw,Rh); }
			}
		    }
		}
	    }
	}
    }
}
void EZ_IWorkAreaDeleteItem(widget, item) EZ_Widget *widget; EZ_Item *item;
{  EZ_IWorkAreaDeleteItemO(widget, item, 1);}


void EZ_IWorkAreaDeleteAllItems(widget)
     EZ_Widget *widget;
{
  if(widget)
    {
      int nitems = EZ_IWorkAreaNItems(widget);
      EZ_Item **data = EZ_IWorkAreaData(widget);
      int i, freezed = EZ_GetWidgetFreezedFlag(widget);

      if(freezed == 0) EZ_FreezeWidget(widget);
      EZ_IWorkAreaNItems(widget) = 0;
      EZ_IWorkAreaSelection(widget) = (EZ_Item *)NULL;
      EZ_IWorkAreaDirty(widget) = 1;

      for(i = 0; i < nitems; i++)
	{ EZ_DestroyItem(data[i]); data[i] = NULL;}

      EZ_IWorkAreaXStart(widget) = 0;
      EZ_IWorkAreaYStart(widget) = 0;

      if(freezed == 0) 
	{
	  EZ_ClearWidgetFreezedFlag(widget);
	  if(EZ_WidgetMapped(widget)) EZ_DrawWidget(widget);
	}
    }
}
/**********************************************************************************/
void  EZ_IWorkAreaInsertItemBeforeAfter(widget, item, newitem, which)
     EZ_Widget *widget;
     EZ_Item *item, *newitem;
     int which;
{
  if(widget)
    {
      int nitems, i,j;
      EZ_Item **data;
      EZ_GrowIWorkAreaDataSpace(widget, 1);
      nitems = EZ_IWorkAreaNItems(widget);
      data = EZ_IWorkAreaData(widget);

      for(i = 0; i < nitems; i++)
	{
	  if(data[i] == item) break;
	}
      if(i < nitems) 
	{  if(which)  i++; } /* after */

      for(j = nitems; j > i; j--)
	data[j] = data[j-1];  /* move */
      data[i] = newitem;
      EZ_IWorkAreaNItems(widget) += 1;
      EZ_IWorkAreaDirty(widget) = 1;
      if(EZ_WidgetMapped(widget)) EZ_DrawWidget(widget);
    }
}

void  EZ_IWorkAreaInsertItemUsingIdx(widget, idx, item)
     EZ_Widget *widget;
     EZ_Item *item;
     int idx;
{
  if(widget)
    {
      int nitems, i;
      EZ_Item **data;
      EZ_GrowIWorkAreaDataSpace(widget, 1);
      nitems = EZ_IWorkAreaNItems(widget);

      if(idx < 0 || idx >= nitems) idx = nitems;

      data = EZ_IWorkAreaData(widget);
      for(i = nitems; i > idx; i--)
	data[i] = data[i-1];  /* move */
      data[idx] = item;
      EZ_IWorkAreaDirty(widget) = 1;
      EZ_IWorkAreaNItems(widget) += 1;
      if(EZ_WidgetMapped(widget)) EZ_DrawWidget(widget);
    }
}

void EZ_IWorkAreaDeleteItemUsingIdxO(widget, idx, disp)
     EZ_Widget *widget;
     int idx, disp;
{
  if(widget)
    {
      int nitems = EZ_IWorkAreaNItems(widget);
      EZ_Item *item, **data = EZ_IWorkAreaData(widget);
      int j, x0,y0,aw,ah;
      
      if(idx < 0 || idx >= nitems) return;
      item = data[idx];
      x0 = EZ_ItemOriginX(item);
      y0 = EZ_ItemOriginY(item);
      aw = EZ_ItemAWidth(item);
      ah = EZ_ItemAHeight(item);
      if(item == EZ_IWorkAreaSelection(widget))
	EZ_IWorkAreaSelection(widget) = (EZ_Item *)NULL;
      EZ_IWorkAreaDirty(widget) = 1;
      EZ_DestroyItem(item);
      for(j = idx; j < nitems; j++)
	data[j] = data[j+1];        /* data always contains > 1 extra slot */
      if(EZ_IWorkAreaNItems(widget) > 0)
	EZ_IWorkAreaNItems(widget) -= 1;
      if(EZ_WidgetMapped(widget)) 
	{
	  if(disp) EZ_DrawWidget(widget);
	  else
	    {
	      int i, nitems = EZ_IWorkAreaNItems(widget); 
	      EZ_Item **data = EZ_IWorkAreaData(widget);    
	      int bw = EZ_WidgetBorderWidth(widget) + EZ_WidgetPadB(widget);
	      int Rx = bw;
	      int Ry = bw;
	      int Rw = EZ_WidgetWidth(widget) - (Rx+Rx);
	      int Rh = EZ_WidgetHeight(widget) -(Rx+Rx);
	      for(i = 0; i < nitems; i++)
		{
		  EZ_Item *sitem = data[i];
		  if(sitem)
		    {
		      int xx0 = EZ_ItemOriginX(sitem);
		      int yy0 = EZ_ItemOriginY(sitem);
		      int w0 = EZ_ItemAWidth(sitem);
		      int h0 = EZ_ItemAHeight(sitem);
		      if( xx0 >= x0 + aw || yy0 >= y0 + ah || xx0 + w0 <= x0  || yy0 + h0 < y0) ;
		      else { EZ_DisplayOneItem(widget, EZ_WidgetWindow(widget), xx0, yy0, sitem, Rx, Ry, Rw,Rh); }
		    }
		}
	    }
	}
    }
}
void EZ_IWorkAreaDeleteItemUsingIdx(widget, idx)      EZ_Widget *widget;  int idx;
{ EZ_IWorkAreaDeleteItemUsingIdxO(widget, idx, 1); }
/*******************************************************************************************/

void EZ_IWorkAreaDeleteSomeItemsUsingIndices(widget, indices, count)
     EZ_Widget *widget;
     int *indices, count;
{
  if(widget)
    {
      int nitems = EZ_IWorkAreaNItems(widget);
      EZ_Item *item, *selection, **data = EZ_IWorkAreaData(widget);
      int idx, i, j, deleted = 0;
      
      selection = EZ_IWorkAreaSelection(widget);
      for(i = 0; i < count; i++)
	{
	  idx = indices[i];

	  if(idx >= 0 && idx < nitems) 
	    {
	      if((item = data[idx]) != NULL)
		{
		  if(item == selection)
		    { EZ_IWorkAreaSelection(widget) = (EZ_Item *)NULL; selection = NULL;}
		  EZ_DestroyItem(item);
		  deleted++;
		  data[idx] = NULL;
		}
	    }
	}
      if(deleted)
	{
	  EZ_IWorkAreaDirty(widget) = 1;
	  i = 0, j = 0; 
	  while(j < nitems)
	    {
	      while(data[i] != NULL) i++;
	      j = (j > i? j: i);
	      while( j < nitems && data[j] == NULL) j++;
	      if(j < nitems)
		{
		  data[i] = data[j];
		  data[j] = NULL;
		  j++;
		  i++;
		}
	    }
	  j = nitems - deleted;
	  EZ_IWorkAreaNItems(widget) = (j >= 0? j: 0);
	  if(EZ_WidgetMapped(widget)) EZ_DrawWidget(widget);
	}
    }
}
/**********************************************************************************/
static void IWATimerCallBack(timer, cdata)
     EZ_Timer *timer; 
     void *cdata;
{
  EZ_Widget *widget = (EZ_Widget *)cdata;
  if(widget && EZ_LookupWidgetFromAllHT(widget) == widget &&
     EZ_WidgetType(widget) == EZ_WIDGET_IWORK_AREA)
    {
      int hmode = (int) EZ_IWorkAreaHighlightMode(widget) & 1;
      if(hmode == 0 &&EZ_WidgetWindow(widget) && EZ_WidgetMapped(widget) &&
	 EZ_IWorkAreaNItems(widget) > 0)
	{
	  EZ_Item *sitem = EZ_IWorkAreaSelection(widget);
	  if(sitem  && EZ_ItemType(sitem) != EZ_FIG_ITEM )
	    {
	      int bw = EZ_WidgetBorderWidth(widget) + EZ_WidgetPadB(widget);
	      int Rx = bw;
	      int Ry = bw;
	      int Rw = EZ_WidgetWidth(widget) - (Rx+Rx);
	      int Rh = EZ_WidgetHeight(widget) -(Rx+Rx);
	      EZ_FlashItemOutline(widget,sitem, Rx,Ry,Rw,Rh);
	    }
	}
    }
  else EZ_CancelTimer(timer);
}
/**********************************************************************************/
static void  MoveIWorkAreaItem(widget, item, nx, ny, ax, ay, Rx,Ry,Rw,Rh)
     EZ_Widget *widget;               /* assuming widget and item are not NULL */
     EZ_Item *item;
     int nx,ny,ax,ay,Rx,Ry,Rw,Rh;
{
  Pixmap          pixmap;
  Window          win;
  GC              gc;
  unsigned long   bgpv;
  int             x,y,w,h;
  unsigned long fg = EZ_IWorkAreaHighlightFG(widget);
  unsigned long bg = EZ_IWorkAreaHighlightBG(widget);
  int  hmode = EZ_IWorkAreaHighlightMode(widget);
  w = EZ_ItemAWidth(item);
  h = EZ_ItemAHeight(item);

  /* 2-13-97: if item is not completely within the border, redraw
   * the whole widget to update the border
   */
  if(nx < Rx || ny < Ry || nx+w > Rx+Rw || ny+h > Ry+Rh)
    {
      EZ_ItemOriginX(item) = nx;
      EZ_ItemOriginY(item) = ny;
      EZ_ItemAOriginX(item) = ax;
      EZ_ItemAOriginY(item) = ay;
      EZ_DrawWidget(widget);
      return;
    }

  win = EZ_WidgetWindow(widget);
  x = EZ_ItemOriginX(item);
  y = EZ_ItemOriginY(item);

  pixmap = XCreatePixmap(EZ_Display, win, w, h, EZ_Depth);    

  /* I: remove item from its old position */

  if(EZ_IWorkAreaTextBG(widget) != ~0)
    {
      XSetForeground(EZ_Display, EZ_WRITABLEGC, EZ_IWorkAreaTextBG(widget));	
      gc = EZ_WRITABLEGC;
    }
  else
    {
      if(EZ_GetBackgroundGC(widget, &gc, &bgpv, 0, 0))
	{
	  int ox, oy;
	  EZ_GetWidgetTileOrigin(widget, &ox, &oy);      
	  XSetTSOrigin(EZ_Display, gc, ox-x,oy-y);
	}
    }
  XFillRectangle(EZ_Display, pixmap, gc, 0,0, w, h); 
  {
    int x_r, y_r, w_r, h_r;
    EZ_IntersectRectangles(Rx,Ry,Rw,Rh,    /* the drawing area */
			   x,y,w,h,
			   &x_r,&y_r, &w_r, &h_r);
    if(w_r | h_r) XCopyArea(EZ_Display, pixmap, win, EZ_WRITABLEGC,x_r-x,y_r-y,w_r,h_r, x_r,y_r);
  }

  /* II: display item at its new position */
  EZ_ItemOriginX(item) = nx;
  EZ_ItemOriginY(item) = ny;
  EZ_ItemAOriginX(item) = ax;
  EZ_ItemAOriginY(item) = ay;
  EZ_DisplayOneItem(widget, win, nx, ny, item, Rx, Ry, Rw,Rh);

  /* III: update affected items at item's old position */
  {
    int i, nitems = EZ_IWorkAreaNItems(widget); 
    EZ_Item **data = EZ_IWorkAreaData(widget);    
    for(i = 0; i < nitems; i++)
      {
	EZ_Item *sitem = data[i];
	if(sitem && sitem != item)
	  {
	    int x0 = EZ_ItemOriginX(sitem);
	    int y0 = EZ_ItemOriginY(sitem);
	    int w0 = EZ_ItemAWidth(sitem);
	    int h0 = EZ_ItemAHeight(sitem);
	    if( x0 >= x + w || y0 >= y + h || x0 + w0 <= x  || y0 + h0 < y) ;
	    else { EZ_DisplayOneItem(widget, win, x0, y0, sitem, Rx, Ry, Rw,Rh); }
	  }
      }
  } 
  /* IV:  finally, highlight the item */
  EZ_HighlightOneItem(widget, win, item, fg, bg, Rx,Ry,Rw,Rh, hmode);  

}
/***********************************************************************************************/
static int EZ_FindItemInIWA(widget, item)
     EZ_Widget *widget; EZ_Item *item;
{
  if(item == NULL) return(-1);
  if(widget)
    {
      int nitems = EZ_IWorkAreaNItems(widget);
      if(nitems > 0)
	{
	  EZ_Item **data = EZ_IWorkAreaData(widget);
	  int i;
	  for(i = 0; i < nitems; i++)
	    if(data[i] == item) return(i);
	}
    }
  return(-1);
}
/***********************************************************************************************/
static void EZ_MakeIWASelectionVisible(widget, sitem, nitem) /* assume widget is mapped */
     EZ_Widget *widget; EZ_Item *sitem, *nitem;
{
  int bw,ww,hh,Rx,Ry,Rw,Rh;
  int x0 = EZ_ItemOriginX(nitem);
  int y0 = EZ_ItemOriginY(nitem);
  int iw = EZ_ItemWidth(nitem);
  int ih = EZ_ItemHeight(nitem);

  bw = EZ_WidgetBorderWidth(widget) + EZ_WidgetPadB(widget);
  Rx = Ry = bw;
  ww = EZ_WidgetWidth(widget);
  Rw = ww - (bw << 1);
  hh = EZ_WidgetHeight(widget);
  Rh = hh -(bw << 1);     

    
  if(y0 < 0 || x0 < 0 || x0 +iw > Rw+Rx|| y0 +ih > Rh+Ry)
    {
      if(y0 < 0)
	{
	  int itmp = -y0 + EZ_IWorkAreaYStart(widget) ;
	  EZ_IWorkAreaYStart(widget) = (itmp<0? itmp : 0);
	}
      else if(y0 + ih > Rh+Ry)
	{
	  int th = EZ_IWorkAreaYSize(widget);
	  if(th >= hh)
	    {
	      int itmp = EZ_IWorkAreaYStart(widget) - ih + hh - y0;
	      int dif = hh - th;
	      EZ_IWorkAreaYStart(widget) = (itmp > dif ? itmp: dif);
	    }
	}
      if(x0<0)
	{
	  int itmp = -x0 + EZ_IWorkAreaXStart(widget);
	  EZ_IWorkAreaXStart(widget) = (itmp < 0? itmp: 0);
	}
      else if(x0 +iw > Rw+Rx)
	{
	  int tw = EZ_IWorkAreaXSize(widget);
	  if(tw >= ww)
	    {
	      int itmp = EZ_IWorkAreaXStart(widget) - iw + ww -x0;
	      int dif = ww - tw;
	      EZ_IWorkAreaXStart(widget) = (itmp > dif? itmp: dif);
	    }
	} 
      EZ_IWorkAreaDirty(widget) = 1;      
      EZ_DrawWidget(widget);
    }
  else
    {
      unsigned long fg = EZ_IWorkAreaHighlightFG(widget);
      unsigned long bg = EZ_IWorkAreaHighlightBG(widget);
      int  hmode = EZ_IWorkAreaHighlightMode(widget);
      if(sitem) EZ_UpdateOneItem(widget,EZ_WidgetWindow(widget),sitem, Rx,Ry,Rw,Rh);
      EZ_HighlightOneItem(widget,EZ_WidgetWindow(widget), nitem, fg, bg, Rx,Ry,Rw,Rh,hmode);
    }
}
/*********************************************************************************************/

void EZ_IWorkAreaSelectItemUsingIdx(widget, idx, location)
     EZ_Widget *widget; int idx, *location;
{
  if(widget)
    {
      int nitems = EZ_IWorkAreaNItems(widget);
      int bw,ww,hh,Rx,Ry,Rw,Rh;

      bw = EZ_WidgetBorderWidth(widget) + EZ_WidgetPadB(widget);
      Rx = Ry = bw;
      ww = EZ_WidgetWidth(widget);
      Rw = ww - (bw << 1);
      hh = EZ_WidgetHeight(widget);
      Rh = hh -(bw << 1);	   

      {
	EZ_Item **data = EZ_IWorkAreaData(widget);
	EZ_Item *sitem = EZ_IWorkAreaSelection(widget);

	EZ_IWorkAreaClearHighlight(widget, Rx,Ry,Rw, Rh);
	EZ_IWorkAreaSelection(widget) = NULL;

	if(idx >= 0 && idx < nitems)
	  {
	    EZ_Item *nitem = data[idx];
	    EZ_IWorkAreaSelection(widget) = nitem;
	    EZ_ItemHighlighted(nitem) = 1;

	    if(EZ_WidgetMapped(widget))
	      {
		if(location)
		  {
		    int x = EZ_ItemOriginX(nitem);
		    int y = EZ_ItemOriginY(nitem);
		  
		    EZ_IWorkAreaXStart(widget) += location[0] - x;
		    EZ_IWorkAreaYStart(widget) += location[1] - y;
		    EZ_DrawWidget(widget);
		  }
		EZ_MakeIWASelectionVisible(widget, sitem, nitem);
		EZ_HandleMotionCallBacks(widget,EZ_WidgetMotionCallBack(widget));	      
	      }
	  }
      }
    }
}
void EZ_IWorkAreaSelectItem(widget, item, location)
     EZ_Widget *widget; EZ_Item *item; int *location;
{
  if(widget)
    {
      if(item == NULL)
	EZ_IWorkAreaSelectItemUsingIdx(widget, -1, NULL);
      else
	{
	  int idx = EZ_FindItemInIWA(widget, item);
	  if(idx >= 0) EZ_IWorkAreaSelectItemUsingIdx(widget, idx, location);
	}
    }
}
/***********************************************************************************************/
static EZ_Item *EZ_FindClosestItem(data, nitems, ositem, dir)
     EZ_Item **data; int nitems; EZ_Item *ositem; int dir;
{
  if(ositem == NULL)
    {
      int i;  
      for(i = 0; i < nitems; i++)
	if(EZ_ItemVisible(data[i])) return(data[i]);
    }
  {
    EZ_Item *item, *sitem, *tmpItems[1024];
    int x = EZ_ItemOriginX(ositem);
    int y = EZ_ItemOriginY(ositem);
    int w = EZ_ItemWidth(ositem);
    int h = EZ_ItemHeight(ositem);
    int ww = 0;
    int hh = 0;
    int i, j, k = 0;

    if(dir & 0x1) /* up down */
      {
	while( k < 6 )
	  {
	    int sx = x - ww;
	    int sw = w + ww;
	    int x2 = x + sw;
	    j = 0;
	    for(i = 0; i < nitems; i++)
	      {
		int tx,tw;
		item = data[i];
		if(item != ositem)
		  {
		    tx = EZ_ItemOriginX(item);
		    tw = EZ_ItemWidth(item);
		    if( tx < x2  && tx + tw > sx ) tmpItems[j++] = item;
		  }
	      }
	    if(j > 0)
	      {
		int zy = (dir == _UP ? -100000 : 100000);
		nitems = j;
		sitem = NULL;
		for(i =0; i < nitems; i++)
		  {
		    int ty,th;
		    item = tmpItems[i];
		    ty = EZ_ItemOriginY(item);
		    th = EZ_ItemHeight(item);	    
		    if(dir == _UP)
		      {
			if(ty <= y && ty > zy)
			  {
			    sitem = item;
			    zy = ty;
			  }
		      }
		    else
		      {
			if(ty > y && ty <= zy)
			  {
			    sitem = item;
			    zy = ty;
			  }
		      }
		  }
		return(sitem);
	      }
	    k++;
	    ww = (ww + 1) << 1;
	    hh = (hh + 1) << 1;
	  }
      }
    else /* left right */
      {
	while( k < 6 )
	  {
	    int sy = y - hh;
	    int sh = h + hh;
	    int y2 = y + sh;
	    j = 0;
	    for(i = 0; i < nitems; i++)
	      {
		int ty,th;
		item = data[i];
		if(item != ositem)
		  {
		    ty = EZ_ItemOriginY(item);
		    th = EZ_ItemHeight(item);	    
		    if( ty < y2 && ty + th > sy ) tmpItems[j++] = item;
		  }
	      }
	    if(j > 0)
	      {
		int zx = (dir == _LEFT ? -100000 : 100000);
		nitems = j;
		sitem = NULL;
		for(i =0; i < nitems; i++)
		  {
		    int tx,tw;
		    item = tmpItems[i];
		    tx = EZ_ItemOriginX(item);
		    tw = EZ_ItemWidth(item);
		    if(dir == _LEFT)
		      {
			if(tx <= x && tx > zx)
			  {
			    sitem = item;
			    zx = tx;
			  }
		      }
		    else
		      {
			if(tx > x && tx <= zx)
			  {
			    sitem = item;
			    zx = tx;
			  }
		      }
		  }
		return(sitem);
	      }
	    k++;
	    ww = (ww + 1 ) << 1;
	    hh  =(hh + 1 ) << 1;
	  }
      }
    return(ositem);
  }
}
/***********************************************************************************************/
static void EZ_IWorkAreaClearHighlight(widget, Rx,Ry,Rw, Rh) 
     EZ_Widget *widget;  
     int Rx, Ry, Rw, Rh;
{
  int rubberband = EZ_IWorkAreaRubberBand(widget);
  int mapped = EZ_WidgetMapped(widget);
  if(rubberband)
    {
      EZ_Item *item, **data = EZ_IWorkAreaData(widget);
      int i, nitems = EZ_IWorkAreaNItems(widget);
      Window win = EZ_WidgetWindow(widget);
      for(i = 0; i < nitems; i++)
	{
	  item = data[i];
	  if(EZ_ItemHighlighted(item))
	    {
	      EZ_ItemHighlighted(item) = 0;
	      if(mapped) EZ_UpdateOneItem(widget, win, item, Rx,Ry,Rw,Rh);
	    }
	}
    }
  else
    {
      EZ_Item *ositem = EZ_IWorkAreaSelection(widget);
      if(ositem)
	{
	  EZ_ItemHighlighted(ositem) = 0;
	  if(mapped) EZ_UpdateOneItem(widget,EZ_WidgetWindow(widget),ositem, Rx,Ry,Rw,Rh);
	}    
    }
}
/*****************************************************************************************/
static int EZ_IWorkAreaCountHighlightItems(widget)
     EZ_Widget *widget;  
{
  EZ_Item *item, **data = EZ_IWorkAreaData(widget);
  int ans = 0,  i, nitems = EZ_IWorkAreaNItems(widget);
  for(i = 0; i < nitems; i++)
    {
      item = data[i];
      if(EZ_ItemHighlighted(item)) ans++;
    }
  return(ans);
}

static void EZ_HighlightItemsInRectangle(widget, rx,ry,rw,rh)
     EZ_Widget *widget; int rx, ry, rw, rh;
{
  EZ_Item **data = EZ_IWorkAreaData(widget);
  int i, nitems = EZ_IWorkAreaNItems(widget);
  int hmode = EZ_IWorkAreaHighlightMode(widget);
  int padb = EZ_WidgetPadB(widget);
  int bw = EZ_WidgetBorderWidth(widget) + padb;
  int Rx = bw;
  int Ry = bw;
  int ww = EZ_WidgetWidth(widget);
  int Rw = ww - (bw << 1);
  int hh = EZ_WidgetHeight(widget);
  int Rh = hh -(bw << 1);
  unsigned long fg = EZ_IWorkAreaHighlightFG(widget);
  unsigned long bg = EZ_IWorkAreaHighlightBG(widget);
  Window   win = EZ_WidgetWindow(widget);

  for(i = 0; i < nitems; i++)
    {
      EZ_Item *item = data[i];
      int x0 = EZ_ItemOriginX(item);
      int y0 = EZ_ItemOriginY(item);
      int w0 = EZ_ItemAWidth(item);
      int h0 = EZ_ItemAHeight(item);
      int cx = x0 + (w0>>1);
      int cy = y0 + (h0>>1);
      
      if(cx >= rx && cx <= rx + rw && cy >= ry && cy <= ry + rh)
	{
	  if(EZ_ItemHighlighted(item) == 0)
	    {
	      EZ_ItemHighlighted(item) = 1;
	      if(win)  EZ_HighlightOneItem(widget, win, item, fg, bg, Rx,Ry,Rw,Rh, hmode);
	    }
	}
      else if(EZ_ItemHighlighted(item))
	{
	  EZ_ItemHighlighted(item) = 0;
	  if(win) EZ_UpdateOneItem(widget, win,item, Rx,Ry,Rw,Rh);
	}
    }
}
/******************************************************************************************/
EZ_Item  **EZ_IWorkAreaGetHighlightedItemsA(widget, cnt_ret) EZ_Widget *widget; int *cnt_ret;
{
  EZ_Item **ret = NULL, *item, **data = EZ_IWorkAreaData(widget);
  int cnt = 0,  i, nitems = EZ_IWorkAreaNItems(widget);
  for(i = 0; i < nitems; i++)
    {
      item = data[i];
      if(EZ_ItemHighlighted(item)) cnt++;
    }
  if(cnt > 0)
    {
      EZ_Item **ptr;
      ret = (EZ_Item **)malloc((cnt+1)*sizeof(EZ_Item *));
      for(ptr = ret, i = 0; i < nitems; i++)
	{
	  item = data[i];
	  if(EZ_ItemHighlighted(item)) *ptr++ = item;
	}
      *ptr = NULL;
    }
  if(cnt_ret) *cnt_ret = cnt;
  return(ret);
}
int EZ_IWorkAreaGetHighlightedItems(widget, ary, siz)
     EZ_Widget *widget; EZ_Item **ary; int siz;
{
  int cnt = 0;
  EZ_Item **hitems = EZ_IWorkAreaGetHighlightedItemsA(widget, &cnt);
  if(hitems)
    {
      int i, total = cnt < siz? cnt: siz;
      for(i = 0; i < total; i++) ary[i] = hitems[i];
      (void)free(hitems);
    }
  return(cnt);
}
/******************************************************************************************/
void EZ_IWorkAreaMoveOneItem(widget, nsitem, x,y, Rx,Ry,Rw,Rh)
     EZ_Widget *widget; EZ_Item *nsitem; int Rx,Ry,Rw,Rh,x,y;
{
  XEvent    xevent;
  int       Ox, Oy, oldx, oldy, dx,dy, tx,ty, done;
  int       ix0,iy0,iww,ihh, iarea, snap = 0, moved = 0,iax0,iay0, Sx,Sy, rootx,rooty;
  int       wox,woy, grabed;
  Window    root, child;
  unsigned  int    mask;

  Ox = Oy = 0; oldx = oldy = 1; iww = ihh = 1; 
  ix0 = iy0 = iax0 = iay0 = Sx = Sy = iarea = 0; /* */
  done = 0;
  moved = 0;   /* catch the first motion */
  grabed = 0; /* has picked up an item or not */
  while(!done)
    {
      do {
	XNextEvent(EZ_Display, &xevent); 
	if(EZ_FilterEvent(&xevent))
	  EZ_InvokePrivateEventHandler(&xevent);
			
	if(xevent.type == MotionNotify) moved = 1;
	else if(xevent.type == ButtonRelease)
	  {
	    if(xevent.xbutton.button == EZ_Btn1) { done = 1; break;}
	  }
	else if(xevent.type == Expose || xevent.type == FocusIn || xevent.type == FocusOut)  
	  EZ_WidgetDispatchEvent(&xevent);
      } while(XPending(EZ_Display) && !done);
	  
      if(!done) /* not done yet */
	{
	  if(moved)
	    {
	      if(grabed == 0)  /* pointer moved, start drag */
		{
		  XTranslateCoordinates(EZ_Display, EZ_WidgetWindow(widget),
					RootWindow(EZ_Display, EZ_ScreenNum),
					0, 0,
					&wox,&woy, &child);
		  ix0 = EZ_ItemOriginX(nsitem);
		  iy0 = EZ_ItemOriginY(nsitem);
		  iax0 = EZ_ItemAOriginX(nsitem);
		  iay0 = EZ_ItemAOriginY(nsitem);
		  iww = EZ_ItemAWidth(nsitem);
		  ihh = EZ_ItemAHeight(nsitem);
		  iarea  = iww * ihh;
				  
		  oldx = wox + x;    /* pointer position in Root */
		  oldy = woy + y;
		  tx = ix0 - x;     /* offset: item origin and pointer position */
		  ty = iy0 - y;
		  Sx = Ox = oldx + tx;
		  Sy = Oy = oldy + ty;
				  
		  /* create a snapshot of the item */
		  if(iarea < 40000 && EZ_MakeSnapShot(widget,1,ix0,iy0,iww,ihh)) snap = 1; 
		  grabed = 1;
		}
	      XQueryPointer(EZ_Display, RootWindow(EZ_Display, EZ_ScreenNum),
			    &root,       /* root return win */
			    &child,      /* child ret win   */
			    &rootx, &rooty,   /* x, y in root    */
			    &tx,&ty,            /* x, y in win     */
			    &mask);  
	      dx = rootx - oldx;  dy = rooty - oldy;
	      oldx = rootx;   oldy = rooty;
		  
	      if(dx | dy)
		{
		  Ox += dx; Oy += dy;
		  EZ_MoveSnapShotOrOutline(snap, Ox, Oy, iww, ihh, 1);
		  EZ_MoveSnapShotOrOutline(snap, Ox, Oy, iww, ihh, 0);
		  XFlush(EZ_Display);
		}
	    }/* moved */
	} /* not done */
      else /* done */
	{
	  if(grabed) /* dragged */
	    {
	      int nx,ny,pw,ph;
	      int inside = 1;

	      EZ_MoveSnapShotOrOutline(snap, 0,0,0,0,1);
	      XFlush(EZ_Display);

	      pw = EZ_WidgetWidth(widget);
	      ph = EZ_WidgetHeight(widget);
	      dx = Sx - Ox;
	      dy = Sy - Oy;

	      nx = ix0 - dx;
	      ny = iy0 - dy;
	      {
		int x,y,w,h;
		EZ_IntersectRectangles(0,0,pw,ph,nx,ny,iww,ihh, &x,&y, &w, &h);
		if( ((w * h) << 2) <= iww * ihh) inside = 0; /* > 3/4 is out */
	      }
	      if(inside == 0)
		{
		  int i, count, tx, ty,txx,tyy,dxx,dyy,sx,sy;
		  if(iarea < 1000) count = 128;
		  else if(iarea < 2000) count = 64;
		  else if(iarea < 3000) count = 32;
		  else if(iarea < 10000) count = 16;
		  else if(iarea < 20000) count = 8;
		  else count = 4;

		  if(snap == 0) count = count >> 2;

		  i = (ABSV(dx) + ABSV(dy)) >> 8;
		  count *= (i + 1);

		  tx = dx/count; dxx = dx - tx * count;
		  ty = dy/count; dyy = dy - ty * count;

		  sx = sy = 1;
		  if(dxx < 0) { sx= -1; dxx = -dxx;}
		  if(dyy < 0) { sy= -1; dyy = -dyy;}
                                  
		  txx = tyy = 0;
		  for(i = 0; i < count; i++)
		    {
		      txx += dxx;  if(txx >= count) { txx -= count; Ox += sx;}
		      tyy += dyy;  if(tyy >= count) { tyy -= count; Oy += sy;}
		      Ox += tx;
		      Oy += ty;
		      EZ_MoveSnapShotOrOutline(snap, Ox, Oy, iww, ihh, 1);
		      EZ_MoveSnapShotOrOutline(snap, Ox, Oy, iww, ihh, 0);
		      XFlush(EZ_Display);
		    }
		  EZ_MoveSnapShotOrOutline(snap, 0, 0, 0, 0, 1);
		}
	      else
		{
		  if(EZ_ItemType(nsitem) != EZ_WIDGET_ITEM && EZ_WidgetType(widget) == EZ_WIDGET_IWORK_AREA)
		    MoveIWorkAreaItem(widget, nsitem, nx, ny,
				      iax0 - dx, iay0 - dy, Rx,Ry,Rw,Rh);
		  else /* widget_item is troublesome, */
		    {
		      EZ_ItemOriginX(nsitem) = nx;
		      EZ_ItemOriginY(nsitem) = ny;
		      EZ_ItemAOriginX(nsitem) = iax0 - dx;
		      EZ_ItemAOriginY(nsitem) = iay0 - dy;
		      EZ_DrawWidget(widget);
		    }
		}
	    }
	}
    } /* while */
}

/**************************************************************************************/
static void EZ_IWorkAreaMoveNItems(widget, nitems, nsitem, x,y, Rx,Ry,Rw,Rh)
     EZ_Widget *widget; EZ_Item *nsitem; /* item under the pointer */
     int nitems; int Rx,Ry,Rw,Rh,x,y; /* x,y, init ptr position */
{
  XEvent    xevent;
  int       Ox, Oy, oldx, oldy, dx,dy, tx,ty, done;
  int       moved = 0, dcnt = 0, Sx,Sy, rootx,rooty;
  int       wox=0,woy=0, grabed;
  Window    root, child;
  unsigned  int    mask;

  Ox = Oy = 0; oldx = oldy = 1; 
  Sx = Sy = 0; /* */
  done = 0;
  moved = 0;   /* catch the first motion */
  grabed = 0; /* has picked up an item or not */
  while(!done)
    {
      do {
	XNextEvent(EZ_Display, &xevent); 
	if(EZ_FilterEvent(&xevent))
	  EZ_InvokePrivateEventHandler(&xevent);
			
	if(xevent.type == MotionNotify) moved = 1;
	else if(xevent.type == ButtonRelease)
	  {
	    if(xevent.xbutton.button == EZ_Btn1) { done = 1; break;}
	  }
	else if(xevent.type == Expose || xevent.type == FocusIn || xevent.type == FocusOut)  
	  EZ_WidgetDispatchEvent(&xevent);
      } while(XPending(EZ_Display) && !done);
      
      if(!done) /* not done yet */
	{
	  if(moved)
	    {
	      if(grabed == 0)  /* pointer moved, start drag */
		{
		  XTranslateCoordinates(EZ_Display, EZ_WidgetWindow(widget),
					RootWindow(EZ_Display, EZ_ScreenNum),
					0, 0, &wox,&woy, &child);
		  /* fill up rectangles */
		  EZ_IWorkAreaMakeNItemOutlines(widget,nitems,wox,woy);
		  oldx = wox + x;    /* pointer position in Root */
		  oldy = woy + y;
		  Sx = oldx;
		  Sy = oldy;
		  grabed = 1;
		}
	      XQueryPointer(EZ_Display, RootWindow(EZ_Display, EZ_ScreenNum),
			    &root,       /* root return win */
			    &child,      /* child ret win   */
			    &rootx, &rooty,   /* x, y in root    */
			    &tx,&ty,            /* x, y in win     */
			    &mask);  
	      dx = rootx - oldx;  dy = rooty - oldy;
	      oldx = rootx;   oldy = rooty;
	      
	      if(dx | dy)
		{
		  EZ_MoveNItemOutlines(dx,dy, 0, dcnt,0);
		  dcnt++;
		  EZ_MoveNItemOutlines(dx,dy, 1, dcnt,0);
		  XFlush(EZ_Display);
		}
	    }/* moved */
	} /* not done */
      else /* done */
	{
	  if(grabed) /* dragged */
	    {
	      int pw,ph;
	      int inside = 1;

	      EZ_MoveNItemOutlines(0,0, 0,1,0); /* erase */
	      
	      pw = EZ_WidgetWidth(widget);
	      ph = EZ_WidgetHeight(widget);

	      if(oldx < wox || oldx > wox + pw || oldy < woy || oldy > woy + ph)
		inside = 0;

	      if(inside == 0)
		{
		  int i, count, tx, ty,txx,tyy,dxx,dyy,sx,sy,ax,ay;
		  dx = Sx - oldx;
		  dy = Sy - oldy;
		  i = (ABSV(dx) + ABSV(dy)) >> 8;
		  count = 64 * (i + 1);
		  
		  tx = dx/count; dxx = dx - tx * count;
		  ty = dy/count; dyy = dy - ty * count;

		  sx = sy = 1;
		  if(dxx < 0) { sx= -1; dxx = -dxx;}
		  if(dyy < 0) { sy= -1; dyy = -dyy;}
                                  
		  txx = tyy = 0; 
		  for(i = 0; i < count; i++)
		    {
		      ax = ay = 0;
		      txx += dxx;  if(txx >= count) { txx -= count; ax = sx;}
		      tyy += dyy;  if(tyy >= count) { tyy -= count; ay = sy;}
		      
		      EZ_MoveNItemOutlines(tx+ax,ty+ay, 0, i,0);
		      EZ_MoveNItemOutlines(tx+ax,ty+ay, 1, i,0);
		      XFlush(EZ_Display);
		    }
		  EZ_MoveNItemOutlines(0,0, 0, 1,0);
		}
	      else /* move n items */
		{
		  EZ_Item *item, **data = EZ_IWorkAreaData(widget);
		  int  i, nitems = EZ_IWorkAreaNItems(widget);
		  /*unsigned long fg = EZ_IWorkAreaHighlightFG(widget);
		  unsigned long bg = EZ_IWorkAreaHighlightBG(widget);
		  int hmode = EZ_IWorkAreaHighlightMode(widget);
		  */
		  dx = Sx - oldx;
		  dy = Sy - oldy;		      

		  for(i = 0; i < nitems; i++)
		    {
		      item = data[i];
		      if(EZ_ItemHighlighted(item))
			{
			  EZ_ItemOriginX(item) -= dx;
			  EZ_ItemOriginY(item) -= dy;
			  EZ_ItemAOriginX(item) -= dx;
			  EZ_ItemAOriginY(item) -= dy;
			}
		    }
		  EZ_DrawWidget(widget);
		  /*
		  for(i = 0; i < nitems; i++)
		    {
		      item = data[i];
		      if(EZ_ItemHighlighted(item))
			EZ_HighlightOneItem(widget, EZ_WidgetWindow(widget),
					    item, fg, bg, Rx,Ry,Rw,Rh, hmode);
		    }
		    */
		}
	    }
	  else  /* not moved */
	    {
	      EZ_Item *ositem = EZ_IWorkAreaSelection(widget);
	      int hmode = EZ_IWorkAreaHighlightMode(widget);
	      
	      EZ_IWorkAreaClearHighlight(widget, Rx,Ry,Rw, Rh);
	      EZ_IWorkAreaSelection(widget) = nsitem;
	      EZ_ItemHighlighted(nsitem) = 1 ;
	      {
		unsigned long fg = EZ_IWorkAreaHighlightFG(widget);
		unsigned long bg = EZ_IWorkAreaHighlightBG(widget);
		EZ_HighlightOneItem(widget, EZ_WidgetWindow(widget),
				    nsitem, fg, bg, Rx,Ry,Rw,Rh, hmode);
	      }
	      if(ositem != nsitem)
		EZ_HandleMotionCallBacks(widget,EZ_WidgetMotionCallBack(widget));	
	    }
	}
    } /* while */
}
/***********************************************************************************************/

static void EZ_IWorkAreaDnDOneItem(widget, nsitem, kind, event, Rx,Ry,Rw,Rh)
     EZ_Widget *widget; EZ_Item *nsitem;
     int Rx,Ry,Rw,Rh;
     XEvent *event;
     int kind;  /* 0: drag one item, and item has DND encoders, use item as DND obj */
                /* 1: drag one or more items, use widget as DnD obj */  
{
  XEvent    xevent;
  int       x,y, Ox, Oy, oldx, oldy, dx,dy, tx,ty, done;
  int       ix0,iy0,iww,ihh, iarea, snap = 0, moved = 0,iax0,iay0, Sx,Sy, rootx,rooty;
  int       wox,woy, grabed;
  Window    root, child;
  unsigned  int    mask;
  unsigned long serial = 0L;
  int newEvent = 0, dcnt = 0;
  Window ptWin = event->xbutton.window;
  Window oldPtWin = ptWin;
  Window widgetwin = ptWin;
  int dragCancelled = 0;
  int helpRequested = 0;
  unsigned int actionType = (event->xbutton.state) & (ShiftMask | ControlMask | Mod1Mask);

  x = event->xbutton.x;
  y = event->xbutton.y;  
  Ox = Oy = 0; oldx = oldy = 1; iww = ihh = 1; 
  done = 0;
  moved = 0;   /* catch the first motion */
  grabed = 0; /* has picked up an item or not */
  while(!done)
    {
      while(XPending(EZ_Display) && !done)
	{
	  XNextEvent(EZ_Display, &xevent); 
	  if(EZ_FilterEvent(&xevent))
	    EZ_InvokePrivateEventHandler(&xevent);
	  newEvent = 1;
	  if(xevent.type == MotionNotify)
	    {
	      moved = 1; 
	      serial = xevent.xmotion.serial;
	      ptWin = xevent.xmotion.window; 
	      if(xevent.xmotion.window == widgetwin)
		{
		  if(xevent.xmotion.x < 0 ||  xevent.xmotion.y < 0 ||
		     xevent.xmotion.x >= EZ_WidgetWidth(widget) ||  
		     xevent.xmotion.y >= EZ_WidgetHeight(widget))
		    ptWin = 0L;
		}
	      if(helpRequested)
		{
		  EZ_DnDSendCancelHelpMsg();
		  EZ_DnDInfo.ptWin = (unsigned long)ptWin;
		  helpRequested = 0;
		}
	    }
	  else if(xevent.type == ButtonRelease)
	    {
	      if(xevent.xbutton.button == EZ_Btn3) 
		{
		  ptWin = xevent.xbutton.window; 
		  if(xevent.xbutton.window == widgetwin)
		    {
		      if(xevent.xbutton.x < 0 ||  xevent.xbutton.y < 0 ||
			 xevent.xbutton.x >= EZ_WidgetWidth(widget) ||  
			 xevent.xbutton.y >= EZ_WidgetHeight(widget))
			ptWin = 0L;
		    }
		  done = 1; 
		  break;
		}
	    }
	  else if(xevent.type == KeyPress)			
	    {
	      KeySym            keysym;
	      XComposeStatus    compose; 
	      char              tmpbuffer[4];
	      int               count;
	      count = XLookupString(&(xevent.xkey), tmpbuffer, 4, &keysym, &compose);
	      switch(keysym)
		{
		case XK_F1: 
#ifdef XK_KP_F1
		case XK_KP_F1:  /* help */
#endif
		  if(grabed && helpRequested == 0 && ptWin != 0)
		    {
		      EZ_DnDInfo.ptWin = (unsigned long)ptWin;
		      EZ_DnDSendRequestHelpMsg();
		      helpRequested = 1;
		    }
		  break;
		case XK_Escape:   /* cancel */
		  {
		    dragCancelled = 1;
		    done = 1;
		  }
		break;
		default:
		  break;
		}
	    }
	  else if(xevent.type == Expose || xevent.type == FocusIn || xevent.type == FocusOut)  
	    EZ_WidgetDispatchEvent(&xevent);
			  
	  if(oldPtWin != ptWin)
	    {
	      if(grabed && oldPtWin != 0) 
		{
		  unsigned long twin = EZ_DnDInfo.ptWin;
		  EZ_DnDInfo.ptWin = (unsigned long)(oldPtWin);
		  EZ_DnDSendLeaveWindowMsg();
		  EZ_DnDInfo.ptWin = twin;
		}
	      oldPtWin = ptWin;
	    }
	}
      /* check the timer */
      if(newEvent == 0)
	{
	  EZ_CheckDnDTimer();
	  EZ_SitForALittleBit(1000);
	}
      
       if(kind == 0)
	 {
	   if(!done && newEvent) /* not done yet */
	     {
	      newEvent = 0;
	      if(moved)
		{
		  if(grabed == 0)  /* pointer moved, start drag */
		    {
		      XTranslateCoordinates(EZ_Display, widgetwin,
					    RootWindow(EZ_Display, EZ_ScreenNum),
					    0, 0, &wox,&woy, &child);
		      ix0 = EZ_ItemOriginX(nsitem);
		      iy0 = EZ_ItemOriginY(nsitem);
		      iax0 = EZ_ItemAOriginX(nsitem);
		      iay0 = EZ_ItemAOriginY(nsitem);
		      iww = EZ_ItemAWidth(nsitem);
		      ihh = EZ_ItemAHeight(nsitem);
		      iarea  = iww * ihh;
				  
		      oldx = wox + x;    /* pointer position in Root */
		      oldy = woy + y;
		      tx = ix0 - x;     /* offset: item origin and pointer position */
		      ty = iy0 - y;
		      Sx = Ox = oldx + tx;
		      Sy = Oy = oldy + ty;

		      EZ_InitDrag(serial, EZ_DND_OBJ_IS_ITEM, widgetwin,
				  widget, nsitem, Sx,Sy, iww, ihh, oldx, oldy, tx, ty);

		      if(EZ_DnDInfo.id == serial)
			{
			  EZ_CallDragInitProcedure();
				      
			  if(EZ_DnDInfo.cursor == None)
			    EZ_DnDInfo.cursor = EZ_ItemDnDDragCursor(nsitem);
			  if(EZ_DnDInfo.cursor != None || EZ_DnDInfo.dragIcon != NULL)
			    snap = EZ_DND_DRAG_ICON_CURSOR;
			  else if(iarea < 40000)
			    snap = EZ_MakeSnapShot(widget,1,ix0,iy0,iww,ihh);
			  else snap = EZ_DND_DRAG_ICON_OUTLINE;
			  {
			    EZ_DnDInfo.actionType = actionType;
			    EZ_DnDInfo.px = oldx;
			    EZ_DnDInfo.py = oldy;
			    EZ_DnDInfo.dragIconType = snap;
			    {
			      char *smsg; int smsg_len;
			      EZ_GenerateDragStartedMessage(&smsg, &smsg_len);
			      EZ_BroadcastDnDMessage(EZ_DND_DRAG_START,smsg, smsg_len, 0);
			    }
			    /*EZ_BroadcastDnDMessage(EZ_DND_DRAG_START,NULL, 0, 0);*/
			  }
			  if(EZ_PointerGrabed == 0)  /* grab the pointer ! */
			    {
			      if(XGrabPointer(EZ_Display,widgetwin, True,
					      ButtonReleaseMask|ButtonPressMask|PointerMotionMask,
					      GrabModeAsync, GrabModeAsync, None, 
					      EZ_DnDInfo.cursor, CurrentTime)
				 == GrabSuccess) EZ_PointerGrabed = 1;
			    }
			  grabed = 1;
			}
		    }
		  if(grabed)
		    {
		      XQueryPointer(EZ_Display, RootWindow(EZ_Display, EZ_ScreenNum),
				    &root,       /* root return win */
				    &child,      /* child ret win   */
				    &rootx, &rooty,   /* x, y in root    */
				    &tx,&ty,            /* x, y in win     */
				    &mask);  
		      dx = rootx - oldx;  dy = rooty - oldy;
		      oldx = rootx;   oldy = rooty;
				  
		      if(dx | dy)
			{
			  Ox += dx; Oy += dy;
			  if(snap != EZ_DND_DRAG_ICON_CURSOR)
			    EZ_MoveSnapShotOrOutline(snap, Ox, Oy, iww, ihh, 1);
			  else if(EZ_DnDInfo.dragIcon != NULL) EZ_ShowDragIcon(rootx, rooty);
			  EZ_DnDInfo.px = oldx;
			  EZ_DnDInfo.py = oldy;
			  EZ_DnDInfo.ptWin = (unsigned long)ptWin;
			  EZ_BroadcastDnDMessage(EZ_DND_DRAG_MOTION, NULL, 0, 0);
			  if(snap != EZ_DND_DRAG_ICON_CURSOR)
			    EZ_MoveSnapShotOrOutline(snap, Ox, Oy, iww, ihh, 0);
			} 
		    }
		}  /* moved */
	    }/* not done */
	  else if(done) /* done */
	    {
	      if(EZ_PointerGrabed)
		{
		  XUngrabPointer(EZ_Display,CurrentTime);
		  EZ_PointerGrabed = 0;
		}
	      if(grabed) /* dragged */
		{
		  char *message;
		  int length;
		  Window commWin;
		  if(snap != EZ_DND_DRAG_ICON_CURSOR)
		    EZ_MoveSnapShotOrOutline(snap, 0,0,0,0,1);
		  else if(EZ_DnDInfo.dragIcon != NULL) EZ_HideWidget(EZ_DnDInfo.dragIcon);
		  if(ptWin != 0) commWin = (Window) EZ_WindowIsDnDTarget(ptWin);
		  else commWin = 0L;
			      
		  /* have to call the next func to release grab !!! */
		  EZ_FinishDrag(commWin, ptWin, oldx, oldy); /* prepare for drop */

		  if(commWin != 0L)
		    {
		      if(helpRequested)
			{
			  EZ_DnDSendCancelHelpMsg();
			  helpRequested = 0;
			}

		      if(dragCancelled == 0)
			{
			  EZ_DnDInfo.actionType = actionType;
			  EZ_GenerateDragIsDropedMessage(&message, &length);
			  EZ_SendDnDMessage(EZ_DND_DRAG_DROPPED, message, length, 1);
			}
		      else  /* cancelled */
			{
			  EZ_SendDnDMessage(EZ_DND_DRAG_CANCELLED, NULL, 0, 0);
			  EZ_AbortDrag();
			}
		    }
		  else if(ptWin == RootWindow(EZ_Display, EZ_ScreenNum) &&
			  dragCancelled == 0)
		    {
		      EZ_DnDDataEncoder  *encoder = EZ_FindSpecialEncoder();
		      if(encoder)
			{
			  char *msg; int mlen, needFree, ok = EZ_DND_FAILURE;

			  EZ_DnDInfo.srcStatus = EZ_DRAG_CONVERTING;
			  ok = (encoder->encoder) (encoder->object,
						   encoder->data,
						   &msg, &mlen, &needFree);
			  if(ok != EZ_DND_FAILURE)
			    {
			      if((encoder->callback).callback)
				{
				  EZ_DnDInfo.srcStatus = EZ_DRAG_EXECUTING_CALLBACK;
				  ((encoder->callback).callback)(encoder->object, 
								 (encoder->callback).data);
				}
			      EZ_DnDInfo.id = 0; /* forget about this drag */   
			    }
			  else EZ_AbortDrag();
			}
		      else  EZ_AbortDrag();
		    }
		  else EZ_AbortDrag();
		}
	    }
	}
      else /* kind = 1 */
	{
	   if(!done && newEvent) /* not done yet */
	     {
	      newEvent = 0;
	      if(moved)
		{
		  if(grabed == 0)  /* pointer moved, start drag */
		    {
		      XTranslateCoordinates(EZ_Display, widgetwin,
					    RootWindow(EZ_Display, EZ_ScreenNum),
					    0, 0, &wox,&woy, &child);
		      oldx = wox + x;    /* pointer position in Root */
		      oldy = woy + y;
		      Sx =  oldx;
		      Sy =  oldy;

		      EZ_InitDrag(serial, EZ_DND_OBJ_IS_ITEM, widgetwin,
				  widget, widget, Sx,Sy, 100, 100, oldx, oldy, 0, 0);
		      
		      if(EZ_DnDInfo.id == serial)
			{
			  EZ_CallDragInitProcedure();
			  
			  if(EZ_DnDInfo.cursor == None)
			    EZ_DnDInfo.cursor = EZ_WidgetDnDDragCursor(widget);
			  if(EZ_DnDInfo.cursor != None || EZ_DnDInfo.dragIcon != NULL)
			    snap = EZ_DND_DRAG_ICON_CURSOR;
			  else 
			    {
			      EZ_IWorkAreaMakeNItemOutlines(widget,1,wox,woy);
			      snap = EZ_DND_DRAG_ICON_SPECIAL;
			    }
			  {
			    EZ_DnDInfo.actionType = actionType;
			    EZ_DnDInfo.px = oldx;
			    EZ_DnDInfo.py = oldy;
			    EZ_DnDInfo.dragIconType = snap;
			    {
			      char *smsg; int smsg_len;
			      EZ_GenerateDragStartedMessage(&smsg, &smsg_len);
			      EZ_BroadcastDnDMessage(EZ_DND_DRAG_START,smsg, smsg_len, 0);
			    }
			    /*EZ_BroadcastDnDMessage(EZ_DND_DRAG_START,NULL, 0, 0);*/
			  }
			  if(EZ_PointerGrabed == 0)  /* grab the pointer ! */
			    {
			      if(XGrabPointer(EZ_Display,widgetwin, True,
					      ButtonReleaseMask|ButtonPressMask|PointerMotionMask,
					      GrabModeAsync, GrabModeAsync, None, 
					      EZ_DnDInfo.cursor, CurrentTime)
				 == GrabSuccess) EZ_PointerGrabed = 1;
			    }
			  grabed = 1;
			}
		    }
		  if(grabed)
		    {
		      XQueryPointer(EZ_Display, RootWindow(EZ_Display, EZ_ScreenNum),
				    &root,       /* root return win */
				    &child,      /* child ret win   */
				    &rootx, &rooty,   /* x, y in root    */
				    &tx,&ty,            /* x, y in win     */
				    &mask);  
		      dx = rootx - oldx;  dy = rooty - oldy;
		      oldx = rootx;   oldy = rooty;
				  
		      if(dx | dy)
			{
			  Ox += dx; Oy += dy;
			  if(snap == EZ_DND_DRAG_ICON_SPECIAL)
			    {
			      EZ_MoveNItemOutlines(dx,dy, 0, dcnt,1);
			      dcnt++;
			      EZ_MoveNItemOutlines(dx,dy, 1, dcnt,1);
			      XFlush(EZ_Display);
			    }
			  else if(EZ_DnDInfo.dragIcon != NULL) EZ_ShowDragIcon(rootx, rooty);
			  EZ_DnDInfo.px = oldx;
			  EZ_DnDInfo.py = oldy;
			  EZ_DnDInfo.ptWin = (unsigned long)ptWin;
			  EZ_BroadcastDnDMessage(EZ_DND_DRAG_MOTION, NULL, 0, 0);
			} 
		    }
		}  /* moved */
	     }/* not done */
	   else if(done) /* done */
	     {
	       if(EZ_PointerGrabed)
		 {
		   XUngrabPointer(EZ_Display,CurrentTime);
		   EZ_PointerGrabed = 0;
		 }
	       if(grabed) /* dragged */
		 {
		   char *message;
		   int length;
		   Window commWin;

		   if(snap == EZ_DND_DRAG_ICON_SPECIAL) EZ_MoveNItemOutlines(0,0, 0,1,1); /* erase */
		   else if(EZ_DnDInfo.dragIcon != NULL) EZ_HideWidget(EZ_DnDInfo.dragIcon);
		   if(ptWin != 0) commWin = (Window) EZ_WindowIsDnDTarget(ptWin);
		   else commWin = 0L;
			      
		  /* have to call the next func to release grab !!! */
		  EZ_FinishDrag(commWin, ptWin, oldx, oldy); /* prepare for drop */

		  if(commWin != 0L)
		    {
		      if(helpRequested)
			{
			  EZ_DnDSendCancelHelpMsg();
			  helpRequested = 0;
			}

		      if(dragCancelled == 0)
			{
			  EZ_DnDInfo.actionType = actionType;
			  EZ_GenerateDragIsDropedMessage(&message, &length);
			  EZ_SendDnDMessage(EZ_DND_DRAG_DROPPED, message, length, 1);
			}
		      else  /* cancelled */
			{
			  EZ_SendDnDMessage(EZ_DND_DRAG_CANCELLED, NULL, 0, 0);
			  EZ_AbortDrag();
			}
		    }
		  else if(ptWin == RootWindow(EZ_Display, EZ_ScreenNum) &&
			  dragCancelled == 0)
		    {
		      EZ_DnDDataEncoder  *encoder = EZ_FindSpecialEncoder();
		      if(encoder)
			{
			  char *msg; int mlen, needFree, ok = EZ_DND_FAILURE;

			  EZ_DnDInfo.srcStatus = EZ_DRAG_CONVERTING;
			  ok = (encoder->encoder) (encoder->object,
						   encoder->data,
						   &msg, &mlen, &needFree);
			  if(ok != EZ_DND_FAILURE)
			    {
			      if((encoder->callback).callback)
				{
				  EZ_DnDInfo.srcStatus = EZ_DRAG_EXECUTING_CALLBACK;
				  ((encoder->callback).callback)(encoder->object, 
								 (encoder->callback).data);
				}
			      EZ_DnDInfo.id = 0; /* forget about this drag */   
			    }
			  else EZ_AbortDrag();
			}
		      else  EZ_AbortDrag();
		    }
		  else EZ_AbortDrag();
		}
	     }
	}
    } /* while */
}

/*************************************************************************************************/
static void EZ_IWorkAreaDnDNItems(widget, nhitems, event, Rx,Ry,Rw,Rh)
     EZ_Widget *widget; 
     int Rx,Ry,Rw,Rh;
     XEvent *event; 
     int nhitems;
{
  XEvent    xevent;
  int       x,y, Ox, Oy, oldx, oldy, dx,dy, tx,ty, done;
  int       snap = 0, moved = 0,Sx,Sy, rootx,rooty;
  int       wox,woy, grabed;
  Window    root, child;
  unsigned  int    mask;
  unsigned long serial = 0L;
  int newEvent = 0, dcnt = 0;
  Window ptWin = event->xbutton.window;
  Window oldPtWin = ptWin;
  Window widgetwin = ptWin;
  int dragCancelled = 0;
  int helpRequested = 0;
  unsigned int actionType = (event->xbutton.state) & (ShiftMask | ControlMask | Mod1Mask);

  x = event->xbutton.x;
  y = event->xbutton.y;  
  Ox = Oy = 0; oldx = oldy = 1; 
  done = 0;
  moved = 0;   /* catch the first motion */
  grabed = 0; /* has picked up an item or not */
  while(!done)
    {
      while(XPending(EZ_Display) && !done)
	{
	  XNextEvent(EZ_Display, &xevent); 
	  if(EZ_FilterEvent(&xevent))
	    EZ_InvokePrivateEventHandler(&xevent);
	  newEvent = 1;
	  if(xevent.type == MotionNotify)
	    {
	      moved = 1; 
	      serial = xevent.xmotion.serial;
	      ptWin = xevent.xmotion.window; 
	      if(xevent.xmotion.window == widgetwin)
		{
		  if(xevent.xmotion.x < 0 ||  xevent.xmotion.y < 0 ||
		     xevent.xmotion.x >= EZ_WidgetWidth(widget) ||  
		     xevent.xmotion.y >= EZ_WidgetHeight(widget))
		    ptWin = 0L;
		}
	      if(helpRequested)
		{
		  EZ_DnDSendCancelHelpMsg();
		  EZ_DnDInfo.ptWin = (unsigned long)ptWin;
		  helpRequested = 0;
		}
	    }
	  else if(xevent.type == ButtonRelease)
	    {
	      if(xevent.xbutton.button == EZ_Btn3) 
		{
		  ptWin = xevent.xbutton.window; 
		  if(xevent.xbutton.window == widgetwin)
		    {
		      if(xevent.xbutton.x < 0 ||  xevent.xbutton.y < 0 ||
			 xevent.xbutton.x >= EZ_WidgetWidth(widget) ||  
			 xevent.xbutton.y >= EZ_WidgetHeight(widget))
			ptWin = 0L;
		    }
		  done = 1; 
		  break;
		}
	    }
	  else if(xevent.type == KeyPress)			
	    {
	      KeySym            keysym;
	      XComposeStatus    compose; 
	      char              tmpbuffer[4];
	      int               count;
	      count = XLookupString(&(xevent.xkey), tmpbuffer, 4, &keysym, &compose);
	      switch(keysym)
		{
		case XK_F1: 
#ifdef XK_KP_F1
		case XK_KP_F1:  /* help */
#endif
		  if(grabed && helpRequested == 0 && ptWin != 0)
		    {
		      EZ_DnDInfo.ptWin = (unsigned long)ptWin;
		      EZ_DnDSendRequestHelpMsg();
		      helpRequested = 1;
		    }
		  break;
		case XK_Escape:   /* cancel */
		  {
		    dragCancelled = 1;
		    done = 1;
		  }
		break;
		default:
		  break;
		}
	    }
	  else if(xevent.type == Expose || xevent.type == FocusIn || xevent.type == FocusOut)  
	    EZ_WidgetDispatchEvent(&xevent);
			  
	  if(oldPtWin != ptWin)
	    {
	      if(grabed && oldPtWin != 0) 
		{
		  unsigned long twin = EZ_DnDInfo.ptWin;
		  EZ_DnDInfo.ptWin = (unsigned long)(oldPtWin);
		  EZ_DnDSendLeaveWindowMsg();
		  EZ_DnDInfo.ptWin = twin;
		}
	      oldPtWin = ptWin;
	    }
	}
      /* check the timer */
      if(newEvent == 0)
	{
	  EZ_CheckDnDTimer();
	  EZ_SitForALittleBit(1000);
	}
      
      if(!done && newEvent) /* not done yet */
	{
	  newEvent = 0;
	  if(moved)
	    {
	      if(grabed == 0)  /* pointer moved, start drag */
		{
		  XTranslateCoordinates(EZ_Display, widgetwin,
					RootWindow(EZ_Display, EZ_ScreenNum),
					0, 0, &wox,&woy, &child);
		  oldx = wox + x;    /* pointer position in Root */
		  oldy = woy + y;
		  Sx =  oldx;
		  Sy =  oldy;

		  EZ_InitDrag(serial, EZ_DND_OBJ_IS_WIDGET, widgetwin,
			      widget, widget, Sx,Sy, 100, 100, oldx, oldy, 0, 0);
		  
		  if(EZ_DnDInfo.id == serial)
		    {
		      EZ_CallDragInitProcedure();
			  
		      if(EZ_DnDInfo.cursor == None)
			EZ_DnDInfo.cursor = EZ_WidgetDnDDragCursor(widget);
		      if(EZ_DnDInfo.cursor != None || EZ_DnDInfo.dragIcon != NULL)
			snap = EZ_DND_DRAG_ICON_CURSOR;
		      else 
			{
			  EZ_IWorkAreaMakeNItemOutlines(widget,nhitems,wox,woy);
			  snap = EZ_DND_DRAG_ICON_SPECIAL;
			}
		      {
			EZ_DnDInfo.actionType = actionType;
			EZ_DnDInfo.px = oldx;
			EZ_DnDInfo.py = oldy;
			EZ_DnDInfo.dragIconType = snap;
			{
			  char *smsg; int smsg_len;
			  EZ_GenerateDragStartedMessage(&smsg, &smsg_len);
			  EZ_BroadcastDnDMessage(EZ_DND_DRAG_START,smsg, smsg_len, 0);
			}
			/*EZ_BroadcastDnDMessage(EZ_DND_DRAG_START,NULL, 0, 0);*/
		      }
		      if(EZ_PointerGrabed == 0)  /* grab the pointer ! */
			{
			  if(XGrabPointer(EZ_Display,widgetwin, True,
					  ButtonReleaseMask|ButtonPressMask|PointerMotionMask,
					  GrabModeAsync, GrabModeAsync, None, 
					  EZ_DnDInfo.cursor, CurrentTime)
			     == GrabSuccess) EZ_PointerGrabed = 1;
			}
		      grabed = 1;
		    }
		}
	      if(grabed)
		{
		  XQueryPointer(EZ_Display, RootWindow(EZ_Display, EZ_ScreenNum),
				&root,       /* root return win */
				&child,      /* child ret win   */
				&rootx, &rooty,   /* x, y in root    */
				&tx,&ty,            /* x, y in win     */
				&mask);  
		  dx = rootx - oldx;  dy = rooty - oldy;
		  oldx = rootx;   oldy = rooty;
				  
		  if(dx | dy)
		    {
		      Ox += dx; Oy += dy;
		      if(snap == EZ_DND_DRAG_ICON_SPECIAL)
			{
			  EZ_MoveNItemOutlines(dx,dy, 0, dcnt,1);
			  dcnt++;
			  EZ_MoveNItemOutlines(dx,dy, 1, dcnt,1);
			  XFlush(EZ_Display);
			}
		      else if(EZ_DnDInfo.dragIcon != NULL) EZ_ShowDragIcon(rootx, rooty);
		      EZ_DnDInfo.px = oldx;
		      EZ_DnDInfo.py = oldy;
		      EZ_DnDInfo.ptWin = (unsigned long)ptWin;
		      EZ_BroadcastDnDMessage(EZ_DND_DRAG_MOTION, NULL, 0, 0);
		    } 
		}
	    }  /* moved */
	}/* not done */
      else if(done) /* done */
	{
	  if(EZ_PointerGrabed)
	    {
	      XUngrabPointer(EZ_Display,CurrentTime);
	      EZ_PointerGrabed = 0;
	    }
	  if(grabed) /* dragged */
	    {
	      char *message;
	      int length;
	      Window commWin;

	      if(snap == EZ_DND_DRAG_ICON_SPECIAL) EZ_MoveNItemOutlines(0,0, 0,1,1); /* erase */
	      else if(EZ_DnDInfo.dragIcon != NULL) EZ_HideWidget(EZ_DnDInfo.dragIcon);
	      if(ptWin != 0) commWin = (Window) EZ_WindowIsDnDTarget(ptWin);
	      else commWin = 0L;
			      
	      /* have to call the next func to release grab !!! */
	      EZ_FinishDrag(commWin, ptWin, oldx, oldy); /* prepare for drop */

	      if(commWin != 0L)
		{
		  if(helpRequested)
		    {
		      EZ_DnDSendCancelHelpMsg();
		      helpRequested = 0;
		    }

		  if(dragCancelled == 0)
		    {
		      EZ_DnDInfo.actionType = actionType;
		      EZ_GenerateDragIsDropedMessage(&message, &length);
		      EZ_SendDnDMessage(EZ_DND_DRAG_DROPPED, message, length, 1);
		    }
		  else  /* cancelled */
		    {
		      EZ_SendDnDMessage(EZ_DND_DRAG_CANCELLED, NULL, 0, 0);
		      EZ_AbortDrag();
		    }
		}
	      else if(ptWin == RootWindow(EZ_Display, EZ_ScreenNum) &&
		      dragCancelled == 0)
		{
		  EZ_DnDDataEncoder  *encoder = EZ_FindSpecialEncoder();
		  if(encoder)
		    {
		      char *msg; int mlen, needFree, ok = EZ_DND_FAILURE;
		      
		      EZ_DnDInfo.srcStatus = EZ_DRAG_CONVERTING;
		      ok = (encoder->encoder) (encoder->object,
					       encoder->data,
					       &msg, &mlen, &needFree);
		      if(ok != EZ_DND_FAILURE)
			{
			  if((encoder->callback).callback)
			    {
			      EZ_DnDInfo.srcStatus = EZ_DRAG_EXECUTING_CALLBACK;
			      ((encoder->callback).callback)(encoder->object, 
							     (encoder->callback).data);
			    }
			  EZ_DnDInfo.id = 0; /* forget about this drag */   
			}
		      else EZ_AbortDrag();
		    }
		  else  EZ_AbortDrag();
		}
	      else EZ_AbortDrag();
	    }
	}
    } /* while */
}
/******************************************************************************************/
static void EZ_IWorkAreaMakeNItemOutlines(widget, nnn, ox,oy) 
     EZ_Widget *widget; int nnn, ox,oy;
{
  EZ_Item **items, *itemsStor[256];
  if(nnn >= 256)
    items = (EZ_Item **)malloc( (nnn+1) * sizeof(EZ_Item *));
  else items = itemsStor;
  {
    EZ_Item **ptr = items, *item, **data = EZ_IWorkAreaData(widget);
    int  i, nitems = EZ_IWorkAreaNItems(widget);
    for(i = 0; i < nitems; i++)
      {
	item = data[i];
	if(EZ_ItemHighlighted(item))
	  *ptr++ = item;
      }
  }
  EZ_MakeNItemOutlines(items, nnn, ox,oy);
  if(nnn >= 256) (void)free(items);
}
/*****************************************************************************************
 *
 *  Used in other files also.
 */
static XRectangle *mvRects = NULL;
static int         nMvRects = 0;
static int         mvRectSpaces = 0;

void EZ_MakeNItemOutlines(items, nnn, ox,oy) 
     EZ_Item **items; int nnn, ox,oy;
{
  XRectangle *xrect;
  EZ_Item    *item;
  int i, counter;

  if(mvRectSpaces + 2 <= nnn)
    {
      if(mvRects == NULL) mvRects = (XRectangle *)my_malloc( (nnn+32)*sizeof(XRectangle),_IWH_ITEM_DATA_);
      else mvRects = (XRectangle *)my_realloc(mvRects, (nnn+32)*sizeof(XRectangle),_IWH_ITEM_DATA_);
      mvRectSpaces = nnn + 32;
    }
  xrect = mvRects;
  
  for(counter=0,i = 0; i < nnn; i++)
    {
      item = items[i];
      if(EZ_ItemHighlighted(item)) 
	{
	  int ix0 = EZ_ItemOriginX(item)+1;
	  int iy0 = EZ_ItemOriginY(item)+1;
	  int iww = EZ_ItemAWidth(item)-2;
	  int ihh = EZ_ItemAHeight(item)-2;	  
	  xrect->x = (short)(ix0 + ox) ;
	  xrect->y = (short)(iy0 + oy);
	  xrect->width = (unsigned short) iww;
	  xrect->height = (unsigned short) ihh;
	  xrect++;
	  counter++;
	}
    }
  nMvRects = counter;
}
void EZ_IFListBoxMakeNRowOutLines(widget, nnn,wox,woy)
     EZ_Widget *widget; int nnn; int wox, woy;
{
  XRectangle *xrect;
  EZ_Item    ***data, **row, *item;
  int i, j, counter, nrows, ncols, x0, y0, x1, y1;

  if(mvRectSpaces + 2 <= nnn)
    {
      if(mvRects == NULL) mvRects = (XRectangle *)my_malloc( (nnn+32)*sizeof(XRectangle),_IWH_ITEM_DATA_);
      else mvRects = (XRectangle *)my_realloc(mvRects, (nnn+32)*sizeof(XRectangle),_IWH_ITEM_DATA_);
      mvRectSpaces = nnn + 32;
    }
  xrect = mvRects;
  data = EZ_IFListBoxData(widget);
  nrows = EZ_IFListBoxRows(widget);
  ncols = EZ_IFListBoxColumns(widget);

  x0=y0=0; x1=y1=3;
  for(counter=0,i = 0; i < nrows; i++)
    {
      row = data[i];
      item = row[0];
      if(item && EZ_ItemHighlighted(item))
	{
	  x0 = EZ_ItemOriginX(item); y0 = EZ_ItemOriginY(item); 
	  x1 = EZ_ItemOriginX(item) + EZ_ItemAWidth(item);
	  y1 = EZ_ItemOriginY(item) + EZ_ItemAHeight(item);
	  for(j = 1; j < ncols; j++)
	    {
	      item = row[j];
	      if(item) x1 = EZ_ItemOriginX(item) + EZ_ItemAWidth(item);
	    }

	  xrect->x = (short)(wox+x0 -1) ;
	  xrect->y = (short)(woy+y0 -1);
	  xrect->width = (unsigned short)(x1 - x0 - 2);
	  xrect->height = (unsigned short)(y1 - y0 -2);
	  xrect++;
	  counter++;
	}
    }
  nMvRects = counter;  
}

void EZ_IFListBoxMakeNItemOutLines(widget, nnn,wox,woy)
     EZ_Widget *widget; int nnn, wox, woy;
{
  EZ_Item **items, *itemsStor[256];
  if(nnn >= 256)  items = (EZ_Item **)malloc( (nnn+1) * sizeof(EZ_Item *));
  else items = itemsStor;
  {
    EZ_Item    ***data, **row, *item;
    int i,j,nrows,ncols, counter = 0;
    data = EZ_IFListBoxData(widget);
    nrows = EZ_IFListBoxRows(widget);
    ncols = EZ_IFListBoxColumns(widget);

    for(counter = 0, i = 0; i < nrows; i++)
      {
	row = data[i];
	for(j = 0; j < ncols; j++)
	  {
	    item = row[j];
	    if(item && EZ_ItemHighlighted(item))
	      items[counter++] = item;
	  }
      }
  }
  EZ_MakeNItemOutlines(items, nnn, wox,woy);
  if(nnn >= 256) (void)free(items);
}
/******************************************************************************************/
void EZ_MoveNItemOutlines(dx, dy, flag, dcnt, dndflag)
     int dx,dy, flag, dcnt, dndflag;
{
  XRectangle *rects = mvRects;
  int i, nitems = nMvRects;
  Window win;

  if(nitems <= 0) return;

  win = RootWindow(EZ_Display, EZ_ScreenNum);  
  if(dcnt && !flag) /* erase previous drawing */
    {
      XDrawRectangles(EZ_Display, win, EZ_XORGC, rects, nitems);
      if(dndflag)
	{
	}
    }
  if(flag)
    {
      for(i = 0; i < nitems; i++)
	{
	  rects[i].x += dx;
	  rects[i].y += dy;
	}
      XDrawRectangles(EZ_Display, win, EZ_XORGC, rects, nitems);
      if(dndflag)
	{
	  
	}
    }
  XFlush(EZ_Display);
}
/***********************************************************************************************/
static void EZ_IWorkAreaSelectOneItem(widget, nsitem, Rx,Ry,Rw,Rh)
     EZ_Widget *widget; EZ_Item *nsitem; int Rx, Ry,Rw,Rh;
{
  EZ_Item **data, *newItem, *guess = nsitem, *saveItem = NULL;
  int x, y, moved = 0, done = 0, nitems;
  XEvent xevent;

  x=y=0;
  data = EZ_IWorkAreaData(widget);
  nitems = EZ_IWorkAreaNItems(widget);
  while(!done)
    {
      do{
	XNextEvent(EZ_Display, &xevent);
	if(EZ_FilterEvent(&xevent)) EZ_InvokePrivateEventHandler(&xevent);
	if(xevent.type == MotionNotify)
	  {
	    x = xevent.xmotion.x;
	    y = xevent.xmotion.y;
	    if(x < 0 ) x = 0; else if(x > Rw) x = Rw;
	    if(y < 0 ) y = 0; else if(y > Rh) y = Rh;
	    moved = 1;
	  }
	else if(xevent.type == ButtonRelease)
	  {
	    if(xevent.xbutton.button == EZ_Btn1) { done = 1; break;}
	  }
	else if(xevent.type == Expose || xevent.type == FocusIn || xevent.type == FocusOut)
	  EZ_WidgetDispatchEvent(&xevent);
      } while(XPending(EZ_Display) && !done) ;

      if(!done)
	{
	  if(moved)
	    {
	      newItem = EZ_FindPointerOnItemIWA(data, nitems, guess, x,y);
	      if(newItem) guess = newItem;
	      if(newItem != nsitem && newItem != saveItem)
		{
		  if(saveItem)
		    EZ_UpdateOneItem(widget,EZ_WidgetWindow(widget),saveItem, Rx, Ry, Rw, Rh);
		  if(newItem && EZ_ItemType(newItem) != EZ_FIG_ITEM )
		    {
		      EZ_FlashItemOutline(widget,newItem, Rx,Ry,Rw,Rh);
		    }
		  saveItem = newItem;
		}
	    }
	}
      else /* done */
	{
	  if(saveItem) /* cannot be the current selection */
	    {
	      unsigned long fg = EZ_IWorkAreaHighlightFG(widget);
	      unsigned long bg = EZ_IWorkAreaHighlightBG(widget);
	      int hmode = EZ_IWorkAreaHighlightMode(widget);  

	      /* remove old selection */
	      {
		EZ_ItemHighlighted(nsitem) = 0 ;
		EZ_UpdateOneItem(widget,EZ_WidgetWindow(widget),nsitem, Rx, Ry, Rw, Rh);
	      }
	      EZ_IWorkAreaSelection(widget) = saveItem;
	      EZ_ItemHighlighted(saveItem) = 1;
	      EZ_HighlightOneItem(widget, EZ_WidgetWindow(widget),
				  saveItem, fg, bg, Rx,Ry,Rw,Rh, hmode);
	      EZ_HandleMotionCallBacks(widget,EZ_WidgetMotionCallBack(widget));	
	    }
	}
    }
}
/***********************************************************************************************/
#undef _UP    
#undef _DOWN
#undef _LEFT
#undef _RIGHT
/***********************************************************************************************/
void EZ_WorkAreaRaiseItem(widget, item)
     EZ_Widget *widget; EZ_Item *item;
{
  if(widget && item)
    {
      EZ_Widget *twidget;
      int type = EZ_WidgetType(widget);
      if(type == EZ_WIDGET_IWORK_AREA) twidget = widget;
      else  if(type == EZ_WIDGET_WORK_AREA) twidget = EZ_WorkAreaIWorkArea(widget);
      else twidget = NULL;
      if(twidget)
	{
	  EZ_Item **data = EZ_IWorkAreaData(twidget);
	  int i, nitems = EZ_IWorkAreaNItems(twidget); 	  
	  for(i = 0; i < nitems; i++)
	    {
	      if(data[i] == item)
		{
		  EZ_Item *save = data[nitems-1];
		  data[nitems-1] = item;
		  data[i] = save;
		  {
		    int bw,ww,hh,Rx,Ry,Rw,Rh;
		    bw = EZ_WidgetBorderWidth(twidget) + EZ_WidgetPadB(twidget);
		    Rx = Ry = bw;
		    ww = EZ_WidgetWidth(twidget);
		    Rw = ww - (bw << 1);
		    hh = EZ_WidgetHeight(twidget);
		    Rh = hh -(bw << 1); 
		    if(EZ_ItemHighlighted(item))
		      {
			unsigned long fg = EZ_IWorkAreaHighlightFG(twidget);
			unsigned long bg = EZ_IWorkAreaHighlightBG(twidget);
			int hmode = EZ_IWorkAreaHighlightMode(twidget);
			EZ_HighlightOneItem(twidget,EZ_WidgetWindow(twidget), item, fg, bg, 
					    Rx,Ry,Rw,Rh, hmode);
		      }
		    else
		      EZ_UpdateOneItem(twidget,EZ_WidgetWindow(twidget),item, Rx,Ry,Rw,Rh);
		    XSync(EZ_Display, False);
		  }
		  break;
		}
	    }
	}
    }
}
void EZ_WorkAreaInsertAndDrawItemAt(wgt, item, location, disp_mode)
     EZ_Widget *wgt; EZ_Item *item; int *location, disp_mode;
{
  if(wgt && item)
    {
      EZ_Widget *widget;
      int type = EZ_WidgetType(wgt);
      if(type == EZ_WIDGET_IWORK_AREA) widget = wgt;
      else  if(type == EZ_WIDGET_WORK_AREA) widget = EZ_WorkAreaIWorkArea(wgt);
      else widget = NULL;
      if(widget)
	{
	  int nitems;
	  EZ_Item **data;
	  int x,y, bw,ww,hh,Rx,Ry,Rw,Rh;
	  bw = EZ_WidgetBorderWidth(widget) + EZ_WidgetPadB(widget);
	  Rx = Ry = bw;
	  ww = EZ_WidgetWidth(widget);
	  Rw = ww - (bw << 1);
	  hh = EZ_WidgetHeight(widget);
	  Rh = hh -(bw << 1); 
	  
	  EZ_GrowIWorkAreaDataSpace(widget, 1);
      	  nitems = EZ_IWorkAreaNItems(widget);
	  data = EZ_IWorkAreaData(widget);
	  data[nitems] = item;
	  EZ_IWorkAreaNItems(widget) += 1;
	  EZ_IWorkAreaDirty(widget) = 1;
	  { 
	    int jw, jh;
	    EZ_ComputeItemSize(item, &jw, &jh);
	    if(location)
	      {
		EZ_ItemOriginX(item) = location[0];
		EZ_ItemOriginY(item) = location[1];
		EZ_ItemAOriginX(item) = location[0] - Rx;		
		EZ_ItemAOriginY(item) = location[1] - Ry;		
	      }
	  }
	  if(EZ_WidgetMapped(widget))
	    {
	      if(disp_mode > 0) EZ_DrawWidget(widget);
	      else 
		{
		  x = EZ_ItemOriginX(item);
		  y = EZ_ItemOriginY(item);
		  EZ_IWorkAreaDirty(widget) = 0;
		  EZ_DisplayOneItem(widget, EZ_WidgetWindow(widget), x, y, item,
				    Rx, Ry, Rw, Rh);
		  XSync(EZ_Display, False);
		}
	    }
	}
    }
}

void EZ_WorkAreaDrawItemAt(wgt, item, x,y)
     EZ_Widget *wgt; EZ_Item *item; int x,y;
{
  if(wgt && item)
    {
      EZ_Widget *widget;
      int type = EZ_WidgetType(wgt);
      if(type == EZ_WIDGET_IWORK_AREA) widget = wgt;
      else  if(type == EZ_WIDGET_WORK_AREA) widget = EZ_WorkAreaIWorkArea(wgt);
      else widget = NULL;
      if(widget)
	{
	  EZ_Item **data;
	  int nitems, jw, jh;
	  int bw,ww,hh,Rx,Ry,Rw,Rh;
	  bw = EZ_WidgetBorderWidth(widget) + EZ_WidgetPadB(widget);
	  Rx = Ry = bw;
	  ww = EZ_WidgetWidth(widget);
	  Rw = ww - (bw << 1);
	  hh = EZ_WidgetHeight(widget);
	  Rh = hh -(bw << 1); 

      	  nitems = EZ_IWorkAreaNItems(widget);
	  data = EZ_IWorkAreaData(widget);
	  EZ_ComputeItemSize(item, &jw, &jh);
	  EZ_ItemOriginX(item) = x;
	  EZ_ItemOriginY(item) = y;
	  EZ_ItemAOriginX(item) = x -Rx;
	  EZ_ItemAOriginY(item) = y -Ry;
	  if(EZ_WidgetMapped(widget))
	    {
	      x = EZ_ItemOriginX(item);
	      y = EZ_ItemOriginY(item);
	      EZ_DisplayOneItem(widget, EZ_WidgetWindow(widget), x, y, item,
				Rx, Ry, Rw, Rh);
	      XSync(EZ_Display, False);
	    }
	}
    }
}
/***********************************************************************************************/
void EZ_WorkAreaScaleItem(wgt, item, mode, sx, sy, tx, ty, redisp)
     EZ_Widget *wgt;
     EZ_Item *item; 
     float sx, sy;
     int  redisp, tx, ty, mode;
{
  EZ_Widget *widget = NULL;
  if( EZ_WidgetType(wgt) == EZ_WIDGET_WORK_AREA)
    { widget = EZ_WorkAreaIWorkArea(wgt); }
  else if( EZ_WidgetType(wgt) == EZ_WIDGET_IWORK_AREA)
    widget = wgt;
    
  if(widget && item)
    {
      int x0,y0,aw,ah,jw,jh, itype = EZ_ItemType(item);

      EZ_IWorkAreaDirty(widget) = 1;
      x0 = EZ_ItemOriginX(item);
      y0 = EZ_ItemOriginY(item);
      aw = EZ_ItemAWidth(item);
      ah = EZ_ItemAHeight(item);
      
      if(itype == EZ_FIG_ITEM && (sx != 1.0 || sy != 1.0))
	{
	  EZ_ScaleFigItem(item, sx, sy, mode);
	  EZ_ComputeItemSize(item, &jw, &jh);
	}

      EZ_ItemOriginX(item) = x0 + tx;
      EZ_ItemOriginY(item) = y0 + ty;

      if(EZ_WidgetMapped(widget))
	{
	  int bw = EZ_WidgetBorderWidth(widget) + EZ_WidgetPadB(widget);
	  int Rx = bw;
	  int Ry = bw;
	  int Rw = EZ_WidgetWidth(widget) - (Rx+Rx);
	  int Rh = EZ_WidgetHeight(widget) -(Rx+Rx);

	  EZ_ItemAOriginX(item) = x0 + tx - Rx;
	  EZ_ItemAOriginY(item) = y0 + ty - Ry;
	  if(redisp > 0)    EZ_DrawWidget(widget);
	  else
	    {
	      int i, nitems = EZ_IWorkAreaNItems(widget);
	      EZ_Item **data = EZ_IWorkAreaData(widget);
	      Window win = EZ_WidgetWindow(widget);
	      EZ_FillItemBackground(widget, item, win, x0, y0, aw, ah, x0, y0, NULL);
	      EZ_IWorkAreaDirty(widget) = 0;

	      for(i = 0; i < nitems; i++)
		{
		  EZ_Item *sitem = data[i];
		  if(sitem != item)
		    {
		      int xx0 = EZ_ItemOriginX(sitem);
		      int yy0 = EZ_ItemOriginY(sitem);
		      int w0 = EZ_ItemAWidth(sitem);
		      int h0 = EZ_ItemAHeight(sitem);
		      if( xx0 >= x0 + aw || yy0 >= y0 + ah || xx0 + w0 <= x0  || yy0 + h0 < y0) ;
		      else { EZ_DisplayOneItem(widget, win, xx0, yy0, sitem, Rx, Ry, Rw,Rh); }
		    }
		}
	      if(redisp >= 0) EZ_DisplayOneItem(widget, win, x0+tx, y0+ty, item, Rx, Ry, Rw,Rh); 
	      XSync(EZ_Display, False);
	    }
	}
    }
}
/***********************************************************************************************/
#undef _EZ_WIDGET_IWORK_AREA_C_
