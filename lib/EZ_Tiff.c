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
#define _EZ_TIFF_C_

#include "EZ_Widget.h"
/**************************************************************/
int EZ_ReadTiffToPixmap MY_ANSIARGS((
				     char     *file,
				     int      *width_return,
				     int       *height_return,
				     Pixmap   *pixmap_return));
int EZ_ReadTiffToRGB MY_ANSIARGS((
                                  char     *file,
                                  int      *width_return,
                                  int       *height_return,
                                  unsigned char  **rgb_return));
/**************************************************************/
#if defined(HAVE_LIBTIFF) && defined(TIFF_SUPPORT)
#undef EXTERN 
#include <tiffio.h>

static int EZ_ReadTiff(fname, w_ret, h_ret, p_ret, rgb_return)
     char *fname;
     int *w_ret, *h_ret;
     Pixmap *p_ret;
     unsigned char **rgb_return;
{
  TIFF               *tif;
  unsigned char      *data, *ptr, r, g, b, a;
  int                 x, y, tmp;
  uint32              w, h, *rast, *tptr;

  if(fname == NULL)   return(0);
  if((tif = TIFFOpen(fname, "r")) == NULL) return(0);
  TIFFGetField(tif, TIFFTAG_IMAGEWIDTH, &w);
  TIFFGetField(tif, TIFFTAG_IMAGELENGTH, &h);
  tmp = w * h;

  if((rast = (uint32 *) _TIFFmalloc(tmp * sizeof(uint32)))== NULL)
    {
      TIFFClose(tif);
      return(0);
    }
  data = NULL;
  if(TIFFReadRGBAImage(tif, w, h, rast, 0))
    {
      if((data = (unsigned char *)my_malloc(w * h * 3 * sizeof(unsigned char),
                                        _TIFF_IMAGE_)) == NULL)
        {
          _TIFFfree(rast);
          TIFFClose(tif);
          return(0);
        }
      ptr = data;
      for(y = 0; y < h; y++)
        {
          tptr = rast;
          tptr += ((h - y - 1) * w);
          for(x = 0; x < w; x++)
            {
              a = TIFFGetA(*tptr);
              b = TIFFGetB(*tptr);
              g = TIFFGetG(*tptr);
              r = TIFFGetR(*tptr);
              tptr++;
              *ptr++ = r;
              *ptr++ = g;
              *ptr++ = b;
            }
        }
    }
  _TIFFfree(rast);
  TIFFClose(tif);

  /* finally .... */
  *w_ret = (int)w;
  *h_ret = (int)h; 
  if(rgb_return)
    {
      *rgb_return = data;
      return(1);
    }
  else
    {
      EZ_RawRGBData2Pixmap(data, (int)w, (int)h,p_ret);
      (void)my_free((char *)data);
      if(*p_ret) { return (1);}
    }
  return(0);
}
/**************************************************************/  
int EZ_ReadTiffToPixmap(fname, w_ret, h_ret, p_ret)
     char *fname;
     int *w_ret, *h_ret;
     Pixmap *p_ret;
{ return(EZ_ReadTiff(fname, w_ret, h_ret, p_ret, NULL)); }

int EZ_ReadTiffToRGB(fname, w_ret, h_ret, rgb_ret)
     char *fname;
     int *w_ret, *h_ret;
     unsigned char **rgb_ret;
{ return(EZ_ReadTiff(fname, w_ret, h_ret, NULL, rgb_ret)); }
/**************************************************************/  
#else
int EZ_ReadTiffToPixmap(fname, w_ret, h_ret, p_ret)
     char *fname;
     int *w_ret, *h_ret;
     Pixmap *p_ret;
{
  *w_ret = 0; *h_ret = 0; 
  *p_ret = (Pixmap)NULL;
  return(0);
}
int EZ_ReadTiffToRGB(fname, w_ret, h_ret, rgb_ret)
     char *fname;
     int *w_ret, *h_ret;
     unsigned char **rgb_ret;
{
  *w_ret = 0; *h_ret = 0; 
  return(0);
}
#endif
/**************************************************************/  
#undef _EZ_TIFF_C_
