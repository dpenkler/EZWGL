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
 ***              Widget Entry                                 ***
 ***                                                           ***
 ***    This is undoubtly the uglest code in this lib !!!      ***
 ***                                                           ***
 *****************************************************************/
#define _EZ_WIDGET_INPUT_C_

#include "EZ_Widget.h"

/*********************************************************************
 * 
 *  Functions implemented in this file:
 */
EZ_Widget        *EZ_CreateEntry MY_ANSIARGS((EZ_Widget *parent, char *init_str));
void             EZ_DrawWidgetEntry MY_ANSIARGS((EZ_Widget *widget));
void             EZ_ComputeWidgetEntrySize MY_ANSIARGS((EZ_Widget *widget, int *w, int *h));
void             EZ_FreeWidgetEntryData MY_ANSIARGS((EZ_Widget *widget));
void             EZ_EntryEventHandle MY_ANSIARGS((EZ_Widget *widget, XEvent *event));

void             EZ_SetEntryString MY_ANSIARGS((EZ_Widget *widget, char *str));
char             *EZ_GetEntryString MY_ANSIARGS((EZ_Widget *widget));

void             EZ_OnOffSecretEntry MY_ANSIARGS((EZ_Widget *widget, int onoff));
int              EZ_EntryIsModified MY_ANSIARGS((EZ_Widget *widget));
/*********************************************************************
 * 
 * Editing functions
 */
void   EZ_EntryEditDeleteChar MY_ANSIARGS((EZ_Widget *widget, int dir));
void   EZ_EntryEditBeginOfInput MY_ANSIARGS((EZ_Widget *widget));
void   EZ_EntryEditBeginningOfInput MY_ANSIARGS((EZ_Widget *widget));
void   EZ_EntryEditEndOfInput MY_ANSIARGS((EZ_Widget *widget));
void   EZ_EntryEditForward MY_ANSIARGS((EZ_Widget *widget));
void   EZ_EntryEditBackward MY_ANSIARGS((EZ_Widget *widget));
void   EZ_EntryEditSetMarker MY_ANSIARGS((EZ_Widget *widget));
void   EZ_EntryEditKillRegion MY_ANSIARGS((EZ_Widget *widget));
void   EZ_EntryEditKillEndOfLine MY_ANSIARGS((EZ_Widget *widget, int flag));
void   EZ_EntryCopySelectedText MY_ANSIARGS((EZ_Widget *widget, int start, int end));
void   EZ_EntryPasteSelection MY_ANSIARGS((EZ_Widget *widget));
void   EZ_EntryEditInsertString  MY_ANSIARGS((EZ_Widget *widget, char *str));
void   EZ_EntryEditForwardWord MY_ANSIARGS((EZ_Widget *widget));
void   EZ_EntryEditBackwardWord MY_ANSIARGS((EZ_Widget *widget));
void   EZ_EntryEditGotoPosition MY_ANSIARGS((EZ_Widget *widget, int p));
/*********************************************************************
 * 
 * Global variables:
 */
extern ITextLine TextWidgetKillBuffer;
/*********************************************************************
 * 
 * Local functions.
 */
static void   EZ_SetEntryCursorPosition MY_ANSIARGS((EZ_Widget *widget, int x, int redisp));
static void   EZ_EntryEditKillRegionReal MY_ANSIARGS((EZ_Widget *widget, int start,int end, 
						      int flag, int mc));
static void   finish_entry MY_ANSIARGS((EZ_Widget *widget, int dcbk ));
static void   ExecuteMotionCallbacks MY_ANSIARGS((EZ_Widget *widget));

/*********************************************************************
 * 
 *  Local Variables.
 */
static EZ_WidgetHandle EZ_EntryHandle =
{
  EZ_ComputeWidgetEntrySize,
  EZ_DrawWidgetEntry,
  EZ_FreeWidgetEntryData,
  EZ_EntryEventHandle,
};

/*********************************************************************/

EZ_Widget  *EZ_CreateEntry(parent, init_str)
     EZ_Widget  *parent;     /* parent widget    */
     char       *init_str;
{
  EZ_Widget  *wptr;

  wptr = EZ_CreateNewWidget(parent);
  /*--------------------------------------------------
   * Register the handling functions for ...
   *  has to be done after  EZ_CreateNewWiget.
   *-------------------------------------------------*/
  EZ_WidgetHandlingFunctions[EZ_WIDGET_ENTRY] = &EZ_EntryHandle;

  EZ_SetWidgetTypeAndNames(wptr,EZ_WIDGET_ENTRY);
  EZ_SetWidgetHeightSetFlag(wptr);
  EZ_WidgetBorderStyle(wptr)  = EZ_BORDER_DOWN;
  EZ_WidgetPadX(wptr) = 0;
  EZ_WidgetPadY(wptr) = 3;
  EZ_WidgetPadB(wptr) = 2;
  EZ_WidgetStacking(wptr) = EZ_HORIZONTAL_RIGHT;

  EZ_WidgetCursor(wptr) = EZ_GetCursor(EZ_C_XTERM);

  EZ_EntryTWidth(wptr)     = 0;
  EZ_EntryTHeight(wptr)    = 0;

  EZ_EntryDataSize(wptr) = EZ_DEFAULT_LABEL_LINE_LENGTH;
  EZ_EntryData(wptr) = (char *)my_malloc( (EZ_EntryDataSize(wptr) +1) * sizeof(char), _ENTRY_I_D_);
  if(!EZ_EntryData(wptr)) EZ_OutOfMemory("EZ_CreateEntry");
  EZ_EntryData(wptr)[0] = '\0';

  EZ_EntryLineLength(wptr) = EZ_EntryDataSize(wptr);
  EZ_EntryFont(wptr) = EZ_GetFontFromId(EZ_INPUT_FONT);

  EZ_EntrySelectionBG(wptr) = ~0;
  EZ_EntrySelectionFG(wptr) = ~0;
  EZ_EntryTextBG(wptr) = ~0;

  EZ_EntryLastChar(wptr) = 0;
  EZ_EntryTWidth(wptr)  = 0;
  EZ_EntryTHeight(wptr) = 0;
  EZ_EntryFontAscent(wptr) = 0;
  EZ_EntryBgnIndex(wptr) = 0;
  EZ_EntryEndIndex(wptr) = 0;
  EZ_EntryPosition(wptr) = 0;
  EZ_EntrySelectionBgn(wptr) = -1;
  EZ_EntrySelectionEnd(wptr)  = -1;
  EZ_EntrySelectionFlag(wptr) = 0;
  EZ_EntryRemberHistory(wptr) = 0;
  EZ_EntrySecrecy(wptr) = 0;
  EZ_EntryEditable(wptr) = 1;
  EZ_EntryOWidth(wptr) = 0;
  EZ_EntryMarker(wptr) = -1;
  EZ_EntryOptEntry(wptr) = NULL;
  EZ_EntryFSE(wptr) = NULL;
  EZ_EntryWAdjust(wptr) = 0;

  EZ_SetWidgetFocusableFlag(wptr);  

  if(init_str != (char *)NULL)
    EZ_SetEntryString(wptr, init_str);
  
  return(wptr);
}


void  EZ_FreeWidgetEntryData(widget)
     EZ_Widget *widget;
{
  if(EZ_EntryData(widget)  != (char *)NULL)
    (void) my_free ((char *)EZ_EntryData(widget));
}
 
/********************************************************************
 *
 *  Figure out the dimension of a normal button
 */
void EZ_ComputeWidgetEntrySize(widget, w, h)
     EZ_Widget *widget;
     int             *w, *h;
{
  int   length, height, cw, ch;

  if(!(EZ_GetWidgetSizeComputedFlag(widget)))
    { 
      if(EZ_EntryLineLength(widget) <= 0 || EZ_EntryLineLength(widget) >= 256)
	EZ_EntryLineLength(widget) = EZ_DEFAULT_LABEL_LINE_LENGTH;
    
      EZ_EntryTHeight(widget) = EZ_EntryFont(widget)->max_bounds.ascent +
	EZ_EntryFont(widget)->max_bounds.descent;
      EZ_EntryFontAscent(widget) = EZ_EntryFont(widget)->max_bounds.ascent;

      EZ_SetWidgetSizeComputedFlag(widget);
      EZ_WidgetStacking(widget) = EZ_HORIZONTAL_RIGHT;
    }
  height = EZ_EntryTHeight(widget);
  length = ((EZ_EntryFont(widget)->max_bounds.width) * EZ_EntryLineLength(widget));
      
  cw = EZ_WidgetPadX(widget) + EZ_WidgetBorderWidth(widget) + EZ_WidgetPadB(widget);
  ch = EZ_WidgetPadY(widget) + EZ_WidgetBorderWidth(widget) + EZ_WidgetPadB(widget);
  cw = cw + cw + length;
  ch = ch + ch + height;

  *w = cw;
  *h = ch;
}

/****************************************************************************
 *
 *  Draw a entry widget
 */

void  EZ_DrawWidgetEntry(wptr)
     EZ_Widget *wptr;
{
  int             w, h,x,y,xx,yy, padb,padb2, tile, bw, bwpad, wadjust, hadjust;
  Pixmap          pixmap;
  Window          win;
  GC              gc;
  unsigned long   bgpv;

  win = EZ_WidgetWindow(wptr);
  w   = EZ_WidgetWidth(wptr);
  h   = EZ_WidgetHeight(wptr);
  padb = EZ_WidgetPadB(wptr);
  padb2 = padb + padb;
  bw = EZ_WidgetBorderWidth(wptr);
  bwpad = padb + bw;
  {
    EZ_Widget *child = EZ_WidgetChildren(wptr);
    if(child)
      { 
        wadjust =  w - EZ_WidgetOriginX(child);
        hadjust = ( (h - EZ_EntryTHeight(wptr)) >> 1 ) - bwpad;
        EZ_EntryWAdjust(wptr) = wadjust;
        EZ_EntryHAdjust(wptr) = hadjust;        
      }
    else 
      { 
        wadjust = EZ_EntryWAdjust(wptr) = 0;
        hadjust = EZ_WidgetYOffset(wptr);
        EZ_EntryHAdjust(wptr) = hadjust;
      }
  }
  /*-----------------------------------------------------------
   *  Create a pixmap, draw into this pixmap in background and
   *  copy to the button window when finished.
   *----------------------------------------------------------*/
  pixmap = XCreatePixmap(EZ_Display, win, w, h, EZ_DepthForWidgets);
  if(EZ_EntryTextBG(wptr)  != ~0)
    {
      bgpv = EZ_EntryTextBG(wptr);	
      XSetForeground(EZ_Display, EZ_WRITABLEGC, bgpv);
      gc = EZ_WRITABLEGC;
    }
  else
    {
      tile =  EZ_GetBackgroundGC(wptr, &gc, &bgpv, 1, 0);
      if(tile)
        {
          int ox, oy;
          EZ_GetWidgetTileOrigin(wptr, &ox, &oy);      
          XSetTSOrigin(EZ_Display, gc, ox, oy);
        }
    }
  XFillRectangle(EZ_Display, pixmap, gc, padb,padb, w-padb2, h-padb2); 
  
  /*-------------------
   *  Render the text
   *------------------*/
  if( EZ_GetWidgetDisabledFlag(wptr) ) 
    {
      EZ_GetBrightBDGC(wptr, &gc);
      XSetFont(EZ_Display, gc, EZ_EntryFont(wptr)->fid);
    }
  else
    {
      if(EZ_EntryForeground(wptr) != EZ_DefaultForeground || 
	 EZ_EntryFont(wptr) != EZ_GetFontFromId(EZ_INPUT_FONT))
	{
	  XSetFont(EZ_Display, EZ_WRITABLEGC, EZ_EntryFont(wptr)->fid);
	  XSetForeground(EZ_Display, EZ_WRITABLEGC, EZ_EntryForeground(wptr));	
	  gc = EZ_WRITABLEGC;
	}
      else gc = EZ_INPUT_TEXTGC;
    }

  xx =  padb; /* always start at the left edge */
  yy =  padb + hadjust;
  
  x = xx  + EZ_WidgetPadX(wptr) + EZ_WidgetBorderWidth(wptr);
  y = yy + EZ_WidgetBorderWidth(wptr) + EZ_WidgetPadY(wptr) + EZ_EntryFontAscent(wptr);
  
  if(EZ_EntryData(wptr) != (char *)NULL)
    {
      if(EZ_EntryOWidth(wptr) != w)
	{
	  /* bug fix: dimension may not be right when displayed at the
	   * first time. Have to know the geometry to calculate the
	   * actual dimension available for displaying entry string
	   * Thanks to <yumj@sun.ihep.ac.cn>, 5-26-97
	   */
          int  len, textwidth, length, start;
          char *str = EZ_EntryData(wptr);
          int  avail_space = EZ_WidgetWidth(wptr)- wadjust -
	    ((EZ_WidgetBorderWidth(wptr) + padb + EZ_WidgetPadX(wptr))<<1);
	  length = EZ_EntryLastChar(wptr);
          start = EZ_EntryBgnIndex(wptr);
	  if(start >= length)
	    {
	      EZ_EntryBgnIndex(wptr) = (length <  EZ_EntryLineLength(wptr)? 0 :
					length -  EZ_EntryLineLength(wptr));
	      start = EZ_EntryBgnIndex(wptr);
	    }
	  str += start;
	  len = 0;
	  length -= start;
	  if(length > 0)
	    {
	      len++;
	      while(1)
		{
		  textwidth = XTextWidth(EZ_EntryFont(wptr),str,len);
		  if(textwidth >= avail_space || len == length) break; 
		  else len++;
		} 
	    }
	  len += start;
          EZ_EntryEndIndex(wptr) = len;
	  if(EZ_EntryPosition(wptr) < start) EZ_EntryPosition(wptr) = start;
	  else if(EZ_EntryPosition(wptr) > len) EZ_EntryPosition(wptr) = len;
	  EZ_EntryOWidth(wptr) = w;
	}
      if(EZ_EntrySecrecy(wptr) == 0)
	{
	  int  len, width, start=0,end=0,x1=0,y1=0;
	  char *str, *selstr=NULL ;
	  
	  /*----------------------------------------------
	   * highlight the selected text if there are any
	   *---------------------------------------------*/
	  if(EZ_EntrySelectionFlag(wptr) == 1)
	    {
	      start = EZ_EntrySelectionBgn(wptr);
	      end = EZ_EntrySelectionEnd(wptr);
	      if(start > end) {x1 = start; start = end; end = x1;}
	      
	      str = EZ_EntryData(wptr) + EZ_EntryBgnIndex(wptr);
	      len = start - EZ_EntryBgnIndex(wptr);
	      x1 = x + XTextWidth(EZ_EntryFont(wptr),str,len);
	      selstr = str = EZ_EntryData(wptr) + start;
	      len = XTextWidth(EZ_EntryFont(wptr),str, end - start);
	      y1 = y -  EZ_EntryFontAscent(wptr);
	      {
		GC tmpgc = EZ_BD_DARKGC; 
		unsigned long bg = EZ_EntrySelectionBG(wptr);
		if(bg == ~0) EZ_GetBrightBDpv(wptr, &bg);
		XSetForeground(EZ_Display, tmpgc, bg);
		XFillRectangle(EZ_Display, pixmap, tmpgc, x1,y1, len, 
			       EZ_EntryTHeight(wptr)); 
	      }
	    }
	  str = EZ_EntryData(wptr) + EZ_EntryBgnIndex(wptr);
	  len =  EZ_EntryEndIndex(wptr) - EZ_EntryBgnIndex(wptr);
	  
	  XDrawString(EZ_Display, pixmap, gc, x,y, str, len);

	  if(EZ_EntrySelectionFlag(wptr) == 1)
            {
              unsigned long fg = EZ_EntrySelectionFG(wptr);
              if(fg != ~0)
                {
                  gc = EZ_WRITABLEGC; 
                  XSetFont(EZ_Display, gc, EZ_EntryFont(wptr)->fid);
                  XSetForeground(EZ_Display, gc, fg);
                  XDrawString(EZ_Display, pixmap, gc, x1,y, selstr, end-start);                  
                }
            }
	  /*
	   * insert a cursor 
	   */
	  if(EZ_EntryEditable(wptr) && (EZ_WidgetFlags(wptr) & EZ_WIDGET_HIGHLIGHT_BIT))
	    {
              int y2;
	      len = EZ_EntryPosition(wptr) - EZ_EntryBgnIndex(wptr);
	      width = XTextWidth(EZ_EntryFont(wptr), str, len);
	      x += width;
	      y = yy + EZ_WidgetBorderWidth(wptr) + EZ_WidgetPadY(wptr) + 1;
              y2= y+EZ_EntryTHeight(wptr)-1;
	      gc = EZ_DARKREDGC;
	      XDrawLine(EZ_Display, pixmap,gc, x,y, x, y2);
              y++; y2--;
              XDrawLine(EZ_Display, pixmap, gc,  x-2,y, x+2, y);
              XDrawLine(EZ_Display, pixmap, gc,  x-2,y2, x+2, y2);
	    }
	}
      else /* don't show the actual string, show a fake one */
	{
	  int len, width, i;
	  char *tmpstr;
	  len = EZ_EntryEndIndex(wptr) - EZ_EntryBgnIndex(wptr);
	  tmpstr = (char *)my_malloc((len+2) * sizeof(char), _ENTRY_I_D_);
	  if(!tmpstr) EZ_OutOfMemory("EZ_DrawWidgetEntry");
	  for(i = 0; i < len; i++) tmpstr[i]='*'; tmpstr[i]=0;
	  XDrawString(EZ_Display, pixmap, gc, x,y, tmpstr, len);
	  /* insert a cursor  */
	  if(EZ_GetWidgetHighlightFlag(wptr))
	    {
	      len = EZ_EntryPosition(wptr) - EZ_EntryBgnIndex(wptr);
	      width = XTextWidth(EZ_EntryFont(wptr), tmpstr, len);
	      x += width;
	      y = yy + EZ_WidgetBorderWidth(wptr) + EZ_WidgetPadY(wptr) + 1;
	      gc = EZ_DARKREDGC;
	      XDrawLine(EZ_Display, pixmap,gc, x,y, x, y+EZ_EntryTHeight(wptr)-1);
	    }
	  (void)my_free( tmpstr);
	}
    }
  EZ_FixFocusHighlightBorder(wptr, pixmap, w, h, padb);

  EZ_DrawRectBorder(wptr, pixmap);
  XCopyArea(EZ_Display,pixmap,win, EZ_WRITABLEGC,0,0,w,h,0,0); 
  XFreePixmap(EZ_Display, pixmap);                             
}

/*************************************************************************
 *
 *   Return the ptr to  widget->entry.data.
 */

char *EZ_GetEntryString(widget)
     EZ_Widget *widget;
{
  EZ_Widget *entry;
  if(widget)
    {
      switch( EZ_WidgetType(widget))
	{
	case EZ_WIDGET_ENTRY:
	  entry = widget;
	  break;
	case EZ_WIDGET_OPTIONAL_ENTRY:
	  entry = EZ_OptEntryEntry(widget);
	  break;
	default:
	  entry = NULL;
	  break;
	}
      if(entry) return(EZ_EntryData(entry));
    }
  return( (char *)NULL);
}
/*************************************************************************
 *
 *  Event handling
 */
#define ENTRY_BUFFER_SIZE    81

void  EZ_EntryEventHandle(widget, event)
     EZ_Widget *widget;
     XEvent          *event;
{
  XEvent            xevent;
  int               count, modifiers;
  KeySym            keysym;
  XComposeStatus    compose; 
  char              tmpbuffer[ENTRY_BUFFER_SIZE];
  int               buffersize = ENTRY_BUFFER_SIZE;

  if(widget == (EZ_Widget *)NULL) return;
  if(event->type == Expose) {EZ_DrawWidget(widget); return;}
  if(EZ_GetWidgetDisabledFlag(widget) ) return;

  switch(event->type)
    {
    case MappingNotify:
      XRefreshKeyboardMapping((XMappingEvent *)event);
      break;
    case SelectionNotify:
      {
	if(EZ_EntryEditable(widget))
	  {
	    XSelectionEvent *Sevent = (XSelectionEvent *)event;
	    Window window = Sevent->requestor;
	    Atom   property = Sevent->property;
	    
	    EZ_PasteSelection(widget, window, property, True,
			      EZ_EntryEditInsertString);
	    EZ_DrawWidget(widget);
	  }
      }
      break;
    case SelectionRequest:
      EZ_HandleSelectionRequest(widget, event);
      break;
    case SelectionClear:
      EZ_EntrySelectionFlag(widget) = 0;
      EZ_DrawWidget(widget);      
      break;
    case EnterNotify:
      if(event->xcrossing.focus == True)
	{
	  EZ_SetWidgetHighlightFlag(widget);
	  EZ_DrawWidget(widget);
	}
      break;
    case LeaveNotify:
      {
	Window fw; int rt;
	XGetInputFocus(EZ_Display, &fw, &rt);
	if(fw != EZ_WidgetWindow(widget))
	  {
	    EZ_ClearWidgetHighlightFlag(widget);
	    EZ_DrawWidget(widget);
	  }
      }
    break;
    case FocusIn:
      EZ_SetWidgetHighlightFlag(widget);
      EZ_DrawWidget(widget);
      break;
    case FocusOut:
      EZ_ClearWidgetHighlightFlag(widget);
      EZ_DrawWidget(widget);
      break;
    case ButtonPress:
      if(event->xbutton.button == EZ_Btn3) 
        {
          EZ_HandleDnDEvents(widget, event);
          break; 
        }
      if(EZ_GetWidgetFocusableFlag(widget))
	{
	  Window fw; int rt;
	  XGetInputFocus(EZ_Display, &fw, &rt);
	  if(fw != EZ_WidgetWindow(widget))
	    {
	      EZ_SetFocusTo(widget);
	      EZ_SetWidgetHighlightFlag(widget);
	      EZ_DrawWidget(widget);
	    }
          EZ_EntrySelectionFlag(widget) = 0;
          EZ_SetEntryCursorPosition(widget,event->xbutton.x, 0);
        }

      if(event->xbutton.button == EZ_Btn1)
	{
          if(EZ_GetWidgetFocusableFlag(widget) == 0)
            {
              EZ_Widget *optEntry = EZ_EntryOptEntry(widget);
              if(optEntry)
                {
                  EZ_Widget *btn = EZ_OptEntryButton(optEntry);   /* the btn */
                  EZ_Widget *popup = EZ_OptEntryPopup(optEntry);   /* the popup */
                  XEvent xkevent;
                  memcpy(&xkevent,event,sizeof(XEvent));
                  if(EZ_WidgetMapped(popup) == 0)
                    {
                      /* redirect this event to btn */
                      Window w1 = EZ_WidgetWindow(btn);
                      xkevent.xkey.window = w1; 
                      xkevent.xkey.time = CurrentTime;
                      XSendEvent(EZ_Display,w1,False,KeyPressMask, (XEvent *)&xkevent);
                    }
                }
            }
          else
            {
              XEvent xevent;
              int done = 0;
              int cursorstart = EZ_EntryPosition(widget);  

              while(!done)
                {
                  int etype;
                  XNextEvent(EZ_Display,&xevent);
                  etype = xevent.type;
                  if(EZ_FilterEvent(&xevent))
                    EZ_InvokePrivateEventHandler(&xevent);
                  if(etype == Expose || etype == SelectionClear || etype == FocusIn || etype == FocusOut) 
                    EZ_WidgetDispatchEvent(&xevent);
                  switch(xevent.type)
                    {
                    case ButtonRelease:
                      if(xevent.xbutton.button == EZ_Btn1)
                        {
                          done = 1;
                          if(EZ_EntrySelectionFlag(widget) )  /* selection bgn and end are set */
                            {
                              int end = EZ_EntrySelectionEnd(widget);
                              if(end != cursorstart)              /* something is selected */
                                {
                                  if(end < cursorstart)           /* make sure bgn < end   */
                                    {
                                      EZ_EntrySelectionEnd(widget) = cursorstart;
                                      EZ_EntrySelectionBgn(widget) = end;
                                    }
                                  /* copy the selected text into the Kill buffer */
                                  EZ_EntryCopySelectedText(widget,EZ_EntrySelectionBgn(widget),
                                                           EZ_EntrySelectionEnd(widget));
                                }
                            }
                          /* EZ_EntrySelectionFlag(widget) = 0;*/ /* stop highlighting */
                          EZ_DrawWidget(widget); 
                        }
                      break;
                    case MotionNotify:
                      EZ_SetEntryCursorPosition(widget,xevent.xmotion.x,1);  /* cursor follows the pointer */
                      if(EZ_EntryPosition(widget) != cursorstart)      /* Pointer has really moved    */
                        {
                          EZ_EntrySelectionBgn(widget) = cursorstart;  /* start position of selection      */
                          EZ_EntryMarker(widget) = cursorstart;        /* marker is also set at this point */
                          EZ_EntrySelectionEnd(widget) =               /* end position of selection        */
                            EZ_EntryPosition(widget);
                          EZ_EntrySelectionFlag(widget) = 1;           /* highlight the selected region    */
                          EZ_DrawWidget(widget);
                        }
                      break;
                    default:
                      break;
                    }
                }
            }
	}
      else if(event->xbutton.button == EZ_Btn2)
	{
	  /* wait for a button release on the same button */
	  while(1)
	    {
	      int etype;
	      XNextEvent(EZ_Display,&xevent);
	      etype = xevent.type;
	      if(EZ_FilterEvent(&xevent))
		EZ_InvokePrivateEventHandler(&xevent);
	      if(etype == FocusIn || etype == FocusOut || etype == Expose)
		EZ_WidgetDispatchEvent(&xevent);
	      if(etype == ButtonRelease && xevent.xbutton.button == EZ_Btn2)
		break;
	    }
	  /*----------------------------------------------
	   * get from the server the previous selection
	   * and past at the current point
	   *---------------------------------------------*/
	  if(EZ_EntryEditable(widget))
	    {
	      EZ_EntrySelectionFlag(widget) = 0;
	      EZ_EntryPasteSelection(widget);
	      EZ_DrawWidget(widget);
	    }
	}
      break;
    case KeyPress:
    {
      /*
       * Implement some simple Emacs editing commands.
       */
      int selected = EZ_EntrySelectionFlag(widget);
      EZ_SetWidgetHighlightFlag(widget);
      keysym = XLookupKeysym((XKeyEvent *)event, 0);
      /* tabing ..., tabing invoke callbacks ?? */
      if(keysym == XK_Tab 
#ifdef 	 XK_KP_Tab
	 || keysym == XK_KP_Tab
#endif
	 ) 
	{
	  finish_entry(widget, 0); 
	  EZ_FocusNextWidget(widget, (event->xkey.state & ShiftMask));
	  break;
	}
      modifiers = event->xkey.state & (ControlMask | Mod1Mask);
      event->xkey.state &= ~(ControlMask | Mod1Mask);
      count = XLookupString(&(event->xkey), tmpbuffer, buffersize, &keysym, &compose);
      if(count == 0 && (keysym == XK_Shift_L || keysym == XK_Shift_R)) break;
      EZ_EntrySelectionFlag(widget) = 0;
      tmpbuffer[count] = '\0';

      if(modifiers & ControlMask)
	{
	  int redraw = 1;
	  switch(keysym)
	    {
	    case XK_A: case XK_a:       EZ_EntryEditBeginOfInput(widget);       break;
	    case XK_B: case XK_b:       EZ_EntryEditBackward(widget);           break;
	    case XK_at: case XK_space:	EZ_EntryEditSetMarker(widget);          break;
	    case XK_D: case XK_d:
	      if(EZ_EntryEditable(widget))
		EZ_EntryEditDeleteChar(widget, 1);  
	      break;
	    case XK_E: case XK_e:	EZ_EntryEditEndOfInput(widget);         break;
	    case XK_F: case XK_f: 	EZ_EntryEditForward(widget);           break;
	    case XK_K: case XK_k:
	      if(EZ_EntryEditable(widget))
		EZ_EntryEditKillEndOfLine(widget,1);   
	      break;
	    case XK_W: case XK_w: 
	      if(EZ_EntryEditable(widget))
		EZ_EntryEditKillRegion(widget);	
	      break;
            case XK_Y: case XK_y: 
	      if(EZ_EntryEditable(widget))
		EZ_EntryPasteSelection(widget); 
	      break;
	    default:   redraw = 0;                                              break;
	    }
	  if(redraw)  EZ_DrawWidget(widget); 
	}
      else  if(modifiers & Mod1Mask)
	{
	  switch(keysym)
	    {
	    case XK_F: case XK_f:
	      EZ_EntryEditForwardWord(widget);
	      EZ_DrawWidget(widget); 
	      break;
	    case XK_B: case XK_b:
	      EZ_EntryEditBackwardWord(widget);
	      EZ_DrawWidget(widget); 
	      break;
	    default:
	      break;
	    }
	}
      else 
	{
          int done=0;
          EZ_Widget *btn=NULL, *popup, *optEntry = EZ_EntryOptEntry(widget);
          XEvent xkevent;
          if(optEntry)
            {
              EZ_Widget *listbox = EZ_OptEntryListbox(optEntry); /* the listbox */
              btn = EZ_OptEntryButton(optEntry);   /* the btn */
              popup = EZ_OptEntryPopup(optEntry);   /* the popup */
              if(EZ_WidgetMapped(popup))
                {
                  EZ_Widget *il = EZ_ListBoxIListBox(listbox);
                  Window w1 = EZ_WidgetWindow(il);
                  memcpy(&xkevent,event,sizeof(XEvent));
                  xkevent.xkey.window = w1;
                  xkevent.xkey.time = CurrentTime;
                  XSendEvent(EZ_Display,w1,False,KeyPressMask, (XEvent *)&xkevent);
                  done = 1;
                }
            }
	  switch(keysym)
	    {
	    case XK_Left: 
#ifdef XK_KP_Left
	    case XK_KP_Left:
#endif
              if(!done) EZ_EntryEditBackward(widget);
              break;
	    case XK_Right: 
#ifdef XK_KP_Right
	    case XK_KP_Right:
#endif
              if(!done)   EZ_EntryEditForward(widget);
              break;
	    case XK_BackSpace: 
	    case XK_Delete:
	      if(!done && EZ_EntryEditable(widget))
		{
		  if(selected) EZ_EntryEditKillRegion(widget);	
		  else EZ_EntryEditDeleteChar(widget,-1); 
		}
	      break;
	    case XK_Return: case XK_Linefeed:  
#ifdef XK_KP_Enter
	    case XK_KP_Enter:
#endif
	      finish_entry(widget, 1);
              EZ_FocusNextWidget(widget, (event->xkey.state & ShiftMask));
	      return;
	      break;
	    case XK_Down: case XK_Up: 
#ifdef XK_KP_Down
	    case XK_KP_Up:
	    case XK_KP_Down: 
#endif
	      /*      Used in optional Entry 
	       * If the optional listbox is not mapped, we pass this event to 
	       * this button to display it. Otherwise, we redirect this event
	       * to the listbox to move around.
	       */
              if(EZ_EntrySpin(widget))
                {
                  EZ_Widget *sbtn = EZ_EntrySpin(widget);
                  EZ_ScrollButtonScrollCount(sbtn) = 
                    (keysym==XK_Up 
#ifdef XK_KP_Down
                     || keysym==XK_KP_Up
#endif
                     ) ? 1 : -1 ;
                  EZ_ExecuteWidgetCallBacks(sbtn);
                }
              else if(optEntry && !done)
                {
                  /* redirect this event to btn */
                  Window w1 = EZ_WidgetWindow(btn);
                  memcpy(&xkevent,event,sizeof(XEvent));
                  xkevent.xkey.window = w1; 
                  xkevent.xkey.time = CurrentTime;
                  XSendEvent(EZ_Display,w1,False,KeyPressMask, (XEvent *)&xkevent);
                }
              
	      return;
	      break;
	    default:
	      if(!done && count > 0 && EZ_EntryEditable(widget))
		{
		  if(selected) EZ_EntryEditKillRegion(widget);	
		  EZ_EntryEditInsertString(widget,tmpbuffer);
		}
	      break;
	    }
	  EZ_DrawWidget(widget);      
	}
    }
    break;
    default:
      break;
    }
}

/***********************************************************************
 *
 *  Set buffer in Entry
 */
void EZ_SetEntryString(widget, str)
     EZ_Widget *widget;
     char      *str;
{
  EZ_Widget *entry = NULL;
  if(widget)
    {
      int type = EZ_WidgetType(widget);
      if(type == EZ_WIDGET_OPTIONAL_ENTRY) entry = EZ_OptEntryEntry(widget);
      else if(type == EZ_WIDGET_ENTRY) entry = widget;
    }

  if(entry)
    {
      EZ_EntrySelectionFlag(entry) = 0;
      if(EZ_WidgetWindow(entry) != (Window )NULL)
	{
	  if(EZ_EntryData(entry)[0] != '\0')
	    {
	      int end = EZ_EntryLastChar(entry);
	      if(end > 0) EZ_EntryEditKillRegionReal(entry,0,end,0,0);
	      EZ_EntryBgnIndex(entry) = 0;
	      EZ_EntryEndIndex(entry) = 0;
	      EZ_EntryPosition(entry) = 0;
	      /* EZ_EntryEditBeginOfInput(entry);
		 EZ_EntryEditKillEndOfLine(entry, 0);*/
  	    }
	  if(str != (char *)NULL) EZ_EntryEditInsertString(entry,str);
          EZ_EntryModified(entry)=0;
	  EZ_DrawWidget(entry);
	}
      else
	{
	  int length;
	  length = (str == (char *)NULL)? 0: strlen(str);
	  EZ_EntryLastChar(entry) = length;
	  EZ_EntryBgnIndex(entry) = (length < EZ_EntryLineLength(entry)? 0 :
				     length - EZ_EntryLineLength(entry));
	  EZ_EntryEndIndex(entry) = length;
	  EZ_EntryPosition(entry) = length;
	  EZ_EntryOWidth(entry) = 0;
	  if(str != (char *)NULL)
	    {
	      if(length >= EZ_EntryDataSize(entry))
		{
		  EZ_EntryData(entry) = 
		    my_realloc(EZ_EntryData(entry), 2*EZ_EntryDataSize(entry) + length, _ENTRY_I_D_);
		  if(!EZ_EntryData(entry)) EZ_OutOfMemory("EZ_SetEntryString");
		  EZ_EntryDataSize(entry)= 2*EZ_EntryDataSize(entry) + length;
		}
	      (void) strcpy(EZ_EntryData(entry), str);
	    }
	  else EZ_EntryData(entry)[0] = '\0';
	}
      if(EZ_WidgetMapped(entry))  EZ_DrawWidget(entry);  
    }
}

/***********************************************************************
 *
 *  Set the cursor position at pixel position x.
 */
static void EZ_SetEntryCursorPosition(widget, x ,redraw)
     EZ_Widget *widget;
     int             x;
     int             redraw;
{
  char   *data = EZ_EntryData(widget) + EZ_EntryBgnIndex(widget);
  int    len = EZ_EntryEndIndex(widget) - EZ_EntryBgnIndex(widget);
  int    done = 0;
  int    textlength = XTextWidth(EZ_EntryFont(widget),data,len);
  
  x -= (EZ_WidgetBorderWidth(widget)+EZ_WidgetPadX(widget) + EZ_WidgetPadB(widget)); 

  if(textlength <= 0) 
    {
      EZ_EntryPosition(widget) = EZ_EntryBgnIndex(widget);
      done = 2;
    }
  
  if(textlength <= x)
    {
      EZ_EntryPosition(widget) = EZ_EntryEndIndex(widget);    /* ? */
      if(EZ_EntryEndIndex(widget) < EZ_EntryLastChar(widget))
	EZ_EntryPosition(widget) += 1;
      done = 2;      
    }
  while(!done)
    {
      len--;
      if(len == 0) done = 1;
      else
	{
	  textlength = XTextWidth(EZ_EntryFont(widget),data,len);
	  if(textlength <= x) done = 1;
	}
    }
  if(done == 1)  /* cursor position has changed */
    EZ_EntryPosition(widget) = EZ_EntryBgnIndex(widget) + len;

  if(redraw) EZ_DrawWidget(widget);      
}		  

/*****************************************************************
 ***                                                           ***
 ***    Entry Buffer Editing Commands                          ***
 ***                                                           ***  
 ****************************************************************/

/* 
 * insert string "buffer" to Entry Data at the cursor position.
 */

void  EZ_EntryEditInsertString(widget,buffer)
     EZ_Widget *widget;
     char            *buffer;
{
  char  *data, tmp[1024];
  int   clength, length, cpos;
  
  if(buffer == NULL) return;
  data = EZ_EntryData(widget);
  clength = strlen(buffer);
  length = EZ_EntryLastChar(widget) + clength;
  cpos  = EZ_EntryPosition(widget);

  /*
   * check if we are running out of space, reallocate more if so
   */
  if(length >= EZ_EntryDataSize(widget))
    {
      EZ_EntryData(widget) = data = my_realloc(data, 2*EZ_EntryDataSize(widget) + clength, _ENTRY_I_D_);
      if(!EZ_EntryData(widget)) EZ_OutOfMemory("EZ_EntryEditInsertString");
      EZ_EntryDataSize(widget)= 2*EZ_EntryDataSize(widget) + clength;
    }

  tmp[0] = '\0';
  if(cpos !=  EZ_EntryLastChar(widget))  /* cursor is not at the end of string */
    (void) strcpy(tmp, data+cpos);
  (void) strcpy(data+cpos, buffer);
  if(tmp[0] != '\0')
    (void) strcat(data+cpos+clength,tmp);

  EZ_EntryPosition(widget) += clength; /* cursor is set at the end of the insertion */
  EZ_EntryLastChar(widget) = length;   /* total # of chars in data                  */
  EZ_EntryEndIndex(widget) += clength;  
  /*-----------------------------------------------
   * figure out the indices of the piece displayed
   *---------------------------------------------*/
  {
    char *str = EZ_EntryData(widget) + EZ_EntryBgnIndex(widget);
    int  avail_space = EZ_WidgetWidth(widget) - EZ_EntryWAdjust(widget)-
      ((EZ_WidgetBorderWidth(widget) + EZ_WidgetPadB(widget)+ EZ_WidgetPadX(widget))<<1);
    int len = EZ_EntryPosition(widget) - EZ_EntryBgnIndex(widget);
    int textlength = XTextWidth(EZ_EntryFont(widget),str,len);
    int done = 0;

    /*-----------------------------------------
     * First, check if the insertion extends
     * beyond the right boundary. fix the bgn
     * index if yes.
     *----------------------------------------*/
    if(textlength < avail_space) done = 2;
    while(!done) 
      {
	len--;
	str++;
	EZ_EntryBgnIndex(widget) += 1;
	if(len == 0) done = 1;
	textlength = XTextWidth(EZ_EntryFont(widget),str,len);
	if(textlength < avail_space) done = 1;
      }
    if(done == 1) 
      EZ_EntryEndIndex(widget) = EZ_EntryPosition(widget);
    /*---------------------------------------
     * fix the end indx if necessary. 
     *--------------------------------------*/
    done = 0;
    if(EZ_EntryPosition(widget) < EZ_EntryEndIndex(widget))
      {
	len = EZ_EntryEndIndex(widget) - EZ_EntryBgnIndex(widget);
	textlength = XTextWidth(EZ_EntryFont(widget),str,len);
	if(textlength < avail_space) done = 1;  
	
	while(!done)
	  {
	    EZ_EntryEndIndex(widget) -= 1;
	    len--;
	    if( XTextWidth(EZ_EntryFont(widget),str,len) < avail_space)
	      done = 1;
	  }
      }
  }
  ExecuteMotionCallbacks(widget); /* 7/18/97 */
  EZ_EntryModified(widget)=1;
}
  

/*
 * Delete a char near current position in Entry Data.
 */

void EZ_EntryEditDeleteChar(widget, direction)
     EZ_Widget *widget;
     int             direction;
{
  char  *data = EZ_EntryData(widget);
  int   cpos  = EZ_EntryPosition(widget);
  char  tmp[1024];  

  if(direction == 1) /* delete forward a character */
    {
      if(cpos == EZ_EntryLastChar(widget)) { EZ_RingBell(); return;}

      (void)strcpy(tmp, data+cpos+1);
      (void)strcpy(data+cpos,tmp);

      EZ_EntryLastChar(widget) -= 1;    
      EZ_EntryModified(widget)=1;
      /* 
       * fix end index.
       */
      if(EZ_EntryEndIndex(widget) > EZ_EntryLastChar(widget))
	EZ_EntryEndIndex(widget) = EZ_EntryLastChar(widget);

      if(EZ_EntryMarker(widget) > EZ_EntryPosition(widget))
	EZ_EntryMarker(widget) -= 1;
    }
  else if(direction == -1) /* delete backward a character */
    {
      if(cpos == 0) { EZ_RingBell(); return;}

      (void)strcpy(tmp, data+cpos);
      (void)strcpy(data+cpos-1,tmp);

      EZ_EntryLastChar(widget) -= 1;    
      EZ_EntryPosition(widget) = cpos-1;
      /* 
       * if the deletion is the first char on display,
       * shift bgn index  
       */
      if(EZ_EntryBgnIndex(widget) == cpos)
	EZ_EntryBgnIndex(widget) = cpos - 1;
      if( EZ_EntryEndIndex(widget) > EZ_EntryLastChar(widget))
	EZ_EntryEndIndex(widget) = EZ_EntryLastChar(widget);

      if(EZ_EntryMarker(widget) > EZ_EntryPosition(widget))
	EZ_EntryMarker(widget) -= 1;   

      {
	char *str = data + EZ_EntryBgnIndex(widget);
	int  len = EZ_EntryEndIndex(widget) - EZ_EntryBgnIndex(widget);
	int  avail_space = EZ_WidgetWidth(widget) - EZ_EntryWAdjust(widget)-
	  ((EZ_WidgetBorderWidth(widget) + EZ_WidgetPadB(widget) + EZ_WidgetPadX(widget))<<1);
	int textwidth = XTextWidth(EZ_EntryFont(widget),str,len);
	int done = 0;

      	
	if(textwidth > avail_space)
	  {
	    EZ_EntryEndIndex(widget) -= 1;
	    done = 2;
	  }
	while(!done)
	  {
	    if( EZ_EntryEndIndex(widget) < EZ_EntryLastChar(widget))
	      {
		EZ_EntryEndIndex(widget) += 1;
		len++;
		if(XTextWidth(EZ_EntryFont(widget),str,len) >= avail_space)
		  done = 1;
	      }
	    else done = 1;
	  }
      }
    }
  ExecuteMotionCallbacks(widget); /* 7/18/97 */
}

/*
 * move cursor to the begining of input
 */
void   EZ_EntryEditBeginningOfInput(widget)
     EZ_Widget *widget;
{
  EZ_EntryEditBeginOfInput(widget);
}
void   EZ_EntryEditBeginOfInput(widget)
     EZ_Widget *widget;
{
  int  len, length;

  EZ_EntryBgnIndex(widget) = 0;
  EZ_EntryPosition(widget) = 0;
  len = EZ_EntryLastChar(widget);
  length = EZ_EntryLineLength(widget);
  {
    char *data = EZ_EntryData(widget);
    int  avail_space = EZ_WidgetWidth(widget) - EZ_EntryWAdjust(widget)-
      ((EZ_WidgetBorderWidth(widget) + EZ_WidgetPadB(widget) + EZ_WidgetPadX(widget))<<1);
    int done = 0;

    length = 0;
    while(!done)
      {
        length++;
        if(length >= EZ_EntryLastChar(widget)) done = 1;
        else if(XTextWidth(EZ_EntryFont(widget),data,length) >= avail_space) done = 1;
      }
    EZ_EntryEndIndex(widget) = length;
  }
}
	

/*
 * move cursor to the end of input
 */
void   EZ_EntryEditEndOfInput(widget)
     EZ_Widget *widget;
{
  int  len, oldend, newend;

  oldend = EZ_EntryEndIndex(widget);
  newend = EZ_EntryLastChar(widget);
  EZ_EntryEndIndex(widget) = newend;
  EZ_EntryPosition(widget) = newend;

  if(oldend == newend) /* EOL is already visible */
    return;
  
  len = newend - (EZ_EntryLineLength(widget)>>1); /* safe */
  len = MAXV(0,len);
  {
    char *data = EZ_EntryData(widget)+len;
    int  avail_space = EZ_WidgetWidth(widget) - EZ_EntryWAdjust(widget)-
      ((EZ_WidgetBorderWidth(widget) + EZ_WidgetPadB(widget) + EZ_WidgetPadX(widget))<<1);
    int length = newend-len;
    while(1)
      {
        if(length < 2 || XTextWidth(EZ_EntryFont(widget),data, length) <= avail_space) break;
        length--; data++; len++;
      }  
    EZ_EntryBgnIndex(widget) = len;
  }
}
	
/*
 *  Forward the cursor.
 */
void   EZ_EntryEditForward(widget)
     EZ_Widget *widget;
{
  int end = EZ_EntryEndIndex(widget);
  int pos = EZ_EntryPosition(widget);
  int lastchar = EZ_EntryLastChar(widget);

  if(pos == lastchar) return;
  
  pos++; EZ_EntryPosition(widget) = pos;
  if(pos >= end)
    {
      int  len, textwidth;
      char *str;
      int  avail_space = EZ_WidgetWidth(widget) - EZ_EntryWAdjust(widget)-
	((EZ_WidgetBorderWidth(widget)+ EZ_WidgetPadB(widget) + EZ_WidgetPadX(widget))<<1);

      EZ_EntryEndIndex(widget)  = pos;

      len = EZ_EntryEndIndex(widget) - EZ_EntryBgnIndex(widget);
      str = EZ_EntryData(widget)+ EZ_EntryBgnIndex(widget);
      textwidth = XTextWidth(EZ_EntryFont(widget),str,len);
      while(textwidth >= avail_space) 
	{
	  EZ_EntryBgnIndex(widget) += 1;
	  str++;
	  len--;
	  textwidth = XTextWidth(EZ_EntryFont(widget),str,len);
	}
    }
}
/*
 * backward the cursor
 */
void   EZ_EntryEditBackward(widget)
     EZ_Widget *widget;
{
  int bgn = EZ_EntryBgnIndex(widget);
  int pos = EZ_EntryPosition(widget);

  if(pos == 0) return;
  
  pos--; EZ_EntryPosition(widget) = pos;
  
  if(pos < bgn)
    {
      int  len, textwidth;
      char *str;
      int  avail_space = EZ_WidgetWidth(widget) - EZ_EntryWAdjust(widget)-
	((EZ_WidgetBorderWidth(widget) + EZ_WidgetPadB(widget)+ EZ_WidgetPadX(widget))<<1);

      EZ_EntryBgnIndex(widget) = pos;
      len = EZ_EntryEndIndex(widget) - EZ_EntryBgnIndex(widget);
      str = EZ_EntryData(widget)+ EZ_EntryBgnIndex(widget);
      textwidth = XTextWidth(EZ_EntryFont(widget),str,len);
      if(textwidth >= avail_space)
	EZ_EntryEndIndex(widget) -= 1;
    }
}


void  EZ_EntryEditBackwardWord(widget)
     EZ_Widget *widget;
{
  int bgn = EZ_EntryBgnIndex(widget);
  int pos = EZ_EntryPosition(widget);
  char *data = EZ_EntryData(widget);

  while(1)
    {
      pos--;
      if(pos <= 0) {pos = 0; break;}
      if(isalnum(data[pos])) break;
    }
  if(pos)
    {
      while(1)
	{
	  if(pos == 0) break;
	  if(!(isalnum(data[pos]))) { pos++; break;}
	  else pos--;
	}
    }
  EZ_EntryPosition(widget) = pos;
  
  if(pos < bgn)
    {
      int  len, textwidth;
      char *str;
      int  avail_space = EZ_WidgetWidth(widget) - EZ_EntryWAdjust(widget)-
	((EZ_WidgetBorderWidth(widget) + EZ_WidgetPadB(widget) + EZ_WidgetPadX(widget))<<1);

      EZ_EntryBgnIndex(widget) = pos;
      len = EZ_EntryEndIndex(widget) - EZ_EntryBgnIndex(widget);
      str = EZ_EntryData(widget)+ EZ_EntryBgnIndex(widget);
      textwidth = XTextWidth(EZ_EntryFont(widget),str,len);
      if(textwidth >= avail_space)
	EZ_EntryEndIndex(widget) -= 1;
    }
}


void  EZ_EntryEditForwardWord(widget)
     EZ_Widget *widget;
{
  int end = EZ_EntryEndIndex(widget);
  int last = EZ_EntryLastChar(widget);
  int pos = EZ_EntryPosition(widget);
  char *data = EZ_EntryData(widget);
  
  while(1)
    {
      if(pos == last) break;
      if(isalnum(data[pos])) break;
      else  pos++;
    }
  if(pos < last)
    {
      while(1)
	{
	  if(pos == last) break;
	  if(!(isalnum(data[pos])))  break;
	  else pos++;
	}
    }
  EZ_EntryPosition(widget) = pos;
  if(pos >= end)
    {
      int  len, textwidth;
      char *str;
      int  avail_space = EZ_WidgetWidth(widget) - EZ_EntryWAdjust(widget)-
	((EZ_WidgetBorderWidth(widget) + EZ_WidgetPadB(widget)+ EZ_WidgetPadX(widget))<<1);

      EZ_EntryEndIndex(widget)  = pos;

      len = EZ_EntryEndIndex(widget) - EZ_EntryBgnIndex(widget);
      str = EZ_EntryData(widget)+ EZ_EntryBgnIndex(widget);
      textwidth = XTextWidth(EZ_EntryFont(widget),str,len);
      while(textwidth >= avail_space) 
	{
	  EZ_EntryBgnIndex(widget) += 1;
	  str++;
	  len--;
	  textwidth = XTextWidth(EZ_EntryFont(widget),str,len);
	}
    }
}

/*
 *  move cursor to location
 */
void EZ_EntryEditGotoPosition(widget, xx)
     EZ_Widget *widget; int xx;
{
  int bgn = EZ_EntryBgnIndex(widget);
  int end = EZ_EntryEndIndex(widget);
  int pos = EZ_EntryPosition(widget);
  int lastchar = EZ_EntryLastChar(widget);

  if(xx < 0) xx = 0;
  else if(xx > lastchar)  xx = lastchar;

  if(pos == xx) return;  
  EZ_EntryPosition(widget) = xx;
  if(pos >= end)
    {
      int  len, textwidth;
      char *str;
      int  avail_space = EZ_WidgetWidth(widget) - EZ_EntryWAdjust(widget)-
	((EZ_WidgetBorderWidth(widget)+ EZ_WidgetPadB(widget) + EZ_WidgetPadX(widget))<<1);

      EZ_EntryEndIndex(widget)  = pos;

      len = EZ_EntryEndIndex(widget) - EZ_EntryBgnIndex(widget);
      str = EZ_EntryData(widget)+ EZ_EntryBgnIndex(widget);
      textwidth = XTextWidth(EZ_EntryFont(widget),str,len);
      while(textwidth >= avail_space) 
	{
	  EZ_EntryBgnIndex(widget) += 1;
	  str++;
	  len--;
	  textwidth = XTextWidth(EZ_EntryFont(widget),str,len);
	}
    }
  else if(pos < bgn)
    {
      int  len, textwidth;
      char *str;
      int  avail_space = EZ_WidgetWidth(widget) - EZ_EntryWAdjust(widget)-
	((EZ_WidgetBorderWidth(widget) + EZ_WidgetPadB(widget) + EZ_WidgetPadX(widget))<<1);
      
      EZ_EntryBgnIndex(widget) = pos;
      len = EZ_EntryEndIndex(widget) - EZ_EntryBgnIndex(widget);
      str = EZ_EntryData(widget)+ EZ_EntryBgnIndex(widget);
      textwidth = XTextWidth(EZ_EntryFont(widget),str,len);
      if(textwidth >= avail_space)
	EZ_EntryEndIndex(widget) -= 1;
    }
}

/*
 * set the marker on entry
 */
void EZ_EntryEditSetMarker(widget)
     EZ_Widget *widget; 
{
  EZ_EntryMarker(widget) = EZ_EntryPosition(widget);
}

void  EZ_EntryEditKillRegion(widget)
     EZ_Widget *widget;      
{
  int marker = EZ_EntryMarker(widget);
  int cpos = EZ_EntryPosition(widget);
  int start, end;

  if(marker < 0 || marker == cpos) return;
  if(marker < cpos) { start = marker; end = cpos;}
  else {start = cpos; end = marker;}
  
  EZ_EntryEditKillRegionReal(widget,start,end,1,1);
}

/*
 * Kill to EOL
 */
void EZ_EntryEditKillEndOfLine(widget,cp)
     EZ_Widget *widget; 
     int cp;
{
  int start = EZ_EntryPosition(widget);
  int end = EZ_EntryLastChar(widget);
  if(end > start)    
    EZ_EntryEditKillRegionReal(widget,start,end,cp,1);
  else 
    { EZ_RingBell();}
}

/*
 * this function does the actual work of kill region.
 */
static void    EZ_EntryEditKillRegionReal(widget,start,end,cp,mck)
     EZ_Widget *widget; 
     int start, end;   /* start < end */
     int cp, mck;
{
  char  tmp[1024];
  int   length;

  length = end - start;

  if(cp)
    EZ_EntryCopySelectedText(widget,start,end);   /* copy the region being killed to the Kill buffer */
  
  if(end < EZ_EntryLastChar(widget))  /* something is on the tail */
    {
      (void)strcpy(tmp, EZ_EntryData(widget)+end); /* the tail */
      (void)strcpy(EZ_EntryData(widget)+start,tmp);
    }
  EZ_EntryLastChar(widget) -= length;  
  EZ_EntryData(widget)[EZ_EntryLastChar(widget)] = '\0'; /* must do this ! */

  EZ_EntryEndIndex(widget) -= (length>>1); /* safe */

  if(EZ_EntryEndIndex(widget) >= EZ_EntryLastChar(widget)) /* EOL is visible */
    EZ_EntryEndIndex(widget) = EZ_EntryLastChar(widget);
  else
    {  
      char *str = EZ_EntryData(widget) + EZ_EntryBgnIndex(widget);
      int  avail_space = EZ_WidgetWidth(widget) - EZ_EntryWAdjust(widget)-
	((EZ_WidgetBorderWidth(widget) + EZ_WidgetPadB(widget)+ EZ_WidgetPadX(widget))<<1);
      int len = EZ_EntryEndIndex(widget) - EZ_EntryBgnIndex(widget);
      int done = 0,textwidth;

      textwidth = XTextWidth(EZ_EntryFont(widget),str,len);
      
      if(textwidth >= avail_space) done = 1;
      while(!done) 
	{
	  len++;
	  EZ_EntryEndIndex(widget) += 1;
	  if(EZ_EntryEndIndex(widget) == EZ_EntryLastChar(widget)) done = 1;
	  else if(XTextWidth(EZ_EntryFont(widget),str,len) >= avail_space) done = 1;
	}
    }
  EZ_EntryModified(widget)=1;
  /*
   * reset the cursor and marker after kill
   */
  EZ_EntryPosition(widget) = start;
  EZ_EntryMarker(widget) =  start;
  if(mck) ExecuteMotionCallbacks(widget); /* 7/18/97 */
}

/******************************************************************
 *
 *  Copy the specified region of EntryData into the KillBuffer.
 */

void  EZ_EntryCopySelectedText(widget,start,end)
     EZ_Widget *widget;
     int             start,end;
{
  if(EZ_EntrySecrecy(widget) == 0)
    {
      int length = end - start;
      EZ_GrowITextKillBuffer(&TextWidgetKillBuffer,length+8);
      (void) strncpy(TextWidgetKillBuffer.data, EZ_EntryData(widget)+start, length);
      (TextWidgetKillBuffer.data)[length] = 0;
      TextWidgetKillBuffer.length = length;
      TextWidgetKillBuffer.width = 0;
      EZ_SetSelectionOwner(widget);
    }
}

/******************************************************************
 *
 *   Paset selection.
 */
void EZ_EntryPasteSelection(widget)
     EZ_Widget *widget;
{
  Window owner;
  EZ_Widget *tmp;

  owner = XGetSelectionOwner(EZ_Display, XA_PRIMARY);
  tmp = EZ_LookupWidgetFromMappedHT(owner);
  if(tmp != (EZ_Widget *)NULL)
    { 
      /*-------------------------------------------------------------
       * the owner is an EZ widget, don't have to go the server.
       *------------------------------------------------------------*/
      EZ_EntryEditInsertString(widget,TextWidgetKillBuffer.data);
    }
  else if(owner != None)
    {
      Atom property = XInternAtom(EZ_Display,"VT_SELECTION",False);
      /*------------------------------
       * have to go through the server
       *------------------------------*/
      XConvertSelection(EZ_Display,
			XA_PRIMARY,   /* the selection */
			XA_STRING,    /* the target    */
			property,     /* property      */
			EZ_WidgetWindow(widget),  /* the requestor */
			CurrentTime);
      /*------------------------------------------
       * Now return. The EventHandling procedure
       * will catch the SelectionNotify event and
       * act appropriately.
       *-----------------------------------------*/
    }
  else /* paste from CutBuffer */
    EZ_PasteSelection(widget, 
		      DefaultRootWindow(EZ_Display),
		      XA_CUT_BUFFER0,
		      False,
		      EZ_EntryEditInsertString);
  EZ_EntryModified(widget)=1;
}
/**************************************************************/

static void finish_entry(widget, docbk)
     EZ_Widget *widget; int docbk;
{
  EZ_Widget *optEntry = EZ_EntryOptEntry(widget);
  if(optEntry)     /* optional entry */
    {
      EZ_Widget *popup = (EZ_Widget *) EZ_OptEntryPopup(optEntry);
      if(popup && EZ_WidgetMapped(popup))
	{
	  XUngrabPointer(EZ_Display,  CurrentTime); 
	  EZ_HideWidget(popup);
	  {
	    Window fw = (Window)NULL; int rt;
	    XGetInputFocus(EZ_Display, &fw, &rt);
	    if(fw != EZ_WidgetWindow(widget))
	      {
		EZ_ClearWidgetHighlightFlag(widget);
		EZ_DrawWidget(widget);
	      }
	  }
	}
    }
  if(docbk)
    {
      EZ_ExecuteWidgetCallBacks(widget);
      if(optEntry) { EZ_ExecuteWidgetCallBacks(optEntry);}
    }

}
/**************************************************************/
void EZ_OnOffSecretEntry(widget, onoff)
     EZ_Widget *widget; int onoff;
{
  if(widget && EZ_WidgetType(widget) == EZ_WIDGET_ENTRY)
    {
      EZ_EntrySecrecy(widget) = (onoff == 0? 0 : 1);
    }
}

/****************************************************************/
static void ExecuteMotionCallbacks(widget) /* 7/18/97 */
     EZ_Widget *widget;
{
  if(EZ_WidgetWindow(widget) != (Window)NULL)
    {
      if(EZ_WidgetMotionCallBack(widget))
	{
	  EZ_CallBackStruct *callbacks = EZ_WidgetMotionCallBack(widget);
	  EZ_HandleMotionCallBacks(widget,callbacks);
	}
      /* if widget is a subwidget of an optEntry, check its motion
       * callback also
       */
      {
	EZ_Widget *optEntry = EZ_EntryOptEntry(widget);
	if(optEntry)
	  {
	    if(EZ_WidgetMotionCallBack(optEntry))
	      {
		EZ_CallBackStruct *callbacks = EZ_WidgetMotionCallBack(optEntry);
		EZ_HandleMotionCallBacks(optEntry,callbacks);
	      }
	  }
      }
    }
}
/****************************************************************/
int EZ_EntryIsModified(widget) EZ_Widget *widget;
{
  EZ_Widget *entry = NULL;
  if(widget)
    {
      int type = EZ_WidgetType(widget);
      if(type == EZ_WIDGET_OPTIONAL_ENTRY) entry = EZ_OptEntryEntry(widget);
      else if(type == EZ_WIDGET_ENTRY) entry = widget;
    }
  if(entry) return( (int) EZ_EntryModified(entry));
  return(0);
}
/****************************************************************/
#undef _EZ_WIDGET_INPUT_C_
