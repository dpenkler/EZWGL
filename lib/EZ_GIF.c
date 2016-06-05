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
 *  This file is modified from gig2ppm.c of the ppmplus package
 */
/* +-------------------------------------------------------------------+ */
/* | Copyright 1990, David Koblas.                                     | */
/* |   Permission to use, copy, modify, and distribute this software   | */
/* |   and its documentation for any purpose and without fee is hereby | */
/* |   granted, provided that the above copyright notice appear in all | */
/* |   copies and that both that copyright notice and this permission  | */
/* |   notice appear in supporting documentation.  This software is    | */
/* |   provided "as is" without express or implied warranty.           | */
/* +-------------------------------------------------------------------+ */

#define _EZ_GIF_C_

#include "EZ_Widget.h"

/*
 *  Functions implemented in this file
 */
int EZ_ReadGIFToPixmap MY_ANSIARGS((
				    char     *file,
				    int      *width_return,
				    int       *height_return,
				    Pixmap   *pixmap_return));
int EZ_ReadGIFToRGB MY_ANSIARGS((
				    char     *file,
				    int      *width_return,
				    int       *height_return,
				    unsigned char   **rgb_return));

/**************************************************************************/
#define	MAXCOLORMAPSIZE		256
#define	TRUE	                1
#define	FALSE	                0
#define CM_RED		        0
#define CM_GREEN	        1
#define CM_BLUE		        2
#define	MAX_LWZ_BITS		12
#define INTERLACE		0x40
#define LOCALCOLORMAP           0x80
#define BitSet(byte, bit)	(((byte) & (bit)) == (bit))
#define	ReadOK(file,buffer,len)	(fread(buffer, len, 1, file) != 0)
#define LM_to_uint(a,b)		(((b)<<8)|(a))

struct {
	unsigned int	Width;
	unsigned int	Height;
	unsigned char	ColorMap[3][MAXCOLORMAPSIZE];
	unsigned int	BitPixel;
	unsigned int	ColorResolution;
	unsigned int	Background;
	unsigned int	AspectRatio;
} GifScreen;

struct {
	int	transparent;
	int	delayTime;
	int	inputFlag;
	int	disposal;
} Gif89 = { -1, -1, -1, 0 };


static int ReadColorMap MY_ANSIARGS((
				     FILE *fd,
				     int  number,
				     unsigned char buffer[3][MAXCOLORMAPSIZE]));
static int DoExtension  MY_ANSIARGS((
				     FILE *fd,
				     int  number));
				     
static int GetDataBlock MY_ANSIARGS((
				     FILE		*fd,
				     unsigned char 	*buf));
static int GetCode MY_ANSIARGS((
				FILE	*fd,
				int	code_size,
				int	flag));
static int LWZReadByte MY_ANSIARGS((
				    FILE    *fd,
				    int flag,
				    int input_code_size));
static int ReadImage MY_ANSIARGS((
				  FILE  	   *fd,
				  unsigned char *image,
				  int	   width, int height,
				  unsigned char cmap[3][MAXCOLORMAPSIZE],
				  int	   interlace));

static int ReadGIF MY_ANSIARGS((
				FILE	*fd,
				int	imageNumber,
				int      *width_return, int *height_return,
				unsigned char **rgb_return));
/*************************************************************************
 *
 *   Read a GIF image into an pixmap.
 */
int EZ_ReadGIFToPixmap(file, width_return, height_return, pixmap_return)
     char     *file;                          /* image file_name         */
     int      *width_return, *height_return;  /* width and height return */
     Pixmap   *pixmap_return;                 /* pixmap return           */
{
  FILE   *fp, *fopen();
  int     result;
  unsigned char *rgb_data = NULL;

  if(file == NULL) return(0);
  if( (fp = fopen(file,"r")) == (FILE *)NULL)
    {
      (void) fprintf(stderr, "Cannot open image file %s\n", file);
      return(0);
    }  
  result = ReadGIF(fp, 1, width_return, height_return, &rgb_data);  
  if(result && rgb_data)
    {
      EZ_RawRGBData2Pixmap(rgb_data, *width_return, *height_return, pixmap_return);
      my_free(rgb_data);
    }
  fclose(fp);
  return(result);
}
/*************************************************************/
int EZ_ReadGIFToRGB(file, width_return, height_return, rgb_return)
     char     *file;                          /* image file_name         */
     int      *width_return, *height_return;  /* width and height return */
     unsigned char  **rgb_return;                 /* pixmap return           */
{
  FILE   *fp, *fopen();
  int     result;

  if(file == NULL) return(0);
  if( (fp = fopen(file,"r")) == (FILE *)NULL)
    {
      (void) fprintf(stderr, "Cannot open image file %s\n", file);
      return(0);
    }  
  *rgb_return = NULL;
  result = ReadGIF(fp, 1, width_return, height_return, rgb_return);  
  fclose(fp);
  return(result);
}
/*************************************************************/
static int ReadGIF(fd,imageNumber, width_return, height_return, rgb_return)
     FILE	*fd;
     int	imageNumber;
     int      *width_return, *height_return;  /* width and height return */
     unsigned char  **rgb_return;
{
  unsigned char	buf[16];
  unsigned char	c;
  unsigned char	localColorMap[3][MAXCOLORMAPSIZE];
  int		useGlobalColormap;
  int		bitPixel;
  char		version[4];
  unsigned char *dataptr;
  int           result = 0;
  
  if (! ReadOK(fd,buf,6))
    { fprintf(stderr,"error reading magic number" ); return (0); }

  if (strncmp(buf,"GIF",3) != 0)
    { fprintf(stderr,"not a GIF file" ); return (0); }

  strncpy(version, buf + 3, 3);
  version[3] = '\0';

  if ((strcmp(version, "87a") != 0) && (strcmp(version, "89a") != 0))
    { fprintf(stderr,"bad version number, not '87a' or '89a'" ); return (0); }
  
  if (! ReadOK(fd,buf,7))
    { fprintf(stderr,"failed to read screen descriptor" ); return (0); }
  
  GifScreen.Width           = LM_to_uint(buf[0],buf[1]);
  GifScreen.Height          = LM_to_uint(buf[2],buf[3]);
  GifScreen.BitPixel        = 2<<(buf[4]&0x07);
  GifScreen.ColorResolution = (((buf[4]&0x70)>>3)+1);
  GifScreen.Background      = buf[5];
  GifScreen.AspectRatio     = buf[6];

  *width_return = GifScreen.Width ;
  *height_return = GifScreen.Height ;

  if (BitSet(buf[4], LOCALCOLORMAP)) {	/* Global Colormap */
    if (ReadColorMap(fd,GifScreen.BitPixel,GifScreen.ColorMap))
      { fprintf(stderr,"error reading global colormap" ); return (0); }
  }

  if((dataptr = (unsigned char *)my_malloc( 4 * GifScreen.Width * GifScreen.Height *
				    sizeof(unsigned char), _GIF_IMAGE_)) == NULL)
    EZ_OutOfMemory("ReadGIF");      
  
  
  for (;;) {
    if (! ReadOK(fd,&c,1))
      { fprintf(stderr,"EOF / read error on image data" ); break; }
    if (c == ';') 
      break;    /* GIF terminator */
    
    if (c == '!') { 	/* Extension */
      if (! ReadOK(fd,&c,1))
	{ fprintf(stderr,"OF / read error on extention function code"); break; }
      DoExtension(fd, c);
      continue;
    }

    if (c != ',') {		/* Not a valid start character */
      continue;
    }
  
    if (! ReadOK(fd,buf,9))
      { fprintf(stderr,"couldn't read left/top/width/height"); break; }
    useGlobalColormap = ! BitSet(buf[8], LOCALCOLORMAP);

    bitPixel = 1<<((buf[8]&0x07)+1);
    /* *width_return = LM_to_uint(buf[4],buf[5]);
     *height_return = LM_to_uint(buf[6],buf[7]);
     */


    if(!useGlobalColormap) 
      {
        if(ReadColorMap(fd, bitPixel, localColorMap))
          { fprintf(stderr,"error reading local colormap"); break; }
        result = ReadImage(fd,dataptr, *width_return, *height_return,
                           localColorMap, BitSet(buf[8], INTERLACE));
      }
    else
      result = ReadImage(fd, dataptr,  *width_return, *height_return,
                         GifScreen.ColorMap, BitSet(buf[8], INTERLACE));
    /*
	 *  Now render dataptr into a pixmap.
	 */
    if(result)
      {
        *rgb_return = dataptr;
        break;
      }
  }
  if(result == 0) { (void)my_free((char *)dataptr);}
  return(result);
}
/*************************************************************/
static int ReadColorMap(fd,number,buffer)
     FILE *fd;
     int  number;
     unsigned char buffer[3][MAXCOLORMAPSIZE];
{
  int		i;
  unsigned char	rgb[3];

  for (i = 0; i < number; ++i) {
    if (! ReadOK(fd, rgb, sizeof(rgb)))
      { fprintf(stderr,"bad colormap"); return (1); }
    buffer[CM_RED][i] = rgb[0] ;
    buffer[CM_GREEN][i] = rgb[1] ;
    buffer[CM_BLUE][i] = rgb[2] ;
  }
  return 0;
}
/*************************************************************/
static int DoExtension(fd, label)
     FILE	*fd;
     int	label;
{
  static char	buf[256];

  switch (label) {
  case 0x01:		/* Plain Text Extension */
    break;
  case 0xff:		/* Application Extension */
    break;
  case 0xfe:		/* Comment Extension */
    while (GetDataBlock(fd, (unsigned char*) buf) != 0) ;
    return FALSE;
  case 0xf9:		/* Graphic Control Extension */
    (void) GetDataBlock(fd, (unsigned char*) buf);
    Gif89.disposal    = (buf[0] >> 2) & 0x7;
    Gif89.inputFlag   = (buf[0] >> 1) & 0x1;
    Gif89.delayTime   = LM_to_uint(buf[1],buf[2]);
    if ((buf[0] & 0x1) != 0)
      Gif89.transparent = buf[3];
    
    while (GetDataBlock(fd, (unsigned char*) buf) != 0)
      ;
    return FALSE;
  default:
    break;
  }
  while (GetDataBlock(fd, (unsigned char*) buf) != 0) ;
  return FALSE;
}
/*************************************************************/
int	ZeroDataBlock = FALSE;
static int GetDataBlock(fd, buf)
     FILE		*fd;
     unsigned char 	*buf;
{
  unsigned char	count;

  if (! ReadOK(fd,&count,1)) {
    return -1;
  }

  ZeroDataBlock = count == 0;

  if ((count != 0) && (! ReadOK(fd, buf, count))) {
    return -1;
  }
  return count;
}
/*************************************************************/
static int GetCode(fd, code_size, flag)
     FILE	*fd;
     int	code_size;
     int	flag;
{
  static unsigned char	buf[280];
  static int		curbit, lastbit, done, last_byte;
  int			i, j, ret;
  unsigned char		count;

  if (flag) {
    curbit = 0;
    lastbit = 0;
    done = FALSE;
    return 0;
  }

  if ( (curbit+code_size) >= lastbit) {
    if (done) {
      if (curbit >= lastbit)
	return -1;
    }
    buf[0] = buf[last_byte-2];
    buf[1] = buf[last_byte-1];

    if ((count = GetDataBlock(fd, &buf[2])) == 0)
      done = TRUE;

    last_byte = 2 + count;
    curbit = (curbit - lastbit) + 16;
    lastbit = (2+count)*8 ;
  }

  ret = 0;
  for (i = curbit, j = 0; j < code_size; ++i, ++j)
    ret |= ((buf[ i / 8 ] & (1 << (i % 8))) != 0) << j;
  
  curbit += code_size;
  
  return ret;
}
/*************************************************************/
static int LWZReadByte(fd, flag, input_code_size)
     FILE    *fd;
     int flag;
     int input_code_size;
{
  static int  fresh = 0;
  int     code, incode;
  static int  code_size, set_code_size;
  static int  max_code, max_code_size;
  static int  firstcode, oldcode;
  static int  clear_code, end_code;
  static int  table[2][(1<< MAX_LWZ_BITS)];
  static int  stack[(1<<(MAX_LWZ_BITS))*2], *sp;
  register int    i;


  if(flag) 
    {
      set_code_size = input_code_size;
      code_size = set_code_size+1;
      clear_code = 1 << set_code_size ;
      end_code = clear_code + 1;
      max_code_size = 2*clear_code;
      max_code = clear_code+2;
    
      GetCode(fd, 0, 1);

      fresh = 1;
      
      for (i = 0; i < clear_code; ++i) {
	table[0][i] = 0;
	table[1][i] = i;
      }
      for (; i < (1<<MAX_LWZ_BITS); ++i) {
	table[0][i] = table[1][0] = 0;
      }
      
      sp = stack;
      return 0;

  } else if (fresh) {

    fresh = 0;
    do {
      firstcode = oldcode = GetCode(fd, code_size, 0);
    } while (firstcode == clear_code);
    return firstcode;
  }

  if (sp > stack)
    return *--sp;

  while ((code = GetCode(fd, code_size, 0)) >= 0) {
    if (code == clear_code) {
      for (i = 0; i < clear_code; ++i) {
	table[0][i] = 0;
	table[1][i] = i;
      }
      
      for (; i < (1<<MAX_LWZ_BITS); ++i) {
	table[0][i] = table[1][i] = 0;
      }

      code_size = set_code_size+1;
      max_code_size = 2*clear_code;
      max_code = clear_code+2;
      sp = stack;
      firstcode = oldcode = GetCode(fd, code_size, 0);
      return firstcode;

    } else if (code == end_code) {
      int     count;
      unsigned char   buf[260];

      if (ZeroDataBlock)
	return -2;
      
      while ((count = GetDataBlock(fd, buf)) > 0)
	;

      if (count != 0)
	return -2;
    }

    incode = code;

    if (code >= max_code) {
      *sp++ = firstcode;
      code = oldcode;
    }

    while (code >= clear_code) {
      *sp++ = table[1][code];
      if (code == table[0][code])
	fprintf(stderr,"circular table entry BIG ERROR\n");
      code = table[0][code];
    }

    *sp++ = firstcode = table[1][code];

    if ((code = max_code) <(1<<MAX_LWZ_BITS)) {
      table[0][code] = oldcode;
      table[1][code] = firstcode;
      ++max_code;
      if ((max_code>=max_code_size) && (max_code_size < (1<<MAX_LWZ_BITS))) {
	max_code_size *= 2;
	++code_size;
      }
    }
    oldcode = incode;

    if (sp > stack)
      return *--sp;
  }
  return code;
}
/**************************************************************/

static int ReadImage(fd, image, width, height, cmap, interlace)
     FILE  	   *fd;
     unsigned char *image;
     int	   width, height;
     unsigned char cmap[3][MAXCOLORMAPSIZE];
     int	   interlace;
{
  unsigned char	c;	
  int		v;
  int		xpos = 0, ypos = 0, pass = 0;
  
  /*
   **  Initialize the Compression routines
   */
  if (! ReadOK(fd,&c,1))
    {fprintf(stderr,"EOF / read error on image data\n"); return(0);}

  if (LWZReadByte(fd, TRUE, c) < 0)
    {fprintf(stderr,"error reading image\n"); return(0);}
  
  while ((v = LWZReadByte(fd,FALSE,c)) >= 0 ) 
    {
      {
	int xi = xpos *3;
	int yi = ypos *width * 3;
	int pindex =xi + yi;
	image[pindex  ] = cmap[CM_RED][v];
	image[pindex+1] = cmap[CM_GREEN][v];
	image[pindex+2] = cmap[CM_BLUE][v];
      }
      ++xpos;
      if(xpos == width) {
	xpos = 0;
	if (interlace) {
	  switch (pass) {
	  case 0:
	  case 1:
	    ypos += 8; break;
	  case 2:
	    ypos += 4; break;
	  case 3:
	    ypos += 2; break;
	  }
	
	  if (ypos >= height) {
	    ++pass;
	    switch (pass) {
	    case 1:
	      ypos = 4; break;
	    case 2:
	      ypos = 2; break;
	    case 3:
	      ypos = 1; break;
	    default:
	      return(1);
	    }
	  }
	} 
	else 
	  {
	    ++ypos;
	  }
      }
      if(ypos >= height)
	break;
    }
  return(1);
}
/*****************************************************************/

#undef _EZ_GIF_C_
