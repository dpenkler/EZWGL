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
 ***              Widget Frame.                                ***
 ***                                                           ***
 *****************************************************************/
#define _EZ_WIDGET_FRAME_C_
#include "EZ_Widget.h"

/*********************************************************************
 * 
 *  Functions implemented in this file:
 */
EZ_Widget         *EZ_CreateFrame MY_ANSIARGS((EZ_Widget *parent, char *label));
void              EZ_DrawWidgetFrame MY_ANSIARGS((EZ_Widget *widget));
void              EZ_FreeWidgetFrameData MY_ANSIARGS((EZ_Widget *widget));
void              EZ_FrameEventHandle MY_ANSIARGS((EZ_Widget *widget, XEvent *event));
void              EZ_ComputeWidgetFrameSize MY_ANSIARGS((EZ_Widget *widget, int *w, int *h));
int               EZ_OnFrameDragHandle MY_ANSIARGS((EZ_Widget *widget, int x, int y));
void              EZ_HighlightFrameDragHandle MY_ANSIARGS((EZ_Widget *widget, int onoff));
int               EZ_DetachFrame MY_ANSIARGS((EZ_Widget *widget, int x, int y));
int               EZ_DockFrame MY_ANSIARGS((EZ_Widget *widget, int force));
int               EZ_XYIsAtCorner MY_ANSIARGS((int x,int y,int w,int h));
int               EZ_FrameIsDetached MY_ANSIARGS((EZ_Widget *widget));

void              EZ_ResetWidgetShape MY_ANSIARGS((EZ_Widget *widget, int sib));
/*********************************************************************
 * 
 *  Local Variables.
 */
static EZ_WidgetHandle EZ_FrameHandle =
{ 
  EZ_ComputeWidgetFrameSize,
  EZ_DrawWidgetFrame,
  EZ_FreeWidgetFrameData,
  EZ_FrameEventHandle,
};

/*********************************************************************/

EZ_Widget  *EZ_CreateFrame(parent,label)
     EZ_Widget  *parent;     /* parent widget    */
     char       *label;
{
  EZ_Widget  *tmp;

  tmp = EZ_CreateNewWidget(parent);
  /*--------------------------------------------------
   * Register the handling functions for Frame.
   *  has to be done after  EZ_CreateNewWiget.
   *-------------------------------------------------*/
  EZ_WidgetHandlingFunctions[EZ_WIDGET_FRAME] = &EZ_FrameHandle;

  EZ_SetWidgetTypeAndNames(tmp,EZ_WIDGET_FRAME);
  EZ_WidgetBorderWidth(tmp) = 0;
  EZ_WidgetPadB(tmp) = 0;
  EZ_WidgetBorderStyle(tmp)  = EZ_BORDER_NONE;;
  EZ_FramePixmap(tmp) = (EZ_Bitmap *)NULL;
  EZ_FrameFont(tmp) = EZ_GetFontFromId(EZ_LABEL_FONT);  
  EZ_FrameLabel(tmp) = EZ_AllocCopyString(label);
  EZ_FrameLabelLength(tmp) = (label == (char *)NULL ? 0 : strlen(label));
  EZ_FrameLineLength(tmp) = EZ_DEFAULT_LABEL_LINE_LENGTH;
  EZ_FrameNLines(tmp) = 1; 
  EZ_FrameTWidth(tmp) = 0;
  EZ_FrameTHeight(tmp) = 0;
  EZ_FrameFontAscent(tmp)= 0;
  EZ_FrameXIndent(tmp) = EZ_WIDGET_DEFAULT_PADX;
  EZ_FrameYIndent(tmp) = EZ_WIDGET_DEFAULT_PADY;
  EZ_FrameHasLabel(tmp) = 0;
  EZ_FrameWAdjust(tmp) = 0;

  EZ_FrameIntBdWidth(tmp) = 1;
  EZ_FrameIntBdType(tmp)  = EZ_BORDER_FRAMED_DOWN;
  EZ_FrameBrokenIntBdLength(tmp) = 0;

  return(tmp);
}

void   EZ_FreeWidgetFrameData(widget)
     EZ_Widget *widget;
{
  if(EZ_FramePixmap(widget) != (EZ_Bitmap *)NULL)
    EZ_FreeBitmap(EZ_FramePixmap(widget));
  if(EZ_FrameLabel(widget) != (char *)NULL)
    (void)my_free( (char *) EZ_FrameLabel(widget));
}
/*******************************************************************
 *
 * Compute Frame size.
 */
void  EZ_ComputeWidgetFrameSize(widget, w,h)
     EZ_Widget *widget;
     int             *w, *h;
{
  int cw, ch, tmp, width, height, lab_padx = 0, lab_pady = 0, adjust = 0;
  int draghandle = EZ_FrameDragHandle(widget);
  int      orien = EZ_FrameBarOrientation(widget);
  
  if(EZ_FrameInternal(widget))
    {
      EZ_WidgetBorderWidth(widget) = 0;
      EZ_WidgetPadB(widget) = 0;
      EZ_WidgetPadX(widget) = 0;
      EZ_WidgetPadY(widget) = 0;
      EZ_WidgetSepX(widget) = 0;
      EZ_WidgetSepY(widget) = 0;
      EZ_WidgetBorderStyle(widget)  = EZ_BORDER_NONE;;
      EZ_FramePixmap(widget) = (EZ_Bitmap *)NULL;
      *(EZ_FrameLabel(widget)) = '\0';
      EZ_FrameLabelLength(widget) = 0;
      EZ_FrameNLines(widget) = 1; 
      EZ_FrameTWidth(widget) = 0;
      EZ_FrameTHeight(widget) = 0;
      EZ_FrameFontAscent(widget)= 0;
      EZ_FrameHasLabel(widget) = 0;
      EZ_FrameWAdjust(widget) = 0;
    }
  
  if(!(EZ_GetWidgetSizeComputedFlag(widget)))
    {
      EZ_Bitmap *bitmap = EZ_FramePixmap(widget);
      char      *label =  EZ_FrameLabel(widget);
      
      if( bitmap || (label && *label))  /* frame has a label */
	{
          int nlines, twidth;
          int labelLen = EZ_FrameLabelLength(widget);
	  adjust = 6;
	  EZ_FrameHasLabel(widget) = 1;

          if(EZ_PTLabel && bitmap && (label && *label)) EZ_FrameLPad(widget) = 2;
          else EZ_FrameLPad(widget) = 0;
          
	  if(bitmap != (EZ_Bitmap *)NULL)
            {
              EZ_FrameMWidth(widget) = bitmap->width;
              EZ_FrameMHeight(widget) = bitmap->height;              
            }
          else
            {
              EZ_FrameMWidth(widget) = 0;
              EZ_FrameMHeight(widget) = 0;
            }

          EZ_FrameTWidth(widget) = 0;
          EZ_FrameNLines(widget) = 0;
          EZ_FrameTHeight(widget) = 0;
          if(!bitmap || EZ_PTLabel)
            {
              if(labelLen > 2 || *label != ' ')
                {
                  EZ_FigureLabelExtent(EZ_FrameFont(widget),
                                       EZ_FrameLabel(widget),
                                       (int)EZ_FrameLabelLength(widget),
                                       (int)EZ_FrameLineLength(widget),
                                       &nlines, &twidth);
                  EZ_FrameTWidth(widget) = twidth;
                  EZ_FrameNLines(widget) = nlines;
                  EZ_FrameTHeight(widget) = (EZ_FrameFont(widget)->ascent +
                                             EZ_FrameFont(widget)->descent);
                  EZ_FrameFontAscent(widget) = EZ_FrameFont(widget)->ascent;
                }
            }
          
	  tmp = EZ_FrameIntBdWidth(widget) + 1;
	  lab_padx = EZ_FrameXIndent(widget) + tmp;
	  lab_pady = EZ_FrameYIndent(widget) + tmp;

          if(orien < EZ_VERTICAL)
            {
              int tmp1 =  EZ_FrameTHeight(widget) * EZ_FrameNLines(widget);
              int tmp2 = EZ_FrameMHeight(widget);
              EZ_WidgetHAdjust(widget) = (tmp1 > tmp2? tmp1 : tmp2);
            }
          else
            {
              EZ_WidgetHAdjust(widget) = EZ_FrameTHeight(widget) * EZ_FrameNLines(widget)
                + EZ_FrameLPad(widget) + EZ_FrameMHeight(widget);
            }
	}
      else  /* a regular frame */
	{ 
	  EZ_FrameHasLabel(widget) = 0;
	  EZ_FrameTWidth(widget) =  0;
	  EZ_FrameTHeight(widget)=  0;
	  EZ_FrameMWidth(widget) =  0;
	  EZ_FrameMHeight(widget)=  0;
	  EZ_WidgetAuxXLeft(widget) = 0;
	  EZ_WidgetAuxXRight(widget) = 0;
	  EZ_WidgetAuxY(widget) = 0;
	  EZ_WidgetHAdjust(widget) = 0;
	}
      EZ_FrameWAdjust(widget) = adjust;
      EZ_WidgetAuxXLeft(widget) = lab_padx;
      EZ_WidgetAuxXRight(widget) = lab_padx;
      EZ_WidgetAuxY(widget) = lab_pady;
      EZ_SetWidgetSizeComputedFlag(widget);

      {
	switch(draghandle)
	  {
	  case EZ_LEFT:
	    EZ_WidgetAuxXLeft(widget) += 8;
	    EZ_FrameDHXL(widget) = 8;
	    EZ_FrameDHXR(widget) = 0;
	    EZ_FrameDHY(widget) = 0;
	    break;
	  case EZ_RIGHT:
	    EZ_WidgetAuxXRight(widget) += 8;
	    EZ_FrameDHXL(widget) = 0;
	    EZ_FrameDHXR(widget) = 8;
	    EZ_FrameDHY(widget) = 0;
	    break;
	  case EZ_TOP:
	    EZ_WidgetHAdjust(widget) += 8;
	    EZ_FrameDHXL(widget) = 0;
	    EZ_FrameDHXR(widget) = 0;
	    EZ_FrameDHY(widget) = 8;
	    break;
	  default:
	    break;
	  }
      }
    }
  if(orien < EZ_VERTICAL)
    {
      int tmp1 = EZ_FrameTHeight(widget) * EZ_FrameNLines(widget);
      int tmp2 = EZ_FrameMHeight(widget);
      width = EZ_FrameTWidth(widget) + EZ_FrameWAdjust(widget) +
        EZ_FrameMWidth(widget) + EZ_FrameLPad(widget);
      height= tmp1 > tmp2 ? tmp1 : tmp2;
    }
  else
    {
      int tmp1 = EZ_FrameTWidth(widget);
      int tmp2 = EZ_FrameMWidth(widget);
      width = (tmp1 > tmp2) ? tmp1 : tmp2;
      height =   EZ_FrameTHeight(widget) * EZ_FrameNLines(widget)+
        EZ_FrameLPad(widget) + EZ_FrameMHeight(widget);
    }
  
  tmp = EZ_WidgetBorderWidth(widget) + EZ_WidgetPadB(widget);
  cw = EZ_WidgetPadX(widget) + tmp + EZ_WidgetAuxXLeft(widget);
  ch = EZ_WidgetPadY(widget) + tmp + EZ_WidgetAuxY(widget);
  if(draghandle && ch < 4) ch  = 4;
  
  *w = cw + cw + width;
  *h = ch + ch + height;
}
/*******************************************************************
 *
 *  Draw A Frame Widget. 
 */
void  EZ_DrawWidgetFrame(wptr)
     EZ_Widget *wptr;
{
  int             w, h, x,y, padb, padb2, tile,dhxl,dhxr,dhy, draghandle, bwpad;
  int             padx, pady;
  Pixmap          pixmap;
  Window          win;
  GC              gc;
  unsigned long   bgpv;

  win = EZ_WidgetWindow(wptr);
  w   = EZ_WidgetWidth(wptr);
  h   = EZ_WidgetHeight(wptr);
  padb = EZ_WidgetPadB(wptr);
  padb2 = padb + padb;
  dhxl = EZ_FrameDHXL(wptr);
  dhxr = EZ_FrameDHXR(wptr);
  dhy = EZ_FrameDHY(wptr);
  draghandle = EZ_FrameDragHandle(wptr);
  bwpad = EZ_WidgetBorderWidth(wptr) + padb;
  padx = EZ_WidgetPadX(wptr);
  pady = EZ_WidgetPadY(wptr);
  

  pixmap = XCreatePixmap(EZ_DisplayForWidgets, win, w, h, EZ_DepthForWidgets); 
  EZ_FixFocusHighlightBorder(wptr, pixmap, w, h, padb);
  tile = EZ_GetBackgroundGC(wptr, &gc,&bgpv, 1, 0);
  if(tile)
    {
      int ox, oy;
      EZ_GetWidgetTileOrigin(wptr, &ox, &oy);      
      XSetTSOrigin(EZ_Display, gc, ox, oy);
    }  
  XFillRectangle(EZ_DisplayForWidgets, pixmap, gc, padb,padb, w-padb2, h-padb2); 
  
  if(EZ_FrameHasLabel(wptr)) /* a labeled frame */
    {
      EZ_Bitmap  *bitmap = EZ_FramePixmap(wptr);
      char *label = EZ_FrameLabel(wptr);
      int labelLen =  EZ_FrameLabelLength(wptr);
      int xoff, yoff, ytmp, wtmp, htmp, itmp;
      int orien = EZ_FrameBarOrientation(wptr);
      int twidth = EZ_FrameTWidth(wptr); 
      int theight = EZ_FrameTHeight(wptr) * EZ_FrameNLines(wptr);
      int pwidth = EZ_FrameMWidth(wptr);
      int pheight = EZ_FrameMHeight(wptr);
      int lpad = EZ_FrameLPad(wptr);      
      int tx0=0, ty0=0;
      int boxwidth, boxheight;

      if(orien < EZ_VERTICAL)
        {
          boxwidth = twidth + lpad + pwidth;
          boxheight = pheight > theight? pheight: theight;
        }
      else
        {
          boxwidth = (pwidth > twidth? pwidth : twidth);
          boxheight = theight + pheight + lpad;
        }
      itmp = bwpad;
      xoff = EZ_FrameXIndent(wptr) + itmp;
      yoff = EZ_FrameYIndent(wptr) + itmp;
      ytmp = (boxheight) >> 1;
      
      x = xoff + dhxl;
      y = ytmp + yoff + dhy;
      wtmp = w - (xoff << 1) - (dhxl + dhxr);
      htmp = h - (yoff << 1) - ytmp - dhy;

      EZ_DrawRectBorderWithSize(wptr, pixmap, x,y,wtmp,htmp,
				(int)EZ_FrameIntBdWidth(wptr),
				(int)EZ_FrameIntBdType(wptr),0,0, 2);
      
      /* find the place to render the label */

      switch(EZ_WidgetLabelPosition(wptr))
	{
	case EZ_CENTER:
	  x = ((w - (boxwidth+6 )) >> 1);
	  break;
	case EZ_RIGHT:
	  x = w - (boxwidth+6 + xoff + padx);
	  x -= EZ_FrameIntBdWidth(wptr);
	  break;
	case EZ_LEFT:
	default:
	  x +=  padx;
	  x += EZ_FrameIntBdWidth(wptr);
	  break;
	}

      x += dhxl - dhxr;
      /*
       * Now render the label.
       */
      if(bitmap || (labelLen >= 2 && *label != ' '))
        {
          XFillRectangle(EZ_Display, pixmap, gc, 
                         x,
                         y - 1, 
                         (int)(boxwidth+6),
                         (int)(EZ_FrameIntBdWidth(wptr) + 2));
          x += 3;
        }

      if(orien >= EZ_VERTICAL) /* label stacked vertically */
        {
          if(bitmap)
            {
              int px0, py0;

              if(twidth > pwidth)
                {
                  px0 = (twidth - pwidth) >> 1; py0 = 0;
                  tx0 = 0; ty0 = pheight + lpad;
                }
              else
                {
                  px0 = 0; py0=0;
                  tx0 = (pwidth-twidth)>>1; 
                  ty0 = pheight + lpad;
                }
              py0 += yoff + dhy;
              px0 += x;
              EZ_RenderPixmapLabel(wptr, bitmap, pixmap, px0, py0);
            }
        }
      else /* horizontal */
        {
          if(bitmap)
            {
              int px0, py0;
              
              if(theight >= pheight)
                {
                  px0 = 0; py0 = (theight - pheight) >> 1;
                  tx0 = pwidth + lpad; ty0 = 0;
                }
              else
                {
                  px0 = 0; py0 = 0;
                  tx0 = pwidth + lpad; 
                  ty0 = (pheight - theight) >> 1;
                }

              py0 += yoff + dhy;
              px0 += x;
              EZ_RenderPixmapLabel(wptr, bitmap, pixmap, px0, py0);
            }
        }
      if(!bitmap || EZ_PTLabel) /* textual label */
	{
          int shadow = EZ_WidgetLabelShadow(wptr);

	  if(labelLen >= 2 || *label != ' ') 
	    {
	      if(EZ_FrameForeground(wptr) != EZ_DefaultForeground ||
		 EZ_FrameFont(wptr) != EZ_GetFontFromId(EZ_LABEL_FONT))
		{
		  XSetFont(EZ_Display, EZ_WRITABLEGC, EZ_FrameFont(wptr)->fid);
		  XSetForeground(EZ_Display, EZ_WRITABLEGC, EZ_FrameForeground(wptr));
		  gc = EZ_WRITABLEGC;
		}
	      else  gc = EZ_LABEL_TEXTGC;

	      y = ty0 + EZ_FrameFontAscent(wptr) + yoff + dhy;
              x += tx0;
	      EZ_RenderLabelText(pixmap, gc, x, y, (int)EZ_FrameTHeight(wptr),
				 EZ_FrameLabel(wptr),
				 (int)EZ_FrameLabelLength(wptr), 
				 (int)EZ_FrameLineLength(wptr),
				 (int)EZ_FrameNLines(wptr),
				 (int)EZ_FrameTWidth(wptr),
				 (int)EZ_WidgetJustification(wptr),
				 EZ_FrameFont(wptr), shadow);
	    }
	}
    }
  
  /*============================================================
   * 
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
    {
      int tmp, jmp;
      switch(draghandle)
	{
	case EZ_LEFT:
	  y = bwpad +1;
	  x = bwpad +1;
	  tmp = h - (y+y);
	  if( EZ_GetNumberOfActiveChildren(wptr) == 0)
	    { jmp = w -(x+x); if(jmp > 16) jmp = 16; }
	  else jmp = 6;
	  EZ_OverlayDragablePixmap(wptr, pixmap, x, y, jmp, tmp);
	  break;
	case EZ_RIGHT:
	  y = bwpad;
	  tmp = h - (y+y);
	  if( EZ_GetNumberOfActiveChildren(wptr) == 0)
	    {
	      x = bwpad + 1;
	      jmp = w -(x+x);
	      if(jmp > 16) jmp = 16;
	      x = w - jmp - x;
	    }
	  else
	    {
	      x = w -(bwpad + 8);
	      jmp = 6;
	    }
	  EZ_OverlayDragablePixmap(wptr, pixmap, x, y, jmp, tmp);
	  break;
	case EZ_TOP:
	  y = bwpad + 1;
	  x = bwpad + 1;
	  tmp = w - (x+x);
	  if( EZ_GetNumberOfActiveChildren(wptr) == 0)
	    { jmp = h-(y+y); if(jmp > 16) jmp = 16;}
	  else jmp = 6;
	  EZ_OverlayDragablePixmap(wptr, pixmap, x, y, tmp, jmp);
	  break;
	default:
	  break;
	}
    }
  /*============================================================*/  
  EZ_DrawRectBorder(wptr, pixmap); 
  XCopyArea(EZ_Display,pixmap,win, EZ_WRITABLEGC,0,0,w,h,0,0); 
  XFreePixmap(EZ_Display, pixmap); 
}

/*******************************************************************
 *
 *  Event handle
 */
void EZ_FrameEventHandle(widget, event)
     EZ_Widget *widget;
     XEvent *event;
{
  if(widget)
    {
      if(event->type == Expose)	EZ_DrawWidget(widget); 
      else if(event->type == ButtonPress)
	{
	  if(event->xbutton.button == EZ_Btn1)
	    {
	      if(EZ_FrameDetachable(widget))
		{
		  int ix = event->xbutton.x;
		  int iy = event->xbutton.y;
		  int w = EZ_WidgetWidth(widget);
		  int h = EZ_WidgetHeight(widget);
		  int  onhandle, corner = 0;
		  onhandle = EZ_OnFrameDragHandle(widget, ix, iy);
		  if(EZ_GetWidgetDetachedFlag(widget))
		    corner =  EZ_XYIsAtCorner(ix,iy,w,h);
		  
		  if(corner)
		    {
		      int x0 = EZ_WidgetOriginX(widget);
		      int y0 = EZ_WidgetOriginY(widget);
		      int x1,y1,x2, y2;
                      switch(corner)
                        {
                        case XC_top_left_corner:
			  x1 = x0; y1 = y0; 
			  x2 = x0 +w; y2 = y0 +h;
                          break;
                        case XC_top_right_corner:
			  x1 = x0 +w; y1 = y0;
			  x2 = x0; y2 = y0 +h;
                          break;
                        case XC_bottom_right_corner:
			  x2 = x0; y2 = y0; 
			  x1 = x0 +w; y1 = y0 +h;
                          break;
                        case XC_bottom_left_corner:
			  x2 = x0 +w; y2 = y0;
			  x1 = x0; y1 = y0 +h;			  
                          break;
                        default:
                          { x1=x2=y1=y2 = 0;}
                          break;
                        }
		      {
			int     x,y, dx,dy, rootx, rooty, oldx, oldy;
			int     dflag = 0;
			int     rx, ry,rw,rh;
			XEvent  xevent;
			Window  root, child, win;
			unsigned int    mask;
			GC       gc = EZ_XORGC;

			win = RootWindow(EZ_Display, EZ_ScreenNum);
			XQueryPointer(EZ_Display, RootWindow(EZ_Display, EZ_ScreenNum),
				      &root,       /* root return win */
				      &child,      /* child ret win   */
				      &rootx, &rooty,   /* x, y in root    */
				      &x,&y,            /* x, y in win     */
				      &mask);  
			oldx = rootx; oldy = rooty;
			rw = x1 - x2; if(rw<0) rw = -rw;
			rh = y1 - y2; if(rh<0) rh = -rh;
			rx = x2 < x1 ? x2: x1;
			ry = y2 < y1 ? y2: y1;
			while(1)
			  {
			    XNextEvent(EZ_Display, &xevent); 
			    EZ_FilterEvent(&xevent);
                            EZ_InvokePrivateEventHandler(&xevent);
			    if(xevent.type == Expose) EZ_WidgetDispatchEvent(&xevent);
			    if(xevent.type == MotionNotify)
			      {
				if(dflag) 
				  {
				    XDrawRectangle(EZ_Display, win, gc, rx,ry,rw, rh);
				    dflag = 0;
				  }
				while (XCheckTypedEvent(EZ_Display, MotionNotify, &xevent))
				  EZ_FilterEvent(&xevent);
				XQueryPointer(EZ_Display, RootWindow(EZ_Display, EZ_ScreenNum),
					      &root,       /* root return win */
					      &child,      /* child ret win   */
					      &rootx, &rooty,   /* x, y in root    */
					      &x,&y,            /* x, y in win     */
					      &mask);  
				dx = rootx - oldx;  dy = rooty - oldy;
				oldx = rootx; oldy = rooty;

				x1 += dx; y1 += dy;
				rw = x1 - x2; if(rw<0) rw = -rw;
				rh = y1 - y2; if(rh<0) rh = -rh;
				rx = x2 < x1 ? x2: x1;
				ry = y2 < y1 ? y2: y1;
				XDrawRectangle(EZ_Display, win, gc, rx,ry,rw, rh);
				dflag = 1;
			      }
			    else if(xevent.type == ButtonRelease)
			      {
				if(dflag) XDrawRectangle(EZ_Display, win, gc, rx,ry,rw, rh);
				EZ_WidgetOriginX(widget) = rx;
				EZ_WidgetOriginY(widget) = ry;
				rw = (rw > 0 ? rw : 10);
				rh = (rh > 0 ? rh : 10);
				EZ_WidgetWidth(widget) = rw;
				EZ_WidgetHeight(widget) = rh;
				EZ_ClearWidgetSizeComputedFlag(widget);      
				EZ_ReDisplayWidget(widget);
                                XRaiseWindow(EZ_Display,EZ_WidgetWindow(widget));
				break;
			      }
			  }
		      }
		      return; /* done resizing, return */
		    }  /* already detached */
		  /* if detached, move window if ptr is on handle 
		   * otherwise, detach and move window 
		   */
		  if(onhandle)
		    {
		      int     x,y, dx,dy, rootx, rooty, oldx, oldy;
		      int     done, moved, detached, grabed;
		      XEvent  xevent;
		      Window  root, child;
		      unsigned int    mask;

		      done = 0;
		      moved = 0;
		      grabed = 0;
                      XDefineCursor(EZ_Display, EZ_WidgetWindow(widget),
                                    EZ_GetCursor(EZ_C_HAND2));
		      XQueryPointer(EZ_Display, RootWindow(EZ_Display, EZ_ScreenNum),
				    &root,       /* root return win */
				    &child,      /* child ret win   */
				    &oldx, &oldy,   /* x, y in root    */
				    &x,&y,            /* x, y in win     */
				    &mask);  
		      detached = EZ_GetWidgetDetachedFlag(widget);

		      while(!done)
			{
			  XNextEvent(EZ_Display, &xevent);  EZ_FilterEvent(&xevent);
                          EZ_InvokePrivateEventHandler(&xevent);
			  if(xevent.type == Expose || xevent.type == ConfigureNotify)
                            EZ_WidgetDispatchEvent(&xevent);

			  if(xevent.type == MotionNotify)
			    {
			      if(detached == 0) 
				{
				  EZ_DetachFrame(widget,0,0);
				  XSync(EZ_Display, False);
				  XGrabPointer(EZ_Display, EZ_WidgetWindow(widget), True, 
					       ButtonReleaseMask|ButtonPressMask|PointerMotionMask,
					       GrabModeAsync, GrabModeAsync, None,
                                               EZ_GetCursor(EZ_C_HAND2), CurrentTime);
				  grabed = 1;
				  detached = 1;
				}
			      XQueryPointer(EZ_Display, RootWindow(EZ_Display, EZ_ScreenNum),
					    &root,       /* root return win */
					    &child,      /* child ret win   */
					    &rootx, &rooty,   /* x, y in root    */
					    &x,&y,            /* x, y in win     */
					    &mask);  
			      dx = rootx - oldx;  dy = rooty - oldy;
			      if(dx|dy)
				{
				  EZ_WidgetOriginX(widget) += dx;
				  EZ_WidgetOriginY(widget) += dy;
				  EZ_WidgetMaped(widget) = 3; 
                                  EZ_ReDisplayWidget(widget);
				  oldx = rootx;   oldy = rooty;
				}
			      while(XCheckTypedEvent(EZ_Display, MotionNotify, &xevent))
				EZ_FilterEvent(&xevent);
			    }
			  else if(xevent.type == ButtonRelease)
			    {
			      if(grabed) {XUngrabPointer(EZ_Display, CurrentTime);}
			      done = 1;
			    }
			}
		      if(detached) EZ_DockFrame(widget, 0);
                      XDefineCursor(EZ_Display, EZ_WidgetWindow(widget),
                                    EZ_WidgetCursor(widget));                           
		      return;
		    }     
		}
	    }
	  else if(event->xbutton.button == EZ_Btn3)
	    {
              /*
                #include "EZ_DnDHandler1.c"
                EZ_DnDEventHandler1(widget, event);
                */
              EZ_HandleDnDEvents(widget, event);
	    }
	  else if(event->xbutton.button == EZ_Btn2)      
	    {
	      if(EZ_GetWidgetDetachedFlag(widget))
		{
		  int ix = event->xbutton.x;
		  int iy = event->xbutton.y;
		  int w = EZ_WidgetWidth(widget);
		  int h = EZ_WidgetHeight(widget);
                  int corner =  EZ_XYIsAtCorner(ix,iy,w,h);
		  if(corner)
		    {
                      int    x,y, dx,dy, rootx, rooty, oldx, oldy;
                      XEvent  xevent;
                      Window  root, child, win;
                      unsigned int    mask;

                      win = RootWindow(EZ_Display, EZ_ScreenNum);
                      XQueryPointer(EZ_Display, RootWindow(EZ_Display, EZ_ScreenNum),
                                    &root,       /* root return win */
                                    &child,      /* child ret win   */
                                    &rootx, &rooty,   /* x, y in root    */
                                    &x,&y,            /* x, y in win     */
                                    &mask);  
                      oldx = rootx; oldy = rooty;
                      while(1)
                        {
                          XNextEvent(EZ_Display, &xevent); 
                          EZ_FilterEvent(&xevent);
                          EZ_InvokePrivateEventHandler(&xevent);
                          if(xevent.type == Expose) EZ_WidgetDispatchEvent(&xevent);
                          if(xevent.type == MotionNotify)
                            {
                              XQueryPointer(EZ_Display, RootWindow(EZ_Display, EZ_ScreenNum),
                                            &root,       /* root return win */
                                            &child,      /* child ret win   */
                                            &rootx, &rooty,   /* x, y in root    */
                                            &x,&y,            /* x, y in win     */
                                            &mask);  
                              dx = rootx - oldx;  dy = rooty - oldy;
                              if(dx|dy)
                                {
				  EZ_WidgetOriginX(widget) += dx;
				  EZ_WidgetOriginY(widget) += dy;
				  EZ_WidgetMaped(widget) = 3; 
				  EZ_ReDisplayWidget(widget);
				  oldx = rootx;   oldy = rooty;
                                }
                              while (XCheckTypedEvent(EZ_Display, MotionNotify, &xevent))
                                EZ_FilterEvent(&xevent);
                            }
			    else if(xevent.type == ButtonRelease)
                              {
                                XRaiseWindow(EZ_Display,EZ_WidgetWindow(widget));
                                break;
                              }
                        }
		    }
                }
	    } 
	}
      else if(event->type == MotionNotify)
	{
	  int ix = event->xmotion.x;
	  int iy = event->xmotion.y;
	  if(EZ_FrameDragHandle(widget))
	    {
	      int onoff =  EZ_OnFrameDragHandle(widget, ix, iy);
	      EZ_HighlightFrameDragHandle(widget, onoff);

	      if(EZ_GetWidgetDetachedFlag(widget))
		{
		  int w = EZ_WidgetWidth(widget);
		  int h = EZ_WidgetHeight(widget);
                  int corner = EZ_XYIsAtCorner(ix,iy,w,h);
                  if(corner && event->xmotion.state == 0)
		    {
		      if(EZ_FrameCursorChanged(widget) == 0)
			{
			  EZ_FrameCursorChanged(widget) = 1;
			  XDefineCursor(EZ_Display, EZ_WidgetWindow(widget),
					EZ_GetCursor(/*EZ_C_HAND2*/corner));
			}
		    }
		  else if(EZ_FrameCursorChanged(widget))
		    {
		      EZ_FrameCursorChanged(widget) = 0;
		      XDefineCursor(EZ_Display, EZ_WidgetWindow(widget),
				    EZ_WidgetCursor(widget));
		    }
		}
	    }
	}
      else if(event->type == LeaveNotify)
	{
	  if(EZ_FrameDragHandle(widget))
	    EZ_HighlightFrameDragHandle(widget, 0);
	  if(EZ_FrameCursorChanged(widget))
	    {
	      EZ_FrameCursorChanged(widget) = 0;
	      XDefineCursor(EZ_Display, EZ_WidgetWindow(widget),
			    EZ_WidgetCursor(widget));
	    }
	}
    }
}

/*******************************************************************/
int EZ_OnFrameDragHandle(widget, ix, iy)
     EZ_Widget *widget; int ix, iy;
{
  if(widget)
    {
      int wtype = EZ_WidgetType(widget);
      if(wtype== EZ_WIDGET_FRAME || wtype == EZ_WIDGET_MENU_BAR)
	{
	  int t = EZ_FrameDragHandle(widget);
	  if(t)
	    {
	      int             w, h, x,y, padb,bwpad;
	      int             padx, pady, tmp, jmp, nc;
	      w   = EZ_WidgetWidth(widget);
	      h   = EZ_WidgetHeight(widget);
	      padb = EZ_WidgetPadB(widget);
	      bwpad = EZ_WidgetBorderWidth(widget) + padb;
	      padx = EZ_WidgetPadX(widget);
	      pady = EZ_WidgetPadY(widget);
	      nc = EZ_GetNumberOfActiveChildren(widget);

	      switch(t)
		{
		case EZ_LEFT:
		  y = bwpad + 1;
		  x = bwpad + 1;
		  tmp = h - (y+y);
		  if(nc == 0) {jmp = w -(x+x);  if(jmp > 16) jmp = 16;}
		  else jmp = 6;
		  if(ix > x && iy > y && ix < x + jmp && iy < y + tmp) return(1);
		  break;
		case EZ_RIGHT:
		  y = bwpad + 1;
		  tmp = h - (y+y);
		  if(nc == 0)
		    {
		      x = bwpad + 1;
		      jmp = w -(x+x);
		      if(jmp > 16) jmp = 16;
		      x = w - jmp - x;		      
		    }
		  else
		    {
		      x = w -(bwpad + 8);
		      jmp = 6;
		    }
		  if(ix > x && iy > y && ix < x + jmp && iy < y + tmp) return(1);
		  break;
		case EZ_TOP:
		  y = bwpad + 1;
		  x = bwpad + 1;
		  tmp = w - (x+x);
		  if(nc == 0) { jmp = h-(y+y); if(jmp > 16) jmp = 16; } else jmp = 6;
		  if(ix > x && iy > y && ix < x + tmp && iy < y + jmp) return(1);
		  break;
		default:
		  break;
		}
	    }
	}
    }
  return(0);
}
/******************************************************************************/
void EZ_HighlightFrameDragHandle(widget, onoff) EZ_Widget *widget; int onoff;
{
  if(widget)
    {
      int wtype = EZ_WidgetType(widget);
      if(wtype == EZ_WIDGET_FRAME || wtype == EZ_WIDGET_MENU_BAR)
	{
	  if(EZ_FrameDragTmp(widget)  != onoff)
	    {
	      int t = EZ_FrameDragHandle(widget);
	      int w, h, x,y, padb,bwpad;
	      int padx, pady, tmp, tile, jmp, nc;
	      GC gc;
	      unsigned long bgpv;
	      Window win = EZ_WidgetWindow(widget);

	      w   = EZ_WidgetWidth(widget);
	      h   = EZ_WidgetHeight(widget);
	      padb = EZ_WidgetPadB(widget);
	      bwpad = EZ_WidgetBorderWidth(widget) + padb;
	      padx = EZ_WidgetPadX(widget);
	      pady = EZ_WidgetPadY(widget);
	      nc = EZ_GetNumberOfActiveChildren(widget);

              if(onoff) { EZ_InstallHighlightColor(widget); } /*4/29/99*/
	      tile = EZ_GetBackgroundGC(widget, &gc,&bgpv,0,onoff);  
	      EZ_FrameDragTmp(widget) = onoff;
              { EZ_UninstallHighlightColor(widget); } /*4/29/99*/
	      if(tile)
		{
		  int ox, oy;
		  EZ_GetWidgetTileOrigin(widget, &ox, &oy);      
		  XSetTSOrigin(EZ_Display, gc, ox, oy);
		}
	      switch(t)
		{
		case EZ_LEFT:
		  y = bwpad + 1;
		  x = bwpad + 1;
		  tmp = h - (y+y);
		  if(nc == 0) { jmp = w -(x+x); if(jmp > 16) jmp=16;}  else jmp = 6;
		  XFillRectangle(EZ_Display, win, gc, x,y,jmp,tmp);
		  EZ_OverlayDragablePixmap(widget, win, x, y, jmp, tmp);
		  break;
		case EZ_RIGHT:
		  y = bwpad + 1;
		  tmp = h - (y+y);
		  if(nc == 0) 
		    { x = bwpad + 1; jmp = w -(x+x); if(jmp > 16) jmp=16; x = w -jmp -x; }
		  else { x = w -(bwpad + 8);  jmp = 6; }
		  XFillRectangle(EZ_Display, win, gc, x,y,jmp,tmp);
		  EZ_OverlayDragablePixmap(widget, win, x, y, jmp, tmp);
		  break;
		case EZ_TOP:
		  y = bwpad + 1;
		  x = bwpad + 1;
		  tmp = w - (x+x);
		  if(nc == 0) { jmp = h-(y+y); if(jmp>16) jmp =16;} else jmp = 6;
		  XFillRectangle(EZ_Display, win, gc, x,y,tmp,jmp);
		  EZ_OverlayDragablePixmap(widget, win, x, y, tmp, jmp);
		  break;
		default:
		  break;
		}
	    }
	}
    }
}
/************************************************************************/
int EZ_DetachFrame(widget, xoff, yoff) 
     EZ_Widget *widget; int xoff, yoff;
{
  EZ_Widget *parent;
  if(widget)
    {
      int wtype = EZ_WidgetType(widget);
      if((wtype == EZ_WIDGET_FRAME || wtype == EZ_WIDGET_MENU_BAR) &&
	 (parent = EZ_WidgetParent(widget)) && EZ_WidgetMapped(widget) &&
	 EZ_GetWidgetDetachedFlag(widget) == 0)
	{
	  Window  root, junkwin, win = EZ_WidgetWindow(widget);
	  int     x, y, bd, bdw;

 	  EZ_FrameSaveX(widget) = EZ_WidgetOriginX(widget);
	  EZ_FrameSaveY(widget) = EZ_WidgetOriginY(widget);

	  root = RootWindow(EZ_Display, EZ_ScreenNum);
	  XTranslateCoordinates(EZ_Display, win, root,
				0,0, &x, &y, &junkwin); 	  
         
	  XReparentWindow(EZ_Display,win, root,  x, y);
	  XDefineCursor(EZ_Display, win, EZ_WidgetCursor(widget));
      
	  EZ_WidgetOriginX(widget) = x + xoff;
	  EZ_WidgetOriginY(widget) = y + yoff;
	  EZ_SetWidgetDetachedFlag(widget);
	  EZ_ClearWidgetSizeComputedFlag(parent); 
	  EZ_ReDisplayWidget(parent);
          /*EZ_ResetWidgetShape(parent, 0);*/
	  bd  = EZ_WidgetBorderStyle(widget);
	  bdw = EZ_WidgetBorderWidth(widget);
	  EZ_FrameSavedBdType(widget) = bd;
	  EZ_FrameSavedBdWidth(widget) = bdw;
	  if(bd == EZ_BORDER_NONE || bdw == 0)
	    {
	      EZ_WidgetBorderStyle(widget) = EZ_BORDER_UP;
	      EZ_WidgetBorderWidth(widget) = 2;
	      EZ_ClearWidgetSizeComputedFlag(widget); 
	      EZ_ReDisplayWidget(widget);
	    }
	  return(1);
	}
    }
  return(0);
}

int EZ_DockFrame(widget, force) 
     EZ_Widget *widget;  int force;
{
  EZ_Widget *parent;
  if(widget)
    {
      int wtype = EZ_WidgetType(widget);
      if((wtype == EZ_WIDGET_FRAME || wtype == EZ_WIDGET_MENU_BAR) &&
	 (parent = EZ_WidgetParent(widget)) && EZ_WidgetMapped(parent) &&
	 EZ_WidgetMapped(widget) && EZ_GetWidgetDetachedFlag(widget) != 0)
	{
	  Window  root, junkwin, win = EZ_WidgetWindow(widget);
	  int  rx, ry, sx, sy, dx, dy, w, h;

	  root = EZ_WidgetWindow(parent);
	  sx = EZ_FrameSaveX(widget);
	  sy = EZ_FrameSaveY(widget);
	  if(force == 0)
	    {
	      XTranslateCoordinates(EZ_Display, win, root,
				    0,0, &rx, &ry, &junkwin); 	  
      
	      w = EZ_WidgetWidth(parent);
	      h = EZ_WidgetHeight(parent);
	      if(rx > 0 && ry > 0 && rx < w && ry < h)
		{
		  dx = sx -rx;
		  dy = sy -ry;
		  dx = (dx >= 0 ? dx : -dx);
		  dy = (dy >= 0 ? dy : -dy);
		  force = (dx < 50 && dy < 50);
		}
	    }
	  if(force)
	    {
	      int bd, bdw;
	      XReparentWindow(EZ_Display,win, root,  sx, sy);
	      EZ_WidgetOriginX(widget) = sx;
	      EZ_WidgetOriginY(widget) = sy;
	      EZ_ClearWidgetDetachedFlag(widget);
	      EZ_ClearWidgetSizeComputedFlag(parent);      
	      EZ_ReDisplayWidget(parent);
              /*EZ_ResetWidgetShape(parent, 0);*/
	      bd = EZ_FrameSavedBdType(widget);
	      bdw = EZ_FrameSavedBdWidth(widget);
	      if(bd != EZ_WidgetBorderStyle(widget) || 
		 EZ_WidgetBorderWidth(widget) != bdw)
		{
		  EZ_WidgetBorderStyle(widget) = bd;
		  EZ_WidgetBorderWidth(widget) = bdw;
		  EZ_ClearWidgetSizeComputedFlag(widget); 
		  EZ_ReDisplayWidget(widget);
		}
	      return(1);
	    }
	}
    }
  return(0);
}


/************************************************************************/
int EZ_XYIsAtCorner(ix,iy,w,h)
     int ix,iy,w,h;
{
  if(ix > 0 && iy > 0 && ix < w && iy < h)
    {
      if(ix < 8 && iy < 8) /*return(1);*/ return(XC_top_left_corner);
      else if(ix > w - 8 && iy < 8) /* return(2);*/ return(XC_top_right_corner);
      else if(ix > w-8 && iy > h - 8) /* return(3);*/ return(XC_bottom_right_corner);
      else if(ix < 8 && iy > h -8) /*return(4);*/ return(XC_bottom_left_corner);
    }
  return(0);
}
/************************************************************************/
int EZ_FrameIsDetached(widget) EZ_Widget *widget;
{
  if(widget && EZ_GetWidgetDetachedFlag(widget)) return(1);
  return(0);
}
/************************************************************************/
void EZ_ResetWidgetShape(widget, sib) EZ_Widget *widget; int sib;
{
  if(widget)
    {
      if(EZ_GetWidgetShapedWinFlag(widget))
        {
          EZ_SetWindowShape(widget, 1);
          EZ_SetWindowShape(widget, 0);
        }
      {
        EZ_Widget *children = EZ_WidgetChildren(widget);
        EZ_ResetWidgetShape(children, 1);
      }
      if(sib)
        {
          EZ_Widget *sibling = EZ_WidgetSibling(widget);
          EZ_ResetWidgetShape(sibling, 1);
        }
    }
}
/************************************************************************/
#undef _EZ_WIDGET_FRAME_C_
