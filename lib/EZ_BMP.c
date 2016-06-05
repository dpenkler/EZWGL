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
 * Contributed by   Arno Dabekaussen <piemail!arnodab@relay.NL.net>
 */
/*********************************************************************
 ***                                                               ***
 ***           Create a pixmap from .bmp file.                     ***
 ***                                                               ***
 *********************************************************************/
#define _EZ_BMP_C_
#include "EZ_Widget.h"

/*********************************************************************
 * 
 *  Functions implemented in this file:
 */
int EZ_ReadBMPToPixmap MY_ANSIARGS((char *filename, int *width_return, int *height_return,
				    Pixmap *pixmap_return));
int EZ_ReadBMPToRGB    MY_ANSIARGS((char *filename, int *width_return, int *height_return,
				    unsigned char **rgb_return));

/*********************************************************************
 * 
 *  Local Functions
 */
static int EZ_ReadBMP_FP_ToPixmap MY_ANSIARGS((FILE *fp, char *filename, int *width_return, 
					       int *height_return, Pixmap *pixmap_return,
                                               unsigned char **rgb_return));
static long BigEndian MY_ANSIARGS((void));

/*
 * some macros for swapping longs and shorts from
 * little endian to big endian and vice versa
 */

#define SWAP(a, b)      { char temp; \
			  temp = a;  \
			  a = b;     \
			  b = temp;  \
		  }

#define SWAPLONG(val)	{ char *help = (char *)&val;\
			  SWAP(help[3], help[0]);   \
			  SWAP(help[1], help[2]);   \
		        }

#define SWAPSHORT(a) {    char *help = (char *)&a;\
			  SWAP(help[0], help[1]); \
                     }


/*
 * file header structure
 */
struct bmp_file_header
{
	char            bfType[2];	/* file ID must be 'BM'  */
	long            bfSize;		/* filelength in bytes 	 */
	short           reserved;	/* must be set to 0    	 */
	short           reserved2;	/* either 		 */
	long            bfOffs;		/* offset to data area 	 */
};
typedef struct bmp_file_header bmp_file_header_t;

/*
 * look up table entry
 */
struct rgb_quad
{
	char            blue;
	char            green;
	char            red;
	char            reserved;
};
typedef struct rgb_quad rgb_quad_t;

/*
 * brief information of the image file
 */
struct bmp_info_header
{
	long            biSize;	      /* bmp_info_header length      	    */
	long            biWidth;      /* width of bitmap in pixels   	    */
	long            biHeight;     /* height of bitmap in pixels  	    */
	short           biPlanes;     /* number of planes, must be set to 1 */
	short           biBitCnt;     /* bits per pixel		            */
	long            biCompr;      /* type of compression		    */
	long            biSizeIm;     /* size of (compressed) image	    */
	long            biXPelsPerM;  /* X pixels per meter	            */
	long            biYPelsPerM;  /* Y pixels per meter	            */
	long            biClrUsed;    /* no. of used colors	            */
	long            biClrImp;     /* no. of important colors            */
	rgb_quad_t     *lut;	      /* lookup table		            */
};
typedef struct bmp_info_header bmp_info_header_t;

/*
 * Compression types
 */
#define BI_RGB	0
#define BI_RLE8	1
#define BI_RLE4	2

/*
 * header size on disk, not that this isn't necessarily the size of
 * the bmp_info_header structure in memory, due to the fact, that
 * intel chips may store longs at misaligned addresses, where other
 * cpu's like 68k, sparc, PowerPC store longs at addresses divisible
 * by 4 (32 bit addresses)!
 */
#define BMP_HEADER_OFFSET 0x36

/*
 * return codes
 */
#define BMP_NO_ERROR		 1
#define BMP_FILE_OPEN_FAILED	 0
#define BMP_FILE_HEADER_CORRUPT	 0
#define BMP_INFO_HEADER_CORRUPT	 0
#define BMP_LOW_MEMORY		 0	/* malloc failed */
#define BMP_LUT_CORRUPT		 0
#define BMP_IMAGE_DATA_CORRUPT	 0
#define BMP_NO_BMP		 0
#define BMP_COMPRESSED		 0
#define BMP_ERROR                0 

/*************************************************************************
 *
 *   Read a MicroSoft BMP image into an pixmap.
 */

int EZ_ReadBMPToPixmap(filename, width_return, height_return, 
		       pixmap_return)
     char     *filename;                      /* image file_name         */
     int      *width_return, *height_return;  /* width and height return */
     Pixmap   *pixmap_return;                 /* pixmap return           */
{
  FILE   *fp, *fopen();
  int    ok = 0;
  char   *fname = "EZ_ReadBMPToPixmap";
  
  if( (fp = fopen(filename,"rb")) == (FILE *) NULL)
  {
      (void) fprintf(stderr, "Cannot open image file %s\n", filename);
      return(0);
  }

  ok = EZ_ReadBMP_FP_ToPixmap(fp, filename, width_return, height_return, 
			      pixmap_return, NULL);

  if (fclose(fp) != 0) 
  {  
    fprintf(stderr, "%s: can't close %s\n", fname, filename); 
    exit(EZ_EXIT_ERROR);
  }
  return(ok);
}
/***********************************************************************/
int EZ_ReadBMPToRGB(filename, width_return, height_return, rgb_return)
     char     *filename;                      /* image file_name         */
     int      *width_return, *height_return;  /* width and height return */
     unsigned char **rgb_return; 
{
  FILE   *fp, *fopen();
  int    ok = 0;
  char   *fname = "EZ_ReadBMPToPixmap";
  
  if( (fp = fopen(filename,"rb")) == (FILE *) NULL)
  {
      (void) fprintf(stderr, "Cannot open image file %s\n", filename);
      return(0);
  }

  ok = EZ_ReadBMP_FP_ToPixmap(fp, filename, width_return, height_return, 
			      NULL, rgb_return);
  
  if (fclose(fp) != 0) 
    {  
      fprintf(stderr, "%s: can't close %s\n", fname, filename); 
      exit(EZ_EXIT_ERROR);
    }
  return(ok);
}



/**IF*********************   Internal Function   **************************

    static long BigEndian(void)

Return  : long
Purpose : Detects a big or little endian machine.

Notes   : 

***/

static long BigEndian(void)
{
  long            testval = 0x12345678;
  char           *bytep = (char *) &testval;

  return (long) (bytep[0] == (char) 0x12);
}

/**IF*********************   Internal Function   **************************

    static int EZ_ReadBMP_FP_ToPixmap(fp, width_return, height_return, 
				      pixmap_return)

Purpose: Read a MicroSoft BMP image (from a stream descriptor) into an pixmap.

         This funtion reads an image in BMP format and stores it into a 
	 given data area. If the look up table pointer is NULL then 
	 workspace for the look up table will be allocated. 
	 Otherwise, look up table data is written to the specified 
	 address (e.g. the look up table ram of a graphic controller
	 If the data pointer is NULL then th function allocates memory 
	 for the image and after the function returns data contains 
	 the address of the image memory start address
 
Parameter:  char *filename		 the name of the image to be 
                                         loaded 
 	    bmp_info_header_t *imageinfo a pointer to an image info
                                         structure to receive
 					 relevant image data after
                                         loading
 	    char **data			 the address of a pointer to 
                                         the data area where
 					 the image should be loaded to

Returns:    BMP_NO_ERROR		 image was loaded correctly
 	    BMP_FILE_OPEN_FAILED	 if the image file couldn't be opened
            BMP_NO_BMP		         if the file that was opened wasn't 
                                         a BMP file
  	    BMP_COMPRESSED		 if the image file was in compressed 
                                         format (not supported)
 	    BMP_LOW_MEMORY		 if memory allocation for the look up 
                                         table or image memory failed
 	    BMP_LUT_CORRUPT		 if the reading of the look up table 
                                         failed
 	    BMP_IMAGE_DATA_CORRUPT       if the image data couldn't be read 

Notes:	    This version expects a consequtive allocated image memory, or
            allocates a consequtive image memory internally.

History:    23-03-95     If columns = 555 then bytesperline is rounded to 558,
                         and thus overflow.
	    07-11-95	 LUT values need not to be swapped on bigendian
	                 machines, because they are written character based.

***/

static int EZ_ReadBMP_FP_ToPixmap(fp, filename, width_return, height_return, 
				  pixmap_return, rgb_return)
     FILE     *fp;                            /* stream descriptor       */
     char     *filename;
     int      *width_return, *height_return;  /* width and height return */
     Pixmap   *pixmap_return;                 /* pixmap return           */
     unsigned char **rgb_return;
{
  unsigned char         *tmpdata, *rgbdata;
  Pixmap                pixmap = None;
  bmp_info_header_t     imageinfo;
  bmp_file_header_t 	fheader;
  char	       		pad[4] = { (char) 0x00, (char) 0x00, 
				   (char) 0x00, (char) 0x00 };
  int			padbytes=0, c;
  long            	i, j, k, l, m, n;
  long            	lutentries = 0, bytesperline=0;
  long                  bytesperimageline=0, datasize;
  long            	result;
  char           	*linebuffer = NULL;
  char		        *fname = "EZ_ReadBMP_FP_ToPixmap";

  /*
   * read file header, due to the suckin' Intel address misalignment we
   * need to read each structure element sperately
   */
	
  if (fread(fheader.bfType, sizeof(fheader.bfType), 1, fp) != 1)
  {
      fprintf(stderr,"%s: ERROR: reading fheader.bfType in %s\n", 
	      fname, filename);

      return BMP_FILE_HEADER_CORRUPT;
 
  }

  if (fread(&fheader.bfSize, sizeof(fheader.bfSize), 1, fp) != 1)
  {
      fprintf(stderr,"%s: ERROR: reading fheader.bfOffs in %s\n", 
	      fname, filename);

      return BMP_FILE_HEADER_CORRUPT;
 
  }


  if (fread(&fheader.reserved, sizeof(fheader.reserved), 2, fp) != 2)
  {
      fprintf(stderr,"%s: ERROR: reading fheader.bfOffs in %s\n", 
	      fname, filename);

      return BMP_FILE_HEADER_CORRUPT;
 
  }
	
  if (fread(&fheader.bfOffs, sizeof(fheader.bfOffs), 1, fp) != 1)
  {
      fprintf(stderr,"%s: ERROR: reading fheader.bfOffs in %s\n", 
	      fname, filename);

      return BMP_FILE_HEADER_CORRUPT;
 
  }

	
  if (BigEndian())
  {
      SWAPLONG(fheader.bfSize);
      SWAPLONG(fheader.bfOffs);
  }


  if (fheader.bfType[0] != 'B' || fheader.bfType[1] != 'M')
  {
      fprintf(stderr, "%s: Not a BMP file\n", fname);

      return BMP_NO_BMP;
  }

  /*
   * read bmp info header
   */
  if (fread(&imageinfo.biSize, sizeof(imageinfo.biSize), 1, fp) != 1)
  {
      fprintf(stderr,"%s: ERROR: reading imageinfo.biSize "
	      "in %s\n", fname, filename);

      return BMP_INFO_HEADER_CORRUPT;
  }

  if (fread(&imageinfo.biWidth, sizeof(imageinfo.biWidth), 1, fp) != 1)
  {
      fprintf(stderr,"%s: ERROR: reading imageinfo.biWidth "
	      "in %s\n", fname, filename);

      return BMP_INFO_HEADER_CORRUPT;
  }
	
  if (fread(&imageinfo.biHeight, sizeof(imageinfo.biHeight), 1, 
	    fp) != 1)
  {
      fprintf(stderr,"%s: ERROR: reading imageinfo.biHeight "
	      "in %s\n", fname, filename);

      return BMP_INFO_HEADER_CORRUPT;
  }

  if (fread(&imageinfo.biPlanes, sizeof(imageinfo.biPlanes), 1, 
	    fp) != 1)
  {
      fprintf(stderr,"%s: ERROR: reading imageinfo.biPlanes "
	      "in %s\n", fname, filename);

      return BMP_INFO_HEADER_CORRUPT;
  }

  if (fread(&imageinfo.biBitCnt, sizeof(imageinfo.biBitCnt), 1, 
	    fp) != 1)
  {
      fprintf(stderr,"%s: ERROR: reading imageinfo.biBitCnt "
	      "in %s\n", fname, filename);

      return BMP_INFO_HEADER_CORRUPT;
  }

  if (fread(&imageinfo.biCompr, sizeof(imageinfo.biCompr), 1, 
	    fp) != 1)
  {
      fprintf(stderr,"%s: ERROR: reading imageinfo.biCompr "
	      "in %s\n", fname, filename);

      return BMP_INFO_HEADER_CORRUPT;
  }

  if (fread(&imageinfo.biSizeIm, sizeof(imageinfo.biSizeIm), 1, 
	    fp) != 1)
  {
      fprintf(stderr,"%s: ERROR: reading imageinfo.biSizeIm "
	      "in %s\n", fname, filename);

      return BMP_INFO_HEADER_CORRUPT;
  }

  if (fread(&imageinfo.biXPelsPerM, sizeof(imageinfo.biXPelsPerM), 
	    1, fp) != 1)
  {
      fprintf(stderr,"%s: ERROR: reading imageinfo.biXPelsPerM "
	      "in %s\n", fname, filename);

      return BMP_INFO_HEADER_CORRUPT;
  }

  if (fread(&imageinfo.biYPelsPerM, sizeof(imageinfo.biYPelsPerM), 
	    1, fp) != 1)
  {
      fprintf(stderr,"%s: ERROR: reading imageinfo.biYPelsPerM "
	      "in %s\n", fname, filename);

      return BMP_INFO_HEADER_CORRUPT;
  }

  if (fread(&imageinfo.biClrUsed, sizeof(imageinfo.biClrUsed), 1, 
		  fp) != 1)
  {
      fprintf(stderr,"%s: ERROR: reading imageinfo.biClrUsed in %s\n", 
	      fname, filename);

      return BMP_INFO_HEADER_CORRUPT;
  }

  if (fread(&imageinfo.biClrImp, sizeof(imageinfo.biClrImp), 1, 
	    fp) != 1)
  {
      fprintf(stderr,"%s: ERROR: reading imageinfo.biClrImp in %s\n", 
	      fname, filename);

      return BMP_INFO_HEADER_CORRUPT;
  }

  if (BigEndian())
  {
      SWAPLONG(imageinfo.biSize);
      SWAPLONG(imageinfo.biWidth);
      SWAPLONG(imageinfo.biHeight);
      SWAPSHORT(imageinfo.biPlanes);
      SWAPSHORT(imageinfo.biBitCnt);
      SWAPLONG(imageinfo.biCompr);
      SWAPLONG(imageinfo.biSizeIm);
      SWAPLONG(imageinfo.biXPelsPerM);
      SWAPLONG(imageinfo.biYPelsPerM);
      SWAPLONG(imageinfo.biClrUsed);
      SWAPLONG(imageinfo.biClrImp);
  }


  if (imageinfo.biCompr != BI_RGB)
  {
      fprintf(stderr, "Unable to load compressed images, not supported!\n");

      return BMP_COMPRESSED;
  }


  /*
   * generate look up table
   */

  if (imageinfo.biBitCnt != 24)
  {
      lutentries = (imageinfo.biClrUsed) 
	           ? imageinfo.biClrUsed : (1L << imageinfo.biBitCnt);
  }


  if (lutentries > 0)
  {

      if ((imageinfo.lut = malloc((size_t) lutentries 
				  * sizeof(rgb_quad_t))) == NULL)
      {
	  fprintf(stderr, "%s: Unable to allocate look up table memory\n", 
		  fname);

	  return BMP_LOW_MEMORY;
      }

      result = (long) fread(imageinfo.lut, sizeof(rgb_quad_t), 
			    (size_t) lutentries, fp);
      if (result != lutentries)
      {
	  fprintf(stderr, "%s: can't read look up table\n", fname);

	  return BMP_LUT_CORRUPT;
      }
  }

  /*
   * read image data, a line must be divisible by 4.
   *
   * This was buggy because bytesperline can get larger then the actual
   * line length:
   *
   *    bytesperline = (bytesperimageline = imageinfo.biWidth) 
   *       + (imageinfo.biWidth % 4); 
   *
   * A. Dabekaussen, Maastricht, 23-03-95
   *
   */
  switch (imageinfo.biBitCnt)
  {
      case 1:
	  bytesperline = (bytesperimageline = imageinfo.biWidth) / 8; 
	  padbytes = ( ((imageinfo.biWidth / 8) % 4) ?
		       (4 - ((imageinfo.biWidth / 8) % 4)) : 0);
	  break;
      case 4:
	  bytesperline = (bytesperimageline = imageinfo.biWidth) / 2; 
	  padbytes = ( ((imageinfo.biWidth / 2) % 4) ?
		       (4 - ((imageinfo.biWidth / 2) % 4)) : 0);
	  break;
      case 8:
	  bytesperline = (bytesperimageline = imageinfo.biWidth); 
	  padbytes = ((imageinfo.biWidth % 4) ? 
		      (4 - (imageinfo.biWidth % 4)) : 0);
	  break;
      case 24:
	  bytesperline = (bytesperimageline = imageinfo.biWidth) * 3; 
	  padbytes = ( ((imageinfo.biWidth * 3) % 4) ?
		       (4 - ((imageinfo.biWidth * 3) % 4)) : 0);
	  break;
      default:
	  fprintf(stderr,"%s: ERROR: imageinfo.biBitCnt invalid "
		  "in %s\n", fname, filename);

	  return BMP_INFO_HEADER_CORRUPT;
  }

  /*
   * calculate the amount of memory to be reserved for image
   * data 24 bit images need 3 bytes all other images need 
   * bytesperimageline for one line.
   */

  if(imageinfo.biBitCnt == 24)
  {
      datasize = imageinfo.biHeight * 3 * (bytesperimageline + padbytes);
  }
  else
  {
      datasize = imageinfo.biHeight * (bytesperimageline + padbytes);
  }


  if ((tmpdata = malloc((size_t) datasize)) == NULL)
  {
      fprintf(stderr, "%s: Unable to allocate %ld bytes of image memory \n", 
	      fname, datasize);

      return BMP_LOW_MEMORY;
  }


  /*
   * images are stored with the last line first
   */

  tmpdata += (imageinfo.biHeight - 1) * (bytesperline+padbytes);

  /*
   * 24 bit images have to be converted from grb to rgb.
   */

  if (imageinfo.biBitCnt == 24)
  {
      if ((linebuffer = malloc((size_t) bytesperline + padbytes)) == NULL)
      {
	  fprintf(stderr, "%s: can't allocate line buffer memory\n",
		  fname);
	  return BMP_LOW_MEMORY;
      }
  }

  for (i = imageinfo.biHeight; i > 0; i--)
  {

      /*
       * read one line
       */
      if (imageinfo.biBitCnt == 24)
      {
	  result = (long) fread(linebuffer, (size_t) bytesperline, 
				1, fp);

	  if (result != 1)
	  {
	      fprintf(stderr, "%s: Unable to read image data line %ld\n", 
		      fname, i);

	      return BMP_IMAGE_DATA_CORRUPT;
	  }

	  /*
	   * Convert from [bgr] to [rgb].
	   */

	  for (j = 0, k = 0; j < bytesperline; j += 3, k += 3)
	  {
	      tmpdata[j] = linebuffer[k + 2];
	      tmpdata[j + 1] = linebuffer[k + 1];
	      tmpdata[j + 2] = linebuffer[k];
	  }

	  /*
	   * read pad bytes because bitmap format expects lines that
	   * are divisible by 4.
	   */

	  result = (long) fread(pad, (size_t) sizeof(char), 
				(size_t) padbytes, fp);

	  if (result != padbytes)
	  {
	      fprintf(stderr, "%s: Unable to read pad bytes at line %ld\n", 
		      fname, i);

	      return BMP_IMAGE_DATA_CORRUPT;
	  }

	  tmpdata -= bytesperline;

      }
      else
      {
	  result = (long) fread(tmpdata, (size_t) bytesperline + padbytes, 
				1, fp);

	  if (result != 1)
	  {
	      fprintf(stderr, "%s: Unable to read image data line %ld\n", 
		      fname, i);

	      return BMP_IMAGE_DATA_CORRUPT;
	  }

	  tmpdata -= bytesperline + padbytes;

      }
  }


  /*
   * we subtracted one line to much so we restore the pointer here
   */

  if (imageinfo.biBitCnt == 24)
  {
      tmpdata += bytesperline;
  }
  else
  {
      tmpdata += bytesperline + padbytes;
  }


  if (linebuffer != NULL)
  {
      free(linebuffer);
  }

  switch (imageinfo.biBitCnt)
  {
      case 1:

	  /*
	   * Now the data is converted with aid of the color lookup table
	   * to RGB data. Watch out the width is expanded because one byte
	   * of tmpdata contains 8 pixels in rgb image.
	   */

	  if( (rgbdata = (unsigned char *) 
	       my_malloc(3 * imageinfo.biWidth * imageinfo.biHeight *
			 sizeof(unsigned char), _BMPM_TMP_D_)) == 
	      (unsigned char *) NULL)
	  {
	      (void) fprintf(stderr, "%s: out of memory\n", fname);
	      return(BMP_ERROR);  
	  }


	  for (i = m = k = l = n = 0; i < imageinfo.biHeight; i++)
	  {
	      for (j = 0; j < 8*(bytesperline+padbytes); j++, l++)
	      {
		  /*
		   * l counts the bits in output image.
		   * k counts the bits in a byte.
		   * m counts the pixels in the rgb image.
		   * n counts the bytes in the packed array.
		   */

		  k = 7 - (l%8);

		  /*
		   * Get the required bit and look it up in the color
		   * lookup table.
		   */

		  c = (tmpdata[n] & (0x01 << k)) >> k;

		  if (k == 0) n++;
 
		  if (j < imageinfo.biWidth) 
		  {
		      rgbdata[m++] = imageinfo.lut[c].red;
		      rgbdata[m++] = imageinfo.lut[c].green;
		      rgbdata[m++] = imageinfo.lut[c].blue;
		  }

	      }
	  }


	  /*-------------------------------------------------------------
	   * Image is in rgbdata, now dither and render it into a pixmap
	   *------------------------------------------------------------*/
          if(rgb_return )
            {
              *rgb_return = rgbdata;
            }
          else
            {
              EZ_RawRGBData2Pixmap(rgbdata, imageinfo.biWidth, 
                                   imageinfo.biHeight, &pixmap);

              (void) my_free( (unsigned char *)rgbdata);
            }
	  /*
	   * Clean up the memory that isn't used anymore.
	   */
	  (void) my_free( (unsigned char *)tmpdata);


	  if (imageinfo.lut != NULL)
	  {
	      free(imageinfo.lut);
	  }

	  break;

      case 4:

	  /*
	   * Now the data is converted with aid of the color lookup table
	   * to RGB data. Watch out the width is expanded because one byte
	   * of tmpdata contains 2 pixels in rgb image.
	   */

	  if( (rgbdata = (unsigned char *) 
	       my_malloc(3 * imageinfo.biWidth * imageinfo.biHeight *
			 sizeof(unsigned char), _BMPM_TMP_D_)) == 
	      (unsigned char *) NULL)
	  {
	      (void) fprintf(stderr, "%s: out of memory\n", fname);
	      return(BMP_ERROR);  
	  }

	  for (i = m = k = l = n = 0; i < imageinfo.biHeight; i++)
	  {
	      for (j = 0; j < 2*(bytesperline+padbytes); j++, l++)
	      {
		  /*
		   * l counts the bits in output image.
		   * k counts the bits in a byte.
		   * m counts the pixels in the rgb image.
		   * n counts the bytes in the packed array.
		   */

		  k = 4*(l%2);

		  /*
		   * Get the required bit and look it up in the color
		   * lookup table.
		   */

		  c = (tmpdata[n] & (0x0f << k)) >> k;

		  if (k == 0) n++;
 
		  if (j < imageinfo.biWidth) 
		  {
		      rgbdata[m++] = imageinfo.lut[c].red;
		      rgbdata[m++] = imageinfo.lut[c].green;
		      rgbdata[m++] = imageinfo.lut[c].blue;
		  }

	      }
	  }


	  /*-------------------------------------------------------------
	   * Image is in rgbdata, now dither and render it into a pixmap
	   *------------------------------------------------------------*/
          if(rgb_return)
            {
              *rgb_return = rgbdata;
            }
          else
            {
              EZ_RawRGBData2Pixmap(rgbdata, imageinfo.biWidth, 
                                   imageinfo.biHeight, &pixmap);

              /*
               * Clean up the memory that isn't used anymore.
               */
              (void) my_free( (unsigned char *)rgbdata);
            }
	  (void) my_free( (unsigned char *)tmpdata);
	  if (imageinfo.lut != NULL)
	  {
	      free(imageinfo.lut);
	  }
	  break;


      case 8:

	  /*
	   * Now the data is converted with aid of the color lookup table
	   * to RGB data.
	   */

	  if( (rgbdata = (unsigned char *) 
	       my_malloc(3 * imageinfo.biWidth * imageinfo.biHeight *
			 sizeof(unsigned char), _BMPM_TMP_D_)) == 
	      (unsigned char *) NULL)
	  {
	      (void) fprintf(stderr, "%s: out of memory\n", fname);
	      return(BMP_ERROR);  
	  }


	  for (i = m = l = 0; i < imageinfo.biHeight; i++)
	  {
	      for (j = 0; j < bytesperline+padbytes; j++, l++)
	      {
		  if (j < imageinfo.biWidth) 
		  {
		      rgbdata[m++] = imageinfo.lut[tmpdata[l]].red;
		      rgbdata[m++] = imageinfo.lut[tmpdata[l]].green;
		      rgbdata[m++] = imageinfo.lut[tmpdata[l]].blue;
		  }
	      }
	  }


	  /*-------------------------------------------------------------
	   * Image is in rgbdata, now dither and render it into a pixmap
	   *------------------------------------------------------------*/
          if(rgb_return)
            {
              *rgb_return = rgbdata;
            }
          else
            {
              EZ_RawRGBData2Pixmap(rgbdata, imageinfo.biWidth, imageinfo.biHeight, 
                                   &pixmap);
              (void) my_free( (unsigned char *)rgbdata);
            }

	  /*
	   * Clean up the memory that isn't used anymore.
	   */

	  (void) my_free( (unsigned char *)tmpdata);

	  if (imageinfo.lut != NULL)
	  {
	      free(imageinfo.lut);
	  }

	  break;

      case 16:

	  /*
	   * Now the data is converted with aid of the color lookup table
	   * to RGB data.
	   */

	  if( (rgbdata = (unsigned char *) 
	       my_malloc(3 * imageinfo.biWidth * imageinfo.biHeight *
			 sizeof(unsigned char), _BMPM_TMP_D_)) == 
	      (unsigned char *) NULL)
	  {
	      (void) fprintf(stderr, "%s: out of memory\n", fname);
	      return(BMP_ERROR);  
	  }


	  for (i = m = l = 0; i < imageinfo.biHeight; i++)
	  {
	      for (j = 0; j < imageinfo.biWidth; j++)
	      {
		  rgbdata[m++] = imageinfo.lut[tmpdata[l]].red;
		  rgbdata[m++] = imageinfo.lut[tmpdata[l]].green;
		  rgbdata[m++] = imageinfo.lut[tmpdata[l++]].blue;
	      }
	  }


	  /*-------------------------------------------------------------
	   * Image is in rgbdata, now dither and render it into a pixmap
	   *------------------------------------------------------------*/

          if(rgb_return)
            {
              *rgb_return = rgbdata;
            }
          else
            {
              EZ_RawRGBData2Pixmap(rgbdata, imageinfo.biWidth, imageinfo.biHeight, 
                                   &pixmap);
              (void) my_free( (unsigned char *)rgbdata);
            }


	  /*
	   * Clean up the memory that isn't used anymore.
	   */

	  (void) my_free( (unsigned char *)tmpdata);

	  if (imageinfo.lut != NULL)
	  {
	      free(imageinfo.lut);
	  }

	  break;

      case 24:

	  /* 
	   * True color images have been loaded.
	   */

	  /*-------------------------------------------------------------
	   * Image is in rgbdata, now dither and render it into a pixmap
	   *------------------------------------------------------------*/
          if(rgb_return)
            {
              *rgb_return = tmpdata;
            }
          else
            {
              EZ_RawRGBData2Pixmap(tmpdata, imageinfo.biWidth, imageinfo.biHeight, 
                                   &pixmap);
              (void)my_free( (unsigned char *) tmpdata);
            }
	  break;

      default:
	  fprintf(stderr,"%s: ERROR: imageinfo.biBitCnt unvalid"
		  "in %s\n", fname, filename);

	  return BMP_INFO_HEADER_CORRUPT;

  }


 
  if( (imageinfo.biWidth > 0) &&  (imageinfo.biHeight > 0))
    {
      *width_return = imageinfo.biWidth;
      *height_return = imageinfo.biHeight;
      if(pixmap_return)  *pixmap_return = pixmap;
      return(BMP_NO_ERROR);       /* was return(1) */
    }
  else
    return(BMP_ERROR);            /* was return(0) */
}
/**********************************************************************************************/
#undef _EZ_BMP_C_

