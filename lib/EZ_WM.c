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
 
/***********************************************************************
 *
 *               Window Manager Hints
 *
 ***********************************************************************/
#define _EZ_WM_C_
#include "EZ_Widget.h"

void EZ_DeleteWidgetWMHints MY_ANSIARGS((EZ_Widget *widget));
void EZ_SetWidgetWMHints MY_ANSIARGS((EZ_Widget *wt, int opt, int i, int j, void *p));

void EZ_SetWMHintsAndSizeHints MY_ANSIARGS((EZ_Widget *widget,int flag));
void EZ_SetWMSizeHints MY_ANSIARGS((EZ_Widget *widget,int flag));
void EZ_SetWMHintForWindow MY_ANSIARGS((Window win, int x, int y, int w, int h));
int EZ_WidgetStartsAsIcon MY_ANSIARGS((EZ_Widget *w));
/***********************************************************************
 *
 *  If  EZ_WM_*** attributes are set on a toplevel window, a copy of
 *  the following data structure is allocated and attached the 
 *  corresponding widget.  This data structure is private to this
 *  file.
 */
typedef struct EZ_WMHint_ {
  /* name and the sort */
  char       *WM_Name;
  char       *WM_IconName;
  char       *WM_Command;
  /* geometry etc */
  int        initialState;
  int        sizeHintFlags;
  int        x,y, w, h, maxW, maxH, minW, minH;
  /* icon etc */
  int        iconX, iconY;
  EZ_Bitmap  *iconPixmap;  /* May not be an X Bitmap ! */
  Window     iconWindow;
} EZ_WMHints;
/*************************************************************************************/
static void iconEventHandler MY_ANSIARGS((EZ_Widget *w, void *d, int et, XEvent *event));
/*************************************************************************************/
static EZ_WMHints *EZ_AllocateWMHints()
{
  EZ_WMHints *hints = (EZ_WMHints *)my_malloc(sizeof(EZ_WMHints), _WM_HINTS_DATA_);
  if(hints == NULL) EZ_OutOfMemory("EZ_AllocateWMHints");
  memset(hints, 0, sizeof(EZ_WMHints));
  hints->initialState=NormalState;
  return(hints);
}
/***********************************************************************/
void EZ_DeleteWidgetWMHints(widget)
     EZ_Widget *widget;
{
  if(widget)
    {
      EZ_WMHints *hints = (EZ_WMHints *)EZ_WidgetWMHints(widget);
      if(hints)
	{
	  EZ_WidgetWMHints(widget) = NULL;

	  if(hints->WM_Name)     (void)my_free((char *)hints->WM_Name);
	  if(hints->WM_IconName) (void)my_free((char *)hints->WM_IconName);
	  if(hints->iconPixmap)  (void)EZ_FreeBitmap(hints->iconPixmap);
	  if(hints->iconWindow)
	    {
	      EZ_CallBackStruct **funcList=(EZ_CallBackStruct **)&EZ_NoWidgetEventHandlers;
	      EZ_RemoveCallBackWork(funcList, (EZ_CallBack)iconEventHandler, widget);
	      XDestroyWindow(EZ_Display, hints->iconWindow);
	    }
	  (void)my_free((char *)hints);
	}
    }
}
/***********************************************************************/
void EZ_SetWidgetWMHints(widget,option, i1,i2,p1)
     EZ_Widget *widget; int option, i1, i2; void *p1;
{
  if(widget && EZ_WidgetParent(widget) == NULL)
    {
      EZ_WMHints *hints = (EZ_WMHints *)EZ_WidgetWMHints(widget);
      if(hints == NULL)	
	{ 
	  hints = EZ_AllocateWMHints();
	  EZ_WidgetWMHints(widget) = (void *)hints;
	}

      switch(option)
	{
	case EZ_WM_INITIAL_STATE:
	  hints->initialState = i1;
	  break;
	case EZ_WMHINTS: 
	  hints->sizeHintFlags |= i1;
	  break;
	case EZ_WM_WINDOW_NAME:
	  if(p1) hints->WM_Name = EZ_ReallocCopyString(hints->WM_Name, p1);
	  break;
	case EZ_WM_ICON_NAME:
	  if(p1) hints->WM_IconName = EZ_ReallocCopyString(hints->WM_IconName, p1);
	  break;
	case EZ_WM_ICON_XPM_FILE:
	  if(p1)
	    {
	      char *file = (char *)p1;
	      EZ_Bitmap *bitmap = (file == (char *)NULL? (EZ_Bitmap *)NULL:
				   EZ_GetBitmapFromFile(file, 1));
	      if(!bitmap) bitmap =  EZ_GetImageFromFile(file, 1);
	      hints->iconPixmap = bitmap;
	    }
	  break;
	case EZ_WM_ICON_PIXMAP:
	  {
	    EZ_Bitmap *bitmap = (EZ_Bitmap *)p1;
	    if(p1) bitmap->ref++;
	    if(hints->iconPixmap) EZ_FreeBitmap(hints->iconPixmap);
	    hints->iconPixmap = bitmap;
	  }
	break;
	case EZ_WM_ICON_LOCATION:
	  hints->iconX = i1; hints->iconY = i2;
	  break;
	case EZ_WM_LOCATION_HINT:
	  hints->x = i1; hints->y = i2; 
	  hints->sizeHintFlags |= PPosition;
	  break;
	case EZ_WM_SIZE_HINT:
	  hints->w = i1; hints->h = i2;
	  hints->sizeHintFlags |= PSize;
	  break;	  
	case EZ_WM_MAX_SIZE_HINT:
	  hints->maxW = i1; hints->maxH = i2;
	  hints->sizeHintFlags |= PMaxSize;	  
	  break;
	case EZ_WM_MIN_SIZE_HINT:
	  hints->minW = i1; hints->minH = i2;
	  hints->sizeHintFlags |= PMinSize;	  
	  break;
	default:
	  break;
	}
      /* if widget window has been created, update immediately */
      if(EZ_WidgetWindow(widget) != None)
	EZ_SetWMHintsAndSizeHints(widget, 0);
    }
}
/***********************************************************************/
#ifndef ABSV 
#define ABSV(a) ((a) > 0? (a) : -(a))
#endif
void EZ_SetWMHintsAndSizeHints(widget, flag )
     EZ_Widget *widget;
     int flag;  /* whether or not to set the USPosition Hints */
{
  if(EZ_WindowExist(EZ_WidgetWindow(widget)))
    {
      XWMHints      *WMHints = XAllocWMHints();       /*  WM Hints. */
      EZ_WMHints    *hints = (EZ_WMHints *)EZ_WidgetWMHints(widget);
      char          *wmName = NULL, *iconName = NULL;
      XTextProperty textProp;

      /* initial state */
      if(flag & 256) WMHints->initial_state = IconicState; 
      else WMHints->initial_state = NormalState;
      /* always accept input */
      WMHints->input = True;
      /* dummyWindow is always the leader */
      WMHints->window_group = EZ_DummyWindow;

      WMHints->flags = StateHint  | InputHint | WindowGroupHint |StateHint;
      
      /* figure out icon hints */
      if(hints)
	{
	  if(WMHints->initial_state == NormalState && hints->initialState == IconicState)
	    WMHints->initial_state =  IconicState;
	  if(hints->WM_Name) wmName = hints->WM_Name;
	  if(hints->WM_IconName) iconName = hints->WM_IconName;	  
	  if(hints->iconX | hints->iconY)
	    {
	      WMHints->icon_x = hints->iconX;  WMHints->icon_y = hints->iconY;
	      WMHints->flags |= IconPositionHint;
	    }
	  if(hints->iconPixmap)
	    {
	      if(hints->iconWindow == None)
		{
		  int       w = hints->iconPixmap->width;
		  int       h = hints->iconPixmap->height;	      
		  int       status, i, count = 0;
		  int       w_set = w, h_set = h, diff=987654;
		  XIconSize *sizeList = NULL;
		  Display   *dpy = EZ_GetDisplay();
		  status = XGetIconSizes(dpy, DefaultRootWindow(dpy), &sizeList, &count);
		  if(status)
		    {
		      for(i = 0; i < count; i++)
			{
			  int mnw = sizeList[i].min_width;
			  int mnh = sizeList[i].min_height;
			  int mxw = sizeList[i].max_width;
			  int mxh = sizeList[i].max_height;
			  int winc = sizeList[i].width_inc;
			  int hinc = sizeList[i].height_inc;
			  int wtmp, htmp, dtmp;
			  if(w > mxw) wtmp = mxw;
			  else if(w < mnh) wtmp = mnh;
			  else
			    {
			      if(winc == 1) wtmp = w;
			      else wtmp = ((w - mnw + winc -1)/winc)*winc + mnw;
			    }
			  if(h > mxh) htmp = mxh;
			  else if(h < mnh) htmp = mnh;
			  else
			    {
			      if(hinc == 1) htmp = h;
			      else htmp = ((h - mnh + hinc -1)/hinc)*hinc + mnh;
			    }
			  dtmp = ABSV(w-wtmp) + ABSV(h-htmp);
			  if(dtmp < diff) {w_set = wtmp; h_set=htmp; diff = dtmp;}
			  if(dtmp == 0) break;
			}
		      if(sizeList) XFree((char *)sizeList);
		    }
		  /* now create icon window */
		  {
		    XSetWindowAttributes   setWAttr;
		    unsigned long          valuemask = 0;		    
		    setWAttr.colormap          = DefaultColormap(EZ_Display, EZ_ScreenNum);
		    setWAttr.background_pixel  = BlackPixel(EZ_Display, EZ_ScreenNum);
		    setWAttr.background_pixmap = None;
		    valuemask = CWBackPixel|CWColormap|CWBackPixmap;
		    
		    hints->iconWindow = 
		      XCreateWindow(EZ_Display, RootWindow(EZ_Display, EZ_ScreenNum),
				    hints->iconX, hints->iconY, w_set, h_set, /*geom */
				    0, DefaultDepth(EZ_Display, EZ_ScreenNum),
				    InputOutput,
				    DefaultVisual(EZ_Display, EZ_ScreenNum),
				    valuemask,
				    &setWAttr);
		    XSelectInput(EZ_Display, hints->iconWindow, ExposureMask);
		    if(EZ_XServerHasSHAPEExt)
		      {
			EZ_Bitmap *bitmap = hints->iconPixmap;
			if(bitmap->shape)
			  XShapeCombineMask(EZ_Display,  hints->iconWindow,
					    ShapeBounding, 0, 0,
					    bitmap->shape, ShapeSet);
		      }
		    /* add a NoWidget event handler */
		    {
		      EZ_CallBackStruct **funcList=(EZ_CallBackStruct **)&EZ_NoWidgetEventHandlers;
		      EZ_AddCallBackWork(funcList, (EZ_CallBack)iconEventHandler,(void *)widget, 0, 1);
		    }
		  }
		}
	    }
	  if(hints->iconWindow)
	    {
	      WMHints->icon_window = hints->iconWindow;
	      WMHints->flags |= IconWindowHint;
	    }
	}
      /* set WM_NAME */
      if(wmName == NULL) wmName = EZ_GetApplicationName();
      if( XStringListToTextProperty(&wmName, 1, &textProp))
	{
	  XSetWMName(EZ_Display, EZ_WidgetWindow(widget), &textProp);
	  XFree((char *)textProp.value);
        }
      /* set WM_ICON_NAME */
      if(iconName == NULL) iconName = wmName;
      XSetIconName(EZ_Display, EZ_WidgetWindow(widget), iconName);
      
      /* set normal hints */
      XSetWMHints(EZ_Display,EZ_WidgetWindow(widget),WMHints);
      XFree((char *)WMHints);
      /* set size hints */
      EZ_SetWMSizeHints(widget, flag);
    }
}
/************************************************************************/
void  EZ_SetWMSizeHints(widget, flag)
     EZ_Widget *widget; int flag;
{
  if(EZ_WindowExist(EZ_WidgetWindow(widget)))
    {
      XSizeHints   *sizeHints = XAllocSizeHints(); /* Size Hints.*/
      EZ_WMHints   *hints = (EZ_WMHints *)EZ_WidgetWMHints(widget);
      sizeHints->x = EZ_WidgetOriginX(widget);
      sizeHints->y = EZ_WidgetOriginY(widget);
      sizeHints->width = EZ_WidgetWidth(widget);
      sizeHints->height = EZ_WidgetHeight(widget);
      sizeHints->width_inc = 4;
      sizeHints->height_inc = 4;
      sizeHints->flags = PPosition|PSize|PResizeInc|USSize;
      if((flag & 1))  sizeHints->flags  |= USPosition;

      if(hints)
	 {
	   int flags = hints->sizeHintFlags;
	   if(flags &  PMinSize)
	     { 
	       if(hints->minW > 0)
		 {
		   sizeHints->min_width  = hints->minW;
		   sizeHints->min_height  = hints->minH;
		 }
	       else
		 {
		   sizeHints->min_width  = EZ_WidgetWidth(widget);
		   sizeHints->min_height = EZ_WidgetHeight(widget);
		 }
	     }
	   if(flags &  PMaxSize)
	     { 
	       if(hints->maxW > 0)
		 {
		   sizeHints->max_width  = hints->maxW;
		   sizeHints->max_height  = hints->maxH;
		 }
	       else
		 {
		   sizeHints->max_width  = EZ_WidgetWidth(widget);
		   sizeHints->max_height = EZ_WidgetHeight(widget);
		 }
	     }
	   if(flags &  PWinGravity) sizeHints->win_gravity  = CenterGravity;
	   sizeHints->flags |= flags;
	 }
      XSetWMNormalHints(EZ_Display,EZ_WidgetWindow(widget),sizeHints);
      XFree((char *)sizeHints);
    }
}
/***********************************************************************/
void  EZ_SetWMHintForWindow(window, x,y,w,h)
     Window window;
     int x,y,w,h;
{
  if(EZ_WindowExist(window))
    {
      XWMHints   *WMHints = XAllocWMHints();       /*  WM Hints. */
      XSizeHints   *sizeHints = XAllocSizeHints(); /* Size Hints.*/

      WMHints->initial_state = NormalState;
      WMHints->input = True;
      WMHints->flags = StateHint | InputHint ;
      
      XSetWMHints(EZ_Display,window,WMHints);
      XFree(WMHints);
	    
      sizeHints->x = x;
      sizeHints->y = y;
      sizeHints->width = w;
      sizeHints->height = h;
      sizeHints->width_inc = 4;
      sizeHints->height_inc = 4;
      sizeHints->flags = PPosition|PSize|PResizeInc|USSize|USPosition;
      XSetWMNormalHints(EZ_Display,window,sizeHints);
      XFree(sizeHints);
    }
}
/***********************************************************************/
static void iconEventHandler(nullW, data, zeroI, event)
     EZ_Widget *nullW;  /* always NULL */
     void      *data;   /* this is the widget */
     int       zeroI;   /* always 0 */
     XEvent    *event;
{
  if(event->type == Expose)
    {
      EZ_Widget *widget = (EZ_Widget *)data;
      Window win = event->xexpose.window;

      if(EZ_LookupWidgetFromAllHT(widget))
	{
	  EZ_WMHints *hints = (EZ_WMHints *)EZ_WidgetWMHints(widget);
	  if(hints && hints->iconWindow == win && hints->iconPixmap)
	    {
	      GC gc = DefaultGC(EZ_Display, EZ_ScreenNum);
	      EZ_Bitmap *bitmap = hints->iconPixmap;
	      
	      if(EZ_BitmapType(bitmap) == EZ_BITMAP_IS_BITMAP)
		XCopyPlane(EZ_Display, bitmap->pixmap, win, gc, 0, 0,
			   (unsigned int)(bitmap->width),
			   (unsigned int)(bitmap->height), 
			   0, 0, 1);
	      else
		XCopyArea(EZ_Display, bitmap->pixmap, win, gc, 0, 0,
			  (unsigned int)(bitmap->width),
			  (unsigned int)(bitmap->height),
			  0, 0);
	      return;
	    }
	}
    }
}
/***********************************************************************/
int EZ_WidgetStartsAsIcon(widget) EZ_Widget *widget;
{
  if(widget && EZ_WidgetParent(widget) == NULL)
    {
      EZ_WMHints *hints = (EZ_WMHints *)EZ_WidgetWMHints(widget);
      if(hints) return((hints->initialState == IconicState));
    }
  return(0);
}
/***********************************************************************/
#undef _EZ_WM_C_
