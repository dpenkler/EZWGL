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
 ***              Arrow Button                                 ***
 ***                                                           ***
 *****************************************************************/
#define _EZ_WIDGET_ARROW_BUTTON_C_
/*
 *  For internal use only.
 */
#include "EZ_Widget.h"

/*********************************************************************
 * 
 *  Functions implemented in this file:
 */
EZ_Widget         *EZ_CreateArrowButton MY_ANSIARGS((EZ_Widget *parent, int type));
void              EZ_DrawArrowButton MY_ANSIARGS((EZ_Widget *widget));
void              EZ_ArrowButtonEventHandle MY_ANSIARGS((EZ_Widget *widget, XEvent *event));
void              EZ_ComputeArrowButtonSize MY_ANSIARGS((EZ_Widget *widget, int *w, int *h));
void              EZ_InitArrowButtonHandle MY_ANSIARGS((void));
/*********************************************************************
 * 
 *  Local Variables.
 */
static EZ_WidgetHandle EZ_ArrowButtonHandle =
{ 
  EZ_ComputeArrowButtonSize,
  EZ_DrawArrowButton,
  EZ_FreeWidgetNButtonData,
  EZ_ArrowButtonEventHandle,
};

int EZ_resetArrowType MY_ANSIARGS((int type));
/*********************************************************************/
void EZ_InitArrowButtonHandle()
{
  EZ_WidgetHandlingFunctions[EZ_WIDGET_ARROW_BUTTON] = &EZ_ArrowButtonHandle;
}
/*********************************************************************/
EZ_Widget  *EZ_CreateArrowButton(parent, type)
     EZ_Widget  *parent;     /* parent widget  */
     int type;               /* type  */
{
  EZ_Widget  *tmp;

  tmp = EZ_CreateButton(parent,NULL, -1);
  /*--------------------------------------------------
   * Register the handling functions for Frame.
   *  has to be done after  EZ_CreateNewWiget.
   *-------------------------------------------------*/
  EZ_WidgetHandlingFunctions[EZ_WIDGET_ARROW_BUTTON] = &EZ_ArrowButtonHandle;
  EZ_SetWidgetTypeAndNames(tmp,EZ_WIDGET_ARROW_BUTTON);

  EZ_ButtonIsArrowButton(tmp) = 1;

  EZ_WidgetBorderWidth(tmp) = 2;
  EZ_WidgetBorderStyle(tmp)  = EZ_BORDER_UP;
  EZ_WidgetPadX(tmp) = 2;
  EZ_WidgetPadY(tmp) = 2;
  EZ_WidgetPadB(tmp) = 2;
  EZ_ArrowButtonType(tmp)  = type;
  EZ_ArrowButtonOptEntry(tmp) = NULL;

  EZ_SetWidgetFocusableFlag(tmp);  
  return(tmp);
}

/*******************************************************************
 *
 * Compute Frame size.
 */
void  EZ_ComputeArrowButtonSize(widget, w,h)
     EZ_Widget *widget;
     int             *w, *h;
{
  int cw, ch, bw, ww, hh;

  if(!(EZ_GetWidgetSizeComputedFlag(widget)))
    {
      EZ_SetWidgetSizeComputedFlag(widget);
    }
  ww = hh = 12;

  bw = EZ_WidgetBorderWidth(widget) + EZ_WidgetPadB(widget);
  cw = bw + EZ_WidgetPadX(widget);
  ch = bw + EZ_WidgetPadY(widget);
  *w =  cw + cw + ww;
  *h =  ch + ch + hh;
}
/*******************************************************************
 *
 *  Draw A Frame Widget. 
 */
void  EZ_DrawArrowButton(wptr)
     EZ_Widget *wptr;
{
  int             w, h,x,y, ww,hh, arwType, bw, padb, padb2;
  Pixmap          pixmap;
  Window          win;
  GC              gc;
  int             bwpad, offset = 0;
  unsigned long   bgpv;
  
  win = EZ_WidgetWindow(wptr);
  w   = EZ_WidgetWidth(wptr);
  h   = EZ_WidgetHeight(wptr);
  padb = EZ_WidgetPadB(wptr);  
  padb2 = padb + padb;
  /*-----------------------------------------------------------
   *  Create a pixmap, draw into this pixmap in background and
   *  copy to the button window when finished.
   *----------------------------------------------------------*/
  pixmap = XCreatePixmap(EZ_DisplayForWidgets, win, w, h, EZ_DepthForWidgets);    
  EZ_FixFocusHighlightBorder(wptr, pixmap, w, h, padb);

  if(EZ_ButtonNoHighlight(wptr))  { EZ_ClearWidgetHighlightFlag(wptr);}
  if(EZ_OffsetEnabled && EZ_WidgetBorderStyle(wptr) == EZ_BORDER_DOWN)   offset = 1;
  
  if(EZ_GetBackgroundGC(wptr, &gc,&bgpv,1,0))
    {
      int ox, oy;
      EZ_GetWidgetTileOrigin(wptr, &ox, &oy);      
      ox += offset; oy += offset;
      XSetTSOrigin(EZ_Display, gc, ox, oy);
    }
  XFillRectangle(EZ_Display, pixmap, gc, padb,padb, w-padb2, h - padb2); 

  /*--------------------------------------------------------
   *  Draw the label.
   *-------------------------------------------------------*/
  bw = EZ_WidgetBorderWidth(wptr);
  bwpad = bw + padb;
  x =  EZ_WidgetPadX(wptr) + bwpad;
  y =  EZ_WidgetPadY(wptr) + bwpad;
  
  ww = w - (x + x);
  hh = h - (y + y);

  x += offset;
  y += offset;
  arwType = EZ_ArrowButtonType(wptr);
  if((EZ_ButtonPressed(wptr) && (bw == 0 || EZ_WidgetBorderStyle(wptr)==0)))
    arwType = EZ_resetArrowType(arwType);
  if( EZ_GetWidgetDisabledFlag(wptr) )
    EZ_DrawSpecial(wptr,pixmap, EZ_BRIGHTGC1, arwType, x,y, ww, hh);
  else
    EZ_DrawSpecial(wptr,pixmap, NULL, arwType, x,y, ww, hh);
  
  EZ_DrawRectBorder(wptr, pixmap);
  XCopyArea(EZ_DisplayForWidgets,pixmap,win, EZ_WRITABLEGC,0,0,w,h,0,0);  
  XFreePixmap(EZ_DisplayForWidgets, pixmap);                               
}
/*******************************************************************
 *
 *  Event handle
 */
void EZ_ArrowButtonEventHandle(widget, event)
     EZ_Widget *widget;
     XEvent *event;
{
  if(widget == (EZ_Widget *)NULL) return;

  if(EZ_ArrowButtonOptEntry(widget))
    {
      EZ_Widget *popup;
      if(event->type == Expose) EZ_DrawWidget(widget);      
      if(event->type == ButtonPress && event->xbutton.button != EZ_Btn1)
        return;
      if( EZ_GetWidgetDisabledFlag(widget) )  return;

      switch(event->type)
        {
        case EnterNotify:
          EZ_SetWidgetHighlightFlag(widget);
          { EZ_InstallHighlightColor(widget); } /*4/29/99*/
          EZ_DrawWidget(widget);
          break;
        case LeaveNotify:
          EZ_ClearWidgetHighlightFlag(widget);
          { EZ_UninstallHighlightColor(widget); } /*4/29/99*/
          EZ_DrawWidget(widget);
          break;
        case KeyPress:
          {
            char              tmpbuffer[32];
            int               buffersize = 8;
            KeySym            keysym;
            XComposeStatus    compose; 
            int count = XLookupString(&(event->xkey), tmpbuffer, buffersize, &keysym, &compose);
            tmpbuffer[count] = '\0';
            if(keysym != XK_Down && keysym != XK_Up  
#ifdef XK_KP_Down
               && keysym != XK_KP_Down && keysym != XK_KP_Up
#endif
               )
              break;
          }
        case ButtonPress:
          {
            int          rx,ry,x,y;
            unsigned int mask;
            Window       root,win;
            Window          w1,w2,w3, w4;
            EZ_Widget       *tmp, *tmp1, *listbox, *entry;
            EZ_Widget *optEntry = (EZ_Widget *)EZ_ArrowButtonOptEntry(widget);
            EZ_Widget *parent = optEntry;
            EZ_Widget *tentry = EZ_OptEntryEntry(optEntry);

            popup = EZ_OptEntryPopup(optEntry);

            /*-----------------------------------------------------------
             * first display the  popup listbox.
             *----------------------------------------------------------*/
            XQueryPointer(EZ_Display, EZ_WidgetWindow(parent),
                          &root,                                    /* root return win */
                          &win,                                     /* child ret win   */
                          &rx, &ry,                                 /* x, y in root    */
                          &x,&y,                                    /* x, y in win     */
                          &mask
                          );
            rx += EZ_WidgetOriginX(tentry)  -x;
            ry += EZ_WidgetOriginY(tentry) + EZ_WidgetHeight(tentry) -y;
	
            EZ_ClearWidgetSizeComputedFlag(popup);
            EZ_WidgetOriginX(popup) = rx;
            EZ_WidgetOriginY(popup) = ry;
            EZ_SetWidgetXYSetFlag(popup);
            EZ_WidgetWidthHint(popup) = EZ_WidgetWidth(popup) = EZ_WidgetWidth(parent) +36;
            EZ_SetWidgetWidthSetFlag(popup);
            EZ_PropagateBgInfo2Widget(widget, popup);
            
            EZ_WidgetHeightHint(popup) = EZ_WidgetHeight(popup) = 200;
            EZ_SetWidgetHeightSetFlag(popup);
            EZ_DisplayWidget(popup);

            tmp = EZ_OptEntryListbox(optEntry);  /* the listbox */
            listbox = EZ_ListBoxIListBox(tmp); /* the ilistbox */
            EZ_IListBoxTakeMotion(listbox) = 1;
            w1 = EZ_WidgetWindow(listbox);
            tmp1 = EZ_ListBoxHScroll(tmp);
            w2 = EZ_WidgetWindow(tmp1);
            tmp1 = EZ_ListBoxVScroll(tmp);
            w3 = EZ_WidgetWindow(tmp1);	
            entry = EZ_OptEntryEntry(optEntry); /* the entry */ 
            w4 = EZ_WidgetWindow(entry);	

            /* and grab the keyboard */
            XGrabPointer(EZ_Display,
                         EZ_WidgetWindow(popup),
                         True,
                         ButtonPressMask|ButtonReleaseMask|ButtonMotionMask|PointerMotionMask,
                         GrabModeAsync,
                         GrabModeAsync, 
                         None,
                         EZ_GetCursor(EZ_C_RIGHT_PTR),
                         CurrentTime);
            
            /* grab the keyboard */
            EZ_SetFocusTo(entry);
            EZ_SetWidgetHighlightFlag(entry);
            EZ_DrawWidget(entry);	  
            EZ_DrawWidget(widget);	  

            /*
             * Now wait until a selection is made or ...
             */
            {
              XEvent          xevent;
              Window          event_window;
              int             done = 0;

              while(1)
                {
                  EZ_TFEvents();
                  XNextEvent(EZ_Display, &xevent);
                  EZ_FilterEvent(&xevent);
                  EZ_InvokePrivateEventHandler(&xevent);
                  event_window = xevent.xany.window;
                  switch(xevent.type)	      
                    {
                    case EnterNotify:
                    case LeaveNotify:
                    case MotionNotify:
                    case Expose:
                    case FocusIn:
                    case FocusOut:
                      tmp = EZ_LookupWidgetFromMappedHT(event_window); 
                      EZ_HandleWidgetWindowEvent(tmp, &xevent);
                      break; 
                    case ButtonPress:
                      if(event_window == EZ_WidgetWindow(widget))
                        {
                          XUngrabPointer(EZ_Display, CurrentTime); 
                          XUngrabKeyboard(EZ_Display,CurrentTime); 	
                          done = 1;
                        }
                      else if(event_window == w1 || event_window == w2 || 
                              event_window == w3 || event_window == w4)
                        {
                          tmp = EZ_LookupWidgetFromMappedHT(event_window); 
                          EZ_HandleWidgetWindowEvent(tmp, &xevent);
                        }
                      else done = 1;
                      break;
                    case KeyPress:
                      if(event_window == EZ_WidgetWindow(widget)) 
                        {
                          event_window = w4;
                          xevent.xkey.window = w4;
                        }
                    case ButtonRelease:
                      EZ_IListBoxTakeMotion(listbox) = 0;
                      if(event_window == w4 || event_window == w2 || event_window == w3 ||
                         event_window == w1)
                        {
                          tmp = EZ_LookupWidgetFromMappedHT(event_window); 
                          EZ_HandleWidgetWindowEvent(tmp, &xevent);
                        }
                      else if(event_window != EZ_WidgetWindow(widget)) done  = 1;
                    default:
                      break;
                    }
                  if(done || EZ_WidgetMapped(popup) == 0) 
                    { 
                      EZ_HideWidget(popup);
                      XUngrabPointer(EZ_Display, CurrentTime); 
                      EZ_ClearWidgetHighlightFlag(entry);
                      EZ_DrawWidget(entry);		  
                      break;
                    }
                }
              EZ_IListBoxTakeMotion(listbox) = 0;
            }
            break;
          default:
            break;
          }
        }
    }
  else
    {
       EZ_NButtonEventHandle(widget, event);
    }
}
/*******************************************************************/
int EZ_resetArrowType(arrow) int arrow;
{
  switch(arrow)
    {
    case EZ_UP_TRIANGLE_UP:   return(EZ_UP_TRIANGLE_DOWN); break;
    case EZ_UP_TRIANGLE_DOWN: return(EZ_UP_TRIANGLE_UP);   break;
    case EZ_DOWN_TRIANGLE_UP: return(EZ_DOWN_TRIANGLE_DOWN); break;
    case EZ_DOWN_TRIANGLE_DOWN: return(EZ_DOWN_TRIANGLE_UP); break;
    case EZ_LEFT_TRIANGLE_UP:   return(EZ_LEFT_TRIANGLE_DOWN); break;
    case EZ_LEFT_TRIANGLE_DOWN: return(EZ_LEFT_TRIANGLE_UP); break;
    case EZ_RIGHT_TRIANGLE_UP:  return(EZ_RIGHT_TRIANGLE_DOWN); break;
    case EZ_RIGHT_TRIANGLE_DOWN: return(EZ_RIGHT_TRIANGLE_UP); break;
    default: break;
    }
  return(arrow);
}
/*******************************************************************/
#undef _EZ_WIDGET_ARROW_BUTTON_C_
