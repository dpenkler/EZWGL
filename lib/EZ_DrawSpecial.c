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
#define _EZ_DRAW_SPECIAL_C_

#include "EZ_Widget.h"
#define EZ_LEFT_TRIANGLE2      EZ_DOUBLE_LEFT_TRIANGLE
#define EZ_RIGHT_TRIANGLE2     EZ_DOUBLE_RIGHT_TRIANGLE
#define EZ_UP_TRIANGLE2        EZ_DOUBLE_UP_TRIANGLE
#define EZ_DOWN_TRIANGLE2      EZ_DOUBLE_DOWN_TRIANGLE

/*********************************************************************
 * 
 *  Functions implemented in this file:
 */
void  EZ_DrawSpecial MY_ANSIARGS(( EZ_Widget     *widget,
				   Drawable      drawable,
				   GC            tgc,
				   int           type,
				   int           x, int y,
				   int           w, int h));

/***************************************************************************
 * 
 * Draw special labels. Arrows
 */

void  EZ_DrawSpecial(widget,drawable, tgc, type, x,y, w, h)
     EZ_Widget     *widget;
     Drawable      drawable;     /* place to draw  */
     GC            tgc;
     int           type;         
     int           x,y;          /* up left corner */
     int           w,h;          /* size */
{
  XPoint c1[8], c2[8];
  GC   gc;

  /* figure the gc to use */
  if(tgc) gc = tgc;
  else
    {
      if(EZ_WidgetForeground(widget) != EZ_DefaultForeground)
	{
	  XSetForeground(EZ_Display, EZ_WRITABLEGC, EZ_WidgetForeground(widget));
	  gc =  EZ_WRITABLEGC;
	}
      else
	{
	  if(EZ_WidgetBGPixmapB(widget) != NULL)
	    {
	      EZ_Bitmap *bitmap = EZ_WidgetBGPixmapB(widget);
	      XSetTile(EZ_Display,EZ_NTILEGC2,bitmap->pixmap);
	      XSetTSOrigin(EZ_Display, EZ_NTILEGC2, x, y);
	      gc = EZ_NTILEGC2;
	    }
	  else if(EZ_WidgetParentBGPixmapB(widget) != NULL)
	    {
	      EZ_Bitmap *bitmap = EZ_WidgetParentBGPixmapB(widget);
	      XSetTile(EZ_Display,EZ_NTILEGC2,bitmap->pixmap);
	      XSetTSOrigin(EZ_Display, EZ_NTILEGC2, x, y);
	      gc = EZ_NTILEGC2;
	    }
	  else
	    gc = EZ_BUTTON_TEXTGC;
	}
    }
  switch(type)
    {
    case EZ_DOWN_TRIANGLE:
      c1[0].x = (short)x;
      c1[0].y = (short)y;
      c1[1].x = (short) (x + (w>>1));
      c1[1].y = (short) (y +  h);
      c1[2].x = (short) (x + w);
      c1[2].y = (short) y;
      c1[3].x = c1[0].x;
      c1[3].y = c1[0].y;

      XFillPolygon(EZ_Display, drawable,gc,
		   c1, 4, Convex, CoordModeOrigin);
      XDrawLines(EZ_Display, drawable,gc,
		 c1, 4, CoordModeOrigin);
      break;
    case EZ_DOWN_TRIANGLE2:
      {
        int h2 = (h>>1);
	EZ_DrawSpecial(widget,drawable, gc, EZ_DOWN_TRIANGLE, x,y, w, h2); 
	EZ_DrawSpecial(widget,drawable, gc, EZ_DOWN_TRIANGLE, x,y+h2, w, h2); 
      }
    break;
    case EZ_UP_TRIANGLE:
      c1[0].x = (short)(x + (w >> 1));
      c1[0].y = (short)y;
      c1[1].x = (short) x;
      c1[1].y = (short) (y +  h);
      c1[2].x = (short) (x + w);
      c1[2].y = (short) (y + h);
      c1[3].x = c1[0].x;
      c1[3].y = c1[0].y;

      XFillPolygon(EZ_Display, drawable,gc,
		   c1, 3, Convex, CoordModeOrigin);
      XDrawLines(EZ_Display, drawable,gc,
		 c1, 4, CoordModeOrigin);
      break;
    case EZ_UP_TRIANGLE2:
      {
        int h2 = (h>>1);
	EZ_DrawSpecial(widget,drawable, gc, EZ_UP_TRIANGLE, x,y, w, h2); 
	EZ_DrawSpecial(widget,drawable, gc, EZ_UP_TRIANGLE, x,y+h2, w, h2); 
      }
    break;
    case EZ_LEFT_TRIANGLE:
      c1[0].x = (short)(x);
      c1[0].y = (short)(y + (h >> 1));
      c1[1].x = (short) (x + w);
      c1[1].y = (short) (y +  h);
      c1[2].x = (short) (x + w);
      c1[2].y = (short) y;
      c1[3].x = c1[0].x;
      c1[3].y = c1[0].y;
      
      XFillPolygon(EZ_Display, drawable,gc,
		   c1, 3, Convex, CoordModeOrigin);
      XDrawLines(EZ_Display, drawable,gc,
		 c1, 4, CoordModeOrigin);
      break;
    case EZ_LEFT_TRIANGLE2:
      {
        int w2 = (w>>1);
	EZ_DrawSpecial(widget,drawable, gc, EZ_LEFT_TRIANGLE, x,y, w2, h); 
	EZ_DrawSpecial(widget,drawable, gc, EZ_LEFT_TRIANGLE, x+w2,y, w2, h); 
      }
    break;

    case EZ_RIGHT_TRIANGLE:
      c1[0].x = (short)(x);
      c1[0].y = (short)(y);
      c1[1].x = (short)(x);
      c1[1].y = (short)(y +  h);
      c1[2].x = (short)(x + w);
      c1[2].y = (short)(y + (h >> 1));
      c1[3].x = c1[0].x;
      c1[3].y = c1[0].y;
      
      XFillPolygon(EZ_Display, drawable,gc,
		   c1, 3, Convex, CoordModeOrigin);
      XDrawLines(EZ_Display, drawable,gc,
		 c1, 4, CoordModeOrigin);
      break;
    case EZ_RIGHT_TRIANGLE2:
      {
        int w2 = (w>>1);
	EZ_DrawSpecial(widget,drawable, gc, EZ_RIGHT_TRIANGLE, x,y, w2, h); 
	EZ_DrawSpecial(widget,drawable, gc, EZ_RIGHT_TRIANGLE, x+w2,y, w2, h); 
      }
    break;

    case EZ_DOWN_ARROW:
      {
	int h2 = (h>>1);
	int w3 = (w/3);
	int w32 = w - w3;
	c1[0].x = (short)(x + w3);
	c1[0].y = (short)(y);
	c1[1].x = (short)(x + w3);
	c1[1].y = (short)(y + h2);
	c1[2].x = (short)(x + w32);
	c1[2].y = (short)(y + h2);
	c1[3].x = (short)(x + w32);
	c1[3].y = (short)(y);
	c1[4].x = c1[0].x;
	c1[4].y = c1[0].y;	
      
	c2[0].x = (short)x;
	c2[0].y = (short)(y+h2);
	c2[1].x = (short)(x + (w>>1));
	c2[1].y = (short)(y + h);
	c2[2].x = (short)(x + w);
	c2[2].y = (short)(y + h2);
	c2[3].x = c2[0].x;
	c2[3].y = c2[0].y;

	XFillPolygon(EZ_Display, drawable,gc,
		     c1, 4, Convex, CoordModeOrigin);
	XFillPolygon(EZ_Display, drawable,gc,
		     c2, 3, Convex, CoordModeOrigin);
	XDrawLines(EZ_Display, drawable,gc,
		   c1, 5, CoordModeOrigin);
	XDrawLines(EZ_Display, drawable,gc,
		   c2, 4, CoordModeOrigin);
    
      }
    break;

    case EZ_UP_ARROW:
      {
	int h2 = (h>>1);
	int w3 = (w/3);
	int w32 = w - w3;
	c1[0].x = (short)(x + w3);
	c1[0].y = (short)(y + h2);
	c1[1].x = (short)(x + w3);
	c1[1].y = (short)(y + h);
	c1[2].x = (short)(x + w32);
	c1[2].y = (short)(y + h);
	c1[3].x = (short)(x + w32);
	c1[3].y = (short)(y + h2);
	c1[4].x = c1[0].x;
	c1[4].y = c1[0].y;	      

	c2[0].x = (short)x ;
	c2[0].y = (short)(y+h2);
	c2[1].x = (short)(x + w);
	c2[1].y = (short)(y + h2);
	c2[2].x = (short)(x + (w>>1));
	c2[2].y = (short)(y);
	c2[3].x = c2[0].x;
	c2[3].y = c2[0].y;

	XFillPolygon(EZ_Display, drawable,gc,
		     c1, 4, Convex, CoordModeOrigin);
	XFillPolygon(EZ_Display, drawable,gc,
		     c2, 3, Convex, CoordModeOrigin);
	XDrawLines(EZ_Display, drawable,gc,
		   c1, 5, CoordModeOrigin);
	XDrawLines(EZ_Display, drawable,gc,
		   c2, 4, CoordModeOrigin);
      }
    break;
    case EZ_LEFT_ARROW:
      {
	int w2 = (w>>1);
	int h3 = (h/3);
	int h32 = h - h3;
	int h2 = h >> 1;

	c1[0].x = (short)(x + w2);
	c1[0].y = (short)(y + h3);
	c1[1].x = (short)(x + w);
	c1[1].y = (short)(y + h3);
	c1[2].x = (short)(x + w);
	c1[2].y = (short)(y + h32);
	c1[3].x = (short)(x + w2);
	c1[3].y = (short)(y + h32);
	c1[4].x = c1[0].x;
	c1[4].y = c1[0].y;	            

	c2[0].x = (short)(x);
	c2[0].y = (short)(y+h2);
	c2[1].x = (short)(x + w2);
	c2[1].y = (short)(y + h);
	c2[2].x = (short)(x + w2);
	c2[2].y = (short)(y);
	c2[3].x = c2[0].x;
	c2[3].y = c2[0].y;

	XFillPolygon(EZ_Display, drawable,gc,
		     c1, 4, Convex, CoordModeOrigin);
	XFillPolygon(EZ_Display, drawable,gc,
		     c2, 3, Convex, CoordModeOrigin);
	XDrawLines(EZ_Display, drawable,gc,
		   c1, 5, CoordModeOrigin);
	XDrawLines(EZ_Display, drawable,gc,
		   c2, 4, CoordModeOrigin);
    
      }
    break;
    case EZ_RIGHT_ARROW:
      {
	int w2 = (w>>1);
	int h3 = (h/3);
	int h32 = h - h3;
	int h2 = h >> 1;

	c1[0].x = (short)(x);
	c1[0].y = (short)(y + h3);
	c1[1].x = (short)(x);
	c1[1].y = (short)(y + h32);
	c1[2].x = (short)(x + w2);
	c1[2].y = (short)(y + h32);
	c1[3].x = (short)(x + w2);
	c1[3].y = (short)(y + h3);
	c1[4].x = c1[0].x;
	c1[4].y = c1[0].y;	      
      
	c2[0].x = (short)(x + w2);
	c2[0].y = (short)(y);
	c2[1].x = (short)(x + w);
	c2[1].y = (short)(y + h2);
	c2[2].x = (short)(x + w2);
	c2[2].y = (short)(y + h);
	c2[3].x = c2[0].x;
	c2[3].y = c2[0].y;


	XFillPolygon(EZ_Display, drawable,gc,
		     c1, 4, Convex, CoordModeOrigin);
	XFillPolygon(EZ_Display, drawable,gc,
		     c2, 3, Convex, CoordModeOrigin);
	XDrawLines(EZ_Display, drawable,gc,
		   c1, 5, CoordModeOrigin);
	XDrawLines(EZ_Display, drawable,gc,
		   c2, 4, CoordModeOrigin);
      }
    break;
    case EZ_UP_ARROW_BAR:
      {
	int h8 = (h>>3) + 1;
	int hh = h - h8 - 1 ;
	
	EZ_DrawSpecial(widget,drawable, gc, EZ_UP_ARROW, x,y+h8+1, w, hh); 

	c1[0].x = (short)(x);
	c1[0].y = (short)(y);
	c1[1].x = (short)(x);
	c1[1].y = (short)(y + h8);
	c1[2].x = (short)(x + w);
	c1[2].y = (short)(y + h8);
	c1[3].x = (short)(x + w);
	c1[3].y = (short)(y);

	XFillPolygon(EZ_Display, drawable,gc,
		     c1, 4, Convex, CoordModeOrigin);
	XDrawLines(EZ_Display, drawable,gc,
		   c1, 4, CoordModeOrigin);

      }
    break;
    case EZ_DOWN_ARROW_BAR:
      {
	int h8 = (h>>3) + 1;
	int hh = h - h8 - 1;

	EZ_DrawSpecial(widget,drawable, gc, EZ_DOWN_ARROW, x,y, w, hh);

	c1[0].x = (short)(x);
	c1[0].y = (short)(y+h);
	c1[1].x = (short)(x+w);
	c1[1].y = (short)(y + h);
	c1[2].x = (short)(x + w);
	c1[2].y = (short)(y + h - h8);
	c1[3].x = (short)(x );
	c1[3].y = (short)(y + h -h8);

	XFillPolygon(EZ_Display, drawable,gc,
		     c1, 4, Convex, CoordModeOrigin);
	XDrawLines(EZ_Display, drawable,gc,
		   c1, 4, CoordModeOrigin);
      }
    break;

    case EZ_LEFT_ARROW_BAR:
      {
	int w8 = (w>>3) + 1;
	int ww = w - w8 - 1 ;
	
	EZ_DrawSpecial(widget,drawable, gc, EZ_LEFT_ARROW, x+w8+1,y, ww, h); 

	c1[0].x = (short)(x);
	c1[0].y = (short)(y);
	c1[1].x = (short)(x+w8);
	c1[1].y = (short)(y);
	c1[2].x = (short)(x+w8);
	c1[2].y = (short)(y+h);
	c1[3].x = (short)(x);
	c1[3].y = (short)(y+h);

	XFillPolygon(EZ_Display, drawable,gc,
		     c1, 4, Convex, CoordModeOrigin);
	XDrawLines(EZ_Display, drawable,gc,
		   c1, 4, CoordModeOrigin);

      }
    break;
    case EZ_RIGHT_ARROW_BAR:
      {
	int w8 = (w>>3) + 1;
	int ww = w - w8 - 1;

	EZ_DrawSpecial(widget,drawable, gc, EZ_RIGHT_ARROW, x,y, ww, h);

	c1[0].x = (short)(x+w-w8);
	c1[0].y = (short)(y);
	c1[1].x = (short)(x+w);
	c1[1].y = (short)(y);
	c1[2].x = (short)(x + w);
	c1[2].y = (short)(y + h);
	c1[3].x = (short)(x+w-w8);
	c1[3].y = (short)(y + h);

	XFillPolygon(EZ_Display, drawable,gc,
		     c1, 4, Convex, CoordModeOrigin);
	XDrawLines(EZ_Display, drawable,gc,
		   c1, 4, CoordModeOrigin);
      }
    break;

    case EZ_DOWN_TRIANGLE_BAR:
      {
	/* thanks to Tomi Sarvela <orava@nether.tky.hut.fi> */
	int h8 = (h>>3) + 1;
	int hh = h - h8 - 1;
	
	EZ_DrawSpecial(widget,drawable, gc, EZ_DOWN_TRIANGLE, x,y, w, hh);
	
	c1[0].x = (short)(x);
	c1[0].y = (short)(y+h);
	c1[1].x = (short)(x+w);
	c1[1].y = (short)(y + h);
	c1[2].x = (short)(x + w);
	c1[2].y = (short)(y + h - h8);
	c1[3].x = (short)(x);
	c1[3].y = (short)(y + h - h8);
	
	XFillPolygon(EZ_Display, drawable,gc,
		     c1, 4, Convex, CoordModeOrigin);
	XDrawLines(EZ_Display, drawable,gc,
		   c1, 4, CoordModeOrigin);
      }
      break;
    case EZ_UP_TRIANGLE_BAR:
      {
	int h8 = (h>>3) + 1;
	int hh = h - h8 -1;
	
	EZ_DrawSpecial(widget,drawable, gc, EZ_UP_TRIANGLE, x,y + h8+1, w, hh);
	
	c1[0].x = (short)(x);
	c1[0].y = (short)(y);
	c1[1].x = (short)(x+w);
	c1[1].y = (short)(y);
	c1[2].x = (short)(x + w);
	c1[2].y = (short)(y + h8);
	c1[3].x = (short)(x);
	c1[3].y = (short)(y + h8);
	
	XFillPolygon(EZ_Display, drawable,gc,
		     c1, 4, Convex, CoordModeOrigin);
	XDrawLines(EZ_Display, drawable,gc,
		   c1, 4, CoordModeOrigin);
      }
      break;
    case EZ_LEFT_TRIANGLE_BAR:
      {
	int w8 = (w>>3) + 1;
	int ww = w - w8 -1;
	
	EZ_DrawSpecial(widget,drawable, gc, EZ_LEFT_TRIANGLE, x+w8+1,y, ww, h);
	
	c1[0].x = (short)(x);
	c1[0].y = (short)(y);
	c1[1].x = (short)(x+w8);
	c1[1].y = (short)(y);
	c1[2].x = (short)(x + w8);
	c1[2].y = (short)(y + h);
	c1[3].x = (short)(x);
	c1[3].y = (short)(y + h);
	
	XFillPolygon(EZ_Display, drawable,gc,
		     c1, 4, Convex, CoordModeOrigin);
	XDrawLines(EZ_Display, drawable,gc,
		   c1, 4, CoordModeOrigin);
      }
      break;
    case EZ_RIGHT_TRIANGLE_BAR:
      {
	int w8 = (w>>3) + 1;
	int ww = w - w8 -1;
	
	EZ_DrawSpecial(widget,drawable, gc, EZ_RIGHT_TRIANGLE, x,y, ww, h);
	
	c1[0].x = (short)(x+w-w8);
	c1[0].y = (short)(y);
	c1[1].x = (short)(x+w);
	c1[1].y = (short)(y);
	c1[2].x = (short)(x + w);
	c1[2].y = (short)(y + h);
	c1[3].x = (short)(x+w-w8);
	c1[3].y = (short)(y + h);
	
	XFillPolygon(EZ_Display, drawable,gc,
		     c1, 4, Convex, CoordModeOrigin);
	XDrawLines(EZ_Display, drawable,gc,
		   c1, 4, CoordModeOrigin);
      }
      break;
    case EZ_LEFT_RIGHT_ARROW:
      {
	int w3 = (w/3) + 1;
	int ww = w - w3;
	
	EZ_DrawSpecial(widget,drawable, gc, EZ_RIGHT_ARROW, x+w3,y, ww, h);
	EZ_DrawSpecial(widget,drawable, gc, EZ_LEFT_ARROW,  x,y, ww, h);
      }
      break;

    case EZ_UP_DOWN_ARROW:
      {
	int h3 = (h/3) + 1;
	int hh = h - h3;
	
	EZ_DrawSpecial(widget,drawable, gc, EZ_DOWN_ARROW, x,y+h3, w, hh);
	EZ_DrawSpecial(widget,drawable, gc, EZ_UP_ARROW,  x,y, w, hh);
      }
      break;
    case EZ_LEFT_BAR_TRIANGLE:
      {
	int w8 = (w>>3) + 1;
	int ww = w - w8 -1;
	
	EZ_DrawSpecial(widget,drawable, gc, EZ_LEFT_TRIANGLE, x-1,y, ww-2, h);
	
	c1[0].x = (short)(x+w-w8);
	c1[0].y = (short)(y);
	c1[1].x = (short)(x+w);
	c1[1].y = (short)(y);
	c1[2].x = (short)(x + w);
	c1[2].y = (short)(y + h);
	c1[3].x = (short)(x+w-w8);
	c1[3].y = (short)(y + h);
	
	XFillPolygon(EZ_Display, drawable,gc,
		     c1, 4, Convex, CoordModeOrigin);
	XDrawLines(EZ_Display, drawable,gc,
		   c1, 4, CoordModeOrigin);
      }
      break;      
    case EZ_RIGHT_BAR_TRIANGLE:
      {
	int w8 = (w>>3) + 1;
	int ww = w - w8 -1;
	
	EZ_DrawSpecial(widget,drawable, gc, EZ_RIGHT_TRIANGLE, x+w8+4,y, ww-2, h);
	
	c1[0].x = (short)(x);
	c1[0].y = (short)(y);
	c1[1].x = (short)(x+w8);
	c1[1].y = (short)(y);
	c1[2].x = (short)(x+w8);
	c1[2].y = (short)(y+h);
	c1[3].x = (short)(x);
	c1[3].y = (short)(y+h);
	
	XFillPolygon(EZ_Display, drawable,gc,
		     c1, 4, Convex, CoordModeOrigin);
	XDrawLines(EZ_Display, drawable,gc,
		   c1, 4, CoordModeOrigin);
      }
      break;      
    case EZ_UP_BAR_TRIANGLE:
      {
	int h8 = (h>>3) + 1;
	int hh = h - h8 -1;
	
	EZ_DrawSpecial(widget,drawable, gc, EZ_UP_TRIANGLE, x,y, w, hh-2);
	
	c1[0].x = (short)(x);
	c1[0].y = (short)(y+h-h8);
	c1[1].x = (short)(x+w);
	c1[1].y = (short)(y+h-h8);
	c1[2].x = (short)(x + w);
	c1[2].y = (short)(y + h);
	c1[3].x = (short)(x);
	c1[3].y = (short)(y + h);
	
	XFillPolygon(EZ_Display, drawable,gc,
		     c1, 4, Convex, CoordModeOrigin);
	XDrawLines(EZ_Display, drawable,gc,
		   c1, 4, CoordModeOrigin);
      }
      break;      
    case EZ_DOWN_BAR_TRIANGLE:
      {
	/* thanks to Tomi Sarvela <orava@nether.tky.hut.fi> */
	int h8 = (h>>3) + 1;
	int hh = h - h8 - 1;
	
	EZ_DrawSpecial(widget,drawable, gc, EZ_DOWN_TRIANGLE, x,y+h8+3, w, hh-2);
	
	c1[0].x = (short)(x);
	c1[0].y = (short)(y);
	c1[1].x = (short)(x+w);
	c1[1].y = (short)(y);
	c1[2].x = (short)(x+w);
	c1[2].y = (short)(y+h8);
	c1[3].x = (short)(x);
	c1[3].y = (short)(y+h8);
	
	XFillPolygon(EZ_Display, drawable,gc,
		     c1, 4, Convex, CoordModeOrigin);
	XDrawLines(EZ_Display, drawable,gc,
		   c1, 4, CoordModeOrigin);
      }
      break;
    case EZ_UP_TRIANGLE_UP:
      {
        int size = (w < h ? w : h) - 1;
        int hlight = EZ_GetWidgetHighlightFlag(widget);
        x += (w-size)/2; y += (h -size)/2;
        EZ_DrawVUArrow(widget, drawable, x, y, size, EZ_BORDER_UP, 1, hlight);
      }
    break;
    case EZ_UP_TRIANGLE_DOWN:
      {
        int size = (w < h ? w : h) - 1;
        int hlight = EZ_GetWidgetHighlightFlag(widget);
        x += (w-size)/2; y += (h -size)/2;
        EZ_DrawVUArrow(widget, drawable, x, y, size, EZ_BORDER_DOWN,1 , hlight);
      }
    break;
    case EZ_DOWN_TRIANGLE_UP:
      {
        int size = (w < h ? w : h) - 1;
        int hlight = EZ_GetWidgetHighlightFlag(widget);
        x += (w-size)/2; y += (h -size)/2;
        EZ_DrawVDArrow(widget, drawable, x, y, size, EZ_BORDER_UP, 1 , hlight);
      }
    break;
    case EZ_DOWN_TRIANGLE_DOWN:
      {
        int size = (w < h ? w : h) - 1;
        int hlight = EZ_GetWidgetHighlightFlag(widget);
        x += (w-size)/2; y += (h -size)/2;
        EZ_DrawVDArrow(widget, drawable, x, y, size, EZ_BORDER_DOWN, 1 , hlight);
      }
    break;
    case EZ_LEFT_TRIANGLE_UP:
      {
        int size = (w < h ? w : h) - 1;
        int hlight = EZ_GetWidgetHighlightFlag(widget);
        x += (w-size)/2; y += (h -size)/2;
        EZ_DrawHLArrow(widget, drawable, x, y, size, EZ_BORDER_UP, 1 , hlight);
      }
    break;
    case EZ_LEFT_TRIANGLE_DOWN:
      {
        int size = (w < h ? w : h) - 1;
        int hlight = EZ_GetWidgetHighlightFlag(widget);
        x += (w-size)/2; y += (h -size)/2;
        EZ_DrawHLArrow(widget, drawable, x, y, size, EZ_BORDER_DOWN, 1 , hlight);
      }
    break;
    case EZ_RIGHT_TRIANGLE_UP:
      {
        int size = (w < h ? w : h) - 1;
        int hlight = EZ_GetWidgetHighlightFlag(widget);
        x += (w-size)/2; y += (h -size)/2;
        EZ_DrawHRArrow(widget, drawable, x, y, size, EZ_BORDER_UP, 1 , hlight);
      }
    break;
    case EZ_RIGHT_TRIANGLE_DOWN:
      {
        int size = (w < h ? w : h) - 1;
        int hlight = EZ_GetWidgetHighlightFlag(widget);
        x += (w-size)/2; y += (h -size)/2;
        EZ_DrawHRArrow(widget, drawable, x, y, size, EZ_BORDER_DOWN, 1 , hlight);
      }
    break;
    case EZ_RECTANGLE:
      XFillRectangle(EZ_Display, drawable, gc, x,y,w,h);
      break;
    case EZ_ELLIPSE:
      XFillArc(EZ_Display, drawable, gc, x, y, w, h, 0, 360*64);
      break;
    default:
      break;
    }
}

/***************************************************************************/
#undef _EZ_DRAW_SPECIAL_C_
