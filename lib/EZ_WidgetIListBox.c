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
 ***              Widget Internal List Box                     ***
 ***                                                           ***
 *****************************************************************/
#define _EZ_WIDGET_ILIST_BOX_C_

#include "EZ_Widget.h"

/*********************************************************************
 * 
 *  Functions implemented in this file:
 */
EZ_Widget        *EZ_CreateIListBox MY_ANSIARGS((EZ_Widget *parent));
void             EZ_DrawWidgetIListBox MY_ANSIARGS((EZ_Widget *widget));
void             EZ_ComputeWidgetIListBoxSize MY_ANSIARGS((EZ_Widget *widget, int *w, int *h));
void             EZ_FreeWidgetIListBoxData MY_ANSIARGS((EZ_Widget *widget));
void             EZ_IListBoxEventHandle MY_ANSIARGS((EZ_Widget *widget, XEvent *event));

void             EZ_SetIListBoxData MY_ANSIARGS((EZ_Widget *widget, char **items, int nitems));
void             EZ_AppendIListBoxItem MY_ANSIARGS((EZ_Widget *widget, char *item));
void             EZ_InsertIListBoxItem MY_ANSIARGS((EZ_Widget *widget, char *item, int where));
void          EZ_InsertIListBoxColorItem MY_ANSIARGS((EZ_Widget *widget, char *item, 
                                                         unsigned long color, int where));
void          EZ_ColorIListBoxItemUsingIdx MY_ANSIARGS((EZ_Widget *widget, int idx ,
                                                        unsigned long clr));
void          EZ_ColorIListBoxItem MY_ANSIARGS((EZ_Widget *widget, char *item,
                                                unsigned long clr));

void             EZ_DeleteIListBoxItem MY_ANSIARGS((EZ_Widget *widget, int idx));
void             EZ_DeleteIListBoxString MY_ANSIARGS((EZ_Widget *widget, char *str));
void             EZ_ClearIListBox MY_ANSIARGS((EZ_Widget *widget));
char             *EZ_GetIListBoxSelectedItem MY_ANSIARGS((EZ_Widget *widget));
char             *EZ_GetIListBoxSelection MY_ANSIARGS((EZ_Widget *widget));
void             EZ_GetIListBoxData MY_ANSIARGS((EZ_Widget *widget, listboxentry **dat, int *cont));

void             EZ_ModifyIListBoxItem MY_ANSIARGS((EZ_Widget *widget, char *item, int idx));

void             EZ_IListBoxSelectItem MY_ANSIARGS((EZ_Widget *widget, char *str));
void             EZ_IListBoxSelectItemUsingIdx MY_ANSIARGS((EZ_Widget *widget, int idx));
/*********************************************************************
 * 
 *  Global data
 */
extern Time  EZ_LastButton1ClickTime;

/*********************************************************************
 * 
 * Local functions.
 */
static void   EZ_IListBoxSetSelection MY_ANSIARGS((EZ_Widget *widget));
static void   update_il_scrollbars MY_ANSIARGS((EZ_Widget *widget));

/*********************************************************************
 * 
 *  Local Variables.
 */
static EZ_WidgetHandle EZ_IListBoxHandle =
{
  EZ_ComputeWidgetIListBoxSize,
  EZ_DrawWidgetIListBox,
  EZ_FreeWidgetIListBoxData,
  EZ_IListBoxEventHandle,
};

/*********************************************************************/

EZ_Widget  *EZ_CreateIListBox(parent)
     EZ_Widget  *parent;     /* parent widget    */
{
  EZ_Widget  *wptr;
  int        i;

  wptr = EZ_CreateNewWidget(parent);
  /*--------------------------------------------------
   * Register the handling functions for ...
   *  has to be done after  EZ_CreateNewWiget.
   *-------------------------------------------------*/
  EZ_WidgetHandlingFunctions[EZ_WIDGET_ILIST_BOX] = &EZ_IListBoxHandle;

  EZ_SetWidgetTypeAndNames(wptr,EZ_WIDGET_ILIST_BOX);

  EZ_WidgetBorderStyle(wptr)  = EZ_BORDER_DOWN;
  EZ_WidgetBorderWidth(wptr)  = 2;
  EZ_WidgetPadB(wptr) = 2;
  EZ_WidgetPadX(wptr) = 1;
  EZ_WidgetPadY(wptr) = 1;

  EZ_IListBoxFont(wptr) = EZ_GetFontFromId(EZ_BOLD_FONT);
  EZ_IListBoxTHeight(wptr)    = 0;

  EZ_IListBoxNLines(wptr)  = 0;
  EZ_IListBoxSLine(wptr) = -1;
  EZ_IListBoxSLineEnd(wptr) = -1;
  EZ_IListBoxFirstLine(wptr)  = 0;
  EZ_IListBoxLastLine(wptr) = 0;
  EZ_IListBoxTextBG(wptr) = ~0;
  EZ_IListBoxSelectionBG(wptr) = ~0;
  EZ_IListBoxSelectionFG(wptr) = ~0;

  EZ_IListBoxListBox(wptr) = (EZ_Widget *)NULL;
  EZ_IListBoxHScroll(wptr) = (EZ_Widget *)NULL;
  EZ_IListBoxVScroll(wptr) = (EZ_Widget *)NULL;

  EZ_IListBoxMaxHSize(wptr) = 1;
  EZ_IListBoxHPageSize(wptr) = 1;
  EZ_IListBoxHStart(wptr) = 0;
  EZ_IListBoxTakeMotion(wptr) = 0;
  EZ_IListBoxCallBackOnRelease(wptr) = 0;

  EZ_IListBoxData(wptr) = ( listboxentry *)my_malloc(128 * sizeof( listboxentry), _ILIST_BOX_D_);
  if(!(EZ_IListBoxData(wptr))) EZ_OutOfMemory("EZ_CreateIListBox");
  EZ_IListBoxSpace(wptr) = 128;
  for(i = 0; i < EZ_IListBoxSpace(wptr); i++)
    EZ_IListBoxData(wptr)[i].data = (char *)NULL;  

  EZ_IListBoxPageSize(wptr) = 1;

  EZ_SetWidgetFocusableFlag(wptr);  

  EZ_IListBoxOptEntry(wptr)  = NULL;
  EZ_IListBoxFSL(wptr)  = NULL;
  
  return(wptr);
}


void  EZ_FreeWidgetIListBoxData(widget)
     EZ_Widget *widget;
{
  int          i;
  listboxentry *tmp = EZ_IListBoxData(widget);
  for(i = 0; i < EZ_IListBoxSpace(widget); i++)
    {
      if(tmp[i].data != (char *)NULL)
	(void)my_free((char *) tmp[i].data);
    }
  (void) my_free((char *)tmp);
}
 
/********************************************************************
 *
 *  Figure out the dimension of an internal list box.
 */
void EZ_ComputeWidgetIListBoxSize(widget, w, h)
     EZ_Widget *widget;
     int             *w, *h;
{
  int   height, cw, ch, padb;

  if(!(EZ_GetWidgetSizeComputedFlag(widget)))
    { 
      EZ_IListBoxTHeight(widget) = EZ_IListBoxFont(widget)->max_bounds.ascent +
	EZ_IListBoxFont(widget)->max_bounds.descent;
      EZ_IListBoxFontAscent(widget) = EZ_IListBoxFont(widget)->max_bounds.ascent;

      EZ_SetWidgetSizeComputedFlag(widget);
    }
  height = EZ_IListBoxTHeight(widget);
  cw = EZ_WidgetPadX(widget) + EZ_WidgetBorderWidth(widget);
  ch = EZ_WidgetPadY(widget) + EZ_WidgetBorderWidth(widget);
  cw = cw + cw + height * 2; /* default size to fit 2 lines, */
  ch = ch + ch + height * 2; /* each line can hold 2 chars   */

  padb = EZ_WidgetPadB(widget)<<1;
  *w = cw + padb;
  *h = ch + padb;
}

/****************************************************************************
 *
 *  Draw a IListBox widget
 */

void  EZ_DrawWidgetIListBox(wptr)
     EZ_Widget *wptr;
{
  int             w, h,x,y,xx,yy,hinc, hinc1,spacing, bw, padb,padb2;
  int             itmp, hsize = 1, disabled;
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
  spacing = EZ_IListBoxSpacing(wptr);
  {
    int tmp, tmp1, height;

    tmp1 = EZ_WidgetHeight(wptr) - EZ_WidgetPadY(wptr) - padb2 -
      ((EZ_WidgetBorderWidth(wptr))<<1);
    height = EZ_IListBoxTHeight(wptr)+spacing;
    tmp = (tmp1)/height;
    EZ_IListBoxPageSize(wptr) = tmp;    /* page size */
    if(EZ_IListBoxFirstLine(wptr)< 0) EZ_IListBoxFirstLine(wptr) = 0;
    tmp1 = EZ_IListBoxFirstLine(wptr) + tmp -1;
    if(tmp1 >= EZ_IListBoxNLines(wptr)) 
      {
	int tmp2 = EZ_IListBoxNLines(wptr) - tmp;
	EZ_IListBoxFirstLine(wptr) = tmp2 < 0? 0: tmp2;
      }
    if(tmp1 < EZ_IListBoxNLines(wptr)) 	EZ_IListBoxLastLine(wptr) = tmp1;
    else  EZ_IListBoxLastLine(wptr) = EZ_IListBoxNLines(wptr) - 1;

    EZ_IListBoxHPageSize(wptr) = EZ_WidgetWidth(wptr) - ((bw+EZ_WidgetPadX(wptr))<<1) - padb2;

    if(EZ_IListBoxHStart(wptr) > 0) EZ_IListBoxHStart(wptr) = 0;
    tmp1 =  EZ_IListBoxHPageSize(wptr) - EZ_IListBoxMaxHSize(wptr);
    if(tmp1 < 0 && EZ_IListBoxHStart(wptr) < tmp1) EZ_IListBoxHStart(wptr) = tmp1;
  }
  {
    int fl = EZ_IListBoxFirstLine(wptr);
    int lw = EZ_IListBoxTHeight(wptr)+spacing;
    int yy = fl * lw;
    EZ_IListBoxYStart(wptr) = -yy;
  }
  /*-----------------------------------------------------------
   *  Create a pixmap, draw into this pixmap in background and
   *  copy to the button window when finished.
   *----------------------------------------------------------*/
  pixmap = XCreatePixmap(EZ_Display, win, w, h, EZ_Depth);    
  {
    if(EZ_IListBoxTextBG(wptr) != ~0)
      {
	XSetForeground(EZ_Display, EZ_WRITABLEGC, EZ_IListBoxTextBG(wptr));	
	gc = EZ_WRITABLEGC;
      }
    else 
      {
	if(EZ_GetBackgroundGC(wptr, &gc, &bgpv, 0, 0))
          {
            int tox, toy;
            if(EZ_WidgetTOSet(wptr))
              EZ_GetWidgetTileOrigin(wptr, &tox, &toy);
            else
              {
                tox = (int)(EZ_IListBoxHStart(wptr));
                toy =    EZ_IListBoxYStart(wptr);
              }
            XSetTSOrigin(EZ_Display, gc, tox,toy);
          }
      }
  }
  XFillRectangle(EZ_Display, pixmap, gc, padb,padb, w -padb2, h-padb2);   
  /*-------------------
   *  Render the text
   *------------------*/
  disabled = EZ_GetWidgetDisabledFlag(wptr);
  if( disabled ) 
    {
      EZ_GetBrightBDGC(wptr, &gc);
      XSetFont(EZ_Display, gc, EZ_IListBoxFont(wptr)->fid);
    }
  else
    {
      XSetFont(EZ_Display, EZ_WRITABLEGC, EZ_IListBoxFont(wptr)->fid);
      XSetForeground(EZ_Display, EZ_WRITABLEGC, EZ_IListBoxForeground(wptr));
      gc = EZ_WRITABLEGC;
    }

  xx = EZ_IListBoxHStart(wptr);
  yy = 0;

  x = xx  + EZ_WidgetPadX(wptr) + bw + padb;
  y = yy + EZ_WidgetBorderWidth(wptr) + EZ_WidgetPadY(wptr) + padb +
    EZ_IListBoxFontAscent(wptr);
  hinc = EZ_IListBoxTHeight(wptr)+spacing;
  hinc1 = EZ_IListBoxFontAscent(wptr);

  if(EZ_IListBoxNLines(wptr) != 0)
    {
      char *str ;
      int  parity, rowbgset, len, i, theY = y;
      GC   rbgcs[2];
      unsigned long ccolor=~0, fgcolor;
      register  listboxentry *item = EZ_IListBoxData(wptr) + EZ_IListBoxFirstLine(wptr);
      rbgcs[0]=rbgcs[1]=None;

      if((rowbgset = EZ_IListBoxRowBGSet(wptr)))
        {
          GC darkgc, brightgc, normalgc;
	  unsigned long *bgs = EZ_IListBoxRowBG(wptr);
          int tile;
	  if(bgs[0])
	    {
              rbgcs[0] = EZ_BD_BRIGHTGC;
	      XSetForeground(EZ_Display, rbgcs[0], bgs[0]);      
	    }
	  else
	    {
	      tile = EZ_GetDarkBrightNormalBDGC(wptr, &darkgc, &brightgc, &normalgc);
              rbgcs[0] = brightgc;
              if(tile)
                {
                  int tox, toy;
                  if(EZ_WidgetTOSet(wptr))
                    EZ_GetWidgetTileOrigin(wptr, &tox, &toy);
                  else
                    {
                      tox = (int)(EZ_IListBoxHStart(wptr));
                      toy =    EZ_IListBoxYStart(wptr);
                    }
                  XSetTSOrigin(EZ_Display, brightgc, tox, toy);
                } 
            }
	  if(bgs[1])
	    {
              rbgcs[1] = EZ_BD_NORMALGC;
	      XSetForeground(EZ_Display, rbgcs[1], bgs[1]);      
	    }
	  else
	    {
	      tile = EZ_GetDarkBrightNormalBDGC(wptr, &darkgc, &brightgc, &normalgc);
              rbgcs[1] = normalgc;
              if(tile)
                {
                  int tox, toy;
                  if(EZ_WidgetTOSet(wptr))
                    EZ_GetWidgetTileOrigin(wptr, &tox, &toy);
                  else
                    {
                      tox = (int)(EZ_IListBoxHStart(wptr));
                      toy =    EZ_IListBoxYStart(wptr);
                    }
                  XSetTSOrigin(EZ_Display, normalgc, tox, toy);
                }
	    } 
        }

      for(i = EZ_IListBoxFirstLine(wptr), parity=i&1;
          i <= EZ_IListBoxLastLine(wptr); i++, item++, theY += hinc, parity = !parity)
	{
	  if(i >= EZ_IListBoxSLine(wptr) && i <= EZ_IListBoxSLineEnd(wptr))
	    {
	      /* do not use EZ_WRITABLEGC ! */ 
	      GC tmpgc = EZ_BD_DARKGC; 
	      unsigned long bg = EZ_IListBoxSelectionBG(wptr);
	      if(bg == ~0) EZ_GetBrightBDpv(wptr, &bg);
	      XSetForeground(EZ_Display, tmpgc, bg);
	      XFillRectangle(EZ_Display, pixmap, tmpgc, bw,theY-hinc1, w-(bw+bw), hinc-spacing); 
              if(disabled == 0)
                {
                  fgcolor = item->color;
                  if(EZ_IListBoxSelectionFG(wptr) != ~0) 
                    fgcolor=EZ_IListBoxSelectionFG(wptr);
                  if(ccolor != fgcolor)
                    {
                      XSetForeground(EZ_Display, gc,
                                     fgcolor==~0? EZ_IListBoxForeground(wptr): fgcolor);
                      ccolor = fgcolor;
                    }
                }
              str = item->data;
              len =  item->length;
              itmp = XTextWidth(EZ_IListBoxFont(wptr),str, len);
              if(itmp > hsize) hsize = itmp;
              XDrawString(EZ_Display, pixmap, gc, x,theY, str, len);
              if(EZ_IListBoxSelectionFG(wptr) != ~0)
                XSetForeground(EZ_Display, gc,
                               EZ_IListBoxForeground(wptr));	
	    }
          else
            {
              if(rowbgset) XFillRectangle(EZ_Display, pixmap, rbgcs[parity],
                                          bw,theY-hinc1, w-(bw+bw), hinc-spacing); 
              str = item->data;
              len =  item->length;
              itmp = XTextWidth(EZ_IListBoxFont(wptr),str, len);
              if(itmp > hsize) hsize = itmp;
              if(disabled == 0)
                {
                  fgcolor = item->color;
                  if(fgcolor != ccolor)
                    {
                      XSetForeground(EZ_Display, gc,
                                     fgcolor==~0? EZ_IListBoxForeground(wptr): fgcolor);
                      ccolor=fgcolor;
                    }
                }
              XDrawString(EZ_Display, pixmap, gc, x,theY, str, len);
            }
        }
    }
  EZ_FixFocusHighlightBorder(wptr, pixmap, w, h, padb);
  EZ_DrawRectBorder(wptr, pixmap);
  XCopyArea(EZ_Display,pixmap,win, EZ_WRITABLEGC,0,0,w,h,0,0); 
  XFreePixmap(EZ_Display, pixmap); 

  EZ_IListBoxMaxHSize(wptr) = hsize;
  update_il_scrollbars(wptr);
}
/*************************************************************************/
static void update_il_scrollbars(wptr)
     EZ_Widget *wptr;
{
  if(EZ_LookupWidgetFromAllHT(EZ_IListBoxHScroll(wptr)) != (EZ_Widget *)NULL)
    {
      EZ_UpdateScrollbarI(EZ_IListBoxHScroll(wptr),
			  EZ_IListBoxMaxHSize(wptr),
			  EZ_IListBoxHPageSize(wptr),
			  EZ_IListBoxHStart(wptr));
    }
  else
    {
      EZ_Widget *p = EZ_IListBoxListBox(wptr);
      EZ_IListBoxHScroll(wptr) = NULL;
      if(p) EZ_ListBoxHScroll(p) = NULL;
    }
  if(EZ_LookupWidgetFromAllHT(EZ_IListBoxVScroll(wptr)) != (EZ_Widget *)NULL)
    {
      EZ_UpdateScrollbarI(EZ_IListBoxVScroll(wptr),
			 EZ_IListBoxNLines(wptr),
			 EZ_IListBoxPageSize(wptr),
			 EZ_IListBoxFirstLine(wptr));
    }
  else
    {
      EZ_Widget *p = EZ_IListBoxListBox(wptr);
      EZ_IListBoxVScroll(wptr) = NULL;
      if(p) EZ_ListBoxVScroll(p) = NULL;
    }
}
/*************************************************************************
 *
 *  Event handling
 */
void  EZ_IListBoxEventHandle(widget, event)
     EZ_Widget *widget;
     XEvent          *event;
{
  XEvent    xevent;
  int       x, y, ysave, ww, hh, line, offset, xoffset, done, xyset, button;
  int       bw, padb, theight, spacing;

  if(widget == (EZ_Widget *)NULL) return;
  if( EZ_GetWidgetDisabledFlag(widget) )
    {
      if(event->type == Expose && event->xexpose.count == 0)
	EZ_DrawWidget(widget);      
      return;
    }
  bw = EZ_WidgetBorderWidth(widget);
  padb = EZ_WidgetPadB(widget);
  offset = EZ_WidgetPadY(widget) + bw + padb;
  xoffset = EZ_WidgetPadX(widget) + bw + padb;
  ww = EZ_WidgetWidth(widget);
  hh = EZ_WidgetHeight(widget);
  theight = EZ_IListBoxTHeight(widget);
  spacing = EZ_IListBoxSpacing(widget);

  switch(event->type)
    {
    case Expose:
      EZ_DrawWidget(widget);      
      break;
    case MappingNotify:
      XRefreshKeyboardMapping((XMappingEvent *)event);
      break;
    case SelectionRequest:
      EZ_HandleSelectionRequest(widget,event);
      break;
    case SelectionClear:
      {
	int line =  EZ_IListBoxSLine(widget);
	if(line != EZ_IListBoxSLineEnd(widget))
	  {
	    EZ_IListBoxSLineEnd(widget) = line;
	    EZ_DrawWidget(widget);
	  }
      }
    break;
    case ButtonPress:
      if(EZ_IListBoxNLines(widget) <= 0) break;
      {
	/* first, get focus if we don;t have it */
	{ 
	  Window fw; int rt;
	  EZ_Widget *tmp;
	  XGetInputFocus(EZ_Display, &fw, &rt);
	  tmp = EZ_LookupWidgetFromMappedHT(fw);
	  if(tmp != widget) EZ_SetFocusTo(widget);
	}
	button = event->xbutton.button;
	if(button == EZ_Btn1) 
	  {
	    Time  time_stamp = event->xbutton.time;   
	    int   elapsedtime = EZ_LastButton1ClickTime - time_stamp;

	    EZ_LastButton1ClickTime = time_stamp;
	    y = event->xbutton.y - offset;
	    line = y/(theight+spacing) + EZ_IListBoxFirstLine(widget);
	    if(line != EZ_IListBoxSLine(widget) &&
	       line <= EZ_IListBoxLastLine(widget))
	      {
		EZ_IListBoxSLine(widget) = line;
		EZ_IListBoxSLineEnd(widget) = line;
		/* EZ_IListBoxSetSelection(widget);*/
		EZ_DrawWidget(widget);
		EZ_HandleMotionCallBacks(widget,EZ_IListBoxMotionCallBack(widget));
	      }
	    /*
	     * a double click cause the execuation of the call back function
	     */
	    if(ABSV(elapsedtime) < DOUBLE_CLICK_TIME_INTERVAL)
	      { 
		/* EZ_Widget *twidget = EZ_IListBoxListBox(widget);*/
		EZ_ExecuteWidgetCallBacks(widget);
		/* if(twidget) {EZ_ExecuteWidgetCallBacks(twidget);}*/
		return;
	      }
	    /*
	     * wait for a matching ButtonRelease. In the mean time, process
	     * MotionNotify and SelectionRequest events.
	     */
	  waitForButtonRelease:
	    xyset = 0; 
	    x = y = 0; /* to please gcc */
	    done = 0;
	    xevent.type = 0;
	    while(!done)
	      {
		if(XPending(EZ_Display)) 
		  {
		    while(XPending(EZ_Display)) 
		      {
			if(xevent.type == ButtonRelease && xevent.xbutton.button == EZ_Btn1) break;
			else if(xevent.type == MotionNotify)
			  {
                            x = xevent.xmotion.x;
                            y = xevent.xmotion.y;
                            if(xevent.xmotion.window != EZ_WidgetWindow(widget))
                              {
                                Window junkwin;
                                int tx = x, ty = y;
                                XTranslateCoordinates(EZ_Display, xevent.xmotion.window,
                                                      EZ_WidgetWindow(widget),
                                                      tx, ty, &x,&y, &junkwin);        
                              }
			    x -=  xoffset;
			    y -=  offset;	
			    xyset = 1;
			  }
			else if(xevent.type == Expose || xevent.type == SelectionClear ||
                                xevent.type == FocusIn || xevent.type == FocusOut)
			  EZ_WidgetDispatchEvent(&xevent);
			XNextEvent(EZ_Display, &xevent); 
			if(EZ_FilterEvent(&xevent))
			  EZ_InvokePrivateEventHandler(&xevent);
		      }
		  }
		else  { EZ_SitForALittleBit(50000); }
		if(xevent.type == ButtonRelease && xevent.xbutton.button == EZ_Btn1) done = 1;
		
		if(!done && xyset)  /* do scroll if necessary */
		  {
		    if(y < 0) line = -1;
		    else line  = y/(theight+spacing) +  EZ_IListBoxFirstLine(widget);
		    if(line >= EZ_IListBoxFirstLine(widget) &&
		       line <= EZ_IListBoxLastLine(widget))
		      {
			if(line != EZ_IListBoxSLine(widget))
			  {
			    EZ_IListBoxSLine(widget) = line;
			    EZ_IListBoxSLineEnd(widget) = line;
			    /*EZ_IListBoxSetSelection(widget);*/
			    EZ_DrawWidget(widget);    
			    EZ_HandleMotionCallBacks(widget,EZ_IListBoxMotionCallBack(widget));  		  
			  }
		      }
		    else if(line > EZ_IListBoxLastLine(widget))
		      {
			if(EZ_IListBoxLastLine(widget) < EZ_IListBoxNLines(widget) - 1)
			  {
			    EZ_IListBoxFirstLine(widget) += 1;
			    EZ_IListBoxLastLine(widget) += 1;
			    EZ_IListBoxSLine(widget) = EZ_IListBoxLastLine(widget);
			    EZ_IListBoxSLineEnd(widget) = EZ_IListBoxLastLine(widget);
			    /*EZ_IListBoxSetSelection(widget); */
			    EZ_DrawWidget(widget);    
			    EZ_HandleMotionCallBacks(widget,EZ_IListBoxMotionCallBack(widget));  		  
			  }
		      }
		    else  if(line < EZ_IListBoxFirstLine(widget))
		      {
			if(EZ_IListBoxFirstLine(widget) > 0)
			  {
			    EZ_IListBoxFirstLine(widget) -= 1;
			    EZ_IListBoxLastLine(widget) -= 1;
			    EZ_IListBoxSLine(widget) =EZ_IListBoxFirstLine(widget);
			    EZ_IListBoxSLineEnd(widget) =EZ_IListBoxFirstLine(widget);
			    /* EZ_IListBoxSetSelection(widget);*/
			    EZ_DrawWidget(widget);    
			    EZ_HandleMotionCallBacks(widget,EZ_IListBoxMotionCallBack(widget)); 
			  }
		      }
		    /*
		     * scroll horizontally
		     */
		    if(x < 0 )
		      {
			if(EZ_IListBoxHStart(widget) < 0)
			  {
			    int iitmp = EZ_IListBoxHStart(widget) + 10;
			    if(iitmp > 0) iitmp = 0;
			    EZ_IListBoxHStart(widget) = iitmp;
			    EZ_DrawWidget(widget);  
			  }
		      }
		    else if( x > EZ_WidgetWidth(widget) - (xoffset<<1))
		      {
			int jjtmp = EZ_IListBoxHPageSize(widget) - EZ_IListBoxMaxHSize(widget);
			if(EZ_IListBoxHStart(widget) > jjtmp)
			  {
			    int iitmp = EZ_IListBoxHStart(widget) - 10;
			    if(iitmp < jjtmp) iitmp = jjtmp;
			    EZ_IListBoxHStart(widget) = iitmp;
			    EZ_DrawWidget(widget);  
			  }
		      }
		  }
	      }
	    /* for special listbox ... popup in optionalEntry  */
	    if(EZ_IListBoxCallBackOnRelease(widget))
	      {
		/* EZ_Widget *twidget = EZ_IListBoxListBox(widget); */
		EZ_ExecuteWidgetCallBacks(widget);
		/*if(twidget) {EZ_ExecuteWidgetCallBacks(twidget);} */
	      }
	    EZ_IListBoxSetSelection(widget);
	  }
	else if(button == EZ_Btn2)  /* set the selection only */
	  {
	    ysave = y = event->xbutton.y - offset;
	    line = y/(theight+spacing) + EZ_IListBoxFirstLine(widget);
	    if(line != EZ_IListBoxSLine(widget) &&
	       line <= EZ_IListBoxLastLine(widget))
	      {
		EZ_IListBoxSLine(widget) = line;
		EZ_IListBoxSLineEnd(widget) = line;
		/* EZ_IListBoxSetSelection(widget);*/
		EZ_DrawWidget(widget);
		EZ_HandleMotionCallBacks(widget,EZ_IListBoxMotionCallBack(widget));
	      }
	    /*
	     * wait for a matching ButtonRelease. In the mean time, process
	     * MotionNotify and SelectionRequest events.
	     */
	    xyset = 0;
	    done = 0;
	    xevent.type = 0;
	    x = y = 0; /* to make gcc happy */
	    while(!done)
	      {
		if(XPending(EZ_Display)) 
		  {
		    while(XPending(EZ_Display)) 
		      {
			if(xevent.type == ButtonRelease && xevent.xbutton.button == EZ_Btn2)
			  {
			    done = 1;
			    break;
			  }
			else if(xevent.type == MotionNotify)
			  {
			    x = xevent.xmotion.x - xoffset;
			    y = xevent.xmotion.y - offset;			
			    xyset = 1;
			  }
			else if(xevent.type == Expose || xevent.type == SelectionClear || xevent.type == FocusIn || xevent.type == FocusOut)
			  EZ_WidgetDispatchEvent(&xevent);
			XNextEvent(EZ_Display, &xevent); 
			if(EZ_FilterEvent(&xevent))
			  EZ_InvokePrivateEventHandler(&xevent);
		      }
		  }
		else  { EZ_SitForALittleBit(50000); }

		if(!done && xyset)  /* do scroll if necessary */
		  {
		    if(y < 0) line = -1;
		    else line  = y/(theight+spacing) +  EZ_IListBoxFirstLine(widget);
		    if(line >= EZ_IListBoxFirstLine(widget) &&
		       line <= EZ_IListBoxLastLine(widget))
		      {
			int bgn = EZ_IListBoxSLine(widget);
			int end = EZ_IListBoxSLineEnd(widget);
			if(line != bgn && line != end)
			  {
			    if(line < bgn) { EZ_IListBoxSLine(widget) = line; }
			    else if(line > end) { EZ_IListBoxSLineEnd(widget) = line;}
			    else  /* (line >= bgn && line <= end) */
			      {
				if(y < ysave) EZ_IListBoxSLineEnd(widget) = line;
				else EZ_IListBoxSLine(widget) = line;
			      }
			    ysave = y;
			    EZ_DrawWidget(widget);  
			    /* EZ_IListBoxSetSelection(widget); */
			  }
		      }
		    else if(line > EZ_IListBoxLastLine(widget))
		      {
			if(EZ_IListBoxLastLine(widget) < EZ_IListBoxNLines(widget) - 1)
			  {
			    EZ_IListBoxFirstLine(widget) += 1;
			    EZ_IListBoxLastLine(widget) += 1;
			    EZ_IListBoxSLineEnd(widget) = EZ_IListBoxLastLine(widget);
			    EZ_DrawWidget(widget);  
			    /* EZ_IListBoxSetSelection(widget);*/
			  }
		      }
		    else  if(line < EZ_IListBoxFirstLine(widget))
		      {
			if(EZ_IListBoxFirstLine(widget) > 0)
			  {
			    EZ_IListBoxFirstLine(widget) -= 1;
			    EZ_IListBoxLastLine(widget) -= 1;
			    EZ_IListBoxSLine(widget) =EZ_IListBoxFirstLine(widget);
			    EZ_DrawWidget(widget);  
			    /* EZ_IListBoxSetSelection(widget); */
			  }
		      }
		    /*
		     * scroll horizontally
		     */
		    if(x < 0 )
		      {
			if(EZ_IListBoxHStart(widget) < 0)
			  {
			    int iitmp = EZ_IListBoxHStart(widget) + 10;
			    if(iitmp > 0) iitmp = 0;
			    EZ_IListBoxHStart(widget) = iitmp;
			    EZ_DrawWidget(widget);  
			  }
		      }
		    else if( x > EZ_WidgetWidth(widget) - (xoffset<<1))
		      {
			int jjtmp = EZ_IListBoxHPageSize(widget) - EZ_IListBoxMaxHSize(widget);
			if(EZ_IListBoxHStart(widget) > jjtmp)
			  {
			    int iitmp = EZ_IListBoxHStart(widget) - 10;
			    if(iitmp < jjtmp) iitmp = jjtmp;
			    EZ_IListBoxHStart(widget) = iitmp;
			    EZ_DrawWidget(widget);  
			  }
		      }
		  }
	      }
	    EZ_IListBoxSetSelection(widget);	  
	  }
	else if(button == EZ_Btn3)
	  {
	    EZ_HandleDnDEvents(widget, event);
	  }
      }
      break;
    case MotionNotify:
      {
	unsigned int mask;
	if(EZ_Btn1 == Button1) mask = Button1Mask;
	else if(EZ_Btn1 == Button2) mask = Button2Mask;
	else mask = Button3Mask;
	if(EZ_IListBoxTakeMotion(widget) &&
	   EZ_IListBoxNLines(widget) > 0 && (event->xmotion.state & mask))
	  goto waitForButtonRelease;
      }
    break;
    case KeyPress:
      if(EZ_IListBoxNLines(widget) <= 0) break;
      if(EZ_IListBoxSLine(widget) == -1) 
	{
	  EZ_IListBoxSLine(widget) = EZ_IListBoxFirstLine(widget);
	}
      EZ_IListBoxSLineEnd(widget) = EZ_IListBoxSLine(widget);
      {
#define TEMP_BUFFER_SIZE    32
	int               count, move_unit = 1;
	KeySym            keysym;
	XComposeStatus    compose; 
	char              tmpbuffer[TEMP_BUFFER_SIZE];
	int               buffersize = TEMP_BUFFER_SIZE;
#undef TEMP_BUFFER_SIZE
	if(event->xkey.state & ShiftMask) move_unit = 2;
	if(event->xkey.state & ControlMask)     move_unit = 4;

	count = XLookupString(&(event->xkey), tmpbuffer, buffersize, &keysym, &compose);
	tmpbuffer[count] = '\0'; 
	
	switch(keysym)
	  {
	  case XK_Up: case XK_k:  case XK_p: case XK_K:  case XK_P: 
#ifdef XK_KP_Up
	  case XK_KP_Up:
#endif
	    line = EZ_IListBoxSLine(widget) - move_unit;
	    if(line < 0) line = 0;
	    if(EZ_IListBoxFirstLine(widget) > line) EZ_IListBoxFirstLine(widget) = line;
	    else if(EZ_IListBoxLastLine(widget) < line)
	      {
		int d = line - EZ_IListBoxLastLine(widget);
		EZ_IListBoxFirstLine(widget) += d;
	      }
	    EZ_IListBoxSLine(widget) = line;
	    EZ_IListBoxSLineEnd(widget) = line;
	    EZ_IListBoxSetSelection(widget);
	    EZ_DrawWidget(widget);
	    EZ_HandleMotionCallBacks(widget,EZ_IListBoxMotionCallBack(widget));  
	    break;
	  case XK_Down: case XK_n: case XK_j: case XK_N: case XK_J:  
#ifdef XK_KP_Down
	  case XK_KP_Down:
#endif
	    line = EZ_IListBoxSLine(widget) + move_unit;
	    if(line >= EZ_IListBoxNLines(widget)) line = EZ_IListBoxNLines(widget) -1;
	    if(EZ_IListBoxLastLine(widget) < line)
	      {
		int d = line - EZ_IListBoxLastLine(widget);
		EZ_IListBoxFirstLine(widget) += d;
	      }
	    else if(EZ_IListBoxFirstLine(widget) > line)
	      EZ_IListBoxFirstLine(widget) = line;
	    
	    EZ_IListBoxSLine(widget) = line;
	    EZ_IListBoxSLineEnd(widget) = line;
	    EZ_IListBoxSetSelection(widget);
	    EZ_DrawWidget(widget);
	    EZ_HandleMotionCallBacks(widget,EZ_IListBoxMotionCallBack(widget));  
	    break;
	  case XK_Left: case XK_b:  case XK_h: case XK_B:  case XK_H:  
#ifdef XK_KP_Left
	  case XK_KP_Left:
#endif
	    {
	      int aaa = (move_unit << 3);
	      int itmp = EZ_IListBoxHStart(widget) + aaa;
	      if(itmp >= 0) itmp = 0;
	      else if(itmp < EZ_IListBoxHPageSize(widget)
		      - EZ_IListBoxMaxHSize(widget))
		itmp =  EZ_IListBoxHPageSize(widget) - 
		  EZ_IListBoxMaxHSize(widget);

	      if(EZ_IListBoxHStart(widget) != itmp)
		{
		  EZ_IListBoxHStart(widget) = itmp;
		  if(EZ_WidgetMapped(widget)) EZ_DrawWidget(widget);
		}
	    }
	    break;
	  case XK_Right: case XK_f: case XK_l: case XK_F: case XK_L: 
#ifdef XK_KP_Right
	  case XK_KP_Right:
#endif
	    {
	      int aaa = -(move_unit << 3);
	      int itmp = EZ_IListBoxHStart(widget) + aaa;
	      if(itmp >= 0) itmp = 0;
	      else if(itmp < EZ_IListBoxHPageSize(widget)
		      - EZ_IListBoxMaxHSize(widget))
		itmp =  EZ_IListBoxHPageSize(widget) - 
		  EZ_IListBoxMaxHSize(widget);

	      if(EZ_IListBoxHStart(widget) != itmp)
		{
		  EZ_IListBoxHStart(widget) = itmp;
		  if(EZ_WidgetMapped(widget)) EZ_DrawWidget(widget);
		}
	    }	    
	    break;
	  case XK_Return: case XK_Linefeed: case XK_space:  
#ifdef XK_KP_Enter
	  case XK_KP_Enter:
#endif
	    { 
	      /* EZ_Widget *twidget = EZ_IListBoxListBox(widget); */
	      EZ_ExecuteWidgetCallBacks(widget);
	      /* if(twidget) {EZ_ExecuteWidgetCallBacks(twidget);} */
	    }
	    break;
	  default:
	    break;
	  }
	{
	  XEvent tmpevent; 
	  while(XCheckTypedEvent(EZ_Display, KeyPress, &tmpevent))
	    if(EZ_FilterEvent(&tmpevent))
	      EZ_InvokePrivateEventHandler(&tmpevent);
	}
      }      
      break;
    case EnterNotify:
      break;
    case LeaveNotify:
      break;
    default:
      break;
    }
}
/***********************************************************************************/

void  EZ_AppendIListBoxItem(widget, str)
     EZ_Widget *widget;
     char      *str; 
{
  EZ_InsertIListBoxItem(widget, str, -1);
}
/***********************************************************************************/
void  EZ_InsertIListBoxItem(widget, str, where)
     EZ_Widget *widget;
     char      *str; 
     int       where;
{
  EZ_InsertIListBoxColorItem(widget, str, ~0, where);
}

void  EZ_InsertIListBoxColorItem(widget, str, color, where)
     EZ_Widget *widget;
     char      *str; 
     int       where;
     unsigned long color;
{
  int space, nlines, i, length;
  listboxentry *item;

  if(widget == (EZ_Widget *)NULL || EZ_WidgetType(widget) != EZ_WIDGET_ILIST_BOX)
    return;  
  if(str == (char *)NULL) return;
  
  space = EZ_IListBoxSpace(widget);
  nlines = EZ_IListBoxNLines(widget);

  if(space <= nlines +1)
    {
      EZ_IListBoxData(widget) = (listboxentry *)
	my_realloc(EZ_IListBoxData(widget), (space + 128) * sizeof(listboxentry),  _ILIST_BOX_D_);      
      if(!EZ_IListBoxData(widget)) EZ_OutOfMemory("EZ_InsertIListBoxItem");
      for(i = space; i < space + 128; i++)
	EZ_IListBoxData(widget)[i].data = (char *)NULL;
      EZ_IListBoxSpace(widget) += 128;
    }
  /* insert at the end */
  if(where < 0 || where >= nlines)
    item = EZ_IListBoxData(widget) + nlines;
  else /* insert at the begining */ 
    { 
      listboxentry *table = EZ_IListBoxData(widget);
      for(i = nlines -1; i >= where; i--)
	{
	  table[i+1].data = table[i].data;
	  table[i+1].length = table[i].length;
	  table[i+1].color = table[i].color;
	}
      item = table + where;
      item->data = (char *)NULL;
      { /* 5-19-97, */
	int itmp = EZ_IListBoxSLine(widget);
	if(itmp >= 0)
	  {
	    if(where <= itmp)
	      {
		EZ_IListBoxSLine(widget) += 1;
		EZ_IListBoxSLineEnd(widget) += 1;
	      }
	  }
      }
    }
  
  /* allocate a new item */
  length = strlen(str);
  if(item->data != (char *)NULL)
    item->data = (char *)my_realloc(item->data, (length+1)*sizeof(char), _ILIST_BOX_D_);
  else
    item->data = (char *)my_malloc((length+1)*sizeof(char),  _ILIST_BOX_D_);
  if(!item->data) EZ_OutOfMemory("EZ_InsertIListBoxItem");
  (void)strcpy(item->data,str);
  item->length = length;
  item->color = color;
  
  EZ_IListBoxNLines(widget) += 1;

  if(EZ_WidgetMapped(widget))
    {
      EZ_DrawWidget(widget); 
      update_il_scrollbars(widget);
    }
}
/**************************************************************************/

void  EZ_SetIListBoxData(widget, items, nitems)
     EZ_Widget *widget;
     char      **items;
     int       nitems;
{
  int          i;

  if(widget == (EZ_Widget *)NULL || EZ_WidgetType(widget) != EZ_WIDGET_ILIST_BOX)
    return;
  if(nitems > 0 &&  items != (char **)NULL)
    {
      EZ_FreezeWidget(widget);
      EZ_ClearIListBox(widget);

      for(i = 0; i < nitems && items[i]; i++)
	EZ_InsertIListBoxItem(widget, items[i], -1);
      EZ_UnFreezeWidget(widget);
      if(EZ_WidgetMapped(widget)) EZ_DrawWidget(widget); 
    }

}
/******************************************************************************/
void  EZ_ClearIListBox(widget)
     EZ_Widget *widget;
{
  if(widget == (EZ_Widget *)NULL || EZ_WidgetType(widget) != EZ_WIDGET_ILIST_BOX)
    return;
  /*
   * reset listbox items to nothing.
   */
  EZ_IListBoxNLines(widget) = 0;
  EZ_IListBoxFirstLine(widget) = 0;
  EZ_IListBoxMaxHSize(widget) = 0;
  EZ_IListBoxHStart(widget) = 0;
  EZ_IListBoxSLine(widget) = -1;
  EZ_IListBoxSLineEnd(widget) = -1;
  if(EZ_WidgetMapped(widget)) EZ_DrawWidget(widget);  
}

/*************************************************************************/
char  *EZ_GetIListBoxSelectedItem(widget)
     EZ_Widget *widget;
{
  if(widget != (EZ_Widget *)NULL && EZ_WidgetType(widget) == EZ_WIDGET_ILIST_BOX)
    {
      int tmp = EZ_IListBoxSLine(widget);
      if(tmp >= 0) return(EZ_IListBoxData(widget)[tmp].data);
    }
  return( (char *)NULL);
}

void  EZ_DeleteIListBoxItem(widget, idx)
     EZ_Widget *widget; int idx;
{
  if(widget != (EZ_Widget *)NULL && EZ_WidgetType(widget) == EZ_WIDGET_ILIST_BOX)
    {
      if(idx >= 0 && idx < EZ_IListBoxNLines(widget))
	{
	  int i;
	  listboxentry *data =  EZ_IListBoxData(widget);
	  (void)my_free( (char *)data[idx].data);
	  for(i = idx; i < EZ_IListBoxNLines(widget) -1; i++)
	    {
	      data[i].data = data[i+1].data;
	      data[i].length = data[i+1].length;
	    }
	  data[i].data = NULL;
	  data[i].length = 0;
	  data[i].color = ~0;
	  EZ_IListBoxNLines(widget) -= 1;
	  if(idx == EZ_IListBoxSLine(widget) ||
	     idx == EZ_IListBoxSLineEnd(widget))
	    EZ_IListBoxSLineEnd(widget) = EZ_IListBoxSLine(widget) = -1;
	  else if(idx < EZ_IListBoxSLine(widget) && EZ_IListBoxSLine(widget) > 0)
	    {
	      EZ_IListBoxSLineEnd(widget) -= 1;
	      EZ_IListBoxSLine(widget) -= 1;
	    }
	  if(EZ_WidgetMapped(widget))
	    EZ_DrawWidget(widget);
	}
    }
}
void  EZ_DeleteIListBoxString(widget, str)
     EZ_Widget *widget; char *str;
{
  if(widget != (EZ_Widget *)NULL && EZ_WidgetType(widget) == EZ_WIDGET_ILIST_BOX && str)
    {
      int i, nlines = EZ_IListBoxNLines(widget);
      listboxentry *data =  EZ_IListBoxData(widget);
      for(i = 0; i < nlines; i++)
	{
	  if(!strcmp(data[i].data, str))
	    {
	      EZ_DeleteIListBoxItem(widget, i);
	      break;
	    }
	}
    }
}
/*************************************************************************/
void EZ_GetIListBoxData(widget, data_ret, count_ret)
     EZ_Widget *widget;
     listboxentry **data_ret;
     int *count_ret;
{
  if(widget && EZ_WidgetType(widget) == EZ_WIDGET_ILIST_BOX)
    {
      *data_ret = EZ_IListBoxData(widget);
      *count_ret = (int) EZ_IListBoxNLines(widget);
      return;
    }
  *data_ret = NULL;
  *count_ret = 0;
}
/*************************************************************************/
extern ITextLine TextWidgetKillBuffer;
/*************************************************************************/
char *EZ_GetIListBoxSelection(widget)
     EZ_Widget *widget;
{
  if(widget != (EZ_Widget *)NULL && EZ_WidgetType(widget) == EZ_WIDGET_ILIST_BOX)
    {
      int bgn = EZ_IListBoxSLine(widget);
      if(bgn >= 0)
	{
	  int end = EZ_IListBoxSLineEnd(widget);
	  int length, i;
	  if(end < bgn) end = bgn;
	  length = 0;
	  EZ_GrowITextKillBuffer(&TextWidgetKillBuffer,length+8);
	  (TextWidgetKillBuffer.data)[length] = 0;
	  for(i = bgn; i <= end; i++)
	    {
	      char *str = EZ_IListBoxData(widget)[i].data;
	      if(str)
		{
		  int len = strlen(str);
		  if(i < end)   length += len + 1;
		  else   length += len;
		  EZ_GrowITextKillBuffer(&TextWidgetKillBuffer,length+8);
		  (void) strncat(TextWidgetKillBuffer.data, str, len);
		  if(i < end) (void) strcat(TextWidgetKillBuffer.data, "\n");
		}
	    }
	  (TextWidgetKillBuffer.data)[length] = 0;
	  TextWidgetKillBuffer.length = length;
	  TextWidgetKillBuffer.width = 0;
	  return((TextWidgetKillBuffer.data));
	}
    }
  return(NULL);
}
/*************************************************************************/

static void  EZ_IListBoxSetSelection(widget)
     EZ_Widget *widget;
{
  int bgn = EZ_IListBoxSLine(widget);
  if(bgn >= 0)
    {
      int end = EZ_IListBoxSLineEnd(widget);
      int length, i;
      if(end < bgn) end = bgn;
      length = 0;
      EZ_GrowITextKillBuffer(&TextWidgetKillBuffer,length+8);
      (TextWidgetKillBuffer.data)[length] = 0;
      for(i = bgn; i <= end; i++)
	{
	  char *str = EZ_IListBoxData(widget)[i].data;
	  if(str)
	    {
	      int len = strlen(str);
	      if(i < end)   length += len + 1;
	      else   length += len;
	      EZ_GrowITextKillBuffer(&TextWidgetKillBuffer,length+8);
	      (void) strncat(TextWidgetKillBuffer.data, str, len);
	      if(i < end) (void) strcat(TextWidgetKillBuffer.data, "\n");
	    }
	}
      (TextWidgetKillBuffer.data)[length] = 0;
      TextWidgetKillBuffer.length = length;
      TextWidgetKillBuffer.width = 0;
      EZ_SetSelectionOwner(widget);
    }
}
/*************************************************************************/
/* contributed by  Antti H{yrynen <hydra@pato.vaala.fi> */
void  EZ_ModifyIListBoxItem(widget, str, where)
     EZ_Widget *widget;
     char      *str; 
     int       where;
{
  int space, nlines, length;
  listboxentry *item;

  if(widget == (EZ_Widget *)NULL || EZ_WidgetType(widget) != EZ_WIDGET_ILIST_BOX)
    return;  
  if(str == (char *)NULL) return;
  
  space = EZ_IListBoxSpace(widget);
  nlines = EZ_IListBoxNLines(widget);

  if (where > nlines-1) return ;
  item = EZ_IListBoxData(widget) + where;

  /* allocate a new item */
  length = strlen(str);
  if(item->data != (char *)NULL)
    item->data = (char *)my_realloc(item->data, (length+1)*sizeof(char), _ILIST_BOX_D_);
  else
    item->data = (char *)my_malloc((length+1)*sizeof(char),  _ILIST_BOX_D_);
  (void)strcpy(item->data,str);
  item->length = length;
  
  if(EZ_WidgetMapped(widget))
    {
      if(where <= EZ_IListBoxFirstLine(widget) + EZ_IListBoxPageSize(widget) &&
         where >= EZ_IListBoxFirstLine(widget))
	EZ_DrawWidget(widget); 
      update_il_scrollbars(widget);
    }
}
/**************************************************************************/
void  EZ_IListBoxSelectItem(widget, str)
     EZ_Widget *widget;
     char      *str; 
{
  int i, nlines;
  listboxentry *data, *item;

  if( widget == (EZ_Widget *)NULL || EZ_WidgetType(widget) != EZ_WIDGET_ILIST_BOX)
    return;  

  nlines = EZ_IListBoxNLines(widget);
  if(nlines <= 0) return;

  data = EZ_IListBoxData(widget);
  if(str == (char *)NULL) i = nlines + 2;
  else
    {
      for(i = 0; i < nlines; i++)
	{
	  item = data + i;
	  if(!strcmp(str, item->data)) break;
	}
    }
  if(i < nlines)
    {
      int osline =  EZ_IListBoxSLine(widget);
      if(i != osline)
	{
	  int fline = EZ_IListBoxFirstLine(widget);
	  int lline = EZ_IListBoxLastLine(widget);
	  
	  if(i > lline)
	    {
	      int  tmp = i - lline;
	      EZ_IListBoxFirstLine(widget) += tmp;
	      EZ_IListBoxLastLine(widget) += tmp;
	    }
	  else  if(i < fline) 
	    {
	      int tmp = fline - i;
	      EZ_IListBoxFirstLine(widget) -= tmp;
	      EZ_IListBoxLastLine(widget) -= tmp;
	    }
	  EZ_IListBoxSLine(widget) = i;
	  EZ_IListBoxSLineEnd(widget) = i;
	  /* EZ_IListBoxSetSelection(widget);*/
	  EZ_DrawWidget(widget);    
	  EZ_HandleMotionCallBacks(widget,EZ_IListBoxMotionCallBack(widget));   
	}
    }
  else if(i > nlines)
    {
      int osline =  EZ_IListBoxSLine(widget); 
      EZ_IListBoxSLine(widget) = -1;
      EZ_IListBoxSLineEnd(widget) = -1;
      if(osline != -1 && EZ_WidgetMapped(widget))   EZ_DrawWidget(widget);    
    }
}

void  EZ_IListBoxSelectItemUsingIdx(widget, idx)
     EZ_Widget *widget;
     int idx;
{
  int nlines;

  if( widget == (EZ_Widget *)NULL || EZ_WidgetType(widget) != EZ_WIDGET_ILIST_BOX)
    return;  

  nlines = EZ_IListBoxNLines(widget);
  
  if(idx < 0 || idx >= nlines) 
    {
      int osline =  EZ_IListBoxSLine(widget); 
      EZ_IListBoxSLine(widget) = -1;
      EZ_IListBoxSLineEnd(widget) = -1;
      if(osline != -1 && EZ_WidgetMapped(widget))   EZ_DrawWidget(widget);    
      return;
    }

  {
    int osline =  EZ_IListBoxSLine(widget);
    int i = idx;
    if(i != osline)
      {
	int fline = EZ_IListBoxFirstLine(widget);
	int lline = EZ_IListBoxLastLine(widget);
	  
	if(i > lline)
	  {
	    int  tmp = i - lline;
	    EZ_IListBoxFirstLine(widget) += tmp;
	    EZ_IListBoxLastLine(widget) += tmp;
	  }
	else  if(i < fline) 
	  {
	    int tmp = fline - i;
	    EZ_IListBoxFirstLine(widget) -= tmp;
	    EZ_IListBoxLastLine(widget) -= tmp;
	  }
	EZ_IListBoxSLine(widget) = i;
	EZ_IListBoxSLineEnd(widget) = i;
	if(EZ_WidgetMapped(widget))
	  {
	    /* EZ_IListBoxSetSelection(widget);*/
	    EZ_DrawWidget(widget);    
	    EZ_HandleMotionCallBacks(widget,EZ_IListBoxMotionCallBack(widget));
	  }
      }
  }
}
void  EZ_ColorIListBoxItemUsingIdx(widget, idx, clr)
     EZ_Widget *widget; int idx; unsigned long clr;
{
  if( widget && EZ_WidgetType(widget) == EZ_WIDGET_ILIST_BOX)
    {
      int nlines = EZ_IListBoxNLines(widget);
      if(idx >= 0 && idx < nlines) 
        {
          listboxentry *data, *item;
          data = EZ_IListBoxData(widget);
	  item = data + idx;
          if(item->color != clr)
            {
              item->color = clr;
              if(EZ_WidgetMapped(widget))   EZ_DrawWidget(widget);    
            }
        }
    }
}
void  EZ_ColorIListBoxItem(widget, str, clr)
     EZ_Widget *widget; char *str; unsigned long clr;
{
  if(widget && EZ_WidgetType(widget) == EZ_WIDGET_ILIST_BOX && str)
    {
      int i, nlines = EZ_IListBoxNLines(widget);
      listboxentry *data =  EZ_IListBoxData(widget);
      for(i = 0; i < nlines; i++)
	{
	  if(!strcmp(data[i].data, str))
	    {
              if(data[i].color != clr)
                {
                  data[i].color = clr;
                  if(EZ_WidgetMapped(widget))   EZ_DrawWidget(widget);    
                  break;
                }
            }
	}
    }
}

/*************************************************************************/
#undef _EZ_WIDGET_ILIST_BOX_C_
