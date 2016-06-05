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
#define _EZ_JPEG_C_

#include "EZ_Widget.h"
/**************************************************************/
int EZ_ReadJpegToPixmap MY_ANSIARGS((
				     char     *file,
				     int      *width_return,
				     int       *height_return,
				     Pixmap   *pixmap_return));
int EZ_ReadJpegToRGB MY_ANSIARGS((
                                  char     *file,
                                  int      *width_return,
                                  int       *height_return,
                                  unsigned char  **rgb_return));
/**************************************************************/
#if defined(HAVE_LIBJPEG) && defined(JPEG_SUPPORT)
/* these consts are defined in jpeg.h sigh- */
#undef EXTERN
#undef HAVE_STDLIB_H
#undef HAVE_STDDEF_H 
#include <setjmp.h>
#include "jpeglib.h"
#include "jerror.h"

/**************************************************************/
static void    jp_error_exit MY_ANSIARGS((j_common_ptr));
static void    jp_error_output MY_ANSIARGS((j_common_ptr));
/**************************************************************/
typedef struct my_error_mgr_
{
  struct jpeg_error_mgr pub;
  jmp_buf   setjmp_buffer;
} my_error_mgr;

typedef struct my_error_mgr_ *my_error_ptr;
/**************************************************************/

static int EZ_ReadJpeg(fname, w_ret, h_ret, p_ret, rgb_return)
     char *fname;
     int *w_ret, *h_ret;
     Pixmap *p_ret;
     unsigned char **rgb_return;
{
  static unsigned char             *data = NULL;
  FILE                             *fp = NULL;
  struct jpeg_decompress_struct    cinfo;
  JSAMPROW                         rowptr[1];
  my_error_mgr                     merr;
  int                              w,h,bperpix;

  if((fp = fopen(fname, "r")) == NULL)   return 0;
  /* Step 1: allocate and initialize JPEG decompression object */
  cinfo.err = jpeg_std_error(&merr.pub);
  merr.pub.error_exit     = jp_error_exit;
  merr.pub.output_message = jp_error_output;

  if(setjmp(merr.setjmp_buffer)) 
    {
      /* jpeg has signaled an error */
      jpeg_destroy_decompress(&cinfo);
      (void)fclose(fp); 
      if(data) (void)my_free(data);
      return(0);
    }
  jpeg_create_decompress(&cinfo);
  /* Step 2: specify data source (eg, a file) */
  jpeg_stdio_src(&cinfo, fp);
  /* Step 3: read file parameters with jpeg_read_header() */
  (void) jpeg_read_header(&cinfo, TRUE);

  jpeg_calc_output_dimensions(&cinfo);
  w = cinfo.output_width;
  h = cinfo.output_height;
 
  bperpix = cinfo.output_components;
  if(bperpix != 1 && bperpix != 3) 
    {
      fprintf(stderr, "%s:  can't read %d-plane JPEG file.",
	      fname, cinfo.output_components);
      jpeg_destroy_decompress(&cinfo);
      fclose(fp); 
      return 0;
    }

  if((data = (unsigned char *)my_malloc( 4 * w * h *
					 sizeof(unsigned char), _JPEG_IMAGE_)) == NULL)
    {
      jpeg_destroy_decompress(&cinfo);
      fclose(fp); 
      EZ_OutOfMemory("ReadJPEG");      
      return 0;
    }
  /* Step 4: set parameters for decompression */
  jpeg_start_decompress(&cinfo);

  /* Step 5: decompress */
  while(cinfo.output_scanline < cinfo.output_height) 
    {
      rowptr[0] = (JSAMPROW) &data[cinfo.output_scanline * w * bperpix];
      (void) jpeg_read_scanlines(&cinfo, rowptr, (JDIMENSION) 1);
    }

  if(cinfo.jpeg_color_space == JCS_GRAYSCALE) 
    {
      unsigned char *t1, *t2, c;
      int i,j;
      t1 = data;
      t2 = t1 + 3* w * h;
      memcpy(t2, t1, w * h);
      for(i = 0; i < h; i++)
	for(j = 0; j < w; j++)
	  {
	    c = *t2++;
	    *t1++ = c; *t1++ = c; *t1++ = c; 
	  }
    }
  /* Step 6: clean up */
  jpeg_finish_decompress(&cinfo);
  jpeg_destroy_decompress(&cinfo);
  fclose(fp); 
  
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
static void jp_error_exit(cinfo) 
     j_common_ptr cinfo;
{
  my_error_ptr myerr;

  myerr = (my_error_ptr) cinfo->err;
  (*cinfo->err->output_message)(cinfo);
  longjmp(myerr->setjmp_buffer, 1);  
}
/**************************************************************/  
static void jp_error_output(cinfo) 
     j_common_ptr cinfo;
{
  my_error_ptr myerr;
  char         buffer[JMSG_LENGTH_MAX];

  myerr = (my_error_ptr) cinfo->err;
  (*cinfo->err->format_message)(cinfo, buffer);

  fprintf(stderr, "%s\n", buffer);  
}
/**************************************************************/  
int EZ_ReadJpegToPixmap(fname, w_ret, h_ret, p_ret)
     char *fname;
     int *w_ret, *h_ret;
     Pixmap *p_ret;
{ return(EZ_ReadJpeg(fname, w_ret, h_ret, p_ret, NULL)); }

int EZ_ReadJpegToRGB(fname, w_ret, h_ret, rgb_ret)
     char *fname;
     int *w_ret, *h_ret;
     unsigned char **rgb_ret;
{ return(EZ_ReadJpeg(fname, w_ret, h_ret, NULL, rgb_ret)); }
/**************************************************************/  
#else
int EZ_ReadJpegToPixmap(fname, w_ret, h_ret, p_ret)
     char *fname;
     int *w_ret, *h_ret;
     Pixmap *p_ret;
{
  *w_ret = 0; *h_ret = 0; 
  *p_ret = (Pixmap)NULL;
  return(0);
}
int EZ_ReadJpegToRGB(fname, w_ret, h_ret, rgb_ret)
     char *fname;
     int *w_ret, *h_ret;
     unsigned char **rgb_ret;
{
  *w_ret = 0; *h_ret = 0; 
  return(0);
}
/**************************************************************/  
#endif /*HAVE_JPEG*/
#undef _EZ_JPEG_C_
