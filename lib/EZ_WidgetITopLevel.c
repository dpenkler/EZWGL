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
 ***       Internal Toplevel Widget                            ***
 ***                                                           ***
 *****************************************************************/
#define _EZ_WIDGET_ITOPLEVEL_C_
#include "EZ_Widget.h"

/*********************************************************************
 * 
 *  Functions implemented in this file:
 */
EZ_Widget       *EZ_CreateFreeLabel MY_ANSIARGS((EZ_Widget *widget, char *label));
EZ_Widget       *EZ_CreateIcon MY_ANSIARGS((void));
EZ_Widget       *EZ_CreateIToplevelFrame MY_ANSIARGS((EZ_Widget *widget, char *label));
void            EZ_IToplevelEventHandle MY_ANSIARGS((EZ_Widget *widget, XEvent *event));
int             EZ_GetIconTextGeometry MY_ANSIARGS((EZ_Widget *icon, int *x, int *y,
                                                    int *w, int *h));
void            EZ_ConstrainFreeLabel MY_ANSIARGS((EZ_Widget *widget, int flag));
/*********************************************************************
 * 
 *  Local Variables.
 */
static void             EZ_DrawWidgetITL MY_ANSIARGS((EZ_Widget *widget));
static void             EZ_ComputeWidgetITLSize MY_ANSIARGS((EZ_Widget *widget, int *w, int *h));
static EZ_WidgetHandle EZ_IToplevelHandle =
{ 
  EZ_ComputeWidgetITLSize,
  EZ_DrawWidgetITL,
  EZ_FreeWidgetLabelData,
  EZ_IToplevelEventHandle,     /* response button events */
};

/*********************************************************************/

EZ_Widget  *EZ_CreateIcon()
{
  EZ_Widget *tmp = EZ_CreateIToplevelFrame(NULL, NULL);
  if(tmp)
    {
      EZ_WidgetBorderStyle(tmp) = EZ_BORDER_UP;
      EZ_WidgetBorderWidth(tmp) = 2;
      EZ_LabelIsLabeledIcon(tmp) = 1;
      EZ_SetWidgetShapedWinFlag(tmp);  /* use shapeExtension */
    }
  return(tmp);
}

EZ_Widget  *EZ_CreateFreeLabel(parent, label)
     EZ_Widget *parent; char *label;
{
  return(EZ_CreateIToplevelFrame(parent, label));
}

EZ_Widget  *EZ_CreateIToplevelFrame(parent, label)
     EZ_Widget *parent; char *label;
{
  EZ_Widget  *tmp = EZ_CreateNewWidget(parent);
  
  EZ_WidgetHandlingFunctions[EZ_WIDGET_ITOPLEVEL] = &EZ_IToplevelHandle;
  EZ_SetWidgetTypeAndNames(tmp,EZ_WIDGET_ITOPLEVEL);

  EZ_SetWidgetXSetFlag(tmp);
  EZ_SetWidgetYSetFlag(tmp);
  EZ_WidgetBorderStyle(tmp)  = EZ_BORDER_FRAMED_UP;
  EZ_WidgetBorderWidth(tmp)  = 0;
  EZ_WidgetSepX(tmp)         = 2;
  EZ_WidgetSepY(tmp)         = 2;
  EZ_WidgetPadB(tmp)         = 0;
  EZ_LabelMaskDirty(tmp)     = 1;
  EZ_LabelMaskNotDirty(tmp)  = 0;
  EZ_LabelTextMaskDirty(tmp) = 1;

  EZ_LabelPixmap(tmp) = (EZ_Bitmap *)NULL;
  EZ_LabelString(tmp) = EZ_AllocCopyString(label);
  EZ_LabelStringLength(tmp) = (label == (char *)NULL ? 1: strlen(label));
  EZ_LabelLineLength(tmp) = EZ_DEFAULT_LABEL_LINE_LENGTH;
  EZ_LabelNLines(tmp) = 1;
  EZ_LabelFont(tmp) = EZ_GetFontFromId(EZ_LABEL_FONT);
  EZ_LabelTWidth(tmp)  = 0;
  EZ_LabelTHeight(tmp) = 0;
  EZ_LabelFontAscent(tmp) = 0;
  EZ_LabelAlwaysHighlight(tmp) = 0;
  /* for free label: serve for special popups */
  EZ_LabelMovable(tmp) = 1; 
  EZ_LabelDontTakeFocus(tmp) = 0;
  EZ_LabelIsMenuTitle(tmp) = 0;
  EZ_LabelIsLabeledIcon(tmp) = 0;
  EZ_LabelTextMask(tmp) = (Pixmap)None;
  EZ_LabelUnderline(tmp) = -1;
  EZ_LabelModifiers(tmp) = 0;
  EZ_LabelShortcut(tmp) = NULL;
  EZ_LabelMenu(tmp) = NULL;
  EZ_LabelCopy(tmp) = NULL;

  EZ_SetDoNotPropagateFlag(tmp);
  EZ_SetWidgetFocusableFlag(tmp);

  return(tmp);
}

/****************************************************************************/
void  EZ_IToplevelEventHandle(widget, event)
     EZ_Widget *widget;
     XEvent    *event;
{
  int     x,y, bx,by, dx,dy, rootx, rooty, oldx, oldy, done, buttonP;
  XEvent  xevent;
  Window  root, child;
  unsigned int    mask;
    
  if(widget == (EZ_Widget *)NULL || 
     widget == EZ_BubbleHelp) return;

  switch(event->type)
    {
    case Expose:
      EZ_DrawWidget(widget);
      break;
    case ButtonPress:
      if(EZ_WidgetParent(widget) != NULL)
	{
	  XRaiseWindow(EZ_Display, EZ_WidgetWindow(widget));
	  EZ_DrawWidget(widget);      
	}
      buttonP = event->xbutton.button;
      if(buttonP == EZ_Btn1)
	{
	  Time  time_stamp = event->xbutton.time;   
	  int   elapsedtime = EZ_LastButton1ClickTime - time_stamp;
          int   constrained = EZ_IToplevelConstrained(widget);
          int   pw=0, ph=0, mw=0, mh=0;

          if(constrained) 
            {
              EZ_Widget *parent = EZ_WidgetParent(widget);
              if(parent) 
                {
                  pw=EZ_WidgetWidth(parent); 
                  ph = EZ_WidgetHeight(parent);
                  mw = EZ_WidgetWidth(widget);
                  mh = EZ_WidgetHeight(widget);
                }
              else constrained = 0;
            }
	  EZ_LastButton1ClickTime = time_stamp;
	  
	  if(ABSV(elapsedtime) < DOUBLE_CLICK_TIME_INTERVAL)
	    {
	      if(EZ_WidgetCallBackFunc(widget))
		{
		  { EZ_ExecuteWidgetCallBacks(widget);}
		  return;
		}
	    }
	
	  if(EZ_LabelMovable(widget) != 0) /* move the label */
	    {
              int moved = 0;
	      bx = event->xbutton.x;
	      by = event->xbutton.y;
	      XQueryPointer(EZ_Display, RootWindow(EZ_Display, EZ_ScreenNum),
			    &root,                          /* root return win */
			    &child,                         /* child ret win   */
			    &oldx, &oldy,                   /* x, y in root    */
			    &x,&y,                          /* x, y in win     */
			    &mask);  
	      /*
	       * wait for a matching ButtonRelease. In the mean time, process
	       * MotionNotify and SelectionRequest events.
	       */
	      done = 0;
	      while(!done)
		{
                  Window win;
		  XNextEvent(EZ_Display, &xevent); 
		  EZ_FilterEvent(&xevent);
                  EZ_InvokePrivateEventHandler(&xevent);
                  win = xevent.xany.window;
		  if(win != EZ_WidgetWindow(widget))
		    EZ_WidgetDispatchEvent(&xevent);
		  else
		    {
                      int txx, tyy;
		      switch(xevent.type)
			{
                        case Expose:
                          while(XCheckWindowEvent(EZ_Display,win, ExposureMask, &xevent));
                          if(xevent.xexpose.count == 0)EZ_DrawWidget(widget);
                          break;
			case MotionNotify:
			  XQueryPointer(EZ_Display, RootWindow(EZ_Display, EZ_ScreenNum),
					&root,       /* root return win */
					&child,      /* child ret win   */
					&rootx, &rooty,   /* x, y in root    */
					&x,&y,            /* x, y in win     */
					&mask);  
			  dx = rootx - oldx;  dy = rooty - oldy;
			  oldx = rootx;   oldy = rooty;
                        
                          if(dx | dy)
                            {
                              txx = EZ_WidgetOriginX(widget) + dx;
                              tyy = EZ_WidgetOriginY(widget) + dy;
                              if(constrained)
                                {
                                  if(pw <= mw)
                                    {
                                      if(txx > 0) txx = 0;
                                      else if(txx < pw-mw) txx = pw -mw;
                                    }
                                  else
                                    {
                                      if(txx < 0) txx = 0;
                                      else if(txx > pw-mw) txx =pw-mw;
                                    }
                                  if(ph <= mh)
                                    {
                                      if(tyy > 0) tyy = 0;
                                      else if(tyy < ph-mh) tyy = ph -mh;
                                    }
                                  else
                                    {
                                      if(tyy<0) tyy=0;
                                      else if(tyy >ph-mh) tyy=ph-mh;
                                    }
                                }
                              EZ_WidgetOriginX(widget) =txx;
                              EZ_WidgetOriginY(widget) =tyy;

                              EZ_ClearWidgetSizeComputedFlag(widget);
                              EZ_LabelMaskNotDirty(widget) = 1;
                              if(EZ_LabelMotionCallBack(widget))
                                {
                                  EZ_CallBackStruct *callbacks = EZ_LabelMotionCallBack(widget);
                                  EZ_HandleMotionCallBacks(widget,callbacks);
                                }
                              /* EZ_ReDisplayWidget(widget);*/
                              XMoveWindow(EZ_Display, EZ_WidgetWindow(widget),
                                          EZ_WidgetOriginX(widget), EZ_WidgetOriginY(widget));
                              moved = 1;
                            }
			  while(XCheckTypedEvent(EZ_Display, MotionNotify, &xevent))
			    EZ_FilterEvent(&xevent);
			  break;
			case ButtonRelease:
			  if(xevent.xbutton.button == buttonP) done = 1;
			  break;
			default:
			  break;
			}
		    }
		}
	      /* done, if this is a toplevel, generate embeding request */
	      if(EZ_WidgetParent(widget) == (EZ_Widget *)NULL &&
		 EZ_GetWidgetEmbededFlag(widget) == 0)
		EZ_GenerateEmbedingRequest(widget, oldx - bx,oldy -by, 
					   (int)EZ_WidgetWidth(widget),
					   (int)EZ_WidgetHeight(widget));
              if(moved)
                {
                  XRaiseWindow(EZ_Display, EZ_WidgetWindow(widget));
                  EZ_DrawWidget(widget);      
                }
	    }
	}
      else if(buttonP == EZ_Btn3)
	{
	  /*
	    #include "EZ_DnDHandler1.c"
	    EZ_DnDEventHandler1(widget, event);
	    */
          EZ_HandleDnDEvents(widget, event);
	}
      break;
    case EnterNotify:
      if(EZ_WidgetParent(widget) == NULL && EZ_WidgetMapped(widget)  &&
	 EZ_LabelDontTakeFocus(widget) == 0 )
	{
	  if(event->xcrossing.focus == False)  EZ_ResetFocus(widget, 100);
	}
      break;
    case LeaveNotify:
      break;
    default:
      break;
    }
}

/********************************************************************
 *
 *  Figure out the dimension of a label widget.
 */

void EZ_ComputeWidgetITLSize(widget, w, h)
     EZ_Widget *widget;
     int             *w, *h;
{
  int length, height, cw, ch, tmp, orien, bw, bwpad;
  int kind, ibw;

  if(EZ_LabelIsLabeledIcon(widget) == 0)
    {
      EZ_ComputeWidgetLabelSize(widget, w, h);
      return;
    }

  /*
   * a hack for real icon ! 
   * an icon has two parts, a pixmap label + a texutal label,
   * with pixmap atop of text 
   */
  bw =  EZ_WidgetBorderWidth(widget);
  ibw = EZ_LabelIBorderWidth(widget);
  bwpad = bw + EZ_WidgetPadB(widget);
  if(!(EZ_GetWidgetSizeComputedFlag(widget)))
    { 
      kind = 0;
      if(EZ_LabelPixmap(widget) != (EZ_Bitmap *)NULL)
	{
	  EZ_Bitmap *bitmap = EZ_LabelPixmap(widget);
	  EZ_LabelPixW(widget) = bitmap->width;
	  EZ_LabelPixH(widget) = bitmap->height;
	  kind++;
	}
      else { EZ_LabelPixW(widget) = 0; EZ_LabelPixH(widget) = 0; }
      /* now the textual part */
	{
	  char *str = EZ_LabelString(widget);
	  if(str && *str)
	    {
	      int nlines, twidth;
	      EZ_FigureLabelExtent(EZ_LabelFont(widget),
				   str,
				   (int)EZ_LabelStringLength(widget),
				   (int)EZ_LabelLineLength(widget),
				   &nlines, &twidth);
	      EZ_LabelTWidth(widget) = twidth;
	      EZ_LabelNLines(widget) = nlines;
	      EZ_LabelTHeight(widget) = (EZ_LabelFont(widget)->ascent +
					 EZ_LabelFont(widget)->descent);
	      EZ_LabelFontAscent(widget) = EZ_LabelFont(widget)->ascent;
	      kind++;
	    }
	  else
	    {
	      EZ_LabelTWidth(widget) = 0;
	      EZ_LabelNLines(widget) = 0;
	      EZ_LabelTHeight(widget) = 0;
              EZ_LabelFontAscent(widget) = 0;
	    }
	}
      EZ_SetWidgetSizeComputedFlag(widget);
      EZ_LabelMaskDirty(widget) |= 8;
      EZ_IconKind(widget) = kind;
    }
  orien = EZ_WidgetStacking(widget);
  kind = EZ_IconKind(widget);
  switch(orien)
    {
    case EZ_HORIZONTAL:
    case EZ_HORIZONTAL_LEFT:
    case EZ_HORIZONTAL_RIGHT:
    case EZ_HORIZONTAL_CENTER:
      /* width */
      length = EZ_LabelTWidth(widget);
      tmp = EZ_LabelPixW(widget);
      if(length > 0 && tmp > 0)
	length += tmp + EZ_WidgetSepX(widget);
      else length += tmp;
      /* height */
      height = EZ_LabelTHeight(widget) * EZ_LabelNLines(widget);
      tmp = EZ_LabelPixH(widget);
      if(height < tmp ) height = tmp;
      if(kind == 2) length += ibw;
      break;
    case EZ_VERTICAL:
    case EZ_VERTICAL_TOP:
    case EZ_VERTICAL_BOTTOM:
    case EZ_VERTICAL_CENTER:
    default:
      /* width */
      length = EZ_LabelTWidth(widget);
      tmp = EZ_LabelPixW(widget);
      if(length < tmp ) length = tmp;

      /* height */
      height = EZ_LabelTHeight(widget) * EZ_LabelNLines(widget);
      tmp = EZ_LabelPixH(widget);
      if(height > 0 && tmp > 0) tmp += EZ_WidgetSepY(widget);
      height += tmp;
      if(kind == 2) height += ibw;
      break;
    }

  cw = EZ_WidgetPadX(widget) + bwpad;
  ch = EZ_WidgetPadY(widget) + bwpad;
  *w = cw + cw + length;
  *h = ch + ch + height;  
}

/*******************************************************************
 *
 *  Draw A Label Widget.
 */
void  EZ_DrawWidgetITL(wptr)
     EZ_Widget *wptr;
{
  int isIcon = EZ_LabelIsLabeledIcon(wptr);
  if(isIcon == 0) {  EZ_DrawWidgetLabel(wptr);   return;   }
  /* icon ! */
  {
    int       w, h,x,y,xx,yy, padb, padb2, shape, pax, pay,tax, tay;
    int       fillx, filly, bwpadx, bwpady, orien, txtx, txty, ascent, isDrag;
    int       kind, txadj, tyadj, bw, ibw;
    Pixmap    pixmap;
    Window    win;
    GC        gc;
    unsigned long   bgpv;

    win = EZ_WidgetWindow(wptr);
    w   = EZ_WidgetWidth(wptr);
    h   = EZ_WidgetHeight(wptr);
    padb = EZ_WidgetPadB(wptr);
    padb2 = padb + padb;
    kind = EZ_IconKind(wptr);
    bw = EZ_WidgetBorderWidth(wptr);
    ibw = EZ_LabelIBorderWidth(wptr);

    EZ_GetWidgetLabelPosition(wptr, &fillx, &filly);
    xx = fillx + EZ_WidgetXOffset(wptr);
    yy = filly + EZ_WidgetYOffset(wptr);

    bwpadx = EZ_WidgetPadX(wptr) + EZ_WidgetBorderWidth(wptr) + padb;
    bwpady = EZ_WidgetPadY(wptr) + EZ_WidgetBorderWidth(wptr) + padb;
    x = xx + bwpadx;
    y = yy + bwpady;

    orien = EZ_WidgetStacking(wptr);
    txadj = tyadj = 0; 
    switch(orien)
      {
      case EZ_HORIZONTAL:
      case EZ_HORIZONTAL_LEFT:
      case EZ_HORIZONTAL_RIGHT:
      case EZ_HORIZONTAL_CENTER:	
	pax = 0;
	pay = ((EZ_WidgetMinHeight(wptr) - EZ_LabelPixH(wptr)) >> 1) - bwpady;
	tax = 0;
	tay =((EZ_WidgetMinHeight(wptr) - EZ_LabelTHeight(wptr) * EZ_LabelNLines(wptr))>>1) -bwpady;
	txtx = EZ_WidgetSepX(wptr) + EZ_LabelPixW(wptr);	
	txty = 1;
        if(kind == 2) txadj= ibw; 
	break;
      case EZ_VERTICAL:
      case EZ_VERTICAL_TOP:
      case EZ_VERTICAL_CENTER:
      case EZ_VERTICAL_BOTTOM:
      default:
	pax = ((EZ_WidgetMinWidth(wptr) - EZ_LabelPixW(wptr)) >> 1) - bwpadx;
	pay = 0;
	tax = ((EZ_WidgetMinWidth(wptr) - EZ_LabelTWidth(wptr)) >> 1) - bwpadx;
	tay = 0;
	txtx = 0;
	txty = EZ_WidgetSepY(wptr) + EZ_LabelPixH(wptr);	
        if(kind == 2) tyadj= ibw; 
	break;
      }
    ascent = EZ_LabelFontAscent(wptr);
    pixmap = XCreatePixmap(EZ_Display, win, w, h, EZ_Depth); 
    
    /* figure out the shape of the icon window */
    if(EZ_XServerHasSHAPEExt && EZ_GetWidgetShapedWinFlag(wptr))
      {
	int dirty = EZ_LabelMaskDirty(wptr); /* bit 1 set: if new label or pixmap is set
					      * bit 3 set: if just may be dirty */
	int realdirty = (EZ_LabelMaskNotDirty(wptr) == 0 ? 9 : 1);
	/* if (dirty & 1) || (dirty && EZ_LabelMaskNotDirty(wptr) == 0) */
	if( (dirty & realdirty) != 0 )
	  {
	    EZ_Bitmap *bitmap = EZ_LabelPixmap(wptr);
	    shape = ( bitmap != NULL && bitmap->shape != None);

	    if(!shape)  /* remove shape */
              {
                if(EZ_GetWidgetShapedSetFlag(wptr))
                  {
                    XShapeCombineMask(EZ_Display,  win,
                                      ShapeBounding, 0, 0, None, ShapeSet);
                    EZ_ClearWidgetShapedSetFlag(wptr);
                  }
              }
	    else
	      {
		/* shape from pixmap */
		XShapeCombineMask(EZ_Display,  win,
				  ShapeBounding, x+pax, y+pay,
				  bitmap->shape, ShapeSet);
                EZ_SetWidgetShapedSetFlag(wptr);
		if(EZ_GetWidgetShapeShiftFlag(wptr))
		  {
		    XShapeCombineMask(EZ_Display,  win,
				      ShapeBounding, x+pax+1, y+pay+1,
				      bitmap->shape, ShapeSet);
		  }
	    
		/* shape for text */
		if(EZ_LabelTHeight(wptr) > 0)
		  {
		    int tw = EZ_LabelTWidth(wptr);
		    int th = EZ_LabelTHeight(wptr) * EZ_LabelNLines(wptr);
                    int shadow = EZ_WidgetLabelShadow(wptr);
                    int shadowx, shadowy;
                    if(shadow &0xf0)
                      {
                        shadowx = (shadow & 0xf0) >> 4;
                        shadowy = (shadow & 0x0f);
                      }
                    else shadowx = shadowy = shadow;                    
                    if(shadowx & 8) shadowx= (8-shadowx);
                    if(shadowy & 8) shadowy= (8-shadowy);
                    { tw += shadowx; th += shadowy;}

		    if( EZ_LabelTextMaskDirty(wptr)) /* create mask */
		      {
			Pixmap oldTMask = EZ_LabelTextMask(wptr);
			Pixmap   bmap = None;			

			if(oldTMask != None)  XFreePixmap(EZ_Display, oldTMask);
			EZ_LabelTextMaskDirty(wptr) = 0;

			if(isIcon > 1)
			  {
			    XImage *image;
			    gc = EZ_WRITABLEGC;
			    XSetFont(EZ_Display, gc, EZ_LabelFont(wptr)->fid);
			    XSetForeground(EZ_Display, gc, 0);
			    XFillRectangle(EZ_Display, pixmap, gc, 0, 0, tw, th); 
			    XSetForeground(EZ_Display, gc, 1);
                            if(shadow)
                              EZ_RenderLabelText(pixmap, gc, shadowx, ascent+shadowy,
                                                 (int)EZ_LabelTHeight(wptr), EZ_LabelString(wptr),
                                                 (int)EZ_LabelStringLength(wptr), (int)EZ_LabelLineLength(wptr),
                                                 (int)EZ_LabelNLines(wptr),
                                                 (int)EZ_LabelTWidth(wptr),
                                                 (int)EZ_WidgetJustification(wptr),
                                                 EZ_LabelFont(wptr), 0);                            
			    EZ_RenderLabelText(pixmap, gc, 0, ascent,
					       (int)EZ_LabelTHeight(wptr), EZ_LabelString(wptr),
					       (int)EZ_LabelStringLength(wptr), (int)EZ_LabelLineLength(wptr),
					       (int)EZ_LabelNLines(wptr),
					       (int)EZ_LabelTWidth(wptr),
					       (int)EZ_WidgetJustification(wptr),
					       EZ_LabelFont(wptr), 0);
		
			    image = XGetImage(EZ_Display, pixmap, 0,0, tw, th, 1, XYPixmap);
			    {
			      int xc, yc, advance, itmp, jtmp, ktmp;
			      unsigned int ch;
			      unsigned char *data, *ptr;
			      unsigned long pixel;
			      itmp = ((tw + 7)/8);
			      jtmp = itmp * th;
			      advance = itmp;
			      data = (unsigned char *)my_malloc(jtmp * sizeof(char), _EZ_LED_DATA_);
		  
			      for(ptr = data, yc = 0; yc < th; yc++, ptr += advance)
				{
				  for(ch = 0, ktmp = 0, itmp = 0, xc = 0; xc < tw; xc++)
				    {
				      pixel = XGetPixel(image,xc,yc);
				      if(pixel != 0) ch |= (1 << ktmp);
				      ktmp++;
				      if(ktmp == 8)
					{
					  ptr[itmp] = (unsigned char)ch;
					  itmp++;
					  ktmp = 0;
					  ch = 0;
					}
				    }
				  if(ktmp != 0){ptr[itmp] = (unsigned char)ch;}
				}
			      bmap = XCreateBitmapFromData(EZ_Display,
							   RootWindow(EZ_Display, EZ_ScreenNum),
							   data,
							   tw, th);
			      my_free((char *)data);
			      XDestroyImage(image);
			    }
			  }
			EZ_LabelTextMask(wptr) = bmap;
                      }
		    /* set the mask */
		    if(EZ_LabelTextMask(wptr) != None)
		      {
			XShapeCombineMask(EZ_Display,  win, ShapeBounding,
					  x + tax +txtx +txadj, y + tay+txty +tyadj,
					  EZ_LabelTextMask(wptr), ShapeUnion);
			if(EZ_GetWidgetShapeShiftFlag(wptr))
			  {
			    XShapeCombineMask(EZ_Display,  win, ShapeBounding,
					      x + tax+txtx+1 +txadj, y + tay+txty+1 + tyadj,
					      EZ_LabelTextMask(wptr), ShapeUnion);

			  }
                        EZ_SetWidgetShapedSetFlag(wptr);
		      }
		    else /* rectangle */
		      {
			XRectangle rects[1];
			rects[0].x = 0;
			rects[0].y = 0;
			rects[0].width = tw + ibw+ibw;
			rects[0].height = th +ibw+ibw;

			XShapeCombineRectangles(EZ_Display,  win, ShapeBounding,
						x + tax + txtx-tyadj, y + tay + txty -txadj,
						rects, 1, ShapeUnion, YSorted);
			if(EZ_GetWidgetShapeShiftFlag(wptr))
			  {
			    XShapeCombineRectangles(EZ_Display,  win, ShapeBounding,
						    x + tax+txtx-tyadj+1, y + tay + txty -txadj+1,
						    rects, 1, ShapeUnion, YSorted);
			    
			  }
                        EZ_SetWidgetShapedSetFlag(wptr);
		      }
		  }
	      }
	    EZ_LabelMaskDirty(wptr) = 0;
	    EZ_LabelMaskNotDirty(wptr) = 0;
	  }
      }
    /* rendering */
    EZ_FixFocusHighlightBorder(wptr, pixmap, w, h, padb);

    if(EZ_GetBackgroundGC(wptr, &gc, &bgpv, 1, 0))
      {
	int ox, oy;
	EZ_GetWidgetTileOrigin(wptr, &ox, &oy);      
	XSetTSOrigin(EZ_Display, gc, ox, oy);
      }
    XFillRectangle(EZ_Display, pixmap, gc, padb,padb, w-padb2, h-padb2); 

    isDrag = EZ_LabelIsDrag(wptr);
    if(isDrag) gc = EZ_DRAGGC;
    else  gc = EZ_WRITABLEGC;

    /* pixmap label first */
    if(EZ_LabelPixmap(wptr) != (EZ_Bitmap *)NULL)
      {
	EZ_Bitmap *tmp = EZ_LabelPixmap(wptr);
	if(isDrag) 
	  {
	    if(tmp->shape != (Pixmap)NULL)
	      {
		XSetClipMask(EZ_Display, gc, tmp->shape);
		XSetClipOrigin(EZ_Display, gc,x+pax,y+pay);
	      }
	    EZ_RenderPixmapLabelWithGC(tmp, pixmap, gc, x+pax,y+pay);
	    if(tmp->shape != None) XSetClipMask(EZ_Display, gc, None);
	  }
	else  EZ_RenderPixmapLabel(wptr, tmp, pixmap, x+pax, y+pay);
      }
    /* textual label */
    if( EZ_LabelTHeight(wptr) > 0)
      {
        int xx0 =  x+tax+txtx +txadj;
        int yy0 = y + txty + tay + tyadj;
        short *txtpos = EZ_IToplevelTextGeom(wptr);
        int shadow = EZ_WidgetLabelShadow(wptr);

	if(!isDrag) XSetForeground(EZ_DisplayForWidgets, gc, EZ_LabelForeground(wptr));
	XSetFont(EZ_DisplayForWidgets, gc, EZ_LabelFont(wptr)->fid);
	
	EZ_RenderLabelText(pixmap, gc, xx0, yy0 + ascent,
			   (int)EZ_LabelTHeight(wptr), EZ_LabelString(wptr),
			   (int)EZ_LabelStringLength(wptr), (int)EZ_LabelLineLength(wptr),
			   (int)EZ_LabelNLines(wptr),
			   (int)EZ_LabelTWidth(wptr),
			   (int)EZ_WidgetJustification(wptr),
			   EZ_LabelFont(wptr), shadow);
        txtpos[0] = (short)xx0;
        txtpos[1] = (short)yy0;
        txtpos[2] = (short)EZ_LabelTWidth(wptr);
        txtpos[3] = (short)EZ_LabelTHeight(wptr) * EZ_LabelNLines(wptr);
      }
    if(kind ==2 && isIcon == 1 && ibw > 0)
      {
	int tw = EZ_LabelTWidth(wptr);
	int th = EZ_LabelTHeight(wptr) * EZ_LabelNLines(wptr);
	EZ_DrawRectBorderWithSize(wptr, pixmap, x+tax+txtx -tyadj, y + txty + tay -txadj,
				  tw +ibw +ibw,             /* ^^^^ */             /* ^^^^ */
				  th +ibw +ibw,
				  ibw,
                                  EZ_LabelIBorderType(wptr),
				  0,0,0);
      }
    /*============================================================
     *for complex Label with NW labels 
     */
    {
      EZ_Widget *children = EZ_WidgetChildren(wptr);
      if(children && EZ_WidgetType(children) == EZ_WIDGET_NW_LABEL)
	{
	  EZ_DisplayNWLabel(children, pixmap, EZ_WidgetForeground(wptr), bgpv,
			    EZ_GetWidgetDisabledFlag(wptr),EZ_GetWidgetHighlightFlag(wptr),
			    0, 0, 0, 1);
	}
    }
    /*============================================================*/  
    EZ_DrawRectBorder(wptr, pixmap);     
    XCopyArea(EZ_Display,pixmap,win, EZ_WRITABLEGC,0,0,w,h,0,0); 
    XFreePixmap(EZ_Display, pixmap); 
  }
}
/*****************************************************************************/
int  EZ_GetIconTextGeometry(icon, x,y,w,h)
     EZ_Widget *icon; int *x,*y, *w,*h;
{
  if(icon && EZ_WidgetType(icon) == EZ_WIDGET_ITOPLEVEL)
    {
      short *txtpos = EZ_IToplevelTextGeom(icon);
      *x = (int)txtpos[0];
      *y = (int)txtpos[1];
      *w = (int)txtpos[2];
      *h = (int)txtpos[3];
      return(1);
    }
  return(0);
}
/*****************************************************************************/
void EZ_ConstrainFreeLabel(widget, mode)
     EZ_Widget *widget; int mode;
{
  if(widget && EZ_WidgetType(widget) == EZ_WIDGET_ITOPLEVEL)  
    { EZ_IToplevelConstrained(widget) = mode; }
}
/*****************************************************************************/
#undef _EZ_WIDGET_ITOPLEVEL_C_


