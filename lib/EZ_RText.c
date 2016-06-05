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
#define _EZ_RTEXT_C_
#include "EZ_Widget.h"
/*******************************************************************/
EZ_Bitmap *EZ_CreateRotateTextPixmap MY_ANSIARGS((char *str, int fId, int llen, int jt, int angle));
EZ_Bitmap *EZ_CreateRotateTextPixmapU MY_ANSIARGS((char *str, int fId, int llen, int jt, int ul, int angle));
/*******************************************************************/
EZ_Bitmap *EZ_CreateRotateTextPixmap(str, fontId, lineLength, justification, angle)
     char *str;
     int fontId, lineLength, justification;
     int angle;
{
  return(EZ_CreateRotateTextPixmapU(str, fontId, lineLength, justification, -1, angle));
}

EZ_Bitmap *EZ_CreateRotateTextPixmapU(str, fontId, lineLength, justification, ul, angle)
     char *str;
     int fontId, lineLength, justification;
     int angle, ul;
{
  XImage *image;
  Pixmap pixmap;
  EZ_Bitmap *bitmap;
  GC gc;
  XFontStruct  *font;
  int   tw,th, twidth, theight, nlines, fheight, fascent;
  int   slen;
  int   pwidth, pheight;
  double rad, sn, cs, dxoff, dyoff;

  if(str == NULL || *str == '\0') return(NULL);
  
  
  rad = -0.0174444 * (double)angle;
  sn = sin(rad);
  cs = cos(rad);
  
  font = EZ_GetFontFromId(fontId);
  slen = strlen(str);
  lineLength = lineLength == 0 ? EZ_DEFAULT_LABEL_LINE_LENGTH: lineLength;
  EZ_FigureLabelExtent(font, str, slen, lineLength, &nlines, &twidth);
  fascent = font->ascent;
  fheight = font->ascent + font->descent;
  theight = nlines * fheight + 1;

  tw = twidth;
  th = theight;
  pixmap = XCreatePixmap(EZ_Display,  RootWindow(EZ_Display, EZ_ScreenNum),
			 tw, th, EZ_Depth); 
  if(pixmap == None) return(NULL);

  gc = EZ_WRITABLEGC;
  XSetForeground(EZ_Display, gc, 0);
  XFillRectangle(EZ_Display, pixmap, gc, 0, 0, tw, th); 
  XSetFont(EZ_Display, gc, font->fid);
  XSetForeground(EZ_Display, gc, 1);
  EZ_RenderLabelText(pixmap, gc, 0,  fascent, fheight, str, slen, lineLength,
		     nlines, twidth, justification, font, 0);
  if(ul >= 0)
    {
      int tlen = XTextWidth(font,str,ul);
      int tlen1 = tlen + XTextWidth(font, str+ul, 1);
      int base =  fheight - 1;
      char *fname = EZ_GetFontNameFromId(fontId);
      int boldfont = EZ_FontWeightIsBold(fname);

      XDrawLine(EZ_Display, pixmap, gc, tlen, base, tlen1, base);
      if(boldfont)
	XDrawLine(EZ_DisplayForWidgets, pixmap, gc,
		  tlen, base+1, tlen1, base+1);
    }
  image = XGetImage(EZ_Display, pixmap, 0,0, tw, th, 1, XYPixmap);
  XFreePixmap(EZ_Display, pixmap);

  /* figure out the dimension of the smallest rectangle that enclose the rotated text */
  {
    double x1,y1,x2,y2,x3,y3, nw, nh;
    double xn,xx,yn,yx, tmp1, tmp2;
    
    x1 = (double)tw * cs;
    y1 = (double)tw * sn;
    x3 = -(double) th * sn;
    y3 =  (double) th * cs;    
    x2 =  x1 + x3;
    y2 =  y1 + y3;
    
    if(x2 > 0.0) { xx = x2; xn = 0.0;}
    else {xx = 0.0; xn = x2;}
    if(x1 > x3) { tmp1 = x1; tmp2 = x3;}
    else { tmp1 = x3; tmp2 = x1;}
    xx = xx > tmp1? xx : tmp1;
    xn  = xn < tmp2? xn : tmp2;

    if(y2 > 0.0) { yx = y2; yn = 0.0;}
    else {yx = 0.0; yn = y2;}
    if(y1 > y3) { tmp1 = y1; tmp2 = y3;}
    else { tmp1 = y3; tmp2 = y1;}
    yx = yx > tmp1? yx : tmp1;
    yn  = yn < tmp2? yn : tmp2;

    nw = (xx - xn + 0.5);
    nh = (yx - yn + 0.5);

    dxoff = xn;
    dyoff = yn;
    
    pwidth = (int) nw;
    pheight = (int) nh;
  }
  /* rotate */
  {
    int xc, yc, advance, itmp, jtmp, ktmp;
    unsigned int ch, mch;
    unsigned char *ptr,*data, *mdata, *mptr;
    unsigned long pixel;
    double xx, yy;
    int   xi, yi;
    
    itmp = ((pwidth + 7)/8);
    jtmp = itmp * pheight;
    advance = itmp;
    data = (unsigned char *)malloc( itmp * pheight * sizeof(char));
    if(data == NULL) {XDestroyImage(image); return(NULL);}
    mdata = (unsigned char *)malloc( itmp * pheight * sizeof(char));
    if(mdata == NULL)
      { free(data); XDestroyImage(image); return(NULL);}
    memset(data, 0,  itmp * pheight);
    memset(mdata, 0,  itmp * pheight);

    sn = -sn;
    for(ptr = data, mptr = mdata, yc = 0; yc < pheight; yc++,
	  ptr += advance, mptr += advance)
      {
	double ysn = -((double)yc + dyoff) * sn + dxoff * cs;
	double ycs =  ((double)yc + dyoff) * cs + dxoff * sn;
	for(ch = 0, mch = 0, ktmp = 0, itmp = 0, xc = 0; xc < pwidth; xc++)
	  {
	    xx = ((double) xc) * cs  + ysn;
	    yy = ((double) xc) * sn + ycs;
	    xi = (int)xx;
	    yi = (int)yy;
	    if(xi >=0 && xi < tw && yi >= 0 && yi < th)
	      {
		int mask = 1 << ktmp;
		pixel = XGetPixel(image,xi,yi);
		if(pixel != 0) ch |= mask;
		mch |= mask;
	      }
	    ktmp++;
	    if(ktmp == 8)
	      {
		ptr[itmp] = (unsigned char)ch;
		mptr[itmp] = (unsigned char)mch;
		itmp++;
		ktmp = 0;
		ch = 0;
		mch = 0;
	      }
	  }
	if(ktmp != 0)
	  {
	    ptr[itmp] = (unsigned char)ch;
	    mptr[itmp] = (unsigned char)mch;
	  }
      }
    bitmap = EZ_CreateLabelPixmapFromXBitmapData(data, pwidth, pheight);
    pixmap = XCreateBitmapFromData(EZ_Display,
				   RootWindow(EZ_Display, EZ_ScreenNum),
				   mdata,
				   pwidth, pheight);
    bitmap->shape = pixmap;
    XDestroyImage(image);
    (void) free((char *)data);
    (void) free((char *)mdata);
    
    return(bitmap);
  }
}
/***************************************************************************/
#undef _EZ_RTEXT_C_
