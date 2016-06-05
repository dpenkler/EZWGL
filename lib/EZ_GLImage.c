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
/***************************************************************************
 *
 *        GL Image
 *
 ***************************************************************************/
#define _EZ_GLIMAGE_C_

#include "EZ_Widget.h"
#include "EZ_Render.h"
/**************************************************************************
 *
 *  Functions implemented in this file.
 */
EZ_GLImage *EZ_CreateGLImage MY_ANSIARGS((EZ_Bitmap *pixmap));
void        EZ_GetGLImageWindow MY_ANSIARGS((int x, int y, int w, int h, EZ_GLImage **imgRet));
void        EZ_GetGLImageXWindow MY_ANSIARGS((int x, int y, int w, int h, EZ_GLImage **imgRet));
void        EZ_FreeGLImage MY_ANSIARGS((EZ_GLImage *glimage));

void EZ_PutGLImage MY_ANSIARGS((EZ_GLImage *glimage, int Sx, int Sy, int Sw,
				int Sh, float fx, float fy, float fz));
void EZ_PutGLImageScreen MY_ANSIARGS((EZ_GLImage *glimage, int Sx, int Sy, int Sw,
				      int Sh, float fx, float fy, float fz));
void EZ_PutGLImageWindow MY_ANSIARGS((EZ_GLImage *glimage, int Sx, int Sy, int Sw,
				      int Sh, int ix, int iy));
void EZ_PutGLImageXWindow MY_ANSIARGS((EZ_GLImage *glimage, int Sx, int Sy, int Sw,
				      int Sh, int ix, int iy));

int EZ_GetGLImageInfo MY_ANSIARGS((EZ_GLImage *image,
				   Pixmap *p_ret, Pixmap *s_ret, int *w, int *h));
EZ_Bitmap *EZ_GetLabelPixmapFromGLImage MY_ANSIARGS((EZ_GLImage *image));
/**************************************************************************
 *
 * Internal functions 
 */

void EZ_PutGLImageToDrawableNoZ MY_ANSIARGS((EZ_GLImage *glimage,
					     int sx,int sy, int sw, int sh, 
					     int x, int y, int z));
void EZ_PutGLImageToXImageNoZ MY_ANSIARGS((EZ_GLImage *glimage,
					     int sx,int sy, int sw, int sh, 
					     int x, int y, int z));
void EZ_PutGLImageNoOp MY_ANSIARGS((EZ_GLImage *glimage,
					     int sx,int sy, int sw, int sh, 
					     int x, int y, int z));
/**************************************************************************/
void EZ_PutGLImage(glimage, Sx, Sy, Sw, Sh, fx, fy, fz)
     EZ_GLImage *glimage;
     int Sx, Sy, Sw, Sh;
     float fx,fy,fz;
{
  if(glimage == NULL) return;
  if(EZ_InCompilingMode != 0)
    {
      EZ_Token *t = AllocateNTokens(9);
      t->i = TOKEN_PUT_IMAGE;
      t[1].ptr = glimage;
      t[2].i = Sx;
      t[3].i = Sy;
      t[4].i = Sw;
      t[5].i = Sh;
      t[6].f = fx;
      t[7].f = fy;
      t[8].f = fz;
    }
  if(EZ_InExecutionMode != 0)
    {
      float hcoor[4], fptr[4];
      int ix, iy, iz;
      fptr[0] = fx;
      fptr[1] = fy;
      fptr[2] = fz;
      TRANSFORM_VERTEX(hcoor,fptr);
      TRANSFORM_2_SCREEN(hcoor, ix,iy,iz);
      EZ_CURRENT_IMAGE_FUNC(glimage, Sx, Sy, Sw,Sh,ix,iy,iz);
    }
}

void EZ_PutGLImageScreen(glimage, Sx, Sy, Sw, Sh, fx, fy, fz)
     EZ_GLImage *glimage;
     int Sx, Sy, Sw, Sh;
     float fx,fy,fz;
{
  if(glimage == NULL) return;
  if(EZ_InCompilingMode != 0)
    {
      EZ_Token *t = AllocateNTokens(9);
      t->i = TOKEN_PUT_IMAGE_SCREEN;
      t[1].ptr = glimage;
      t[2].i = Sx;
      t[3].i = Sy;
      t[4].i = Sw;
      t[5].i = Sh;
      t[6].f = fx;
      t[7].f = fy;
      t[8].f = fz;
    }
  if(EZ_InExecutionMode != 0)
    {
      float tmpf[4];
      int ix, iy, iz;  

      tmpf[0] = fx;
      tmpf[1] = fy;
      tmpf[2] = fz;
      tmpf[3] = 1.0;

      TRANSFORM_2_SCREEN(tmpf, ix, iy, iz);
      EZ_CURRENT_IMAGE_FUNC(glimage, Sx, Sy, Sw,Sh,ix,iy,iz);
    }
}

void EZ_PutGLImageXWindow(glimage, Sx, Sy, Sw, Sh, ix, iy)
     EZ_GLImage *glimage;
     int Sx, Sy, Sw, Sh;
     int ix, iy;
{
  if(glimage == NULL) return;
  if(EZ_InCompilingMode != 0)
    {
      EZ_Token *t = AllocateNTokens(9);
      t->i = TOKEN_PUT_IMAGE_WINDOW;
      t[1].ptr = glimage;
      t[2].i = Sx;
      t[3].i = Sy;
      t[4].i = Sw;
      t[5].i = Sh;
      t[6].i = ix;
      t[7].i = iy;
    }
  if(EZ_InExecutionMode != 0)
    {
      int iz = - (1 << 30);
      EZ_CURRENT_IMAGE_FUNC(glimage, Sx, Sy, Sw,Sh,ix,iy,iz);
    }
}

void EZ_PutGLImageWindow(glimage, Sx, Sy, Sw, Sh, ix, iy)
     EZ_GLImage *glimage;
     int Sx, Sy, Sw, Sh;
     int ix, iy;
{
  if(glimage == NULL) return;
  if(EZ_InCompilingMode != 0)
    {
      EZ_Token *t = AllocateNTokens(9);
      t->i = TOKEN_PUT_IMAGE_XWINDOW;
      t[1].ptr = glimage;
      t[2].i = Sx;
      t[3].i = Sy;
      t[4].i = Sw;
      t[5].i = Sh;
      t[6].i = ix;
      t[7].i = iy;
    }
  if(EZ_InExecutionMode != 0)
    {
      int iz = - (1 << 30);
      iy = EZ_WindowHeight - iy;
      EZ_CURRENT_IMAGE_FUNC(glimage, Sx, Sy, Sw,Sh,ix,iy,iz);
    }
}


/**************************************************************************/
EZ_GLImage *EZ_CreateGLImage(bpixmap)
     EZ_Bitmap *bpixmap;
{
  if(bpixmap)
    {
      EZ_GLImage *glimage = (EZ_GLImage *)my_malloc(sizeof(EZ_GLImage), _GL_IMAGE_);
      if(glimage == NULL) return(NULL);
      bpixmap->ref += 1234;
      glimage->pixmap = bpixmap;
      glimage->ximage = NULL;
      glimage->maskdata = NULL;
      return(glimage);
    }
  return(NULL);
}
void EZ_FreeGLImage(glimage)
     EZ_GLImage *glimage;
{
  if(glimage)
    {
      glimage->pixmap->ref -= 1233;
      EZ_FreeLabelPixmap(glimage->pixmap);
      if(glimage->maskdata) my_free(glimage->maskdata);
      if(glimage->ximage) XDestroyImage(glimage->ximage);
    }
}
/************************************************************************************
 *
 * Copy an image to dpy buffer, in window coordinates, (x,y) = UL corner.
 */
void EZ_PutGLImageToDrawableNoZ(glimage, Sx, Sy, Sw, Sh, x, y, z)
     EZ_GLImage *glimage;
     int x,y,z;
     int Sx, Sy, Sw, Sh;
{
  EZ_Bitmap *bitmap;
  if(glimage && (bitmap = glimage->pixmap) != NULL)
    {
      if(Sx >= bitmap->width || Sy >= bitmap->height) return;
      if(Sx < 0) Sx = 0; if(Sy < 0) Sy = 0;
      if(Sx + Sw > bitmap->width) Sw = bitmap->width - Sx;
      if(Sy + Sh > bitmap->height) Sh = bitmap->height - Sy;

      if(x >= EZ_WindowWidth || x + Sw <= 0 ||
	 y >= EZ_WindowHeight || y +Sh <= 0) return;
      
      /* copy the pixmap */
      if(EZ_BitmapType(bitmap) == EZ_BITMAP_IS_BITMAP)
	{
	  /* set current color */
	  {
	    unsigned long pixel_value;

	    if(EZ_ColorMode == EZ_CMAP_MODE)
	      {
		int ir = EZ_CurrentColorI[0] >> COLOR_SHIFT_BITS;
		pixel_value = EZ_ColorArray[ir];
	      }
	    else
	      {
		int r = EZ_CurrentColorI[0] >> COLOR_SHIFT_BITS;
		int g = EZ_CurrentColorI[1] >> COLOR_SHIFT_BITS;
		int b = EZ_CurrentColorI[2] >> COLOR_SHIFT_BITS;
		if(EZ_Depth <= 8)
		  {
		    EZ_DitherRGB(pixel_value,4,5, r,g,b);
		  }
		else
		  {
		    pixel_value = RGB2PixelLookUp(r,g,b);
		  }
	      }
	    XSetForeground(EZ_Display,EZ_MainGC,pixel_value);
	  }
	  XSetClipMask(EZ_Display, EZ_MainGC, bitmap->pixmap);
	  XSetClipOrigin(EZ_Display, EZ_MainGC,x-Sx,y-Sy);
	  XCopyPlane(EZ_Display,
		     bitmap->pixmap,
		     EZ_Drawable,
		     EZ_MainGC,
		     Sx,Sy,
		     (unsigned int)(Sw),
		     (unsigned int)(Sh),
		     x,y,
		     1);
	  XSetClipMask(EZ_Display, EZ_MainGC, None);
	}
      else 
	{
	  if(bitmap->shape != (Pixmap)NULL)
	    {
	      XSetClipMask(EZ_Display, EZ_MainGC, bitmap->shape);
	      XSetClipOrigin(EZ_Display, EZ_MainGC,x-Sx,y-Sy);
	    }
	  XCopyArea(EZ_DisplayForWidgets,
		    bitmap->pixmap,
		    EZ_Drawable,
		    EZ_MainGC,
		    Sx,Sy,
		    (unsigned int)(Sw),
		    (unsigned int)(Sh),
		    x,y);      
	  if(bitmap->shape != (Pixmap)NULL)
	    XSetClipMask(EZ_Display, EZ_MainGC, None);
	}
      XSync(EZ_Display, False); 
    }
}


/***************************************************************************/
void EZ_PutGLImageToXImageNoZ(glimage, Sx, Sy, Sw, Sh, x, y, z)
     EZ_GLImage *glimage;
     int Sx, Sy, Sw, Sh;
     int x,y,z;
{
  EZ_Bitmap *bitmap;
  if(glimage && (bitmap = glimage->pixmap) != NULL)
    {
      if(Sx >= bitmap->width || Sy >= bitmap->height) return;
      if(Sx < 0) Sx = 0; if(Sy < 0) Sy = 0;
      if(Sx + Sw > bitmap->width) Sw = bitmap->width - Sx;
      if(Sy + Sh > bitmap->height) Sh = bitmap->height - Sy;

      if(x >= EZ_WindowWidth || x + Sw <= 0 ||
	 y >= EZ_WindowHeight || y +Sh <= 0) return;

      if(glimage->maskdata == NULL)
	{
	  Pixmap pixmap;
	  if(EZ_BitmapType(bitmap) == EZ_BITMAP_IS_BITMAP) pixmap = bitmap->pixmap;
	  else pixmap = bitmap->shape;
	  if(pixmap != None)
	    {
	      int width = bitmap->width;
	      int height = bitmap->height;
	      XImage *image = XGetImage(EZ_Display, pixmap, 0, 0,
					width,height, 1L, XYPixmap);
	      if(image)
		{
		  register int xx, c, b;
		  register char *ptr;
		  int yy, bytes_per_line, tsize;

		  bytes_per_line = (width+7)/8;
		  tsize = bytes_per_line * height;
		  glimage->maskBPL = bytes_per_line;
		  glimage->maskdata = (char *) my_malloc(tsize, _GL_IMAGE_);

		  if(glimage->maskdata != NULL)
		    {
		      ptr = glimage->maskdata;
		      c = 0; b=1;
		      for(yy = 0; yy < height; yy++) 
			{
			  for(xx = 0; xx < width; )
			    {
			      if(XGetPixel(image, xx, yy)) c |= b;
			      b <<= 1;
			      if(!(++xx & 7)) {  *(ptr++)=c;  c=0; b=1;}
			    }
			  if(xx & 7) {  *(ptr++)=c;   c=0; b=1;}
			}
		    }
		  XDestroyImage(image);
		}
	    }
	}
      /* now render the pixmap */
      if(EZ_BitmapType(bitmap) == EZ_BITMAP_IS_BITMAP)
	{
	  if(glimage->maskdata != NULL)
	    {
	      /* find current color */
	      unsigned long pixel_value;

	      if(EZ_ColorMode == EZ_CMAP_MODE)
		{
		  int ir = EZ_CurrentColorI[0] >> COLOR_SHIFT_BITS;
		  pixel_value = EZ_ColorArray[ir];
		}
	      else
		{
		  int r = EZ_CurrentColorI[0] >> COLOR_SHIFT_BITS;
		  int g = EZ_CurrentColorI[1] >> COLOR_SHIFT_BITS;
		  int b = EZ_CurrentColorI[2] >> COLOR_SHIFT_BITS;
		  if(EZ_Depth <= 8)
		    {
		      EZ_DitherRGB(pixel_value,4,5, r,g,b);
		    }
		  else
		    {
		      pixel_value = RGB2PixelLookUp(r,g,b);
		    }
		}
	      /* copy the bitmap to XImage backbuffer*/
	      {
		int dx0, dy0, sx0, sx1, sy0, sy1, madvance, badvance;
		register char *mptr;

		if(x < 0) {sx0 = Sx-x; Sw += x; dx0 = 0;}
		else {sx0 = Sx; dx0 = x;}
		if(y < 0) {sy0 = Sy-y; Sh += y; dy0 = 0;}
		else {sy0 = Sy; dy0 = y; }
	
		if(x + Sw < EZ_WindowWidth) { sx1 = sx0 + Sw;}
		else { sx1 = sx0 + EZ_WindowWidth - x ;}
		if(y+Sh < EZ_WindowHeight) { sy1 = sy0+Sh;}
		else {sy1 =  sy0 + EZ_WindowHeight - y;}
		madvance = glimage->maskBPL;
		badvance = EZ_Image3->bytes_per_line;
		mptr = glimage->maskdata + sy0 * madvance;

		if(EZ_Depth == 8)
		  {
		    unsigned char *destdata = EZ_ImageData;
		    unsigned char *destptr = destdata +(dy0 * badvance) + dx0;
		    int sx, sy;
		    
		    for(sy = sy0; sy < sy1; sy++)
		      {
			register unsigned char *dest = destptr;
			for(sx = sx0; sx < sx1; sx++)
			  {
			    int tidx = sx >> 3;
			    int tbit =  1 << (sx & 7);
			    if(mptr[tidx] & tbit) 
			      *dest = pixel_value;
			    dest++;
			  }
			destptr += badvance;
			mptr += madvance;
		      }
		  }
		else if(EZ_Depth <= 16)
		  {
		    unsigned char *destdata = EZ_ImageData;
		    unsigned char *destptr;
		    int sx, sy;

		    destptr = destdata +(dy0 * badvance) + (dx0 * sizeof(short)/sizeof(char));
		    for(sy = sy0; sy < sy1; sy++)
		      {
			register unsigned short *dest = (unsigned short *)destptr;
			for(sx = sx0; sx < sx1; sx++)
			  {
			    int tidx = sx >> 3;
			    int tbit =  1 << (sx & 7);
			    if(mptr[tidx] & tbit)   *dest = pixel_value;
			    dest++;
			  }
			destptr += badvance;
			mptr += madvance;
		      }
		  }	
		else if(EZ_Depth >= 24 && EZ_BitsPerPixel == 32)
		  {
		    unsigned char *destdata = EZ_ImageData;
		    unsigned char *destptr;
		    int sx, sy;

		    destptr = destdata +(dy0 * badvance) +(dx0 * sizeof(int)/sizeof(char));
		    for(sy = sy0; sy < sy1; sy++)
		      {
			register unsigned int *dest = (unsigned int *)destptr;
			for(sx = sx0; sx < sx1; sx++)
			  {
			    int tidx = sx >> 3;
			    int tbit =  1 << (sx & 7);
			    if(mptr[tidx] & tbit)   *dest = pixel_value;
			    dest++;
			  }
			destptr += badvance;
			mptr += madvance;
		      }
		  } 
		else if(EZ_Depth == 24 && EZ_BitsPerPixel == 24)
		  {
		    unsigned char *destdata = EZ_ImageData;
		    unsigned char *destptr;
		    unsigned char *pv4 = (unsigned char *)&pixel_value;
		    int sx, sy;

		    destptr = destdata +(dy0 * badvance) + (3 * dx0);
		    for(sy = sy0; sy < sy1; sy++)
		      {
			register unsigned char *dest = destptr;
			for(sx = sx0; sx < sx1; sx++)
			  {
			    int tidx = sx >> 3;
			    int tbit =  1 << (sx & 7);
			    if(mptr[tidx] & tbit)
			      {	dest[0] = pv4[0]; dest[1] = pv4[1]; dest[2] = pv4[2]; }
			    dest += 3;
			  }
			destptr += badvance;
			mptr += madvance;
		      }
		  }
		else 
		  {
		    /* give up */
		  }
	      }
	    }
	}
      else   /* Pixmap */
	{
	  if(glimage->ximage == NULL)
	    glimage->ximage = XGetImage(EZ_Display, bitmap->pixmap, 0,0,
					(unsigned int)(bitmap->width),
					(unsigned int)(bitmap->height),
					(1<< EZ_Depth)-1,
					ZPixmap);
	  if(glimage->ximage != NULL)
	    {
	      int dx0, dy0, sx0, sx1, sy0, sy1, advance, badvance;

	      if(x < 0) {sx0 = Sx-x; Sw += x; dx0 = 0;}
	      else {sx0 = Sx; dx0 = x;}
	      if(y < 0) {sy0 = Sy-y; Sh += y; dy0 = 0;}
	      else {sy0 = Sy; dy0 = y; }
	
	      if(x + Sw < EZ_WindowWidth) { sx1 = sx0 + Sw;}
	      else { sx1 = sx0 + EZ_WindowWidth - x ;}
	      if(y+Sh < EZ_WindowHeight) { sy1 = sy0+Sh;}
	      else {sy1 =  sy0 + EZ_WindowHeight - y;}

	      advance = glimage->ximage->bytes_per_line;
	      badvance = EZ_Image3->bytes_per_line;

	      if(glimage->maskdata != NULL)
		{
		  int madvance = glimage->maskBPL;
		  register char *mptr = glimage->maskdata + sy0 * madvance;
		  
		  if(EZ_Depth == 8)
		    {
		      unsigned char *srcdata = glimage->ximage->data;
		      unsigned char *destdata = EZ_ImageData;
		      unsigned char *srcptr, *destptr;
		      int sx, sy;
		      srcptr = srcdata + (sy0 * advance) + sx0;
		      destptr = destdata +(dy0 * badvance) + dx0;
		      for(sy = sy0; sy < sy1; sy++)
			{
			  register unsigned char *src = srcptr;
			  register unsigned char *dest = destptr;
			  
			  for(sx = sx0; sx < sx1; sx++)
			    {
			      int tidx = sx >> 3;
			      int tbit =  1 << (sx & 7);
			      if(mptr[tidx] & tbit)  *dest= *src;
			      dest++; src++;
			    }
			  srcptr += advance;
			  destptr += badvance;
			  mptr += madvance;
			}
		    }
		  else if(EZ_Depth <= 16)
		    {
		      unsigned char *srcdata = glimage->ximage->data;
		      unsigned char *destdata = EZ_ImageData;
		      unsigned char *srcptr, *destptr;
		      int sx, sy;
		      srcptr = srcdata + (sy0 * advance) + (sx0 * sizeof(short)/sizeof(char));
		      destptr = destdata +(dy0 * badvance) + (dx0 * sizeof(short)/sizeof(char));
		      for(sy = sy0; sy < sy1; sy++)
			{
			  unsigned short *src, *dest;
			  src = ((unsigned short *)srcptr);
			  dest =((unsigned short *)destptr);
			  for(sx = sx0; sx < sx1; sx++)
			    {
			      int tidx = sx >> 3;
			      int tbit =  1 << (sx & 7);
			      if(mptr[tidx] & tbit)   *dest = *src;
			      dest++; src++;
			    }
			  srcptr += advance;
			  destptr += badvance;
			  mptr += madvance;
			}
		    }
		  else if(EZ_Depth >= 24 && EZ_BitsPerPixel == 32)
		    {
		      unsigned char *srcdata = glimage->ximage->data;
		      unsigned char *destdata = EZ_ImageData;
		      unsigned char *srcptr, *destptr;
		      int sx, sy;
		      srcptr = srcdata + (sy0 * advance) + (sx0 * sizeof(int)/sizeof(char));
		      destptr = destdata +(dy0 * badvance) +(dx0 * sizeof(int)/sizeof(char));
		      for(sy = sy0; sy < sy1; sy++)
			{
			  unsigned int *src, *dest;
			  src = ((unsigned int *)srcptr);
			  dest = ((unsigned int *)destptr);
			  for(sx = sx0; sx < sx1; sx++)
			    {
			      int tidx = sx >> 3;
			      int tbit =  1 << (sx & 7);
			      if(mptr[tidx] & tbit)   *dest = *src;
			      dest++; src++;
			    }
			  srcptr += advance;
			  destptr += badvance;
			  mptr += madvance;
			}
		    }
		  else  if(EZ_Depth >= 24 && EZ_BitsPerPixel == 24)
		    {
		      unsigned char *srcdata = glimage->ximage->data;
		      unsigned char *destdata = EZ_ImageData;
		      unsigned char *srcptr, *destptr;
		      int sx, sy;
		      srcptr = srcdata + (sy0 * advance) + (3 * sx0);
		      destptr = destdata +(dy0 * badvance) + (3 * dx0);
		      for(sy = sy0; sy < sy1; sy++)
			{
			  register unsigned char *src = srcptr;
			  register unsigned char *dest = destptr;			  
			  for(sx = sx0; sx < sx1; sx++)
			    {
			      int tidx = sx >> 3;
			      int tbit =  1 << (sx & 7);
			      if(mptr[tidx] & tbit)
				{ dest[0] = src[0]; dest[1] = src[1]; dest[2] = src[2]; }
			      dest += 3; src += 3;
			    }
			  srcptr += advance;
			  destptr += badvance;
			  mptr += madvance;
			}
		    }
		  else
		    {
		      /* should not get to here */
		    }
		}
	      else /* no mask */
		{
		  if(EZ_Depth == 8)
		    {
		      unsigned char *srcdata = glimage->ximage->data;
		      unsigned char *destdata = EZ_ImageData;
		      unsigned char *srcptr, *destptr;
		      int sx, sy;
		      srcptr = srcdata + (sy0 * advance) + sx0;
		      destptr = destdata +(dy0 * badvance) + dx0;
		      for(sy = sy0; sy < sy1; sy++)
			{
			  register unsigned char *src = srcptr;
			  register unsigned char *dest = destptr;
			  for(sx = sx0; sx < sx1; sx++)   *dest++ = *src++;
			  srcptr += advance;
			  destptr += badvance;
			}
		    }
		  else if(EZ_Depth <= 16)
		    {
		      unsigned char *srcdata = glimage->ximage->data;
		      unsigned char *destdata = EZ_ImageData;
		      unsigned char *srcptr, *destptr;
		      int sx, sy;
		      srcptr = srcdata + (sy0 * advance) + (sx0 * sizeof(short)/sizeof(char));
		      destptr = destdata +(dy0 * badvance) + (dx0 * sizeof(short)/sizeof(char));
		      for(sy = sy0; sy < sy1; sy++)
			{
			  unsigned short *src, *dest;
			  src = (unsigned short *)srcptr;
			  dest =(unsigned short *)destptr;
			  for(sx = sx0; sx < sx1; sx++)    *dest++ = *src++;
			  srcptr += advance;
			  destptr += badvance;
			}
		    }
		  else if(EZ_Depth >= 24 && EZ_BitsPerPixel == 32)
		    {
		      unsigned char *srcdata = glimage->ximage->data;
		      unsigned char *destdata = EZ_ImageData;
		      unsigned char *srcptr, *destptr;
		      int sx, sy;
		      srcptr = srcdata + (sy0 * advance) + (sx0 * sizeof(int)/sizeof(char));
		      destptr = destdata +(dy0 * badvance) +(dx0 * sizeof(int)/sizeof(char));
		      for(sy = sy0; sy < sy1; sy++)
			{
			  unsigned int *src, *dest;
			  src = (unsigned int *)srcptr;
			  dest = (unsigned int *)destptr;
			  for(sx = sx0; sx < sx1; sx++)    *dest++ = *src++;
			  srcptr += advance;
			  destptr += badvance;
			}
		    }
		  else  if(EZ_Depth >= 24 && EZ_BitsPerPixel == 24)
		    {
		      unsigned char *srcdata = glimage->ximage->data;
		      unsigned char *destdata = EZ_ImageData;
		      unsigned char *srcptr, *destptr;
		      int sx, sy;
		      srcptr = srcdata + (sy0 * advance) + (3 * sx0);
		      destptr = destdata +(dy0 * badvance) + (3 * dx0);
		      for(sy = sy0; sy < sy1; sy++)
			{
			  register unsigned char *src = srcptr;
			  register unsigned char *dest = destptr;
			  
			  for(sx = sx0; sx < sx1; sx++)
			    {*dest++ = *src++; *dest++ = *src++; *dest++ = *src++;}
			  srcptr += advance;
			  destptr += badvance;
			}
		    }
		  else
		    {
		      /* should not get to here */
		    }
		}
	      XSync(EZ_Display, False); 
	    }
	}
    }
}
/************************************************************************************/    
void EZ_PutGLImageNoOp(glimage, Sx, Sy, Sw, Sh, x, y, z)
     EZ_GLImage *glimage;
     int x,y,z;
     int Sx, Sy, Sw, Sh;
{
  XSync(EZ_Display, False); 
}
/************************************************************************************/    
int EZ_GetGLImageInfo(image, p_ret, s_ret, w_ret, h_ret)
     EZ_GLImage *image;
     Pixmap *p_ret, *s_ret;
     int *w_ret, *h_ret;
{
  if(image) return(EZ_GetLabelPixmapInfo(image->pixmap, p_ret, s_ret, w_ret, h_ret));
  return(0);
}

/************************************************************************************/    
static void EZ_GetGLImageWork(x,y,w,h, imgRet,mode)
     int x, y,  w,  h, mode;
     EZ_GLImage **imgRet;
{
  if(EZ_InCompilingMode != 0)
    {
      EZ_Token *t = AllocateNTokens(6);
      t->i = mode;
      t[1].i = x;
      t[2].i = y;
      t[3].i = w;
      t[4].i = h;
      t[5].ptr = (void *)imgRet;
      *imgRet = NULL;
    }
  if(EZ_InExecutionMode != 0)
    {
      EZ_GLImage *glimage;
      EZ_Bitmap  *bitmap;
      Pixmap pixmap;
      int x0,y0, ww, hh;
      
      x0 = y0 = 0; ww = w; hh = h;
      if(x < 0) { x0 = -x; x = 0;  w += x;}
      if(y < 0) { y0 = -y; y = 0; h += y;}
      
      if(x+w > EZ_WindowWidth)  w = EZ_WindowWidth - x;
      if(y+h > EZ_WindowHeight) h = EZ_WindowHeight - y;
      if(w <= 0 || h <= 0)   {  *imgRet = NULL; return; }

      pixmap = XCreatePixmap(EZ_Display, EZ_Drawable, ww, hh, EZ_Depth); 
      if(pixmap == None) { *imgRet = NULL; return; }
      bitmap = (EZ_BitmapHTEntry *)my_malloc(sizeof(EZ_BitmapHTEntry), _BITMAP_HT_ENTRY_);
      if(bitmap == NULL) { XFreePixmap(EZ_Display, pixmap); *imgRet = NULL; return; }
      glimage = (EZ_GLImage *)my_malloc(sizeof(EZ_GLImage), _GL_IMAGE_);
      if(glimage == NULL) 
	{
	  (void)my_free(bitmap);
	  XFreePixmap(EZ_Display, pixmap); 
	  *imgRet = NULL;
	  return; 
	}
      bitmap->ref = 1234;
      bitmap->special = 0;
      bitmap->bucket =  EZ_WIDGET_HASH_TABLE_SIZE + 2;
      bitmap->type = EZ_BITMAP_IS_PIXMAP;
      bitmap->width = ww;
      bitmap->height = hh;
      bitmap->x = 0;
      bitmap->y = 0;
      bitmap->pixmap = pixmap;
      bitmap->htpixmap = None;
      bitmap->dkpixmap = None;
      bitmap->btpixmap = None;
      bitmap->shape = None;
      bitmap->bucket = 0;
      bitmap->name = NULL;
      bitmap->next =NULL;
      glimage->pixmap = bitmap;
      glimage->ximage = NULL;
      glimage->maskdata = NULL;
      glimage->maskBPL = 0;
 
      *imgRet = glimage;
      
      /* front buffer  or pixmap backbuffer */
      if(EZ_FrontBufferSet != 0 || EZ_DoubleBufferSet == EZ_PIXMAP) 
	XCopyArea(EZ_Display,EZ_Drawable,pixmap,EZ_MainGC, x, y, w, h, x0, y0);
      else
	{
#if defined(HAVE_XSHM) && defined(USE_XSHM)
	  if(EZ_HasSHM != 0)
	    XShmPutImage(EZ_Display, pixmap, EZ_MainGC, EZ_Image3, x, y, 
			 x0, y0, w, h, False);
	  else
#endif
	    XPutImage(EZ_Display, pixmap, EZ_MainGC, EZ_Image3, x,y, 
		      x0, y0, w,h);
	}
      XSync(EZ_Display, False);
    }
}
/************************************************************************************/    
void EZ_GetGLImageWindow(x,y,w,h, imgRet)
     int x, y,  w,  h;
     EZ_GLImage **imgRet;
{
  EZ_GetGLImageWork(x,EZ_WindowHeight - y,w,h, imgRet, TOKEN_GET_IMAGE_WINDOW);
}
void EZ_GetGLImageXWindow(x,y,w,h, imgRet)
     int x, y,  w,  h;
     EZ_GLImage **imgRet;
{
  EZ_GetGLImageWork(x,y,w,h, imgRet, TOKEN_GET_IMAGE_XWINDOW);
}
/************************************************************************************/    
EZ_Bitmap *EZ_GetLabelPixmapFromGLImage(image)
     EZ_GLImage *image;
{
  EZ_Bitmap *b = NULL;
  if(image)
    {
      b = image->pixmap;
      if(b) b->ref++;
    }
  return(b);
}
/************************************************************************************/    
#undef _EZ_GLIMAGE_C_
