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
 ***              Draw Arrows                                  ***
 ***                                                           ***
 *****************************************************************/
#define _EZ_ARROW_C_
#include "EZ_Widget.h"

/*********************************************************************
 * 
 *  Functions implemented in this file:
 */
void  EZ_DrawHLArrow MY_ANSIARGS((EZ_Widget *widget, 
				  Drawable drawable,
				  int x, int y, int size, 
				  int borderstyle,
				  int borderwidth,
				  int highlight));
void  EZ_DrawHRArrow MY_ANSIARGS((EZ_Widget *widget, 
				  Drawable drawable,
				  int x, int y, int size, 
				  int borderstyle,
				  int borderwidth,
				  int highlight));
void  EZ_DrawVUArrow MY_ANSIARGS((EZ_Widget *widget, 
				  Drawable drawable,
				  int x, int y, int size, 
				  int borderstyle,
				  int borderwidth,
				  int highlight));
void  EZ_DrawVDArrow MY_ANSIARGS((EZ_Widget *widget, 
				  Drawable drawable,
				  int x, int y, int size, 
				  int borderstyle,
				  int borderwidth,
				  int highlight));

void  EZ_DrawArrow4 MY_ANSIARGS((int type, EZ_Widget *widget, 
				  Drawable drawable,
				  int x, int y, int size, 
				  int borderstyle,
				  int borderwidth,
				  int highlight));

/***************************************************************************/
void  EZ_DrawArrow4(type,widget,drawable,x,y,size,borderstyle, borderwidth,highlight)
     int           type;
     EZ_Widget     *widget;
     Drawable      drawable;     /* place to draw  */
     int           x,y;          /* up left corner */
     int           size;         /* size in pixels, including border */
     int           borderstyle;  /* type of border */
     int           borderwidth;  /* border width   */
     int           highlight;    /* highlight ?    */
{
  switch(type)
    {
    case EZ_LEFT:
      EZ_DrawHLArrow(widget,drawable,x,y,size,borderstyle, borderwidth,highlight);
      break;
    case EZ_RIGHT:
      EZ_DrawHRArrow(widget,drawable,x,y,size,borderstyle, borderwidth,highlight);
      break;
    case EZ_UP:
      EZ_DrawVUArrow(widget,drawable,x,y,size,borderstyle, borderwidth,highlight);
        break;
    case EZ_DOWN:
      EZ_DrawVDArrow(widget,drawable,x,y,size,borderstyle, borderwidth,highlight);
        break;
    default:
      break;
    }
}
/***************************************************************************/

void  EZ_DrawHRArrow(widget,drawable,x,y,size,borderstyle, borderwidth,highlight)
     EZ_Widget     *widget;
     Drawable      drawable;     /* place to draw  */
     int           x,y;          /* up left corner */
     int           size;         /* size in pixels, including border */
     int           borderstyle;  /* type of border */
     int           borderwidth;  /* border width   */
     int           highlight;    /* highlight ?    */
{
  XPoint base[3], left[4], top[4], bottom[4];
  GC   gcleft, gctop, gcbottom, othergc;
  int  yoffset, xoffset;
  unsigned long bgpv;

  xoffset = (borderwidth * 17)>>3;
  yoffset = (borderwidth *50)>>5;

  base[0].x = (short)x;
  base[0].y = (short)y;
  base[1].x = (short) (x + size);
  base[1].y = (short) (y + (size>>1));
  base[2].x = (short)x;
  base[2].y = (short)(y+size);
  
  left[0].x =  base[0].x;
  left[0].y =  base[0].y;
  left[1].x = (short)(x + borderwidth);  
  left[1].y = (short)(y + yoffset);
  left[2].x = (short)(x + borderwidth);  
  left[2].y = (short)(y + size - yoffset-1);
  left[3].x =  base[2].x;
  left[3].y =  base[2].y;

  top[0].x = left[0].x;
  top[0].y = left[0].y;
  top[1].x = left[1].x;
  top[1].y = left[1].y;
  top[2].x = (short)(x + size - xoffset);
  top[2].y = base[1].y;
  top[3].x = base[1].x;
  top[3].y = base[1].y;

  bottom[0].x = base[1].x;
  bottom[0].y = base[1].y;
  bottom[1].x = top[2].x;
  bottom[1].y = base[1].y;
  bottom[2].x = left[1].x;
  bottom[2].y = (short)(y + size - yoffset);
  bottom[3].x = (short)x;
  bottom[3].y = (short)(y + size);

  if(EZ_WidgetBGPixmapB(widget) != NULL)
    {
      EZ_Bitmap *bitmap = EZ_WidgetBGPixmapB(widget);
      othergc = EZ_NTILEGC2;
      if(highlight) XSetTile(EZ_Display,EZ_NTILEGC2,bitmap->htpixmap);
      else XSetTile(EZ_Display,EZ_NTILEGC2,bitmap->pixmap);
      XSetTSOrigin(EZ_Display, EZ_NTILEGC2, 0, 0);
    }
  else if(EZ_WidgetParentBGPixmapB(widget) != NULL)
    {
      EZ_Bitmap *bitmap = EZ_WidgetParentBGPixmapB(widget);
      othergc = EZ_NTILEGC2;
      if(highlight) XSetTile(EZ_Display,EZ_NTILEGC2,bitmap->htpixmap);
      else XSetTile(EZ_Display,EZ_NTILEGC2,bitmap->pixmap);
      XSetTSOrigin(EZ_Display, EZ_NTILEGC2, 0, 0);
    }
  else
    {
      int tile = EZ_GetBackgroundGC(widget,&othergc, &bgpv, 0, highlight);
      if(tile)
        {
          int ox, oy;
          EZ_GetWidgetTileOrigin(widget, &ox, &oy);
          XSetTSOrigin(EZ_Display, othergc, ox, oy);
        }
    }
  XFillPolygon(EZ_DisplayForWidgets, drawable,othergc,
	       base, 3, Convex, CoordModeOrigin);

  if(borderstyle == EZ_BORDER_UP)
    EZ_SetupUpBdGC(widget, &gctop, &gcleft,&othergc,&othergc,&othergc,&gcbottom,0,0,1);
  else
    EZ_SetupUpBdGC(widget, &gcbottom, &othergc, &othergc,&othergc,&gcleft,&gctop,0,0,1);
  XFillPolygon(EZ_DisplayForWidgets, drawable, gctop,
	       top, 4, Convex, CoordModeOrigin);
  XFillPolygon(EZ_DisplayForWidgets, drawable, gcleft,
	       left, 4, Convex, CoordModeOrigin);
  XFillPolygon(EZ_DisplayForWidgets, drawable, gcbottom,
	       bottom, 4, Convex, CoordModeOrigin);
}

/***************************************************************************/

void EZ_DrawHLArrow(widget,drawable,x,y,size,borderstyle, borderwidth,highlight)
     EZ_Widget     *widget;
     Drawable      drawable;     /* place to draw  */
     int           x,y;          /* up left corner */
     int           size;         /* size in pixels, including border */
     int           borderstyle;  /* type of border */
     int           borderwidth;  /* border width   */
     int           highlight;    /* highlight ?    */
{
  XPoint base[3], right[4], top[4], bottom[4];
  GC   gcright, gctop, gcbottom, othergc;
  int  yoffset, xoffset;
  unsigned long bgpv;

  x--; size++; /* compensate */

  xoffset = (borderwidth * 17)>>3;
  yoffset = (borderwidth *50)>>5;

  base[0].x = (short)x;
  base[0].y = (short)(y + (size>>1));
  base[1].x = (short) (x + size);
  base[1].y = (short)y;
  base[2].x = (short)(x + size);
  base[2].y = (short)(y + size);
  
  right[0].x = base[1].x;
  right[0].y = base[1].y;
  right[1].x = (short)(x + size - borderwidth);  
  right[1].y = (short)(y + yoffset);
  right[2].x = right[1].x;
  right[2].y = (short)(y + size - yoffset);
  right[3].x = base[2].x;
  right[3].y = base[2].y;

  top[0].x = base[1].x;
  top[0].y = base[1].y;
  top[1].x = right[1].x;
  top[1].y = right[1].y;
  top[2].x = (short)(x + xoffset);
  top[2].y = base[0].y;
  top[3].x = base[0].x;
  top[3].y = base[0].y;

  bottom[0].x = base[2].x;
  bottom[0].y = base[2].y;
  bottom[1].x = right[2].x;
  bottom[1].y = right[2].y;
  bottom[2].x = top[2].x;
  bottom[2].y = base[0].y;
  bottom[3].x = base[0].x;
  bottom[3].y = base[0].y;

  if(EZ_WidgetBGPixmapB(widget) != NULL)
    {
      EZ_Bitmap *bitmap = EZ_WidgetBGPixmapB(widget);
      othergc = EZ_NTILEGC2;
      if(highlight) XSetTile(EZ_Display,EZ_NTILEGC2,bitmap->htpixmap);
      else XSetTile(EZ_Display,EZ_NTILEGC2,bitmap->pixmap);
      XSetTSOrigin(EZ_Display, EZ_NTILEGC2, 0, 0);
    }
  else if(EZ_WidgetParentBGPixmapB(widget) != NULL)
    {
      EZ_Bitmap *bitmap = EZ_WidgetParentBGPixmapB(widget);
      othergc = EZ_NTILEGC2;
      if(highlight) XSetTile(EZ_Display,EZ_NTILEGC2,bitmap->htpixmap);
      else XSetTile(EZ_Display,EZ_NTILEGC2,bitmap->pixmap);
      XSetTSOrigin(EZ_Display, EZ_NTILEGC2, 0, 0);
    }
  else
    {
      int tile = EZ_GetBackgroundGC(widget,&othergc, &bgpv, 0, highlight);
      if(tile) 
        { 
          int ox, oy;
          EZ_GetWidgetTileOrigin(widget, &ox, &oy);
          XSetTSOrigin(EZ_Display, othergc, ox, oy);
        }
    }
  XFillPolygon(EZ_DisplayForWidgets, drawable, othergc,
	       base, 3, Convex, CoordModeOrigin);

  if(borderstyle == EZ_BORDER_UP)
    EZ_SetupUpBdGC(widget, &othergc, &gctop,&othergc,&othergc,&gcbottom,&gcright,0,0,1);
  else
    EZ_SetupUpBdGC(widget, &gcright, &gcbottom,&othergc,&othergc,&othergc,&gctop,0,0,1);
  XFillPolygon(EZ_DisplayForWidgets, drawable, gcright,
	       right, 4, Convex, CoordModeOrigin);
  XFillPolygon(EZ_DisplayForWidgets, drawable, gcbottom,
	       bottom, 4, Convex, CoordModeOrigin);
  XFillPolygon(EZ_DisplayForWidgets, drawable, gctop,
	       top, 4, Convex, CoordModeOrigin);

}

/***************************************************************************/

void EZ_DrawVUArrow(widget, drawable,x,y,size,borderstyle, borderwidth,highlight)
     EZ_Widget     *widget;
     Drawable      drawable;     /* place to draw  */
     int           x,y;          /* up left corner */
     int           size;         /* size in pixels, including border */
     int           borderstyle;  /* type of border */
     int           borderwidth;  /* border width   */
     int           highlight;    /* highlight ?    */
{
  XPoint base[3], right[4], left[4], bottom[4];
  GC   gcright, gcleft, gcbottom, othergc;
  int  yoffset, xoffset;
  unsigned long bgpv;

  y--; size++; /* compensate */
  xoffset = (borderwidth * 17)>>3;  /* x and y are switched */
  yoffset = (borderwidth * 50)>>5;

  base[0].x = (short)(x + (size >>1));
  base[0].y = (short)y;
  base[1].x = (short)x;
  base[1].y = (short)(y+size);
  base[2].x = (short)(x + size);
  base[2].y = (short)(y + size);

  bottom[0].x = base[1].x-1;
  bottom[0].y = base[1].y;
  bottom[1].x = (short)(x + yoffset);
  bottom[1].y = (short)(y + size - borderwidth);
  bottom[2].x = (short)(x + size - yoffset);
  bottom[2].y = (short)(y + size - borderwidth);
  bottom[3].x = base[2].x;
  bottom[3].y = base[2].y;
  
  right[0].x = base[0].x;
  right[0].y = base[0].y;
  right[1].x = base[0].x;
  right[1].y = (short)(y + xoffset)-1;
  right[2].x = (short)(x + size - yoffset);  
  right[2].y = bottom[1].y;
  right[3].x = base[2].x;
  right[3].y = base[2].y;

  left[0].x = base[0].x;
  left[0].y = base[0].y;
  left[1].x = right[1].x;
  left[1].y = right[1].y;
  left[2].x = bottom[1].x;
  left[2].y = bottom[1].y;
  left[3].x = base[1].x;
  left[3].y = base[1].y;

  if(EZ_WidgetBGPixmapB(widget) != NULL)
    {
      EZ_Bitmap *bitmap = EZ_WidgetBGPixmapB(widget);
      othergc = EZ_NTILEGC2;
      if(highlight) XSetTile(EZ_Display,EZ_NTILEGC2,bitmap->htpixmap);
      else XSetTile(EZ_Display,EZ_NTILEGC2,bitmap->pixmap);
      XSetTSOrigin(EZ_Display, EZ_NTILEGC2, 0, 0);
    }
  else if(EZ_WidgetParentBGPixmapB(widget) != NULL)
    {
      EZ_Bitmap *bitmap = EZ_WidgetParentBGPixmapB(widget);
      othergc = EZ_NTILEGC2;
      if(highlight) XSetTile(EZ_Display,EZ_NTILEGC2,bitmap->htpixmap);
      else XSetTile(EZ_Display,EZ_NTILEGC2,bitmap->pixmap);
      XSetTSOrigin(EZ_Display, EZ_NTILEGC2, 0, 0);
    }
  else
    {
      int tile = EZ_GetBackgroundGC(widget,&othergc, &bgpv, 0, highlight);
      if(tile) 
        { 
          int ox, oy;
          EZ_GetWidgetTileOrigin(widget, &ox, &oy);
          XSetTSOrigin(EZ_Display, othergc, ox, oy);
        }
    }  


  XFillPolygon(EZ_DisplayForWidgets, drawable, othergc,
	       base, 3, Convex, CoordModeOrigin);

  if(borderstyle == EZ_BORDER_UP)
    EZ_SetupUpBdGC(widget, &othergc, &gcleft,&othergc,&othergc,&gcbottom,&gcright,0,0,1);
  else
    EZ_SetupUpBdGC(widget, &gcright, &gcbottom,&othergc,&othergc,&othergc,&gcleft,0,0,1);
  XFillPolygon(EZ_DisplayForWidgets, drawable, gcright,
	       right, 4, Convex, CoordModeOrigin);
  XFillPolygon(EZ_DisplayForWidgets, drawable, gcbottom,
	       bottom, 4, Convex, CoordModeOrigin);
  XFillPolygon(EZ_DisplayForWidgets, drawable, gcleft,
	       left, 4, Convex, CoordModeOrigin);
}

/***************************************************************************/

void EZ_DrawVDArrow(widget, drawable,x,y,size,borderstyle, borderwidth,highlight)
     EZ_Widget     *widget;
     Drawable      drawable;     /* place to draw  */
     int           x,y;          /* up left corner */
     int           size;         /* size in pixels, including border */
     int           borderstyle;  /* type of border */
     int           borderwidth;  /* border width   */
     int           highlight;    /* highlight ?    */
{
  XPoint base[3], right[4], left[4], top[4];
  GC   gcright, gcleft, gctop, othergc;
  int  yoffset, xoffset;
  unsigned long bgpv;

  xoffset = (borderwidth * 17)>>3;  /* x and y are switched */
  yoffset = (borderwidth * 50)>>5;

  base[0].x = (short)x;
  base[0].y = (short)y;
  base[1].x = (short)(x+size);
  base[1].y = (short)y;
  base[2].x = (short)(x + (size>>1));
  base[2].y = (short)(y + size);

  top[0].x = (short)x;
  top[0].y = (short)y;
  top[1].x = (short)(x + yoffset);
  top[1].y = (short)(y + borderwidth);
  top[2].x = (short)(x + size - yoffset);
  top[2].y = (short)(y + borderwidth);
  top[3].x = (short)(x+size);
  top[3].y = (short)(y);
  
  right[0].x = top[3].x;
  right[0].y = top[3].y;
  right[1].x = top[2].x;
  right[1].y = top[2].y;
  right[2].x = (short)(1+x + (size>>1));  
  right[2].y = (short)(y + size - xoffset);
  right[3].x = right[2].x;
  right[3].y = (short)(y + size);

  left[0].x = top[0].x;
  left[0].y = top[0].y;
  left[1].x = top[1].x;
  left[1].y = top[1].y;
  left[2].x = right[2].x;
  left[2].y = right[2].y;
  left[3].x = right[3].x;
  left[3].y = right[3].y;

  if(EZ_WidgetBGPixmapB(widget) != NULL)
    {
      EZ_Bitmap *bitmap = EZ_WidgetBGPixmapB(widget);
      othergc = EZ_NTILEGC2;
      if(highlight) XSetTile(EZ_Display,EZ_NTILEGC2,bitmap->htpixmap);
      else XSetTile(EZ_Display,EZ_NTILEGC2,bitmap->pixmap);
      XSetTSOrigin(EZ_Display, EZ_NTILEGC2, 0, 0);
    }
  else if(EZ_WidgetParentBGPixmapB(widget) != NULL)
    {
      EZ_Bitmap *bitmap = EZ_WidgetParentBGPixmapB(widget);
      othergc = EZ_NTILEGC2;
      if(highlight) XSetTile(EZ_Display,EZ_NTILEGC2,bitmap->htpixmap);
      else XSetTile(EZ_Display,EZ_NTILEGC2,bitmap->pixmap);
      XSetTSOrigin(EZ_Display, EZ_NTILEGC2, 0, 0);
    }
  else
    {
      int tile = EZ_GetBackgroundGC(widget,&othergc, &bgpv, 0, highlight);
      if(tile) 
        { 
          int ox, oy;
          EZ_GetWidgetTileOrigin(widget, &ox, &oy);
          XSetTSOrigin(EZ_Display, othergc, ox, oy);
        }
    }  
  XFillPolygon(EZ_DisplayForWidgets, drawable, othergc,
	       base, 3, Convex, CoordModeOrigin);

  if(borderstyle == EZ_BORDER_UP)
    EZ_SetupUpBdGC(widget, &gcleft, &gctop,&othergc,&othergc,&othergc,&gcright,0,0,1);
  else
    EZ_SetupUpBdGC(widget, &gcleft, &gcright, &othergc,&othergc,&othergc,&gctop,0,0,1);
  XFillPolygon(EZ_DisplayForWidgets, drawable, gcright,
	       right, 4, Convex, CoordModeOrigin);
  XFillPolygon(EZ_DisplayForWidgets, drawable, gctop,
	       top, 4, Convex, CoordModeOrigin);
  XFillPolygon(EZ_DisplayForWidgets, drawable, gcleft,
	       left, 4, Convex, CoordModeOrigin);
}
/******************************************************************************/
#undef _EZ_ARROW_C_
