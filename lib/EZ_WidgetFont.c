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
/***********************************************************************
 ***                                                                 ***
 ***  Functions that deal with XFonts.  Loaded fonts will never      ***
 ***  be freed. The XFontStruct is kept in a dynamic array           ***
 ***  EZ_FontList. However, only one copy of a font is kept. I.e.,   ***
 ***  If you call EZ_LoadXFont(name) twice with the same name, it    ***
 ***  will only store the corresponding font once.                   ***
 ***                                                                 ***
 ***********************************************************************/
#define _EZ_WIDGET_FONT_C_

#include "EZ_Widget.h"

/***********************************************************************
 *
 * Functions implemented in this file:
 */
XFontStruct    *EZ_GetWidgetFont MY_ANSIARGS((EZ_Widget *widget));
XFontStruct    *EZ_GetFontFromId MY_ANSIARGS((int id));
XFontStruct    *EZ_GetFontFromXFontId MY_ANSIARGS((Font font));
XFontStruct    *EZ_GetFontFromName MY_ANSIARGS((char *fname));
void           EZ_InitFontList MY_ANSIARGS((void));
int            EZ_LoadXFont MY_ANSIARGS((char *fname));
void           EZ_SetXFont MY_ANSIARGS((XFontStruct **ptr, int id));
char           *EZ_GetFontNameFromId MY_ANSIARGS((int id));
char           *EZ_GetFontName MY_ANSIARGS((XFontStruct *fptr));
int            EZ_GetFontID MY_ANSIARGS((XFontStruct *fptr));
void           EZ_DestroyFontList MY_ANSIARGS((void));
void           EZ_SetDefaultLabelFont MY_ANSIARGS((char *fname));
void           EZ_SetDefaultEntryTextFont MY_ANSIARGS((char *fname));
void           EZ_SetDefaultMenuTitleFont MY_ANSIARGS((char *fname));
int            EZ_FontWeightIsBold MY_ANSIARGS((char *fname));
/***********************************************************************/
static void    EZ_AlterDefaultFont MY_ANSIARGS((int id, char *fname));
/***********************************************************************
 *  Default fonts.
 */
#define EZ_DEFAULT_FONT_BEGIN      EZ_DEFAULT_FONT
#define EZ_DEFAULT_FONT_END        EZ_HUGE_FONT
/**********************************************************************
 * 
 * This is the data structure used to store Fonts. The first entry
 * is the defaylt font of the DefaultGC.
 */
typedef struct {
  int          font_id;           /* the font identifier, assigned */
  XFontStruct  *xfont_struct;     /* ptr to the XFontStruct        */
  char         *font_name;        /* name of the font              */
} EZ_Font;

/*******************************************************************
 *
 *  Local Variable, keep track of loaded fonts.
 *  We save all loaded fonts in EZ_FontList.
 */
static EZ_Font       *EZ_FontList = (EZ_Font *)NULL;
static int           EZ_NumFonts = 0;
static char          *EZ_DefaultFontNames[] =
{
  "9x15",
  "-Adobe-Helvetica-Medium-R-Normal--*-120-*-*-*-*-*-*",
  "-*-Courier-Medium-R-Normal--*-120-*-*-*-*-*-*",
  "-Adobe-Helvetica-Bold-R-Normal--*-120-*-*-*-*-*-*",
  "-Adobe-Times-Bold-I-Normal--*-120-*-*-*-*-*-*",
  "5x7",
  "7x14",
  "12x24",
  NULL
};

/*******************************************************************
 *  Set default fonts
 */
void EZ_SetDefaultLabelFont(name)  char *name;
{
  EZ_AlterDefaultFont(EZ_BOLD_FONT, name);
  if(EZ_FontWeightIsBold(name)) EZ_LabelFontIsBold = 1;
  else EZ_LabelFontIsBold = 0;
}
void EZ_SetDefaultEntryTextFont(name)   char *name;
{ EZ_AlterDefaultFont(EZ_THIN_FONT, name);}
void EZ_SetDefaultMenuTitleFont(name)  char *name;
{ EZ_AlterDefaultFont(EZ_ITALIC_FONT, name);}

void EZ_SetGlobalLabelFont(name)  char *name;
{ EZ_SetDefaultLabelFont(name);}
void EZ_SetGlobalEntryTextFont(name)   char *name;
{ EZ_AlterDefaultFont(EZ_THIN_FONT, name);}
void EZ_SetGlobalMenuTitleFont(name)  char *name;
{ EZ_AlterDefaultFont(EZ_ITALIC_FONT, name);}

/*******************************************************************
 *
 *  return 1 if the weight spec of name if bold.
 */
int EZ_FontWeightIsBold(name) 
     char *name;
{
  int i=0, count=0, len = strlen(name);
  char tmp[256];
  while(i < len) {tmp[i] = tolower(name[i]); i++;}
  i = 0;
  while(i < len && count < 3)
    {
      if(tmp[i] == '-') count++;
      i++;
    }
  if(i < len && !strncmp(tmp+i,"bold",4))
    return(1);
  return(0);
}
/*******************************************************************
 *
 *  Initialize EZ_FontList. Put the default font of the default
 *  GC in the first entry of EZ_FontList.  
 */
 
void  EZ_InitFontList()
{
  register int  i;  

  if(EZ_FontList == (EZ_Font *)NULL)
    {
      EZ_NumFonts = EZ_DEFAULT_FONT_END + EZ_DEFAULT_FONT_BEGIN + 1;
      EZ_FontList = (EZ_Font *)my_malloc( EZ_NumFonts * sizeof(EZ_Font), _FONT_TABLE_ENTRY_);
      if(!EZ_FontList) EZ_OutOfMemory("EZ_InitFontList");
      for(i = 0; i < EZ_NumFonts; i++)
	{
	  EZ_FontList[i].font_id = -1;
	  EZ_FontList[i].xfont_struct = (XFontStruct *)NULL;
	  EZ_FontList[i].font_name = (char *)NULL;	  
	}
      /*------------------------------------
       * Our default fonts for widgets.
       *-----------------------------------*/
      for(i = EZ_DEFAULT_FONT_BEGIN; i <= EZ_DEFAULT_FONT_END; i++)
	EZ_LoadXFont(EZ_DefaultFontNames[i]);
    }
}
void  EZ_DestroyFontList()
{
  register int  i;  

  if(EZ_FontList)
    {
      for(i = 0; i < EZ_NumFonts && EZ_FontList[i].font_id != -1; i++)
	{
	  XFreeFont(EZ_Display,EZ_FontList[i].xfont_struct);
	  (void)my_free(EZ_FontList[i].font_name);
	}
      (void)my_free((char *)EZ_FontList);
      EZ_FontList = NULL;
    }
  EZ_NumFonts = 0;
}
/*******************************************************************
 *
 *  load a font.
 */
int  EZ_LoadXFont(font_name) 
     char    *font_name;
{
  XFontStruct   *xfont_struct;
  register int  i;

  /*----------------------------------------------
   * If EZ_FontList is NULL, initialize it first.
   *---------------------------------------------*/
  if(EZ_FontList == (EZ_Font *)NULL)   EZ_InitFontList();

  /*----------------------------------------------
   * First search to see if this font has already
   * been loaded. If so, just return the id.
   * Should really do a regular expression matching !
   *---------------------------------------------*/
  for(i = 0; i < EZ_NumFonts && EZ_FontList[i].font_id != -1; i++)
    if( EZ_FontList[i].font_name &&  !strcmp(EZ_FontList[i].font_name, font_name))
      return(EZ_FontList[i].font_id);
  /*----------------------------------------------
   * this is a new font, have to load it.
   *---------------------------------------------*/
  if(font_name == NULL) return(0);
  if( (xfont_struct = XLoadQueryFont(EZ_DisplayForWidgets, font_name))  == (XFontStruct *)NULL)
    {
      (void) fprintf(stderr, "Font %s does not exist\n", font_name);
      return(0);
    }
  if(i == EZ_NumFonts)
    {
      EZ_FontList = (EZ_Font *)my_realloc(EZ_FontList,
					  (EZ_NumFonts + EZ_NumFonts)*
					  sizeof(EZ_Font),  _FONT_TABLE_ENTRY_);
      if(!EZ_FontList) EZ_OutOfMemory("EZ_LoadXFont");
      
      for(i = EZ_NumFonts; i < EZ_NumFonts + EZ_NumFonts; i++)
	{
	  EZ_FontList[i].font_id = -1;
	  EZ_FontList[i].xfont_struct = (XFontStruct *)NULL;
	  EZ_FontList[i].font_name = (char *)NULL;	  
	}
      i = EZ_NumFonts;
      EZ_NumFonts += EZ_NumFonts;
    }
  EZ_FontList[i].font_id = i;
  EZ_FontList[i].xfont_struct = xfont_struct;

  EZ_FontList[i].font_name = (char *)my_malloc( strlen(font_name) +1, _FONT_NAME_);
  if(!(EZ_FontList[i].font_name)) EZ_OutOfMemory("EZ_LoadXFont");
  (void) strcpy(EZ_FontList[i].font_name, font_name);

  return(i);
}
/*****************************************************************/
static void  EZ_AlterDefaultFont(id, name)
     int id; char *name; /* reset font id to the font given by name */
{
  if(id >= EZ_DEFAULT_FONT_BEGIN   && id <= EZ_DEFAULT_FONT_END )
    {
      XFontStruct   *old_xfont_struct, *xfont_struct;
      if(name == NULL) return;
      if( (xfont_struct = XLoadQueryFont(EZ_Display, name))  == (XFontStruct *)NULL)
	{
	  (void) fprintf(stderr, "Font %s does not exist\n", name);
	  return;
	}    
      old_xfont_struct = EZ_FontList[id].xfont_struct;
      if(old_xfont_struct == xfont_struct) return;
      EZ_FontList[id].xfont_struct = xfont_struct;
      (void)my_free(EZ_FontList[id].font_name);
      EZ_FontList[id].font_name = (char *)my_malloc((strlen(name)+1)*sizeof(char),_FONT_NAME_);
      if(!(EZ_FontList[id].font_name)) EZ_OutOfMemory("EZ_AlterDefaultFont");
      (void) strcpy(EZ_FontList[id].font_name, name);      

      /* update the default GC */
      { 
	unsigned long valuemask = GCFont;
	XGCValues values;
	values.font = xfont_struct->fid;
	XChangeGC(EZ_Display, EZ_GCList[id+DARKRED], valuemask, &values);
      }

      /*
       * in case this is called not right after initialization.
       * go through the whole widget hash table, change all
       * widget with the old_xfont_struct to xfont_struct.
       */
      EZ_FreezeDisplay();
      EZ_SetItemDefaultFontAll(old_xfont_struct, xfont_struct);
      EZ_ThawDisplay();
      {
	extern EZ_AllWidgetHTEntry EZ_AllWidgetsHashTable[];
	EZ_AllWidgetHTEntry  *tmp;  
	EZ_Widget *toplevels[256];
	int       ntoplevels = 0;
	int i;
	for(i = 0; i < EZ_WIDGET_HASH_TABLE_SIZE; i++)
	  {
	    tmp = EZ_AllWidgetsHashTable[i].next;
	    while(tmp)
	      {
		EZ_Widget *widget = tmp->widget;
		if(EZ_WidgetFont(widget) == old_xfont_struct)
		  {
		    EZ_Widget *parent = NULL;
		    EZ_WidgetFont(widget) =  xfont_struct;
		    while(widget)
		      {
			EZ_ClearWidgetSizeComputedFlag(widget);
			parent = widget;
			widget = EZ_WidgetParent(widget);
		      }
		    {
		      int j;
		      for(j = 0; j < ntoplevels; j++)
			if(toplevels[j] == parent) break;
		      if(j == ntoplevels)
			toplevels[ntoplevels++] = parent;
		    }
		  }
		tmp = tmp->next;
	      }
	  }
	for(i = 0; i < ntoplevels; i++)
	  {
	    EZ_Widget *widget = toplevels[i];
	    if(EZ_GetWidgetSizeComputedFlag(widget) == 0 &&
	       EZ_WidgetMapped(widget))
	      EZ_DisplayWidget(widget);
	  }
      }
    }
}
/*****************************************************************
 *
 * Set font to the font with record id.
 */
void EZ_SetXFont(fontstructptr, font_id)
     XFontStruct **fontstructptr;
     int         font_id;
{
  /*----------------------------------------------
   * If EZ_FontList is NULL, initialize it first.
   *---------------------------------------------*/
  if(EZ_FontList == (EZ_Font *)NULL)
    EZ_InitFontList();

  if(font_id < 0 || font_id >= EZ_NumFonts ||
     EZ_FontList[font_id].font_id == -1)
    {
      (void) fprintf(stderr, "font %d has not been loaded\n", font_id);
      font_id = 0;
    }
  *fontstructptr = EZ_FontList[font_id].xfont_struct;
}

/*****************************************************************
 *
 * return the ptr to XFontStruct for font with record id.
 */
XFontStruct  *EZ_GetFontFromId(font_id)
     int         font_id;
{
  /*----------------------------------------------
   * If EZ_FontList is NULL, initialize it first.
   *---------------------------------------------*/
  if(EZ_FontList == (EZ_Font *)NULL)
    EZ_InitFontList();

  if(font_id < 0 || font_id >= EZ_NumFonts ||
     EZ_FontList[font_id].font_id == -1)
    {
      (void) fprintf(stderr, "font %d has not been loaded\n", font_id);
      font_id = 0;
    }
  return(EZ_FontList[font_id].xfont_struct);
}


XFontStruct  *EZ_GetFontFromXFontId(font)
     Font font;
{
  int i;
  if(EZ_FontList == (EZ_Font *)NULL) EZ_InitFontList();
  
  for(i = 0; i < EZ_NumFonts; i++)
    {
      XFontStruct *fs =  EZ_FontList[i].xfont_struct;
      if(fs && fs->fid == font) return(fs);
    }
  return(EZ_FontList[0].xfont_struct);
}
/*****************************************************************
 *
 * return the ptr to XFontStruct for font with name font_name.
 */

XFontStruct  *EZ_GetFontFromName(font_name)
     char         *font_name;
{
  int id = EZ_LoadXFont(font_name);
  return(EZ_FontList[id].xfont_struct);
}
/************************************************************/
char *EZ_GetFontNameFromId(id)
     int id;
{
  if(id >= 0 && id < EZ_NumFonts)
    {
      if(EZ_FontList[id].font_id != -1) 
	return(EZ_FontList[id].font_name);
    }
  return(EZ_FontList[0].font_name);
}
/************************************************************/
char *EZ_GetFontName(xfont)
     XFontStruct *xfont;
{
  if(xfont)
    {
      int i;
      for(i = 0; i < EZ_NumFonts; i++) 
	{
	  if(EZ_FontList[i].xfont_struct == xfont) 
	    return(EZ_FontList[i].font_name);
	}
    }
  return(EZ_FontList[0].font_name);
}
int EZ_GetFontID(xfont)
     XFontStruct *xfont;
{
  if(xfont)
    {
      int i;
      for(i = 0; i < EZ_NumFonts; i++) 
	{
	  if(EZ_FontList[i].xfont_struct == xfont) 
            return(i);
	}
    }
  return(0);
}
/************************************************************/
XFontStruct *EZ_GetWidgetFont(widget) EZ_Widget *widget;
{
  if(widget) return(EZ_WidgetFont(widget));
  else   return(EZ_FontList[0].xfont_struct);
}
/************************************************************/
#undef _EZ_WIDGET_FONT_C_
