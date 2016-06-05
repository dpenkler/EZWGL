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
 ***       Embeder Widget                                      ***
 ***                                                           ***
 *****************************************************************/
#define _EZ_WIDGET_EMBEDER_C_

#include "EZ_Widget.h"
#include "EZ_WidgetConfig.h"

/*********************************************************************
 * 
 *  Functions implemented in this file:
 */
int             EZ_MakeSnapShot MY_ANSIARGS((EZ_Widget *widget,int type, int x, int y, int w, int h));
void            EZ_MoveSnapShotOrOutline MY_ANSIARGS((int which, int x, int y, int w, int h, int flag));
void            EZ_SetExecutorCommand MY_ANSIARGS((EZ_Widget *widget, char *cmd, char *args));

EZ_Widget       *EZ_CreateEmbeder MY_ANSIARGS((EZ_Widget *parent));
EZ_Widget       *EZ_CreateExecutor MY_ANSIARGS((EZ_Widget *parent));
void            EZ_EmbederEventHandle MY_ANSIARGS((EZ_Widget *widget, XEvent *event));
void            EZ_DrawEmbeder MY_ANSIARGS((EZ_Widget *widget));
void            EZ_ComputeEmbederSize MY_ANSIARGS((EZ_Widget *widget, int *w, int *h));
void            EZ_FreeEmbederData MY_ANSIARGS((EZ_Widget *widget));

void            EZ_ComputeExecutorSize MY_ANSIARGS((EZ_Widget *widget, int *w, int *h));
void            EZ_DrawExecutor MY_ANSIARGS((EZ_Widget *widget));
void            EZ_FreeExecutorData MY_ANSIARGS((EZ_Widget *widget));

void            EZ_IntersectRectangles MY_ANSIARGS((int px, int py, int pw, int ph,
						    int x, int y, int w, int h,
						    int *xr, int *yr, int *wr, int *hr));
/*********************************************************************
 * 
 *  Local Variables.
 */
static EZ_WidgetHandle EZ_EmbederHandle =
{ 
  EZ_ComputeEmbederSize, 
  EZ_DrawEmbeder,
  EZ_FreeUnknownData,
  EZ_EmbederEventHandle, 
};

static EZ_WidgetHandle EZ_ExecutorHandle =
{ 
  EZ_ComputeExecutorSize, 
  EZ_DrawExecutor,
  EZ_FreeExecutorData,
  EZ_EmbederEventHandle, 
};

/*********************************************************************/
EZ_Widget  *EZ_CreateExecutor(parent)
     EZ_Widget *parent;
{
  EZ_Widget  *tmp;
  tmp = EZ_CreateNewWidget(parent);
  EZ_WidgetHandlingFunctions[EZ_WIDGET_EXECUTOR] = &EZ_ExecutorHandle;  

  EZ_SetWidgetTypeAndNames(tmp,  EZ_WIDGET_EXECUTOR);

  EZ_WidgetBorderStyle(tmp)  = EZ_BORDER_NONE;
  EZ_WidgetBorderWidth(tmp)  = 0;
  EZ_WidgetPadX(tmp)         = 2;
  EZ_WidgetPadY(tmp)         = 2;
  EZ_WidgetCursor(tmp) = EZ_GetCursor(EZ_C_HAND2);

  EZ_EmbederConfirmed(tmp)   = -1;
  EZ_EmbederCommuWin(tmp)    = (Window )NULL;
  EZ_EmbederWin(tmp)         = (Window )NULL;
  EZ_EmbederWidgetAddr(tmp)  = 0;

  EZ_EmbederWidth(tmp)       = 1;
  EZ_EmbederHeight(tmp)      = 1;
  EZ_EmbederId(tmp)         = 0;
  EZ_ExecutorPreviousW(tmp)  = 0;
  EZ_ExecutorPreviousH(tmp)  = 0;
  EZ_ExecutorMayBeDirty(tmp) = 0;
  EZ_ExecutorMinW(tmp)       = 0;
  EZ_ExecutorMinH(tmp)       = 0;
  EZ_ExecutorCommand(tmp)    = (char *) NULL;
  EZ_ExecutorCommandArgs(tmp)= (char *) NULL;
  EZ_ExecutorDepth(tmp)      = 0;
  EZ_SetWidgetFocusableFlag(tmp);  

  return(tmp);
}
/*********************************************************************/

EZ_Widget  *EZ_CreateEmbeder(parent)
     EZ_Widget *parent;
{
  EZ_Widget  *tmp;
  
  tmp = EZ_CreateNewWidget(parent);
  EZ_WidgetHandlingFunctions[EZ_WIDGET_EMBEDER] = &EZ_EmbederHandle;
  
  EZ_SetWidgetTypeAndNames(tmp,  EZ_WIDGET_EMBEDER);

  EZ_SetWidgetXSetFlag(tmp);
  EZ_SetWidgetYSetFlag(tmp);
  EZ_WidgetBorderStyle(tmp)  = EZ_BORDER_FRAMED_UP;
  EZ_WidgetBorderWidth(tmp)  = 2;
  EZ_WidgetPadX(tmp)         = 0;
  EZ_WidgetPadY(tmp)         = 0;
  EZ_WidgetCursor(tmp) = EZ_GetCursor(EZ_C_HAND2);

  EZ_EmbederConfirmed(tmp)   = -1;
  EZ_EmbederCommuWin(tmp)    = (Window )NULL;
  EZ_EmbederWin(tmp)         = (Window )NULL;
  EZ_EmbederWidgetAddr(tmp)  = 0;

  EZ_EmbederWidth(tmp)       = 1;
  EZ_EmbederHeight(tmp)      = 1;
  EZ_EmbederId(tmp)         = 0;

  EZ_SetDoNotPropagateFlag(tmp);
  EZ_SetWidgetFocusableFlag(tmp);  
 
  return(tmp);
}

/****************************************************************************/

void  EZ_ComputeEmbederSize(widget, w,h)
     EZ_Widget *widget;
     int       *w, *h;
{
  int width = EZ_EmbederWidth(widget);
  int height = EZ_EmbederHeight(widget);
  int bw = EZ_WidgetBorderWidth(widget);
  int cw = bw + EZ_WidgetPadX(widget);
  int ch = bw + EZ_WidgetPadY(widget);
      
  *w = cw + cw + width;
  *h = ch + ch + height;

  { EZ_SetWidgetSizeComputedFlag(widget); }
}

/****************************************************************************/

void  EZ_ComputeExecutorSize(widget, w,h)
     EZ_Widget *widget;
     int       *w, *h;
{
  int width = EZ_ExecutorMinW(widget);  
  int height = EZ_ExecutorMinH(widget);
  int bw = EZ_WidgetBorderWidth(widget);
  int cw = bw + EZ_WidgetPadX(widget);
  int ch = bw + EZ_WidgetPadY(widget);
      
  *w = cw + cw + width;
  *h = ch + ch + height;
      
  EZ_ExecutorPreviousW(widget)  = *w;  
  EZ_ExecutorPreviousH(widget)  = *h;

  { EZ_SetWidgetSizeComputedFlag(widget);}
}
/****************************************************************************/
void EZ_FreeExecutorData(widget)
     EZ_Widget *widget;
{
  if(widget)
    {
      if(EZ_ExecutorCommand(widget))
	(void)my_free( (char *) EZ_ExecutorCommand(widget));
      if(EZ_ExecutorCommandArgs(widget))
	(void)my_free( (char *) EZ_ExecutorCommandArgs(widget));
    }
}
/****************************************************************************/
void  EZ_DrawEmbeder(wptr)
     EZ_Widget *wptr;
{
  int             w, h;
  Pixmap          pixmap;
  Window          win;
  GC              gc;
  unsigned long   bgpv;

  win = EZ_WidgetWindow(wptr);
  w   = EZ_WidgetWidth(wptr);
  h   = EZ_WidgetHeight(wptr);

  pixmap = XCreatePixmap(EZ_Display, win, w, h, EZ_Depth); 
  if(EZ_GetBackgroundGC(wptr, &gc,&bgpv, 0, 0))
    {
      int ox, oy;
      EZ_GetWidgetTileOrigin(wptr, &ox, &oy);      
      XSetTSOrigin(EZ_Display, gc, ox, oy);
    }
  XFillRectangle(EZ_Display, pixmap, gc, 0,0, w, h); 

  EZ_DrawRectBorder(wptr, pixmap); 
  XCopyArea(EZ_Display,pixmap,win, EZ_WRITABLEGC,0,0,w,h,0,0); 
  XFreePixmap(EZ_Display, pixmap);   
}
/****************************************************************************/
void  EZ_DrawExecutor(wptr)
     EZ_Widget *wptr;
{
  int             w, h;
  Pixmap          pixmap;
  Window          win;
  GC              gc;
  unsigned long   bgpv;

  win = EZ_WidgetWindow(wptr);
  w   = EZ_WidgetWidth(wptr);
  h   = EZ_WidgetHeight(wptr);

  pixmap = XCreatePixmap(EZ_Display, win, w, h, EZ_Depth); 
  if(EZ_GetBackgroundGC(wptr, &gc,&bgpv, 0, 0))
    {
      int ox, oy;
      EZ_GetWidgetTileOrigin(wptr, &ox, &oy);      
      XSetTSOrigin(EZ_Display, gc, ox, oy);
    }
  XFillRectangle(EZ_Display, pixmap, gc, 0,0, w, h); 
  
  EZ_DrawRectBorder(wptr, pixmap); 
  XCopyArea(EZ_Display,pixmap,win, EZ_WRITABLEGC,0,0,w,h,0,0); 
  XFreePixmap(EZ_Display, pixmap);   

  if(EZ_EmbederConfirmed(wptr) < 0) /* need to execute application */ 
    {
      /* guessing something about the client. start the client and
       * after getting a confirmation back, setup the geometry of
       * the client 
       */
      EZ_EmbederConfirmed(wptr) = 0;
      if(EZ_ExecutorCommand(wptr))
	{
	  char cmd[1024], opt[256], opta[32];
	  int xx, yy;

	  /* hack: verify the embeding depth. Avoiding embeding loops */
	  if(EZ_EmbedingDepth >= EZ_EMBEDING_DEPTH)
	    {
	      fprintf(stderr, "Executor: Embeding depth limit reached: %d\n",
		      EZ_EMBEDING_DEPTH+1);
	      return;
	    }
	  xx = EZ_WidgetPadX(wptr) + EZ_WidgetBorderWidth(wptr);
	  yy = EZ_WidgetPadY(wptr) + EZ_WidgetBorderWidth(wptr);
	  (void) strcpy(cmd, EZ_ExecutorCommand(wptr));

	  { /* try to run the application with the same background */
	    unsigned long bgpv;
	    int r,g,b;

	    if(EZ_WidgetBackground(wptr) != 0) bgpv = EZ_WidgetBackground(wptr);
	    else if(EZ_WidgetParentBG(wptr) && *(EZ_WidgetParentBG(wptr)))
	      bgpv = *(EZ_WidgetParentBG(wptr));
	    else bgpv = EZ_ColorArray[EZ_NORMAL_GRAY1];
	    EZ_PixelValue2RGB(bgpv, &r,&g,&b);
	    sprintf(opta, " -bg \"#%02x%02x%02x\" ", r,g,b);
	  }

	  EZ_ExecutorDepth(wptr) = EZ_EmbedingDepth + 1;
	  if(EZ_GetWidgetSizeSetFlag(wptr)) /* executor has a fixed size */
	    {
	      int ww = w - (xx + xx);
	      int hh = h - (yy + yy);
	      
	      EZ_EmbederWidth(wptr) = ww;  /* this will be the estimated dimension of the embeding */
	      EZ_EmbederHeight(wptr) = hh;	      
	      (void) sprintf(opt, " -EmBeD %lx_%lx -geom %dx%d+%d+%d -EDepTh %d &", 
			     EZ_DummyWindow, win, ww,hh, xx,yy, EZ_ExecutorDepth(wptr));
	    }
	  else /* just run the application, get feedback to figure out its dimension */
	    {
	      (void) sprintf(opt, " -EmBeD %lx_%lx -geom +%d+%d -EDepTh %d &", 
			     EZ_DummyWindow, win, xx, yy, EZ_ExecutorDepth(wptr));
	    }

	  /* set the command */
	  strcat(cmd,opta);  /* if a -bg is set as cmd arg, that will be used */
	  if(EZ_ExecutorCommandArgs(wptr)) (void) strcat(cmd,EZ_ExecutorCommandArgs(wptr));
	  (void) strcat(cmd, opt);
	  (void) system(cmd);
	}
    }
  else /* embeding is running */
    {
      if(EZ_ExecutorMayBeDirty(wptr) || w != EZ_ExecutorPreviousW(wptr) ||
	 h != EZ_ExecutorPreviousH(wptr))
	{
	  Window eWin = EZ_EmbederWin(wptr);
	  Window commWin = EZ_EmbederCommuWin(wptr);
	  unsigned long id; 
	  int xx,yy;
	  if(EZ_VerifyExistence(commWin) && EZ_WindowExist(eWin)) 
	    {
	      id = EZ_EmbederWidgetAddr(wptr);
	      xx = EZ_WidgetPadX(wptr) + EZ_WidgetBorderWidth(wptr);
	      yy = EZ_WidgetPadY(wptr) + EZ_WidgetBorderWidth(wptr); 
	      if(EZ_WidgetFillMode(wptr) != EZ_FILL_NONE)
		{
		  int ww = w - (xx+xx);
		  int hh = h - (yy+yy);
		  /* this will be the size of the embeding ! */
		  /* it should be > than the min size of the embeding */
		  EZ_EmbederWidth(wptr) = ww; 
		  EZ_EmbederHeight(wptr) = hh;
		  EZ_SendEmbedingMessage(EZ_EMBEDING_MOVE_RESIZE_WINDOW_C,
					 EZ_DummyWindow, win, (unsigned long)wptr, 
					 commWin, eWin, id,
					 EZ_ApplicationSetupTime,
					 xx,yy,ww,hh,0,0);
		}
	      else
		{
		  int ww = w - (xx+xx);
		  int hh = h - (yy+yy);
		  int xoff = ((ww - EZ_EmbederWidth(wptr)));
		  int yoff = ((hh - EZ_EmbederHeight(wptr)));
		  
		  if( xoff < 0 || yoff < 0 ) /* embeder is not big enough */
		    {
		      int width = xoff < 0 ? ww : EZ_EmbederWidth(wptr);
		      int height = yoff < 0 ? hh : EZ_EmbederHeight(wptr);

		      xoff = ((ww - width)>>1);
		      yoff = ((hh - height)>>1);
		      EZ_EmbederWidth(wptr) = width;    /* the new dimension of the client */
		      EZ_EmbederHeight(wptr) = height; 

		      EZ_SendEmbedingMessage(EZ_EMBEDING_MOVE_RESIZE_WINDOW_C,
					     EZ_DummyWindow, win, (unsigned long)wptr, 
					     commWin, eWin, id, 
					     EZ_ApplicationSetupTime,
					     xx+xoff,yy+yoff,width,height,0,0);
		    }
		  else
		    {
		      xoff = xoff >>1; yoff = yoff >> 1;
		      EZ_SendEmbedingMessage(EZ_EMBEDING_MOVE_RESIZE_WINDOW_C,
					     EZ_DummyWindow, win, (unsigned long)wptr, 
					     commWin, eWin, id, 
					     EZ_ApplicationSetupTime,
					     xx+xoff,yy+yoff,ww,hh,0,0);
		    }
		}
	    }
	  EZ_ExecutorPreviousW(wptr) = w; 
	  EZ_ExecutorPreviousH(wptr) = h;
	  EZ_ExecutorMayBeDirty(wptr) = 0;
	}
    }
}

/****************************************************************************/
void  EZ_EmbederEventHandle(widget, event)
     EZ_Widget *widget;
     XEvent    *event;
{
  int       x,y, dx,dy, rootx, rooty, oldx, oldy, done, buttonP, corner,bx,by;
  int       width, height, Ox, Oy, Sx, Sy, snap, moved = 0, inside = 1, isexe;
  XEvent    xevent;
  Window    root, child, pwin;
  unsigned  int    mask;
  EZ_Widget *parent;

  if(widget == (EZ_Widget *)NULL) return;
  parent = EZ_WidgetParent(widget); 
  isexe = (EZ_WidgetType(widget) == EZ_WIDGET_EXECUTOR);
  if(parent && (pwin = EZ_WidgetWindow(parent)))
    {
      switch(event->type)
	{
	case Expose:
	  EZ_DrawWidget(widget);
	  break;
        case MotionNotify:
          bx = event->xmotion.x;
          by = event->xmotion.y;
	  width = EZ_WidgetWidth(widget);
	  height = EZ_WidgetHeight(widget);
          corner = EZ_XYIsAtCorner(bx,by,width,height);
          if(corner && event->xmotion.state == 0)
            {
              EZ_EmbederCursorChanged(widget) = 1;
              XDefineCursor(EZ_Display, EZ_WidgetWindow(widget),
                            EZ_GetCursor(/*EZ_C_HAND2*/corner));
            }
          else if(EZ_EmbederCursorChanged(widget))
            {
              EZ_EmbederCursorChanged(widget) = 0;
              XDefineCursor(EZ_Display, EZ_WidgetWindow(widget),
                            EZ_WidgetCursor(widget));
            }
          break;
        case LeaveNotify:
	  if(EZ_EmbederCursorChanged(widget))
	    {
	      EZ_EmbederCursorChanged(widget) = 0;
	      XDefineCursor(EZ_Display, EZ_WidgetWindow(widget),
			    EZ_WidgetCursor(widget));
	    }
          break;
	case ButtonPress:
          bx = event->xbutton.x;
          by = event->xbutton.y;
	  width = EZ_WidgetWidth(widget);
	  height = EZ_WidgetHeight(widget);
          corner = EZ_XYIsAtCorner(bx,by,width,height);

          if(corner == 0 && isexe )  return;
          if(isexe == 0) /* embeder */
            {
              XRaiseWindow(EZ_Display, EZ_WidgetWindow(widget));
              EZ_DrawWidget(widget);           
            }
	  buttonP = event->xbutton.button;
          XQueryPointer(EZ_Display, RootWindow(EZ_Display, EZ_ScreenNum),
                        &root,                          /* root return win */
                        &child,                         /* child ret win   */
                        &oldx, &oldy,                   /* x, y in root    */
                        &x,&y,                          /* x, y in win     */
                        &mask); 
      
          Sx = Ox = oldx - bx;  /* origin of window in screen coor */
          Sy = Oy = oldy - by;
	  
          /*
           * wait for a matching ButtonRelease. In the mean time, process
           * MotionNotify and SelectionRequest events.
           */
          EZ_GrabServer(); 
          /* 
           * The purpose of grabing the server is so that the display won't be garbaged 
           * by the XORed drag window. Grabing the server is really a bad idea
           */
          if(EZ_PointerGrabed == 0)  /* grab the pointer ! */
            {
              if(XGrabPointer(EZ_Display,EZ_WidgetWindow(widget), True,
                              ButtonReleaseMask|ButtonPressMask|PointerMotionMask,
                              GrabModeAsync, GrabModeAsync, None, 
                              (corner == 0? None: EZ_GetCursor(corner)), CurrentTime)
                 == GrabSuccess) EZ_PointerGrabed = 1;
            }	
          done = 0;

          if(corner)
            {
              int x1,y1,x2, y2, rx, ry,rw,rh, dflag = 0;
              GC  gc = EZ_XORGC;
              Window win = RootWindow(EZ_Display, EZ_ScreenNum);
              switch(corner)
                {
                case XC_top_left_corner:
                  x1 = Sx; y1 = Sy;
                  x2 = x1 +width; y2 = y1 +height;
                  break;
                case XC_top_right_corner:
                  x1 = Sx +width; y1 = Sy;
                  x2 = Sx; y2 = y1 +height;
                  break;
                case XC_bottom_right_corner:
                  x2 = Sx; y2 = Sy;
                  x1 = x2 +width; y1 = y2 +height;
                  break;
                case XC_bottom_left_corner:
                  x2 = Sx +width; y2 = Sy;
                  x1 = Sx; y1 = y2 +height;    
                  break;
                default:
                  { x1=x2=y1=y2 = 0;}
                  break;
                }
              rw = x1 - x2; if(rw<0) rw = -rw;
              rh = y1 - y2; if(rh<0) rh = -rh;
              rx = x2 < x1 ? x2: x1;
              ry = y2 < y1 ? y2: y1;

              
              while(!done)
                {
                  do {
                    XNextEvent(EZ_Display, &xevent); 
                    if(xevent.type == ButtonRelease)
                      if(xevent.xbutton.button == buttonP) done = 1; 
                    if(EZ_FilterEvent(&xevent))
                      EZ_InvokePrivateEventHandler(&xevent);

                    if(done) break;
                    else if(xevent.type == Expose ||xevent.type == FocusIn ||
                            xevent.type == FocusOut)    
                      EZ_WidgetDispatchEvent(&xevent);
                  } while(XPending(EZ_Display) && !done);
	      
                  if(!done)
                    {
                      XQueryPointer(EZ_Display, win,
                                    &root,       /* root return win */
                                    &child,      /* child ret win   */
                                    &rootx, &rooty,   /* x, y in root    */
                                    &x,&y,            /* x, y in win     */
                                    &mask);  
                      dx = rootx - oldx;  dy = rooty - oldy;
                      oldx = rootx;   oldy = rooty;
		      
                      if(dx | dy )
                        {
                          if(dflag) 
                            {
                              XDrawRectangle(EZ_Display, win, gc, rx,ry,rw, rh);
                              dflag = 0;
                            }
                          x1 += dx; y1 += dy;
                          rw = x1 - x2; if(rw<0) rw = -rw;
                          rh = y1 - y2; if(rh<0) rh = -rh;
                          rx = x2 < x1 ? x2: x1;
                          ry = y2 < y1 ? y2: y1;
                          XDrawRectangle(EZ_Display, win, gc, rx,ry,rw, rh);
                          dflag = 1;  
                        }
                    }
                  else /* done */
                    {
                      int nx, ny,pw,ph;
                      if(dflag) XDrawRectangle(EZ_Display, win, gc, rx,ry,rw, rh);
                      rw = (rw > 0 ? rw : 10);
                      rh = (rh > 0 ? rh : 10);
                      if(isexe == 0)
                        {
                          nx = EZ_WidgetOriginX(widget) + rx - Sx;
                          ny = EZ_WidgetOriginY(widget) + ry - Sy;
                          pw = EZ_WidgetWidth(parent);
                          ph = EZ_WidgetHeight(parent);
                          {
                            int x,y,w,h;
                            EZ_IntersectRectangles(0,0,pw,ph,nx,ny,rw,rh, &x,&y, &w, &h);
                            if( ((w * h) << 1) <= rw * rh) inside = 0; /* > half is out */
                          }
                          if(inside == 0)
                            {
                              EZ_SendEmbedingMessage(EZ_EMBEDING_MANAGE_YOUSELF_C,
                                                     EZ_DummyWindow,EZ_WidgetWindow(widget),
                                                     (unsigned long)widget,
                                                     EZ_EmbederCommuWin(widget),
                                                     EZ_EmbederWin(widget),
                                                     (unsigned long)EZ_EmbederWidgetAddr(widget),
                                                     EZ_ApplicationSetupTime,
                                                     Ox,Oy,0,0,EZ_EmbederId(widget),0);
                              EZ_EmbederCommuWin(widget) = (Window)NULL;  
                              EZ_EmbederWin(widget) = (Window) NULL;
                              EZ_EmbederConfirmed(widget) = 0; 
                              /* Part of EZ_DestroyWidget, needs the window id */
                              EZ_RemoveWidgetFromMappedHT(widget); 
                              /* we destroy widget, but leave its window for a while
                               * when the embeded widget done with manage itself, it
                               * will destroy this window. Again, this is not a
                               * perfect soln. It lets the embeded application destroy
                               * its embeder, a window created by another application.
                               * Probably implementing a waiting mechanism would be better.
                               */
                              EZ_RemoveFromDnDWindowList(widget); /* 5-20-97 */
                              EZ_WidgetWindow(widget) = (Window)NULL;
                              EZ_DestroyWidget(widget);
                            }
                          else
                            {
                              dx = rw - EZ_WidgetWidth(widget);
                              dy = rh - EZ_WidgetHeight(widget);
                              rw = EZ_EmbederWidth(widget) + dx;
                              rh = EZ_EmbederHeight(widget) + dy;
                              if(rw < 10) rw = 10; if(rh < 10) rh = 10;
                              EZ_ClearWidgetSizeComputedFlag(widget);      
                              EZ_EmbederWidth(widget) = rw;
                              EZ_EmbederHeight(widget) = rh;  
                              EZ_WidgetOriginX(widget) = nx;
                              EZ_WidgetOriginY(widget) = ny;
                              EZ_ClearWidgetSizeComputedFlag(widget);
                              EZ_DisplayWidget(widget);

                              EZ_SendEmbedingMessage(EZ_EMBEDING_RESIZE_WINDOW_C,
                                                     EZ_DummyWindow,EZ_WidgetWindow(widget),
                                                     (unsigned long)widget,
                                                     EZ_EmbederCommuWin(widget), EZ_EmbederWin(widget),
                                                     (unsigned long)EZ_EmbederWidgetAddr(widget),
                                                     EZ_ApplicationSetupTime,
                                                     0,0,rw,rh,0,0);
                            }
                        }
                      else   /* executor */
                        {
                          dx = rw - EZ_WidgetWidth(widget);
                          dy = rh - EZ_WidgetHeight(widget);
                          dx = EZ_EmbederWidth(widget) + dx;
                          dy = EZ_EmbederHeight(widget) + dy;
                          if(dx < 10) dx = 10; if(dy < 10) dy = 10;
                          EZ_EmbederWidth(widget) = dx;
                          EZ_EmbederHeight(widget) = dy; 
                          EZ_ClearWidgetSizeComputedFlag(widget);      
                          EZ_WidgetWidth(widget) = rw;
                          EZ_WidgetWidthHint(widget) =rw;
                          EZ_WidgetHeight(widget) = rh; 
                          EZ_WidgetHeightHint(widget) = rh; 
                          EZ_SetWidgetSizeSetFlag(widget);
                          EZ_ExecutorMayBeDirty(widget) = 1;
                          EZ_DrawWidget(widget); /* refresh embeding first */
                          EZ_DisplayWidget(widget);
                        }
                    }
                }
            }
          else if(isexe == 0)
            {
              if(width * height < 40000 && EZ_MakeSnapShot(widget,0,0,0,1,1)) 
                snap = EZ_DND_DRAG_ICON_PIXMAP;
              else snap = EZ_DND_DRAG_ICON_OUTLINE;

              while(!done)
                {
                  do {
                    XNextEvent(EZ_Display, &xevent); 
                    if(xevent.type == ButtonRelease)
                      if(xevent.xbutton.button == buttonP) done = 1; 
                    if(EZ_FilterEvent(&xevent))
                      EZ_InvokePrivateEventHandler(&xevent);

                    if(done) break;
                    else if(xevent.type == Expose ||xevent.type == FocusIn ||
                            xevent.type == FocusOut)    
                      EZ_WidgetDispatchEvent(&xevent);
                  } while(XPending(EZ_Display) && !done);
	      
                  if(!done)
                    {
                      XQueryPointer(EZ_Display, RootWindow(EZ_Display, EZ_ScreenNum),
                                    &root,       /* root return win */
                                    &child,      /* child ret win   */
                                    &rootx, &rooty,   /* x, y in root    */
                                    &x,&y,            /* x, y in win     */
                                    &mask);  
                      dx = rootx - oldx;  dy = rooty - oldy;
                      oldx = rootx;   oldy = rooty;
		      
                      if(dx | dy )
                        {
                          Ox += dx; Oy += dy;
                          EZ_MoveSnapShotOrOutline(snap, Ox, Oy, width, height, 1);
                          EZ_MoveSnapShotOrOutline(snap, Ox, Oy, width, height, 0);
                          moved = 1;
                        }
                    }
                  else /* done */
                    {
                      int nx, ny,pw,ph;
                      if(moved) EZ_MoveSnapShotOrOutline(snap, 0, 0, 0, 0, 1);
                      dx = Ox - Sx;
                      dy = Oy - Sy;
                      pw = EZ_WidgetWidth(parent);
                      ph = EZ_WidgetHeight(parent);
                      nx = EZ_WidgetOriginX(widget) + dx;
                      ny = EZ_WidgetOriginY(widget) + dy;
                      {
                        int x,y,w,h;
                        EZ_IntersectRectangles(0,0,pw,ph,nx,ny,width,height, &x,&y, &w, &h);
                        if( ((w * h) << 1) <= width * height) inside = 0; /* > half is out */
                      }
                      if(inside == 0)
                        {
                          EZ_SendEmbedingMessage(EZ_EMBEDING_MANAGE_YOUSELF_C,
                                                 EZ_DummyWindow,EZ_WidgetWindow(widget),
                                                 (unsigned long)widget,
                                                 EZ_EmbederCommuWin(widget),
                                                 EZ_EmbederWin(widget),
                                                 (unsigned long)EZ_EmbederWidgetAddr(widget),
                                                 EZ_ApplicationSetupTime,
                                                 Ox,Oy,0,0,EZ_EmbederId(widget),0);
                          EZ_EmbederCommuWin(widget) = (Window)NULL;  
                          EZ_EmbederWin(widget) = (Window) NULL;
                          EZ_EmbederConfirmed(widget) = 0; 
                          /* Part of EZ_DestroyWidget, needs the window id */
                          EZ_RemoveWidgetFromMappedHT(widget); 
                          /* we destroy widget, but leave its window for a while
                           * when the embeded widget done with manage itself, it
                           * will destroy this window. Again, this is not a
                           * perfect soln. It lets the embeded application destroy
                           * its embeder, a window created by another application.
                           * Probably implementing a waiting mechanism would be better.
                           */
                          EZ_RemoveFromDnDWindowList(widget); /* 5-20-97 */
                          EZ_WidgetWindow(widget) = (Window)NULL;
                          EZ_DestroyWidget(widget);
                        }
                      else
                        {
                          EZ_WidgetOriginX(widget) += dx;
                          EZ_WidgetOriginY(widget) += dy;
                          EZ_ClearWidgetSizeComputedFlag(widget);
                          EZ_ReDisplayWidget(widget); 
                        }	  
                    }
                }
            }
          /* done */
          EZ_UngrabServer();
          if(EZ_PointerGrabed != 0)  /* grab the pointer ! */
            {
              XUngrabPointer(EZ_Display,CurrentTime);
              EZ_PointerGrabed = 0;	     
            }
	  break;
	default:
	  break;
	}
    }
}
/***************************************************************************
 *  
 *  Move the widget window, for the portion outside its parent, display
 *  an outline.
 */
/*****************************************************************************/  
void EZ_MoveSnapShotOrOutline(which, x, y, w, h, flag)
     int which;        /* if which != 0 then we have a pixmap */
     int x,y,w,h;
     int flag;         /* 0: draw a new one, 1: draw the old one */
{
  static int LastX = 0, LastY = 0, LastW = 0, LastH = 0;

  if(which == EZ_DND_DRAG_ICON_PIXMAP)
    {
      if(flag == 333)
	{

	}
      else
	{
	  if(LastW && flag)
	    {
	      int ww = EZ_SnapShotBitmap->width;
	      int hh = EZ_SnapShotBitmap->height;
	      
	      XCopyArea(EZ_Display, EZ_SnapShotBitmap->pixmap,
			RootWindow(EZ_Display,EZ_ScreenNum),
			EZ_XORGC, 0,0,ww,hh,LastX,LastY);
	    }
	  LastX = x;  LastY = y; LastW = w; LastH = h;
	  
	  if(LastW && !flag)
	    {
	      XCopyArea(EZ_Display, EZ_SnapShotBitmap->pixmap,
			RootWindow(EZ_Display,EZ_ScreenNum),
			EZ_XORGC, 0,0,w,h,LastX,LastY);
	    }
	}
    }
  else if(which == EZ_DND_DRAG_ICON_OUTLINE)
    {
      if(flag == 333)
	{
	}
      else
	{
	  if(flag && (LastW || LastH))
	    {
	      XDrawRectangle(EZ_Display, RootWindow(EZ_Display, EZ_ScreenNum),
			     EZ_XORGC, LastX, LastY, LastW, LastH);
	      XDrawLine(EZ_Display, RootWindow(EZ_Display, EZ_ScreenNum),
			EZ_XORGC, LastX, LastY, LastX+LastW, LastY+LastH);
	      XDrawLine(EZ_Display, RootWindow(EZ_Display, EZ_ScreenNum),
			EZ_XORGC, LastX+LastW, LastY, LastX, LastY+LastH);
	    }
	  LastX = x;  LastY = y; LastW = w; LastH = h;
	  
	  if(!flag && (LastW || LastH))
	    {
	      XDrawRectangle(EZ_Display, RootWindow(EZ_Display, EZ_ScreenNum),
			     EZ_XORGC, LastX, LastY, LastW, LastH);
	      XDrawLine(EZ_Display, RootWindow(EZ_Display, EZ_ScreenNum),
			EZ_XORGC, LastX, LastY, LastX+LastW, LastY+LastH);
	      XDrawLine(EZ_Display, RootWindow(EZ_Display, EZ_ScreenNum),
			EZ_XORGC, LastX+LastW, LastY, LastX, LastY+LastH);
	    }
	}
    }
  XFlush(EZ_Display);
}
/*****************************************************************************
 * 
 * Make a snap shot of widget. Save the result in SnapShotBitmap.
 */
#include "xpms/ez.xpm"
static EZ_Bitmap *default_drag_bitmap = (EZ_Bitmap *)NULL;


int EZ_MakeSnapShot(widget, type, XX,YY,WW,HH)
     EZ_Widget * widget; 
     int type,XX,YY,WW,HH;
{
  int    (*OldErrorHandler)();
  XImage *image;
  Pixmap pixmap=(Pixmap)NULL;
  int  x,y, w,h; 

  if(widget == (EZ_Widget *)NULL || EZ_WidgetMaped(widget) == 0)
    return(EZ_DND_DRAG_ICON_OUTLINE);
  
  if(type == 0) /* grab the whole widget window */
    {
      x = 0; 
      y = 0;
      w = EZ_WidgetWidth(widget);
      h = EZ_WidgetHeight(widget);
    }
  else  /* grab part of the widget window */
    {
      x = XX;
      y = YY;
      w = WW;
      h = HH;
    }

  /* read the window */
  EZ_XErrorCode = 0;
  OldErrorHandler = XSetErrorHandler(EZ_XErrorHandler);
  image = XGetImage(EZ_Display,
		    EZ_WidgetWindow(widget),
		    x, y, w, h,
		    (1<< EZ_Depth)-1,
		    ZPixmap);
  XSync(EZ_Display, False);
  XSetErrorHandler(OldErrorHandler);

  if(EZ_XErrorCode != 0) /* cannot read the region, just create a default one */
    {
      GC gc = EZ_NTILEGC;

      if(default_drag_bitmap == (EZ_Bitmap *)NULL)
	default_drag_bitmap = EZ_CreateLabelPixmapFromXpmData(ez_xpm);

      if(default_drag_bitmap)
	{
	  pixmap = XCreatePixmap(EZ_Display,RootWindow(EZ_Display, EZ_ScreenNum),
				 w, h, EZ_Depth);      
	  XSetTile(EZ_Display,gc, default_drag_bitmap->pixmap);
	  XSetTSOrigin(EZ_Display, gc, 0,0);
	  XFillRectangle(EZ_DisplayForWidgets, pixmap, gc, 0,0, w, h);       
	}
    }
  else
    {
      pixmap = XCreatePixmap(EZ_Display,RootWindow(EZ_Display, EZ_ScreenNum),
			     w, h, EZ_Depth);      
      XPutImage(EZ_Display, pixmap, EZ_DARKREDGC,image, 0,0,0,0,image->width, image->height);
      XDestroyImage(image);
    }
  if(pixmap != (Pixmap)NULL)
    {
      EZ_SnapShotBitmap->pixmap = pixmap;
      EZ_SnapShotBitmap->width = w;
      EZ_SnapShotBitmap->height = h;
      return(EZ_DND_DRAG_ICON_PIXMAP);
    }
  else return(EZ_DND_DRAG_ICON_OUTLINE);
}
/***********************************************************************************/
void  EZ_SetExecutorCommand(widget, command, cargs)
     EZ_Widget *widget; char *command, *cargs;
{
  if(widget && EZ_WidgetType(widget) == EZ_WIDGET_EXECUTOR)
    {
      char *tmp = (char *)NULL, *atmp = (char *)NULL;
      if(command)
	{
	  int len = strlen(command) + 2;
	  tmp = (char *)my_malloc(len * sizeof(char), _EXECUTOR_COMMAND_);
	  if(!tmp) EZ_OutOfMemory("EZ_SetExecutorCommand");
	  (void) strcpy(tmp, command);
	  (void) strcat(tmp, " ");
	  if(cargs)
	    {
	      int len = strlen(cargs) + 1;
	      atmp = (char *)my_malloc(len * sizeof(char), _EXECUTOR_ARGS_);
	      if(!atmp) EZ_OutOfMemory("EZ_SetExecutorCommand");
	      (void) strcpy(atmp, cargs);
	    }
	}
      if(EZ_ExecutorCommand(widget)) 
	{
	  if(EZ_EmbederConfirmed(widget) > 0) /* executor is running something */
	    { /* destroy the embeded application */
	      Window commWin = EZ_EmbederCommuWin(widget);
	      Window eWin = EZ_EmbederWin(widget);
	      unsigned long id = EZ_EmbederWidgetAddr(widget);
	      unsigned long pid = EZ_VerifyExistence(commWin);
	      if(pid == EZ_EmbederId(widget) && EZ_WindowExist(eWin))
		{
		  EZ_SendEmbedingMessage(EZ_EMBEDING_DESTROY_YOUSELF_C,
					 EZ_DummyWindow, EZ_WidgetWindow(widget),
					 (unsigned long)widget,
					 commWin, eWin, id, 
					 EZ_EmbederId(widget),
					 0,0,0,0, 0,0);
		}
	    }
	  EZ_EmbederCommuWin(widget) = (Window)NULL;
	  EZ_EmbederWin(widget) = (Window)NULL;
	  EZ_EmbederWidgetAddr(widget) = 0;
	  EZ_EmbederConfirmed(widget) = -1;
	  EZ_ExecutorMinW(widget) = 1;
	  EZ_ExecutorMinH(widget) = 1;
	  (void)my_free(EZ_ExecutorCommand(widget));
	  if(EZ_ExecutorCommandArgs(widget)) 
	    (void)my_free(EZ_ExecutorCommandArgs(widget));	  
	}
      EZ_ExecutorCommand(widget) = tmp;
      EZ_ExecutorCommandArgs(widget) = atmp;
      if(EZ_WidgetMapped(widget)) EZ_DisplayWidget(widget);
    }
}

/***********************************************************************************
 *
 * Compute the intersection of two rectangles. 
 */
void EZ_IntersectRectangles(px,py,pw,ph,x,y,w,h, x_r,y_r, w_r, h_r)
     int px, py, pw,ph;        /* origin and dimension of the first rectangle */
     int x,y,w,h;              /* of the second */
     int *x_r,*y_r,*w_r,*h_r;  /* the return */
{
  int xx[4], yy[4], tmp;

  if(x >= pw + px || y >= ph + py || x + w <= px || y + h <= py) { *w_r = 0; *h_r = 0; return;}

  if(x < px)       { xx[0] = x;  xx[2] = px; }
  else             { xx[0] = px;  xx[2] = x; }
  if(x+w < px+pw)  { xx[1] = x+w;xx[3] = px+pw;}
  else             { xx[1] = px+pw; xx[3] = x+w;}
  if(xx[1] > xx[2])
    {
      tmp = xx[1]; xx[1] = xx[2]; xx[2] = tmp;
    }

  if(y < py)       { yy[0] = y;  yy[2] = py; }
  else             { yy[0] = py;  yy[2] = y; }
  if(y+h < py+ph)  { yy[1] = y+h;yy[3] = py+ph;}
  else             { yy[1] = py+ph; yy[3] = y+h;}
  if(yy[1] > yy[2])
    {
      tmp = yy[1]; yy[1] = yy[2]; yy[2] = tmp;
    }

  *x_r = xx[1];
  *y_r = yy[1];
  *w_r = xx[2] - xx[1];
  *h_r = yy[2] - yy[1];
}
/***********************************************************************************/
#undef _EZ_WIDGET_EMBEDER_C_


