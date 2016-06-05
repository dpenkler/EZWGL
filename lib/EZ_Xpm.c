/*********************************************************************
 *
 *         EZWGL, the EZ Widget and Graphics Library
 *
 *             Copyright (C) 1996, 1997, 1998  Maorong Zou 
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
 *        A simple XPM image reader.
 *
 ***************************************************************************/
#define _EZ_XPM_C_

#include "EZ_Widget.h"

/**************************************************************************
 *
 *  Functions implemented in this file.
 */
int EZ_CreateXPixmapFromXpmData MY_ANSIARGS((char **data,
					     unsigned int *w, unsigned int *h, 
					     Pixmap *pix, Pixmap *shape));
int EZ_CreateXPixmapFromXpmFile MY_ANSIARGS((char *file,
					     unsigned int *w, unsigned int *h, 
					     Pixmap *pix, Pixmap *shape));
int EZ_CreateXPixmapFromBitmapFile MY_ANSIARGS((char *file,
					     unsigned int *w, unsigned int *h, 
					     Pixmap *pix, Pixmap *shape));

int EZ_ReadXpmToRGB MY_ANSIARGS((char *file,
                                 unsigned int *w, unsigned int *h, 
                                 unsigned char **rgb_return));


int  EZ_ReadXpmData MY_ANSIARGS((char **data,
                                 unsigned int *w, unsigned int *h, 
                                 Pixmap *pix, Pixmap *shape,
                                 unsigned char **rgb_return));
/**************************************************************************/

typedef struct XPM_Color_ {
  char     name[16];             /* anyone uses 16 cpp ? */
  unsigned long pixel;
  unsigned char r,g,b, flag;
} XPM_Color;

typedef struct XPM_Hash_Entry_ {
  unsigned int value;   /* base 256 integer. handle cpp < 4 images */
  struct XPM_Hash_Entry_ *next;
  XPM_Color xpmcolor;
} XPM_Hash_Entry;

#define XPM_HTB_SIZE  257


/**************************************************************************
 *
 *  local functions.
 */
static int           XpmGetDataFromFile MY_ANSIARGS((char *file, char ***data, int *nlines));
static int           next_word MY_ANSIARGS((char *str, char *to, int *adv, int *len));
static void          To_lower_case MY_ANSIARGS((char *str));
static unsigned int  XPM_Hash_Name MY_ANSIARGS((char *str, int len));
static void          parse_color MY_ANSIARGS((char *str, int *mask, int visualType, XPM_Color *xc));

/**************************************************************************
 *
 *  Create a Pixmap from Data
 */
int  EZ_CreateXPixmapFromXpmFile(char *file,
				 unsigned int *width_ret, unsigned int *height_ret,
				 Pixmap *pixmap_ret, Pixmap*shape_ret)
{
  int    nlines, i, ans = 0;
  char   **data;
  if(XpmGetDataFromFile(file, &data, &nlines))
    {
      ans= EZ_ReadXpmData(data, width_ret, height_ret, pixmap_ret,shape_ret, NULL);
      for(i = 0; i < nlines; i++)
	if(data[i]) (void)my_free(data[i]);
      (void)my_free( (char *)data);
    }
  return(ans);
}

int  EZ_CreateXPixmapFromXpmData( char **xpmdata,
				  unsigned int *width_ret, unsigned int *height_ret,
				  Pixmap *pixmap_ret, Pixmap *shape_ret)
{
  return(EZ_ReadXpmData(xpmdata, width_ret, height_ret, pixmap_ret,shape_ret, NULL));
}

int  EZ_ReadXPMToRGB( char *file, int  *width_ret, int *height_ret,
		      unsigned char **rgb_return)
{
  int    nlines, i, ans = 0;
  char   **data;
  if(XpmGetDataFromFile(file, &data, &nlines))
    {
      ans= EZ_ReadXpmData(data, (unsigned int *)width_ret, 
                          (unsigned int *) height_ret, 
                          NULL, NULL, rgb_return);
      for(i = 0; i < nlines; i++)
	if(data[i]) (void)my_free(data[i]);
      (void)my_free( (char *)data);
    }
  return(ans);
}

/**************************************************************************
 *
 *  Create a Pixmap from Data
 */
int  EZ_ReadXpmData( char **xpmdata, unsigned int *width_ret, unsigned int *height_ret,
		     Pixmap *pixmap_ret, Pixmap *shape_ret,  unsigned char **rgb_return)
{
  int       w, h, ncolors, cpp, values[6];
  int       c=0, i, j, k, l, m, n, indx, visual_type = 1, itmp;
  char      **data = xpmdata, *str, *tstr;
  char      word[64], mask_name[16];
  Pixmap    pixmap = None, shape = None;
  XPM_Color *xpmcolor = NULL;
  XPM_Hash_Entry *xpm_hash_table = NULL;
  unsigned int has_mask = 0, hash_value;
  unsigned char   *rgbdata = NULL;

  if(data == NULL) return(0);
  mask_name[0] = 0;

#if defined(__cplusplus) || defined(c_plusplus)
  if(EZ_Visual->c_class == GrayScale || EZ_Visual->c_class == StaticGray) visual_type = 0;
#else
  if(EZ_Visual->class == GrayScale || EZ_Visual->class == StaticGray) visual_type = 0;
#endif

  /*
   * parse the values section 
   */
  str = data[0];
  for(j = 0; j < 4; j++)
    {
      if(next_word(str, word, &k, &l) == 0) return(0);
      str += k;
      values[j] = atoi(word);
    }
  w = values[0]; h = values[1]; ncolors = values[2]; cpp = values[3];

  i = 1;
  /*
   * parse the color section
   */
  {
    if(cpp == 1) 
      {
	indx = 257;
	xpmcolor = (XPM_Color *)my_malloc( indx * sizeof(XPM_Color), _XPM_COLOR_);
	if(!xpmcolor) EZ_OutOfMemory("EZ_CreateXPixmapFromXpmData");
	for(j = 0; j < ncolors; j++)
	  {
	    str = data[i++];
	    /* color name */
	    indx = (int) *str;
	    tstr = xpmcolor[indx].name;
	    *tstr++ = *str++;  *tstr = 0;
            parse_color(str, &itmp, visual_type, &(xpmcolor[indx]));
	    if(itmp)  /* this color is a mask */
	      {
		has_mask = (unsigned int) indx;
		mask_name[0] = (xpmcolor[indx].name)[0];
		mask_name[1] = 0;
	      }
	  }
      }
    else 
      {
	XPM_Hash_Entry *tail, *new_entry;
	xpm_hash_table = (XPM_Hash_Entry *)my_malloc( XPM_HTB_SIZE *
						  sizeof(XPM_Hash_Entry), _XPM_TMP_HT_);
	if(!xpm_hash_table) EZ_OutOfMemory("EZ_CreateXPixmapFromXpmData");
	for(j = 0; j < XPM_HTB_SIZE; j++)
	  xpm_hash_table[j].next = (XPM_Hash_Entry *)NULL;
	for(j = 0; j < ncolors; j++)
	  {
	    str = data[i++];
	    hash_value = XPM_Hash_Name(str,cpp);
	    indx = hash_value % XPM_HTB_SIZE;
	    tail = xpm_hash_table[indx].next;
	    new_entry = (XPM_Hash_Entry *)my_malloc(sizeof(XPM_Hash_Entry), _XPM_TMP_HT_E_);
	    if(!new_entry) EZ_OutOfMemory("EZ_CreateXPixmapFromXpmData");
	    new_entry->value = hash_value;
	    if(cpp >= 4) 
	      {
		tstr = (new_entry->xpmcolor).name;
		for(k = 0; k < cpp; k++) tstr[k] = str[k]; tstr[k] = 0;
	      }
            parse_color(str+cpp, &itmp, visual_type, &(new_entry->xpmcolor));
	    if(itmp) /* mask color */
	      {
		has_mask = hash_value;
		(void)strncpy(mask_name, str, cpp);
	      }
	    new_entry->next = tail;
	    xpm_hash_table[indx].next = new_entry;	    
	  }
      }
  }
  /* 
   * parse the pixel section 
   */
  if(rgb_return) /* return RGB */
    {
      unsigned char *uptr;
      rgbdata =  (unsigned char *)my_malloc( 3 * w * h + 2, _XPM_TMP_IMAGE_D_);
      if(rgbdata == NULL) EZ_OutOfMemory("EZ_CreateXPixmapFromXpmData");
    
      uptr = rgbdata;
      if(cpp == 1)
        {
          for( m = 0; m < h; m++)
            {
              str = data[i++];
              for(n = 0; n < w; n++)
                {
                  c = *str++;
                  *uptr++ = xpmcolor[c].r;
                  *uptr++ = xpmcolor[c].g;
                  *uptr++ = xpmcolor[c].b;
                }
            }
        }
      else if(cpp < 4)
        {
          XPM_Hash_Entry *htbptr;
          for(m = 0; m < h; m++)
            {
              str = data[i++];
              for(n = 0; n < w; n++)
                {
                  hash_value = XPM_Hash_Name(str,cpp);  str += cpp;
                  indx = hash_value % XPM_HTB_SIZE;
                  htbptr = xpm_hash_table[indx].next;
                  while(htbptr && htbptr->value != hash_value) htbptr = htbptr->next;
                  if(htbptr)
                    {
                      *uptr++ = (htbptr->xpmcolor).r;
                      *uptr++ = (htbptr->xpmcolor).g;
                      *uptr++ = (htbptr->xpmcolor).b;
                    }
                  else uptr += 3;
                }
            }	
        }
      else  /* cpp >= 4 */
        {
          XPM_Hash_Entry *htbptr;
          for(m = 0; m < h; m++)
            {
              str = data[i++];
              for(n = 0; n < w; n++)
                {
                  hash_value = XPM_Hash_Name(str,cpp);  str += cpp;
                  indx = hash_value % XPM_HTB_SIZE;
                  htbptr = xpm_hash_table[indx].next;
                  while(htbptr && hash_value != htbptr->value &&
                        strncmp((htbptr->xpmcolor).name, str, cpp))
                    htbptr = htbptr->next;
                  if(htbptr)
                    {
                      *uptr++ = (htbptr->xpmcolor).r;
                      *uptr++ = (htbptr->xpmcolor).g;
                      *uptr++ = (htbptr->xpmcolor).b;
                    }
                  else uptr += 3;
                }
            }
        }
    }
  else /* return pixmap */
    {
      XImage *image; 
      char   *mask_data = NULL, *mask_ptr=NULL, *mask_pixel;
      int    bit_pad = 8, ktmp, bmap_bytes_per_line = 1;

      if(EZ_Depth > 16)     bit_pad = 32;
      else if(EZ_Depth > 8) bit_pad = 16;
    
      image = XCreateImage(EZ_Display, EZ_Visual, EZ_Depth, ZPixmap, 0, 0, w, h, bit_pad, 0);
      image->data =  (char *)my_malloc(image->bytes_per_line * h, _XPM_TMP_IMAGE_D_);
      if(!image->data) EZ_OutOfMemory("EZ_CreateXPixmapFromXpmData");
      if(has_mask)  
        {
          bmap_bytes_per_line = (w + 7)/8;
          mask_data = (char *)my_malloc(bmap_bytes_per_line * h * sizeof(char), _XPM_TMP_MASK_D_);
          if(!mask_data) EZ_OutOfMemory("EZ_CreateXPixmapFromXpmData");
          memset(mask_data, 0, bmap_bytes_per_line * h);
        }
    
      if(cpp == 1)
        {
          for(itmp = 0, m = 0; m < h; m++, itmp += bmap_bytes_per_line)
            {
              str = data[i++];
              if(has_mask) mask_ptr = mask_data + itmp;
              for(n = 0; n < w; n++)
                {
                  c = *str++;
                  if(has_mask == (unsigned int)c)
                    {
                      ktmp = n & 0x7;
                      mask_pixel = mask_ptr + (n >> 3);
                      *mask_pixel |= (1 << ktmp);
                    }
                  XPutPixel(image, n, m, xpmcolor[c].pixel);
                }
            }
        }
      else if(cpp < 4)
        {
          XPM_Hash_Entry *htbptr;
          for(itmp = 0, m = 0; m < h; m++, itmp += bmap_bytes_per_line)
            {
              str = data[i++];
              if(has_mask) mask_ptr = mask_data + itmp;
              for(n = 0; n < w; n++)
                {
                  hash_value = XPM_Hash_Name(str,cpp);  str += cpp;
                  if(has_mask == hash_value)
                    {
                      ktmp = n & 0x7;
                      mask_pixel = mask_ptr + (n >> 3);
                      *mask_pixel |= (1 << ktmp);
                    }
                  indx = hash_value % XPM_HTB_SIZE;
                  htbptr = xpm_hash_table[indx].next;
                  while(htbptr && htbptr->value != hash_value)
                    htbptr = htbptr->next;
                  if(htbptr) XPutPixel(image, n, m, (htbptr->xpmcolor).pixel);
                }
            }	
        }
      else  /* cpp >= 4 */
        {
          XPM_Hash_Entry *htbptr;
          for(itmp = 0, m = 0; m < h; m++, itmp += bmap_bytes_per_line)
            {
              str = data[i++];
              if(has_mask) mask_ptr = mask_data + itmp;
              for(n = 0; n < w; n++)
                {
                  hash_value = XPM_Hash_Name(str,cpp);  str += cpp;
                  if(has_mask == hash_value && !strcmp(str, mask_name))
                    {
                      ktmp = n & 0x7;
                      mask_pixel = mask_ptr + (n >> 3);
                      *mask_pixel |= (1 << ktmp);
                    }
                  indx = hash_value % XPM_HTB_SIZE;
                  htbptr = xpm_hash_table[indx].next;
                  while(htbptr && hash_value != htbptr->value &&
                        strncmp((htbptr->xpmcolor).name, str, cpp))
                    htbptr = htbptr->next;
                  if(htbptr) XPutPixel(image, n, m, (htbptr->xpmcolor).pixel);
                }
            }
        }
      /* now make the pixmaps */
      pixmap = XCreatePixmap(EZ_Display, 
                             RootWindow(EZ_Display, EZ_ScreenNum),
                             w, h, EZ_Depth);
      XPutImage(EZ_Display, pixmap, EZ_WRITABLEGC, image, 0,0,0,0, w,h);
    
      if(has_mask && shape_ret != NULL)
        shape = XCreatePixmapFromBitmapData(EZ_Display, 
                                            RootWindow(EZ_Display, EZ_ScreenNum),
                                            mask_data, w, h, 0, 1, 1);
      (void)my_free(image->data);
      image->data = NULL;
      XDestroyImage(image);
      if(has_mask) (void)my_free(mask_data);
    }
  
  if(xpmcolor) (void)my_free(xpmcolor);
  if(xpm_hash_table)
    {
      XPM_Hash_Entry *htbptr, *next;      
      for(j = 0; j < XPM_HTB_SIZE; j++)
	{
	  htbptr = xpm_hash_table + j;
	  next = htbptr->next;
	  while(next)
	    {
	      htbptr = next;
	      next = htbptr->next;
	      (void)my_free((char *)htbptr);
	    }
	}
      (void)my_free( (char *)xpm_hash_table);
    }
  if(rgb_return) 
    {
      *rgb_return = rgbdata;
    }
  else
    {
      *pixmap_ret = pixmap;
      if(shape_ret != NULL) *shape_ret = shape;
    }
  *width_ret = (unsigned int) w;
  *height_ret = (unsigned int) h;
  return(1);
}

/***************************************************************************/
static char *skip_white_spaces(    char *line, int  size)
{
  int  i = 0;
  if(line)
    {
      while(i < size && (*line == ' ' || *line == '\t' || *line == '\n')) line++;  
      if(i == size || *line == '\0') return(NULL); 
    }
  return(line);
}
/***************************************************************************
 *
 *  Read a line from fp. replace comments by ' ' and return the striped string
 */

static char *strip_comment_get_next_line( FILE *fp,  char *buf, int  size, int *incomment)
     
{
  char  *cptr;
  int   count = 0,quote=0;

  if(fgets(buf, size, fp) == 0) { buf[0] = '\0'; return(NULL);}
  cptr = buf;
  while(1)
    {
	if( *cptr == '\"') quote=1;
      if(quote==0 && *incomment == 0 && *cptr == '/' && *(cptr+1) == '*')
	{ 
	  /* we are in comment */
	  *cptr++ = ' '; *cptr++ = ' '; count += 2;
	  *incomment = 1;
	}
      if(*incomment)
	{
	  while( *cptr && count < size)
	    {
	      if(*cptr == '*' && *(cptr+1) == '/')
		{
		  *incomment = 0; 
		  *cptr++ = ' '; count++;
		}
	      *cptr++ = ' '; count++;
	    }
	}
      if(*cptr == '\r') *cptr = ' ';
      if(count >= size || *cptr == '\0') { *cptr = 0; break;}
      cptr++;
    }
  return(buf);
}
static void free_tmp_data(char **data, int size)
     
{
  int i;
  for(i = 0; i < size; i++)
    { if(data[i]) (void) my_free(data[i]);}
  (void)my_free( (char *)data);
}
/***************************************************************************
 *
 *  Read an XPM file into data.
 */
static int XpmGetDataFromFile( char *file, char ***data_return, int *nlines_return)
{
  char **data, *cptr, *tptr;
  FILE *fp;
  char buf[4096];
  int  i, count, done, csize, format;
  int  incomment = 0, linesize = 1024, vincrement=256;

  if((fp = fopen(file, "r")) == NULL)  return(0);
  /*
   * check for magic
   */
  do {
      if(fgets(buf, 1024, fp) == 0) { fclose(fp); return(0);}    /* ~empty file  */
  } while((cptr = skip_white_spaces(buf,1024)) == NULL);
  if( !strncmp(cptr, "/* XPM */", 6)) format = 0;
  else if( !strncmp(cptr, "! XPM2", 6)) format = 1;
  else  { fclose(fp); return(0);}    /* wrong magic  */

  if(format == 0)
    {
      /*
       * so we have a right magic. check for static ...
       */
      do 
	{
	  if(strip_comment_get_next_line(fp, buf,1024, &incomment) == NULL) 
	    {  fclose(fp); return(0); }    /* EOF */
	} while((cptr = skip_white_spaces(buf,1024)) == NULL);
      if(strncmp(cptr, "static", 6)) {  fclose(fp); return(0);}  /* file is not of the correct format */
      /*
       * file looks right. Search for left paren. First, alter the current line
       * by replacing all chars except '{' with ' '.
       */
      while(cptr && *cptr && *cptr != '{') *cptr++ = ' ';
      cptr = skip_white_spaces(buf,4096);
      while(1)
	{
	  if(cptr == NULL)
	    {
	      if(strip_comment_get_next_line(fp, buf,4096, &incomment) == NULL) 
		{  fclose(fp); return(0);}
	      cptr = skip_white_spaces(buf,4096);
	    }
	  else if(*cptr == '{')
	    {
	      *cptr++ = ' ';
	      break;
	    }
	  else  /* something is wrong */
	    {
	      fclose(fp); return(0);
	    }
	}
    }

  /*
   * we are pretty sure the file is of the corrext format 
   */
  data = (char **)my_malloc(256 * sizeof(char *), _XPM_CONTENTS_);
  if(!data) EZ_OutOfMemory("XpmGetDataFromFile");
  memset((void *)data, 0, 256 * sizeof(char *));
  data[0] = (char *)my_malloc(256 * sizeof(char),_XPM_CONTENTS_D_);
  if(!data[0]) EZ_OutOfMemory("XpmGetDataFromFile");

  csize = 256;

  if(format == 0)
    {
      done = 0; count = 0;
      while(!done)
	{
	  while((cptr = skip_white_spaces(buf,4096)) == NULL) 
	    {
	      if(strip_comment_get_next_line(fp, buf,4096, &incomment) == NULL){ done = 1; break;}
	    }
	  if(!done)
	    {
	      if( *cptr == '}') break;       /* the last line */
	      else if(*cptr != '"') {free_tmp_data(data, csize);  fclose(fp); return(0);}
              cptr++;
              if(count == 0)
                {
                  int itmp=0, kk,ll, jj, values[5];
                  char word[64], *tstr = data[0];
		  
                  tptr = data[count++];
                  while(*cptr && *cptr != '"' && itmp < 255) {*tptr++ = *cptr++; itmp++;}*tptr = '\0';
                  for(jj = 0; jj < 4; jj++)
                    {
                      if(next_word(tstr, word, &kk, &ll) == 0) return(0);
                      tstr += kk;
                      values[jj] = atoi(word);
                    }
                  linesize = values[0] * values[3] + 4;
		  if(linesize < 1024) linesize = 1024;
		  if(values[1] > 256) vincrement = values[1] - 254;
                  for(jj=1; jj <256; jj++)
                    data[jj] = (char *)my_malloc(linesize * sizeof(char),_XPM_CONTENTS_D_);
                }
              else
                {
                  tptr = data[count++];
                  while(*cptr && *cptr != '"') *tptr++ = *cptr++; *tptr = '\0';
                }
	      if(count == csize)
		{
		  data = (char **)my_realloc(data, (csize + vincrement)*sizeof(char *), _XPM_CONTENTS_D_);
		  csize += vincrement;
		  for(i = count; i < csize; i++) 
		    {
		      data[i] = (char *)my_malloc(linesize * sizeof(char), _XPM_CONTENTS_D_);
		      if(!data[i]) EZ_OutOfMemory("XpmGetDataFromFile");
		    }
		}
	      if(strip_comment_get_next_line(fp, buf,4096, &incomment) == NULL)
		{ done = 1; break;}
	    }
	}
    }
  else  /* XPM2 */
    {
      done = 0; count = 0;
      while(!done)
	{
	  char *ptr;
	  if(fgets(buf, 4096, fp) == NULL) { done = 1; break; }
	  ptr = buf;
	  while(*ptr) { if(*ptr == '\r') {*ptr = '\0'; break;} ptr++;}
          if(count == 0) /* the first line, read off the image info  */
            {
              int kk,ll, jj, values[5];
              char word[64], *tstr = data[0];

              strncpy(data[count++], buf, 255);
              for(jj = 0; jj < 4; jj++)
                {
                  if(next_word(tstr, word, &kk, &ll) == 0) return(0);
                  tstr += kk;
                  values[jj] = atoi(word);
                }
              linesize = values[0] * values[3] + 4;
	      if(linesize < 1024) linesize = 1024;
              if(values[1] > 256) vincrement = values[1] - 254;
              for(jj=1; jj <256; jj++)
                data[jj] = (char *)my_malloc(linesize * sizeof(char),_XPM_CONTENTS_D_);
            }
          else strcpy(data[count++], buf);
	  if(count == csize)
	    {
	      data = (char **)my_realloc(data, (csize + vincrement)*sizeof(char *), _XPM_CONTENTS_D_);
	      csize += vincrement;
	      for(i = count; i < csize; i++) 
		{
		  data[i] = (char *)my_malloc(linesize * sizeof(char), _XPM_CONTENTS_D_);
		  if(!data[i]) EZ_OutOfMemory("XpmGetDataFromFile");
		}
	    }	  
	}
    }
  *data_return = data;
  *nlines_return = csize;
  fclose(fp);
  return(1);
}

/****************************************************************
 *
 *  Extract the next word in str. 
 *   to:       place to store the word.
 *   advance:  position after the word, rel to str.
 */
static  int next_word( char *str, char *to, int*advance, int *length)
     
{
  int  i = 0, j = 0;

  if(str)
    {
      while(*str == ' ' || *str == '\t' || *str == '\n') { str++; i++;}
      if(*str)
        {
          while(*str && *str != ' ' && *str != '\t' && *str != '\n')
            { *to++ = *str++; j++;}
          *to = 0;
          *advance = i+j;
	  *length = j;
          return(1);
        }
    }
  *to = 0;
  *advance = i;  
  *length = j;
  return(0);
}

/****************************************************************
 *
 *  Parse color info.
 */
static void parse_color( char *str,
			 int  *mask,      /* 1 if this color is none    */
			 int visual_type, /* 1 for color and 0 for gray */
			 XPM_Color *xpmColor)
{
  int           advance, cset = 0, gset =0, mset= 0, itmp;
  unsigned long cpixel, gpixel, mpixel, pixel;
  char word[64], color[64],tmpstr[64], *sptr, *tptr;
  XColor  xc;

  *mask = 0;
  xc.red = xc.green = xc.blue = 0;
  cpixel = gpixel = EZ_ColorArray[EZ_NORMAL_GRAY1];
  mpixel = 0;
  while(next_word(str,word, &advance, &itmp)) /* key first */
    {
      str += advance;
      if(next_word(str,color, &advance, &itmp) == 0) return; /* format problem */
      str += advance;
      /*
       * unfortunately, a color name may consists of several words.
       */
      sptr = str;
      while(next_word(sptr, tmpstr, &advance, &itmp))
	{
	  if(itmp == 1 && (tmpstr[0] == 'c' || tmpstr[0] == 'm' ||
			   tmpstr[0] == 'g' || tmpstr[0] == 's'))
	    break;
	  else if(itmp == 2 && tmpstr[0] == 'g' && tmpstr[1] == '4') break;
	  else 
	    {
	      sptr += advance;
	      (void) strcat(color,tmpstr);
	    }
	}
      str = sptr;
      To_lower_case(color);
      tptr = color; 
      if(*tptr == '#') tptr++;
      if(!strcmp(tptr, "none") || !strcmp(tptr, "mask") || !strcmp(tptr, "transparent"))
	*mask = 1;
      else
	{
	  if(*word == 'c')
	    {
	      cset = 1;
              if(XParseColor(EZ_Display,EZ_Colormap,color,&xc))
                cpixel = EZ_AllocateColorFromNameWork(&xc);
	    }
	  else if(*word == 'g') /* g and g4 are treated the same */
	    {
	      gset = 1;
              if(XParseColor(EZ_Display,EZ_Colormap,color,&xc))
                gpixel = EZ_AllocateColorFromNameWork(&xc);
	    }
	  else if(*word == 'm')
	    {
	      mset = 1;
              if(XParseColor(EZ_Display,EZ_Colormap,color,&xc))
                mpixel = EZ_AllocateColorFromNameWork(&xc);
	    }
	}
    }
  if(visual_type)
    {
      if(cset) pixel = cpixel;
      else if(gset) pixel = gpixel;
      else if(mset) pixel = mpixel;
      else pixel = cpixel;
    }
  else
    {
      if(gset) pixel = gpixel;
      else if(cset) pixel = cpixel;
      else if(mset) pixel = mpixel;
      else pixel = gpixel;
    }
  xpmColor->pixel = pixel;
  xpmColor->r = xc.red >>8;
  xpmColor->g = xc.green >>8;
  xpmColor->b = xc.blue >>8;
}
/************************************************************************************/
static void To_lower_case(  char *str)
{
  if(str)
    {
      while(*str)
	{
	  if(isupper(*str)) *str = tolower(*str);
	  str++;
	}
    }
}
/************************************************************************************/
static unsigned int XPM_Hash_Name( char *name, int len)
     
{
  int i;
  unsigned int v = 0;

  for(i = 0; i < len; i++)
    v = ( (v & 0x00ffffff) << 8) + (unsigned int) name[i];
  return(v);
}

/************************************************************************************
 * hack !
 */
int EZ_CreateXPixmapFromBitmapFile( char *file,
				    unsigned int  *width_ret, unsigned int *height_ret,
				    Pixmap *pixmap_ret, Pixmap *shape_ret)
{
  int dummyx, dummyy;

  if(shape_ret) *shape_ret = (Pixmap)NULL;
  if(XReadBitmapFile(EZ_Display,
		     RootWindow(EZ_Display, EZ_ScreenNum),
		     file,
		     width_ret,
		     height_ret,
		     pixmap_ret,
		     &dummyx,
		     &dummyy) == BitmapSuccess)
    return(1);
  return(0);
}
/************************************************************************************/    
#undef _EZ_XPM_C_
