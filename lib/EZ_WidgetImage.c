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
/*
 *  May 1997. Handling of raw rgb images added by Arno Dabekaussen.
 */
/*********************************************************************
 ***                                                               ***
 ***           Create a pixmap from an image file.                 ***
 ***                                                               ***
 *********************************************************************/
#define _EZ_WIDGET_IMAGE_C_
#include "EZ_Widget.h"

/*********************************************************************
 * 
 *  Functions implemented in this file:
 */
int   EZ_CreateXPixmapFromImageFile MY_ANSIARGS((char *file, unsigned int *w, unsigned int *h, Pixmap *pix));
int   EZ_CreateXPixmapFromRawRGB MY_ANSIARGS((unsigned char *data, int width, int height, 
                                              Pixmap *pixmap_return));
void  EZ_RawRGBData2Pixmap MY_ANSIARGS((unsigned char *data, int w, int h, Pixmap *pix));
void  EZ_RawRGBData2PixmapWork MY_ANSIARGS((unsigned char *data, int w, int h, Pixmap *pix, EZ_XImage *im));
void  EZ_PixelValueData2Pixmap MY_ANSIARGS((unsigned int *data, int w, int h, Pixmap *pix));
void  EZ_SetDarkAndBrightPixmapReturns MY_ANSIARGS((Pixmap *dk, Pixmap *bt, Pixmap *ht));
int   EZ_GetImageFileType MY_ANSIARGS((char *file));
int   EZ_ReadImageFile MY_ANSIARGS(( char *file, int *width_return, int *height_return,
                                     unsigned char **rgb_return));
EZ_XImage *EZ_CreateXImage MY_ANSIARGS((int w, int h, int shmflag));
void   EZ_DestroyXImage  MY_ANSIARGS((EZ_XImage *image));
/*********************************************************************
 *
 *  Local functions:
 */
static int  EZ_ReadPPMToPixmap MY_ANSIARGS((char *file, int *w, int *h, Pixmap *pix));
static int  EZ_ReadPPM_FP_ToPixmap MY_ANSIARGS((FILE *fp, int *w, int *h, Pixmap *pix, 
                                                unsigned char **rgb_ret));
static int  EZ_ReadXPMToPixmap MY_ANSIARGS((char *file, int *w, int *h, Pixmap *pix));

static int  EZ_ReadPPMToRGB  MY_ANSIARGS((char *file, int *w, int *h, unsigned char **rgb));

/********************************************************************/

#define EZ_UNKNOWN_IMAGE_FORMAT   -1
#define EZ_PPM_FORMAT             0
#define EZ_GIF_FORMAT             1
#define EZ_XPM_FORMAT             2
#define EZ_BMP_FORMAT             3
#define EZ_JPG_FORMAT             4
#define EZ_PNG_FORMAT             5
#define EZ_TIF_FORMAT             6

/********************************************************************/
typedef int  (*PixmapReader) MY_ANSIARGS((char *file, int *w, int *h, Pixmap *pix));
typedef int  (*ImageReader) MY_ANSIARGS((char *file, int *w, int *h, unsigned char **rgb));
/********************************************************************/
static  PixmapReader PixmapReaders[] =
{
  EZ_ReadPPMToPixmap,       /* PNM/PPM/PGM Format */
  EZ_ReadGIFToPixmap,       /* GIF Format         */
  EZ_ReadXPMToPixmap,       /* XPM Format         */
  EZ_ReadBMPToPixmap,       /* BMP Format         */
  EZ_ReadJpegToPixmap,      /* JPEG Format        */
  EZ_ReadPngToPixmap,       /* PNG Format         */
  EZ_ReadTiffToPixmap,      /* TIFF Format        */
  NULL,
};
static  ImageReader ImageReaders[] =
{
  EZ_ReadPPMToRGB,       /* PNM/PPM/PGM Format */
  EZ_ReadGIFToRGB,       /* GIF Format         */
  EZ_ReadXPMToRGB,       /* XPM Format         */
  EZ_ReadBMPToRGB,       /* BMP Format         */
  EZ_ReadJpegToRGB,      /* JPEG Format        */
  EZ_ReadPngToRGB,       /* PNG  Format        */
  EZ_ReadTiffToRGB,      /* TIFF Format        */
  NULL,
};

/********************************************************************/

int EZ_GetImageFileType(fname)
     char *fname;
{
  int           ret = -1, fd = 0;
  unsigned char	buf[1028];
  int           nbytes = 0;

  if(!fname) return(-1);
  if((fd = open(fname, O_RDONLY)) < 0) 
    {
      (void)fprintf(stderr, "Cannot open %s\n", fname);
      return(-1);
    }
  if((nbytes = read(fd, (char *)buf, 1024)) <= 0)
    {
      (void)fprintf(stderr, "Cannot read from %s\n", fname);
      close(fd);
      return(-1);
    }
  (void) close(fd);
  buf[nbytes++] = '\0';	

  if(!strncmp(buf, "/* XPM */", 9))         ret = EZ_XPM_FORMAT;
  else if(!strncmp(buf, "GIF8", 4))         ret = EZ_GIF_FORMAT;
  else if(buf[0] == 'B' && buf[1] == 'M')   ret = EZ_BMP_FORMAT;
  else if(buf[0] == 'P' &&
	  (buf[1] >= '1' || buf[1] <= '6')) ret = EZ_PPM_FORMAT;
  else if((int)buf[0]== 255  &&  (int)buf[1]== 216 &&
	  (int)buf[2]== 255)                ret = EZ_JPG_FORMAT;
  else if((int)buf[0]== 137  && buf[1] == 'P' &&
          buf[2] == 'N' && buf[3] == 'G')   ret = EZ_PNG_FORMAT;
  else if(((buf[0] == 'M') && (buf[1] == 'M') && (buf[2] == 0x00) && (buf[3] == 0x2a)) ||
          ((buf[0] == 'I') && (buf[1] == 'I') && (buf[2] == 0x2a) && (buf[3] == 0x00)))
    ret = EZ_TIF_FORMAT;
  return(ret);
}

/*************************************************************************
 *
 *   Read a XPM image into an pixmap.
 */
static int EZ_ReadXPMToPixmap(file, width_return, height_return, pixmap_return)
     char     *file;                          /* image file_name         */
     int      *width_return, *height_return;  /* width and height return */
     Pixmap   *pixmap_return;                 /* pixmap return           */
{
  return(EZ_CreateXPixmapFromXpmFile(file, width_return, height_return,
				     pixmap_return, NULL));
}
/*************************************************************************
 *
 *   Read a PPM/PGM/PBM image into an pixmap.
 */
static int EZ_ReadPPMToPixmap(file, width_return, height_return, pixmap_return)
     char     *file;                          /* image file_name         */
     int      *width_return, *height_return;  /* width and height return */
     Pixmap   *pixmap_return;                 /* pixmap return           */
{
  FILE   *fp, *fopen();
  int    ok = 0;

  if(!file) return(0);
  if( (fp = fopen(file,"r")) == (FILE *)NULL)
    {
      (void) fprintf(stderr, "Cannot open image file %s\n", file);
      return(0);
    }
  ok = EZ_ReadPPM_FP_ToPixmap(fp, width_return, height_return, pixmap_return, NULL);
  (void) fclose(fp);
  return(ok);
}
/*************************************************************************/
static int EZ_ReadPPMToRGB(file, width_return, height_return, rgb_return)
     char     *file;                          /* image file_name         */
     int      *width_return, *height_return;  /* width and height return */
     unsigned char   **rgb_return;
{
  FILE   *fp, *fopen();
  int    ok = 0;

  if(!file) return(0);
  if( (fp = fopen(file,"r")) == (FILE *)NULL)
    {
      (void) fprintf(stderr, "Cannot open image file %s\n", file);
      return(0);
    }
  *rgb_return = NULL;
  ok = EZ_ReadPPM_FP_ToPixmap(fp, width_return, height_return, NULL, rgb_return);
  (void) fclose(fp);
  return(ok);
}
/*************************************************************************
 *
 *   Read a PPM/PGM/PBM image (from a stream descriptor) into an pixmap.
 */
static int EZ_ReadPPM_FP_ToPixmap(fp, width_return, height_return, pixmap_return, rgb_ret)
     FILE     *fp;                            /* stream descriptor       */
     int      *width_return, *height_return;  /* width and height return */
     Pixmap   *pixmap_return;                 /* pixmap return           */
     unsigned char **rgb_ret;
{
  unsigned char *tmpdata, *tptr;
  char          tbuf[512];
  int           r,g,b,i,j, c, maxval, width,height, fmt;
  Pixmap        pixmap;

  /*----------------------------------------------------------
   * read the PPM Header
   *--------------------------------------------------------*/
  {
    c = getc(fp);
    j = 0;
    for(i = 0; i < 4; i++)
      {
	for(; ;)
	  {
	    while(c == ' ' || c == '\t' || c == '\n') c = getc(fp);
	    if(c == '#')
	      {
		while(c != '\n' && c != EOF) c = getc(fp);  /* comment */
		c = getc(fp);
	      }
	    else break;
	  }
	while(!(isspace(c)) && (c != EOF)  && j < 500)
	  {
	    tbuf[j++] = c;
	    c = getc(fp);
	  }
	tbuf[j++] = ' ';
	tbuf[j] = '\0';
      }

    fmt = -1;
    if(tbuf[0] == 'P')
      {
	if(tbuf[1] == '6')         fmt = 6;
	else if(tbuf[1] == '5')    fmt = 5;
	else if(tbuf[1] == '3')    fmt = 3;
      }
    j = sscanf(tbuf+3,"%d %d %d", &width,&height,&maxval);
    if(fmt == -1 || j != 3 || width <= 0 || height <= 0 || maxval <= 0 || maxval > 255)
      return(0);
  }
  /*-----------------------------------------------------------
   *  PBM/PPM/PGM header is sucessfully read.
   *----------------------------------------------------------*/
  if( (tmpdata = (unsigned char *)my_malloc(4 * (width)*(height) *
					 sizeof(unsigned char),_PPM_TMP_D_)) == (unsigned char *)NULL)
    {
      EZ_OutOfMemory("EZ_ReadPPM_FP_ToPixmap");
      return(0);  
}
  /*---------------------------------------
   * Now read in image data
   *--------------------------------------*/
  if(fmt == 6)  /* PPM */
    {
      int length = width*height*3;

      tptr = tmpdata;
      (void) fread(tptr,1,length,fp);
      if(maxval != 255)
	{
	  tptr = tmpdata;
	  for(i = 0; i < length; i++)
	    {
	      j  = (int)(*tptr)*255/maxval;
	      *tptr = (unsigned char)j;
	      tptr++;
	    }
	}
    }
  else if(fmt == 5)  /* PGM */
    {
      int length = width*height;
      unsigned char uc, *ucptr;

      tptr = tmpdata + 3*length;
      (void) fread(tptr,1,length,fp);
      if(maxval != 255)
	{
	  tptr = tmpdata + 3*length;
	  ucptr = tmpdata;
	  for(i = 0; i < length; i++)
	    {
	      j  = (int)(*tptr)*255/maxval;
	      tptr++;
	      *ucptr++ = (unsigned char)j;
	      *ucptr++ = (unsigned char)j;
	      *ucptr++ = (unsigned char)j;
	    }
	}
      else
	{
	  tptr = tmpdata + 3*length;
	  ucptr = tmpdata;
	  
	  for(i = 0; i < length; i++)
	    {
	      uc = *tptr++;
	      *ucptr++ = uc;
	      *ucptr++ = uc;
	      *ucptr++ = uc;
	    }
	}
    }
  else if(fmt == 3)     /* PPM/ASCII */
    {
      tptr = tmpdata;
      for(i = 0; i < height; i++)
	{
	  for(j = 0; j < width; j++)
	    {
	      fscanf(fp,"%d", &r);
	      fscanf(fp,"%d", &g);
	      fscanf(fp,"%d", &b);
	      if( maxval != 255)
		{
		  r = r*255/maxval;
		  g = r*255/maxval;
		  b = r*255/maxval;
		}
	      *tptr++ = (unsigned char) r;
	      *tptr++ = (unsigned char) g;
	      *tptr++ = (unsigned char) b;
	    }
	}
    }
  /*-------------------------------------------------------------
   * Image is in tmpdata, now dither and render it into a pixmap
   *------------------------------------------------------------*/
  if(rgb_ret)
    {
      *width_return = width;
      *height_return = height;
      *rgb_ret = tmpdata;
      return(1);      
    }
  else
    {
      EZ_RawRGBData2Pixmap(tmpdata,width,height, &pixmap);
      (void)my_free( (char *)tmpdata);
      if(pixmap != (Pixmap )NULL && width > 0 && height > 0)
        {
          *width_return = width;
          *height_return = height;
          *pixmap_return = pixmap;
          return(1);
        }
    }
  return(0);
}

/**IF*********************   Internal Function   **************************

    int EZ_CreateXPixmapFromRawRGB(data, width, height, pixmap_return)

Purpose: Create an Xpixmap from raw rgb data.

Returns: 1 if successful, 0 if not.

Author : Arno Dabekaussen, 04-05-97 Maastricht, NL

***/

int EZ_CreateXPixmapFromRawRGB(data, width, height, pixmap_return)
     unsigned char     *data;
     int      width, height;
     Pixmap   *pixmap_return;     
{
    Pixmap    pixmap;


    /*-------------------------------------------------------------
     * Image is in rgbdata, now dither and render it into a pixmap
     *------------------------------------------------------------*/

    EZ_RawRGBData2Pixmap(data, width, height, &pixmap);

 
    if (pixmap != (Pixmap ) NULL)
      {
	*pixmap_return = pixmap;
	return(1);  
      }
    else
      {
	return(0); 
      }

}
/*********************************************************************
 *
 *  Given a RGB array repsenting an image of width X height,
 *  create a pixmap form of the image using the current color setup.
 */
void  EZ_RawRGBData2PixmapWork(sdata, width,height,pixmap_return,ezim)
     unsigned char *sdata;          /* raw RGB data, one byte for each primary component */
     int           width, height;   /* dimension.  data is of length width * height * 3  */
     Pixmap        *pixmap_return;  /* place to return the created pixmap                */
     EZ_XImage     *ezim;
{
  XImage *image;
  int    bytes_per_line, i,j, shm;
  Pixmap pixmap;
  unsigned char *data = (unsigned char *)sdata;
  
  if(ezim == NULL) { *pixmap_return = None; return;}
  image = ezim->ximage;
  shm = ezim->shm;

  /*----------------------------------------------
   * 4/28/99. Move createXImage to caller
   *---------------------------------------------*/
  bytes_per_line = image->bytes_per_line;
  
  /*-------------------------------------------------------
   * Now render the image
   *-----------------------------------------------------*/
  if(EZ_Depth <= 8)
    {
      {
	/*------------------------------------------------
	 * We are on a 8bit display, which means we cannot
	 * allocate enough exact colors most of the time. 
	 * So we dither the image. 
	 * We use the Floyd-Steinberg error diffusion 
	 * algorithm, it produces images better than our
	 * 3D canvas dithering algorithm.
	 *-----------------------------------------------*/
	unsigned char *pixel, *dptr;
	char          *row = image->data;
	int            *errorstorage, *err, *rerr1, *rerr2, *gerr1,*gerr2,*berr1,*berr2;
	int            r,g,b, ir,ig,ib, index, rerror,gerror,berror,hstep;
	int            r1,g1,b1,mmaaxx;
	XColor         *xc;

	errorstorage = (int *)my_malloc( 6 * (width + 6 ) * sizeof(int), _DITHER_TMP_D_);
	if(errorstorage == (int *)NULL)
	  {
	    *pixmap_return = (Pixmap )NULL;
	    EZ_OutOfMemory("EZ_RawRGBData2PixmapWork");
	    return;
	  }
	err = errorstorage;                /* initialize error terms    */
	memset(err, 0, 6*(width+3)*sizeof(int));

	/*-------------------------------------------
	 * storage for errors for R G B components.
	 *-------------------------------------------*/
	i = width + 3;
	rerr1 = errorstorage + 2;         /* current row  */
	rerr2 = errorstorage + i + 2;     /* next row     */
	gerr1 = errorstorage + 2*i + 2;
	gerr2 = errorstorage + 3*i + 2;
	berr1 = errorstorage + 4*i + 2;
	berr2 = errorstorage + 5*i + 2;

	/*-----------------------------------------
	 * Create the dithering map. We need a way
	 * to check when we have to do this !!
	 *----------------------------------------*/
	EZ_CreateDitheringMap();
	mmaaxx = DitherIntensityLevels[255]; 

	/*-----------------------------------------
	 * Half step between consecutive levels in
	 * the dithering map.
	 *----------------------------------------*/
	if(EZ_TotalNumOfColors > 232)        hstep = 25;
	else if(EZ_TotalNumOfColors > 141)   hstep = 31;
	else if(EZ_TotalNumOfColors > 80)    hstep = 42;
	else if(EZ_TotalNumOfColors > 43)    hstep = 63;
	else                                 hstep = 126; 
	/*-------------------------------------------
	 * Now dither the image.
	 *-------------------------------------------*/
	for(i = 0; i <= height; i++) 
	  {
	    if(i == height) 
	      {
		data = (unsigned char *)sdata;
		row = (unsigned char *)(image->data);
		pixel = (unsigned char *)row;
	      }
	    else pixel = (unsigned char *)row;
	    dptr = data;
	    for(j = 0; j <= width; j++)
	      {
		if(j < width) { r1 = (int)(*data++); g1 = (int)(*data++); b1 = (int)(*data++);}
		else { r1 = (int)(*dptr++);  g1 = (int)(*dptr++);  b1 = (int)(*dptr++);}

		r = r1 + rerr1[j]/16; g = g1 + gerr1[j]/16;  b = b1 + berr1[j]/16;
		
		r = (r < 0? 0: (r > 255 ? 255: r));
		g = (g < 0? 0: (g > 255 ? 255: g));
		b = (b < 0? 0: (b > 255 ? 255: b));

		ir = DitherIntensityLevels[r]; 
		ig = DitherIntensityLevels[g];
		ib = DitherIntensityLevels[b];
		if(r - ir > hstep && ir < mmaaxx) ir++;
		if(g - ig > hstep && ig < mmaaxx) ig++;
		if(b - ib > hstep && ib < mmaaxx) ib++;

		/*----------------------------------------------
		 * Approximate color for this pixel 
		 *---------------------------------------------*/
		index = 16 + ir + DitherIndexTable[ig + DitherIndexTable[ib]];
		
		if(j < width) *pixel++ = (unsigned char) (EZ_ColorArray[index]);
		else row[0] = (unsigned char) (EZ_ColorArray[index]);
		
		/*-----------
		 * errors
		 *-----------*/
		xc = &(EZ_XColorArray[index]);
		rerror = r - (int)((xc->red) >> 8);
		gerror = g - (int)((xc->green)>> 8);
		berror = b - (int)((xc->blue) >> 8);
		
		/*-----------------------------------------
		 * done with this pixel, zero the errors 
		 *----------------------------------------*/
		rerr1[j] = 0; 	gerr1[j] = 0; 	berr1[j] = 0;

		/*-----------------------------
		 * diffuse the current errors 
		 *----------------------------*/
		rerr1[j+1] += 7 * rerror;
		rerr2[j] += 5 * rerror;
		rerr2[j+1] += rerror;
		gerr1[j+1] += 7 * gerror; 
		gerr2[j] += 5 * gerror;
		gerr2[j+1] += gerror;
		berr1[j+1] += 7 * berror;
		berr2[j] += 5 * berror;
		berr2[j+1] += berror;
		if(j != 0)
		  {
		    rerr2[j-1] += 3 * rerror;
		    gerr2[j-1] += 3 * gerror; 
		    berr2[j-1] += 3 * berror;
		  }
		else
		  {
		    rerr2[width] += 3 * rerror;
		    gerr2[width] += 3 * gerror; 
		    berr2[width] += 3 * berror;
		  }
	      }
	    row += bytes_per_line;  /* advance to the next row */
	    /*-----------------------------------
	     * swamp the error pointers
	     *----------------------------------*/
	    err = rerr1; rerr1 = rerr2; rerr2 = err;
	    err = gerr1; gerr1 = gerr2; gerr2 = err;
	    err = berr1; berr1 = berr2; berr2 = err; 
	  }
	/*-------------------------------------------------------
	 * finish dithering, create an image and copy the image
	 * into the pixmap
	 *------------------------------------------------------*/
	pixmap = XCreatePixmap(EZ_Display,
			       EZ_DummyWindow,
			       width,
			       height,
			       EZ_Depth);
	if(pixmap != (Pixmap )NULL)
          {
#if defined(HAVE_XSHM) && defined(USE_XSHM)
            if(shm)
              XShmPutImage(EZ_Display,pixmap, EZ_WRITABLEGC,image,0,0,0,0,width,height,False);
            else
#endif
              XPutImage(EZ_Display,pixmap, EZ_WRITABLEGC,image,0,0,0,0,width,height);
          }
	else
	  (void)fprintf(stderr, "Out of memory\n");

	(void)my_free( (char *)errorstorage);
	*pixmap_return = pixmap;
	return;
      }
    }
  else if(EZ_Depth <= 16)
    {
      {
	/*--------------------------------------
	 * we are on a 16 bit TrueColor visual 
	 *--------------------------------------*/
	char           *row;
	unsigned short *pixel;
	int             r,g,b;

        row = image->data;
        for(i = 0; i < height; i++)
          {
            pixel = (unsigned short *)row;
            for(j = 0; j < width; j++)
              {
                r = (int)(*data++);
                g = (int)(*data++);
                b = (int)(*data++);
                *pixel++ = (unsigned short)RGB2PixelLookUp_Fix(r, g, b);
              }
            row += bytes_per_line;
          }
	pixmap = XCreatePixmap(EZ_Display,
			       EZ_DummyWindow,
			       width,
			       height,
			       EZ_Depth);

	if(pixmap != (Pixmap )NULL)
          {
#if defined(HAVE_XSHM) && defined(USE_XSHM)
            if(shm)
              XShmPutImage(EZ_Display,pixmap, EZ_WRITABLEGC,image,0,0,0,0,width,height,False);
            else
#endif
              XPutImage(EZ_Display,pixmap, EZ_WRITABLEGC,image,0,0,0,0,width,height);
          }
	else
	  (void)fprintf(stderr, "Out of memory\n");

	*pixmap_return = pixmap;
	return;
      }
    }
  else if(EZ_Depth >= 24 && EZ_BitsPerPixel == 32)
    {
      {
	/*--------------------------------------
	 * we are on a 24 bit TrueColor visual 
	 *--------------------------------------*/
	char           *row;
	unsigned int   *pixel;
	int             r,g,b;

	row = image->data;
	for(i = 0; i < height; i++)
	  {
	    pixel = (unsigned int *)row;
	    for(j = 0; j < width; j++)
	      {
		r = (int)(*data++);
		g = (int)(*data++);
		b = (int)(*data++);
		*pixel++ = (unsigned int)RGB2PixelLookUp_Fix(r, g, b);
	      }
	    row += bytes_per_line;
	  }
	pixmap = XCreatePixmap(EZ_Display,
			       EZ_DummyWindow,
			       width,
			       height,
			       EZ_Depth);
	if(pixmap != (Pixmap )NULL)
          {
#if defined(HAVE_XSHM) && defined(USE_XSHM)
            if(shm)
              XShmPutImage(EZ_Display,pixmap, EZ_WRITABLEGC,image,0,0,0,0,width,height,False);
            else
#endif
              XPutImage(EZ_Display,pixmap, EZ_WRITABLEGC,image,0,0,0,0,width,height);
          }
	else
	  (void)fprintf(stderr, "Out of memory\n");

	*pixmap_return = pixmap;
	return;
      }
    }
  else if(EZ_Depth >= 24 && EZ_BitsPerPixel == 24)
    {
      {
	/*--------------------------------------
	 * we are on a 24 bit TrueColor visual 
	 *--------------------------------------*/
	unsigned char   *row, *ddd;
	unsigned char   *pixel;
	int             r,g,b, bpl;
	unsigned long   pv;
	unsigned char   *pv4 = (unsigned char *)&pv;

	ddd = (unsigned char *)(image->data);
	bpl = bytes_per_line;
	for(row = ddd, i = 0; i < height; i++, row += bpl)
	  {
	    for(pixel=row, j = 0; j < width; j++)
	      {
		r = (int)(*data++);
		g = (int)(*data++);
		b = (int)(*data++);
		pv = RGB2PixelLookUp_Fix(r, g, b);
		*pixel++ = pv4[0];
		*pixel++ = pv4[1];
		*pixel++ = pv4[2];
	      }
	  }
	pixmap = XCreatePixmap(EZ_Display,
			       EZ_DummyWindow,
			       width,
			       height,
			       EZ_Depth);
	if(pixmap != (Pixmap )NULL)
          {
#if defined(HAVE_XSHM) && defined(USE_XSHM)
            if(shm)
              XShmPutImage(EZ_Display,pixmap, EZ_WRITABLEGC,image,0,0,0,0,width,height,False);
            else
#endif

              XPutImage(EZ_Display,pixmap, EZ_WRITABLEGC,image,0,0,0,0,width,height);
          }
	else
	  (void)fprintf(stderr, "Out of memory\n");

	*pixmap_return = pixmap;
	return;
      }
    }
  else if(EZ_Depth >= 8)
    {
      {
	unsigned long  pixel;
	int             r,g,b;

	for(i = 0; i < height; i++)
	  {
	    for(j = 0; j < width; j++)
	      {
		r = (int)(*data++);
		g = (int)(*data++);
		b = (int)(*data++);
		pixel = (unsigned int)RGB2PixelLookUp(r, g, b);
		XPutPixel(image, j, i, pixel);
	      }
	  }
	pixmap = XCreatePixmap(EZ_Display,
			       EZ_DummyWindow,
			       width,
			       height,
                               EZ_Depth);
	if(pixmap != (Pixmap )NULL)
          {
#if defined(HAVE_XSHM) && defined(USE_XSHM)
            if(shm)
              XShmPutImage(EZ_Display,pixmap, EZ_WRITABLEGC,image,0,0,0,0,width,height,False);
            else
#endif
              XPutImage(EZ_Display,pixmap, EZ_WRITABLEGC,image,0,0,0,0,width,height);
          }
	else
	  (void)fprintf(stderr, "Out of memory\n");
	*pixmap_return = pixmap;
	return;
      }
    }
  else
    {
      (void)fprintf(stderr, "Unknown visual depth.\n");
      *pixmap_return = (Pixmap )NULL;
      return;
    }
}

/*********************************************************************/
static Pixmap *DarkPixmap, *BrightPixmap, *HighlightPixmap;
void  EZ_SetDarkAndBrightPixmapReturns(dk,bt, ht) 
     Pixmap *dk, *bt, *ht;
{ DarkPixmap = dk; BrightPixmap = bt; HighlightPixmap = ht;}
/********************************************************************/
void  EZ_RawRGBData2Pixmap(sdata, width,height,pixmap_return)
     unsigned char *sdata;          /* raw RGB data, one byte for each primary component */
     int           width, height;   /* dimension.  data is of length width * height * 3  */
     Pixmap        *pixmap_return;  /* place to return the created pixmap                */
{
  EZ_XImage *ezXImage = EZ_CreateXImage(width, height, 0);
  EZ_RawRGBData2PixmapWork(sdata, width,height,pixmap_return, ezXImage);
  if(ezXImage && (DarkPixmap || BrightPixmap ))
    {
      unsigned char *tdata;
      int i, tlen = 3 * height * width;
      unsigned char dki[260], bti[260], hti[260];
      
      if(DarkPixmap) *DarkPixmap = None;
      if(BrightPixmap) *BrightPixmap = None;
      if( (tdata = my_malloc(tlen *sizeof(unsigned char),_MISC_DATA_)) == NULL)
	{
	  EZ_OutOfMemory("EZ_RawRGBData2Pixmap");
	  return;       
	}
      for(i = 0; i <= 256; i++)
        {
          unsigned int tmp;
          tmp = (unsigned int)(i * 7 / 10);
          dki[i] = (unsigned char) tmp;
          tmp = (unsigned int)(i * 13 / 10);
          bti[i] = (unsigned char)(tmp >= 255? 255 : tmp);
          tmp = (unsigned int)(i * 115 / 100);
          hti[i] = (unsigned char)(tmp >= 255? 255 : tmp);
        }
      if(DarkPixmap)
	{
	  for(i = 0; i < tlen; i++)  tdata[i] = dki[sdata[i]];
	  EZ_RawRGBData2PixmapWork(tdata, width,height,DarkPixmap,ezXImage);
	}
      if(BrightPixmap)
	{
	  for(i = 0; i < tlen; i++)  tdata[i] = bti[sdata[i]];
	  EZ_RawRGBData2PixmapWork(tdata, width,height,BrightPixmap, ezXImage);
	}
      if(HighlightPixmap)
	{
	  for(i = 0; i < tlen; i++)  tdata[i] = hti[sdata[i]];
	  EZ_RawRGBData2PixmapWork(tdata, width,height,HighlightPixmap, ezXImage);
	}
      (void)my_free(tdata);
    }
  if(ezXImage) EZ_DestroyXImage(ezXImage);
}
/********************************************************************/
void  EZ_PixelValueData2Pixmap(data, width, height, pixmap_return)
     unsigned int *data;           /* pixel values */
     int           width, height;   /* dimension.  */
     Pixmap        *pixmap_return;  /* place to return the created pixmap */
{
  EZ_XImage *ezXImage;
  XImage *image;
  int    bytes_per_line, i,j, k;
  Pixmap pixmap;

  ezXImage = EZ_CreateXImage(width, height, 0);
  if(ezXImage == NULL) { *pixmap_return = None; return;}
  image = ezXImage->ximage;
  /*----------------------------------------------
   * 4/28/99. CreateXImage code removed.
   *---------------------------------------------*/
  bytes_per_line = image->bytes_per_line;
  
  /* write to image */
  k = 0;
  for(i = 0; i < height; i++)
    {
      for(j = 0; j < width; j++)
	XPutPixel(image, j, i, (unsigned long) (data[k +j]));
      k += width;
    }
  /* write to pixmap */
  pixmap = XCreatePixmap(EZ_Display,
			 EZ_DummyWindow,
			 width,
			 height,
			 EZ_Depth);
  if(pixmap != (Pixmap )NULL)
    {
#if defined(HAVE_XSHM) && defined(USE_XSHM)
      if(ezXImage->shm)
        XShmPutImage(EZ_Display,pixmap, EZ_WRITABLEGC,image,0,0,0,0,width,height,False);
      else
#endif
        XPutImage(EZ_Display,pixmap, EZ_WRITABLEGC,image,0,0,0,0,width,height);
    }
  else
    (void)fprintf(stderr, "Out of memory\n");

  EZ_DestroyXImage(ezXImage);
  *pixmap_return = pixmap;
  return;
}
/********************************************************************
 *
 *  Create a pixmap from a given image.
 *
 *   Return 1 if successful, 0 if not.
 *
 ********************************************************************/

int EZ_CreateXPixmapFromImageFile(file, width_return, height_return, pixmap_return)
     char     *file;
     unsigned int      *width_return, *height_return;
     Pixmap   *pixmap_return;     
{
  int     status, fmt = -1;
  Pixmap  pixmap;
  int     width, height;

  pixmap = (Pixmap )NULL;
  width = 0;
  height = 0;

  fmt = EZ_GetImageFileType(file);
  if(fmt == -1)
    {
      (void)fprintf(stderr, "Unknown image: %s\n", file?file:"");
      return(0);
    }

  status = PixmapReaders[fmt](file,&width,&height,&pixmap);

  if(status <= 0)
    {
      (void)fprintf(stderr,"Warning: Error reading image file '%s'\n",file);
      return(0);
    }
  /*---------------------------------------------------------
   *  Set the return values
   *--------------------------------------------------------*/
  if(pixmap != (Pixmap )NULL && width > 0 && height > 0)
    {
      *width_return = width;
      *height_return = height;
      *pixmap_return = pixmap;
      return(1);
    }
  return(0);
}
/**********************************************************************************************/

int EZ_ReadImageFile(file, width_return, height_return, rgb_return)
     char     *file;
     int      *width_return, *height_return;
     unsigned char     **rgb_return;
{
  int     status, fmt = -1;
  int     width, height;

  *rgb_return = NULL;
  width = 0;
  height = 0;

  fmt = EZ_GetImageFileType(file);
  if(fmt == -1)
    {
      (void)fprintf(stderr, "Unknown image: %s\n", file?file:"");
      return(0);
    }
  status = ImageReaders[fmt](file,&width,&height,rgb_return);
  if(status <= 0)
    {
      (void)fprintf(stderr,"Warning: Error reading image file '%s'\n",file);
      return(0);
    }
  /*---------------------------------------------------------
   *  Set the return values
   *--------------------------------------------------------*/
  if(*rgb_return && width > 0 && height > 0)
    {
      *width_return = width;
      *height_return = height;
      return(1);
    }
  return(0);
}
/**********************************************************************************************/
EZ_XImage *EZ_CreateXImage(width, height, shmflag)
     int width, height, shmflag;
{
  EZ_XImage *ezximage = (EZ_XImage *)my_malloc(sizeof(EZ_XImage),_MISC_DATA_);
  if(ezximage)
    {
      memset(ezximage, 0, sizeof(EZ_XImage));
#if defined(HAVE_XSHM) && defined(USE_XSHM)
      if(shmflag)
        {
          XShmSegmentInfo   shminf; 
          ezximage->ximage = EZ_AllocateSHMXImage(width, height, &shminf);
          if(ezximage->ximage != NULL)
            {
              ezximage->shm = 1;
              ezximage->shminfo = (XShmSegmentInfo*)my_malloc(sizeof(XShmSegmentInfo),_MISC_DATA_);
              memcpy(ezximage->shminfo, &shminf, sizeof(XShmSegmentInfo));
            }
        }
#endif
      if(ezximage->ximage == NULL)
        {
          int bytes_per_line;
          ezximage->ximage = XCreateImage(EZ_Display,
                                          EZ_Visual,
                                          EZ_Depth,
                                          ZPixmap,
                                          0,
                                          (char *)NULL,
                                          width,
                                          height,
                                          (EZ_Depth > 16)? 32 : ((EZ_Depth > 8)? 16 : 8),
                                          0);
          if(ezximage->ximage == (XImage *)NULL)
            {
              my_free((void *)ezximage);
              EZ_OutOfMemory("EZ_CreateXImage");
              return(NULL);
            }
          bytes_per_line =  ezximage->ximage->bytes_per_line;
          ezximage->ximage->data = (char *)my_malloc(bytes_per_line * height, _PPM_TMP_IMAGE_D_);
          if( ezximage->ximage->data == (char *)NULL)
            {
              XDestroyImage(ezximage->ximage);
              my_free((void *)ezximage);
              EZ_OutOfMemory("EZ_CreateXImage");
              return(NULL);
            }
        }
    }
  return(ezximage);
}

void EZ_DestroyXImage(ezximage)
     EZ_XImage *ezximage;
{
  if(ezximage)
    {
#if defined(HAVE_XSHM) && defined(USE_XSHM)
      if(ezximage->shm)
        {
          XShmSegmentInfo   *shminf = (XShmSegmentInfo *)ezximage->shminfo;
          XShmDetach(EZ_Display, (XShmSegmentInfo *)ezximage->shminfo);
          XDestroyImage(ezximage->ximage);
          ezximage->ximage = NULL;
          shmdt(shminf->shmaddr );
          shmctl(shminf->shmid, IPC_RMID, 0); 
          my_free((void *)ezximage->shminfo);
        }
#endif
      if(ezximage->ximage)
        {
          (void)my_free( (char *)(ezximage->ximage->data));
          ezximage->ximage->data = NULL;
          XDestroyImage(ezximage->ximage);
        }
      my_free((void *)ezximage);
    }
}
/**********************************************************************************************/
#undef _EZ_WIDGET_IMAGE_C_

