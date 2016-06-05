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
 ***              Widget Lcd.                                  ***
 ***                                                           ***
 *****************************************************************/
#define _EZ_WIDGET_LCD_C_
#include "EZ_Widget.h"

/*********************************************************************
 * 
 *  Functions implemented in this file:
 */
void       EZ_SetLcdString MY_ANSIARGS((EZ_Widget *parent, char *str));
void       EZ_SetLcdInteger MY_ANSIARGS((EZ_Widget *lcd, int n));
void       EZ_SetLcdFloat MY_ANSIARGS((EZ_Widget *cd, float f));
void       EZ_ClearLcd  MY_ANSIARGS((EZ_Widget *lcd));
void       EZ_SetLcdNDigits  MY_ANSIARGS((EZ_Widget *lcd, int n));
void       EZ_SetLcdFontSize  MY_ANSIARGS((EZ_Widget *lcd, int n));
void       EZ_SetLcdForeground  MY_ANSIARGS((EZ_Widget *lcd, char *clr));
void       EZ_SetLcdBackground  MY_ANSIARGS((EZ_Widget *lcd, char *clr));
/*********************************************************************/
EZ_Widget  *EZ_CreateLcd MY_ANSIARGS((EZ_Widget *parent, int n));
void        EZ_DrawWidgetLcd MY_ANSIARGS((EZ_Widget *widget));
void        EZ_ComputeWidgetLcdSize MY_ANSIARGS((EZ_Widget *widget, int *w, int *h));
void        EZ_FreeWidgetLcdData MY_ANSIARGS((EZ_Widget *widget));
void        EZ_LcdEventHandle MY_ANSIARGS((EZ_Widget *widget, XEvent *event));
/*********************************************************************/

static void DrawLCDDigit MY_ANSIARGS((EZ_Widget *parent, Drawable d,
				      GC bgc, GC gc, GC dgc, GC lgc, int *loc,
				      int ch, int xsize, int ysize));
static char *LcdGetCharCode MY_ANSIARGS(( char c ));
static void EZ_UpdateLcd  MY_ANSIARGS((EZ_Widget *parent, char *str));
/*********************************************************************
 * 
 *  Local Variables.
 */
static EZ_WidgetHandle EZ_LcdHandle =
{ 
  EZ_ComputeWidgetLcdSize,
  EZ_DrawWidgetLcd,
  EZ_FreeWidgetLcdData,
  EZ_LcdEventHandle,    /* response only to Expose */
};

/*********************************************************************/

EZ_Widget  *EZ_CreateLcd(parent, ndigits)
     EZ_Widget  *parent;     /* parent widget    */
     int ndigits;
{
  EZ_Widget  *tmp;
  
  tmp = EZ_CreateNewWidget(parent);
  /*--------------------------------------------------
   * Register the handling functions for label.
   *  has to be done after  EZ_CreateNewWiget.
   *-------------------------------------------------*/
  EZ_WidgetHandlingFunctions[EZ_WIDGET_LCD] = &EZ_LcdHandle;

  EZ_SetWidgetTypeAndNames(tmp,EZ_WIDGET_LCD);
  EZ_WidgetBorderStyle(tmp)  = EZ_BORDER_NONE;
  EZ_WidgetBorderWidth(tmp)  = 0;
  EZ_WidgetPadB(tmp)  = 0;
  EZ_LCDNDigits(tmp) = (ndigits> 0? ndigits: 6);
  EZ_LCDString(tmp) = EZ_AllocCopyString(NULL);
  EZ_LCDStringLength(tmp) = 0;
  EZ_LCDSize(tmp) = 8;
  EZ_LCDBG(tmp) = ~0;
  EZ_LCDFG(tmp) = ~0;
  EZ_LCDFGDark(tmp) = ~0;
  EZ_LCDFGBright(tmp) = ~0;

  return(tmp);
}


void   EZ_FreeWidgetLcdData(widget)
     EZ_Widget *widget;
{
  if(EZ_LCDString(widget) != (char *)NULL)
    (void) my_free((char *)EZ_LCDString(widget));
}

 
/********************************************************************
 *
 *  Figure out the dimension of a lcd widget.
 */
void EZ_ComputeWidgetLcdSize(widget, w, h)
    EZ_Widget *widget;
     int             *w, *h;
{
  int      length, height, cw, ch;
	    
  if(!(EZ_GetWidgetSizeComputedFlag(widget))){ EZ_SetWidgetSizeComputedFlag(widget);}
  length = EZ_LCDSize(widget) * (1 + EZ_LCDNDigits(widget)) * 6/5; 
  height = EZ_LCDSize(widget) * 12/5;
  cw = EZ_WidgetPadX(widget) + EZ_WidgetBorderWidth(widget) + EZ_WidgetPadB(widget);
  ch = EZ_WidgetPadY(widget) + EZ_WidgetBorderWidth(widget) + EZ_WidgetPadB(widget);
  cw = cw + cw + length;
  ch = ch + ch + height;  

  *w = cw;
  *h = ch;
}

/*******************************************************************
 *
 *  Draw A Lcd Widget.
 */
void  EZ_DrawWidgetLcd(wptr)
     EZ_Widget *wptr;
{
  int       w, h,padb, padb2, ox,oy,torigin, tile,bktile,dtile,ltile,ttt;
  Pixmap    pixmap;
  Window    win;
  GC        gc;
  unsigned long   bgpv;

  torigin=tile=bktile=dtile=ltile = ttt= 0;
  win = EZ_WidgetWindow(wptr);
  w   = EZ_WidgetWidth(wptr);
  h   = EZ_WidgetHeight(wptr);
  padb = EZ_WidgetPadB(wptr);
  padb2 = padb + padb;

  pixmap = XCreatePixmap(EZ_Display, win, w, h, EZ_DepthForWidgets); 
  EZ_FixFocusHighlightBorder(wptr, pixmap, w, h, padb);

  /* if LCD_BACKGROUND is set, use it */
  if(EZ_LCDBG(wptr) != ~0)
    {
      bgpv = EZ_LCDBG(wptr);	
      XSetForeground(EZ_Display, EZ_WRITABLEGC, bgpv);
      gc = EZ_WRITABLEGC;
    }
  else 
    {
      if((torigin =EZ_GetBackgroundGC(wptr, &gc, &bgpv, 1, 0)))
	{
	  EZ_GetWidgetTileOrigin(wptr, &ox, &oy);      
	  XSetTSOrigin(EZ_Display, gc, ox, oy);
	}
    }
  XFillRectangle(EZ_DisplayForWidgets, pixmap, gc, padb,padb, w-padb2, h-padb2); 
  {
    char *str;
    int loc[2], i, j, strlen;
    int tmp =  (padb + EZ_WidgetBorderWidth(wptr));
    int tmpx =  (tmp + EZ_WidgetPadX(wptr))<<1;
    int tmpy =  (tmp + EZ_WidgetPadY(wptr))<<1;
    int xlen       = (EZ_WidgetWidth(wptr) - tmpx)*5/(EZ_LCDNDigits(wptr)*(6) + 1);
    int ylen       = (EZ_WidgetHeight(wptr) -tmpy)*5/12;
    int xsep       = xlen*6/5;
    int xx         = ((EZ_WidgetWidth(wptr) - tmpx)- EZ_LCDNDigits(wptr)*xsep + xlen/5)/2;
    int yy         = ((EZ_WidgetHeight(wptr)-tmpy) - ylen*2 )/2;
    int x, y;
    GC  gc, lgc, dgc;
    
    if(EZ_WidgetBGPixmapB(wptr) != NULL)
      {
	EZ_Bitmap *bitmap = EZ_WidgetBGPixmapB(wptr);
	gc = EZ_NTILEGC2;
	dgc = EZ_DTILEGC2;
	lgc = EZ_BTILEGC2;
	XSetTile(EZ_Display,EZ_NTILEGC2,bitmap->pixmap);
	XSetTile(EZ_Display,EZ_DTILEGC2,bitmap->dkpixmap);
	XSetTile(EZ_Display,EZ_BTILEGC2,bitmap->btpixmap);
	ttt=tile=dtile=ltile = 1;
      }
    else if(EZ_WidgetParentBGPixmapB(wptr) != NULL)
      {
	EZ_Bitmap *bitmap = EZ_WidgetParentBGPixmapB(wptr);
	gc = EZ_NTILEGC2;
	dgc = EZ_DTILEGC2;
	lgc = EZ_BTILEGC2;
	XSetTile(EZ_Display,EZ_NTILEGC2,bitmap->pixmap);
	XSetTile(EZ_Display,EZ_DTILEGC2,bitmap->dkpixmap);
	XSetTile(EZ_Display,EZ_BTILEGC2,bitmap->btpixmap);
	ttt=tile=dtile=ltile = 1;
      }
    else
      {
	if(EZ_GetDarkBrightNormalBDGC(wptr, &dgc, &lgc, &gc))
	  {
	    ttt= tile=dtile=ltile = 1;
	  }
      }  

    /* get tile origin if needed */
    if(ttt && torigin==0) EZ_GetWidgetTileOrigin(wptr, &ox, &oy);      
    {
      unsigned long pixel = EZ_LCDFGDark(wptr);
      if(pixel != ~0)
	{
	  XSetForeground(EZ_Display, EZ_BD_DARKGC, pixel);
	  dgc = EZ_BD_DARKGC;
	  dtile = 0;
	}
      pixel = EZ_LCDFGBright(wptr);
      if(pixel != ~0)
	{
	  XSetForeground(EZ_Display, EZ_BD_BRIGHTGC, pixel);
	  lgc = EZ_BD_BRIGHTGC;	
	  ltile = 0;
	}
      pixel = EZ_LCDFG(wptr);
      if(pixel != ~0)
	{
	  XSetForeground(EZ_Display, EZ_BD_NORMALGC, pixel);
	  gc = EZ_BD_NORMALGC;
	  tile = 0;
	}
      else
	{
	  pixel = EZ_WidgetForeground(wptr);
	  if(pixel != EZ_DefaultForeground)
	    {
	      XSetForeground(EZ_Display, EZ_BD_NORMALGC, pixel);
	      gc = EZ_BD_NORMALGC;
	      tile = 0;
	    }
	}
    }
    if(tile)   XSetTSOrigin(EZ_Display, gc, ox, oy);
    if(dtile)  XSetTSOrigin(EZ_Display, dgc, ox, oy);
    if(ltile)  XSetTSOrigin(EZ_Display, lgc, ox, oy);

    x = xx + EZ_WidgetPadX(wptr) + EZ_WidgetBorderWidth(wptr) + padb;
    y = yy + EZ_WidgetBorderWidth(wptr) + EZ_WidgetPadY(wptr) + padb;  
    
    str = EZ_LCDString(wptr);
    strlen = EZ_LCDStringLength(wptr);
    tmp = EZ_LCDNDigits(wptr) - strlen;
    for(i = (tmp < 0? - tmp: 0), j=tmp+i; i < strlen; i++, j++)
      {
	int c = str[i];
	loc[0] = x + j * xsep; loc[1] = y;
	DrawLCDDigit(wptr, pixmap, (GC)0, gc, dgc, lgc, loc, c, xlen, ylen);
      }
  }
  EZ_DrawRectBorder(wptr, pixmap);     
  XCopyArea(EZ_DisplayForWidgets,pixmap,win, EZ_WRITABLEGC,0,0,w,h,0,0); 
  XFreePixmap(EZ_DisplayForWidgets, pixmap); 
}
/***************************************************************/
void EZ_LcdEventHandle(widget, event)
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
	      Time  time_stamp = event->xbutton.time;   
	      int   elapsedtime = EZ_LastButton1ClickTime - time_stamp;
	      EZ_LastButton1ClickTime = time_stamp;
	  
	      if(ABSV(elapsedtime) < DOUBLE_CLICK_TIME_INTERVAL)
		{
		  if(EZ_WidgetCallBackFunc(widget))
		    {
		      { EZ_ExecuteWidgetCallBacks(widget);}
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
	}
    }
}
/****************************************************************************/

static void DrawLCDDigit(widget, drawable, backgc, gc, dgc, lgc, pos, ch, xsize, ysize)
     int *pos, ch, xsize, ysize; 
     GC backgc, gc, dgc, lgc;
     EZ_Widget *widget; Drawable drawable;
{
  static XPoint xy[10];
  int    loc[2];
  char *segs= LcdGetCharCode(ch);
  int  i, adj, w, w2, w4, w8;

  if(xsize <= ysize)
    {
      w = ysize/5;
      i = w+w;
      if(i+i > xsize) w = xsize/4;
    }
  else 
    {
      w = xsize/5;
      i = w+w;
      if(i+i > ysize) w = ysize/4;
    }
  adj = ((w&1) ? 1 : 0);

  w8 = (w+5)/8;
  w4 = w8+w8;
  w2 = w4+w4;

  if(backgc != (GC)0)
    XFillRectangle(EZ_Display, drawable, backgc, pos[0],pos[1], 
		   xsize+xsize, ysize+ysize+1); 

  for(i = 0; segs[i] !=0 ; i++)
    {
      int which = segs[i];
      loc[0] = pos[0]; loc[1] = pos[1];
      switch(which)
	{
	case 1 :
	  xy[0].x = loc[0] + w4;            xy[0].y = loc[1]+w4;
	  xy[1].x = loc[0] + w2;            xy[1].y = loc[1];
	  xy[2].x = loc[0] + xsize -w2 -1;  xy[2].y = loc[1];
	  xy[3].x = loc[0] + xsize -w4 -1;  xy[3].y = loc[1] + w4;
	  xy[4].x = loc[0] + xsize - w -1;  xy[4].y = loc[1] + w;
	  xy[5].x = loc[0] + w+1;           xy[5].y = loc[1] + w;
	  xy[6].x = xy[0].x;                xy[6].y = xy[0].y;
	  XFillPolygon(EZ_Display, drawable, gc, xy, 7,Convex,CoordModeOrigin);
	  XDrawLines(EZ_Display, drawable, lgc, xy, 4, CoordModeOrigin);
	  XDrawLines(EZ_Display, drawable, dgc, xy+3, 3, CoordModeOrigin);
	  break;
	case 2 :
	  loc[1]++;
	  xy[0].x = loc[0];                xy[0].y = loc[1] + ysize-w4 -2 ;
	  xy[1].x = loc[0];                xy[1].y = loc[1] + w2;
	  xy[2].x = loc[0] + w4;           xy[2].y = loc[1] + w4;
	  xy[3].x = loc[0] + w;            xy[3].y = loc[1] + w;
	  xy[4].x = loc[0] + w;            xy[4].y = loc[1] + ysize-w/2-2;
	  xy[5].x = loc[0] + w4;           xy[5].y = loc[1] + ysize-w8 -2;
	  xy[6].x = xy[0].x;               xy[6].y = xy[0].y;
	  XFillPolygon(EZ_Display, drawable, gc, xy, 6, Convex, CoordModeOrigin);
	  XDrawLines(EZ_Display, drawable, lgc, xy, 4, CoordModeOrigin);
	  XDrawLines(EZ_Display, drawable, dgc, xy+3, 4, CoordModeOrigin);
	  break;
	case 3 :
	  loc[0] += (xsize - 1);
	  loc[1]++;
	  xy[0].x = loc[0];                xy[0].y = loc[1] +w2-1;
	  xy[1].x = loc[0];                xy[1].y = loc[1] + ysize-w4 -2;
	  xy[2].x = loc[0]-w4;             xy[2].y = loc[1] + ysize-w8 -2;
	  xy[3].x = loc[0]-w;              xy[3].y = loc[1] + ysize-w/2-2;
	  xy[4].x = loc[0]-w;              xy[4].y = loc[1] + w;
	  xy[5].x = loc[0]-w4;             xy[5].y = loc[1] +w4;
	  xy[6].x = loc[0];                xy[6].y = loc[1] +w2;
	  XFillPolygon(EZ_Display, drawable, gc, xy, 6, Convex, CoordModeOrigin);
	  XDrawLines(EZ_Display, drawable, lgc, xy+3, 4, CoordModeOrigin);
	  XDrawLines(EZ_Display, drawable, dgc, xy, 4, CoordModeOrigin);
	  break;
	case 4 :
	  loc[1] += ysize;
	  xy[0].x = loc[0]+w4;             xy[0].y = loc[1]-w8;
	  xy[1].x = loc[0]+w;              xy[1].y = loc[1] -w/2;
	  xy[2].x = loc[0]+xsize-w-1;      xy[2].y = loc[1] -w/2;
	  xy[3].x = loc[0]+xsize-1-w4;     xy[3].y = loc[1] -w8;
	  xy[4].x = loc[0]+xsize-1-w4;     xy[4].y = loc[1] +w8;
	  xy[5].x = loc[0]+xsize-w-adj*2-1;xy[5].y = loc[1] +w/2 +adj;
	  xy[6].x = loc[0]+w+adj*2;        xy[6].y = loc[1] +w/2 +adj;
	  xy[7].x = loc[0]+w4;             xy[7].y = loc[1] +w8;
	  xy[8].x = loc[0]+w4;             xy[8].y = loc[1]-w8;
	  XFillPolygon(EZ_Display, drawable, gc, xy, 8, Convex, CoordModeOrigin);
	  XDrawLines(EZ_Display, drawable, lgc, xy, 4, CoordModeOrigin);
	  XDrawLines(EZ_Display, drawable, dgc, xy+3, 5, CoordModeOrigin);
	  break;
	case 5 :
	  loc[1] += (ysize + 1);
	  xy[0].x = loc[0];                xy[0].y = loc[1] +ysize-2-w2;
	  xy[1].x = loc[0];                xy[1].y = loc[1]+w4;
	  xy[2].x = loc[0]+w4;             xy[2].y = loc[1] +w8;
	  xy[3].x = loc[0]+w;              xy[3].y = loc[1] +w/2;
	  xy[4].x = loc[0]+w;              xy[4].y = loc[1] +ysize-w-2;
	  xy[5].x = loc[0]+w4;             xy[5].y = loc[1] +ysize-2-w4;
	  xy[6].x = xy[0].x;               xy[6].y = xy[0].y;  
	  XFillPolygon(EZ_Display, drawable, gc, xy, 6, Convex, CoordModeOrigin);
	  XDrawLines(EZ_Display, drawable, lgc, xy, 4, CoordModeOrigin);
	  XDrawLines(EZ_Display, drawable, dgc, xy+3, 4, CoordModeOrigin);
	  break;
	case 6 :
	  loc[0] += (xsize - 1);
	  loc[1] += (ysize + 1);
	  xy[0].x = loc[0] -w;             xy[0].y = loc[1] + ysize-w-2;
	  xy[1].x = loc[0] -w;             xy[1].y = loc[1] + w/2;
	  xy[2].x = loc[0]-w4;             xy[2].y = loc[1]+w8;
	  xy[3].x = loc[0];                xy[3].y = loc[1]+w4;
	  xy[4].x = loc[0];                xy[4].y = loc[1] + ysize-2-w2;
	  xy[5].x = loc[0]-w4;             xy[5].y = loc[1] + ysize-2-w4;
	  xy[6].x = xy[0].x;               xy[6].y = xy[0].y;
	  XFillPolygon(EZ_Display, drawable, gc, xy, 6, Convex, CoordModeOrigin);
	  XDrawLines(EZ_Display, drawable, lgc, xy, 4, CoordModeOrigin);
	  XDrawLines(EZ_Display, drawable, dgc, xy+3, 4, CoordModeOrigin);
	  break;
	case 7 :
	  loc[1] += (ysize + ysize)-1;
	  xy[0].x = loc[0]+w4+1;           xy[0].y = loc[1] -w4;
	  xy[1].x = loc[0]+w+1;            xy[1].y = loc[1] -w;
	  xy[2].x = loc[0]+xsize-w-1;      xy[2].y = loc[1] -w;
	  xy[3].x = loc[0]+xsize-w4-1;     xy[3].y = loc[1] -w4;
	  xy[4].x = loc[0]+xsize-w2-1;     xy[4].y = loc[1]; 
	  xy[5].x = loc[0]+w2+1;           xy[5].y = loc[1];
	  xy[6].x = xy[0].x;               xy[6].y = xy[0].y; 
	  XFillPolygon(EZ_Display, drawable, gc, xy, 6, Convex, CoordModeOrigin);
	  XDrawLines(EZ_Display, drawable, lgc, xy, 3, CoordModeOrigin);
	  XDrawLines(EZ_Display, drawable, dgc, xy+3, 3, CoordModeOrigin);
	  break;
	case 8 :
	  loc[0] += xsize/2;
	  loc[1] += (ysize+ysize);
	  xy[0].x = loc[0];                xy[0].y = loc[1];
	  xy[1].x = loc[0]+w;              xy[1].y = loc[1];
	  xy[2].x = loc[0]+w;              xy[2].y = loc[1]-w;
	  xy[3].x = loc[0];                xy[3].y = loc[1]-w;
	  xy[4].x = loc[0];                xy[4].y = loc[1];
	  XFillPolygon(EZ_Display, drawable, gc, xy, 4, Convex, CoordModeOrigin);
	  XDrawLines(EZ_Display, drawable, dgc, xy, 3, CoordModeOrigin);
	  XDrawLines(EZ_Display, drawable, lgc, xy+2, 3, CoordModeOrigin);
	  break;
	case 9 :
	  loc[0] += (xsize/2 - w/2 + 1);
	  loc[1] += (ysize/2 + w);
	  xy[0].x = loc[0];                xy[0].y = loc[1];
	  xy[1].x = loc[0]+w;              xy[1].y = loc[1];
	  xy[2].x = loc[0]+w;              xy[2].y = loc[1]-w;
	  xy[3].x = loc[0];                xy[3].y = loc[1]-w;
	  xy[4].x = loc[0];                xy[4].y = loc[1];
	  XFillPolygon(EZ_Display, drawable, gc, xy, 4, Convex, CoordModeOrigin);
	  XDrawLines(EZ_Display, drawable, dgc, xy, 3, CoordModeOrigin);
	  XDrawLines(EZ_Display, drawable, lgc, xy+2, 3, CoordModeOrigin);
	  break;
	case 10 :
	  loc[0] += ((xsize>>1) - (w>>1) + 1);
	  loc[1] += (((3*ysize)>>1) + w);
	  xy[0].x = loc[0];                xy[0].y = loc[1];
	  xy[1].x = loc[0]+w;              xy[1].y = loc[1];
	  xy[2].x = loc[0]+w;              xy[2].y = loc[1]-w;
	  xy[3].x = loc[0];                xy[3].y = loc[1]-w;
	  xy[4].x = loc[0];                xy[4].y = loc[1];
	  XFillPolygon(EZ_Display, drawable, gc, xy, 4, Convex, CoordModeOrigin);
	  XDrawLines(EZ_Display, drawable, dgc, xy, 3, CoordModeOrigin);
	  XDrawLines(EZ_Display, drawable, lgc, xy+2, 3, CoordModeOrigin);
	  break;
	case 11 :
	  xy[0].x = loc[0] + w4;            xy[0].y = loc[1]+w4;
	  xy[1].x = loc[0] + w2;            xy[1].y = loc[1];
	  xy[2].x = loc[0] + xsize -w2 -1;  xy[2].y = loc[1];
	  xy[3].x = loc[0] + xsize -w4 -1;  xy[3].y = loc[1] + w4;
	  xy[4].x = loc[0] + xsize - w -1;  xy[4].y = loc[1] + w;
	  xy[5].x = loc[0] + w+1;           xy[5].y = loc[1] + w;
	  xy[6].x = xy[0].x;                xy[6].y = xy[0].y;
	  XFillPolygon(EZ_Display, drawable, gc, xy, 7,Convex,CoordModeOrigin);
	  XDrawLines(EZ_Display, drawable, lgc, xy, 4, CoordModeOrigin);
	  XDrawLines(EZ_Display, drawable, dgc, xy+3, 3, CoordModeOrigin);
	  break;
	default:
	  break;
	}
    }
}
/***********************************************************************/

static char LcdCharCode[30][8] = {
  { 1, 2, 3, 5, 6, 7,0, 0},		/* 0	0*/
  { 3, 6,0, 0, 0, 0, 0, 0},		/* 1	1*/
  { 1, 3, 4, 5, 7,0, 0, 0},		/* 2	2*/
  { 1, 3, 4, 6, 7,0, 0, 0},		/* 3	3*/
  { 2, 3, 4, 6,0, 0, 0, 0},		/* 4	4*/
  { 1, 2, 4, 6, 7,0, 0, 0},		/* 5	5*/
  { 1, 2, 4, 5, 6, 7,0, 0},		/* 6	6*/
  { 1, 3, 6,0, 0, 0, 0, 0},		/* 7	7*/
  { 1, 2, 3, 4, 5, 6, 7,0},		/* 8	8*/
  { 1, 2, 3, 4, 6, 7,0, 0},		/* 9	9*/
  { 1, 2, 3, 4, 5, 6,0, 0},		/* 10	A*/
  { 2, 4, 5, 6, 7,0, 0, 0},		/* 11	B*/
  { 1, 2, 5, 7,0, 0, 0, 0},		/* 12	C*/
  { 3, 4, 5, 6, 7,0, 0, 0},		/* 13	D*/
  { 1, 2, 4, 5, 7,0, 0, 0},		/* 14	E*/
  { 1, 2, 4, 5,0, 0, 0, 0},		/* 15	F*/
  { 4,0, 0, 0, 0, 0, 0, 0},		/* 16	-*/
  { 8,0, 0, 0, 0, 0, 0, 0},		/* 17	.*/
  { 9, 10, 0, 0, 0, 0, 0, 0},           /* 18   :*/
  { 0, 0, 0, 0, 0, 0, 0, 0} };          /* 19	 */

/***********************************************************************/
static char *LcdGetCharCode( c )
     char c;
{
  if(c >= '0' && c <= '9')
    return LcdCharCode[c - '0'];
  else if(c >= 'A' && c <= 'F')
    return LcdCharCode[c - 'A' + 10];
  else if(c >= 'a' && c <= 'f')
    return LcdCharCode[c - 'a' + 10];
  else if(c == '-') 
    return LcdCharCode[16];
  else if(c == '.') 
    return LcdCharCode[17];
  else if(c == ':') 
    return LcdCharCode[18];
  else
    return LcdCharCode[19];
}
/***********************************************************************/
void EZ_SetLcdString(widget, str)
     EZ_Widget *widget; char *str;
{
  if(widget && EZ_WidgetType(widget) == EZ_WIDGET_LCD)  
    {
      EZ_UpdateLcd(widget, str);
    }
}
/***********************************************************************/
void EZ_SetLcdInteger(widget, n)
     EZ_Widget *widget; int n;
{
  if(widget && EZ_WidgetType(widget) == EZ_WIDGET_LCD)
    {
      char result[20];
      sprintf(result, "%d", n);
      EZ_UpdateLcd(widget, result);
      /*
      EZ_SetWidgetLabelString(widget, result);
      if(EZ_WidgetMapped(widget)) EZ_DrawWidget(widget);
      */
    }
} 
/***********************************************************************/
void EZ_SetLcdFloat(widget, f)
     EZ_Widget *widget; float f;
{
  if(widget && EZ_WidgetType(widget) == EZ_WIDGET_LCD)
    {
      char result[20];
      sprintf(result, "%g", f);
      EZ_UpdateLcd(widget, result);
    }
} 
/***********************************************************************/
void EZ_ClearLcd (widget)
     EZ_Widget *widget;
{
  if(widget && EZ_WidgetType(widget) == EZ_WIDGET_LCD)
    {
      EZ_LCDString(widget)[0] = 0;
      EZ_LCDStringLength(widget) = 0;
      if(EZ_WidgetMapped(widget)) EZ_DrawWidget(widget);
    }
}
/***********************************************************************/
void EZ_SetLcdNDigits(widget, n)
     EZ_Widget *widget; int n;
{
  if(widget && EZ_WidgetType(widget) == EZ_WIDGET_LCD)
    {
      if(n > 0 && EZ_LCDNDigits(widget) != n)
	{
	  EZ_LCDNDigits(widget) = n;
	  EZ_ClearWidgetSizeComputedFlag(widget);  
	  if(EZ_WidgetMapped(widget)) EZ_ReDisplayWidget(widget);
	}
    }
}
/***********************************************************************/
void EZ_SetLcdFontSize(widget, n)
     EZ_Widget *widget; int n;
{
  if(widget && EZ_WidgetType(widget) == EZ_WIDGET_LCD)
    {
      if(n > 0 && EZ_LCDSize(widget) != n)
	{
	  EZ_LCDSize(widget) = n;
	  EZ_ClearWidgetSizeComputedFlag(widget);  
	  if(EZ_WidgetMapped(widget)) EZ_ReDisplayWidget(widget);
	}
    }
}

void EZ_SetLcdBackground(widget, color)
     EZ_Widget *widget; char *color;
{
  if(widget && color && EZ_WidgetType(widget) == EZ_WIDGET_LCD)
    {
      unsigned long  pixel_value;
      pixel_value = EZ_AllocateColorFromName(color);
      if(EZ_LCDBG(widget) != pixel_value)
	{
	  EZ_LCDBG(widget) = pixel_value;
	  if(EZ_WidgetMapped(widget)) EZ_DrawWidget(widget);
	}
    }
}
void EZ_SetLcdForeground(widget, color)
     EZ_Widget *widget; char *color;
{
  if(widget && color && EZ_WidgetType(widget) == EZ_WIDGET_LCD)
    {
      unsigned long  pixel_value;
      pixel_value = EZ_AllocateNewBackground(color);
      if(pixel_value != 0) 
	{
	  if(EZ_LCDFG(widget) != pixel_value)
	    {
	      EZ_LCDFG(widget) = pixel_value;
	      EZ_LCDFGBright(widget) = EZ_GetBrightColor(pixel_value);
	      EZ_LCDFGDark(widget) = EZ_GetDarkColor(pixel_value);
	      if(EZ_WidgetMapped(widget)) EZ_DrawWidget(widget);
	    }
	}
      else
	{
	  pixel_value = EZ_AllocateColorFromName(color);
	  EZ_LCDFGBright(widget) = ~0;
	  EZ_LCDFGDark(widget) = ~0;
	  EZ_LCDFG(widget) = pixel_value; 
	  if(EZ_WidgetMapped(widget)) EZ_DrawWidget(widget);
	}
    }
  if(EZ_WidgetMapped(widget)) EZ_DrawWidget(widget);
}

/*******************************************************************
 *
 *  Update a LCD
 */
static void  EZ_UpdateLcd(wptr, nstr)
     EZ_Widget *wptr; char *nstr;
{
  int       viewable=1;
  EZ_Widget *pp = wptr;
  while(pp)
    {
      if(EZ_WidgetMapped(pp) == 0) {viewable = 0; break;}
      pp = EZ_WidgetParent(pp);
    }
  if(viewable)
    {
      int       w, h,padb, padb2;
      Window    win;
      GC        backgc, gc, lgc, dgc;
      unsigned long   bgpv;
      Pixmap pixmap;
      int bktile, tile, ltile, dtile, ox, oy;

      bktile=tile=ltile=dtile=ox=oy = 0;
      win = EZ_WidgetWindow(wptr);
      w   = EZ_WidgetWidth(wptr);
      h   = EZ_WidgetHeight(wptr);
      padb = EZ_WidgetPadB(wptr);
      padb2 = padb + padb;
      /* if LCD_BACKGROUND is set, use it */
      if(EZ_LCDBG(wptr) != ~0)
	{
	  bgpv = EZ_LCDBG(wptr);	
	  XSetForeground(EZ_Display, EZ_WRITABLEGC, bgpv);
	  backgc = EZ_WRITABLEGC; /* EZ_WRITABLEGC for background */
	}
      else
	{
	  bktile = EZ_GetBackgroundGC(wptr, &backgc, &bgpv, 1, 0);
	}
      {
	char *ostr, tstr1[256], tstr2[256];
	int loc[2], i, j, ostrlen, nstrlen, ndigits;
	int tmp =  (padb + EZ_WidgetBorderWidth(wptr));
	int tmpx =  (tmp + EZ_WidgetPadX(wptr))<<1;
	int tmpy =  (tmp + EZ_WidgetPadY(wptr))<<1;
	int xlen       = (EZ_WidgetWidth(wptr) - tmpx)*5/(EZ_LCDNDigits(wptr)*(6) + 1);
	int ylen       = (EZ_WidgetHeight(wptr) -tmpy)*5/12;
	int xsep       = xlen*6/5;
	int xx         = ((EZ_WidgetWidth(wptr) - tmpx)- EZ_LCDNDigits(wptr)*xsep + xlen/5)/2;
	int yy         = ((EZ_WidgetHeight(wptr)-tmpy) - ylen*2 )/2;
	int x, y, txx, tyy;

	x = xx + EZ_WidgetPadX(wptr) + EZ_WidgetBorderWidth(wptr) + padb;
	y = yy + EZ_WidgetBorderWidth(wptr) + EZ_WidgetPadY(wptr) + padb;  
	
	nstrlen = (nstr==NULL? 0 : strlen(nstr));
	ostr = EZ_LCDString(wptr);
	ostrlen = EZ_LCDStringLength(wptr);
	ndigits = EZ_LCDNDigits(wptr);
	for(j = ndigits -1, i=ostrlen-1; j >= 0; j--, i--) 
	  { if(i >= 0) tstr1[j] = ostr[i]; else  tstr1[j] = 0; } 
	for(j = ndigits -1, i=nstrlen-1; j >= 0; j--, i--) 
	  { if(i >= 0) tstr2[j] = nstr[i]; else  tstr2[j] = 0; } 
	pixmap = XCreatePixmap(EZ_Display, win, xlen+xlen, ylen+ylen+1, EZ_Depth); 

	/* set up gc */
	if(EZ_WidgetBGPixmapB(wptr) != NULL)
	  {
	    EZ_Bitmap *bitmap = EZ_WidgetBGPixmapB(wptr);
	    gc = EZ_NTILEGC2;
	    dgc = EZ_DTILEGC2;
	    lgc = EZ_BTILEGC2;
	    XSetTile(EZ_Display,EZ_NTILEGC2,bitmap->pixmap);
	    XSetTile(EZ_Display,EZ_DTILEGC2,bitmap->dkpixmap);
	    XSetTile(EZ_Display,EZ_BTILEGC2,bitmap->btpixmap);
	    tile=dtile=ltile = 1;
	  }
	else if(EZ_WidgetParentBGPixmapB(wptr) != NULL)
	  {
	    EZ_Bitmap *bitmap = EZ_WidgetParentBGPixmapB(wptr);
	    gc = EZ_NTILEGC2;
	    dgc = EZ_DTILEGC2;
	    lgc = EZ_BTILEGC2;
	    XSetTile(EZ_Display,EZ_NTILEGC2,bitmap->pixmap);
	    XSetTile(EZ_Display,EZ_DTILEGC2,bitmap->dkpixmap);
	    XSetTile(EZ_Display,EZ_BTILEGC2,bitmap->btpixmap);
	    tile=dtile=ltile = 1;
	  }
	else
	  {
	    if(EZ_GetDarkBrightNormalBDGC(wptr, &dgc, &lgc, &gc))
	      {
		tile=dtile=ltile = 1;
	      }
	  }  

	if(bktile|tile|ltile|dtile)  EZ_GetWidgetTileOrigin(wptr, &ox, &oy);      

	/* override default gc if needed */
	{
	  unsigned long pixel = EZ_LCDFGDark(wptr);
	  if(pixel != ~0)
	    {
	      XSetForeground(EZ_Display, EZ_BD_DARKGC, pixel);
	      dgc = EZ_BD_DARKGC;
	      dtile = 0;
	    }
	  pixel = EZ_LCDFGBright(wptr);
	  if(pixel != ~0)
	    {
	      XSetForeground(EZ_Display, EZ_BD_BRIGHTGC, pixel);
	      lgc = EZ_BD_BRIGHTGC;	
	      ltile = 0;
	    }
	  pixel = EZ_LCDFG(wptr);
	  if(pixel != ~0)
	    {
	      XSetForeground(EZ_Display, EZ_BD_NORMALGC, pixel);
	      gc = EZ_BD_NORMALGC;
	      tile = 0;
	    }
	  else
	    {
	      pixel = EZ_WidgetForeground(wptr);
	      if(pixel != EZ_DefaultForeground)
		{
		  XSetForeground(EZ_Display, EZ_BD_NORMALGC, pixel);
		  gc = EZ_BD_NORMALGC;
		}
	    }
	}
	for(i = 0; i < ndigits; i++)
	  {
	    if(tstr1[i] != tstr2[i])
	      {
		int c = tstr2[i]; 
		int desX = x + i * xsep;
		int desY = y;

		loc[0] = 0; loc[1] = 0;
		txx = ox - desX;  tyy = oy - desY;
		if(bktile)  XSetTSOrigin(EZ_Display, backgc, txx, tyy);
		if(tile)  XSetTSOrigin(EZ_Display, gc, txx, tyy);
		if(dtile)  XSetTSOrigin(EZ_Display, dgc, txx, tyy);
		if(ltile)  XSetTSOrigin(EZ_Display, lgc, txx, tyy);
		DrawLCDDigit(wptr, pixmap, backgc, gc, dgc, lgc, loc, c, xlen, ylen);
		XCopyArea(EZ_Display,pixmap,win, EZ_WRITABLEGC,
			  0,0, xlen, ylen+ylen+1, x + i * xsep, y);
	      }
	  }
	XFreePixmap(EZ_Display, pixmap); 
      }
    }
  /* finally, save the new string */
  EZ_SetWidgetLabelString(wptr, nstr);
}

/***********************************************************************/
#undef _EZ_WIDGET_LCD_C_
