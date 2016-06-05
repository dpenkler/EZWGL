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
#define _EZ_PNG_C_

#include "EZ_Widget.h"
/**************************************************************/
int EZ_ReadPngToPixmap MY_ANSIARGS((
				     char     *file,
				     int      *width_return,
				     int       *height_return,
				     Pixmap   *pixmap_return));
int EZ_ReadPngToRGB MY_ANSIARGS((
                                  char     *file,
                                  int      *width_return,
                                  int       *height_return,
                                  unsigned char  **rgb_return));
/**************************************************************/
#if defined(HAVE_LIBPNG) && defined(PNG_SUPPORT)
#undef EXTERN 
#include <png.h>

static int EZ_ReadPng(fname, w_ret, h_ret, p_ret, rgb_return)
     char *fname;
     int *w_ret, *h_ret;
     Pixmap *p_ret;
     unsigned char **rgb_return;
{
  FILE               *fp;
  png_structp         png_ptr;
  png_infop           info_ptr;
  unsigned char      *data, *ptr, *ptr2, **rows, r, g, b, a;
  int                 i, x, y, bit_depth, color_type, interlace_type;
  png_uint_32        w, h, tmp;
  png_color_16p image_background;
  double file_gamma;

  if((fp = fopen(fname, "r")) == NULL)   return(0);
  
  png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
  if(!png_ptr) { fclose(fp); return(0);}
  if((info_ptr = png_create_info_struct(png_ptr)) == NULL)
    {
      png_destroy_read_struct(&png_ptr, NULL, NULL);
      fclose(fp);
      return(0);
    }
  if(setjmp(png_jmpbuf(png_ptr)))
    {
      fclose(fp);
      png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
      return(0);
    }
  
  if(png_get_color_type(png_ptr,info_ptr) == PNG_COLOR_TYPE_RGB_ALPHA)
    {
      png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
      return(0);
    }
  png_init_io(png_ptr, fp);
  png_read_info(png_ptr, info_ptr);  /* header */
  png_get_IHDR(png_ptr, info_ptr, &w, &h, &bit_depth, &color_type, &interlace_type, NULL, NULL);

  if(png_get_bit_depth(png_ptr,info_ptr) < 8) png_set_packing(png_ptr);
  if(png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS))   png_set_expand(png_ptr);
   png_set_filler(png_ptr, 0xff, PNG_FILLER_AFTER);

  if (png_get_gAMA(png_ptr, info_ptr, &file_gamma))
        png_set_gamma(png_ptr, 2.2, file_gamma);
  else  png_set_gamma(png_ptr, 2.2, 0.45);

   
   if (png_get_bKGD(png_ptr, info_ptr, &image_background))
       png_set_background(png_ptr, image_background,
           PNG_BACKGROUND_GAMMA_FILE, 1/*needs to be expanded*/, 1);
  else 
    {
      png_color_16 my_background;
      my_background.red = my_background.green = my_background.blue = my_background.gray = 0;
      png_set_background(png_ptr, &my_background, PNG_BACKGROUND_GAMMA_SCREEN, 0, 2.2);
    }

  if(png_get_bit_depth(png_ptr,info_ptr) == 16) png_set_strip_16(png_ptr);

  if((png_get_color_type(png_ptr,info_ptr) == PNG_COLOR_TYPE_GRAY) || 
     (png_get_color_type(png_ptr,info_ptr) == PNG_COLOR_TYPE_GRAY_ALPHA))
     png_set_expand(png_ptr);
     
   if((data = (unsigned char *)my_malloc( 4 * w * (h+1)* sizeof(unsigned char), _PNG_IMAGE_)) == NULL)
     {
       png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
       fclose(fp);
       return(0);
     }
   if((rows = (unsigned char **)my_malloc(h * sizeof(unsigned char *), _PNG_IMAGE_)) == NULL)
     {
       (void)my_free(data);
       png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
       fclose(fp);
       return(0);
     }
   tmp = 4 * w;
   ptr = data + tmp;
   for(i = 0; i < h; i++) { rows[i] = ptr; ptr += tmp;}
   png_read_image(png_ptr, rows);
   png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
   ptr = data;
   if((color_type == PNG_COLOR_TYPE_GRAY) ||
       (color_type == PNG_COLOR_TYPE_GRAY_ALPHA))
     {
       for(y = 0; y < h; y++)
         {
           ptr2 = rows[y];
           for(x = 0; x < w; x++)
             {
               r = *ptr2++;
               a = *ptr2++;
               *ptr++ = r;
               *ptr++ = r;
               *ptr++ = r;
             }
         }
     }
   else
     {
	for(y = 0; y < h; y++)
	  {
            ptr2 = rows[y];
            for(x = 0; x < w; x++)
              {
                r = *ptr2++;
                g = *ptr2++;
                b = *ptr2++;
                a = *ptr2++;
                *ptr++ = r;
                *ptr++ = g;
                *ptr++ = b;
              }
	  }
     }
   fclose(fp);
   my_free(rows);

  /* finally .... */
  *w_ret = w;
  *h_ret = h; 
  if(rgb_return)
    {
      *rgb_return = data;
      return(1);
    }
  else
    {
      EZ_RawRGBData2Pixmap(data, w,h,p_ret);
      (void)my_free((char *)data);
      if(*p_ret) { return (1);}
    }
  return(0);
}
/**************************************************************/  
int EZ_ReadPngToPixmap(fname, w_ret, h_ret, p_ret)
     char *fname;
     int *w_ret, *h_ret;
     Pixmap *p_ret;
{ return(EZ_ReadPng(fname, w_ret, h_ret, p_ret, NULL)); }

int EZ_ReadPngToRGB(fname, w_ret, h_ret, rgb_ret)
     char *fname;
     int *w_ret, *h_ret;
     unsigned char **rgb_ret;
{ return(EZ_ReadPng(fname, w_ret, h_ret, NULL, rgb_ret)); }
/**************************************************************/  
#else
int EZ_ReadPngToPixmap(fname, w_ret, h_ret, p_ret)
     char *fname;
     int *w_ret, *h_ret;
     Pixmap *p_ret;
{
  *w_ret = 0; *h_ret = 0; 
  *p_ret = (Pixmap)NULL;
  return(0);
}
int EZ_ReadPngToRGB(fname, w_ret, h_ret, rgb_ret)
     char *fname;
     int *w_ret, *h_ret;
     unsigned char **rgb_ret;
{
  *w_ret = 0; *h_ret = 0; 
  return(0);
}
#endif
/**************************************************************/  
#undef _EZ_PNG_C_
