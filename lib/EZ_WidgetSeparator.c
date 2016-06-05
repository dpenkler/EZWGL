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
 ***       Separator                                           ***
 ***                                                           ***
 *****************************************************************/
#define _EZ_WIDGET_SEPARATOR_C_

#include "EZ_Widget.h"

/*********************************************************************
 * 
 *  Functions implemented in this file:
 */
EZ_Widget       *EZ_CreateSeparator MY_ANSIARGS((EZ_Widget *parent));
void            EZ_DrawSeparator MY_ANSIARGS((EZ_Widget *widget));
void            EZ_ComputeSeparatorSize MY_ANSIARGS((EZ_Widget *widget, int *w, int *h));
static void     fixOrientation  MY_ANSIARGS((EZ_Widget *widget));
/*********************************************************************
 * 
 *  Local Variables.
 */

static EZ_WidgetHandle EZ_SeparatorHandle =
{ 
  EZ_ComputeSeparatorSize,
  EZ_DrawSeparator,
  EZ_FreeUnknownData,
  EZ_UnkownEventHandle,
};

/*********************************************************************/
EZ_Widget  *EZ_CreateSeparator(parent)
     EZ_Widget *parent;
{
  EZ_Widget  *tmp;

  if(parent == (EZ_Widget *)NULL) return(NULL);

  tmp = EZ_CreateNewWidget(parent);
  EZ_WidgetHandlingFunctions[EZ_WIDGET_SEPARATOR] = &EZ_SeparatorHandle;

  EZ_SetWidgetTypeAndNames(tmp, EZ_WIDGET_SEPARATOR);
  EZ_SetWidgetTransientFlag(tmp);
  EZ_WidgetPadX(tmp) = 0;
  EZ_WidgetPadY(tmp) = 0;
  EZ_SeparatorWidth(tmp)  = 3;
  EZ_SeparatorStyle(tmp)  = EZ_BORDER_ENGRAVED;
  EZ_SetDoNotPropagateFlag(tmp);

  return(tmp);
}
/****************************************************************************/
void  EZ_ComputeSeparatorSize(widget, w,h)
     EZ_Widget *widget;
     int             *w, *h;
{
  int orien, style;
  fixOrientation(widget);
  orien = EZ_WidgetStacking(widget); /* orientation */
  style = EZ_SeparatorStyle(widget); 
  if(!(EZ_GetWidgetSizeComputedFlag(widget)))
    {
      EZ_SetWidgetSizeComputedFlag(widget);
      switch(style)
        {
        case EZ_UP:  
        case EZ_BORDER_UP:  
          style = EZ_BORDER_FRAMED_UP;   
          break;
        case EZ_DOWN: 
        case EZ_BORDER_DOWN: 
          style = EZ_BORDER_FRAMED_DOWN;
          break;
        case EZ_BORDER_FRAMED_UP: 
        case EZ_BORDER_FRAMED_DOWN:
        case EZ_BORDER_CARVED:
        case EZ_BORDER_EMBOSSED:
        case EZ_BORDER_FLAT:
          break;
        default:  style = EZ_BORDER_FRAMED_DOWN; 
          break;
        }
      EZ_SeparatorStyle(widget) = style; 
    }      
  EZ_WidgetBorderWidth(widget) = 0; /* no border */

  if(orien >= EZ_HORIZONTAL && orien <= EZ_HORIZONTAL_CENTER)
    {
      *w = 32;
      *h = EZ_SeparatorWidth(widget);
    }
  else
    {
      *w = EZ_SeparatorWidth(widget);
      *h = 32;
    }
}
/****************************************************************************/
void EZ_DrawSeparator(wptr)
     EZ_Widget *wptr;
{
  int      x,y, ox, oy, w, h, w2, h2, tile;
  Pixmap   pixmap;
  Window   win;
  int      style = EZ_SeparatorStyle(wptr);
  int      bw =  EZ_SeparatorWidth(wptr);
  int      orien = EZ_WidgetStacking(wptr);
  unsigned long bgpv;
  GC       gc;

  fixOrientation(wptr);
  win = EZ_WidgetWindow(wptr);
  w   = EZ_WidgetWidth(wptr);
  h   = EZ_WidgetHeight(wptr);
  pixmap = XCreatePixmap(EZ_Display, win, w, h, EZ_Depth); 

  if((tile= EZ_GetBackgroundGC(wptr, &gc,&bgpv, 1, 0)))
    {
      EZ_GetWidgetTileOrigin(wptr, &ox, &oy);      
      XSetTSOrigin(EZ_Display, gc, ox, oy);
    }
  XFillRectangle(EZ_Display, pixmap, gc, 0,0, w, h); 

  if(orien >= EZ_HORIZONTAL && orien <= EZ_HORIZONTAL_CENTER)
    { 
      w2 = w + bw + bw + 4; 
      h2 = h;
      x = -bw-2;
      y = 0;
    }
  else
    {
      w2 = w;
      h2 = h + bw + bw + 4;
      x = 0;
      y = -bw -2;
    }
  EZ_DrawRectBorderWithSize(wptr, pixmap, x,y,w2,h2,bw, style, 0, 0, 0);
  XCopyArea(EZ_Display,pixmap,win, EZ_WRITABLEGC,0,0,w,h,0,0); 
  XFreePixmap(EZ_Display, pixmap);                             
} 
/****************************************************************************/
static void fixOrientation(widget)
     EZ_Widget *widget;
{
  EZ_Widget   *parent= EZ_WidgetParent(widget);
  EZ_GManager *gmanager = EZ_WidgetGManager(parent);
  if(parent && gmanager == NULL)
    {
      int orien = EZ_WidgetStacking(parent);
      if(orien >= EZ_HORIZONTAL && orien <= EZ_HORIZONTAL_CENTER)
        {
          EZ_ClearWidgetHeightSetFlag(widget);
          EZ_SetWidgetWidthSetFlag(widget);
          EZ_WidgetStacking(widget) = EZ_VERTICAL;
        }
      else 
        {
          EZ_ClearWidgetWidthSetFlag(widget);
          EZ_SetWidgetHeightSetFlag(widget);
          EZ_WidgetStacking(widget) = EZ_HORIZONTAL;
        }
    }
}
/****************************************************************************/
#undef _EZ_WIDGET_SEPARATOR_C_
