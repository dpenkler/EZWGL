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
 *  Border Stuff.
 */
#define _EZ_WIDGET_BORDER_C_

#include "EZ_Widget.h"
/*******************************************************************
 *
 * Functions implemented in this file:
 */

void                EZ_FillRectBorderCoor MY_ANSIARGS((
						       int width, int height, int bdwidth,
						       int xoff, int yoff,
						       XPoint *top, XPoint *bottom,
						       XPoint *left, XPoint *right));
void                EZ_FillDiamondBorderCoor MY_ANSIARGS((
							  int width, int height, int bdwidth,
							  int xoff, int yoff,
							  XPoint *tr, XPoint *bl,
							  XPoint *tl, XPoint *br));

void                EZ_FillShadowBorderCoor MY_ANSIARGS((
						       int width, int height, int bdwidth,
						       int xoff, int yoff,
						       XPoint *top, XPoint *bottom,
						       XPoint *left, XPoint *right, int d3));

void                EZ_DrawRectBorder MY_ANSIARGS((EZ_Widget *widget, Drawable drawable));
void                EZ_DrawRectBorderWithSize MY_ANSIARGS((
							   EZ_Widget *widget, Drawable drawable,
							   int x, int y, int w, int h, int bw,
							   int bdtype, int a, int b, int c));
void                EZ_DrawDiamondBorderWithSize MY_ANSIARGS((
							      EZ_Widget *widget, Drawable drawable,
							      int x, int y, int w, int h, int bw,
							      int bdtype, int a, int b, int c));
void                EZ_RenderBorder  MY_ANSIARGS((
						  Drawable  drawable,
						  int       type,
						  EZ_BorderPiece *top, 
						  EZ_BorderPiece *bottom, 
						  EZ_BorderPiece *left,
						  EZ_BorderPiece *right,
						  EZ_BorderPiece *extra,
						  EZ_BorderPiece *extraB  ));

void                EZ_FillNBBorderCoor  MY_ANSIARGS((int width, int height, int bwidth, int padb,
						      int xoffset, int yoffset,
						      XPoint *left, XPoint *ul, XPoint *top,
						      XPoint *ur, XPoint *right  ));
void                EZ_FillNBBorderCoorB  MY_ANSIARGS((int width, int height, int bwidth, int padb,
						      int xoffset, int yoffset,
						      XPoint *left, XPoint *ul, XPoint *top,
						      XPoint *ur, XPoint *right  ));
void                EZ_FillNBBorderCoorL  MY_ANSIARGS((int width, int height, int bwidth, int padb,
						      int xoffset, int yoffset,
						      XPoint *left, XPoint *ul, XPoint *top,
						      XPoint *ur, XPoint *right  ));
void                EZ_FillNBBorderCoorR  MY_ANSIARGS((int width, int height, int bwidth, int padb,
						      int xoffset, int yoffset,
						      XPoint *left, XPoint *ul, XPoint *top,
						      XPoint *ur, XPoint *right  ));
void                EZ_SetupUpBdGC MY_ANSIARGS((
						EZ_Widget *widget,
						GC        *left,  GC *uleft, GC *top, GC *uright,
						GC *right, GC *bottom, int a, int b, int c));
void                EZ_SetupDownBdGC MY_ANSIARGS((
						  EZ_Widget *widget,
						  GC        *left,  GC *uleft, GC *top, GC *uright,
						  GC *right, GC *bottom, int a, int b, int c));

void                EZ_DrawFocusIndicator MY_ANSIARGS((EZ_Widget *widget, Drawable drawable, 
						       int inout));

void              EZ_FillSliderBorderCoor MY_ANSIARGS((int width, int height, int bwidth, 
                                                       int xoffset, int yoffset,
                                                       XPoint *uleft, XPoint *left, XPoint *top,
                                                       XPoint *right, XPoint *uright, int type  ));


/******************************************************************/

#define EZ_BORDER_POLYGON               1
#define EZ_BORDER_LINE                  2
#define EZ_BORDER_SHADOW_LP             3

/******************************************************************
 * 
 * Draw Border.  Border are like the following:
 *
 *     -----------------------------------------------------
 *    |\                TOP, BrightGray2                   /|
 *    | \-------------------------------------------------/ |
 *    |  |                                               |  |
 *    |  |                                               |  |
 *    |  |                                               |  |
 *    |  |                                               |  |
 *    |  |                                               |  |
 *    |  |                                               |  |
 *    |  |                                               |  |  <- Right 
 *    |  | <- Left                                       |  |    DrakGray1
 *    |  |      BrightGray1                              |  |
 *    |  |                                               |  |
 *    |  |                                               |  |
 *    |  |            THE WIDGET                         |  |
 *    |  |                                               |  |
 *    |  |                                               |  |
 *    |  |                                               |  |
 *    |  |                                               |  |
 *    |  |                                               |  |
 *    |  |                                               |  |
 *    |  /-----------------------------------------------\  |
 *    | /           Bottom DarkGray2                      \ |
 *     ------------------------------------------------------
 *
 ******************************************************************
 *
 *  All drawing are done in a pixmap and then copied to the 
 *  widget window.
 *
 ******************************************************************/

/******************************************************************
 *
 *  Draw the Rectangular border of a Rectangular shaped widget.
 *  Note:  A widget always fill completely its supporting window !
 */
void  EZ_DrawRectBorder(widget, drawable)
     EZ_Widget *widget;
     Drawable        drawable;
{
  int                w,h,bw, padb, padb2, secode;

  bw  = EZ_WidgetBorderWidth(widget);
  w   = EZ_WidgetWidth(widget);
  h   = EZ_WidgetHeight(widget);
  padb= EZ_WidgetPadB(widget);
  padb2 = padb+padb;

  if( bw > 0) 
    EZ_DrawRectBorderWithSize(widget, drawable, padb,padb, w-padb2,h-padb2,bw,
			      (int)(EZ_WidgetBorderStyle(widget)), 0, 0 ,0);
  
  if(EZ_WeAreInFocus != 0 && widget == EZ_CurrentFocusingWidget)
    EZ_DrawFocusIndicator(widget, drawable, FocusIn);
  else if(widget == EZ_CurrentFlashWidget)
    {
      w--; h--;
      EZ_FlashWidgetWindowBorder(widget, drawable, 0, 0, w, h, 0);
    }
  else if(widget == EZ_CurrentFlashWidget2)
    {
      w--; h--;
      EZ_FlashWidgetWindowBorder(widget, drawable, 0, 0, w, h, 1);
    }
  /* inspired by arkeia */
  secode = EZ_WidgetSECode(widget);
  if(secode == EZ_NAIL_IT)
    {
#include "xpms/screws.xpm"
      extern EZ_BitmapHTEntry *EZ_Nails[4];
      int nail;
      int offset = bw+padb + 4;
      unsigned int ww, hh;
      Pixmap pixmap, shape;
      GC gc = EZ_WRITABLEGC;
      int separam = (int) EZ_WidgetSEParam(widget);

      nail = ((separam & 0x30)>>4);
      if(EZ_Nails[0] == NULL)
        {
          EZ_Nails[0] = EZ_CreateLabelPixmapFromXpmData(screwBig);
          EZ_Nails[1] = EZ_CreateLabelPixmapFromXpmData(screwSmall);
          EZ_Nails[2] = EZ_CreateLabelPixmapFromXpmData(nailBig);
          EZ_Nails[3] = EZ_CreateLabelPixmapFromXpmData(nailSmall);
        }
      pixmap = EZ_Nails[nail]->pixmap;
      shape =  EZ_Nails[nail]->shape;
      ww =  (unsigned) EZ_Nails[nail]->width;
      hh =  (unsigned) EZ_Nails[nail]->height;
      XSetClipMask(EZ_Display, gc, shape);
      if(separam & 1)
        {
          XSetClipOrigin(EZ_Display, gc,offset,offset);
          XCopyArea(EZ_Display, pixmap, drawable, gc, 0, 0,  ww, hh, offset, offset);
          XSetClipOrigin(EZ_Display, gc,offset,h-hh-offset);
          XCopyArea(EZ_Display, pixmap, drawable, gc, 0, 0,  ww, hh, offset, h-hh-offset);
          XSetClipOrigin(EZ_Display, gc,w-ww-offset,h-hh-offset);
          XCopyArea(EZ_Display, pixmap, drawable, gc, 0, 0,  ww, hh, w-ww-offset, h-hh-offset);
          XSetClipOrigin(EZ_Display, gc,w-ww-offset,offset);
          XCopyArea(EZ_Display, pixmap, drawable, gc, 0, 0,  ww, hh, w-ww-offset, offset);
        }
      if(separam & 2)
        {
          int htmp = (h-hh)>>1;
          XSetClipOrigin(EZ_Display, gc,offset,htmp);
          XCopyArea(EZ_Display, pixmap, drawable, gc, 0, 0,  ww, hh, offset, htmp);
          XSetClipOrigin(EZ_Display, gc,w-ww-offset,htmp);
          XCopyArea(EZ_Display, pixmap, drawable, gc, 0, 0,  ww, hh, w-ww-offset, htmp);
        }
      if(separam & 4)
        {
          int wtmp = (w-ww)>>1;
          XSetClipOrigin(EZ_Display, gc,wtmp, offset);
          XCopyArea(EZ_Display, pixmap, drawable, gc, 0, 0,  ww, hh, wtmp, offset);
          XSetClipOrigin(EZ_Display, gc,wtmp, h-hh-offset);
          XCopyArea(EZ_Display, pixmap, drawable, gc, 0, 0,  ww, hh, wtmp, h-hh-offset);
        }
      if(separam & 8)
        {
          int htmp = (h-hh)>>1;
          int wtmp = (w-ww)>>1;
          XSetClipOrigin(EZ_Display, gc,wtmp,htmp);
          XCopyArea(EZ_Display, pixmap, drawable, gc, 0, 0,  ww, hh, wtmp, htmp);
        }
      XSetClipMask(EZ_Display, gc, None);
    }
}

/*********************************************************************************/
void EZ_DrawFocusIndicator(widget, drawable, inout)
     EZ_Widget *widget; Drawable drawable; int inout;
{
  int  padb= EZ_WidgetPadB(widget);
  int  tile = 0;
  XRectangle rects[4];

  if(padb > 0)
    {
      int  w   = EZ_WidgetWidth(widget) - 1;
      int  h   = EZ_WidgetHeight(widget) -1;
      int  q = (padb > 1? 2 : 1);
      int  z = q-1;
      int  style, refresh = 0;

      GC gc;
      unsigned long pv;
      if(drawable == (Drawable)0) { drawable = EZ_WidgetWindow(widget); refresh = 1; }
      if(inout == FocusIn)
	{
	  XSetForeground(EZ_Display, EZ_WRITABLEGC2, EZ_KbdFocusHighlightColor);
	  gc = EZ_WRITABLEGC2;
	}
      else
	{
	  tile = EZ_GetParentBgGC(widget, &gc, &pv);
	  if(tile)
	    {
	      int tx, ty;
	      EZ_GetWidgetTileOrigin(EZ_WidgetParent(widget), &tx, &ty);
	      if(tile > 1)
		{ tx -= EZ_WidgetOriginX(widget);  ty -= EZ_WidgetOriginY(widget); }
	      XSetTSOrigin(EZ_Display, gc, tx, ty);
	    }
	}
      style = EZ_WidgetBorderStyle(widget);
      switch(style)
	{
	case EZ_BORDER_NB_UP:
	case EZ_BORDER_NB_DOWN:
	  rects[0].x = 0;           rects[0].y = 0;
	  rects[0].width = w;       rects[0].height = q;
	  rects[1].x = 0;           rects[1].y = 0;
	  rects[1].width = q;       rects[1].height = h;
	  rects[2].x = w-z;         rects[2].y = 0;
	  rects[2].width = q;      rects[2].height = h;
	  XFillRectangles(EZ_Display, drawable, gc, rects, 3); 
	  /*
	  XFillRectangle(EZ_Display, drawable, gc, 0,0, w, 2); 
	  XFillRectangle(EZ_Display, drawable, gc, 0,0, 2, h); 
	  XFillRectangle(EZ_Display, drawable, gc, w-1,0, 2, h); 
	  */
	  /*XDrawLine(EZ_Display, drawable, gc, 0,1, w, 1);
	  XDrawLine(EZ_Display, drawable, gc, w,0, w, h);
	  XDrawLine(EZ_Display, drawable, gc, 1,h, 1, 1); */
	  break;
	case EZ_BORDER_NB_UP_B:
	case EZ_BORDER_NB_DOWN_B:
	  rects[0].x = 0;           rects[0].y = 0;
	  rects[0].width = q;       rects[0].height = h;
	  rects[1].x = q;           rects[1].y = h-z;
	  rects[1].width = w;       rects[1].height = q;
	  rects[2].x = w-z;         rects[2].y = 0;
	  rects[2].width = q;      rects[2].height = h;
	  XFillRectangles(EZ_Display, drawable, gc, rects, 3); 
	  /*
	  XFillRectangle(EZ_Display, drawable, gc, w,0, 2, h); 
	  XFillRectangle(EZ_Display, drawable, gc, 0,0, 2, h); 
	  XFillRectangle(EZ_Display, drawable, gc, 0,h, w, 2); 
	  */
	  /*XDrawLine(EZ_Display, drawable, gc, w,0, w, h);
	  XDrawLine(EZ_Display, drawable, gc, 1,h, 1, 1);
	  XDrawLine(EZ_Display, drawable, gc, w+1,h, 0, h);*/
	  break;
	case EZ_BORDER_NB_UP_L:
	case EZ_BORDER_NB_DOWN_L:
	  rects[0].x = 0;           rects[0].y = 0;
	  rects[0].width = w;       rects[0].height = q;
	  rects[1].x = 0;           rects[1].y = 0;
	  rects[1].width = q;       rects[1].height = h;
	  rects[2].x = q;           rects[2].y = h-z;
	  rects[2].width = w+1;     rects[2].height = q;
	  XFillRectangles(EZ_Display, drawable, gc, rects, 3); 
	  /*
	  XDrawLine(EZ_Display, drawable, gc, 0,1, w, 1);
	  XDrawLine(EZ_Display, drawable, gc, 1,h, 1, 1);
	  XDrawLine(EZ_Display, drawable, gc, w+1,h, 0, h);
	  */
	  break;
	case EZ_BORDER_NB_UP_R:
	case EZ_BORDER_NB_DOWN_R:
	  rects[0].x = 0;           rects[0].y = 0;
	  rects[0].width = w;       rects[0].height = q;
	  rects[1].x = w-z;         rects[1].y = 0;
	  rects[1].width = q;       rects[1].height = h;
	  rects[2].x = 0;           rects[2].y = h-z;
	  rects[2].width = w+1;     rects[2].height = q;
	  XFillRectangles(EZ_Display, drawable, gc, rects, 3); 
	  /*
	  XDrawLine(EZ_Display, drawable, gc, 0,1, w, 1);
	  XDrawLine(EZ_Display, drawable, gc, w,0, w, h);
	  XDrawLine(EZ_Display, drawable, gc, w+1,h, 0, h);
	  */
	  break;
	default:
	  rects[0].x = 0;           rects[0].y = 0;
	  rects[0].width = w;       rects[0].height = q;
	  rects[1].x = w-z;         rects[1].y = 0;
	  rects[1].width = q;       rects[1].height = h;
	  rects[2].x = 0;           rects[2].y = h-z;
	  rects[2].width = w+1;     rects[2].height = q;
	  rects[3].x = 0;           rects[3].y = 0;
	  rects[3].width = q;       rects[3].height = h;
	  XFillRectangles(EZ_Display, drawable, gc, rects, 4); 
	  /*
	  XFillRectangle(EZ_Display, drawable, gc, 0,0, w, 2); 
	  XFillRectangle(EZ_Display, drawable, gc, w-1,0, 2, h); 
	  XFillRectangle(EZ_Display, drawable, gc, 0,h-1, w, 2); 
	  XFillRectangle(EZ_Display, drawable, gc, 0,0, 2, h); 
	  */
	  /*
	  XDrawLine(EZ_Display, drawable, gc, 0,1, w, 1);
	  XDrawLine(EZ_Display, drawable, gc, w,0, w, h);
	  XDrawLine(EZ_Display, drawable, gc, 1,h, 1, 1);	  
	  XDrawLine(EZ_Display, drawable, gc, w+1,h, 0, h);
	  */
	  break;
	}
      if(refresh) XSync(EZ_Display, False);
    }
}
/*********************************************************************************
 *
 *  Draw Rectangular border.
 *
 *   aaz:     flags 
 *            bit 1:  signal to use alternate tile pixmap if
 *                     available (arrows, slider btns and scroll btns)
 *            bit 2:  signal not to fixed the corners
 *                    of shadow borders. Used in rendering
 *                    interior border of labeled frames and
 *                    display items.
 *            bit 3:  signal that (ax,ay) to be added to the tile offset.
 */
void EZ_SetupUpBdGC(widget, left, uleft, top, uright,right, bottom, aax, aay, aaz)
     EZ_Widget *widget;
     GC        *top, *bottom, *left, *right, *uleft, *uright;     
     int       aax, aay, aaz;
{
  unsigned long pixel;
  int az1 = aaz & 1;
  int az3 = aaz & 4;
  int ax, ay;

  if(az1)
    {
      if(EZ_WidgetBGPixmapB(widget) != NULL)
	{
	  EZ_Bitmap *bitmap = EZ_WidgetBGPixmapB(widget);
	  *right = *bottom = EZ_DTILEGC1;
	  *uleft = *uright = *top = *left = EZ_BTILEGC1;      
	  XSetTile(EZ_Display,EZ_DTILEGC1,bitmap->dkpixmap);
	  XSetTile(EZ_Display,EZ_BTILEGC1,bitmap->btpixmap);
	  XSetTSOrigin(EZ_Display, EZ_DTILEGC1, aax, aay);
	  XSetTSOrigin(EZ_Display, EZ_BTILEGC1, aax, aay);
	  return;
	}
      else if(EZ_WidgetParentBGPixmapB(widget) != NULL)
	{
	  EZ_Bitmap *bitmap = EZ_WidgetParentBGPixmapB(widget);
	  *right = *bottom = EZ_DTILEGC1;
	  *uleft = *uright = *top = *left = EZ_BTILEGC1;      
	  XSetTile(EZ_Display,EZ_DTILEGC1,bitmap->dkpixmap);
	  XSetTile(EZ_Display,EZ_BTILEGC1,bitmap->btpixmap);
	  XSetTSOrigin(EZ_Display, EZ_DTILEGC1, aax, aay);
	  XSetTSOrigin(EZ_Display, EZ_BTILEGC1, aax, aay);
	  return;
	}
    }

  if(az3) {ax = aax; ay = aay;} else {ax = 0; ay = 0;}
  
  if(EZ_WidgetBackground(widget))
    {
      pixel = EZ_GetDarkColor(EZ_WidgetBackground(widget));
      XSetForeground(EZ_Display, EZ_BD_DARKGC, pixel);
      pixel = EZ_GetBrightColor(EZ_WidgetBackground(widget));
      XSetForeground(EZ_Display, EZ_BD_BRIGHTGC, pixel);
      *right = *bottom = EZ_BD_DARKGC;
      *uleft = *uright = *top = *left = EZ_BD_BRIGHTGC;
    }
  else if(EZ_WidgetBGPixmap(widget) != NULL)
    {
      EZ_Bitmap *bitmap = EZ_WidgetBGPixmap(widget);
      *right = *bottom = EZ_DTILEGC1;
      *uleft = *uright = *top = *left = EZ_BTILEGC1;      
      XSetTile(EZ_Display,EZ_DTILEGC1,bitmap->dkpixmap);
      XSetTile(EZ_Display,EZ_BTILEGC1,bitmap->btpixmap);
      {
	int ox, oy; 
	EZ_GetWidgetTileOrigin(widget, &ox, &oy);
	ox += ax; oy += ay;
	XSetTSOrigin(EZ_Display, EZ_DTILEGC1, ox, oy);
	XSetTSOrigin(EZ_Display, EZ_BTILEGC1, ox, oy);
      }
    }
  else if(EZ_WidgetParentBG(widget) && *(EZ_WidgetParentBG(widget)))
    {
      pixel = EZ_GetDarkColor(*(EZ_WidgetParentBG(widget)));
      XSetForeground(EZ_Display, EZ_BD_DARKGC, pixel);
      pixel = EZ_GetBrightColor(*(EZ_WidgetParentBG(widget)));
      XSetForeground(EZ_Display, EZ_BD_BRIGHTGC, pixel);
      *uleft = *uright = *top = *left = EZ_BD_BRIGHTGC;	  
      *right = *bottom = EZ_BD_DARKGC;
    }
  else if(EZ_WidgetParentBGPixmap(widget) != NULL)
    {
      EZ_Bitmap *bitmap = EZ_WidgetParentBGPixmap(widget);
      *right = *bottom = EZ_DTILEGC1;
      *uleft = *uright = *top = *left = EZ_BTILEGC1;      
      XSetTile(EZ_Display,EZ_DTILEGC1,bitmap->dkpixmap);
      XSetTile(EZ_Display,EZ_BTILEGC1,bitmap->btpixmap);
      {
	int ox, oy; 
	EZ_GetWidgetTileOrigin(widget, &ox, &oy);
	ox += ax; oy += ay;
	XSetTSOrigin(EZ_Display, EZ_DTILEGC1, ox, oy);
	XSetTSOrigin(EZ_Display, EZ_BTILEGC1, ox, oy);
      }
    }
  else
    {
      *top = EZ_BRIGHTGC2;  *bottom = EZ_DARKGC2;
      *left = EZ_BRIGHTGC1;  *right = EZ_DARKGC1;
      *uleft = EZ_BRIGHTGC2; *uright = EZ_BRIGHTGC1;
    }  
}

void EZ_SetupDownBdGC(widget, left, uleft, top, uright,right, bottom, aax, aay, aaz)
     EZ_Widget *widget;
     GC        *top, *bottom, *left, *right, *uleft, *uright;     
     int       aax,aay,aaz;
{
  unsigned long pixel;
  int az1 = aaz & 1;
  int az3 = aaz & 4;
  int ax, ay;

  if(az1)
    {
      if(EZ_WidgetBGPixmapB(widget) != NULL)
	{
	  EZ_Bitmap *bitmap = EZ_WidgetBGPixmapB(widget);
	  *right = *bottom = EZ_BTILEGC1;
	  *uleft = *uright = *top = *left = EZ_DTILEGC1;      
	  XSetTile(EZ_Display,EZ_DTILEGC1,bitmap->dkpixmap);
	  XSetTile(EZ_Display,EZ_BTILEGC1,bitmap->btpixmap);
	  XSetTSOrigin(EZ_Display, EZ_DTILEGC1, aax, aay);
	  XSetTSOrigin(EZ_Display, EZ_BTILEGC1, aax, aay);
	  return;
	}
      else if(EZ_WidgetParentBGPixmapB(widget) != NULL)
	{
	  EZ_Bitmap *bitmap = EZ_WidgetParentBGPixmapB(widget);
	  *right = *bottom = EZ_BTILEGC1;
	  *uleft = *uright = *top = *left = EZ_DTILEGC1;      	  
	  XSetTile(EZ_Display,EZ_DTILEGC1,bitmap->dkpixmap);
	  XSetTile(EZ_Display,EZ_BTILEGC1,bitmap->btpixmap);
	  XSetTSOrigin(EZ_Display, EZ_DTILEGC1, aax, aay);
	  XSetTSOrigin(EZ_Display, EZ_BTILEGC1, aax, aay);
	  return;
	}
    }
  if(az3) {ax = aax; ay = aay;} else {ax = 0; ay = 0;}

  if(EZ_WidgetBackground(widget))
    {
      pixel = EZ_GetDarkColor(EZ_WidgetBackground(widget));
      XSetForeground(EZ_Display, EZ_BD_DARKGC, pixel);
      pixel = EZ_GetBrightColor(EZ_WidgetBackground(widget));
      XSetForeground(EZ_Display, EZ_BD_BRIGHTGC, pixel);
      *right = *bottom = EZ_BD_BRIGHTGC;
      *uleft = *uright = *left = *top = EZ_BD_DARKGC;
    }
  else if(EZ_WidgetBGPixmap(widget) != NULL)
    {
      EZ_Bitmap *bitmap = EZ_WidgetBGPixmap(widget);
      *right = *bottom = EZ_BTILEGC1;
      *uleft = *uright = *top = *left = EZ_DTILEGC1;      
      XSetTile(EZ_Display,EZ_DTILEGC1,bitmap->dkpixmap);
      XSetTile(EZ_Display,EZ_BTILEGC1,bitmap->btpixmap);
      {
	int ox, oy; 
	EZ_GetWidgetTileOrigin(widget, &ox, &oy);
	ox += ax; oy += ay;
	XSetTSOrigin(EZ_Display, EZ_DTILEGC1, ox, oy);
	XSetTSOrigin(EZ_Display, EZ_BTILEGC1, ox, oy);
      }
    }
  else if(EZ_WidgetParentBG(widget) && *(EZ_WidgetParentBG(widget)))
    {
      pixel = EZ_GetDarkColor(*(EZ_WidgetParentBG(widget)));
      XSetForeground(EZ_Display, EZ_BD_DARKGC, pixel);
      pixel = EZ_GetBrightColor(*(EZ_WidgetParentBG(widget)));
      XSetForeground(EZ_Display, EZ_BD_BRIGHTGC, pixel);
      *uleft = *uright = *left = *top = EZ_BD_DARKGC;
      *bottom = *right = EZ_BD_BRIGHTGC;	  
    }
  else if(EZ_WidgetParentBGPixmap(widget) != NULL)
    {
      EZ_Bitmap *bitmap = EZ_WidgetParentBGPixmap(widget);
      *right = *bottom = EZ_BTILEGC1;
      *uleft = *uright = *top = *left = EZ_DTILEGC1;      
      XSetTile(EZ_Display,EZ_DTILEGC1,bitmap->dkpixmap);
      XSetTile(EZ_Display,EZ_BTILEGC1,bitmap->btpixmap);
      {
	int ox, oy; 
	EZ_GetWidgetTileOrigin(widget, &ox, &oy);
	ox += ax; oy += ay;
	XSetTSOrigin(EZ_Display, EZ_DTILEGC1, ox, oy);
	XSetTSOrigin(EZ_Display, EZ_BTILEGC1, ox, oy);
      }
    }
  else
    {
      *uleft = *top = EZ_DARKGC2;  *bottom = EZ_BRIGHTGC2;
      *uright = *left = EZ_DARKGC1;  *right = EZ_BRIGHTGC1;  
    }  
}
/********************************************************************/

void EZ_DrawRectBorderWithSize(widget, drawable, x,y,w,h,bw, bdtype, ax, ay, az)
     EZ_Widget *widget;
     Drawable  drawable;
     int       x,y,w,h, bdtype,bw; 
     int       ax, ay, az;
{
  EZ_BorderPiece     top, bottom, left, right, uleft, uright;
  int                padb, bw2 = bw << 1;
  GC                 gc; 
  int                az3 = az & 4;  
  int                aax,aay;

  if(az3) {aax = ax; aay = ay;} else {aax =0; aay = 0;}
  switch(bdtype)
    {
    case  EZ_BORDER_NONE:          /* nothing need to be done    */
      break;

    case EZ_BORDER_FLAT:           /* draw a black flat border   */
      EZ_SetupUpBdGC(widget, &left.gc, &uleft.gc, &top.gc,&uright.gc, &right.gc, &bottom.gc,ax, ay, az);
      top.gc = left.gc = right.gc = bottom.gc;
      EZ_FillRectBorderCoor(w,h,bw,x,y,top.xp,bottom.xp,left.xp,right.xp);
      EZ_RenderBorder(drawable,EZ_BORDER_POLYGON, &top, &bottom, &left, &right, 0, 0);
      break;
    case  EZ_BORDER_TB_FLAT:
      EZ_SetupUpBdGC(widget, &left.gc, &uleft.gc, &top.gc,&uright.gc, &right.gc, &bottom.gc,ax, ay, az);
      EZ_FillRectBorderCoor(w,h,bw,x,y, top.xp,bottom.xp,left.xp,right.xp);
      top.xp[2].x=top.xp[1].x; top.xp[3].x=top.xp[0].x;
      bottom.xp[2].x=bottom.xp[1].x; bottom.xp[3].x=bottom.xp[0].x;
      top.gc = bottom.gc;
      EZ_RenderBorder(drawable,EZ_BORDER_POLYGON, &top, &bottom, &top, &bottom,0, 0);  
      break; 

    case  EZ_BORDER_LR_FLAT:
      EZ_SetupUpBdGC(widget, &left.gc, &uleft.gc, &top.gc,&uright.gc, &right.gc, &bottom.gc,ax, ay, az);
      EZ_FillRectBorderCoor(w,h,bw,x,y, top.xp,bottom.xp,left.xp,right.xp);
      left.xp[2].y=left.xp[1].y; left.xp[3].y=left.xp[0].y;
      right.xp[2].y=right.xp[1].y; right.xp[3].y=right.xp[0].y; 
      left.gc = right.gc = bottom.gc;
      EZ_RenderBorder(drawable,EZ_BORDER_POLYGON, &left, &right, &left, &right,0, 0);  
      break; 

    case EZ_BORDER_FRAMED_UP:
    case EZ_BORDER_FRAMED_DOWN:
      {
	GC  gc;
	unsigned long bgpv;
	if(bw > 0)
	  {
	    int tile = EZ_GetBackgroundGC(widget, &gc, &bgpv, 0, 0);
	    XRectangle rects[4];
	    if(tile)
	      {
		int ox, oy; 
		EZ_GetWidgetTileOrigin(widget, &ox, &oy);
                ox += aax; oy += aay;
		XSetTSOrigin(EZ_Display, gc, ox, oy);
	      }
	    rects[0].x = x;        rects[0].y = y; 
	    rects[0].width = w;    rects[0].height = bw;
	    rects[1].x = x;        rects[1].y = y; 
	    rects[1].width = bw;   rects[1].height = h;
	    rects[2].x = x+w-bw;   rects[2].y = y; 
	    rects[2].width = bw;   rects[2].height = h;
	    rects[3].x = x;        rects[3].y = h-bw+y; 
	    rects[3].width = w;    rects[3].height = bw;
	    XFillRectangles(EZ_Display, drawable, gc, rects, 4);
	    /*
	    XFillRectangle(EZ_Display, drawable, gc, x, y, (unsigned int)w, (unsigned int)bw);
	    XFillRectangle(EZ_Display, drawable, gc, x, y, (unsigned int)bw, (unsigned int)h);
	    XFillRectangle(EZ_Display, drawable, gc, x+w-bw,y,(unsigned int)bw, (unsigned int)h);
	    XFillRectangle(EZ_Display, drawable, gc, x,h-bw+y,(unsigned int)w, (unsigned int)bw);
	    */
	  }
      }
      if(bdtype == EZ_BORDER_FRAMED_UP)
	EZ_SetupUpBdGC(widget, &left.gc, &uleft.gc, &top.gc,&uright.gc, &right.gc, &bottom.gc,ax, ay, az);
      else
	EZ_SetupDownBdGC(widget, &left.gc, &uleft.gc, &top.gc,&uright.gc, &right.gc, &bottom.gc,ax, ay, az);
      EZ_FillRectBorderCoor(w-1,h-1,0,x,y,top.xp,bottom.xp,left.xp,right.xp);
      EZ_RenderBorder(drawable,EZ_BORDER_LINE, &top, &bottom, &left, &right,0, 0);
      if(bdtype == EZ_BORDER_FRAMED_UP)
	EZ_SetupDownBdGC(widget, &left.gc, &uleft.gc, &top.gc,&uright.gc, &right.gc, &bottom.gc,ax, ay, az);
      else
	EZ_SetupUpBdGC(widget, &left.gc, &uleft.gc, &top.gc,&uright.gc, &right.gc, &bottom.gc,ax, ay, az);
      EZ_FillRectBorderCoor(w-bw2-1,h-bw2-1,0,x+bw, y+bw, top.xp,bottom.xp,left.xp,right.xp);
      EZ_RenderBorder(drawable,EZ_BORDER_LINE, &top, &bottom, &left, &right,0, 0);
      break;

    case  EZ_BORDER_UP:
      EZ_SetupUpBdGC(widget, &left.gc, &uleft.gc, &top.gc,&uright.gc, &right.gc, &bottom.gc,ax, ay, az);
      EZ_FillRectBorderCoor(w,h,bw,x,y, top.xp,bottom.xp,left.xp,right.xp);
      EZ_RenderBorder(drawable,EZ_BORDER_POLYGON, &top, &bottom, &left, &right,0, 0);     
      break; 

    case  EZ_BORDER_DOWN:
      EZ_SetupDownBdGC(widget, &left.gc, &uleft.gc, &top.gc,&uright.gc, &right.gc, &bottom.gc,ax, ay, az);
      EZ_FillRectBorderCoor(w,h,bw,x,y,top.xp,bottom.xp,left.xp,right.xp);
      EZ_RenderBorder(drawable,EZ_BORDER_POLYGON, &top, &bottom, &left, &right,0, 0);  
      break; 

    case  EZ_BORDER_TB_UP:
      EZ_SetupUpBdGC(widget, &left.gc, &uleft.gc, &top.gc,&uright.gc, &right.gc, &bottom.gc,ax, ay, az);
      EZ_FillRectBorderCoor(w,h,bw,x,y, top.xp,bottom.xp,left.xp,right.xp);
      top.xp[2].x=top.xp[1].x; top.xp[3].x=top.xp[0].x;
      bottom.xp[2].x=bottom.xp[1].x; bottom.xp[3].x=bottom.xp[0].x;
      EZ_RenderBorder(drawable,EZ_BORDER_POLYGON, &top, &bottom, &top, &bottom,0, 0);  
      break; 

    case  EZ_BORDER_TB_DOWN:
      EZ_SetupDownBdGC(widget, &left.gc, &uleft.gc, &top.gc,&uright.gc, &right.gc, &bottom.gc,ax, ay, az);
      EZ_FillRectBorderCoor(w,h,bw,x,y,top.xp,bottom.xp,left.xp,right.xp);
      top.xp[2].x=top.xp[1].x; top.xp[3].x=top.xp[0].x;
      bottom.xp[2].x=bottom.xp[1].x; bottom.xp[3].x=bottom.xp[0].x;
      EZ_RenderBorder(drawable,EZ_BORDER_POLYGON, &top, &bottom, &top, &bottom,0, 0);  
      break; 

    case  EZ_BORDER_LR_UP:
      EZ_SetupUpBdGC(widget, &left.gc, &uleft.gc, &top.gc,&uright.gc, &right.gc, &bottom.gc,ax, ay, az);
      EZ_FillRectBorderCoor(w,h,bw,x,y, top.xp,bottom.xp,left.xp,right.xp);
      left.xp[2].y=left.xp[1].y; left.xp[3].y=left.xp[0].y;
      right.xp[2].y=right.xp[1].y; right.xp[3].y=right.xp[0].y; 
      EZ_RenderBorder(drawable,EZ_BORDER_POLYGON, &left, &right, &left, &right,0, 0);  
      break; 

    case  EZ_BORDER_LR_DOWN:
      EZ_SetupDownBdGC(widget, &left.gc, &uleft.gc, &top.gc,&uright.gc, &right.gc, &bottom.gc,ax, ay, az);
      EZ_FillRectBorderCoor(w,h,bw,x,y,top.xp,bottom.xp,left.xp,right.xp);
      left.xp[2].y=left.xp[1].y; left.xp[3].y=left.xp[0].y;
      right.xp[2].y=right.xp[1].y; right.xp[3].y=right.xp[0].y; 
      EZ_RenderBorder(drawable,EZ_BORDER_POLYGON, &left, &right, &left, &right,0, 0);  
      break; 

    case EZ_BORDER_NB_UP_L:
      {
	EZ_Bitmap *bitmap;
	padb = EZ_WidgetPadB(widget);
	EZ_FillNBBorderCoorL(w,h,bw,padb,x,y,left.xp,uleft.xp, top.xp, uright.xp, right.xp);
	EZ_SetupUpBdGC(widget, &top.gc, &uleft.gc, &left.gc, &uright.gc, &bottom.gc, &right.gc,ax, ay, az);
	EZ_RenderBorder(drawable,EZ_BORDER_POLYGON, &left, &uleft, &top, &uright, &right, 0);  
	/* fix the corner */
	if(EZ_WidgetParentBG(widget) && *(EZ_WidgetParentBG(widget)))
	  {
	    XSetForeground(EZ_Display, EZ_WRITABLEGC, *(EZ_WidgetParentBG(widget)));
	    gc = EZ_WRITABLEGC;	  
	  }
	else if( (bitmap = EZ_WidgetParentBGPixmap(widget)))
	  {
	    int ox,oy;
	    XSetTile(EZ_Display,EZ_NTILEGC2,bitmap->pixmap);
	    EZ_GetWidgetTileOrigin(EZ_WidgetParent(widget), &ox, &oy);
	    ox += aax - EZ_WidgetOriginX(widget);
	    oy += aay - EZ_WidgetOriginY(widget);
	    XSetTSOrigin(EZ_Display, EZ_NTILEGC2, ox, oy);
	    gc = EZ_NTILEGC2;
	  }
	else gc = EZ_NORMALGC1;

	(uleft.xp)[0].x = x;  (uleft.xp)[0].y = x;
	XFillPolygon(EZ_Display, drawable, gc,  uleft.xp, 3, Convex, CoordModeOrigin); 
	(uright.xp)[0].x = x;  (uright.xp)[0].y = y+h;
	XFillPolygon(EZ_Display, drawable, gc,  uright.xp, 3, Convex, CoordModeOrigin); 
      }
      break;
    case EZ_BORDER_NB_DOWN_L:
      {
	EZ_Bitmap *bitmap;
	padb = EZ_WidgetPadB(widget);
	EZ_FillNBBorderCoorL(w,h,bw,padb,x,y,left.xp,uleft.xp, top.xp, uright.xp, right.xp);
	EZ_SetupDownBdGC(widget, &top.gc, &uleft.gc, &left.gc, &uright.gc, &bottom.gc, &right.gc,ax, ay, az);
	EZ_RenderBorder(drawable,EZ_BORDER_POLYGON, &left, &uleft, &top, &uright, &right, 0);  
	/* fix the corner */
	if(EZ_WidgetParentBG(widget) && *(EZ_WidgetParentBG(widget)))
	  {
	    XSetForeground(EZ_Display, EZ_WRITABLEGC, *(EZ_WidgetParentBG(widget)));
	    gc = EZ_WRITABLEGC;	  
	  }
	else if( (bitmap = EZ_WidgetParentBGPixmap(widget)))
	  {
	    int ox,oy;
	    XSetTile(EZ_Display,EZ_NTILEGC2,bitmap->pixmap);
	    EZ_GetWidgetTileOrigin(EZ_WidgetParent(widget), &ox, &oy);
	    ox += aax - EZ_WidgetOriginX(widget);
	    oy += aay - EZ_WidgetOriginY(widget);
	    XSetTSOrigin(EZ_Display, EZ_NTILEGC2, ox, oy);
	    gc = EZ_NTILEGC2;
	  }
	else gc = EZ_NORMALGC1;
	(uleft.xp)[0].x = x;  (uleft.xp)[0].y = x;
	XFillPolygon(EZ_Display, drawable, gc,  uleft.xp, 3, Convex, CoordModeOrigin); 
	(uright.xp)[0].x = x;  (uright.xp)[0].y = y+h;
	XFillPolygon(EZ_Display, drawable, gc,  uright.xp, 3, Convex, CoordModeOrigin); 
      }
      break;
    case EZ_BORDER_NB_UP_R:
      {
	EZ_Bitmap *bitmap;
	padb = EZ_WidgetPadB(widget);
	EZ_FillNBBorderCoorR(w,h,bw,padb,x,y,left.xp,uleft.xp, top.xp, uright.xp, right.xp);
	EZ_SetupUpBdGC(widget, &uleft.gc, &bottom.gc, &left.gc, &uright.gc, &top.gc, &right.gc,ax, ay, az);
	uright.gc = top.gc;
	EZ_RenderBorder(drawable,EZ_BORDER_POLYGON, &left, &uleft, &top, &uright, &right, 0);  
	/* fix the corner */
	if(EZ_WidgetParentBG(widget) && *(EZ_WidgetParentBG(widget)))
	  {
	    XSetForeground(EZ_Display, EZ_WRITABLEGC, *(EZ_WidgetParentBG(widget)));
	    gc = EZ_WRITABLEGC;	  
	  }
	else if( (bitmap = EZ_WidgetParentBGPixmap(widget)))
	  {
	    int ox,oy;
	    XSetTile(EZ_Display,EZ_NTILEGC2,bitmap->pixmap);
	    EZ_GetWidgetTileOrigin(EZ_WidgetParent(widget), &ox, &oy);
	    ox += aax - EZ_WidgetOriginX(widget);
	    oy += aay - EZ_WidgetOriginY(widget);
	    XSetTSOrigin(EZ_Display, EZ_NTILEGC2, ox, oy);
	    gc = EZ_NTILEGC2;
	  }
	else gc = EZ_NORMALGC1;
	(uleft.xp)[0].x = x+w;  (uleft.xp)[0].y = y;
	XFillPolygon(EZ_Display, drawable, gc,  uleft.xp, 3, Convex, CoordModeOrigin); 
	(uright.xp)[0].x = x+w;  (uright.xp)[0].y = y +h;
	XFillPolygon(EZ_Display, drawable, gc,  uright.xp, 3, Convex, CoordModeOrigin); 
      }
      break;
    case EZ_BORDER_NB_DOWN_R:
      {
	EZ_Bitmap *bitmap;
	padb = EZ_WidgetPadB(widget);
	EZ_FillNBBorderCoorR(w,h,bw,padb,x,y,left.xp,uleft.xp, top.xp, uright.xp, right.xp);
	EZ_SetupDownBdGC(widget, &uleft.gc, &bottom.gc, &left.gc, &uright.gc, &top.gc, &right.gc,ax, ay, az);
	uright.gc = top.gc;
	EZ_RenderBorder(drawable,EZ_BORDER_POLYGON, &left, &uleft, &top, &uright, &right, 0);  
	/* fix the corner */
	if(EZ_WidgetParentBG(widget) && *(EZ_WidgetParentBG(widget)))
	  {
	    XSetForeground(EZ_Display, EZ_WRITABLEGC, *(EZ_WidgetParentBG(widget)));
	    gc = EZ_WRITABLEGC;	  
	  }
	else if( (bitmap = EZ_WidgetParentBGPixmap(widget)))
	  {
	    int ox,oy;
	    XSetTile(EZ_Display,EZ_NTILEGC2,bitmap->pixmap);
	    EZ_GetWidgetTileOrigin(EZ_WidgetParent(widget), &ox, &oy);
	    ox += aax - EZ_WidgetOriginX(widget);
	    oy += aay - EZ_WidgetOriginY(widget);
	    XSetTSOrigin(EZ_Display, EZ_NTILEGC2, ox, oy);
	    gc = EZ_NTILEGC2;
	  }
	else gc = EZ_NORMALGC1;
	(uleft.xp)[0].x = x+w;  (uleft.xp)[0].y = y;
	XFillPolygon(EZ_Display, drawable, gc,  uleft.xp, 3, Convex, CoordModeOrigin); 
	(uright.xp)[0].x = x+w;  (uright.xp)[0].y = y +h;
	XFillPolygon(EZ_Display, drawable, gc,  uright.xp, 3, Convex, CoordModeOrigin); 
      }
      break;
    case EZ_BORDER_NB_UP:
      {
	EZ_Bitmap *bitmap;
	padb = EZ_WidgetPadB(widget);
	EZ_FillNBBorderCoor(w,h,bw,padb,x,y,left.xp,uleft.xp, top.xp, uright.xp, right.xp);
	EZ_SetupUpBdGC(widget, &left.gc, &uleft.gc, &top.gc, &uright.gc, &right.gc, &bottom.gc,ax, ay, az);
	EZ_RenderBorder(drawable,EZ_BORDER_POLYGON, &left, &uleft, &top, &uright, &right, 0);  
	/* fix the corner */
	if(EZ_WidgetParentBG(widget) && *(EZ_WidgetParentBG(widget)))
	  {
	    XSetForeground(EZ_Display, EZ_WRITABLEGC, *(EZ_WidgetParentBG(widget)));
	    gc = EZ_WRITABLEGC;	  
	  }
	else if( (bitmap = EZ_WidgetParentBGPixmap(widget)))
	  {
	    int ox,oy;
	    XSetTile(EZ_Display,EZ_NTILEGC2,bitmap->pixmap);
	    EZ_GetWidgetTileOrigin(EZ_WidgetParent(widget), &ox, &oy);
	    ox += aax - EZ_WidgetOriginX(widget);
	    oy += aay - EZ_WidgetOriginY(widget);
	    XSetTSOrigin(EZ_Display, EZ_NTILEGC2, ox, oy);
	    gc = EZ_NTILEGC2;
	  }
	else gc = EZ_NORMALGC1;
	(uleft.xp)[0].x = x;  (uleft.xp)[0].y = y;
	XFillPolygon(EZ_Display, drawable, gc,  uleft.xp, 3, Convex, CoordModeOrigin); 
	(uright.xp)[0].x = x+w;  (uright.xp)[0].y = y;
	XFillPolygon(EZ_Display, drawable, gc,  uright.xp, 3, Convex, CoordModeOrigin); 
      }
      break;
    case EZ_BORDER_NB_DOWN:
      {
	EZ_Bitmap *bitmap;
	padb = EZ_WidgetPadB(widget);
	EZ_FillNBBorderCoor(w,h,bw,padb,x,y,left.xp, uleft.xp, top.xp, uright.xp, right.xp);
	EZ_SetupDownBdGC(widget, &left.gc, &uleft.gc, &top.gc,&uright.gc, &right.gc, &bottom.gc,ax, ay, az);
	EZ_RenderBorder(drawable,EZ_BORDER_POLYGON, &left, &uleft, &top, &uright, &right, 0);  
	/* fix the corner */
	if(EZ_WidgetParentBG(widget) && *(EZ_WidgetParentBG(widget)))
	  {
	    XSetForeground(EZ_Display, EZ_WRITABLEGC, *(EZ_WidgetParentBG(widget)));
	    gc = EZ_WRITABLEGC;	  
	  }
	else if( (bitmap = EZ_WidgetParentBGPixmap(widget)))
	  {
	    int ox,oy;
	    XSetTile(EZ_Display,EZ_NTILEGC2,bitmap->pixmap);
	    EZ_GetWidgetTileOrigin(EZ_WidgetParent(widget), &ox, &oy);
	    ox += aax - EZ_WidgetOriginX(widget);
	    oy += aay - EZ_WidgetOriginY(widget);
	    XSetTSOrigin(EZ_Display, EZ_NTILEGC2, ox, oy);
	    gc = EZ_NTILEGC2;
	  }
	else  gc = EZ_NORMALGC1;
	(uleft.xp)[0].x = x;  (uleft.xp)[0].y = y;
	XFillPolygon(EZ_Display, drawable, gc,  uleft.xp, 3, Convex, CoordModeOrigin); 
	(uright.xp)[0].x = x+w;  (uright.xp)[0].y = y;
	XFillPolygon(EZ_Display, drawable, gc,  uright.xp, 3, Convex, CoordModeOrigin);     
      }
      break;
    case EZ_BORDER_NB_UP_B:
      {
	EZ_Bitmap *bitmap;
	padb = EZ_WidgetPadB(widget);
	EZ_FillNBBorderCoorB(w,h,bw,padb,x,y,left.xp,uleft.xp, top.xp, uright.xp, right.xp);
	EZ_SetupUpBdGC(widget, &uleft.gc, &bottom.gc, &left.gc, &uright.gc, &right.gc, &top.gc,ax, ay, az);
	uright.gc = right.gc;
	EZ_RenderBorder(drawable,EZ_BORDER_POLYGON, &left, &uleft, &top, &uright, &right, 0);  
	/* fix the corner */
	if(EZ_WidgetParentBG(widget) && *(EZ_WidgetParentBG(widget)))
	  {
	    XSetForeground(EZ_Display, EZ_WRITABLEGC, *(EZ_WidgetParentBG(widget)));
	    gc = EZ_WRITABLEGC;	  
	  }
	else if( (bitmap = EZ_WidgetParentBGPixmap(widget)))
	  {
	    int ox,oy;
	    XSetTile(EZ_Display,EZ_NTILEGC2,bitmap->pixmap);
	    EZ_GetWidgetTileOrigin(EZ_WidgetParent(widget), &ox, &oy);
	    ox += aax - EZ_WidgetOriginX(widget);
	    oy += aay - EZ_WidgetOriginY(widget);
	    XSetTSOrigin(EZ_Display, EZ_NTILEGC2, ox, oy);
	    gc = EZ_NTILEGC2;
	  }
	else gc = EZ_NORMALGC1;
	(uleft.xp)[0].x = x;  (uleft.xp)[0].y = h+y;
	XFillPolygon(EZ_Display, drawable, gc,  uleft.xp, 3, Convex, CoordModeOrigin); 
	(uright.xp)[0].x = x+w;  (uright.xp)[0].y = h+y;
	XFillPolygon(EZ_Display, drawable, gc,  uright.xp, 3, Convex, CoordModeOrigin); 
      }
      break;

    case EZ_BORDER_NB_DOWN_B:
      {
	EZ_Bitmap *bitmap;
	padb = EZ_WidgetPadB(widget);
	EZ_FillNBBorderCoorB(w,h,bw,padb,x,y,left.xp, uleft.xp, top.xp, uright.xp, right.xp);
	EZ_SetupDownBdGC(widget, &left.gc, &uleft.gc, &bottom.gc ,&uright.gc, &right.gc, &top.gc,ax, ay, az);
	uright.gc = right.gc;
	EZ_RenderBorder(drawable,EZ_BORDER_POLYGON, &left, &uleft, &top, &uright, &right, 0);  
	/* fix the corner */
	if(EZ_WidgetParentBG(widget) && *(EZ_WidgetParentBG(widget)))
	  {
	    XSetForeground(EZ_Display, EZ_WRITABLEGC, *(EZ_WidgetParentBG(widget)));
	    gc = EZ_WRITABLEGC;	  
	  }
	else if( (bitmap = EZ_WidgetParentBGPixmap(widget)))
	  {
	    int ox,oy;
	    XSetTile(EZ_Display,EZ_NTILEGC2,bitmap->pixmap);
	    EZ_GetWidgetTileOrigin(EZ_WidgetParent(widget), &ox, &oy);
	    ox += aax - EZ_WidgetOriginX(widget);
	    oy += aay - EZ_WidgetOriginY(widget);
	    XSetTSOrigin(EZ_Display, EZ_NTILEGC2, ox, oy);
	    gc = EZ_NTILEGC2;
	  }
	else  gc = EZ_NORMALGC1;
	(uleft.xp)[0].x = x;  (uleft.xp)[0].y = h+y;
	XFillPolygon(EZ_Display, drawable, gc,  uleft.xp, 3, Convex, CoordModeOrigin); 
	(uright.xp)[0].x = x+w;  (uright.xp)[0].y = h+y;
	XFillPolygon(EZ_Display, drawable, gc,  uright.xp, 3, Convex, CoordModeOrigin); 
      }
      break;

    case EZ_BORDER_FRAMED:
    case EZ_BORDER_CARVED:
      {
	GC gc;
	unsigned long bgpv;
	if(bw > 0)
	  {
	    int tile = EZ_GetBackgroundGC(widget, &gc, &bgpv, 0, 0);
	    XRectangle rects[4];
	    if(tile)
	      {
		int ox, oy; 
		EZ_GetWidgetTileOrigin(widget, &ox, &oy);
		ox += aax; oy += aay;
		XSetTSOrigin(EZ_Display, gc, ox, oy);
	      }	
	    rects[0].x = x;        rects[0].y = y; 
	    rects[0].width = w;    rects[0].height = bw;
	    rects[1].x = x;        rects[1].y = y; 
	    rects[1].width = bw;   rects[1].height = h;
	    rects[2].x = x+w-bw;   rects[2].y = y; 
	    rects[2].width = bw;   rects[2].height = h;
	    rects[3].x = x;        rects[3].y = h-bw+y; 
	    rects[3].width = w;    rects[3].height = bw;
	    XFillRectangles(EZ_Display, drawable, gc, rects, 4);
	    /*
	    XFillRectangle(EZ_Display, drawable, gc, x, y, (unsigned int)w, (unsigned int)bw);
	    XFillRectangle(EZ_Display, drawable, gc, x, y, (unsigned int)bw, (unsigned int)h);
	    XFillRectangle(EZ_Display, drawable, gc, x+w-bw,y,(unsigned int)bw, (unsigned int)h);
	    XFillRectangle(EZ_Display, drawable, gc, x,h-bw+y,(unsigned int)w, (unsigned int)bw);
	    */
	  }
      }
      {
	if(bw >= 3)
	  {
	    int rbw = (bw+2)/3;
	    int mbw = bw - rbw;
	    int sadjust= bw2 - (rbw + rbw);
	    if(bdtype == EZ_BORDER_FRAMED)
	      EZ_SetupUpBdGC(widget, &left.gc, &uleft.gc, &top.gc,&uright.gc, &right.gc, &bottom.gc,ax, ay, az);
	    else
	      EZ_SetupDownBdGC(widget, &left.gc, &uleft.gc, &top.gc,&uright.gc, &right.gc, &bottom.gc,ax, ay, az);
	    EZ_FillRectBorderCoor(w,h,rbw,x,y, top.xp,bottom.xp,left.xp,right.xp);
	    EZ_RenderBorder(drawable,EZ_BORDER_POLYGON, &top, &bottom, &left, &right,0, 0);     
	    if(bdtype == EZ_BORDER_FRAMED)
	      EZ_SetupDownBdGC(widget, &left.gc, &uleft.gc, &top.gc,&uright.gc, &right.gc, &bottom.gc,ax, ay, az);
	    else
	      EZ_SetupUpBdGC(widget, &left.gc, &uleft.gc, &top.gc,&uright.gc, &right.gc, &bottom.gc,ax, ay, az);
	    EZ_FillRectBorderCoor(w-sadjust,h-sadjust,rbw,x+mbw, y+mbw, top.xp,bottom.xp,left.xp,right.xp);
	    EZ_RenderBorder(drawable,EZ_BORDER_POLYGON, &top, &bottom, &left, &right,0, 0);
	  }
	else
	  {
	    if(bdtype == EZ_BORDER_FRAMED)	    
	      EZ_SetupUpBdGC(widget, &left.gc, &uleft.gc, &top.gc,&uright.gc, &right.gc, &bottom.gc,ax, ay, az);
	    else
	      EZ_SetupDownBdGC(widget, &left.gc, &uleft.gc, &top.gc,&uright.gc, &right.gc, &bottom.gc,ax, ay, az);
	    EZ_FillRectBorderCoor(w-1,h-1,0,x,y,top.xp,bottom.xp,left.xp,right.xp);
	    EZ_RenderBorder(drawable,EZ_BORDER_LINE, &top, &bottom, &left, &right,0, 0);
	    if(bdtype == EZ_BORDER_FRAMED)	    	    
	      EZ_SetupDownBdGC(widget, &left.gc, &uleft.gc, &top.gc,&uright.gc, &right.gc, &bottom.gc,ax, ay, az);
	    else
	      EZ_SetupUpBdGC(widget, &left.gc, &uleft.gc, &top.gc,&uright.gc, &right.gc, &bottom.gc,ax, ay, az);
	    EZ_FillRectBorderCoor(w-bw2-1,h-bw2-1,0,x+bw, y+bw, top.xp,bottom.xp,left.xp,right.xp);
	    EZ_RenderBorder(drawable,EZ_BORDER_LINE, &top, &bottom, &left, &right,0, 0);
	  }
      }
      break;
    case EZ_BORDER_SHADOW:
    case EZ_BORDER_SHADOW3:
      {
        int d3 = (bdtype == EZ_BORDER_SHADOW3);
	EZ_Bitmap *bitmap;
        EZ_SetupDownBdGC(widget, &left.gc, &uleft.gc, &top.gc,&uright.gc, &right.gc, &bottom.gc,ax, ay, az);
        EZ_FillShadowBorderCoor(w,h,bw,x,y, top.xp,bottom.xp,left.xp,right.xp, d3);
        bottom.gc = right.gc = left.gc; left.gc = top.gc;
        EZ_RenderBorder(drawable,EZ_BORDER_SHADOW_LP, &top, &bottom, &left, &right,0, 0);     
        /* fix the two corners */
        if( (az & 2 ) == 0 )
          {
            if(EZ_WidgetParentBG(widget) && *(EZ_WidgetParentBG(widget)))
              {
                XSetForeground(EZ_Display, EZ_WRITABLEGC, *(EZ_WidgetParentBG(widget)));
                gc = EZ_WRITABLEGC;	  
              }
            else if( (bitmap = EZ_WidgetParentBGPixmap(widget)))
              {
                int ox,oy;
                XSetTile(EZ_Display,EZ_NTILEGC2,bitmap->pixmap);
                EZ_GetWidgetTileOrigin(EZ_WidgetParent(widget), &ox, &oy);
                ox += aax - EZ_WidgetOriginX(widget);
                oy += aay - EZ_WidgetOriginY(widget);
                XSetTSOrigin(EZ_Display, EZ_NTILEGC2, ox, oy);
                gc = EZ_NTILEGC2;
              }
            else  gc = EZ_NORMALGC1;
            if(d3)
              {
                right.xp[1].x=x+w;    right.xp[1].y=y+bw;
                right.xp[2].x=x+w;    right.xp[2].y=y;
                XFillPolygon(EZ_Display, drawable, gc,  right.xp, 3, Convex, CoordModeOrigin); 
                bottom.xp[1].x=x+bw;   bottom.xp[1].y=y+h;
                bottom.xp[2].x=x;      bottom.xp[1].y=y+h;
                XFillPolygon(EZ_Display, drawable, gc,  bottom.xp, 3, Convex, CoordModeOrigin); 
              }
            else
              {
                right.xp[0].x=x+w-bw+1;
                right.xp[1].x=x+w-bw+1; right.xp[1].y=y;
                right.xp[2].x=x+w;    right.xp[2].y=y;
                XFillPolygon(EZ_Display, drawable, gc,  right.xp, 4, Convex, CoordModeOrigin); 
                bottom.xp[1].x=x;    bottom.xp[1].y=y+h-bw+1;
                bottom.xp[2].x=x;    bottom.xp[2].y=y+h;
                bottom.xp[0].y =y+h-bw+1;
                XFillPolygon(EZ_Display, drawable, gc,  bottom.xp, 4, Convex, CoordModeOrigin); 
              }
          }
      }
      break;       
    case EZ_HSLIDER_BORDER:
    case EZ_VSLIDER_BORDER:
      {
        EZ_SetupUpBdGC(widget, &left.gc, &uleft.gc, &top.gc,&uright.gc, &right.gc, &bottom.gc,ax, ay, az);
        EZ_FillSliderBorderCoor(w,h,bw,x,y, uleft.xp, left.xp, top.xp,right.xp, uright.xp, bdtype);
        if(bdtype == EZ_HSLIDER_BORDER)
          {
            uleft.gc = left.gc;
            uright.gc = right.gc;
            right.gc = bottom.gc;
          }
        else
          {
            uleft.gc = bottom.gc;
          }
        EZ_RenderBorder(drawable,EZ_BORDER_POLYGON, &top, &left, &right, &uleft, &uright,NULL);     
        /* fix the two corners */
        if((az & 2 ) == 0)
          {
            int wtype = EZ_WidgetType(widget);
            if(wtype == EZ_HORIZONTAL_SLIDER || wtype == EZ_VERTICAL_SLIDER)
              {
                unsigned long bgpv;
                int tile = EZ_GetBackgroundGC(widget, &gc, &bgpv,0, 0);
                if(tile)
                  {
                    int tox, toy;
                    EZ_GetWidgetTileOrigin(widget, &tox, &toy);      
                    XSetTSOrigin(EZ_Display, gc, tox, toy);
                  }
              }
            else
              {
                EZ_Bitmap *bitmap;
                if(EZ_WidgetParentBG(widget) && *(EZ_WidgetParentBG(widget)))
                  {
                    XSetForeground(EZ_Display, EZ_WRITABLEGC, *(EZ_WidgetParentBG(widget)));
                    gc = EZ_WRITABLEGC;	  
                  }
                else if( (bitmap = EZ_WidgetParentBGPixmap(widget)))
                  {
                    int ox,oy;
                    XSetTile(EZ_Display,EZ_NTILEGC2,bitmap->pixmap);
                    EZ_GetWidgetTileOrigin(EZ_WidgetParent(widget), &ox, &oy);
                    ox += aax - EZ_WidgetOriginX(widget);
                    oy += aay - EZ_WidgetOriginY(widget);
                    XSetTSOrigin(EZ_Display, EZ_NTILEGC2, ox, oy);
                    gc = EZ_NTILEGC2;
                  }
                else  gc = EZ_NORMALGC1;
              }
            if(bdtype == EZ_VSLIDER_BORDER)
              {
                uright.xp[2].x=x+w; uright.xp[2].y=y;
                XFillPolygon(EZ_Display, drawable, gc,  uright.xp, 3, Convex, CoordModeOrigin); 
                right.xp[2].x=x+w; right.xp[2].y=y+h;
                XFillPolygon(EZ_Display, drawable, gc,  right.xp, 3, Convex, CoordModeOrigin); 
              }
            else
              {
                uleft.xp[2].x=x; uleft.xp[2].y=y+h;
                XFillPolygon(EZ_Display, drawable, gc,  uleft.xp, 3, Convex, CoordModeOrigin); 
                uright.xp[2].x=x+w; uright.xp[2].y=y+h;
                XFillPolygon(EZ_Display, drawable, gc,  uright.xp, 3, Convex, CoordModeOrigin); 
              }
          }
      }
      break;       
    default:
      (void) fprintf(stderr, "Unknown border type!\n");
      break;
    }
}
/*********************************************************************************
 *
 *  Compute the coordinates of the edge polygons for rectangular shaped widgets.
 *  See figure above.
 *
 */
void EZ_FillRectBorderCoor(width,height,bwidth,xoffset,yoffset,top,bottom,left,right)
     int        width, height, bwidth,xoffset, yoffset;
     XPoint     *left, *right, *top, *bottom;
{
  short x,y,w,h,bw;
    
  x  = (short)xoffset;      /*   x coor of origin  */              
  y  = (short)yoffset;      /*   y coor of origin  */
  w  = (short)width;        /*   width and height, */
  h  = (short)height;       /*   including border  */
  bw = (short)bwidth;       /*   border width      */
  

  /* the top border piece     */
  top[0].x = x;          top[0].y = y;
  top[1].x = w+x;        top[1].y = y;  
  top[2].x = w-bw+x;     top[2].y = bw+y;  
  top[3].x = bw+x;       top[3].y = bw+y;  

  /* the bottom border piece  */  
  bottom[0].x = x;       bottom[0].y = h+y;
  bottom[1].x = w+x;     bottom[1].y = h+y;
  bottom[2].x = w-bw+x;  bottom[2].y = h-bw+y;
  bottom[3].x = bw+x;    bottom[3].y = h-bw+y;

  /* the left border piece   */
  left[0].x = x;         left[0].y = y;
  left[1].x = x;         left[1].y = h+y;  
  left[2].x = bw+x;      left[2].y = h-bw+y;
  left[3].x = bw+x;      left[3].y = bw+y;

  /* the right border piece  */
  right[0].x= w+x;       right[0].y = y;
  right[1].x= w+x;       right[1].y = h+y;
  right[2].x= w-bw+x;    right[2].y = h-bw+y;
  right[3].x= w-bw+x;    right[3].y = bw+y;
}

/*********************************************************************************
 *     ___
 * top   |   left  |__  (lines)
 * botttom and right ==  ||
 */
void EZ_FillShadowBorderCoor(width,height,bwidth,xoffset,yoffset,top,bottom,left,right, d3)
     int        width, height, bwidth,xoffset, yoffset;
     XPoint     *left, *right, *top, *bottom; int d3;
{
  short x,y,w,h,bw;
    
  x  = (short)xoffset;      /*   x coor of origin  */              
  y  = (short)yoffset;      /*   y coor of origin  */
  w  = (short)width;        /*   width and height, */
  h  = (short)height;       /*   including border  */
  bw = (short)bwidth;       /*   border width      */
  

  /* the top border piece     */
  top[0].x = x;          top[0].y = y;
  top[1].x = w-bw+x;     top[1].y = y;  
  top[2].x = w-bw+x;     top[2].y = h-bw+y;  

  /* the left border piece   */
  left[0].x = x;         left[0].y = y;
  left[1].x = x;         left[1].y = h+y-bw;  
  left[2].x = w-bw+x;    left[2].y = h-bw+y;

  /* the bottom border piece  */  
  if(d3) bottom[0].x = x;  else  bottom[0].x = x+bw; 
  bottom[0].y = h+y-bw;
  bottom[1].x = w+x;     bottom[1].y = h+y-bw;
  bottom[2].x = w+x;     bottom[2].y = h+y;
  bottom[3].x = bw+x;    bottom[3].y = h+y;

  /* the right border piece  */
  if(d3) right[0].y = y; else right[0].y = y+bw;
  right[0].x= w+x-bw;    
  right[1].x= w+x-bw;     right[1].y = h+y;
  right[2].x= w+x;        right[2].y = h+y;
  right[3].x= w+x;        right[3].y = bw+y;
}

void EZ_DrawDiamondBorderWithSize(widget, drawable,x,y,w,h,bw, btype, ax,ay,az)
     EZ_Widget  *widget;
     Drawable   drawable;
     int        x,y,w,h,bw, btype;
     int        ax,ay,az;  
{
  int                 bw2,bw22;
  EZ_BorderPiece     tr, bl, tl, br, jk1, jk2;

  switch(btype)
    {
    case  EZ_BORDER_NONE:          /* nothing need to be done    */
      break;

    case EZ_BORDER_FLAT:           /* draw a black flat border   */
      EZ_SetupDownBdGC(widget, &br.gc, &jk1.gc, &bl.gc, &jk2.gc, &tl.gc, &bl.gc,ax, ay, az);
      tr.gc = bl.gc = tl.gc = br.gc;
      EZ_FillDiamondBorderCoor(w,h,bw,x,y,tr.xp,bl.xp,tl.xp,br.xp);
      EZ_RenderBorder(drawable,EZ_BORDER_POLYGON, &tr, &bl, &tl, &br,0, 0);
      break;

    case   EZ_BORDER_FRAMED_UP:
      bw2 = (int) (1.4142135623730951* (float) bw);
      bw22 = bw2 <<1;
      EZ_SetupDownBdGC(widget, &br.gc, &jk1.gc, &bl.gc, &jk2.gc, &tl.gc, &bl.gc,ax, ay, az);
      EZ_FillDiamondBorderCoor(w,h,0,x,y,tr.xp,bl.xp,tl.xp,br.xp);
      EZ_RenderBorder(drawable,EZ_BORDER_LINE, &tr, &bl, &tl, &br,0, 0);
      EZ_SetupUpBdGC(widget, &br.gc, &jk1.gc, &bl.gc, &jk2.gc, &tl.gc, &tr.gc,ax, ay, az);
      EZ_FillDiamondBorderCoor(w-bw22,h-bw22,0,x+bw2, y, tr.xp,bl.xp,tl.xp,br.xp);
      EZ_RenderBorder(drawable,EZ_BORDER_LINE, &tr, &bl, &tl, &br,0, 0);
      break;

    case   EZ_BORDER_FRAMED_DOWN:
      bw2 = (int) (1.4142135623730951* (float) bw);
      bw22 = bw2 <<1;
      EZ_SetupUpBdGC(widget, &br.gc, &jk1.gc, &bl.gc, &jk2.gc, &tl.gc, &tr.gc,ax, ay, az);
      EZ_FillDiamondBorderCoor(w,h,0,x,y,tr.xp,bl.xp,tl.xp,br.xp);
      EZ_RenderBorder(drawable,EZ_BORDER_LINE, &tr, &bl, &tl, &br,0, 0);
      EZ_SetupDownBdGC(widget, &br.gc, &jk1.gc, &bl.gc, &jk2.gc, &tl.gc, &bl.gc,ax, ay, az);
      EZ_FillDiamondBorderCoor(w-bw22,h-bw22,0,x+bw2, y, tr.xp,bl.xp,tl.xp,br.xp);
      EZ_RenderBorder(drawable,EZ_BORDER_LINE, &tr, &bl, &tl, &br,0, 0);
      break;
      
    case  EZ_BORDER_UP:
      EZ_SetupDownBdGC(widget, &br.gc, &jk1.gc, &bl.gc, &jk2.gc, &tl.gc, &tr.gc,ax, ay, az);
      EZ_FillDiamondBorderCoor(w,h,bw,x,y, tr.xp,bl.xp,tl.xp,br.xp);
      EZ_RenderBorder(drawable,EZ_BORDER_POLYGON, &tr, &bl, &tl, &br,0, 0);     
      break; 

    case  EZ_BORDER_DOWN:
      EZ_SetupUpBdGC(widget, &br.gc, &jk1.gc, &bl.gc, &jk2.gc, &tl.gc, &tr.gc,ax, ay, az);
      EZ_FillDiamondBorderCoor(w,h,bw,x,y, tr.xp,bl.xp,tl.xp,br.xp);
      EZ_RenderBorder(drawable,EZ_BORDER_POLYGON, &tr, &bl, &tl, &br,0, 0);  
      break; 
    default:
      (void) fprintf(stderr, "Unknown border style!\n");
      break;
    }
}
/*********************************************************************************
 *
 *  Compute the coordinates of the edge polygons for diamond shaped widgets.
 *
 *
 *                  /\             
 *             tl  /  \ tr         
 *                /    \           
 *        (x,y)  <      >       
 *                \    /        
 *              bl \  / br      
 *                  \/          
 */
void EZ_FillDiamondBorderCoor(width,height,bwidth,xoffset,yoffset,tr,bl,tl,br)
     int        width, height, bwidth,xoffset, yoffset;
     XPoint     *tl, *tr, *bl, *br;
{
  short x,y,w,h,bw;
  int   tmp;

  x  = (short)xoffset;      /*   x coor of origin  */              
  y  = (short)yoffset;      /*   y coor of origin  */

  tmp = (width + 1) >> 1;
  if(tmp != 0)   w = (short) tmp;
  else           w = 1;  
  h = w;

  bw = (short) (1.4142135623730951 * (float) bwidth);
  
  /* the tl border piece     */
  tl[0].x = x;          tl[0].y = y;
  tl[1].x = w+x;        tl[1].y = y - h;  
  tl[2].x = w+x;        tl[2].y = bw+y-h;  
  tl[3].x = bw+x;       tl[3].y = y;  

  /* the tr border piece  */  
  tr[0].x = x+w;       tr[0].y = y-h;
  tr[1].x = x+w+w;     tr[1].y = y;
  tr[2].x = x+w+w-bw;  tr[2].y = y;
  tr[3].x = x+w;       tr[3].y = y-h+bw;

  /* the bl border piece   */
  bl[0].x = x;         bl[0].y = y;
  bl[1].x = x+w;       bl[1].y = h+y;  
  bl[2].x = x+w;       bl[2].y = y+h-bw;
  bl[3].x = x+bw;      bl[3].y = y;

  /* the br border piece  */
  br[0].x= w+x;       br[0].y = y+h;
  br[1].x= w+x+w;     br[1].y = y;
  br[2].x= w+x+w-bw;  br[2].y = y;
  br[3].x= w+x;       br[3].y = y+h-bw;
}

/****************************************************************************
 *
 *   Render a 3D border of a widget.
 */
void   EZ_RenderBorder(drawable,type, top, bottom, left, right, extra, extraB)
     Drawable        drawable;
     int             type;
     EZ_BorderPiece  *top, *bottom, *left, *right, *extra, *extraB;
{
  switch(type)
    {
    case EZ_BORDER_POLYGON:

      XFillPolygon(EZ_Display, drawable,
		   top->gc,
		   top->xp,
		   4,
		   Convex, CoordModeOrigin);  

      XFillPolygon(EZ_Display, drawable,
		   bottom->gc,
		   bottom->xp,
		   4,
		   Convex, CoordModeOrigin);  
      XFillPolygon(EZ_Display, drawable,
		   left->gc,
		   left->xp,
		   4,
		   Convex, CoordModeOrigin);  
      XFillPolygon(EZ_Display, drawable,
		   right->gc,
		   right->xp,
		   4,
		   Convex, CoordModeOrigin);  

      if(extra) XFillPolygon(EZ_Display, drawable,
			     extra->gc,
			     extra->xp,
			     4,
			     Convex, CoordModeOrigin);  	
      if(extraB) XFillPolygon(EZ_Display, drawable,
			      extraB->gc,
			      extraB->xp,
			      4,
			      Convex, CoordModeOrigin);  	

      break;

    case EZ_BORDER_LINE:
      XDrawLines(EZ_Display, drawable,
		 top->gc,
		 top->xp,
		 2,
		 CoordModeOrigin);
      XDrawLines(EZ_Display, drawable,
		 bottom->gc,
		 bottom->xp,
		 2,
		 CoordModeOrigin);
      XDrawLines(EZ_Display, drawable,
		 left->gc,
		 left->xp,
		 2,
		 CoordModeOrigin);
      XDrawLines(EZ_Display, drawable,
		 right->gc,
		 right->xp,
		 2,
		 CoordModeOrigin);

      if(extra)  XDrawLines(EZ_Display, drawable,
			    extra->gc,
			    extra->xp,
			    2,
			    CoordModeOrigin);
      if(extraB) XDrawLines(EZ_Display, drawable,
			    extraB->gc,
			    extraB->xp,
			    2,
			    CoordModeOrigin);

      break;
    case EZ_BORDER_SHADOW_LP:      
      XDrawLines(EZ_Display, drawable,
		 left->gc,
		 left->xp,
		 3,
		 CoordModeOrigin);      
      XDrawLines(EZ_Display, drawable,
		 top->gc,
		 top->xp,
		 3,
		 CoordModeOrigin);      
      XFillPolygon(EZ_Display, drawable,
		   bottom->gc,
		   bottom->xp,
		   4,
		   Convex, CoordModeOrigin);  
      XFillPolygon(EZ_Display, drawable,
		   right->gc,
		   right->xp,
		   4,
		   Convex, CoordModeOrigin);  
      break;
    default:
      break;
    }
}
/******************************************************************************/

void EZ_FillNBBorderCoor(width,height,bwidth,bpad,xoffset,yoffset,left,ul, top, ur, right)
     int        width, height, bwidth,xoffset, yoffset, bpad;
     XPoint     *left, *ul, *top, *ur, *right;
{
  short x,y,w,h,bw,bw2, bww, padb;
    
  x  = (short)xoffset;      /*   x coor of origin  */              
  y  = (short)yoffset;      /*   y coor of origin  */
  w  = (short)width;        /*   width and height, */
  h  = (short)height;       /*   including border  */
  bw = (short)bwidth;       /*   border width      */
  padb = (short) bpad;
  bww = (bw * 19) >> 3;
  bw2 = bw << 1;

  /* the left border piece   */
  left[0].x = x;         left[0].y = h+y + padb;
  left[1].x = bw+x;      left[1].y = h+y + padb;  
  left[2].x = bw+x;      left[2].y = bww+y;
  left[3].x = x;         left[3].y = bw2+y;

  /* the upper left piece    */
  ul[0].x = left[2].x;   ul[0].y = left[2].y;
  ul[1].x = left[3].x;   ul[1].y = left[3].y;
  ul[2].x = bw2 + x;     ul[2].y = y;
  ul[3].x = bww + x;     ul[3].y = bw + y;

  /* the top border piece     */
  top[0].x = ul[2].x;    top[0].y = ul[2].y;
  top[1].x = ul[3].x;    top[1].y = ul[3].y;  
  top[2].x = w-bww+x;    top[2].y = bw+y;  
  top[3].x = w-bw2+x;    top[3].y = y;  

  /* the upper right piece    */
  ur[0].x = top[2].x;    ur[0].y = top[2].y;
  ur[1].x = top[3].x;    ur[1].y = top[3].y;
  ur[2].x = w + x;       ur[2].y = bw2 + y;
  ur[3].x = w - bw + x;  ur[3].y = bww + y;


  /* the right border piece  */
  right[0].x= ur[3].x;   right[0].y = ur[3].y;
  right[1].x= ur[2].x;   right[1].y = ur[2].y;
  right[2].x= w+x;       right[2].y = h + y + padb;
  right[3].x= w-bw+x;    right[3].y = h + y + padb;
}
/******************************************************************************/

void EZ_FillNBBorderCoorB(width,height,bwidth,bpad,xoffset,yoffset,left,ul, top, ur, right)
     int        width, height, bwidth,xoffset, yoffset, bpad;
     XPoint     *left, *ul, *top, *ur, *right;
{
  short x,y,w,h,bw,bw2, bww, padb;
    
  x  = (short)xoffset;      /*   x coor of origin  */              
  y  = (short)yoffset;      /*   y coor of origin  */
  w  = (short)width;        /*   width and height, */
  h  = (short)height;       /*   including border  */
  bw = (short)bwidth;       /*   border width      */
  padb = (short) bpad;
  bww = (bw * 19) >> 3;
  bw2 = bw << 1;

  /* the left border piece   */
  left[0].x = x;         left[0].y = y - padb;
  left[1].x = bw+x;      left[1].y = y - padb;
  left[2].x = bw+x;      left[2].y = h - bww + y;
  left[3].x = x;         left[3].y = h - bw2 + y;

  /* the lower left piece    */
  ul[0].x = left[2].x;   ul[0].y = left[2].y;
  ul[1].x = left[3].x;   ul[1].y = left[3].y;
  ul[2].x = bw2 + x;     ul[2].y = h + y;
  ul[3].x = bww + x;     ul[3].y = h - bw + y;

  /* the bottom border piece     */
  top[0].x = ul[2].x;    top[0].y = ul[2].y;
  top[1].x = ul[3].x;    top[1].y = ul[3].y;  
  top[2].x = w-bww+x;    top[2].y = h - bw+y;  
  top[3].x = w-bw2+x;    top[3].y = h + y;  

  /* the upper right piece    */
  ur[0].x = top[2].x;    ur[0].y = top[2].y;
  ur[1].x = top[3].x;    ur[1].y = top[3].y;
  ur[2].x = w + x;       ur[2].y = h - bw2 + y;
  ur[3].x = w - bw + x;  ur[3].y = h - bww + y;


  /* the right border piece  */
  right[0].x= ur[3].x;   right[0].y = ur[3].y;
  right[1].x= ur[2].x;   right[1].y = ur[2].y;
  right[2].x= w+x;       right[2].y =  y - padb;
  right[3].x= w-bw+x;    right[3].y =  y - padb;
}
/******************************************************************************/

void EZ_FillNBBorderCoorL(width,height,bwidth,bpad,xoffset,yoffset,left,ul, top, ur, right)
     int        width, height, bwidth,xoffset, yoffset, bpad;
     XPoint     *left, *ul, *top, *ur, *right;
{
  short x,y,w,h,bw,bw2, bww, padb;
    
  x  = (short)xoffset;      /*   x coor of origin  */              
  y  = (short)yoffset;      /*   y coor of origin  */
  w  = (short)width;        /*   width and height, */
  h  = (short)height;       /*   including border  */
  bw = (short)bwidth;       /*   border width      */
  padb = (short) bpad;
  bww = (bw * 19) >> 3;
  bw2 = bw << 1;

  /* the left border piece   */
  left[0].x = x+w +padb; left[0].y = y;
  left[1].x = x+w +padb; left[1].y = y+bw;
  left[2].x = x+bww;     left[2].y = y+bw;
  left[3].x = x+bw2;     left[3].y = y;

  /* the lower left piece    */
  ul[0].x = left[2].x;   ul[0].y = left[2].y;
  ul[1].x = left[3].x;   ul[1].y = left[3].y;
  ul[2].x = x;           ul[2].y = y + bw2;
  ul[3].x = x+bw;        ul[3].y = y + bww;

  /* the bottom border piece     */
  top[0].x = ul[2].x;    top[0].y = ul[2].y;
  top[1].x = ul[3].x;    top[1].y = ul[3].y;  
  top[2].x = x + bw;     top[2].y = h - bww+y;  
  top[3].x = x;          top[3].y = h -bw2+ y;  

  /* the upper right piece    */
  ur[0].x = top[2].x;    ur[0].y = top[2].y;
  ur[1].x = top[3].x;    ur[1].y = top[3].y;
  ur[2].x = bw2 + x;     ur[2].y = h + y;
  ur[3].x = bww + x   ;  ur[3].y = h - bw + y;


  /* the right border piece  */
  right[0].x= ur[3].x;   right[0].y = ur[3].y;
  right[1].x= ur[2].x;   right[1].y = ur[2].y;
  right[2].x= w+x+padb;  right[2].y =  y+h;
  right[3].x= w+x+padb;  right[3].y =  y+h -bw;
}

/******************************************************************************/

void EZ_FillNBBorderCoorR(width,height,bwidth,bpad,xoffset,yoffset,left,ul, top, ur, right)
     int        width, height, bwidth,xoffset, yoffset, bpad;
     XPoint     *left, *ul, *top, *ur, *right;
{
  short x,y,w,h,bw,bw2, bww, padb;
    
  x  = (short)xoffset;      /*   x coor of origin  */              
  y  = (short)yoffset;      /*   y coor of origin  */
  w  = (short)width;        /*   width and height, */
  h  = (short)height;       /*   including border  */
  bw = (short)bwidth;       /*   border width      */
  padb = (short) bpad;
  bww = (bw * 19) >> 3;
  bw2 = bw << 1;

  /* the left border piece   */
  left[0].x = x-padb;    left[0].y = y;
  left[1].x = x-padb;    left[1].y = y+bw;
  left[2].x = x+ w -bww; left[2].y = y+bw;
  left[3].x = x+ w - bw2;left[3].y = y;

  /* the lower left piece    */
  ul[0].x = left[2].x;   ul[0].y = left[2].y;
  ul[1].x = left[3].x;   ul[1].y = left[3].y;
  ul[2].x = x + w;       ul[2].y = y + bw2;
  ul[3].x = x+w-bw;      ul[3].y = y + bww;

  /* the bottom border piece     */
  top[0].x = ul[2].x;    top[0].y = ul[2].y;
  top[1].x = ul[3].x;    top[1].y = ul[3].y;  
  top[2].x = x+ w - bw;  top[2].y = h - bww+y;  
  top[3].x = x+w;        top[3].y = h -bw2+ y;  

  /* the upper right piece    */
  ur[0].x = top[2].x;    ur[0].y = top[2].y;
  ur[1].x = top[3].x;    ur[1].y = top[3].y;
  ur[2].x = x + w -bw2;  ur[2].y = h + y;
  ur[3].x = x +w - bww;  ur[3].y = h - bw + y;

  /* the right border piece  */
  right[0].x= ur[3].x;   right[0].y = ur[3].y;
  right[1].x= ur[2].x;   right[1].y = ur[2].y;
  right[2].x= x-padb;    right[2].y = y+h;
  right[3].x= x-padb;    right[3].y = y+h -bw;
}
/******************************************************************************/
void EZ_FillSliderBorderCoor(width,height,bwidth,xoffset,yoffset,
                             uleft, left, top, right, uright,type)
     int width,height,xoffset,yoffset,bwidth,type;
     XPoint *uleft, *left, *top, *right, *uright;
{
  short x,y,th,h,hh,tw,w,ww,bw, bw3;
  double tmp;

  x  = (short)xoffset;      /*   x coor of origin  */              
  y  = (short)yoffset;      /*   y coor of origin  */
  tw  = (short)width;       /*   width and height, */
  th  = (short)height;      /*   including border  */
  bw = (short)bwidth;       /*   border width      */

  if(type == EZ_HSLIDER_BORDER)
    {
      hh  = (th >> 2);
      h = th - hh;
      w = tw;
      tmp = (double)bwidth * (double)(width - hh -hh)/(double)width;
      bw3 = (int)tmp;

      /* the top border piece     */
      top[0].x = x;          top[0].y = y;
      top[1].x = w+x;        top[1].y = y;  
      top[2].x = w-bw+x;     top[2].y = bw+y;  
      top[3].x = bw+x;       top[3].y = bw+y;  

      /* the left border piece   */
      left[0].x = x;         left[0].y = y;
      left[1].x = x;         left[1].y = h+y;  
      left[2].x = bw+x;      left[2].y = h+y;
      left[3].x = bw+x;      left[3].y = bw+y;

      /* the lower left corner */
      uleft[0].x = x;          uleft[0].y = h+y;
      uleft[1].x = x+(w>>1);   uleft[1].y = y+th;
      uleft[2].x = uleft[1].x; uleft[2].y = th-bw+y;
      uleft[3].x = bw+x;       uleft[3].y = uleft[0].y -(bw3);

      /* the right border piece  */
      right[0].x= w+x;       right[0].y = y;
      right[1].x= w+x;       right[1].y = h+y;
      right[2].x= w-bw+x;    right[2].y = h+y;
      right[3].x= w-bw+x;    right[3].y = bw+y;

      /* the lower right corner */
      uright[0].x = w+x;         uright[0].y = h+y;
      uright[1].x = x+(w>>1);    uright[1].y = y+th;
      uright[2].x = uright[1].x; uright[2].y = th-bw+y;
      uright[3].x = w+x-bw;      uright[3].y = uright[0].y -(bw3);
    }
  else
    {
      ww  = (tw >> 2);
      w = tw - ww;
      h = th;
      tmp = (double)bwidth * (double)(height - ww -ww)/(double)height;
      bw3 = (int)tmp;

      /* the top border piece     */
      top[0].x = x;          top[0].y = y;
      top[1].x = w+x;        top[1].y = y;  
      top[2].x = w+x;        top[2].y = bw+y;  
      top[3].x = bw+x;       top[3].y = bw+y;  

      /* the left border piece   */
      left[0].x = x;         left[0].y = y;
      left[1].x = x;         left[1].y = h+y;  
      left[2].x = bw+x;      left[2].y = h-bw+y;
      left[3].x = bw+x;      left[3].y = bw+y;

      /* the upper right corner */
      uright[0].x = w+x;         uright[0].y = y;
      uright[1].x = x+tw;        uright[1].y = y+(th>>1);
      uright[2].x = x+tw-bw;     uright[2].y = uright[1].y;
      uright[3].x = w+x-(bw3); uright[3].y = y+bw;
      
      /* the lower right border piece  */
      right[0].x= w+x;       right[0].y = y+th;
      right[1].x= tw+x;      right[1].y = y +(th>>1);
      right[2].x= tw-bw+x;   right[2].y = y +(th>>1);
      right[3].x= w+x-(bw3); right[3].y = th+y-bw;

      /* the bottom border piece  */  
      uleft[0].x = x;       uleft[0].y = h+y;
      uleft[1].x = w+x;     uleft[1].y = h+y;
      uleft[2].x = w+x-(bw3);  uleft[2].y = h+y-bw;
      uleft[3].x = bw+x;    uleft[3].y = h-bw+y;
    }
}
/******************************************************************************/
#undef _EZ_WIDGET_BORDER_C_
