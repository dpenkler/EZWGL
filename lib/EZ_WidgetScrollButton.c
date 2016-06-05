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
 ***              Scroll Button                                 ***
 ***                                                           ***
 *****************************************************************/
#define _EZ_WIDGET_SCROLL_BUTTON_C_
/*
 *  For internal use only.
 */
#include "EZ_Widget.h"

/*********************************************************************
 * 
 *  Functions implemented in this file:
 */

int      EZ_GetScrollButtonScrollCount MY_ANSIARGS((EZ_Widget *widget));

/*********************************************************************/
EZ_Widget         *EZ_CreateScrollButton MY_ANSIARGS((EZ_Widget *parent, int type));
void              EZ_DrawScrollButton MY_ANSIARGS((EZ_Widget *widget));
void              EZ_ScrollButtonEventHandle MY_ANSIARGS((EZ_Widget *widget, XEvent *event));
void              EZ_ComputeScrollButtonSize MY_ANSIARGS((EZ_Widget *widget, int *w, int *h));
/*********************************************************************
 * 
 *  Local Variables.
 */
static EZ_WidgetHandle EZ_ScrollButtonHandle =
{ 
  EZ_ComputeScrollButtonSize,
  EZ_DrawScrollButton,
  EZ_FreeUnknownData,
  EZ_ScrollButtonEventHandle,
};

/*********************************************************************/

EZ_Widget  *EZ_CreateScrollButton(parent, type)
     EZ_Widget  *parent;     /* parent widget  */
     int type;               /* type  */
{
  EZ_Widget *wptr = EZ_CreateNewWidget(parent);
  EZ_Widget *tmp = wptr;

  EZ_WidgetHandlingFunctions[EZ_WIDGET_SCROLL_BUTTON] = &EZ_ScrollButtonHandle;
  EZ_SetWidgetTypeAndNames(tmp,EZ_WIDGET_SCROLL_BUTTON);

  EZ_ButtonLabel(wptr) = EZ_AllocCopyString(NULL);
  EZ_ButtonLabelLength(wptr) = 1;
  EZ_ButtonPixmap(wptr) = (EZ_Bitmap *)NULL;
  EZ_ButtonFont(wptr) = EZ_GetFontFromId(EZ_BUTTON_FONT);
  EZ_WidgetBorderStyle(wptr)  = EZ_BORDER_UP;
  EZ_ButtonTWidth(wptr)     = 0;
  EZ_ButtonTHeight(wptr)    = 0;
  EZ_ButtonFontAscent(wptr) = 0;
  EZ_ButtonLabel2(wptr)     = (char *)NULL;
  EZ_ButtonLabel2Length(wptr) = 0;
  EZ_ButtonLabel2Width(wptr)  = 0;
  EZ_ButtonLineLength(wptr) = EZ_DEFAULT_LABEL_LINE_LENGTH;
  EZ_ButtonNLines(wptr)  = 1;
  EZ_ButtonPressed(wptr) = 0;
  EZ_ButtonNoHighlight(wptr) = 0;
  EZ_ButtonSetBorder(wptr)  = 0;
  EZ_SetWidgetFocusableFlag(wptr);
  /* special fields */
  EZ_ButtonCompanion(wptr) = NULL;
  EZ_ButtonFSB(wptr)       = NULL;

  EZ_WidgetSepX(wptr) = EZ_WIDGET_DEFAULT_PADX;
  EZ_WidgetSepY(wptr) = EZ_WIDGET_DEFAULT_PADY;
  EZ_ScrollButtonType(tmp) = type;
  EZ_WidgetBorderWidth(tmp) = 2;
  EZ_WidgetPadX(tmp) = 2;
  EZ_WidgetPadY(tmp) = 2;
  EZ_WidgetPadB(tmp) = 1;
  EZ_ScrollButtonBorderWidth(tmp) = 2;
  EZ_ScrollButtonBorderStyle(tmp)[0] = EZ_BORDER_UP;
  EZ_ScrollButtonBorderStyle(tmp)[1] = EZ_BORDER_UP;
  EZ_SetWidgetFocusableFlag(tmp);  
  EZ_ScrollButtonArrowType(tmp) = 0;
  EZ_ScrollButtonScrollCount(tmp) = 0;

  EZ_ButtonUnderLine(wptr) = -1;
  EZ_ButtonModifiers(wptr) = 0;
  EZ_ButtonShortcut(wptr)  = (char *)NULL;
  EZ_ButtonCopy(wptr) = NULL;
  return(wptr);
}

/*******************************************************************
 *
 * Compute Frame size.
 */
void  EZ_ComputeScrollButtonSize(widget, w,h)
     EZ_Widget *widget;
     int             *w, *h;
{
  int cw, ch, bw, ww, hh, type, sbw;

  if(!(EZ_GetWidgetSizeComputedFlag(widget)))
    {
      EZ_SetWidgetSizeComputedFlag(widget);
    }
  sbw = EZ_ScrollButtonBorderWidth(widget);  
  if(sbw == 0) sbw = EZ_ScrollButtonBorderWidth(widget) = 2;
  type = EZ_ScrollButtonType(widget);
  if(type == EZ_LEFT_RIGHT)
    {
      ww = 18 +(sbw+sbw); hh = 12;
      EZ_ScrollButtonArrowType(widget) = 0;
    }
  else if(type == EZ_UP_DOWN) 
    {
      hh = 18+(sbw+sbw); ww = 12;
      EZ_ScrollButtonArrowType(widget) = 0;
    }
  else
    {
      ww = hh = 12;
    }
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
void  EZ_DrawScrollButton(wptr)
     EZ_Widget *wptr;
{
  int             w, h,x,y, ww,hh, bw, sbw, padb, padb2, w2, h2;
  Pixmap          pixmap;
  Window          win;
  GC              gc;
  int             bwpad, type, highlight, itmp, xx, yy, xoff, yoff;
  int             arrowType, *box1, *box2;
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
  pixmap = XCreatePixmap(EZ_Display, win, w, h, EZ_Depth);    
  EZ_FixFocusHighlightBorder(wptr, pixmap, w, h, padb);

  arrowType = EZ_ScrollButtonArrowType(wptr);
  if(EZ_GetBackgroundGC(wptr, &gc,&bgpv,arrowType,0)) 
    {
      int ox, oy;
      EZ_GetWidgetTileOrigin(wptr, &ox, &oy);      
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

  box1 = EZ_ScrollButtonBox1(wptr);
  box2 = EZ_ScrollButtonBox2(wptr);
  if(arrowType)
    {
      int offset;
      if(EZ_OffsetEnabled && bw && EZ_WidgetBorderStyle(wptr) == EZ_BORDER_DOWN) offset = 1;
      else offset = 0;
      if(ww > hh) { itmp = hh; yoff = 0; xoff = (ww-hh)>>1;}
      else { itmp = ww; xoff = 0; yoff = (hh-ww)>>1;}
      xx = x + xoff; yy = y + yoff;
      xx += offset;  yy += offset;      
      if( EZ_GetWidgetDisabledFlag(wptr))
        EZ_DrawSpecial(wptr,pixmap, EZ_BRIGHTGC1, arrowType, xx,yy, itmp, itmp);
      else
        EZ_DrawSpecial(wptr,pixmap, None, arrowType, xx,yy, itmp, itmp);
      box1[0]=xx;  box1[1]=yy;  box1[2]= itmp;
    }
  else if( EZ_GetWidgetDisabledFlag(wptr) == 0)
    {
      type = EZ_ScrollButtonType(wptr);
      sbw = EZ_ScrollButtonBorderWidth(wptr);
      switch(type)
        {
        case EZ_LEFT:
        case EZ_RIGHT:
        case EZ_UP:
        case EZ_DOWN:
          if(ww > hh) { itmp = hh; yoff = 0; xoff = (ww-hh)>>1;}
          else { itmp = ww; xoff = 0; yoff = (hh-ww)>>1;}
          xx = x + xoff; yy = y + yoff;
          highlight = EZ_ScrollButtonHighlight(wptr)[0] && EZ_HighlightEnabled;
          if(highlight) { EZ_InstallHighlightColor(wptr); } /*4/29/99*/        
          EZ_DrawArrow4(type, wptr,pixmap, xx, yy, itmp,
                        EZ_ScrollButtonBorderStyle(wptr)[0],
                        sbw, highlight);
          if(highlight) { EZ_UninstallHighlightColor(wptr); } /*4/29/99*/        
          box1[0]=xx; box1[1]=yy; box1[2]=itmp;
          break;
        case EZ_LEFT_RIGHT:
          w2 = (ww >> 1);
          if(w2 > hh) { itmp = hh; yoff = 0; xoff = (w2-hh)>>1;}
          else { itmp = w2; xoff = 0; yoff = (hh-w2)>>1;}
          xx = x + xoff; yy = y + yoff;
          highlight = EZ_ScrollButtonHighlight(wptr)[0] && EZ_HighlightEnabled;
          if(highlight) { EZ_InstallHighlightColor(wptr); } /*4/29/99*/        
          EZ_DrawHLArrow(wptr,pixmap, xx, yy, itmp,
                         EZ_ScrollButtonBorderStyle(wptr)[0],
                         EZ_ScrollButtonBorderWidth(wptr), highlight);
          if(highlight) { EZ_UninstallHighlightColor(wptr); } /*4/29/99*/        
          box1[0]=xx; box1[1]=yy; box1[2]=itmp;
          xx += w2;
          highlight = EZ_ScrollButtonHighlight(wptr)[1] && EZ_HighlightEnabled;
          if(highlight) { EZ_InstallHighlightColor(wptr); } /*4/29/99*/        
          EZ_DrawHRArrow(wptr,pixmap, xx, yy, itmp,
                         EZ_ScrollButtonBorderStyle(wptr)[1],
                         EZ_ScrollButtonBorderWidth(wptr), highlight);
          if(highlight) { EZ_UninstallHighlightColor(wptr); } /*4/29/99*/        
          box2[0]=xx; box2[1]=yy; box2[2]=itmp;
          break;
        case EZ_UP_DOWN:
          h2 = (hh >> 1);
          if(ww > h2) { itmp = h2; yoff = 0; xoff = (ww-h2)>>1;}
          else { itmp = ww; xoff = 0; yoff = (h2-ww)>>1;}
          xx = x + xoff; yy = y + yoff;
          highlight = EZ_ScrollButtonHighlight(wptr)[1] && EZ_HighlightEnabled;
          if(highlight) { EZ_InstallHighlightColor(wptr); } /*4/29/99*/        
          EZ_DrawVUArrow(wptr,pixmap, xx, yy, itmp,
                         EZ_ScrollButtonBorderStyle(wptr)[1],
                         sbw, highlight);
          box2[0]=xx; box2[1]=yy; box2[2]=itmp;
          yy += h2;
          if(highlight) { EZ_UninstallHighlightColor(wptr); } /*4/29/99*/        
          highlight = EZ_ScrollButtonHighlight(wptr)[0] && EZ_HighlightEnabled;
          if(highlight) { EZ_InstallHighlightColor(wptr); } /*4/29/99*/        
          EZ_DrawVDArrow(wptr,pixmap, xx, yy, itmp,
                         EZ_ScrollButtonBorderStyle(wptr)[0],
                         sbw, highlight);
          if(highlight) { EZ_UninstallHighlightColor(wptr); } /*4/29/99*/        
          box1[0]=xx; box1[1]=yy; box1[2]=itmp;
          break;
        default:
          break;
        }
    }
  EZ_DrawRectBorder(wptr, pixmap);
  XCopyArea(EZ_Display,pixmap,win, EZ_WRITABLEGC,0,0,w,h,0,0);  
  XFreePixmap(EZ_Display, pixmap);                               
}
/*******************************************************************/
static int getDirection(type,which) int type, which;
{
  switch(type) 
    {
    case EZ_LEFT:  return(-1); break;
    case EZ_RIGHT: return(1);  break;
    case EZ_UP:    return(1);  break;
    case EZ_DOWN:  return(-1); break;
    case EZ_LEFT_RIGHT: case EZ_UP_DOWN:
      return(which); break;
    default: break;
    }
  return(0);
}

/*******************************************************************
 *
 *  Event handle
 */
void EZ_ScrollButtonEventHandle(widget, event)
     EZ_Widget *widget;
     XEvent *event;
{
  if(widget == (EZ_Widget *)NULL) return;
  if( EZ_GetWidgetDisabledFlag(widget) )   return;
  {
    int type = EZ_ScrollButtonType(widget);
    int *box1 = EZ_ScrollButtonBox1(widget);
    int *box2 = EZ_ScrollButtonBox2(widget);
    switch(event->type)
      {
      case ButtonPress:
        if(event->xbutton.button == EZ_Btn1)
          {
            int x = event->xbutton.x;
            int y = event->xbutton.y;
            int doit=0;
            if(x>=box1[0] && x<=box1[0]+box1[2] &&
               y>=box1[1] && y<=box1[1]+box1[2])
              {
                EZ_ScrollButtonScrollCount(widget)=getDirection(type,-1);
                EZ_ScrollButtonHighlight(widget)[0] = 1;
                EZ_ScrollButtonBorderStyle(widget)[0] = EZ_BORDER_DOWN;
                doit =1;
              }
            else if(x>=box2[0] && x<=box2[0]+box2[2] &&
                    y>=box2[1] && y<=box2[1]+box2[2])
              {
                EZ_ScrollButtonScrollCount(widget)=getDirection(type,1);
                EZ_ScrollButtonHighlight(widget)[1] = 1;
                EZ_ScrollButtonBorderStyle(widget)[1] = EZ_BORDER_DOWN;
                doit=1;
              }
            if(doit)
              {
                XEvent xevent;
                int jjj=10,nnn=10;
                unsigned long pp = 20000;
                if(EZ_ScrollButtonArrowType(widget))
                  {
                    int obd = EZ_WidgetBorderStyle(widget);
                    { EZ_InstallHighlightColor(widget); } /*4/29/99*/
                    EZ_WidgetBorderStyle(widget) = EZ_OppsiteBorder(obd, 1);
                  }
                EZ_DrawWidget(widget);
                { EZ_ExecuteWidgetCallBacks(widget);}
                XSync(EZ_Display, False);
                while(doit)
                  {
                    EZ_SitForALittleBit(pp);
                    while(XPending(EZ_Display))
                      {
                        XNextEvent(EZ_Display, &xevent);
                        if(xevent.type == ButtonRelease && xevent.xbutton.button == EZ_Btn1)
                          doit = 0;
                        else if(xevent.type == Expose || xevent.type == FocusIn || xevent.type == FocusOut) 
                          EZ_WidgetDispatchEvent(&xevent);
                      }
                    if(!doit) break;
		    else
		      {
			if(--jjj <= 0) 
			  { 
			    int t=EZ_ScrollButtonScrollCount(widget);
			    jjj = --nnn;
			    { EZ_ExecuteWidgetCallBacks(widget);}
			    EZ_ScrollButtonScrollCount(widget)= t+(t>0? 1: -1);
                            if(jjj < -100) pp = 0;
                            else if(jjj < -50) pp = 10000;
			  }
		      }
                  }
                EZ_ScrollButtonBorderStyle(widget)[0] = EZ_BORDER_UP;
                EZ_ScrollButtonBorderStyle(widget)[1] = EZ_BORDER_UP;
                EZ_ScrollButtonHighlight(widget)[0] = 0;
                EZ_ScrollButtonHighlight(widget)[1] = 0;
                if(EZ_ScrollButtonArrowType(widget))
                  {
                    int obd = EZ_WidgetBorderStyle(widget);
                    EZ_ClearWidgetHighlightFlag(widget);
                    { EZ_UninstallHighlightColor(widget); } /*4/29/99*/
                    EZ_WidgetBorderStyle(widget) = EZ_OppsiteBorder(obd,0);
                  }
                EZ_DrawWidget(widget);
              }
          }
        else  if(event->xbutton.button == EZ_Btn3)
          {
            EZ_HandleDnDEvents(widget, event);
          }
        break;
      case MotionNotify:
        {
          int x = event->xmotion.x;
          int y = event->xmotion.y;
          if(x>=box1[0] && x<=box1[0]+box1[2] &&
             y>=box1[1] && y<=box1[1]+box1[2])
            {
              if(EZ_ScrollButtonHighlight(widget)[0] == 0)
                {
                  EZ_ScrollButtonHighlight(widget)[0] = 1;
                  EZ_DrawWidget(widget);
                }
            }
          else
            {
              if(EZ_ScrollButtonHighlight(widget)[0] != 0)
                {
                  EZ_ScrollButtonHighlight(widget)[0] = 0;
                  EZ_DrawWidget(widget);
                }
            }
          if(x>=box2[0] && x<=box2[0]+box2[2] &&
             y>=box2[1] && y<=box2[1]+box2[2])
            {
              if(EZ_ScrollButtonHighlight(widget)[1] == 0)
                {
                  EZ_ScrollButtonHighlight(widget)[1] = 1;
                  EZ_DrawWidget(widget);
                }
            }
          else
            {
              if(EZ_ScrollButtonHighlight(widget)[1] != 0)
                {
                  EZ_ScrollButtonHighlight(widget)[1] = 0;
                  EZ_DrawWidget(widget);
                }
            }
          break;
        }
       break;
      case EnterNotify:
        if(EZ_ScrollButtonArrowType(widget))
          {
            EZ_SetWidgetHighlightFlag(widget);
            { EZ_InstallHighlightColor(widget); } /*4/29/99*/        
            EZ_DrawWidget(widget);
          }
      case LeaveNotify:
        {
          EZ_ScrollButtonHighlight(widget)[0] = 0;
          EZ_ScrollButtonHighlight(widget)[1] = 0;
          EZ_ScrollButtonBorderStyle(widget)[0] = EZ_BORDER_UP;
          EZ_ScrollButtonBorderStyle(widget)[1] = EZ_BORDER_UP;
          if(EZ_ScrollButtonArrowType(widget))
            {
              EZ_ClearWidgetHighlightFlag(widget);
              { EZ_UninstallHighlightColor(widget); } /*4/29/99*/
            }
          EZ_DrawWidget(widget);
        }
      break;
      case KeyPress:
	{
	  char              tmpbuffer[8];
	  int               buffersize = 8, acc = 1;
	  KeySym            keysym;
	  XComposeStatus    compose; 
	  int count = XLookupString(&(event->xkey), tmpbuffer, buffersize, &keysym, &compose);
	  tmpbuffer[count] = '\0';
	  if(event->xkey.state & ShiftMask) acc = 10;

	  switch(keysym)
	    {
	    case XK_Up: case XK_k:  case XK_p: case XK_K:  case XK_P: 
#ifdef XK_KP_Up
	    case XK_KP_Up:
#endif
	    case XK_Right: case XK_f: case XK_l: case XK_F: case XK_L:  
#ifdef XK_KP_Right
	    case XK_KP_Right: 
#endif
	      EZ_ScrollButtonScrollCount(widget)=acc * getDirection(type,1);	      
	      { EZ_ExecuteWidgetCallBacks(widget);}
	      break;
	    case XK_Down: case XK_n: case XK_j: case XK_N: case XK_J:  
#ifdef XK_KP_Down
	    case XK_KP_Down:
#endif
	    case XK_Left: case XK_b:  case XK_h: case XK_B:  case XK_H:  
#ifdef XK_KP_Left
	    case XK_KP_Left:
#endif
	      EZ_ScrollButtonScrollCount(widget)=acc * getDirection(type,-1);
	      { EZ_ExecuteWidgetCallBacks(widget);}
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
      default:
        break;
      }  
  }
}
/*******************************************************************/
int EZ_GetScrollButtonScrollCount(widget) EZ_Widget *widget;
{if(widget) return(EZ_ScrollButtonScrollCount(widget)); return(0);}
/*******************************************************************/
#undef _EZ_WIDGET_SCROLL_BUTTON_C_
