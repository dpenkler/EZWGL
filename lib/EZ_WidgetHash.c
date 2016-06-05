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
 *  May 1997. Handling of raw rgb images added by Arno Dabekaussen.
 *  November 1997.  Release Version 1.3
 *  November 1998.  Release Version 1.4
 *  December 1999.  Release Version 1.50
 */
/*********************************************************************
 ***                                                               ***
 ***            Simple hash tables for widgets.                    ***
 ***             For quick loop up of a widget                     ***
 ***                  from its window id                           ***
 ***            And from its address                               ***
 ***                                                               ***
 *********************************************************************/
#define _EZ_WIDGET_HASH_C_
#include "EZ_Widget.h"

#define STANDARD_BITMAP_INCLUDE "/usr/include/X11/bitmaps/"
#define STANDARD_PIXMAP_INCLUDE "/usr/include/X11/pixmaps/"

/*********************************************************************
 * 
 *  Functions implemented in this file:
 */
EZ_Bitmap         *EZ_CreateShapeMaskFromFigure MY_ANSIARGS((EZ_Item *fitem));
EZ_Bitmap         *EZ_CreateLabelPixmapFromXPixmap MY_ANSIARGS((Pixmap pix, Pixmap shape,
                                                                int x, int y, int w, int h, int flag));
EZ_Bitmap         *EZ_CreateLabelPixmapFromXpmFile MY_ANSIARGS((char *file));
EZ_Bitmap         *EZ_CreateLabelPixmapFromXpmData MY_ANSIARGS((char **data));
EZ_Bitmap         *EZ_CreateLabelPixmapFromRawRGB MY_ANSIARGS((unsigned char *data, int width, int height));
EZ_Bitmap         *EZ_CreateLabelPixmapFromStaticRawRGB MY_ANSIARGS((unsigned char *data, 
                                                                     int width, int height));
EZ_Bitmap         *EZ_CreateLabelPixmapFromXBitmapFile MY_ANSIARGS((char *file));
EZ_Bitmap         *EZ_CreateLabelPixmapFromXBitmapData MY_ANSIARGS((char *file, unsigned int w, unsigned int h));
EZ_Bitmap         *EZ_CreateLabelPixmapFromImageFile MY_ANSIARGS((char *file));

void              EZ_GetDarkBrightHighlightPixmap MY_ANSIARGS((EZ_Bitmap *bitmap,
                                                      Pixmap *dk, Pixmap *bt, Pixmap *ht,Pixmap *sp));

void              EZ_FreeLabelPixmap MY_ANSIARGS((EZ_Bitmap *bitmap));
void              EZ_FreeXPixmap MY_ANSIARGS((Pixmap pixmap));
int               EZ_GetLabelPixmapInfo MY_ANSIARGS((EZ_Bitmap *bitmap,
						     Pixmap *p_ret, Pixmap *s_ret, int *w, int *h));
void              EZ_SetIconPath MY_ANSIARGS((char *path));
char              *EZ_GetIconPath MY_ANSIARGS((void));

EZ_Bitmap         *EZ_CreateBackgroundPixmapsFromXPixmap MY_ANSIARGS((Pixmap pix,int x,int y,int w,int h));
EZ_Bitmap         *EZ_CreateBackgroundPixmapsFromXPixmapX MY_ANSIARGS((Pixmap pix,int x,int y,int w,int h,
                                                                      int p1, int r, int g, int b, int p2));
EZ_Bitmap         *EZ_CreateBackgroundPixmapFromRawRGB MY_ANSIARGS((unsigned char *data,int width,int height));
int               EZ_SwapLabelPixmaps  MY_ANSIARGS((EZ_Bitmap *b1, EZ_Bitmap *b2));
int               EZ_CopyLabelPixmaps  MY_ANSIARGS((EZ_Bitmap *dest, EZ_Bitmap *src));

EZ_Bitmap         *EZ_CreateBackgroundPixmapsFromImageFile MY_ANSIARGS((char *file));
EZ_Bitmap         *EZ_CheckBGPixmap MY_ANSIARGS((EZ_Bitmap *vv));
EZ_Bitmap         *EZ_ScaleLabelPixmap MY_ANSIARGS((EZ_Bitmap *pix, int w, int h, int darkness, int smooth));
unsigned char     *EZ_ScaleRGB  MY_ANSIARGS((unsigned char *rgb, int sw, int sh,
                                             int w, int h, int darkness, int smooht));
EZ_Bitmap         *EZ_CreateBackgroundPixmapsFromRawRGB MY_ANSIARGS((unsigned char *data, int w, int h));
unsigned char     *EZ_ScaleRGBX  MY_ANSIARGS((unsigned char *rgb, int sw, int sh,
                                              int w, int h, int sr, int sg, int sb,
                                              int r, int g, int b, int bright, int smooth));

unsigned char     *EZ_XPixmap2RGB MY_ANSIARGS((Pixmap pixmap, int x, int y, int w, int h));
/*******************************************************************/
void              EZ_InitializeWidgetHT MY_ANSIARGS((void));
void              EZ_RemoveWidgetFromMappedHT  MY_ANSIARGS((EZ_Widget *widget));
void              EZ_RemoveWidgetFromAllHT  MY_ANSIARGS((EZ_Widget *widget));
void              EZ_InsertWidgetToMappedHT  MY_ANSIARGS((EZ_Widget *widget));
void              EZ_InsertWidgetToAllHT  MY_ANSIARGS((EZ_Widget *widget));
EZ_Widget         *EZ_LookupWidgetFromMappedHT  MY_ANSIARGS((Window win));
EZ_Widget         *EZ_LookupWidgetFromAllHT  MY_ANSIARGS((EZ_Widget *widget));
EZ_Widget         *EZ_LookupWidgetFromAllHTUL  MY_ANSIARGS((unsigned long addr));

EZ_Widget         *EZ_WindowIsWidgetWindow  MY_ANSIARGS((Window window));
EZ_Widget         *EZ_WidgetExist  MY_ANSIARGS((EZ_Widget *widget));

int               EZ_HashString  MY_ANSIARGS((char *str, int size));
EZ_Bitmap         *EZ_GetBitmapFromFile  MY_ANSIARGS((char *file, int ref));

EZ_Bitmap         *EZ_GetBitmapFromData  MY_ANSIARGS((char *data, unsigned int w, unsigned int h));
EZ_Bitmap         *EZ_GetPixmapFromData  MY_ANSIARGS((char  **data));
EZ_Bitmap         *EZ_GetPixmapFromRawRGB  MY_ANSIARGS((unsigned char  *data, int width, int height));
EZ_Bitmap         *EZ_GetPixmapFromStaticRawRGB  MY_ANSIARGS((unsigned char  *data, int width, int height));
EZ_Bitmap         *EZ_GetPixmapFromPixmap  MY_ANSIARGS((Pixmap pix, int x, int y, int w, int h));
EZ_Bitmap         *EZ_GetPixmapFromPermPixmap  MY_ANSIARGS((Pixmap pix, int x, int y, int w, int h));
EZ_Bitmap         *EZ_GetImageFromFile  MY_ANSIARGS((char *file, int ref));
EZ_Bitmap         *EZ_GetAnyPixmapFromFile  MY_ANSIARGS((char *file));
char              *EZ_GetBitmapFileName  MY_ANSIARGS((EZ_Bitmap *bitmap));
void              EZ_FreeBitmap  MY_ANSIARGS((EZ_Bitmap *bitmap));
void              EZ_DestroyAllWidgets  MY_ANSIARGS((void));

void              EZ_TraverseAllWidgets  MY_ANSIARGS((EZ_CallBack cb, void *data));
/*********************************************************************
 *
 *  local functions.
 */
static  void  EZ_InitiailzeBitmapHT  MY_ANSIARGS((void));
static  int   EZ_WidgetHashWin  MY_ANSIARGS((Window win));
static  int   EZ_WidgetHashAddress  MY_ANSIARGS((unsigned long addr));
static  EZ_Bitmap *AllocateBitmapEntry  MY_ANSIARGS((int bucket, char *fname, int type,
						    Pixmap pixmap, Pixmap shape,
						    int w, int h, int ref, int savename));
static EZ_Bitmap *EZ_CheckBGPixmapWork MY_ANSIARGS((EZ_Bitmap *vv, int p, int r, int g, int b, int q));
/*********************************************************************
 *
 *  This is the hash table for widget windows. All created X windows
 *  are stored here.
 */
EZ_MappedWidgetHTEntry         EZ_MappedWidgetsHashTable[EZ_WIDGET_HASH_TABLE_SIZE]; /* for mapped widgets */
EZ_AllWidgetHTEntry            EZ_AllWidgetsHashTable[EZ_WIDGET_HASH_TABLE_SIZE];

static char *ezIconPath = NULL;

/**********************************************************************/
char *EZ_TildeExpandPath(pin) char *pin;
{
  int  length = 1024;
  if(pin)
    {
      char *ret = (char *) malloc(length * sizeof(char));
      char *p = pin, *q, *tilde;
      int  tlen = 0;
      ret[0] = '\0';
      while(p)
	{
	  q = strchr(p, ':');
	  if(q) { *q = 0; q++;}
	  tilde = EZ_TildeExpand(p);
	  if(tilde)
	    {
	      tlen += strlen(tilde);
	      if(tlen >= length)
		{
		  length *= 2;
		  ret = (char *)realloc(ret, length * sizeof(char));
		}
	      strcat(ret,tilde);
	      if(q)strcat(ret, ":");
	      free(tilde);
	    }
	  p = q;
	}
      return(ret);
    }
  return(NULL);
}
/**********************************************************************/
void EZ_SetIconPath(path) char *path;
{
  if(ezIconPath != NULL) (void)my_free(ezIconPath);
  ezIconPath = NULL;
  ezIconPath = EZ_TildeExpandPath(path);
}
char *EZ_GetIconPath MY_ANSIARGS((void))
{ return(ezIconPath);}
/**********************************************************************
 *
 *  Initialization.
 */
void  EZ_InitializeWidgetHT()
{
  register int i;
  for(i = 0; i < EZ_WIDGET_HASH_TABLE_SIZE; i++)
    {
      EZ_MappedWidgetsHashTable[i].next = (EZ_MappedWidgetHTEntry *)NULL;
      EZ_MappedWidgetsHashTable[i].widget = (EZ_Widget *)NULL;
      EZ_AllWidgetsHashTable[i].next = (EZ_AllWidgetHTEntry *)NULL;
      EZ_AllWidgetsHashTable[i].widget = (EZ_Widget *)NULL;
    }
  EZ_InitiailzeBitmapHT();
  ezIconPath = NULL;
}

/*************************************************************
 *
 *  Destroy all widgets. Have to destroy one widget a time
 *  since EZ_DestroyWidget modify the global hash table.
 */
void EZ_DestroyAllWidgets()
{
  EZ_AllWidgetHTEntry  *tmp;  
  EZ_Widget **all;
  int i, count = 0;
  for(i = 0; i < EZ_WIDGET_HASH_TABLE_SIZE; i++)
    {
      tmp = EZ_AllWidgetsHashTable[i].next;
      while(tmp) { count++;  tmp = tmp->next; }
    }
  all = (EZ_Widget **)malloc( (count+1) * sizeof(EZ_Widget *));
  for(count = 0, i = 0; i < EZ_WIDGET_HASH_TABLE_SIZE; i++)
    {
      tmp = EZ_AllWidgetsHashTable[i].next;
      while(tmp) { all[count] = tmp->widget; count++;  tmp = tmp->next;}
    }
  all[count] = NULL;
  for(i = 0; i < count; i++)
    { if(EZ_WidgetParent(all[i]) == NULL) EZ_DestroyWidget(all[i]);}
  for(i = 0; i < count; i++) EZ_DestroyWidget(all[i]);
  (void)free(all);
}
/*************************************************************
 *
 *  Extermely simple hash functions. (ID % EZ_WIDGET_HASH_TABLE_SIZE)
 */
static int EZ_WidgetHashAddress(widget)
     unsigned long widget;
{
  return( widget % EZ_WIDGET_HASH_TABLE_SIZE);
}
static int EZ_WidgetHashWin(win)
     Window win;
{
  unsigned long tmp1;

  tmp1 = (unsigned long) win;
  return( tmp1 % EZ_WIDGET_HASH_TABLE_SIZE);
}
/*************************************************************/
/*
 * Insert a widget into the hash table.
 */
void EZ_InsertWidgetToAllHT(widget)
     EZ_Widget *widget;
{
  EZ_AllWidgetHTEntry  *tmp;
  int                  hash_value;

  if(widget)
    {
      hash_value = EZ_WidgetHashAddress((unsigned long)widget);
      tmp = (EZ_AllWidgetHTEntry *)my_malloc(sizeof(EZ_AllWidgetHTEntry), _WIDGET_ALL_HT_ENTRY_);
      if(!tmp) EZ_OutOfMemory("EZ_InsertWidgetToAllHT");
      tmp->widget = widget;
      tmp->next = EZ_AllWidgetsHashTable[hash_value].next;
      EZ_AllWidgetsHashTable[hash_value].next = tmp;
    }
}

void EZ_InsertWidgetToMappedHT(widget)
     EZ_Widget *widget;
{
  EZ_MappedWidgetHTEntry *tmp;
  int                     hash_value;

  if(EZ_WidgetWindow(widget) == (Window )NULL) return;
  hash_value = EZ_WidgetHashWin(EZ_WidgetWindow(widget));
  
  tmp = (EZ_MappedWidgetHTEntry *)my_malloc(sizeof(EZ_MappedWidgetHTEntry),_WIDGET_MAPPED_HT_ENTRY_);
  if(!tmp) EZ_OutOfMemory("EZ_InsertWidgetToMappedHT");
  tmp->widget = widget;
  tmp->next = (EZ_MappedWidgetsHashTable[hash_value].next);
  EZ_MappedWidgetsHashTable[hash_value].next = tmp;
}

/*
 *  Lookup a wisget.
 */
EZ_Widget  *EZ_LookupWidgetFromAllHT(widget)
     EZ_Widget *widget;
{
  EZ_AllWidgetHTEntry *tmp;
  int                 hash_value;

  if(widget)
    {
      hash_value = EZ_WidgetHashAddress((unsigned long)widget);
      tmp = EZ_AllWidgetsHashTable[hash_value].next;
      while(tmp)
	{
	  if(tmp->widget == widget) return(widget);
	  tmp = tmp->next;
	}
    }
  return( (EZ_Widget *)NULL);
}

/* play safe. Lookup a widget when the ptr to widget is casted to a ulong */
EZ_Widget  *EZ_LookupWidgetFromAllHTUL(widgetaddr)
     unsigned long widgetaddr;
{
  EZ_AllWidgetHTEntry *tmp;
  int                 hash_value;

  if(widgetaddr)
    {
      hash_value = EZ_WidgetHashAddress((unsigned long)widgetaddr);
      tmp = EZ_AllWidgetsHashTable[hash_value].next;
      while(tmp)
	{
	  if((unsigned long)(tmp->widget) == widgetaddr) return(tmp->widget);
	  tmp = tmp->next;
	}
    }
  return( (EZ_Widget *)NULL);
}


EZ_Widget  *EZ_LookupWidgetFromMappedHT(win)
     Window  win;
{
  EZ_MappedWidgetHTEntry *tmp;
  int               hash_value;

  hash_value = EZ_WidgetHashWin(win);
  
  tmp = (EZ_MappedWidgetsHashTable[hash_value].next);
  while(tmp)
    {
      if(EZ_WidgetWindow(tmp->widget) == win)
	return(tmp->widget);
      tmp = tmp->next;
    }
  return( (EZ_Widget *)NULL);
}
/*
 * Remove an entry form HT.
 */
void  EZ_RemoveWidgetFromAllHT(widget)
     EZ_Widget *widget;
{
  EZ_AllWidgetHTEntry *tmp, *tmpa;
  int                  hash_value;

  if(widget)
    {
      hash_value = EZ_WidgetHashAddress((unsigned long)widget);
      tmp = &(EZ_AllWidgetsHashTable[hash_value]);
      while(tmp && tmp->next)
	{
	  if(tmp->next->widget == widget)
	    {
	      tmpa = tmp->next;
	      tmp->next = tmp->next->next;
	      (void)my_free( (char *)(tmpa));
	      return;
	    }
	  tmp = tmp->next;
	}
    }
}

void  EZ_RemoveWidgetFromMappedHT(widget)
     EZ_Widget *widget;
{
  EZ_MappedWidgetHTEntry *tmp, *tmpa;
  int               hash_value;

  hash_value = EZ_WidgetHashWin(EZ_WidgetWindow(widget));
  
  tmp = &(EZ_MappedWidgetsHashTable[hash_value]);
  while(tmp && tmp->next)
    {
      if(tmp->next->widget == widget)
	{
	  tmpa = tmp->next;
	  tmp->next = tmp->next->next;
	  (void)my_free( (char *)(tmpa));
	  return;
	}
      tmp = tmp->next;
    }
}

/*******************************************************************
 ***                                                             ***
 ***  Well, we maintain also another hash table for bitmaps.     ***
 ***                                                             ***
 *******************************************************************/

#define  EZ_PRIME  EZ_WIDGET_HASH_TABLE_SIZE

EZ_BitmapHTEntry  EZ_BitmapHashTable[EZ_PRIME+6];

/******************************************************************
 *
 *  Initialize the hash table.
 */
static void  EZ_InitiailzeBitmapHT()
{
  register int i;

  for(i = 0; i < EZ_PRIME + 4; i++)
    EZ_BitmapHashTable[i].next = (EZ_BitmapHTEntry *)NULL;
  /*
   * This last one is saved for drag and drop. It will be constantly
   * modified so it is hidden.
   */
  EZ_SnapShotBitmap = AllocateBitmapEntry(EZ_PRIME + 3,  (char *)NULL,  EZ_BITMAP_IS_PIXMAP,
					  (Pixmap)NULL, (Pixmap)NULL, 0, 0, 1, 0);
  EZ_SnapShotBitmap->ref = 1234567;
}

/****************************************************************/
static char *locateImageFilePath(inFile, inOut) char *inFile, *inOut;
{
  if(EZ_FileReadable(inFile)) return(inFile);
  
  /* file is not readable or does not exist */
  {
    /*----------------------------------------
     * file is not in the current directory.
     * check $EZ_LIBDIR
     *---------------------------------------*/
    char   *p,*tmp = getenv("EZ_LIBDIR");
    char   tmp_name[1024], path[1024];
    char   filename[512];
    int    file_ok = 0;
    if(ezIconPath != NULL)
      {
        if(tmp) sprintf(path, "%s:%s", ezIconPath, tmp);
        else strcpy(path, ezIconPath);
	tmp = path;
      }
    if(tmp != (char *)NULL)
      {
	tmp = strcpy(tmp_name, tmp);
	while(tmp)
	  {
	    int len;
	    p = strchr(tmp, ':');
	    if(p) *p++ = '\0';
	    strcpy(filename, tmp);
	    len = strlen(filename);
	    if(filename[len-1] !='/')
	      {
		filename[len] = '/';
		filename[len+1] = '\0';
	      }
	    (void) strcat(filename, inFile);
	    file_ok = EZ_FileReadable(filename);
	    if(file_ok) { strcpy(inOut, filename); return(inOut); break;}
	    tmp = p;
	  }
      }
  }
  return(NULL);
}
/******************************************************************
 *
 *  Given a file name, create a bitmap from data contained in file.
 */
EZ_Bitmap *EZ_GetBitmapFromFile(file_name, ref)
     char *file_name; int ref;  /* reference type 1 for widgets, 0 for text in textwidget*/
{
  int              hash_value;
  EZ_BitmapHTEntry *tmp;
  
  /*------------------------------
   * Check the hash table first.
   *-----------------------------*/
  hash_value = EZ_HashString(file_name, EZ_PRIME);
  tmp = EZ_BitmapHashTable[hash_value].next;
  while(tmp != (EZ_BitmapHTEntry *)NULL)
    {
      if((tmp->name)[0] == file_name[0])
	{
	  if(!strcmp(tmp->name,file_name))
	    {
	      /* got it */
	      if(ref) tmp->ref  += 1;
	      else    tmp->special = 1;  /* used in TextWidget */
	      return(tmp);
	    }
	}
      tmp = tmp->next;
    }

  /*-------------------------------------
   * so we don't have it already,
   * create a new HT entry if we can
   * create a bitmap from file.
   *------------------------------------*/
  {
    Pixmap pixmap = (Pixmap )NULL;
    Pixmap shape_mask = (Pixmap )NULL;
    char   tmp_name[1024];
    char   *file = locateImageFilePath(file_name, tmp_name);
    int    file_ok = (file != NULL);
    /*-----------------------------------
     * Still not found,
     * check  "/usr/include/X11/bitmaps/"
     * and    "/usr/include/X11/pixmaps/"
     *----------------------------------*/
    if(file_ok == 0)
      {
	(void)strcpy(tmp_name, STANDARD_BITMAP_INCLUDE);
	(void)strcat(tmp_name, file_name);
	file_ok = EZ_FileReadable(tmp_name);
	if(file_ok == 0)
	  {
	    (void)strcpy(tmp_name, STANDARD_PIXMAP_INCLUDE);
	    (void)strcat(tmp_name, file_name);
	    file_ok = EZ_FileReadable(tmp_name);
	  }
	file = tmp_name;
      }
    
    if(file_ok)
      {
	unsigned int width, height;
	int      dummyx, dummyy, created = 0;
	if(XReadBitmapFile(EZ_DisplayForWidgets,
			   RootWindow(EZ_DisplayForWidgets, EZ_ScreenNumForWidgets),
			   file,
			   &width,
			   &height,
			   &pixmap,
			   &dummyx,
			   &dummyy) == BitmapSuccess)
	  created = 1;
	else
	  {
	    if(EZ_CreateXPixmapFromXpmFile(file, &width, &height, 
				       &pixmap,&shape_mask))
	      created = 2;
	  }
	if(created)
	  return(AllocateBitmapEntry(hash_value, file_name,
				     (created == 1? EZ_BITMAP_IS_BITMAP : EZ_BITMAP_IS_PIXMAP),
				     pixmap, shape_mask, width, height, ref, 1));
      }
  }
  return((EZ_Bitmap *)NULL);
}

/******************************************************************
 *
 *  Create a bitmap from pixmap data
 */
EZ_Bitmap *EZ_GetBitmapFromData(data, width, height)
     char          *data;
     unsigned int  width, height;
{
  EZ_BitmapHTEntry *tmp;
  Pixmap pixmap = (Pixmap )NULL;  

  /*----------------------------------------------------------------
   * Bitmap created from bitmap data are saved in table[EZ_PRIME+1]
   *---------------------------------------------------------------*/
  tmp = EZ_BitmapHashTable[EZ_PRIME+1].next;
  while(tmp != (EZ_BitmapHTEntry *)NULL)
    {
      if(tmp->name == data &&
	 tmp->width == width && tmp->height == height)
	{
	  tmp->ref  += 1;
	  return(tmp);
	}
      tmp = tmp->next;
    }
  /*-------------------------------------
   * so we don't have it
   *------------------------------------*/
  if((pixmap = XCreateBitmapFromData(EZ_Display,
				     RootWindow(EZ_Display, EZ_ScreenNum),
				     data,
				     width, height)) != None)
    return(AllocateBitmapEntry(EZ_PRIME+1, data, EZ_BITMAP_IS_BITMAP,
			       pixmap, (Pixmap)NULL, width, height, 1, 1));
  return((EZ_Bitmap *)NULL);
}
/******************************************************************
 *
 *  Create a pixmap from pixmap data
 */
EZ_Bitmap *EZ_GetPixmapFromData(data)
     char **data;
{
  EZ_BitmapHTEntry *tmp;
  
  /*-------------------------------------------------------
   * Bitmap created from data are saved in table[EZ_PRIME]
   *------------------------------------------------------*/
  tmp = EZ_BitmapHashTable[EZ_PRIME].next;
  while(tmp != (EZ_BitmapHTEntry *)NULL)
    {
      if((char **)(tmp->name) == data)
	{
	  tmp->ref  += 1;
	  return(tmp);
	}
      tmp = tmp->next;
    }
  /*-------------------------------------
   * so we don't have it already,
   * create a new HT entry
   *------------------------------------*/
  {
    Pixmap pixmap = (Pixmap )NULL;
    Pixmap shape_mask = (Pixmap )NULL;
    unsigned int width=0, height=0;
    
    if(EZ_CreateXPixmapFromXpmData(data, &width, &height, &pixmap,&shape_mask))
      return(AllocateBitmapEntry(EZ_PRIME, (char *)data, EZ_BITMAP_IS_PIXMAP,
				 pixmap, shape_mask, width, height, 1, 0));
  }
  return((EZ_Bitmap *)NULL);
}


/******************************************************************
 *
 *  Create a pixmap from raw rgb data
 *  Arno Dabekaussen, 04-05-97, Maastricht, NL.
 *
 */
static EZ_Bitmap *EZ_GetPixmapFromRawRGBWork(data, width, height, flag)
     unsigned  char *data;
     int width;
     int height;
     int flag; /* flag whether data is static */
{
  EZ_BitmapHTEntry *tmp = NULL;

  if(flag)  /* if data is static, reuse the image if possible */
    {
      tmp = EZ_BitmapHashTable[EZ_PRIME].next;
      while(tmp != (EZ_BitmapHTEntry *)NULL)
	{
	  if((unsigned char *)(tmp->name) == data)
	    {
	      tmp->ref  += 1;

	      /*
	       * Bug found:
	       * What happens if a bitmap was found in hash table but
	       * the size has been changed ? Maybe better to free it, 
	       * so that a new one can be allocated. Now it is not possible to 
	       * call ConfigureWidget several times with an identical start
	       * pointer to new bitmap data (with different sizes).
	       *
	       * return(tmp);
	       *
	       */
	      /* Sorry arno, I take this off.
	       * If one wants to manipulate images, he/she should
	       * explicitly recreate the image. 
	       *
	       * EZ_FreeBitmap(tmp);
	       * break;
	       */
	      return(tmp);	  
	    }
	  tmp = tmp->next;
	}
    }

  /*-----------------------------------------
   * so we don't have it already or freed it,
   * create a new HT entry
   *----------------------------------------*/

  {
    Pixmap pixmap = (Pixmap ) NULL;
    Pixmap shape_mask = (Pixmap )NULL;
    
    if(EZ_CreateXPixmapFromRawRGB(data, width, height, &pixmap))
      {
	if(flag) return(AllocateBitmapEntry(EZ_PRIME, data, EZ_BITMAP_IS_PIXMAP,
					    pixmap, shape_mask, width, height, 1, 0));
	else
	  {
	    EZ_Bitmap *newp = (EZ_BitmapHTEntry *)my_malloc(sizeof(EZ_BitmapHTEntry), _BITMAP_HT_ENTRY_);
	    if(!newp) EZ_OutOfMemory("AllocateBitmapEntry");
	    newp->ref = 1; newp->special = 0; 
	    newp->type = EZ_BITMAP_IS_PIXMAP;
	    newp->width = width;
	    newp->height = height;
	    newp->x = 0;
	    newp->y = 0;
	    newp->bucket = EZ_PRIME + 5;
	    newp->next = NULL;
	    newp->pixmap = pixmap;
	    newp->shape = shape_mask;
	    newp->dkpixmap = None;
	    newp->btpixmap = None;
	    newp->htpixmap = None;
	    newp->name = NULL;
	    return(newp);
	  }
      }
  }
  return((EZ_Bitmap *) NULL);
}

EZ_Bitmap *EZ_GetPixmapFromRawRGB(data, width, height)
     unsigned char *data;
    int width;
    int height;
{
  return(EZ_GetPixmapFromRawRGBWork(data, width, height, 0));
}

EZ_Bitmap *EZ_GetPixmapFromStaticRawRGB(data, width, height)
     unsigned char *data;
    int width;
    int height;
{
  return(EZ_GetPixmapFromRawRGBWork(data, width, height, 1));
}

/******************************************************************/
static Pixmap extractMaskBitmap(sshape,x,y,w,h) 
     Pixmap sshape; int x,y,w,h;
{
  Pixmap shape= None;
  if(sshape)
    {
      XImage *image;
      if( (image = XGetImage(EZ_Display, sshape, x,y, w, h, 1, XYPixmap)))
        {
          int xc,yc, advance = (w + 7)/8;
          unsigned char *mdata, *mptr;
          mdata = (unsigned char *)malloc( advance * h * sizeof(char));
          memset(mdata, 0, advance * h * sizeof(char));
          for(mptr = mdata, yc = 0; yc < h; yc++, mptr += advance)
            {
              for(xc = 0; xc < w; xc++)
                {
                  if(XGetPixel(image,xc,yc)==0)
                    {
                      int ktmp = xc & 0x7;
                      int idx =  (xc >> 3);
                      mptr[idx] |= (1 << ktmp);
                    }
                }
            }
          shape = XCreatePixmapFromBitmapData(EZ_Display, 
                                              RootWindow(EZ_Display, EZ_ScreenNum),
                                              mdata, w, h, 0, 1, 1);
          (void)free(mdata);
          XDestroyImage(image);
        }
    }
  return(shape);
}
/******************************************************************/
static EZ_Bitmap  *EZ_GetPixmapFromPixmapWork(src,sshape, x,y,w,h, flag)
     Pixmap src, sshape;
     int    x,y,w,h;
     int    flag;
{
  EZ_BitmapHTEntry *tmp;


  if(flag) /* static pixamp, reuse */
    {
      /*-------------------------------------------------------
       * Bitmap created from data are saved in table[EZ_PRIME+2]
       *------------------------------------------------------*/
      tmp = EZ_BitmapHashTable[EZ_PRIME+2].next;
      while(tmp != (EZ_BitmapHTEntry *)NULL)
	{
	  if((Pixmap)(tmp->name) == src &&
	     tmp->width == w && tmp->height == h &&
	     tmp->x == x && tmp->y == y)
	    {
	      tmp->ref  += 1;
	      return(tmp);
	    }
	  tmp = tmp->next;
	}
    }
  /*-------------------------------------
   * so we don't have it already,
   * create a new HT entry
   *------------------------------------*/
  tmp = NULL;
  if(src && w > 0 && h > 0 && x >= 0 && y >= 0)
    {
      Window    win = EZ_DummyWindow;
      Pixmap pixmap, shape = None;
      
      pixmap = XCreatePixmap(EZ_Display, win, w, h, EZ_Depth);
      XCopyArea(EZ_Display,src,pixmap,EZ_WRITABLEGC,x,y,w,h,0,0);
      if(sshape)
        {
          shape = extractMaskBitmap(sshape,x,y,w,h);
        }
      if(flag)
	{
	  tmp = AllocateBitmapEntry(EZ_PRIME+2, (char *)src, EZ_BITMAP_IS_PIXMAP,
				    pixmap, shape, w, h, 1, 0);
	  if(tmp) {	tmp->x = x; tmp->y = y;}
	}
      else
	{
	  EZ_Bitmap *newp = (EZ_BitmapHTEntry *)my_malloc(sizeof(EZ_BitmapHTEntry), _BITMAP_HT_ENTRY_);
	  if(!newp) EZ_OutOfMemory("AllocateBitmapEntry");
	  newp->ref = 1; newp->special = 0; 
	  newp->type = EZ_BITMAP_IS_PIXMAP;
	  newp->width = w;
	  newp->height = h;
	  newp->x = 0;
	  newp->y = 0;
	  newp->bucket = EZ_PRIME+2;
	  newp->next = NULL;
	  newp->pixmap = pixmap;
	  newp->shape = shape;
	  newp->dkpixmap = None;
	  newp->btpixmap = None;
	  newp->htpixmap = None;
	  newp->name = NULL;
	  tmp = newp;
	}
    }
  return(tmp);
}

/*******************************************************************/
EZ_Bitmap  *EZ_GetPixmapFromPixmap(src,x,y,w,h)
     Pixmap src;
     int    x,y,w,h;
{
  return(EZ_GetPixmapFromPixmapWork(src,None, x,y,w,h, 0));
}

EZ_Bitmap  *EZ_GetPixmapFromPermPixmap(src,x,y,w,h)
     Pixmap src;
     int    x,y,w,h;
{
  return(EZ_GetPixmapFromPixmapWork(src,None, x,y,w,h,1));
}
EZ_Bitmap *EZ_CreateLabelPixmapFromXPixmap(src, shape,x,y,w,h,flag)
     Pixmap src, shape; int x, y, w, h, flag;
{
  return(EZ_GetPixmapFromPixmapWork(src,shape, x,y,w,h,flag));
}
/*******************************************************************
 *
 *   Free a bitmap entry.
 */
void EZ_FreeBitmap(bitmap)
     EZ_Bitmap *bitmap;
{
  int bucket; 
  if(bitmap == (EZ_Bitmap *)NULL) return;

  bucket = bitmap->bucket; 
  bitmap->ref -= 1;
  if(bitmap->ref <= 0 && bitmap->special == 0)
    { 
      EZ_Bitmap *tmp,*prev = EZ_BitmapHashTable + bucket;
      while(prev && prev->next != bitmap) prev = prev->next;
      if(prev)
	{
	  tmp = prev->next;
	  prev->next = tmp->next;
	  if(bucket < EZ_PRIME) (void)my_free(tmp->name); /* !!!!!!! */
	}
      if(bitmap->dkpixmap != bitmap->pixmap && bitmap->dkpixmap != (Pixmap )None)
	XFreePixmap(EZ_Display, bitmap->dkpixmap);
      if(bitmap->btpixmap != bitmap->pixmap && bitmap->btpixmap != (Pixmap )None)
	XFreePixmap(EZ_Display, bitmap->btpixmap);
      if(bitmap->htpixmap != bitmap->pixmap && bitmap->htpixmap != (Pixmap )None)
	XFreePixmap(EZ_Display, bitmap->htpixmap);
      if(bitmap->shape != (Pixmap )None && bitmap->shape != bitmap->pixmap)
	XFreePixmap(EZ_Display, bitmap->shape);
      if(bitmap->pixmap != (Pixmap )None)
	XFreePixmap(EZ_Display, bitmap->pixmap);

      (void)my_free( (char *)bitmap);
    }
}

/*
 * free the bitmap hash table.  EZ_DestroyAllWidget should
 * take care of freeing this table. However, user may use
 * EZ_CreateLabelBitmapFromm*** without freeing them. And
 * the last entry is not freed.
 */
void EZ_DestroyAllBitmaps()
{
  int i, notdone;
  EZ_BitmapHTEntry *tmp;
  
  for(i = 0; i < EZ_PRIME + 4; i++)
    {
      notdone = 1;
      while(notdone)
	{
	  tmp = EZ_BitmapHashTable[i].next;
	  if(tmp)
	    {
	      tmp->ref = 0; tmp->special = 0;
	      EZ_FreeBitmap(tmp);
	    }
	  else notdone = 0;
	}
    }
}
/******************************************************************
 *
 *  Retrieve the bitmap file name for a given bitmap.
 */
char *EZ_GetBitmapFileName(bitmap)
     EZ_Bitmap *bitmap;
{
  if(bitmap && bitmap->bucket < EZ_PRIME)
    return(bitmap->name);
  return(NULL);
}
/*******************************************************************/
int EZ_HashString(s, size)
     char *s; int size;
{
  char *p;
  unsigned long h = 0, g;
  
  for(p = s; *p ; p++) 
    {
      h = (h << 4) + (*p);
      if((g = h & 0xf0000000))
        {
          h = h ^ (g >> 24);
          h = h ^ g;
        }
    }
  return((int) (h % size));
}
/******************************************************************/

EZ_Bitmap *EZ_GetImageFromFile(file_name, ref)
     char *file_name; int ref;
{
  int              hash_value;
  EZ_BitmapHTEntry *tmp;
  
  /*------------------------------
   * Check the hash table first.
   *-----------------------------*/
  hash_value = EZ_HashString(file_name, EZ_PRIME);
  tmp = EZ_BitmapHashTable[hash_value].next;
  while(tmp != (EZ_BitmapHTEntry *)NULL)
    {
      if((tmp->name)[0] == file_name[0])
	{
	  if(!strcmp(tmp->name,file_name))
	    {
	      /* got it */
	      if(ref) tmp->ref  += 1;
	      else tmp->special = 1;
	      return(tmp);
	    }
	}
      tmp = tmp->next;
    }

  /*-------------------------------------
   * so we don't have it
   *------------------------------------*/
  {
    Pixmap pixmap = None, shape = None;
    char   tmp_name[1024];
    char   *file = locateImageFilePath(file_name, tmp_name);
    int    file_ok = (file != NULL);

    if(file_ok)
      {
	int itype, width, height,ans;
        itype = EZ_GetImageFileType(file);
        if(itype == EZ_XPM_FORMAT)
          ans = EZ_CreateXPixmapFromXpmFile(file, &width, &height, &pixmap, &shape);
        else
          ans = EZ_CreateXPixmapFromImageFile(file,&width, &height, &pixmap);
	if(ans != 0)
	  return( AllocateBitmapEntry(hash_value, file_name, EZ_BITMAP_IS_PIXMAP,
				      pixmap, shape, width, height, ref, 1));
      }
  }
  return((EZ_Bitmap *)NULL);
}
/*************************************************************************************/
EZ_Bitmap *EZ_CreateShapeMaskFromFigure(fitem) EZ_Item *fitem;
{
  int w, h;
  Pixmap shape=None, pixmap = EZ_MakePixmapFromFigItem(fitem, &w, &h);
  if(pixmap)
    {
      EZ_Bitmap *b=NULL;
      shape = extractMaskBitmap(pixmap,0,0,w,h);
      XFreePixmap(EZ_Display, pixmap);      
      if(shape)
        b = AllocateBitmapEntry(EZ_PRIME+2, (char *)NULL, EZ_BITMAP_IS_BITMAP, shape, shape, 
                                w, h, 1, 0);
      return(b);
    }
  return(NULL);
}
/*************************************************************************************/
static EZ_Bitmap *AllocateBitmapEntry(bucket, fname, type, pixmap, shape_mask, width,  
				      height, ref_type,savefname)
     int     bucket, width, height, type, ref_type, savefname;
     Pixmap  pixmap, shape_mask;
     char    *fname;
{
  EZ_Bitmap *tmp = EZ_BitmapHashTable[bucket].next;
  EZ_Bitmap *newp = (EZ_BitmapHTEntry *)my_malloc(sizeof(EZ_BitmapHTEntry), _BITMAP_HT_ENTRY_);
  if(!newp) EZ_OutOfMemory("AllocateBitmapEntry");
  if(savefname)
    {
      newp->name = (char *)my_malloc( (1+strlen(fname)) * sizeof(char), _BITMAP_NAME_);
      if(!newp->name) EZ_OutOfMemory("AllocateBitmapEntry");      
      (void)strcpy(newp->name, fname);
    }
  else newp->name = fname;

  if(ref_type) { newp->ref = 1; newp->special = 0; }
  else  { newp->ref = 0; newp->special = 1;}
  newp->type = type;
  newp->width = width;
  newp->height = height;
  newp->x = 0;
  newp->y = 0;
  newp->bucket = bucket;
  newp->next = tmp;
  newp->pixmap = pixmap;
  newp->shape = shape_mask;
  newp->dkpixmap = None;
  newp->btpixmap = None;
  newp->htpixmap = None;
  EZ_BitmapHashTable[bucket].next = newp;
  return(newp);
}
/*************************************************************************************/
EZ_Bitmap *EZ_GetAnyPixmapFromFile(file)
     char *file;
{
  EZ_Bitmap *bitmap = EZ_GetBitmapFromFile(file, 0);
  if(bitmap == NULL) bitmap = EZ_GetImageFromFile(file, 0);
  return(bitmap);
}
/*************************************************************************************
 *
 *       Addons ...
 *
 *************************************************************************************/
void  EZ_FreeXPixmap(pixmap)
     Pixmap pixmap;
{
  if(pixmap) XFreePixmap(EZ_Display, pixmap);
}

/*************************************************************************************/
void   EZ_FreeLabelPixmap(pixlabel)
     EZ_Bitmap *pixlabel;
{
  if(pixlabel) EZ_FreeBitmap(pixlabel);
}
/*************************************************************************************/
EZ_Bitmap  *EZ_CreateLabelPixmapFromXpmFile(file)
     char *file;
{
  return(EZ_GetBitmapFromFile(file, 1));
}
EZ_Bitmap  *EZ_CreateLabelPixmapFromXBitmapFile(file)
     char *file;
{
  return(EZ_GetBitmapFromFile(file, 1));
}
/*************************************************************************************/
EZ_Bitmap  *EZ_CreateLabelPixmapFromXBitmapData(data, w, h)
     char *data; unsigned int w, h;
{
  return(EZ_GetBitmapFromData(data, w, h));
}

/*****************************************************************************/

/*
 * Allows labels to consist of raw rgb data, to avoid complicated xpm
 * handling.
 * Arno Dabekaussen, 04-05-97, Maastricht, NL.
 */

EZ_Bitmap  *EZ_CreateLabelPixmapFromRawRGB(data, width, height)
     unsigned  char *data;
     int  width;
     int  height;
{
  return(EZ_GetPixmapFromRawRGB(data, width, height));
}

EZ_Bitmap  *EZ_CreateLabelPixmapFromStaticRawRGB(data, width, height)
     unsigned char *data;
     int  width;
     int  height;
{
  return(EZ_GetPixmapFromStaticRawRGB(data, width, height));
}
/*************************************************************************************/
EZ_Bitmap  *EZ_CreateLabelPixmapFromXpmData(data)
     char **data;
{
  return(EZ_GetPixmapFromData(data));
}
/*************************************************************************************/
EZ_Bitmap  *EZ_CreateLabelPixmapFromImageFile(file)
     char *file;
{
  return(EZ_GetImageFromFile(file, 1));
}
/*************************************************************************************/
EZ_Widget  *EZ_WindowIsWidgetWindow(window)
     Window window;
{
  return(EZ_LookupWidgetFromMappedHT(window));
}
EZ_Widget  *EZ_WidgetExist(widget)
     EZ_Widget *widget;
{
  return(EZ_LookupWidgetFromAllHT(widget));
}
/*************************************************************************************/
int EZ_GetLabelPixmapInfo(bitmap, p_ret, s_ret, w_ret, h_ret)
     EZ_Bitmap *bitmap;
     Pixmap *p_ret, *s_ret;
     int *w_ret, *h_ret;
{
  if(bitmap)
    {
      if(p_ret) *p_ret = bitmap->pixmap;
      if(s_ret) *s_ret = bitmap->shape;
      if(w_ret) *w_ret = bitmap->width;
      if(h_ret) *h_ret = bitmap->height;
      if(bitmap->type == EZ_BITMAP_IS_BITMAP) return(1);
      return(EZ_Depth);
    }
  return(0);
}
/*************************************************************************************/
EZ_Bitmap *EZ_CreateBackgroundPixmapsFromXPixmapX(src, x, y, w, h, p1,r,g,b,p2)
     Pixmap src; int x, y, w, h;
     int p1,r,g,b,p2;
{
  EZ_Bitmap *bb = EZ_CreateLabelPixmapFromXPixmap(src, None, x,y,w,h, 0);
  return(EZ_CheckBGPixmapWork(bb, p1,r,g,b,p2));
}

EZ_Bitmap *EZ_CreateBackgroundPixmapsFromXPixmap(src, x, y, w, h)
     Pixmap src; int x, y, w, h;
{
  EZ_Bitmap *bb = EZ_CreateLabelPixmapFromXPixmap(src, None, x,y,w,h, 0);
  return(EZ_CheckBGPixmapWork(bb, 100,0,0,0,100));
}
/*************************************************************************************/
EZ_Bitmap *EZ_CreateBackgroundPixmapsFromRawRGB(rgb,w,h)
     unsigned char *rgb; int w; int h;
{
  Pixmap dkpixmap = None, btpixmap = None, htpixmap = None;
  EZ_Bitmap *bb;
  EZ_SetDarkAndBrightPixmapReturns(&dkpixmap, &btpixmap, &htpixmap);
  bb = EZ_CreateLabelPixmapFromRawRGB(rgb, w, h);
  if(bb)
    {
      bb->dkpixmap = dkpixmap;
      bb->btpixmap = btpixmap;
      bb->htpixmap = htpixmap;
    }  
  EZ_SetDarkAndBrightPixmapReturns(NULL, NULL, NULL);
  return(EZ_CheckBGPixmapWork(bb,100,0,0,0,100));  
}
/*************************************************************************************/
EZ_Bitmap *EZ_CreateBackgroundPixmapsFromImageFile(file_name)
     char *file_name;
{
  int              hash_value;
  EZ_BitmapHTEntry *tmp;
  EZ_Bitmap        *vv = NULL;
  /*------------------------------
   * Check the hash table first.
   *-----------------------------*/
  hash_value = EZ_HashString(file_name, EZ_PRIME);
  tmp = EZ_BitmapHashTable[hash_value].next;
  while(tmp != (EZ_BitmapHTEntry *)NULL)
    {
      if((tmp->name)[0] == file_name[0])
	{
	  if(!strcmp(tmp->name,file_name))
	    {
	      /* got it */
	      tmp->ref  += 1;
	      vv = tmp;
	      break;
	    }
	}
      tmp = tmp->next;
    }
  /*-------------------------------------
   * so we don't have it
   *------------------------------------*/
  if(vv == NULL)
    {
      Pixmap pixmap = None, shape=None, dkpixmap = None, btpixmap = None, htpixmap = None;
      char   tmp_name[1024];
      char   *file = locateImageFilePath(file_name, tmp_name);
      int    file_ok = (file != NULL);

      EZ_SetDarkAndBrightPixmapReturns(&dkpixmap, &btpixmap, &htpixmap);
      if(file_ok)
	{
	  int itype, width, height,ans;
          itype = EZ_GetImageFileType(file);
          if(itype == EZ_XPM_FORMAT)
            ans = EZ_CreateXPixmapFromXpmFile(file, &width, &height, &pixmap, &shape);
          else
            ans = EZ_CreateXPixmapFromImageFile(file,&width, &height, &pixmap);
	  if(ans != 0)
	    {
	      vv =  AllocateBitmapEntry(hash_value, file_name, EZ_BITMAP_IS_PIXMAP,
					pixmap, shape, width, height, 1, 1);
	      if(vv)
		{
		  vv->dkpixmap = dkpixmap;
		  vv->btpixmap = btpixmap;
		  vv->htpixmap = htpixmap;
		}
	    }
	}
      EZ_SetDarkAndBrightPixmapReturns(NULL, NULL, NULL);
    }

  /* verify vv is set properly or fix it */
  vv = EZ_CheckBGPixmap(vv);

  if(vv) 
    {
      if(vv->type != EZ_BITMAP_IS_PIXMAP) /* cannot use Bitmap for bg tile */
	{
	  EZ_FreeLabelPixmap(vv);
	  vv = NULL;
	}
      else
	{
	  if(vv->dkpixmap == None) vv->dkpixmap = vv->pixmap;
	  if(vv->btpixmap == None) vv->btpixmap = vv->pixmap;
	  if(vv->htpixmap == None) vv->htpixmap = vv->pixmap;
	}
    }
  return(vv);
}

/***********************************************************************/
void EZ_GetDarkBrightHighlightPixmap(bitmap, dk, bt, ht, shape)
     EZ_Bitmap *bitmap; Pixmap *dk, *bt, *ht, *shape;
{
  if(bitmap)
    {
      EZ_CheckBGPixmap(bitmap);
      if(dk) *dk = bitmap->dkpixmap;
      if(bt) *bt = bitmap->btpixmap;
      if(ht) *ht = bitmap->htpixmap;
      if(shape) *shape = bitmap->shape;
    }
}
/***********************************************************************/
static int XimageToRGB(image,dptr,w,h)
     XImage *image; unsigned char *dptr; int w,h;
{
  if(image == NULL || dptr == NULL) return(0);
  if(EZ_Depth <= 8)
    {
      int           i, x,y;
      XColor        *xc, xcolors[COLOR_TAB_SIZE];
      unsigned long pixel;
      
      for(i = 0; i < COLOR_TAB_SIZE; i++) 
        {
          xcolors[i].pixel = i;
          xcolors[i].red=xcolors[i].green=xcolors[i].blue=0;
        }
      XQueryColors(EZ_Display,EZ_Colormap, xcolors, EZ_Visual->map_entries);
      for(y = 0; y < h; y++)
        {
          for(x = 0; x < w; x++)
            {
              pixel = XGetPixel(image,x,y);
              xc = xcolors + (int)pixel;
              *dptr++ = (unsigned char)((xc->red)>>8);
              *dptr++ = (unsigned char)((xc->green)>>8);
              *dptr++ = (unsigned char)((xc->blue)>>8);
            }
        }
      return(1);
    }
  else if(EZ_Depth > 8)
    {
      int           x,y,r,g,b;
      unsigned long pixel;
      /*---------------------------------------
       * we are on a >= 16bit TrueColor display
       *--------------------------------------*/
      for(y = 0; y < h; y++)
        {
          for(x = 0; x < w; x++)
            {
              pixel = XGetPixel(image,x,y);
              EZ_PIXEL2RGB(pixel,r,g,b);
              *dptr++ = r;
              *dptr++ = g;
              *dptr++ = b;
            }
        }      
      return(2);
    }
  return(0);
}
/***********************************************************************/
static EZ_Bitmap *EZ_CheckBGPixmapWork(vv, percent, r,g,b, pp) 
     EZ_Bitmap *vv; int percent; int r,g,b,pp; /* pp is brightness scale factor */
{
  if(vv)
    {
      if(pp < 0) pp = 0; if(percent < 0) percent = 0;
      if( vv->type == EZ_BITMAP_IS_PIXMAP && vv->dkpixmap == None)
	{
	  int i, w = vv->width, h = vv->height;
	  XImage *image = EZ_ReadDrawable2XImage(vv->pixmap, 0, 0, w, h);
	  if(image)
	    {
	      unsigned char *sdata, *tdata, *dptr;
	      if((sdata = my_malloc( (3 * (w)* h + 2) * sizeof(unsigned char),_MISC_DATA_)) &&
		 (tdata = my_malloc( (3 * (w)* h + 2) * sizeof(unsigned char),_MISC_DATA_)))
		{
		  dptr = sdata;
                  if(XimageToRGB(image,dptr,w,h))
                    {
                      int tlen = 3*w*h;
                      int mix = (r|g|b);
                      int ppp = percent * pp;
                      int ppercent = (ppp << 12)/10000;
                      int altered = 0;
		      Pixmap dkpix=None, btpix=None, htpix = None;
                      EZ_XImage *ezXImage = EZ_CreateXImage(w,h, 0);
                      
                      XDestroyImage(image); image = NULL;
                      
                      if(percent != 100 || pp != 100 || mix != 0)
                        {
                          Pixmap tmppix = None;
                          if(mix)
                            {
                              unsigned char rri[260], ggi[260], bbi[260];
                              r = r * 4096 * pp/100 ; g = g * 4096 *pp/100; b =  b * 4096*pp/100;
                              for(i = 0; i < 256; i++)
                                {
                                  unsigned int itmp;
                                  itmp = (i * ppercent + r) >> 12;
                                  rri[i] =  (unsigned char)(itmp  > 255? 255 : itmp);
                                  itmp = (i * ppercent + g) >> 12;
                                  ggi[i] =  (unsigned char)(itmp  > 255? 255 : itmp);
                                  itmp = (i * ppercent + b) >> 12;
                                  bbi[i] =  (unsigned char)(itmp  > 255? 255 : itmp); 
                                }
                              for(i = 0; i < tlen; ) 
                                {
                                  sdata[i] = rri[sdata[i]]; i++;
                                  sdata[i] = ggi[sdata[i]]; i++;
                                  sdata[i] = bbi[sdata[i]]; i++;
                                }
                              altered = 1;
                            }
                          else if(ppp != 10000)
                            {
                              unsigned char rgbi[260];
                              for(i = 0; i < 256; i++)
                                {
                                  unsigned int itmp = (i * ppercent) >> 12;
                                  rgbi[i] = (unsigned char) (itmp > 255? 255: itmp);
                                }
                              for(i = 0; i < tlen; i++) 
                                sdata[i] = rgbi[sdata[i]];
                              altered = 1;
                            }
                          if(altered)
                            {
                              EZ_RawRGBData2PixmapWork(sdata, w,h,&tmppix,ezXImage);
                              if(mix == 0)
                                {
                                  int jtmp = percent * pp/ 10000;
                                  if(jtmp > 135 && jtmp < 150) dkpix = vv->pixmap;
                                  if(jtmp > 82 && jtmp < 91) { btpix = vv->pixmap;}
                                  else if( jtmp > 73 && jtmp < 81) { htpix =vv -> pixmap;}
                                  else  XFreePixmap(EZ_Display, vv->pixmap);
                                } 
                              else  XFreePixmap(EZ_Display, vv->pixmap);
                              vv->pixmap = tmppix;
                            }
                        }
                      if(dkpix == None)
                        {
                          unsigned char rgbi[260]; 
                          for(i=0; i<256;i++) rgbi[i] = (unsigned char) (i * 7 /10);
                          for(i = 0; i < tlen; i++) tdata[i] =  rgbi[sdata[i]];
                          EZ_RawRGBData2PixmapWork(tdata, w,h,&dkpix,ezXImage);
                        }
                      if(htpix == None)
                        {
                         unsigned char rgbi[260]; 
                          for(i=0; i<256;i++)
                            {
                              unsigned int itmp = i * 115/100;
                              rgbi[i] = (unsigned char)(itmp > 255? 255: itmp);
                            }
                          for(i = 0; i < tlen; i++) 
                            tdata[i] = rgbi[sdata[i]];
                          EZ_RawRGBData2PixmapWork(tdata, w,h,&htpix, ezXImage);
                        }
                      if(btpix == None)
                        {
                          unsigned char rgbi[260]; 
                          for(i=0; i<256;i++)
                            {
                              unsigned int itmp = i * 13/10;
                              rgbi[i] =  (unsigned char) (itmp > 255? 255: itmp);
                            }
                          for(i = 0; i < tlen; i++)
                            tdata[i] = rgbi[sdata[i]];
                          EZ_RawRGBData2PixmapWork(sdata, w,h,&btpix, ezXImage);
                        }
		      vv->dkpixmap = dkpix;
		      vv->btpixmap = btpix;
		      vv->htpixmap = htpix;
                      EZ_DestroyXImage(ezXImage);
		    }
		  (void)my_free(sdata); (void)my_free(tdata);
		}
	      if(image) XDestroyImage(image);
	    }
        }
      return(vv);
    }
  return(NULL);
}
/*************************************************************************************/
EZ_Bitmap *EZ_CheckBGPixmap(vv) EZ_Bitmap *vv;
{ return(EZ_CheckBGPixmapWork(vv, 100,0,0,0,100));}
/*************************************************************************************/
int EZ_CopyLabelPixmaps(dest, src) EZ_Bitmap *dest, *src;
{ return(EZ_SwapLabelPixmaps(dest,src)); }
/*************************************************************************************/
int EZ_SwapLabelPixmaps(b1,b2)   EZ_Bitmap *b1, *b2;
{
  if(b1 && b2)
    {
      EZ_Bitmap tmp;
      memcpy(&tmp, b1, sizeof(EZ_Bitmap));
      memcpy(b1, b2, sizeof(EZ_Bitmap));
      b1->ref = tmp.ref; b1->special = tmp.special; b1->bucket = tmp.bucket;
      tmp.ref = b2->ref; tmp.special =b2->special; tmp.bucket = b2->bucket;
      memcpy(b2,  &tmp, sizeof(EZ_Bitmap));
      return(1);
    }
  return(0);
}
/*************************************************************************************
 *
 *  rgb[w0,h0] --->nrgb[w,h], RGB scaled by   ff/100
 */
unsigned char *EZ_ScaleRGB(sdata, w0,h0,w,h,ff,smooth)
     unsigned char *sdata; int  w0,h0,w,h,ff,smooth;
{ return(EZ_ScaleRGBX(sdata, w0, h0, w, h, ff, ff, ff, 0, 0, 0, 100,smooth)); }
/*************************************************************************************
 *
 *  scale rgb data:  r-> (sr * r/100 + rt)*bright,
 *                   g-> (sg * g/100 + gt)*bright,
 *                   b-> (sb * b/100 + bt)*bright
 */
unsigned char *EZ_ScaleRGBX(sdata, w0,h0,w,h,sr,sg,sb, rt,gt, bt, bright,smooth)
     unsigned char *sdata; int  w0,h0,w,h,sr,sg,sb,rt,gt,bt, bright, smooth;
{
  unsigned char *tdata=NULL, *ndata = NULL;
  int i, tlen = 3 * w0 * h0;
  if((ndata = my_malloc( (3 * (w0)* h0 + 2) * sizeof(unsigned char),_MISC_DATA_)) == NULL)
    EZ_OutOfMemory("EZ_ScaleRGBX");      
  if(sr != 100 || sg != 100 || sb != 100 || rt != 0 || gt != 0 || bt != 0 || bright != 100)
    {
      int ssr = ((sr * bright) << 12)/10000;
      int ssg = ((sg * bright) << 12)/10000;
      int ssb = ((sb * bright) << 12)/10000;
      int ttr = ((rt * bright) << 12)/100;
      int ttg = ((gt * bright) << 12)/100;
      int ttb = ((bt * bright) << 12)/100;
      unsigned char rri[260], ggi[260], bbi[260];

      /* 4/28/99, tlen is far bigger than 256. Move mult out of the long loop */
      for(i = 0 ; i < 256; i++)
        {
          int aaa = (i * ssr + ttr) >> 12;
          int bbb = (i * ssg + ttg) >> 12;
          int ccc = (i * ssb + ttb) >> 12;
          rri[i] =  (unsigned char) (aaa > 255? 255: aaa);
          ggi[i] =  (unsigned char) (bbb > 255? 255: bbb);
          bbi[i] =  (unsigned char) (ccc > 255? 255: ccc);
        }
      for(i = 0; i < tlen; i += 3)
        {
          ndata[i] =    rri[(int)sdata[i]];
          ndata[i+1] =  ggi[(int)sdata[i+1]];
          ndata[i+2] =  bbi[(int)sdata[i+2]];
        }  
    }
  else memcpy(ndata,sdata, tlen);
  
  if(w == w0 && h == h0)
    { tdata = ndata; ndata = NULL;  }
  else /* scale */
    {
      int     i, j;
      double  xs, ys, dx0, dy0;
      xs = (double)(w0)/(double)w;
      ys = (double)(h0)/(double)h;

      if((tdata = my_malloc( (3 * (w)* h + 2) * sizeof(unsigned char),_MISC_DATA_)) == NULL)
        EZ_OutOfMemory("EZ_ScaleRGBX");     
      dy0 = 0.0;
      for(i = 0; i < h; i++)
        {
          int yoff =  3 * i * w;
          int yoff0 = 3 * ((int) (dy0)) * w0;
          dy0 += ys;
          dx0 = 0.0;
          for(j = 0; j < w; j++)
            {
              int xoff0 = 3 * ((int) dx0);
              int xoff =  3 *j;
              int kk = yoff + xoff;
              int kk0 = yoff0 + xoff0;
              tdata[kk++] = ndata[kk0++]; 
              tdata[kk++] = ndata[kk0++];
              tdata[kk++] = ndata[kk0++];
              dx0 += xs;
            }
        }
      /* smooth out, average neighboring 3x3 */
      if(smooth &&(w > w0 || h > h0))
        {
          int prow, nrow, row, rowinc;
          rowinc = 3 * w; 
          prow = 0;  row  = rowinc; nrow = row+row;
          for(i = 2; i < h; i++)
            {
              int pp1;
              for(pp1 = 0, j = 2; j < w ; j++, pp1 += 3)
                {
                  int prowI = prow + pp1;
                  int rowI = row + pp1;
                  int srowI = rowI + 3;
                  int nrowI = nrow + pp1;
                  int r,g,b, r1, g1, b1;
                  r = tdata[prowI] + tdata[prowI+3] + tdata[prowI+6] +
                    tdata[rowI] + tdata[rowI+6] +
                    tdata[nrowI] + tdata[nrowI+3] + tdata[nrowI+6];
                  r1 = (tdata[rowI+3]) << 3;
                  prowI++; rowI++; nrowI++;
                  g = tdata[prowI] + tdata[prowI+3] + tdata[prowI+6] +
                    tdata[rowI] + tdata[rowI+6] +
                    tdata[nrowI] + tdata[nrowI+3] + tdata[nrowI+6];
                  g1 = (tdata[rowI+3]) << 3;
                  prowI++; rowI++; nrowI++;
                  b = tdata[prowI] + tdata[prowI+3] + tdata[prowI+6] +
                    tdata[rowI] + tdata[rowI+6] +
                    tdata[nrowI] + tdata[nrowI+3] + tdata[nrowI+7];
                  b1 = (tdata[rowI+3]) << 3;
                                  
                  tdata[srowI++] = (r+r1)>>4; 
                  tdata[srowI++] = (g+g1)>>4;
                  tdata[srowI++] = (b+b1)>>4;
                }
              prow = row; row = nrow;  nrow += rowinc;
            }
        }
    }
  if(ndata) (void) my_free(ndata);
  return(tdata);
}
/*************************************************************************************/
EZ_Bitmap *EZ_ScaleLabelPixmap(pix, w, h, darkness, smooth)
     EZ_Bitmap *pix; int w,h,darkness, smooth;
{
  if(pix)
    {
      int w0 = pix->width;
      int h0 = pix->height;
      if(w == 0 || h == 0) {w = w0; h = h0;}
      if(w == w0 && h == h0 && darkness == 100) 
        {
          pix->ref += 1; 
          return(pix);
        }
      else if(w > 0 && h > 0 && darkness > 0)
        {
          EZ_Bitmap     *newp = NULL;
	  XImage        *image = EZ_ReadDrawable2XImage(pix->pixmap, 0, 0, w0, h0);
	  if(image)
	    {
	      unsigned char *sdata, *tdata =NULL, *dptr;
	      if((sdata = my_malloc( (3 * (w0)* h0 + 2) * sizeof(unsigned char),_MISC_DATA_)) == NULL)
		EZ_OutOfMemory("EZ_ScaleLabelPixmap");      
	      dptr = sdata;
	      if(XimageToRGB(image,dptr,w0,h0))
		{
                  EZ_XImage *ezXImage = EZ_CreateXImage(w,h, 0);
                  Pixmap tmppix = None;
                  tdata =EZ_ScaleRGB(sdata,w0,h0,w,h,darkness,smooth);
                  EZ_RawRGBData2PixmapWork(tdata, w,h,&tmppix,ezXImage);
                  EZ_DestroyXImage(ezXImage);
                  if(tmppix)
                    newp = AllocateBitmapEntry(EZ_PRIME + 2,  (char *)NULL,  EZ_BITMAP_IS_PIXMAP,
                                               tmppix, None, w, h, 1, 0);
		}
	      if(tdata) my_free((char *)tdata);
	      my_free((char *)sdata);
	    }
	  return(newp);
	}
    }
  return(NULL);
}
/*************************************************************************************/
void EZ_TraverseAllWidgets(cb,data)EZ_CallBack cb; void *data;
{
  if(cb)
    {
      EZ_AllWidgetHTEntry  *tmp;  
      EZ_Widget **all;
      int i, count = 0;
      for(i = 0; i < EZ_WIDGET_HASH_TABLE_SIZE; i++)
        {
          tmp = EZ_AllWidgetsHashTable[i].next;
          while(tmp)
            {
              count++;
              tmp = tmp->next;
            }
	}
      all = (EZ_Widget **)malloc( (count+1) * sizeof(EZ_Widget *));
      for(count = 0, i = 0; i < EZ_WIDGET_HASH_TABLE_SIZE; i++)
        {
          tmp = EZ_AllWidgetsHashTable[i].next;
          while(tmp)
            {
              all[count] = tmp->widget; count++;
              tmp = tmp->next;
            }
        }
      all[count] = NULL;
      for(i = 0; i < count; i++) cb(all[i], data);
      (void) free(all);
    }
}
/*************************************************************************************/
unsigned char *EZ_XPixmap2RGB(pixmap, x0, y0, w0, h0)
     Pixmap pixmap;  int x0,y0,w0,h0;
{
  unsigned char *sdata = NULL;
  if(pixmap != None)
    {
      XImage *image = EZ_ReadDrawable2XImage(pixmap, x0, y0, w0, h0);
      if(image)
        {
          if((sdata =(unsigned char *) malloc( (3 * w0 * h0 + 2) * sizeof(unsigned char))))
            {
              if(XimageToRGB(image,sdata,w0,h0) == 0)
                { (void) free((char *)sdata); sdata = NULL;}
            }
          XDestroyImage(image);
        }
    }
  return(sdata);
}
/*************************************************************************************/

#ifdef EZ_DEBUG 

Pixmap getNormalPixmap(bp) EZ_Bitmap *bp;
{ if(bp) return(bp->pixmap); return(None);}
Pixmap getDarkPixmap(bp) EZ_Bitmap *bp;
{ if(bp) return(bp->dkpixmap); return(None);}
Pixmap getBrightPixmap(bp) EZ_Bitmap *bp;
{ if(bp) return(bp->btpixmap); return(None);}
Pixmap getHighlightPixmap(bp) EZ_Bitmap *bp;
{ if(bp) return(bp->htpixmap); return(None);}

void EZ_DumpWidgetHashTable()
{
  printf("========Mapped Widget Hash Tabble=========\n");
  {
    register int i;

    EZ_MappedWidgetHTEntry  *tmp;
    for(i = 0; i < EZ_WIDGET_HASH_TABLE_SIZE; i++)
      {
	tmp = EZ_MappedWidgetsHashTable[i].next;
	if(tmp)
	  {
	    printf("%d: ", i);
	    while(tmp)
	      {
		printf(" --> %lx == %lx ==> %s",(unsigned long)tmp->widget,
		       (EZ_WidgetWindow(tmp->widget)),
		       EZ_WidgetDefaultClassNames[(int)EZ_WidgetType(tmp->widget)]);
		tmp = tmp->next;
	      }
	    printf("\n");
	  }
      }
  }
  printf("======All Widgets Hash Tabble========\n");
  {
    register int i;
    EZ_AllWidgetHTEntry  *tmp;

    for(i = 0; i < EZ_WIDGET_HASH_TABLE_SIZE; i++)
      {
	tmp = EZ_AllWidgetsHashTable[i].next;
	if(tmp)
	  {
	    printf("%d: ", i);
	    while(tmp)
	      {
		printf(" --> %lx=>%s", (unsigned long)tmp->widget,
		       EZ_WidgetDefaultClassNames[(int)EZ_WidgetType(tmp->widget)]);
		tmp = tmp->next;
	      }
	    printf("\n");
	  }
      }
  }
  printf("======= BitmapHashTable ========\n");
  {
    int i, total = 0;
    EZ_BitmapHTEntry *tmp;
  
    for(i = 0; i < EZ_PRIME+4; i++)
      {
	tmp = EZ_BitmapHashTable[i].next;
	while(tmp != (EZ_BitmapHTEntry *)NULL)
	  {
	    if(i < EZ_PRIME)     printf("%s ", tmp->name);
	    else    printf("%lx ", (unsigned long)tmp->name);
	    printf(" Ref=%d Special=%d, size=[%d %d]\n", 
                   tmp->ref,tmp->special, tmp->width, tmp->height);
            total += tmp->width * tmp->height;
	    tmp = tmp->next;
	  }
      }
    printf("Total pixmap size = %d\n", total);
  }
}
#endif
/*************************************************************************************/
#undef  EZ_PRIME
#undef _EZ_WIDGET_HASH_C_

