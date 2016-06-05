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
#define _EZ_DND_C_

#include "EZ_Widget.h"

/*********************************************************************
 * 
 *  Global Variables
 */
EZ_DnDWidgetListEntry  EZ_DnDWidgetList;
/* atom list */
#define EZ_DefineDnDAtom(a,b)  Atom a;
#include "EZ_DnDAtom.h"
#undef EZ_DefineDnDAtom

/*********************************************************************
 * 
 *  Functions implemented in this file:
 */
Atom EZ_GetAtom  MY_ANSIARGS((char *name));
int  EZ_DnDTargetTryNextTarget MY_ANSIARGS((void));
void EZ_InitializeDnD MY_ANSIARGS((void));
void EZ_Insert2DnDWidgetList MY_ANSIARGS((EZ_Widget *widget));
void EZ_RemveWidgetFromDnDList MY_ANSIARGS((EZ_Widget *widget));
void EZ_DestroyDnDList MY_ANSIARGS((void));
void EZ_GrabButtonRelease MY_ANSIARGS((void));
void EZ_UngrabButtonRelease MY_ANSIARGS((void));
void EZ_DnDSendDataConversionRequest MY_ANSIARGS((void));
void EZ_InitDrag MY_ANSIARGS((unsigned long id,
			      int type,
			      Window window,
			      EZ_Widget *widget,
			      void      *item,
			      int x, int y, int w, int h,
			      int px, int py, int xoff, int yoff));
void EZ_FinishDrag MY_ANSIARGS((Window cwin, Window win, int px, int py));
void EZ_AbortDrag MY_ANSIARGS((void));

void EZ_DnDMatchTargetsWithDecoders MY_ANSIARGS((void));
void EZ_DnDFindNextDecoder MY_ANSIARGS((void));
void EZ_DnDFindDataEncoder MY_ANSIARGS((Atom target));
void EZ_DnDSendErrorMessage MY_ANSIARGS((int sender, int errType, EZ_DnDMessage *theMsg));
void EZ_DnDSendConversion MY_ANSIARGS((Atom type, char *message, int length ));
void EZ_HighlightDropSiteWidget MY_ANSIARGS((void));
void EZ_DnDInformDeliverError MY_ANSIARGS((int type, EZ_DnDMessage *theMsg));
void EZ_EncodeDnDMessageHeader MY_ANSIARGS((int type, char *message, int length, Window *receiver));
void EZ_DecodeDnDMessageHeader  MY_ANSIARGS((char *message, EZ_DnDMessage *theMsg));

void EZ_DnDSendRequestHelpMsg MY_ANSIARGS((void));
void EZ_DnDSendCancelHelpMsg MY_ANSIARGS((void));
void EZ_DnDShowTargetHelp MY_ANSIARGS((void));
void EZ_DnDCancelTargetHelp MY_ANSIARGS((void));
void EZ_DnDSendLeaveWindowMsg MY_ANSIARGS((void));

void EZ_EnableDnDBubbleHelp MY_ANSIARGS((void));
void EZ_DisableDnDBubbleHelp MY_ANSIARGS((void));
int  EZ_CheckDnDTimer MY_ANSIARGS((void));

void EZ_Insert2DnDWindowList MY_ANSIARGS((EZ_Widget *widget));
void EZ_RemoveFromDnDWindowList MY_ANSIARGS((EZ_Widget *widget));
void EZ_ResetGVDnD1C MY_ANSIARGS((void));
void EZ_DnDFinishDrag MY_ANSIARGS((void));
void EZ_DnDFinishDrop MY_ANSIARGS((void));
void EZ_DnDGetDragConversionTargets MY_ANSIARGS((Atom **targets_ret, int **ntargets_ret, 
						 unsigned int **t));
void EZ_DnDGetMatchedConversionTargets MY_ANSIARGS((Atom **targets_ret, int **ntargets_ret, 
						    unsigned int **t));
void EZ_CallDragInitProcedure  MY_ANSIARGS((void));
void EZ_SetDragIcon MY_ANSIARGS((EZ_Widget *widget, int dpyStyle));
void EZ_SetDragCursor MY_ANSIARGS((Cursor cursor));
void EZ_ShowDragIcon MY_ANSIARGS((int x, int y));

void EZ_HandleDnDEvents MY_ANSIARGS((EZ_Widget *widget, XEvent *event));
void EZ_SelectInput MY_ANSIARGS((Window win, long mask));
long EZ_GetForeignWindowEventMask MY_ANSIARGS((Window win));
void EZ_ResetForeignWindowEventMask MY_ANSIARGS((void));
/***************************************************************************************/
static int  find_widget_in_dnd_list MY_ANSIARGS((EZ_Widget *widget, 
						 EZ_DnDWidgetListEntry  *head));
/***************************************************************************************/
Atom EZ_GetAtom(name)
     char *name;
{
  Atom tmp = XInternAtom(EZ_Display, name, False);
  return(tmp);
}
/***************************************************************************************/
static void DnDHelpTimerCallback();

void EZ_InitializeDnD()
{
  EZ_DnDWidgetList.widget = (EZ_Widget *)NULL;
  EZ_DnDWidgetList.next = (EZ_DnDWidgetListEntry *)NULL;

  EZ_DnDInfo.cursor = (Cursor)NULL;
  EZ_DnDInfo.dragIcon = NULL;
  EZ_DnDInfo.siteHelpStatus = 0;
  EZ_DnDInfo.siteHelpString = NULL;  
  EZ_DnDInfo.site = NULL;
  EZ_DnDInfo.id = 0;
  EZ_DnDInfo.srcApplicationId = 0;
  EZ_DnDInfo.targetApplicationId = 0;
  
  EZ_DnDInfo.timerEnabled = 1;
  EZ_DnDInfo.timer = EZ_CreateTimer(1,0,-1, DnDHelpTimerCallback, NULL, 0);
  EZ_DnDInfo.dropMessage = (char *)my_malloc(12 * sizeof(char), _MISC_DATA_);
  (void) strcpy(EZ_DnDInfo.dropMessage, "Hi, there!");
  EZ_DnDInfo.dropMessageLength = 0;

#define EZ_DefineDnDAtom(a,b) { a = XInternAtom(EZ_Display, b, False); }
#include "EZ_DnDAtom.h"
#undef EZ_DefineDnDAtom
}
/***************************************************************************************/
void EZ_Insert2DnDWidgetList(widget)
     EZ_Widget *widget;
{
  if(widget && EZ_LookupWidgetFromAllHT(widget))
    {
      EZ_DnDWidgetListEntry  *tmp, *head = &EZ_DnDWidgetList;

      if(find_widget_in_dnd_list(widget, head)) return;      

      tmp = (EZ_DnDWidgetListEntry *)my_malloc(sizeof(EZ_DnDWidgetListEntry), _DND_LIST_);
      if(tmp)
	{
	  tmp->next = head->next;
	  tmp->widget = widget;
	  head->next = tmp;
	}
      else EZ_OutOfMemory("EZ_Insert2DnDWidgetList");
    }
}

static int find_widget_in_dnd_list(widget, head)
     EZ_Widget *widget;
     EZ_DnDWidgetListEntry  *head;
{
  EZ_DnDWidgetListEntry  *tmp = head->next;
  
  while(tmp)
    {
      if(tmp->widget == widget) { return(1); break; }
      tmp = tmp->next;
    }
  return(0);
}
/***************************************************************************************/
void EZ_DestroyDnDList()
{
  EZ_DnDWidgetListEntry  *head = &EZ_DnDWidgetList;  
  EZ_DnDWidgetListEntry  *tmp = head->next;
  head->next = NULL;

  while(tmp)
    {
      head = tmp->next;
      (void)my_free((char *) tmp);
      tmp = head;
    }
}
/***************************************************************************************/
void EZ_RemveWidgetFromDnDList(widget)
     EZ_Widget *widget;
{
  if(widget)
    {
      EZ_DnDWidgetListEntry  *front = &EZ_DnDWidgetList, *cur = front->next;

      while(cur)
	{
	  if(cur->widget == widget)
	    {
	      front->next = cur->next;
	      (void)my_free((char *)cur);
	      break;
	    }
	  else
	    {
	      front = cur;
	      cur = front->next;
	    }
	}
    }
}
/***************************************************************************************/
void EZ_GrabButtonRelease()
{
  int    (*OldErrorHandler)();
  Window         cwindow;
  char           *p, *q;
  EZ_ApplRoster *roster = EZ_OpenEZWGLRoster(0);
  for(p = roster->data; (p - roster->data) < roster->length;)
    {
      q = p;
      while( *p != 0) p++; p++;
      
      if(sscanf(q,"%lx",&cwindow) == 1)
	{
	  if(cwindow != EZ_DummyWindow && EZ_VerifyExistence(cwindow))
	    {
	      Atom          aType;
	      int           aFormat = 0, ans;
	      char          *data = NULL;
	      unsigned long bytesAfter, length;
	      OldErrorHandler = XSetErrorHandler(EZ_XErrorHandler);
	      EZ_XErrorCode = 0; 
	      ans = XGetWindowProperty(EZ_Display,
				       cwindow,
				       EZ_WLIST_ATOM,
				       0, EZ_XServerDataReqSize,
				       False,
				       XA_WINDOW,
				       &aType,
				       &aFormat,
				       &length,
				       &bytesAfter,
				       (unsigned char **)&data);
	      if(EZ_XErrorCode == 0 && (ans == Success))
		{
		  if((aFormat == 32) && (aType == XA_WINDOW) && length > 0)
		    {
		      Window *wins = (Window *)data;
		      int i;
		      for(i = 0; i < length; i++)
                        {
                          long mask = EZ_GetForeignWindowEventMask(wins[i]);
                          XSelectInput(EZ_Display, wins[i], ButtonReleaseMask|PointerMotionMask|
                                       ExposureMask|PointerMotionHintMask|KeyPressMask|mask);
                        }
		    }
		}
	      if(data != NULL) XFree(data);
	      XSetErrorHandler(OldErrorHandler);  
	    }
	}
    }
  {
    Window root = RootWindow(EZ_Display, EZ_ScreenNum);
    long mask = EZ_GetForeignWindowEventMask(root);
    XSelectInput(EZ_Display, root, ButtonReleaseMask|PointerMotionMask|
                 ExposureMask|PointerMotionHintMask|
                 KeyPressMask|mask);
  }
  EZ_CloseEZWGLRoster(roster);
  XSync(EZ_Display, False);
}
/***************************************************************************************/
void EZ_UngrabButtonRelease()
{
  int    (*OldErrorHandler)();
  Window         cwindow;
  char           *p, *q;
  EZ_ApplRoster *roster = EZ_OpenEZWGLRoster(0);
  for(p = roster->data; (p - roster->data) < roster->length;)
    {
      q = p;
      while( *p != 0) p++; p++;
      
      if(sscanf(q,"%lx",&cwindow) == 1)
	{
	  if(cwindow != EZ_DummyWindow && EZ_VerifyExistence(cwindow))
	    {
	      Atom          aType;
	      int           aFormat = 0, ans;
	      char          *data = NULL;
	      unsigned long bytesAfter, length;
	      OldErrorHandler = XSetErrorHandler(EZ_XErrorHandler);
	      EZ_XErrorCode = 0; 
	      ans = XGetWindowProperty(EZ_Display,
				       cwindow,
				       EZ_WLIST_ATOM,
				       0, EZ_XServerDataReqSize,
				       False,
				       XA_WINDOW,
				       &aType,
				       &aFormat,
				       &length,
				       &bytesAfter,
				       (unsigned char **)&data);
	      if(EZ_XErrorCode == 0 && (ans == Success))
		{
		  if((aFormat == 32) && (aType == XA_WINDOW) && length > 0)
		    {
		      Window *wins = (Window *)data;
		      int i;
		      for(i = 0; i < length; i++)
                        {
                          long mask = EZ_GetForeignWindowEventMask(wins[i]);
                          XSelectInput(EZ_Display, wins[i], mask);
                        }
                    }
		}
	      if(data != NULL) XFree(data);
	      XSetErrorHandler(OldErrorHandler);  
	    }
	}
    }
  {
    Window root = RootWindow(EZ_Display, EZ_ScreenNum);
    long mask = EZ_GetForeignWindowEventMask(root);
    XSelectInput(EZ_Display, root, mask);
  }
  EZ_CloseEZWGLRoster(roster);
  XSync(EZ_Display, False);
}
/***********************************************************************************************
 *  This func is called just before a drag started: Button is pressed over a dragSrc 
 *  and has been moved. Initialize the DragInfo struct.
 *
 *   Called by DragSrc only.
 */
void EZ_InitDrag(id, type, window, widget, item, x,y, w, h, px, py, xoff, yoff)
     unsigned long id;                /* unique id for this and the only drag */
     Window window;                   /* window it is started */
     int type;                        /* type: EZ_DND_OBJ_IS_ITEM or WIDGET */
     EZ_Widget *widget;               /* if item, it is the widget the item belongs, otherwise same as item */
     void      *item;                 /* the item (or the same as widget) */
     int x,y,w,h;                     /* geometry of item */
     int px,py,xoff,yoff;             /* pointer coor and offsets of pointer and item origin */
{
  if(EZ_DnDInfo.id != 0)
    {
      /* if not yet finished with the current drag, send a message to
       * the current drop target to check whether it is still interested
       * in the current drop. The target should response to this request.
       */
      unsigned long CommWin;
      unsigned long Win;

      if(EZ_DnDInfo.ImSrc)
	{
	  CommWin = EZ_DnDInfo.targetCommWin;
	  Win = EZ_DnDInfo.targetWin;
	  if(EZ_DnDInfo.targetApplicationId == EZ_VerifyExistence(CommWin) &&
	     CommWin == EZ_WindowIsDnDTarget((Window)Win))
	    {
	      EZ_SendDnDMessage(EZ_DND_SRC_REQUEST_UNLOCK, NULL, 0, 0);
	      return;
	    }
	  else EZ_DnDFinishDrag(); /* party no longer exist */
	}
      else if(EZ_DnDInfo.ImTarget)  /* src != target if we are here */
	{
	  CommWin = EZ_DnDInfo.srcCommWin;
	  Win = EZ_DnDInfo.srcWin;
	  if(EZ_DnDInfo.srcApplicationId == EZ_VerifyExistence(CommWin) &&
	     CommWin == EZ_WindowIsDnDSrc((Window)Win))
	    {
	      EZ_SendDnDMessage(EZ_DND_TARGET_REQUEST_UNLOCK, NULL, 0, 0);
	      return;	      
	    }
	  else EZ_DnDFinishDrop(); /* party no longer exist */
	}
    }
  EZ_DnDInfo.id = id;
  EZ_DnDInfo.actionType = 0;
  EZ_DnDInfo.ImSrc = 1;
  EZ_DnDInfo.ImTarget = 0;
  EZ_DnDInfo.srcType = type;
  EZ_DnDInfo.srcCommWin = (unsigned long)EZ_DummyWindow;
  EZ_DnDInfo.srcWin =  (unsigned long)window;
  EZ_DnDInfo.srcItem =  (unsigned long)item;
  EZ_DnDInfo.srcApplicationId = EZ_ApplicationSetupTime;
  EZ_DnDInfo.srcX =  x;
  EZ_DnDInfo.srcY =  y;
  EZ_DnDInfo.srcW =  w;
  EZ_DnDInfo.srcH =  h;
  EZ_DnDInfo.srcX1 = xoff;
  EZ_DnDInfo.srcY1 = yoff;

  EZ_DnDInfo.srcStatus = EZ_DRAG_STARTED;

  EZ_DnDInfo.srcNTargets = 0;
  EZ_DnDInfo.srcEncoder = (EZ_DnDDataEncoder  *)NULL;
  EZ_DnDInfo.thisAtom = 0L;  
  
  EZ_DnDInfo.px = px;
  EZ_DnDInfo.py = py;
  EZ_DnDInfo.ptWin = (unsigned long)window;
  EZ_DnDInfo.cursor = (Cursor)NULL;
  
  EZ_DnDInfo.ImTarget = 0;
  EZ_DnDInfo.targetType = 0;
  EZ_DnDInfo.targetCommWin = 0L;
  EZ_DnDInfo.targetWin =  0L;
  EZ_DnDInfo.targetItem =  0L;
  EZ_DnDInfo.targetApplicationId = 0;
  EZ_DnDInfo.targetX =  0;
  EZ_DnDInfo.targetY =  0;
  EZ_DnDInfo.targetW =  0;
  EZ_DnDInfo.targetH =  0;
  EZ_DnDInfo.targetX1 =  0;
  EZ_DnDInfo.targetY1 =  0;

  EZ_DnDInfo.targetStatus = 0;

  EZ_DnDInfo.targetDecoder = (EZ_DnDDataDecoder  *)NULL;

  EZ_DnDInfo.site = NULL;
  EZ_DnDInfo.siteHelpString = NULL;
  EZ_DnDInfo.siteHelpStatus = 0;
  EZ_DnDInfo.dragIcon = NULL;
  
  EZ_GrabButtonRelease(); 
}
/***********************************************************************
 *  This func is called when a drag is droped in a DropTarget, a window.
 *  Just fill out the info needed for communication.
 *
 *  Called by DnDSrc only. 
 */
void EZ_FinishDrag(commWin, win, px, py)
     Window commWin, win; /* info about target */
     int px, py;
{
  EZ_DnDInfo.srcStatus = EZ_DRAG_RELEASED;

  EZ_DnDInfo.px = px;
  EZ_DnDInfo.py = py;
  EZ_DnDInfo.ptWin = (unsigned long)win;
  
  EZ_DnDInfo.targetCommWin = (unsigned long)commWin;
  EZ_DnDInfo.targetWin =  (unsigned long)win;
  EZ_DnDInfo.targetApplicationId = (commWin == 0L? 0L: EZ_VerifyExistence(commWin));

  EZ_UngrabButtonRelease(); 
}

/***********************************************************************
 *  
 *  If for some reason a drop cannot be done, this is called by the
 *  drag src.
 */
void EZ_AbortDrag()
{
  if( EZ_DnDInfo.id )
    {
      int snap; 

      EZ_DnDInfo.srcStatus = EZ_DRAG_ABORTING;
      snap = EZ_DnDInfo.dragIconType;

      if(snap == EZ_DND_DRAG_ICON_CURSOR)
	{
	  int desX, desY, srcX, srcY;
	  int i, count, dx,dy,tx,ty,nx,ny, cx,cy;
	  Window win = (Window)EZ_DnDInfo.srcWin;
	  
	  XFlush(EZ_Display);
	  if(XGrabPointer(EZ_Display,win, True, 0L, GrabModeAsync, GrabModeAsync, None, 
			  EZ_DnDInfo.cursor, CurrentTime) == GrabSuccess)
	    EZ_PointerGrabed = 1;

	  srcX = EZ_DnDInfo.px;     /* current cursor position */
	  srcY = EZ_DnDInfo.py;
	  desX = EZ_DnDInfo.srcX - EZ_DnDInfo.srcX1;
	  desY = EZ_DnDInfo.srcY - EZ_DnDInfo.srcY1;
	  
	  dx = desX - srcX; 
	  dy = desY - srcY;
	  count = ( ((ABSV(dx) + ABSV(dy)) >> 8) + 1) << 2;
	  
	  tx = dx/count; 
	  ty = dy/count; 
	  cx = nx = srcX;
	  cy = ny = srcY;
	  for(i = 0; i < count; i++)
	    {
	      if(i == count-1) {tx = desX - cx; ty = desY - cy;}
	      if(EZ_DnDInfo.dragIcon != NULL) EZ_ShowDragIcon(cx,cy);
	      else XWarpPointer(EZ_Display, RootWindow(EZ_Display,EZ_ScreenNum),
				None,  cx, cy, EZ_XDisplayWidth, EZ_XDisplayHeight,
				tx,ty);
	      nx += tx; ny += ty;
	      cx = nx; cy = ny;
	    }
	  if(EZ_DnDInfo.dragIcon != NULL)
	    {
	      EZ_ShowDragIcon(cx,cy);
	      XSync(EZ_Display, False);
	      EZ_HideWidget(EZ_DnDInfo.dragIcon);
	    }
	  else  XWarpPointer(EZ_Display, RootWindow(EZ_Display,EZ_ScreenNum),
			     None,  cx, cy, EZ_XDisplayWidth, EZ_XDisplayHeight,
			     srcX-cx,srcY-cy);
	  if(EZ_PointerGrabed)
	    {
	      XUngrabPointer(EZ_Display,CurrentTime);
	      EZ_PointerGrabed = 0;
	    }
	}
      else if(snap == EZ_DND_DRAG_ICON_SPECIAL) /* workarea, flistblx, tree */
	{
	  int i, count, tx, ty,txx,tyy,dxx,dyy,sx,sy,ax,ay,Sx,Sy,oldx,oldy,dx,dy;

	  Sx = EZ_DnDInfo.srcX;  /* pointer location when DND started */
	  Sy = EZ_DnDInfo.srcY;
	  oldx = EZ_DnDInfo.px;  /* pointer location when Drag is dropped */
	  oldy = EZ_DnDInfo.py;

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
	      
	      EZ_MoveNItemOutlines(tx+ax,ty+ay, 0, i,1);
	      EZ_MoveNItemOutlines(tx+ax,ty+ay, 1, i,1);
	      XFlush(EZ_Display);
	    }
	  EZ_MoveNItemOutlines(0,0, 0, 1,1);
	}
      else  /* item/widget */
	{
	  int i, iarea, count, tx, ty, txx,tyy, dxx, dyy,sx,sy;
	  int dx,dy, nx, ny, snap,iww,ihh, ox, oy;

	  EZ_DnDInfo.srcStatus = EZ_DRAG_ABORTING;
	  iww = EZ_DnDInfo.srcW;
	  ihh = EZ_DnDInfo.srcH;
	  iarea = iww * ihh;
	  
	  if(iarea)
	    {
	      if(iarea < 1000) count = 64;
	      else if(iarea < 2000) count = 32;
	      else if(iarea < 3000) count = 20;
	      else if(iarea < 10000) count = 16;
	      else if(iarea < 20000) count = 8;
	      else count = 4;

	      
	      snap = EZ_DnDInfo.dragIconType;
	      if(snap == 0) count = count >> 2;

	      ox = EZ_DnDInfo.srcX - EZ_DnDInfo.srcX1;
	      oy = EZ_DnDInfo.srcY - EZ_DnDInfo.srcY1;

	      nx = EZ_DnDInfo.px + EZ_DnDInfo.srcX1;
	      ny = EZ_DnDInfo.py + EZ_DnDInfo.srcY1;
	      
	      dx = EZ_DnDInfo.srcX - nx;
	      dy = EZ_DnDInfo.srcY - ny;
	      
	      i = (ABSV(dx) + ABSV(dy)) >> 8;
	      count *= i + 1;
	      
	      tx = dx/count; dxx = dx - tx * count;
	      ty = dy/count; dyy = dy - ty * count;
	      
	      sx = sy = 1;
	      if(dxx < 0) {sx = -1; dxx = -dxx;}
	      if(dyy < 0) {sy = -1; dyy = -dyy;}
	      
	      txx = tyy = 0;
	      for(i = 0; i < count; i++)
		{
		  txx += dxx;  if(txx >= count) { txx -= count; nx += sx;}
		  tyy += dyy;  if(tyy >= count) { tyy -= count; ny += sy;}
		  nx += tx;
		  ny += ty;
		  XDrawLine(EZ_Display, RootWindow(EZ_Display, EZ_ScreenNum),
			    EZ_XORGC, ox, oy, nx, ny);
		  XDrawLine(EZ_Display, RootWindow(EZ_Display, EZ_ScreenNum),
			    EZ_XORGC, ox, oy, nx+iww, ny);

		  XDrawLine(EZ_Display, RootWindow(EZ_Display, EZ_ScreenNum),
			    EZ_XORGC, ox, oy, nx+iww, ny+ihh);
		  XDrawLine(EZ_Display, RootWindow(EZ_Display, EZ_ScreenNum),
			    EZ_XORGC, ox, oy, nx, ny+ihh);
		  EZ_MoveSnapShotOrOutline(snap, nx, ny, iww, ihh, 1);

		  XDrawLine(EZ_Display, RootWindow(EZ_Display, EZ_ScreenNum),
			    EZ_XORGC, ox, oy, nx, ny);
		  XDrawLine(EZ_Display, RootWindow(EZ_Display, EZ_ScreenNum),
			    EZ_XORGC, ox, oy, nx+iww, ny);
		  XDrawLine(EZ_Display, RootWindow(EZ_Display, EZ_ScreenNum),
			    EZ_XORGC, ox, oy, nx+iww, ny+ihh);
		  XDrawLine(EZ_Display, RootWindow(EZ_Display, EZ_ScreenNum),
			    EZ_XORGC, ox, oy, nx, ny+ihh);
		  EZ_MoveSnapShotOrOutline(snap, nx, ny, iww, ihh, 0);

		  XFlush(EZ_Display);
		}
	      EZ_MoveSnapShotOrOutline(snap, 0, 0, 0, 0, 1);
	      XFlush(EZ_Display);
	    }
	}
      EZ_DnDInfo.id = 0L;
    }
}

/************************************************************************************************
 *  given an array of targets src can convert, the dropSite (dropTarget) match all the targets
 *  with its registered decodes, and record the matches in EZ_DnDInf.targetNTargets and
 *  EZ_DnDInfo.targetTargets. 
 */
void EZ_DnDMatchTargetsWithDecoders()
{
  int ntargets = 0;
  EZ_Item   *item = NULL;
  EZ_Widget *widget = NULL;
  void      *obj = NULL;

  if(EZ_DnDInfo.targetItem)
    {
      EZ_DnDDataDecoder   *decoders = NULL;

      if(EZ_DnDInfo.targetType == EZ_DND_OBJ_IS_ITEM)
	{
	  item = (EZ_Item *)EZ_DnDInfo.targetItem;
	  if(item && EZ_LookupItemFromHT(item))
	    decoders = EZ_ItemDnDDataDecoders(item);
	  else item = NULL;
	}
      else if(EZ_DnDInfo.targetType == EZ_DND_OBJ_IS_WIDGET)
	{
	  widget = (EZ_Widget *)EZ_DnDInfo.targetItem;
	  if(widget && EZ_LookupWidgetFromAllHT(widget))
	    decoders = EZ_WidgetDnDDataDecoders(widget);
	  else widget = NULL;
	}
      
      if(item)  { EZ_ItemBlocked(item) += 1; obj = item;}
      if(widget){ EZ_WidgetBlocked(widget) += 1; obj = widget;}
      
      if(decoders)
	{
	  EZ_DnDDataDecoder  *ptr;
	  Atom  *TargetAtoms = EZ_DnDInfo.targetTargets;
	  Atom  *SrcAtoms = EZ_DnDInfo.srcTargets;
	  int    count = EZ_DnDInfo.srcNTargets;
	  unsigned int action_type = EZ_DnDInfo.actionType;

	  int i;

	  for(i = 0; i < count; i++)
	    {
	      Atom target = SrcAtoms[i];
	      ptr = decoders;

	      while(ptr)
		{
		  if(ptr->type == target  && ptr->tag == action_type)
		    {
		      if(ptr->decoder) 
			{TargetAtoms[ntargets] = target; ntargets++;}
		      break;
		    }
		  else ptr = ptr->next;
		}
	    }
	  EZ_DnDInfo.targetNTargets = ntargets;

	  /* if dnd target has filters (encoders for Atom EZ_DND_DROP_START_ATOM), invoke it now */
	  {
	    EZ_DnDDataDecoder *sdecoder = EZ_FindDecoderGivenTarget(decoders, EZ_DND_DROP_START_ATOM);

	    if(sdecoder)
	      {
		if(sdecoder->decoder)
		  (sdecoder->decoder)(obj, sdecoder->data, EZ_DnDInfo.dropMessage,
				      EZ_DnDInfo.dropMessageLength);
		if((sdecoder->callback).callback)   
		  ((sdecoder->callback).callback)(sdecoder->object,(sdecoder->callback).data);
		if(EZ_DnDInfo.targetNTargets > EZ_DND_MAX_TARGETS)
		  EZ_DnDInfo.targetNTargets = EZ_DND_MAX_TARGETS;
	      }
	  }
	}
    }
}

/*****************************************************************************/

void EZ_DnDGetMatchedConversionTargets(targets_ret, ntargets_ret, at_ret)
     Atom **targets_ret; int **ntargets_ret; unsigned int **at_ret;
{
  if(targets_ret)  *targets_ret = EZ_DnDInfo.targetTargets;
  if(ntargets_ret) *ntargets_ret = (int *)&(EZ_DnDInfo.targetNTargets);
  if(at_ret)       *at_ret = &(EZ_DnDInfo.actionType);
}

void EZ_DnDGetDragConversionTargets(targets_ret, ntargets_ret, at_ret)
     Atom **targets_ret; int **ntargets_ret; unsigned int **at_ret;
{
  if(targets_ret)  *targets_ret = EZ_DnDInfo.srcTargets;
  if(ntargets_ret) *ntargets_ret = (int *) &(EZ_DnDInfo.srcNTargets);
  if(at_ret)       *at_ret = &(EZ_DnDInfo.actionType);
}

/*****************************************************************************/
void EZ_DnDFindNextDecoder()
{
  EZ_DnDDataDecoder *decoders = NULL;
  int count = EZ_DnDInfo.targetNTargets;

  if(EZ_DnDInfo.targetType == EZ_DND_OBJ_IS_ITEM)
    decoders = EZ_ItemDnDDataDecoders((EZ_Item *)(EZ_DnDInfo.targetItem));
  else if(EZ_DnDInfo.targetType == EZ_DND_OBJ_IS_WIDGET)
    decoders = EZ_WidgetDnDDataDecoders((EZ_Widget *)(EZ_DnDInfo.targetItem));
  
  EZ_DnDInfo.targetDecoder = NULL;
  if(decoders && count > 0)
    {
      Atom *atoms = EZ_DnDInfo.targetTargets;
      unsigned int action_type = EZ_DnDInfo.actionType;
      int i = 0;

      while(i < count && atoms[i] == 0) i++;
      if(i < count) 
	{
	  Atom target = atoms[i];
	  EZ_DnDDataDecoder *ptr = decoders;
	  while(ptr)
	    {
	      if(ptr->type == target && ptr->tag == action_type) 
		{
		  if(ptr->decoder)   EZ_DnDInfo.targetDecoder = ptr;
		  break;
		}
	      ptr = ptr->next;
	    }
	}
    }
}

/************************************************************************************************/
int EZ_DnDTargetTryNextTarget()  /* return 1 if no more targets available */
{
  int i, count = EZ_DnDInfo.targetNTargets;
  Atom *atoms = EZ_DnDInfo.targetTargets;
  EZ_DnDDataDecoder *decoder = EZ_DnDInfo.targetDecoder;
  Atom ctarget = decoder->type;
  int  done = 1;

  /* we try the possible targets sequentially */
  i = 0;
  while(i < count)
    {
      if(atoms[i] == ctarget) 
	{ atoms[i] = 0; break; }
      i++; 
    }
  if(i < count)
    {
      EZ_DnDFindNextDecoder();          /* try a new target */
      if(EZ_DnDInfo.targetDecoder)
	{
	  EZ_DnDInfo.targetStatus = EZ_DROP_WAITING_FOR_DATA;
	  EZ_DnDSendDataConversionRequest();
	  done = 0;
	}
    }
  return(done);
}
/************************************************************************************************/
void EZ_DnDFindDataEncoder(target)
     Atom           target;
{
  EZ_DnDDataEncoder  *ptr, *encoders = NULL;

  if(EZ_DnDInfo.srcType == EZ_DND_OBJ_IS_ITEM)
    {
      EZ_Item *item = (EZ_Item *)EZ_DnDInfo.srcItem;
      if(item && EZ_LookupItemFromHT(item))
	encoders = EZ_ItemDnDDataEncoders(item);
    }
  else if(EZ_DnDInfo.srcType == EZ_DND_OBJ_IS_WIDGET)
    {
      EZ_Widget *widget = (EZ_Widget *)EZ_DnDInfo.srcItem;
      if(widget && EZ_LookupWidgetFromAllHT(widget))
	encoders = EZ_WidgetDnDDataEncoders(widget);
    }

  EZ_DnDInfo.srcEncoder = NULL;
  if(encoders)
    {
      unsigned int action_type = EZ_DnDInfo.actionType;
      ptr = encoders;
      while( ptr )
	{
	  if(ptr->type  == target && ptr->tag == action_type && ptr->encoder)
	    {
	      EZ_DnDInfo.srcEncoder = ptr;
	      return;
	    }
	  ptr = ptr->next;
	}
    }
}
/************************************************************************************************/
void EZ_DnDSendDataConversionRequest()
{
  char message[32];
  char *str = message;
  sprintf(str, "%lx ", (EZ_DnDInfo.targetDecoder)->type);

  EZ_SendDnDMessage(EZ_DND_REQUEST_CONVERSION, message, 32, 0);  
}
/************************************************************************************************/
void  EZ_DnDSendErrorMessage(sender, errType, theMsg)
     int sender;    /* EZ_DND_SENDER_IS_SRC or EZ_DND_SENDER_IS_TARGET */
     int errType;   /* the error type */
     EZ_DnDMessage *theMsg;   /* previous message received */
{
  /* if this is called, it means we had an error
   * in the current DnD transaction. It means that
   * we might not have set up the DnDInfo structure
   * yet. This is why we need help from theMsg
   * The party called this function aborts itself right
   * after the call. No future correspondence can be
   * done regarding this DnD message id. So actually there is
   * no needs to restore the DnDInfo structure ...
   */
  if(sender == EZ_DND_SENDER_IS_SRC)  /* sender is a drag src */
    {
      unsigned long oldTCommWin = EZ_DnDInfo.targetCommWin;
      unsigned long oldTWin = EZ_DnDInfo.targetWin; 
      unsigned long oldTItem = EZ_DnDInfo.targetItem; 

      EZ_DnDInfo.targetCommWin = theMsg->fmCommWin;
      EZ_DnDInfo.targetWin = theMsg->fmWin;
      EZ_DnDInfo.targetItem = theMsg->fmItem;
      EZ_SendDnDMessage(errType, NULL, 0, 0);
      
      EZ_DnDInfo.targetCommWin = oldTCommWin;
      EZ_DnDInfo.targetWin = oldTWin;
      EZ_DnDInfo.targetItem = oldTItem;
    }
  else  if(sender == EZ_DND_SENDER_IS_TARGET)  /* sender is a drag target */
    {
      unsigned long oldSCommWin = EZ_DnDInfo.srcCommWin;
      unsigned long oldSWin = EZ_DnDInfo.srcWin; 
      unsigned long oldSItem = EZ_DnDInfo.srcItem; 

      EZ_DnDInfo.srcCommWin = theMsg->fmCommWin;
      EZ_DnDInfo.srcWin = theMsg->fmWin;
      EZ_DnDInfo.srcItem = theMsg->fmItem;
      EZ_SendDnDMessage(errType, NULL, 0, 0);
      
      EZ_DnDInfo.srcCommWin = oldSCommWin;
      EZ_DnDInfo.srcWin = oldSWin;
      EZ_DnDInfo.srcItem = oldSItem; 
    }
}

/************************************************************************************************/
void EZ_DnDSendConversion(type, message, length)
     Atom type;
     char *message;
     int  length;
{
  EZ_DnDInfo.srcStatus = EZ_DRAG_SENDING_DATA;
  EZ_DnDInfo.thisAtom = type;
  EZ_SendDnDMessage(EZ_DND_DATA, message, length, 0);
  EZ_DnDInfo.srcStatus = EZ_DRAG_WAITING_FOR_ACK;
}

/****************************************************************************************************/

void EZ_DnDSendLeaveWindowMsg()
{
  unsigned long cw,w;
  unsigned long commWin =  EZ_WindowIsDnDTarget((Window)EZ_DnDInfo.ptWin);
  if(commWin != 0)
    {
      cw = EZ_DnDInfo.targetCommWin;
      w = EZ_DnDInfo.targetWin;
      
      EZ_DnDInfo.targetCommWin = commWin;
      EZ_DnDInfo.targetWin = (unsigned long)EZ_DnDInfo.ptWin;
      EZ_SendDnDMessage(EZ_DND_DRAG_LEFT, NULL, 0,0);
      EZ_DnDInfo.targetCommWin = cw;
      EZ_DnDInfo.targetWin = w;
    }
}
/************************************************************************************************/
void EZ_HighlightDropSiteWidget()
{
  if(EZ_LastDropSite && EZ_LookupWidgetFromAllHT(EZ_LastDropSite))
    {
      if(EZ_WidgetMapped(EZ_LastDropSite))
	{
	  XPoint points[5];
	  int w, h;
	  Window  win;
	  GC gc = EZ_DNDXORGC; 

	  win =  EZ_WidgetWindow(EZ_LastDropSite);
	  w = (int) EZ_WidgetWidth(EZ_LastDropSite) -1;
	  h = (int) EZ_WidgetHeight(EZ_LastDropSite) -1;

	  points[0].x = 1;   points[0].y = 1;
	  points[1].x = w;   points[1].y = 1;
	  points[2].x = w;   points[2].y = h;
	  points[3].x = 1;   points[3].y = h;
	  points[4].x = 1;   points[4].y = 1;
	  XDrawLines(EZ_Display, win, gc, points, 5, CoordModeOrigin);
	}
    }
}
/************************************************************************************************/
void EZ_DnDInformDeliverError(message_type, theMsg)
     int message_type;
     EZ_DnDMessage *theMsg;
{
  (void)fprintf(stderr, "DnDMessageDeliverError: from [%lx %lx] to [%lx %lx]\n",
		theMsg->fmCommWin, theMsg->fmWin, theMsg->toCommWin, theMsg->toWin);
  (void)fprintf(stderr, "\t Message id=%lx, type=%d\n",theMsg->id, message_type);
}
/************************************************************************************************/
void EZ_EncodeDnDMessageHeader(type, message, length, receiver)
     int type;
     char *message;
     int length;
     Window *receiver;
{
  int toWhom;
  unsigned long msgLength = (unsigned long)length + EZ_DND_MSG_HEADER_SIZE;
  *receiver = (Window)NULL;
#define TO_DND_SRC      0
#define TO_DND_TARGET   1
#define TO_DND_UNKNOWN  -1
#define msgHeader  EZ_DnDInfo.theHeader

  switch(type)
    {
    case     EZ_DND_DRAG_START:
    case     EZ_DND_DRAG_MOTION:
    case     EZ_DND_DRAG_DROPPED:
    case     EZ_DND_CONVERSION_FAILED:
    case     EZ_DND_CONVERSION_REFUSED:
    case     EZ_DND_DATA:
    case     EZ_DND_ABORT_TO_TARGET:      
    case     EZ_DND_SRC_REQUEST_UNLOCK:
    case     EZ_DND_DRAG_CANCELLED:
    case     EZ_DND_SRC_REQUEST_HELP:
    case     EZ_DND_SRC_CANCEL_HELP:
    case     EZ_DND_DRAG_LEFT:
      toWhom = TO_DND_TARGET;
      break;
    case     EZ_DND_REQUEST_CONVERSION:
    case     EZ_DND_ABORT_TO_SRC:
    case     EZ_DND_TARGET_REQUEST_UNLOCK:
    case     EZ_DND_DROP_DONE: 
      toWhom = TO_DND_SRC;
      break;
    default:
      toWhom = TO_DND_UNKNOWN;
      break;
    }
  if(toWhom == TO_DND_SRC) /* I'm a DnDTarget, send to DnDSrc */
    { 
      /*   1  2    3     4     5    6    7    8     9    10    11    12   13    14   15  16 17 18 19  20 21 22 23*/
      /* type id len tType tCwin tWin tItem  tId  fCwin fWin  fItem  fId  ptWin ptX ptY   X  Y  W  H  x1 y1 ^^ atom*/
      /*                   1  2   3   4   5   6   7   8   9  10   11 12   13 14 15 16 17 18 19 20 21 22 */ 
      sprintf(msgHeader, "%x %lx %lx %lx %lx %lx %lx %lx %lx %lx %lx %lx %lx %d %d %d %d %d %d %d %d %d %lx",
	      /* 1 */type,
	      /* 2 */EZ_DnDInfo.id,
	      /* 3 */(unsigned long)msgLength,
	      /* 4 */(unsigned long)EZ_DND_RECEIVER_IS_SRC,
	      /* 5 */EZ_DnDInfo.srcCommWin,
	      /* 6 */EZ_DnDInfo.srcWin,
	      /* 7 */EZ_DnDInfo.srcItem,
	      /* 8 */EZ_DnDInfo.srcApplicationId,
	      /* 9 */EZ_DnDInfo.targetCommWin,	      
	      /* 10 */EZ_DnDInfo.targetWin,
	      /* 11 */EZ_DnDInfo.targetItem,
	      /* 12 */EZ_DnDInfo.targetApplicationId,
	      /* 13 */EZ_DnDInfo.ptWin,
	      /* 14 */EZ_DnDInfo.px,
	      /* 15 */EZ_DnDInfo.py,
	      /* 16 */EZ_DnDInfo.targetX,
	      /* 17 */EZ_DnDInfo.targetY,
	      /* 18 */EZ_DnDInfo.targetW,
	      /* 19 */EZ_DnDInfo.targetH,
	      /* 20 */EZ_DnDInfo.targetX1,
	      /* 21 */EZ_DnDInfo.targetY1,
	      /* 22 */ 0,
	      /* 23 */ 0L);
      *receiver = (Window)EZ_DnDInfo.srcCommWin;
    }
  else if(toWhom == TO_DND_TARGET) /* I'm a DnDSrc, send to DnDTarget */
    { 
      /*   1  2    3     4     5    6    7    8     9    10    11    12   13    14   15  16 17 18 19  20 21 22  23*/
      /* type id len tType tCwin tWin tItem  tId  fCwin fWin  fItem  fId  ptWin ptX ptY   X  Y  W  H  x1 y1 ^^ atom*/
      /*                   1  2   3   4   5   6   7   8   9  10  11  12  13  14 15 16 17 18 19 20 21 22 23*/
      sprintf(msgHeader, "%x %lx %lx %lx %lx %lx %lx %lx %lx %lx %lx %lx %lx %d %d %d %d %d %d %d %d %d %lx",
	      /* 1 */type,
	      /* 2 */EZ_DnDInfo.id,
	      /* 3 */(unsigned long)msgLength,
	      /* 4 */(unsigned long)EZ_DND_RECEIVER_IS_TARGET,
	      /* 5 */EZ_DnDInfo.targetCommWin,
	      /* 6 */EZ_DnDInfo.targetWin,
	      /* 7 */EZ_DnDInfo.targetItem,
	      /* 8 */EZ_DnDInfo.targetApplicationId,
	      /* 9 */EZ_DnDInfo.srcCommWin,	      
	      /* 10 */EZ_DnDInfo.srcWin,
	      /* 11 */EZ_DnDInfo.srcItem,
	      /* 12 */EZ_DnDInfo.srcApplicationId,
	      /* 13 */EZ_DnDInfo.ptWin,
	      /* 14 */EZ_DnDInfo.px,
	      /* 15 */EZ_DnDInfo.py,
	      /* 16 */EZ_DnDInfo.srcX,
	      /* 17 */EZ_DnDInfo.srcY,
	      /* 18 */EZ_DnDInfo.srcW,
	      /* 19 */EZ_DnDInfo.srcH,
	      /* 20 */EZ_DnDInfo.srcX1,
	      /* 21 */EZ_DnDInfo.srcY1,
	      /* 22 */EZ_DnDInfo.dragIconType,
	      /* 23 */EZ_DnDInfo.thisAtom);
      *receiver = (Window)EZ_DnDInfo.targetCommWin;
    }
#undef msgHeader
#undef TO_DND_SRC     
#undef TO_DND_TARGET  
#undef TO_DND_UNKNOWN 
}

void EZ_DecodeDnDMessageHeader(message, theMsg)
     char *message;
     EZ_DnDMessage *theMsg;
{
  theMsg->id = 0;

  /*   1  2    3     4     5    6    7    8     9    10    11    12   13    14   15  16 17 18 19  20 21 22 23 */
  /* type id len tType tCwin tWin tItem  tId  fCwin fWin  fItem  fId  ptWin ptX ptY   X  Y  W  H  x1 y1 ^^ dt */
  /*                 1  2   3   4   5   6   7   8   9  10  11  12  13  14 15 16 17 18 19 20 21 22 23 */
  sscanf(message,  "%x %lx %lx %lx %lx %lx %lx %lx %lx %lx %lx %lx %lx %d %d %d %d %d %d %d %d %d %lx",
	 /* 1 */&(theMsg->type),
	 /* 2 */&(theMsg->id),
	 /* 3 */&(theMsg->length),
	 /* 4 */&(theMsg->toType),
	 /* 5 */&(theMsg->toCommWin),
	 /* 6 */&(theMsg->toWin),
	 /* 7 */&(theMsg->toItem),
	 /* 8 */&(theMsg->toId),
	 /* 9 */&(theMsg->fmCommWin),
	 /* 10 */&(theMsg->fmWin),
	 /* 11 */&(theMsg->fmItem),
	 /* 12 */&(theMsg->fmId),
	 /* 13 */&(theMsg->ptWin),
	 /* 14 */&(theMsg->px),
	 /* 15 */&(theMsg->py),
	 /* 16 */&(theMsg->x),
	 /* 17 */&(theMsg->y),
	 /* 18 */&(theMsg->w),
	 /* 19 */&(theMsg->h),
	 /* 20 */&(theMsg->x1),
	 /* 21 */&(theMsg->y1),
	 /* 22 */&(theMsg->iconType),
	 /* 23 */&(theMsg->thisAtom));
  theMsg->message = message;
}
/*******************************************************************/
/*
dump_dndinfo()
{
  printf("EZ_DnDInfo.id %d\n", EZ_DnDInfo.id);

  printf("EZ_DnDInfo.ImSrc %d\n", EZ_DnDInfo.ImSrc);
  printf("EZ_DnDInfo.srcCommWin %lx\n", EZ_DnDInfo.srcCommWin);
  printf("EZ_DnDInfo.srcWin %lx\n", EZ_DnDInfo.srcWin);
  printf("EZ_DnDInfo.srcItem %lx\n\n", EZ_DnDInfo.srcItem);
  printf("EZ_DnDInfo.XYWH %d %d %d %d %d %d\n\n", EZ_DnDInfo.srcX,EZ_DnDInfo.srcY,
	 EZ_DnDInfo.srcW,EZ_DnDInfo.srcH, EZ_DnDInfo.srcX1,EZ_DnDInfo.srcY1);


  printf("pointer is at [%d %d]\n", EZ_DnDInfo.px,  EZ_DnDInfo.py);
  printf("EZ_DnDInfo.ImTarget %d\n", EZ_DnDInfo.ImTarget);
  printf("EZ_DnDInfo.targetCommWin %lx\n", EZ_DnDInfo.targetCommWin);
  printf("EZ_DnDInfo.targetWin %lx\n", EZ_DnDInfo.targetWin);
  printf("EZ_DnDInfo.targetItem %lx\n", EZ_DnDInfo.targetItem);
}
*/

/****************************************************************************************************/

void EZ_DnDSendCancelHelpMsg()
{
  unsigned long cw,w;
  unsigned long commWin = EZ_WindowIsDnDTarget((Window)EZ_DnDInfo.ptWin);
  cw = EZ_DnDInfo.targetCommWin;
  w = EZ_DnDInfo.targetWin;
  
  EZ_DnDInfo.targetCommWin = commWin;
  EZ_DnDInfo.targetWin = (unsigned long)EZ_DnDInfo.ptWin;
  EZ_SendDnDMessage(EZ_DND_SRC_CANCEL_HELP, NULL, 0,0);
  EZ_DnDInfo.targetCommWin = cw;
  EZ_DnDInfo.targetWin = w;
}

void EZ_DnDSendRequestHelpMsg()
{
  unsigned long cw,w;
  unsigned long commWin =  EZ_WindowIsDnDTarget((Window)EZ_DnDInfo.ptWin);
  cw = EZ_DnDInfo.targetCommWin;
  w = EZ_DnDInfo.targetWin;
  
  EZ_DnDInfo.targetCommWin = commWin;
  EZ_DnDInfo.targetWin = (unsigned long)EZ_DnDInfo.ptWin;
  EZ_SendDnDMessage(EZ_DND_SRC_REQUEST_HELP, NULL, 0,0);
  EZ_DnDInfo.targetCommWin = cw;
  EZ_DnDInfo.targetWin = w;
}
/****************************************************************************************************/
void EZ_EnableDnDBubbleHelp()
{
  if(EZ_DnDInfo.timerEnabled == 0 || EZ_DnDInfo.timer == NULL)
    {
      EZ_DnDInfo.timer = EZ_CreateTimer(1,0,-1, DnDHelpTimerCallback, NULL, 0);
      EZ_DnDInfo.timerEnabled = 1;
    }
}
void EZ_DisableDnDBubbleHelp()
{
  if(EZ_DnDInfo.timerEnabled)
    {
      EZ_CancelTimer(EZ_DnDInfo.timer);
      EZ_DnDInfo.timerEnabled = 0;
    }
}

static void DnDHelpTimerCallback(timer)
     EZ_Timer *timer;
{
  if(EZ_DnDInfo.siteHelpStatus == 0 && EZ_DnDInfo.site != NULL && EZ_DnDInfo.siteHelpString != NULL )
    EZ_DnDShowTargetHelp();
}   

/***********************************************************************************************
 *
 *  unpost the "Bubble" help on current drop site
 */
void EZ_DnDCancelTargetHelp()
{
  EZ_HideWidget(EZ_BubbleHelp);
  EZ_DnDInfo.site = NULL;
  EZ_DnDInfo.siteType = EZ_DND_OBJ_IS_UNKNOWN;
  EZ_DnDInfo.siteHelpString = NULL;
  EZ_DnDInfo.siteHelpStatus = 0;
}

/*
 *  post a "Bubble" help on current drop site
 */
void EZ_DnDShowTargetHelp()
{
  if(EZ_DnDInfo.siteHelpStatus == 0 && EZ_DnDInfo.siteType != EZ_DND_OBJ_IS_UNKNOWN &&
     EZ_DnDInfo.site != NULL && EZ_DnDInfo.siteHelpString != NULL)
    {
      int x,y,xx,yy,w,h,h1, ww,hh;
      int offset = 16;
      if(EZ_DnDInfo.dragIconType == EZ_DND_DRAG_ICON_CURSOR) offset = 0;	    
      
      if(EZ_DnDInfo.siteType == EZ_DND_OBJ_IS_ITEM)
	{
	  EZ_SetWidgetLabelString(EZ_BubbleHelp, EZ_DnDInfo.siteHelpString);
	  EZ_ClearWidgetSizeComputedFlag(EZ_BubbleHelp);
	  EZ_ComputeWidgetWindowSize(EZ_BubbleHelp, &ww, &hh, NULL);

	  xx = EZ_DnDInfo.px;
	  yy = EZ_DnDInfo.py;
	  h = EZ_DnDInfo.srcH;
	  h1 = (int)EZ_ItemAHeight((EZ_Item *)EZ_DnDInfo.site);
	  h = MAXV(h,h1);

	  x = xx - (ww >> 1); if(x < 0) x = 0;
	  y = yy - (hh + h + offset); if(y < 0) y = yy + ( h +offset);

	  EZ_SetWidgetPosition(EZ_BubbleHelp,x,y);
	  EZ_DisplayWidget(EZ_BubbleHelp);
	  EZ_DnDInfo.siteHelpStatus = 1;
	}
      else if(EZ_DnDInfo.siteType == EZ_DND_OBJ_IS_WIDGET)
	{
	  int top = 1;
	  int wox,woy,dx,dy,tw,th;
	  Window child;
	  EZ_Widget *widget = (EZ_Widget *)EZ_DnDInfo.site;

	  offset = 4;
	  
	  /* figure out the origin of widget */
	  XTranslateCoordinates(EZ_Display, EZ_WidgetWindow(widget),
				RootWindow(EZ_Display, EZ_ScreenNum),
				0, 0, &wox,&woy, &child);
	  tw = (int)EZ_WidgetWidth(widget);
	  th = (int)EZ_WidgetHeight(widget);

	  if(th > tw) top = 0;  /* pop help to the left */

	  /* ptr location */
	  xx = EZ_DnDInfo.px;
	  yy = EZ_DnDInfo.py;
	  
	  /* offset */
	  dx = xx - wox;
	  dy = yy - woy;

	  h = EZ_DnDInfo.srcH;
	  w = EZ_DnDInfo.srcW;

	  EZ_SetWidgetLabelString(EZ_BubbleHelp, EZ_DnDInfo.siteHelpString);
	  EZ_ClearWidgetSizeComputedFlag(EZ_BubbleHelp);
	  EZ_ComputeWidgetWindowSize(EZ_BubbleHelp, &ww, &hh, NULL);
	  
	  if(top)
	    {
	      int tmp = MAXV(dy, h);
	      y = yy - (tmp + offset + hh); 
	      if(y < 0)
		{
		  tmp = woy + th - yy;
		  tmp = MAXV(tmp, h);
		  y = yy + (tmp +offset); 
		}
	      x = xx - (ww >> 1); if(x < 0) x = 0;
	    }
	  else
	    {
	      int tmp = MAXV(dx, w);
	      x = xx - ( tmp + offset + ww);
	      if(x < 0)
		{
		  tmp = wox + tw - xx;
		  tmp = MAXV(tmp,w);
		  x = xx + (tmp + offset);
		}
	      y = yy - (hh >> 1); if(y < 0) y = 0;
	    }
	  EZ_SetWidgetPosition(EZ_BubbleHelp,x,y);
	  EZ_DisplayWidget(EZ_BubbleHelp);
	  EZ_DnDInfo.siteHelpStatus = 1;
	}
    }
}
/****************************************************************************************************/
extern void   EZ_GetCurrentTime();
int EZ_CheckDnDTimer()
{
  struct timeval ctv,*tv;
  long ts, tus;
  
  tv = &ctv;
  EZ_GetCurrentTime(tv);
  ts = tv->tv_sec; tus = tv->tv_usec;

  return(EZ_CheckRealTimers(ts, tus));
}
/****************************************************************************************************/
static int    windowsCnt = 0;
static Window windows[512];
/****************************************************************************************************/
void EZ_ResetGVDnDC() { windowsCnt = 0; }
/****************************************************************************************************/
void EZ_Insert2DnDWindowList(widget)
     EZ_Widget *widget;
{
  Window window;

  if(widget == NULL) return;
  window = EZ_WidgetWindow(widget);
  if(window != (Window)None)
    {
      int i;
      for(i = 0; i < windowsCnt; i++)
	if(windows[i] == window) return;
      /* window is not currently registered */
      if(windowsCnt < 511)
	{
	  windows[windowsCnt++] = window;
	  XChangeProperty(EZ_Display,
			  EZ_DummyWindow,
			  EZ_WLIST_ATOM,
			  XA_WINDOW, 32,
			  PropModeReplace,
			  (unsigned char *)windows,
			  windowsCnt);
	}
    }
}
     
void EZ_RemoveFromDnDWindowList(widget)
     EZ_Widget *widget;
{
  Window window;
  if(widget == NULL) return;
  window = EZ_WidgetWindow(widget);
  if(window != (Window)None)
    {
      int i, j;

      for(i = 0; i < windowsCnt; i++)
	{
	  if(windows[i] == window)
	    {
	      for(j = i, i++;  i < windowsCnt; i++, j++)
		windows[j] = windows[i];
	      windowsCnt--; if(windowsCnt < 0) windowsCnt = 0;
	      XChangeProperty(EZ_Display,
			      EZ_DummyWindow,
			      EZ_WLIST_ATOM,
			      XA_WINDOW, 32,
			      PropModeReplace,
			      (unsigned char *)windows,
			      windowsCnt);
	      XSync(EZ_Display, False);
	      break;
	    }
	}
    }
}
/****************************************************************************************************/
/* added  3/10/98 */
void EZ_DnDFinishDrag()
{
  EZ_Item   *item = NULL;
  EZ_Widget *widget = NULL;
  if(EZ_DnDInfo.srcItem)
    {
      EZ_DnDDataEncoder  *sencoder, *encoders = NULL;

      if(EZ_DnDInfo.srcType == EZ_DND_OBJ_IS_ITEM)
	{
	  item = (EZ_Item *)EZ_DnDInfo.srcItem;
	  if(item && EZ_LookupItemFromHT(item))	  
	    encoders = EZ_ItemDnDDataEncoders(item);
	  else item = NULL;
	}
      else if(EZ_DnDInfo.srcType == EZ_DND_OBJ_IS_WIDGET)
	{
	  widget = (EZ_Widget *)EZ_DnDInfo.srcItem;
	  if(widget && EZ_LookupWidgetFromAllHT(widget))
	    encoders = EZ_WidgetDnDDataEncoders(widget);
	  else widget = NULL;
	}

      sencoder = EZ_FindEncoderGivenTarget(encoders, EZ_DND_DRAG_FINISH_ATOM);
      if(sencoder)
	{
	  if(sencoder)
	      {
		int length = 0, needFree = 0;
		char *message = NULL;
		
		if(sencoder->encoder)
		  (sencoder->encoder)(sencoder->object, sencoder->data, &message, &length, &needFree);
		if((sencoder->callback).callback)   
		  ((sencoder->callback).callback)(sencoder->object,(sencoder->callback).data);
		/* this is insane !! */
		if(length>0 && needFree && message) (void)my_free((void *)message);
	      }
	}
    }
  if(item)   EZ_ItemBlocked(item) -= 1;
  if(widget) EZ_WidgetBlocked(widget) -= 1;
}
void EZ_DnDFinishDrop()
{
  EZ_Item   *item = NULL;
  EZ_Widget *widget = NULL;
  
  if(EZ_DnDInfo.targetItem)
    {
      EZ_DnDDataDecoder  *sdecoder, *decoders = NULL;

      if(EZ_DnDInfo.targetType == EZ_DND_OBJ_IS_ITEM)
	{
	  item = (EZ_Item *)EZ_DnDInfo.targetItem;
	  if(item && EZ_LookupItemFromHT(item))
	    decoders = EZ_ItemDnDDataDecoders(item);
	  else item = NULL;
	}
      else if(EZ_DnDInfo.targetType == EZ_DND_OBJ_IS_WIDGET)
	{
	  widget = (EZ_Widget *)EZ_DnDInfo.targetItem;
	  if(widget && EZ_LookupWidgetFromAllHT(widget))
	    decoders = EZ_WidgetDnDDataDecoders(widget);
	  else widget = NULL;
	}
      sdecoder = EZ_FindDecoderGivenTarget(decoders, EZ_DND_DROP_FINISH_ATOM);
      if(sdecoder)
	{
	  if(sdecoder)
	      {
		if(sdecoder->decoder)
		  (sdecoder->decoder)(sdecoder->object, sdecoder->data, NULL, 0);
		if((sdecoder->callback).callback)   
		  ((sdecoder->callback).callback)(sdecoder->object,(sdecoder->callback).data);
	      }
	}
    }
  if(item)   EZ_ItemBlocked(item) -= 1;
  if(widget) EZ_WidgetBlocked(widget) -= 1;
}
/****************************************************************************************************/
void  EZ_CallDragInitProcedure()
{
  EZ_DnDDataEncoder *sencoder, *encoders = NULL;

  if(EZ_DnDInfo.srcType == EZ_DND_OBJ_IS_ITEM)
    {
      EZ_Item *item = (EZ_Item *)EZ_DnDInfo.srcItem;
      if(item && EZ_LookupItemFromHT(item)) encoders = EZ_ItemDnDDataEncoders(item);
    }
  else  if(EZ_DnDInfo.srcType == EZ_DND_OBJ_IS_WIDGET)
    {
      EZ_Widget *widget = (EZ_Widget *)EZ_DnDInfo.srcItem;
      if(widget && EZ_LookupWidgetFromAllHT(widget))encoders = EZ_WidgetDnDDataEncoders(widget);
    }
  sencoder = EZ_FindEncoderGivenTarget(encoders, EZ_DND_DRAG_INIT_ATOM);
  if(sencoder)
    {
      int dmLength = 0, dmNeedFree = 0;
      char *dmessage = NULL;
      if(sencoder->encoder)
	(sencoder->encoder)((void *)EZ_DnDInfo.srcItem, sencoder->data, &dmessage,  &dmLength, &dmNeedFree);
      if((sencoder->callback).callback)   
	((sencoder->callback).callback)(sencoder->object,(sencoder->callback).data);
    }
}
/****************************************************************************************************/
void  EZ_SetDragIcon(widget, dpyStyle) EZ_Widget *widget; int dpyStyle;
{
  if(widget && EZ_LookupWidgetFromAllHT(widget) == widget &&
     EZ_WidgetType(widget) == EZ_WIDGET_ITOPLEVEL)
    {
      EZ_DnDInfo.dragIcon = widget;
      EZ_LabelIsDrag(widget) = dpyStyle;
    }
  else fprintf(stderr, "EZ_SetDragIcon: dragIcon must be an ICON widget\n");
}
void EZ_ShowDragIcon(x,y) int x,y;
{
  EZ_Widget *widget = EZ_DnDInfo.dragIcon;
  if(widget && EZ_LookupWidgetFromAllHT(widget) == widget)
    {
      int tx, ty;
      EZ_Bitmap *bitmap = EZ_LabelPixmap(widget);
      int shape = ( bitmap != NULL && bitmap->shape != None);
      if(EZ_XServerHasSHAPEExt && EZ_GetWidgetShapedWinFlag(widget) && shape)
	{
	  int orien, w, h, fillx, filly,xx,yy, padb, bw, bwpadx,bwpady;
	  EZ_ComputeWidgetWindowSize(widget, &w, &h, NULL);
	  orien = EZ_WidgetStacking(widget);
	  EZ_GetWidgetLabelPosition(widget, &fillx, &filly);
	  xx = fillx + EZ_WidgetXOffset(widget);
	  yy = filly + EZ_WidgetYOffset(widget);
	  bw = EZ_WidgetBorderWidth(widget);
	  padb = EZ_WidgetPadB(widget);
	  bwpadx = EZ_WidgetPadX(widget) + bw + padb;
	  bwpady = EZ_WidgetPadY(widget) + bw + padb;

	  switch(orien)
	    {
	    case EZ_HORIZONTAL:
	    case EZ_HORIZONTAL_LEFT:
	    case EZ_HORIZONTAL_RIGHT:
	    case EZ_HORIZONTAL_CENTER:	
	      tx = 2-(xx+bwpadx);
	      ty = 2-(yy+((EZ_WidgetMinHeight(widget) - EZ_LabelPixH(widget)) >> 1));
	      break;
	    case EZ_VERTICAL:
	    case EZ_VERTICAL_TOP:
	    case EZ_VERTICAL_CENTER:
	    case EZ_VERTICAL_BOTTOM:
	    default:
	      tx = 2-(xx + ((EZ_WidgetMinWidth(widget) - EZ_LabelPixW(widget)) >> 1));
	      ty = 2-(yy + bwpady);
	      break;
	    }
	}
      else  tx = ty = 2;
      EZ_WidgetOriginX(widget) = x+tx;
      EZ_WidgetOriginY(widget) = y+ty;
      EZ_ClearWidgetSizeComputedFlag(widget);
      EZ_ReDisplayWidget(widget);     
      EZ_LabelMaskNotDirty(widget) = 1; 
      }
}

void EZ_SetDragCursor(cursor) Cursor cursor;
{
  EZ_DnDInfo.cursor = cursor;
}
/****************************************************************************************************/
void EZ_HandleDnDEvents(widget, event) EZ_Widget *widget; XEvent *event;
{
#include "EZ_DnDHandler1.c"
  if(event->type == ButtonPress)
    { EZ_DnDEventHandler1(widget, event);}
}
/****************************************************************************************************/
#define HASH_SIZE  59
typedef struct eventMask_ {
  Window window;
  long   mask;
  struct eventMask_ *next;
} ForeignEventMaskEntry;

static ForeignEventMaskEntry femhtb[HASH_SIZE];

void EZ_SelectInput(win, mask) Window win; long mask;
{
  if(win != None && EZ_LookupWidgetFromMappedHT(win) == NULL)
    {
      int hash = ((unsigned long)win) % HASH_SIZE;
      ForeignEventMaskEntry *prev = &(femhtb[hash]), *ptr = prev->next;
      int doit = 0;
      while(ptr)
        {
          if(ptr->window == win) 
            {
              if(mask == 0) { prev->next = ptr->next; (void) my_free((char *)ptr);}
              else { doit = (ptr->mask != mask); ptr->mask = mask;}
              break;
            }
          prev = ptr;
          ptr = ptr->next;
        }
      if(mask && ptr == NULL)
        {
          ForeignEventMaskEntry *tmp = (ForeignEventMaskEntry *)
            my_malloc(sizeof(ForeignEventMaskEntry), _MISC_DATA_);
          tmp->window = win;
          tmp->mask = mask;
          tmp->next = femhtb[hash].next;
          femhtb[hash].next = tmp;
          doit = 1;
        }
      if(doit)
        {
          int    (*OldErrorHandler)();
          OldErrorHandler = XSetErrorHandler(EZ_XErrorHandler);
          XSelectInput(EZ_Display, win, mask);
          XSetErrorHandler(OldErrorHandler);            
        }
    }
}

long EZ_GetForeignWindowEventMask(win) Window win;
{
  if(win)
    {
      int hash = ((unsigned long)win) % HASH_SIZE;
      ForeignEventMaskEntry *ptr = femhtb[hash].next;
      while(ptr)
        {
          if(ptr->window == win)  return(ptr->mask);
          ptr = ptr->next;
        }
    }
  return(0L);
}
void EZ_ResetForeignWindowEventMask()
{
  int i;
  for(i = 0; i < HASH_SIZE; i++)
    {
      ForeignEventMaskEntry *ptr = femhtb[i].next;
      while(ptr)
        {
          ptr = ptr->next;
          (void)my_free((char *)ptr);
        }
      femhtb[i].next = NULL;      
    }
}
/****************************************************************************************************/
#undef _EZ_DND_C_
