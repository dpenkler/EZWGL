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
/*******************************************************************
 *  
 *  EZ Widget library. 
 */
#define _EZ_WIDGET_ARRANGE_C_
#include "EZ_Widget.h"
#define  IBefore 1
#define  IAfter  2
#define  ISwap   3
/*******************************************************************
 *
 * Functions implemented in this file:
 */

void                  EZ_InsertWidgetBefore MY_ANSIARGS((EZ_Widget *a, EZ_Widget *b));
void                  EZ_InsertWidgetAfter MY_ANSIARGS((EZ_Widget *a, EZ_Widget *b));
void                  EZ_SwapWidgets MY_ANSIARGS((EZ_Widget *a, EZ_Widget *b));
void                  EZ_DestroyWidgetWindows MY_ANSIARGS((EZ_Widget *widget));
void                  EZ_MakeWidgetWindowExist MY_ANSIARGS((EZ_Widget *widget));
void                  EZ_ReparentWidget  MY_ANSIARGS((EZ_Widget *c, EZ_Widget *p));
void                  EZ_ReparentWidgetXY  MY_ANSIARGS((EZ_Widget *c, EZ_Widget *p, int x, int y));
/*******************************************************************/

static void           EZ_InsertWidgetAB MY_ANSIARGS((EZ_Widget *a, EZ_Widget *b, int where));
static int            AisAncestorOfB MY_ANSIARGS((EZ_Widget *a, EZ_Widget *b));
static void           DestroyWidgetWindowsWork MY_ANSIARGS((EZ_Widget *widget, int level));

/*******************************************************************/
void  EZ_InsertWidgetBefore(a,b)
     EZ_Widget *a,*b;  
{
  EZ_InsertWidgetAB(a,b,IBefore);
}
void  EZ_InsertWidgetAfter(a,b)
     EZ_Widget *a,*b;  
{
  EZ_InsertWidgetAB(a,b,IAfter);
}
void  EZ_SwapWidgets(a,b)
     EZ_Widget *a,*b;  
{
  EZ_InsertWidgetAB(a,b,ISwap);
}
/*******************************************************************
 *
 *  Insert a before/after b or swap a and b..
 */
static void  EZ_InsertWidgetAB(a,b,where)
     EZ_Widget *a,*b;  
     int where;
{
  EZ_Widget *parenta, *parentb, *beforeA, *tmp;
  Window    pwinA, pwinB, winA, winB;
  int illegal = 0, mappeda = 0, mappedb = 0;

  if(a == (EZ_Widget *)NULL ||  b == (EZ_Widget *)NULL || a == b)
    return;

  if(EZ_WidgetType(a) == EZ_WIDGET_NB_PAGE || EZ_WidgetType(b) == EZ_WIDGET_NB_PAGE)
    illegal = 1;
  else
    {
      parenta = EZ_WidgetParent(a);
      parentb = EZ_WidgetParent(b);
      switch(where)
	{
	case IBefore:
	case IAfter:
	  if(parentb == (EZ_Widget *)NULL || AisAncestorOfB(a,b)) 
	    illegal = 1;
	  break;
	case ISwap:
	  if(parenta == (EZ_Widget *)NULL || parentb == (EZ_Widget *)NULL ||
	     (AisAncestorOfB(a,b) | AisAncestorOfB(b,a)))
	    illegal = 1;
	  break;
	default:
	  break;
	}
    }
  if(illegal)
    {
      fprintf(stderr,"In EZ_InsertWidget/EZ_SwapWidget, illegal operation\n");
      return;
    }
  /*
   * Record the windows
   */
  winA = EZ_WidgetWindow(a);
  winB = EZ_WidgetWindow(b);
  mappedb = EZ_WidgetMapped(parentb);
  pwinB = EZ_WidgetWindow(parentb);
  EZ_ClearWidgetSizeComputedFlag(parentb);
  if(parenta)
    {
      if(parenta != parentb)
	{
	  mappeda = EZ_WidgetMapped(parenta);
	  pwinA = EZ_WidgetWindow(parenta);
	  EZ_ClearWidgetSizeComputedFlag(parenta);  
	}
      else pwinA = pwinB;
    }
  else pwinA = (Window)NULL;
  
  /*
   * delete a from its parent's children list
   */
  beforeA = NULL;
  if(parenta)
    {
      tmp = EZ_WidgetChildren(parenta);
      if(tmp == a) { EZ_WidgetChildren(parenta) = EZ_WidgetSibling(tmp);}
      else
	{
	  EZ_Widget *tail = EZ_WidgetSibling(tmp);
	  while(tail != a)
	    {
	      tmp = tail;
	      tail = EZ_WidgetSibling(tail);
	    }
	  EZ_WidgetSibling(tmp) = EZ_WidgetSibling(tail);
	  beforeA = tmp;
	}
      EZ_WidgetSibling(a) = NULL;
    }

  /* update the widget trees */
  switch(where)
    {
    case IBefore:
      if(parenta) EZ_WidgetNumChildren(parenta) -= 1;
      EZ_WidgetNumChildren(parentb) += 1;
      tmp = EZ_WidgetChildren(parentb);
      if(tmp == b)
	{
	  EZ_WidgetSibling(a) = b;
	  EZ_WidgetChildren(parentb) = a;
	}
      else
	{
	  EZ_Widget *tail = EZ_WidgetSibling(tmp);	 
	  while(tail != b)
	    {
	      tmp = tail;
	      tail = EZ_WidgetSibling(tail);
	    }
	  EZ_WidgetSibling(a) = tail;
	  EZ_WidgetSibling(tmp) = a;
	}
      EZ_WidgetParent(a) = parentb;
      break;
    case IAfter:
      if(parenta) EZ_WidgetNumChildren(parenta) -= 1;
      EZ_WidgetNumChildren(parentb) += 1;
      EZ_WidgetSibling(a) = EZ_WidgetSibling(b);
      EZ_WidgetSibling(b) = a;
      EZ_WidgetParent(a) = parentb;
      break;
    case ISwap:
      if(beforeA == b)
	{
	  tmp = EZ_WidgetChildren(parentb);
	  if(tmp == b)
	    {
	      EZ_WidgetSibling(a) = b;
	      EZ_WidgetChildren(parentb) = a;
	    }
	  else
	    {
	      EZ_Widget *tail = EZ_WidgetSibling(tmp);	 
	      while(tail != b)
		{
		  tmp = tail;
		  tail = EZ_WidgetSibling(tail);
		}
	      EZ_WidgetSibling(a) = b;
	      EZ_WidgetSibling(tmp) = a;
	    }
	}
      else
	{
	  tmp = EZ_WidgetChildren(parentb);
	  if(tmp == b)  /* a and b have the same parent */
	    {
	      EZ_WidgetSibling(a) = EZ_WidgetSibling(b);
	      EZ_WidgetChildren(parentb) = a;
	      if(beforeA)
		{
		  EZ_WidgetSibling(b) = EZ_WidgetSibling(beforeA);
		  EZ_WidgetSibling(beforeA) = b;
		}
	      else
		{
		  EZ_WidgetSibling(b) = EZ_WidgetChildren(parenta);
		  EZ_WidgetChildren(parenta) = b;
		}
	    }
	  else
	    {
	      EZ_Widget *tail = EZ_WidgetSibling(tmp);	 
	      while(tail != b)
		{
		  tmp = tail;
		  tail = EZ_WidgetSibling(tail);
		}
	      EZ_WidgetSibling(a) =  EZ_WidgetSibling(tail);
	      EZ_WidgetSibling(tmp) = a;
	      if(beforeA)
		{
		  EZ_WidgetSibling(b) = EZ_WidgetSibling(beforeA);
		  EZ_WidgetSibling(beforeA) = b;
		}
	      else
		{
		  EZ_WidgetSibling(b) = EZ_WidgetChildren(parenta);
		  EZ_WidgetChildren(parenta) = b;
		}
	    }
	}
      EZ_WidgetParent(a) = parentb;
      EZ_WidgetParent(b) = parenta; 
      /* swap gm data */
      {
	int x1 = EZ_WidgetGBX(a);
	int x2 = EZ_WidgetGBY(a);
	int x3 = EZ_WidgetGBW(a);
	int x4 = EZ_WidgetGBH(a);
	EZ_WidgetGBX(a) = EZ_WidgetGBX(b);
	EZ_WidgetGBY(a) = EZ_WidgetGBY(b);
	EZ_WidgetGBW(a) = EZ_WidgetGBY(b);
	EZ_WidgetGBH(a) = EZ_WidgetGBY(b);
	EZ_WidgetGBX(b) = x1;
	EZ_WidgetGBY(b) = x2;
	EZ_WidgetGBW(b) = x3;
	EZ_WidgetGBH(b) = x4;
      }
      break;
    default:
      break;
    }


  /* recompute the geometry of parents */
  {
    int w,h;
    EZ_ComputeWidgetWindowSize(parenta, &w, &h, NULL);
    EZ_ComputeWidgetWindowSize(parentb, &w, &h, NULL);
    EZ_WidgetMaped(a) = 3;  /* cause a move-resize window when redisplayed */
    EZ_WidgetMaped(b) = 3; 
  }

  /* reparenting X windows */
  if(winA) 
    {
      /* a has a window. Make sure parentb has a window also */
      EZ_MakeWidgetWindowExist(parentb);
      if(pwinA != EZ_WidgetWindow(parentb))
	XReparentWindow(EZ_Display,winA,EZ_WidgetWindow(parentb),
			EZ_WidgetOriginX(a), EZ_WidgetOriginY(a)); 
    }
 if(where == ISwap && winB != (Window)NULL)
   {
     EZ_MakeWidgetWindowExist(parenta);
     if(pwinB != EZ_WidgetWindow(parenta))
       XReparentWindow(EZ_Display,winB,EZ_WidgetWindow(parenta),
			EZ_WidgetOriginX(b), EZ_WidgetOriginY(b)); 
   }

  /* finally, redisplay the widgets */
  if(mappedb)
    {
      EZ_ClearWidgetSizeComputedFlag(parentb);
      EZ_DisplayWidget(parentb);
    }
  if(parenta && parenta != parentb)
    {
      EZ_ClearWidgetSizeComputedFlag(parenta);
      if(mappeda) EZ_DisplayWidget(parenta);
    }
}
/******************************************************************************/ 
void EZ_MakeWidgetWindowExist(widget)
     EZ_Widget *widget;
{
  EZ_Widget *root, *parent, *tree[256];
  int i = 0;
  
  if(widget == (EZ_Widget *)NULL || EZ_WidgetWindow(widget) != (Window)NULL) return;
  
  parent = widget;
  while(parent != (EZ_Widget *)NULL)
    {
      root = parent;
      if(EZ_WidgetWindow(root) != (Window)NULL)
	break;
      else tree[i++] = root;
      
      parent = EZ_WidgetParent(root);
    }
  while( --i >= 0)
    EZ_CreateWindowForWidget(tree[i],0);
}
/******************************************************************************/  
static int  AisAncestorOfB(a,b)
     EZ_Widget *a, *b;
{
  if(a && b)
    {
      EZ_Widget *parent = EZ_WidgetParent(b);
      while( parent)
	{
	  if(parent == a) return(1);
	  parent = EZ_WidgetParent(parent);
	}
    }
  return(0);
}
/******************************************************************************/  

void EZ_DestroyWidgetWindows(widget)
     EZ_Widget *widget;
{
  if(widget) DestroyWidgetWindowsWork(widget, 0);
}
static void DestroyWidgetWindowsWork(widget, level)
     EZ_Widget *widget; int level;
{
  if(widget)
    {
      EZ_Widget *children = EZ_WidgetChildren(widget);
      EZ_Widget *sibling = EZ_WidgetSibling(widget);
      Window     wwin = EZ_WidgetWindow(widget);

      if(children) DestroyWidgetWindowsWork(children, 1);
      if(sibling && level) DestroyWidgetWindowsWork(sibling, 1);
      if(wwin)
	{
	  int WType = EZ_WidgetType(widget);
	  if(WType == EZ_WIDGET_EMBEDER || WType == EZ_WIDGET_EXECUTOR)
	    {
	      if(EZ_EmbederConfirmed(widget) > 0)  /* embeding is alive */
		{
		  Window commWin = EZ_EmbederCommuWin(widget);
		  Window eWin = EZ_EmbederWin(widget);
		  /*unsigned long id = EZ_EmbederWidgetAddr(widget);*/
		  
		  EZ_GrabServer(); /* have to grab server */
		  if(EZ_EmbederId(widget) == EZ_VerifyExistence(commWin) &&
		     EZ_WindowExist(eWin))
		    {
		      int w = EZ_WidgetWidth(widget);
		      int h = EZ_WidgetHeight(widget);
		      int x,y;
		      Window child;

		      XTranslateCoordinates(EZ_Display, eWin,
					    RootWindow(EZ_Display, EZ_ScreenNum),
					    0, 0, &x,&y, &child);
		      EZ_SetWMHintForWindow(eWin, x,y,w,h);
		      XReparentWindow(EZ_Display, eWin, 
				      RootWindow(EZ_Display, EZ_ScreenNum),
				      x,y);  /* approximate location */
		    }
		  EZ_UngrabServer(); 
		  EZ_EmbederCommuWin(widget) = 0L;
		  EZ_EmbederWin(widget) = 0L;
		  EZ_EmbederConfirmed(widget) = 0;
		}
	    }
	  EZ_RemoveWidgetFromMappedHT(widget);
	  EZ_RemoveFromDnDWindowList(widget); /* 5-20-97 */
	  EZ_WidgetWindow(widget) = None;
          EZ_WidgetMapped(widget) = 0;
	  XDestroyWindow(EZ_Display,wwin);
	}
    }
}
/********************************************************************************/
void  EZ_ReparentWidget(c, p)
     EZ_Widget *c,*p;  
{
  EZ_ReparentWidgetXY(c,p,10,10);
}

void  EZ_ReparentWidgetXY(c, p, xx, yy)
     EZ_Widget *c,*p;   int xx,yy;
{
  EZ_Widget *parentC, *tmp;
  Window     winC, pwinC;
  int  mappedC = 0, mappedP;

  if(c == (EZ_Widget *)NULL || c == p || (parentC = EZ_WidgetParent(c)) == p)  return;

  if(EZ_WidgetType(c) == EZ_WIDGET_NB_PAGE || (p && AisAncestorOfB(c,p)))
    {
      fprintf(stderr,"In EZ_ReparentWidget, illegal operation\n");
      return;
    }
  /*
   * delete c from its parent's children list
   */
  if(parentC)
    {
      pwinC = EZ_WidgetWindow(parentC);
      mappedC = EZ_WidgetMapped(parentC);
      EZ_ClearWidgetSizeComputedFlag(parentC);
      tmp = EZ_WidgetChildren(parentC);
      if(tmp == c) { EZ_WidgetChildren(parentC) = EZ_WidgetSibling(tmp);}
      else
	{
	  EZ_Widget *tail = EZ_WidgetSibling(tmp);
	  while(tail != c)
	    {
	      tmp = tail;
	      tail = EZ_WidgetSibling(tail);
	    }
	  EZ_WidgetSibling(tmp) = EZ_WidgetSibling(tail);
	}
      EZ_WidgetParent(c) = NULL;
      EZ_WidgetGroupLeader(c) = c;
      EZ_WidgetSibling(c) = NULL;
      EZ_WidgetNumChildren(parentC) -= 1;
    }
  else pwinC = None;
  
  /*
   * insert c at the end of p's children list 
   */
  if(p)
    {
      tmp = EZ_WidgetChildren(p);
      if(tmp == NULL) { EZ_WidgetChildren(p) = c;}
      else
        {
          while(EZ_WidgetSibling(tmp))
            tmp = EZ_WidgetSibling(tmp);
          EZ_WidgetSibling(tmp) = c;
        }
      EZ_WidgetParent(c) = p;
      EZ_WidgetNumChildren(p) += 1;
      EZ_ClearWidgetSizeComputedFlag(p);
    }
  /* reparenting X windows */
  winC = EZ_WidgetWindow(c);
  if(winC) 
    {
      /* c has a window. Make sure p has a window also */
      if(p)
        {
          mappedP = EZ_WidgetMapped(p);  
          EZ_MakeWidgetWindowExist(p);
          EZ_WidgetOriginX(c) = xx; EZ_WidgetOriginY(c)= yy;
          if(pwinC != EZ_WidgetWindow(p))
            XReparentWindow(EZ_Display,winC,EZ_WidgetWindow(p), xx, yy);
          EZ_WidgetMapped(c) = 3;
          if(mappedP) EZ_DisplayWidget(p);
        }
      else
        {
          EZ_DestroyWidgetWindows(c);
          EZ_DisplayWidget(c);
        }
    }
  if(mappedC) EZ_DisplayWidget(parentC);
}
/********************************************************************************/
#undef _EZ_WIDGET_ARRANGE_C_
