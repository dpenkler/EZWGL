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
/********************************************************************
 *
 *  Misc functions dealing with TextWidget
 *
 ********************************************************************/
#define _EZ_TEXT_C_

#include "EZ_Widget.h"
#include "EZ_WidgetConfig.h"

#define LINE_GROW_UNIT   16
#define lineWrapWidth 6
#define lineWrapHeight 9
static unsigned char lineWrapBits[] = 
{
  0x1e, 0x3e, 0x30, 0x30, 0x39, 0x1f, 0x0f, 0x0f, 0x1f,
};
static Pixmap lineWrapBitmap;
/********************************************************************
 *
 *  public functions
 */
void       EZ_ITextLineDimension MY_ANSIARGS((
					      EZ_Widget *widget,
					      ITextLine *line,
					      int       start, int length,
					      int       *w_ret, int *h_ret, int *a_ret));
void       EZ_ITextLineDimensionWork MY_ANSIARGS((
					      XFontStruct *defaultFont,
					      int       spacing,
					      ITextLine *line,
					      int       start, int length,
					      int       *w_ret, int *h_ret, int *a_ret));

void       EZ_ITextRenderText MY_ANSIARGS((
					   EZ_Widget    *widget,
					   ITextLine    *line,
					   int          x, int y,
					   int maxheight, int  maxAscent,
					   Drawable     drawable,
					   int lineNum, int CurX, int CurY));

void       EZ_ITextRenderTextWork MY_ANSIARGS((
					   XFontStruct  *dfont,
					   unsigned long dfg,
					   unsigned long dbg,
					   int           spacing,
					   int           editable,
					   int           xoffset,
					   ITextLine    *line,
					   int          x, int y,
					   int maxheight, int  maxAscent,
					   Drawable     drawable,
					   int lineNum, int CurX, int CurY));


TextProp   *EZ_GetTextPropertyFromID MY_ANSIARGS((
						  int font_id, 
						  int fg_set, int bg_set, int underline,
						  unsigned long fg, unsigned long bg,
						  EZ_Bitmap *bitmap));

TextProp   *EZ_CopyTextPropIgnoreSpecial MY_ANSIARGS((TextProp *tprop));

void       EZ_InitializeTxetPropertyHashTable MY_ANSIARGS((void));
void       EZ_TPHTCollectGarbage MY_ANSIARGS((void));
void       EZ_ClearTextPropHT MY_ANSIARGS((void));
void       EZ_CountNewLines MY_ANSIARGS((char *str, int  *len_ret, int *nNewlines));
void       EZ_GrowITextLine  MY_ANSIARGS((ITextLine **tline, int       increment));
void       EZ_FreeTextLine  MY_ANSIARGS((ITextLine *tline));
					  
void       EZ_FreeTextProperty MY_ANSIARGS((TextProp *tprop));

void       EZ_GrowITextKillBuffer MY_ANSIARGS((ITextLine *buf, int len));


ITextLine  *EZ_GetWrappedTextLine MY_ANSIARGS((void));
void       EZ_BreakITextLines  MY_ANSIARGS((EZ_Widget *itxt, int flag));
ITextLine *EZ_WrapThisTextLine MY_ANSIARGS((EZ_Widget *itxt, ITextLine *lineIn, 
                                            int hsize, int wword, int *c_ret));
/********************************************************************/
static ITextLine *WrappedTLinePool=NULL;
/********************************************************************
 *
 * Private functions
 */
static int      EZ_TextPropHash MY_ANSIARGS((TextProp *tprop));
static int      EZ_CompareTextProp MY_ANSIARGS((TextProp *tprop, EZ_TextProp *p2));
static TextProp *EZ_AllocateCopyTextProp MY_ANSIARGS((TextProp *tprop));
static void     EZ_MarkUsedTextProperties MY_ANSIARGS((EZ_Widget *widget));
/**********************************************************************
 *
 * Global Variables. This variable is used in all EZ widgets that
 * use selection.
 */
ITextLine TextWidgetKillBuffer;
ITextLine TWKBufferA;
/********************************************************************
 *
 * Figure out the height and width of a piece of text in line
 */
void EZ_ITextLineDimension(widget, line, start, length, w_ret, h_ret, a_ret)
     EZ_Widget *widget;
     ITextLine *line;
     int       start, length;
     int       *w_ret, *h_ret, *a_ret;
{
  EZ_ITextLineDimensionWork(EZ_ITextFont(widget),(int)EZ_ITextVSpacing(widget),
			    line, start, length, w_ret, h_ret, a_ret);
}

void EZ_ITextLineDimensionWork(defaultfont, spacing, line, start, length, w_ret, h_ret, a_ret)
     XFontStruct *defaultfont;
     ITextLine *line;
     int       start, length, spacing;
     int       *w_ret, *h_ret, *a_ret;
{
  TextProp **prop, *cprop;
  char     *str;
  int      len, i,bgn, count, itmp;
  int      width = 0, height = 0, ascent = 0, theight;
  int      font_id;
  XFontStruct *xfont = NULL;
  EZ_Bitmap *bitmap;

  if(line == (ITextLine *)NULL || length <= 0 ) { *w_ret = 0, *h_ret = 0; *a_ret = 0; return;}
  len = line->count? line->count: line->length;
  if(start >= len) { *w_ret = 0, *h_ret = 0; *a_ret = 0; return;}
  if(start + length > len) length = len - start;

  str  = line->data + start;
  prop = line->prop + start;

  cprop =  *prop;
  if(cprop) 
    {
      font_id = (int) EZ_TextPropertyFont(cprop);
      bitmap =  EZ_TextPropertyBitmap(cprop);
    }
  else { font_id = -1; bitmap = NULL;}
  if(font_id < 0) xfont = defaultfont;
  else xfont = EZ_GetFontFromId(font_id);
  
  i = 0;
  bgn = 0;
  while(1)
    {
      count = 0;
      while( i < length )
	{
	  if(prop[i] != cprop) break;
	  i++;
	  count++;
	}
      if(count)
	{
	  if(bitmap) 
	    {
	      width += bitmap->width; 
	      theight = bitmap->height;
	      itmp = xfont->max_bounds.ascent - xfont->max_bounds.descent;
	      itmp = (theight + itmp)>> 1;
	      ascent = MAXV(itmp, ascent);
	    }
	  else
	    {
	      width += XTextWidth(xfont, str+bgn, count);
	      theight = xfont->max_bounds.ascent + xfont->max_bounds.descent;
	      ascent = MAXV(xfont->max_bounds.ascent, ascent);
	    }
	  height = MAXV(theight, height);
	}
      if(i < length)
	{
	  bgn += count;
	  cprop = prop[i];
	  if(cprop)
	    {
	      font_id = (int) EZ_TextPropertyFont(cprop);
	      bitmap =  EZ_TextPropertyBitmap(cprop);
	    }
	  else { font_id = -1; bitmap = NULL; }
	  if(font_id < 0) xfont = defaultfont;
	  else xfont = EZ_GetFontFromId(font_id);	      
	}
      else break;
    }
  *w_ret = width;
  *h_ret = height + spacing;
  *a_ret = ascent;
}

void EZ_ITextRenderText(widget, line, x, y, maxheight,maxAscent, drawable, lineNum, CurX, CurY)
     EZ_Widget    *widget;
     ITextLine    *line;
     int          x, y, maxheight, maxAscent, lineNum, CurX, CurY;
     Drawable     drawable;
{
  XFontStruct *defaultfont = EZ_ITextFont(widget);
  unsigned long defaultbg, defaultfg = EZ_ITextForeground(widget);
  int spacing = EZ_ITextVSpacing(widget);
  int editable = EZ_ITextEditable(widget);
  int xoffset = EZ_WidgetWidth(widget)-EZ_WidgetBorderWidth(widget)-8;
  if(EZ_ITextBackground(widget))  defaultbg = EZ_ITextBackground(widget);
  else if(EZ_WidgetParentBG(widget) && *(EZ_WidgetParentBG(widget)))
    defaultbg = *(EZ_WidgetParentBG(widget));
  else defaultbg = EZ_ColorArray[EZ_NORMAL_GRAY1];
  EZ_ITextRenderTextWork(defaultfont, defaultfg, defaultbg, spacing, editable,xoffset,  
			 line, x, y, maxheight,maxAscent, drawable, lineNum, CurX, CurY);
}

void EZ_ITextRenderTextWork(defaultfont, dfg, dbg, spacing, editable,xoffset,
			    line, x, y, maxheight,maxAscent, drawable, lineNum, CurX, CurY)
     XFontStruct  *defaultfont;
     unsigned long dfg, dbg;
     ITextLine    *line;
     int          spacing, editable, xoffset;
     int          x, y, maxheight, maxAscent, lineNum, CurX, CurY;
     Drawable     drawable;
{
  TextProp **prop, *cprop;
  char     *str;
  int      len, i,bgn, count;
  int      width = 0, xx = x, yy = y + maxAscent;
  unsigned long bg, fg, defaultfg=dfg, defaultbg=dbg;
  XFontStruct   *xfont = NULL;
  int           font_id, fg_set, bg_set, underline;
  GC           gc;
  EZ_Bitmap    *bitmap;
  if(line ==  (ITextLine *)NULL) return;

  len = line->count? line->count: line->length;
  str =  line->data;
  prop = line->prop;

  cprop =  *prop;
  if(cprop) 
    { 
      font_id = (int) EZ_TextPropertyFont(cprop);
      fg_set = (int) EZ_TextPropertyFGSet(cprop);
      bg_set = (int) EZ_TextPropertyBGSet(cprop);
      underline = (int) EZ_TextPropertyUnderLine(cprop);
      bitmap = EZ_TextPropertyBitmap(cprop);
    }
  else 
    {
      font_id = -1;
      bg_set = 0;
      fg_set = 0;
      underline = 0;
      bitmap = NULL;
    }
  if(font_id < 0) xfont = defaultfont;
  else xfont = EZ_GetFontFromId(font_id);
  if(fg_set == 0)    fg = defaultfg;
  else fg = EZ_TextPropertyForeground(cprop);
  if(bg_set == 0)    bg = defaultbg;
  else bg = EZ_TextPropertyBackground(cprop);

  gc = EZ_WRITABLEGC;
  bgn = 0; 
  i = 0;
  while(1)
    {
      count = 0;
      while( i < len )
	{
	  if(prop[i] != cprop) break;
	  i++;
	  count++;
	}
      if(count)
	{
	  if(bitmap)
	    {
	      int tyy = y + ((maxheight - bitmap->height - spacing)>> 1);
	      width = (int) bitmap->width;
	      XSetForeground(EZ_Display, gc, fg);
	      XSetBackground(EZ_Display, gc, bg );
	      if(bitmap->shape != (Pixmap)NULL)
		{
		  XSetClipOrigin(EZ_Display, gc, xx, tyy);
		  XSetClipMask(EZ_Display,gc, bitmap->shape);
		}
	      EZ_RenderPixmapLabelWithGC(bitmap, drawable, gc, xx, tyy);
	      if(bitmap->shape != (Pixmap)NULL)	      
		XSetClipMask(EZ_Display,gc, None);
	    }
	  else
	    {
              width = XTextWidth(xfont, str+bgn, count);
              XSetFont(EZ_Display, gc, xfont->fid);
              XSetForeground(EZ_Display, gc, fg);
              if(bg_set != 0)
                {
                  XSetBackground(EZ_Display, gc, bg);
                  XDrawImageString(EZ_Display,drawable, gc, xx,yy,str+bgn, count);
                }
              else  XDrawString(EZ_Display,drawable, gc, xx, yy, str+bgn, count);
              if(underline)
                {
                  int uy = xfont->max_bounds.descent + yy -1;
                  XDrawLine(EZ_Display,drawable, gc, xx, uy, xx+width, uy);
                }
	    }
          
	  if(lineNum == CurY &&  editable != 0)  /* insert a cursor */
	    {
	      if(CurX >= bgn &&  CurX < i)
		{
		  int tmpx, tmpy, tmpy1,tmp_width;
		  int tmp_str_len = CurX - bgn;
		  EZ_ITextLineDimensionWork(defaultfont,spacing, line, bgn, tmp_str_len, &tmp_width, &tmpx, &tmpy);
		  tmpx = xx + tmp_width;
		  tmpy = y + maxheight;
                  tmpy1= tmpy - spacing;
		  XDrawLine(EZ_Display, drawable, EZ_DARKREDGC,  tmpx,y, tmpx, tmpy1);
                  y++, tmpy1--;
		  XDrawLine(EZ_Display, drawable, EZ_DARKREDGC,  tmpx-2,y, tmpx+2, y);
		  XDrawLine(EZ_Display, drawable, EZ_DARKREDGC,  tmpx-2,tmpy1, tmpx+2, tmpy1);
		}
	    }
	  xx += width;
	}
      if(i < len)
	{
	  bgn += count;
	  cprop = prop[i];
	  if(cprop) 
	    { 
	      font_id = (int) EZ_TextPropertyFont(cprop);
	      fg_set = (int) EZ_TextPropertyFGSet(cprop);
	      bg_set = (int) EZ_TextPropertyBGSet(cprop);
	      underline = (int) EZ_TextPropertyUnderLine(cprop);
	      bitmap = EZ_TextPropertyBitmap(cprop);
	    }
	  else 
	    {
	      font_id = -1;
	      bg_set = 0;
	      fg_set = 0;
	      underline = 0;
	      bitmap = NULL;
	    }
	  if(font_id < 0) xfont = defaultfont;
	  else xfont = EZ_GetFontFromId(font_id);
	  if(fg_set == 0)    fg = defaultfg;
	  else fg = EZ_TextPropertyForeground(cprop);
	  if(bg_set == 0)    bg = defaultbg;
	  else bg = EZ_TextPropertyBackground(cprop);
	}
      else break;      
    }
  if(line->count)
    {
      yy -= 10;
      xx = xoffset;
      XSetClipMask(EZ_Display, EZ_DARKREDGC, lineWrapBitmap);
      XSetClipOrigin(EZ_Display, EZ_DARKREDGC,xx,yy);      
      XCopyPlane(EZ_Display, lineWrapBitmap, drawable, EZ_DARKREDGC, 0,0, 6,9, xx,yy, 1);      
      XSetClipMask(EZ_Display, EZ_DARKREDGC, None);
    }
}
/*******************************************************************************
 *
 *          Text Properties.
 *
 ******************************************************************************/
#define TEXT_PROP_HASH_SIZE   EZ_WIDGET_HASH_TABLE_SIZE

typedef struct EZ_TPHTab_ {
  TextProp          *textprop;   /* ptr to the property           */
  struct EZ_TPHTab_ *next;       /* next entry                    */
} EZ_TPHTab;

static EZ_TPHTab   TextPropHashTable[TEXT_PROP_HASH_SIZE];

/**********************************************************/

static int EZ_TextPropHash(tprop)
     TextProp   *tprop;
{
  unsigned long value, bitmap, fg = 0, bg = 0;
  unsigned long font_id, underline;
  
  font_id = (unsigned long) EZ_TextPropertyFont(tprop);

  if(EZ_TextPropertyFGSet(tprop))  fg = ( EZ_TextPropertyForeground(tprop) >> 1);
  if(EZ_TextPropertyBGSet(tprop))  bg = ( EZ_TextPropertyBackground(tprop) >> 1);
  underline = (unsigned long) EZ_TextPropertyUnderLine(tprop);
  bitmap = ( (unsigned long) (EZ_TextPropertyBitmap(tprop)) >> 1);
  value = font_id + fg + bg + underline + bitmap;
  
  return( value % TEXT_PROP_HASH_SIZE);
}
/**********************************************************/

static int EZ_CompareTextProp(prop1, prop2)
     TextProp   *prop1, *prop2;
{
  if(prop1)
    {
      if(prop2)
	{
	  return ((EZ_TextPropertyFont(prop1) == EZ_TextPropertyFont(prop2)) &&
		  (EZ_TextPropertyFGSet(prop1) == EZ_TextPropertyFGSet(prop2)) &&
		  (EZ_TextPropertyBGSet(prop1) == EZ_TextPropertyBGSet(prop2)) &&
		  (EZ_TextPropertyForeground(prop1) == EZ_TextPropertyForeground(prop2)) &&		  
		  (EZ_TextPropertyBackground(prop1) == EZ_TextPropertyBackground(prop2)) &&		  
		  (EZ_TextPropertyUnderLine(prop1) == EZ_TextPropertyUnderLine(prop2)) &&
		  (EZ_TextPropertyBitmap(prop1) == EZ_TextPropertyBitmap(prop2)));
	}
      else  return(0);
    }
  else  return (prop2 == (TextProp *)NULL);
}
/**********************************************************/

void EZ_InitializeTxetPropertyHashTable()
{
  int i;
  for(i = 0; i < TEXT_PROP_HASH_SIZE; i++)
    TextPropHashTable[i].next = (EZ_TPHTab *)NULL;

  TextWidgetKillBuffer.width = 0;   /* used to mark whether the killbuffer has text property */
  TextWidgetKillBuffer.length = 0;
  TextWidgetKillBuffer.space = 0;
  TextWidgetKillBuffer.data = (char *)NULL;
  TextWidgetKillBuffer.prop = (TextProp **)NULL;  
  TWKBufferA.width = 0;   /* used to mark whether the killbuffer has text property */
  TWKBufferA.length = 0;
  TWKBufferA.space = 0;
  TWKBufferA.data = (char *)NULL;
  TWKBufferA.prop = (TextProp **)NULL;  
  lineWrapBitmap = XCreateBitmapFromData(EZ_Display, RootWindow(EZ_Display, EZ_ScreenNum),
                                         lineWrapBits, lineWrapWidth, lineWrapHeight);
}

/**********************************************************/
static TextProp *EZ_AllocateCopyTextProp(tprop)
     TextProp *tprop;
{
  TextProp *tmp;
  if(tprop)
    {
      tmp = (TextProp *)my_malloc(sizeof(TextProp), _TEXT_PROP_);
      if(!tmp) EZ_OutOfMemory("EZ_AllocateCopyTextProp");
      EZ_TextPropertyFont(tmp) = EZ_TextPropertyFont(tprop);
      EZ_TextPropertyForeground(tmp) = EZ_TextPropertyForeground(tprop);
      EZ_TextPropertyBackground(tmp) = EZ_TextPropertyBackground(tprop);
      EZ_TextPropertyFGSet(tmp) = EZ_TextPropertyFGSet(tprop);
      EZ_TextPropertyBGSet(tmp) = EZ_TextPropertyBGSet(tprop);
      EZ_TextPropertyUnderLine(tmp) = EZ_TextPropertyUnderLine(tprop);
      EZ_TextPropertyBitmap(tmp) = EZ_TextPropertyBitmap(tprop);
      return(tmp);
    }
  return(NULL);
}

/********************************************************************************
 *
 *  Given a textproperty specification data, find
 *  or create the matching textproperty entry.
 */
TextProp   *EZ_GetTextPropertyFromID(font_id, fg_set, bg_set, underline,
				     fg, bg, bitmap)
     int font_id, fg_set, bg_set, underline;
     unsigned long fg, bg;
     EZ_Bitmap *bitmap;
{
  TextProp   textproperty, *tprop;

  if(font_id < 0 && fg_set == 0 && bg_set == 0 && underline == 0  && bitmap == NULL)
    return((TextProp *)NULL);
  else 
    {
      int         hash_value;
      EZ_TPHTab   *entry;
      TextProp    *tmpprop;

      tprop = &textproperty;
      EZ_TextPropertyFont(tprop) = font_id;
      EZ_TextPropertyForeground(tprop) =  fg;
      EZ_TextPropertyBackground(tprop) =  bg;
      EZ_TextPropertyFGSet(tprop) = (fg_set == 0) ? 0: 1;
      EZ_TextPropertyBGSet(tprop) = (bg_set == 0) ? 0: 1;
      EZ_TextPropertyUnderLine(tprop) = (underline == 0) ? 0: 1;
      EZ_TextPropertyBitmap(tprop) = bitmap;

      hash_value = EZ_TextPropHash(tprop);
      entry = TextPropHashTable[hash_value].next;
      while(entry)
	{
	  tmpprop = entry->textprop;
	  if(EZ_CompareTextProp(tprop, tmpprop)) return(tmpprop);
	  entry = entry->next;
	}
      /*
       * property not found. Install a new one.
       */
      entry = (EZ_TPHTab  *)my_malloc( sizeof(EZ_TPHTab), _TP_HT_ENTRY_);
      if(!entry) EZ_OutOfMemory("EZ_GetTextPropertyFromID");
      entry->next = TextPropHashTable[hash_value].next;
      TextPropHashTable[hash_value].next = entry;
      tmpprop = EZ_AllocateCopyTextProp(tprop);
      entry->textprop = tmpprop;
      return(tmpprop);
    }
}

TextProp  *EZ_CopyTextPropIgnoreSpecial(tprop)
     TextProp *tprop;
{
  if(tprop && EZ_TextPropertyBitmap(tprop))
    {
      TextProp *tmp;
      tmp = EZ_GetTextPropertyFromID(EZ_TextPropertyFont(tprop),
				     EZ_TextPropertyFGSet(tprop),
				     EZ_TextPropertyBGSet(tprop),
				     EZ_TextPropertyUnderLine(tprop),
				     EZ_TextPropertyForeground(tprop),
				     EZ_TextPropertyBackground(tprop),
				     NULL);
      return(tmp);
    }
  return(tprop);
}
/***************************************************************
 *
 * Gabarge collection.
 */
extern EZ_MappedWidgetHTEntry    EZ_MappedWidgetsHashTable[];     /* for mapped widgets   */
extern EZ_BitmapHTEntry          EZ_BitmapHashTable[];             /* Beware of the size ! */


void EZ_TPHTCollectGarbage()
{
  EZ_TPHTab   *entry, *prev, *tmp;
  TextProp    *tprop;
  EZ_Widget   *widget;
  EZ_MappedWidgetHTEntry *whtentry;
  EZ_Bitmap   *bitmap;

  int         i;

  /*
   *  Step 1. Mark all text property 0.
   */
  for(i = 0; i < TEXT_PROP_HASH_SIZE; i++)
    {
      entry = TextPropHashTable[i].next;
      while(entry)
	{
	  tprop = entry->textprop;
	  if(EZ_TextPropertyMark(tprop) < 5)
	    EZ_TextPropertyMark(tprop) = 0;
	  entry = entry->next;
	}
    }
  /* and clear all reference to bitmaps */
  for(i = 0; i < EZ_WIDGET_HASH_TABLE_SIZE; i++)  /* the last entry in bitmap HT is special */
    {
      bitmap = EZ_BitmapHashTable[i].next;
      while(bitmap) 
	{
	  bitmap->special = 0;
	  bitmap = bitmap->next;
	}
    }

  /*
   *  Step 2. Mark all text properties that are currently in use.
   */ 
  for(i = 0; i < EZ_WIDGET_HASH_TABLE_SIZE; i++)
    {
      whtentry = EZ_MappedWidgetsHashTable[i].next;
      while(whtentry)
	{
	  widget = whtentry->widget;
	  if(EZ_WidgetType(widget) == EZ_WIDGET_ITEXT)
	    EZ_MarkUsedTextProperties(widget);
	  else if(EZ_WidgetType(widget) == EZ_WIDGET_SPREAD_SHEET)
	    EZ_MarkSSheetUsedTextProperties(widget);
	  whtentry = whtentry->next;
	}
    }
  /*
   * and mark the text properties in the kill buffer
   */
  if(TextWidgetKillBuffer.width != 0)  /* properties in theKill buffer */
    {
      ITextLine *tline = &TextWidgetKillBuffer;
      TextProp *prop, **tprop = tline->prop;
      int i, length = tline->length;
      for(i = 0; i < length; i++)
	{
	  if((prop = tprop[i]) != (TextProp *)NULL)
	    {
	      if(EZ_TextPropertyMark(prop) == 0)
		EZ_TextPropertyMark(prop) = 1;
	      bitmap = EZ_TextPropertyBitmap(prop);
	      if(bitmap) bitmap->special = 1;
	    }
	}
    }
  /*
   *  Step 3. Collect garbage.
   */ 
  for(i = 0; i < TEXT_PROP_HASH_SIZE; i++)
    {
      prev = &(TextPropHashTable[i]);
      entry = prev->next;
      while(entry)
	{
	  tprop = entry->textprop;
	  if(EZ_TextPropertyMark(tprop) == 0)
	    {
	      tmp = entry;
	      entry = prev->next = entry->next;
	      (void)my_free( (char *)tprop);
	      (void)my_free( (char *)tmp);
	    }
	  else
	    {
	      prev = entry;
	      entry = entry->next;
	    }
	}
    }
  /* and de-reference unused bitmaps */
  for(i = 0; i < EZ_WIDGET_HASH_TABLE_SIZE; i++) 
    {
      bitmap = EZ_BitmapHashTable[i].next;
      while(bitmap) 
	{
	  if(bitmap->ref <= 0 && bitmap->special == 0)
	    {
	      EZ_Bitmap *tmp = bitmap;   /* beware. EZ_FreeBitmap alters the list */
	      bitmap = bitmap->next;
	      EZ_FreeBitmap(tmp);
	    }
	  else  bitmap = bitmap->next;
	}
    }  
}

static void EZ_MarkUsedTextProperties(widget)
     EZ_Widget *widget;
{
  int       i, j, length, nlines;
  ITextLine **data, *tline;
  TextProp  *cprop, **tprop;
  EZ_Bitmap *bitmap;

  nlines = EZ_ITextNLines(widget);
  data = EZ_ITextData(widget);
  for(i = 0; i < nlines; i++)
    {
      tline = data[i];
      length = tline->length;
      tprop = tline->prop;
      for(j = 0; j < length; j++)
	{
	  cprop = tprop[j];
	  if(cprop)
	    {
	      if(EZ_TextPropertyMark(cprop) == 0)
		EZ_TextPropertyMark(cprop) = 1;
	      bitmap = EZ_TextPropertyBitmap(cprop);
	      if(bitmap) bitmap->special = 1;
	    }		
	}    
    }
}

/**********************************************************/	      
void  EZ_CountNewLines(str, len_ret, nNewlines)
     char *str;
     int  *len_ret, *nNewlines;
{
  int  length, newlines;
  char *tmp;

  if(str == (char *)NULL) 
    {
      *len_ret = 0;
      *nNewlines = 0;
      return;
    }
  tmp = str;
  length = 0;
  newlines = 0;
  while(*tmp)
    {
      if(*tmp == '\n') newlines++;
      length++;
      tmp++;
    }
  *len_ret = length;
  *nNewlines = newlines;
  return;
}

/**********************************************************
 *
 *  Expand a text line.
 */
void EZ_GrowITextLine(tline, increment)
     ITextLine **tline;
     int       increment;
{
  ITextLine *tmp;
  int       size;

  if(*tline == (ITextLine *)NULL)
    {
      size = MAXV(LINE_GROW_UNIT, increment) + 8;

      *tline = tmp = (ITextLine *)my_malloc(sizeof(ITextLine), _ITEXT_LINE_);
      if(!tline) EZ_OutOfMemory("EZ_GrowITextLine");
      memset(tmp, 0, sizeof(ITextLine));
      tmp->length = 0;
      tmp->space = size;
      tmp->data = (char *)my_malloc(size * sizeof(char), _ITEXT_LINE_D_);
      tmp->prop = (TextProp **)my_malloc(size * sizeof(TextProp *),_ITEXT_LINE_P_);
      if(!tmp->data || ! tmp->prop) EZ_OutOfMemory("EZ_GrowITextLine");
    }
  else
    {
      int clength, cspace;
      tmp = *tline;
      clength = tmp->length + 8;
      cspace = tmp->space;
      size = clength + increment;

      if(size >= cspace)
	{
	  tmp->data = (char *)my_realloc(tmp->data, size * sizeof(char), _ITEXT_LINE_D_);
	  tmp->prop = (TextProp **)my_realloc(tmp->prop, size * sizeof(TextProp *),_ITEXT_LINE_P_);
	  if(!tmp->data || ! tmp->prop) EZ_OutOfMemory("EZ_GrowITextLine");
	  tmp->space = size;
	}
    }
}
/*************************************************************************************/
void EZ_GrowITextKillBuffer(kbuf,length)
     int length;
     ITextLine *kbuf;
{
  length = MAXV(length, 0) + 16;

  if(kbuf->data)
    {
      kbuf->data = 
	(char *)my_realloc(kbuf->data, length * sizeof(char), _KILL_BUF_D_);
      kbuf->prop = 
	(TextProp **)my_realloc(kbuf->prop, length * sizeof(TextProp *),_KILL_BUF_P_);
    }
  else
    {
      kbuf->data = (char *)my_malloc(length * sizeof(char), _KILL_BUF_D_);
      kbuf->prop = (TextProp **)my_malloc(length * sizeof(TextProp *),_KILL_BUF_P_);
    }
  if(!kbuf->data || !kbuf->prop) EZ_OutOfMemory("EZ_GrowITextKillBuffer");
  kbuf->space = length;	
}
/**********************************************************/
void  EZ_FreeTextLine(tline)
     ITextLine *tline;
{
  if(tline)
    {
      if(tline->leader != NULL) /* this is a wrapped line */
        {
          tline->leader = WrappedTLinePool;
          WrappedTLinePool = tline;
        }
      else
        {
          if(tline->data) (void)my_free((char *) tline->data);
          if(tline->prop) (void)my_free((char *) tline->prop);
          (void)my_free((char *) tline);
        }
    }
}
/*******************************************************************************
 *
 *  Given a textproperty specification, find or create the matching textproperty
 *  entry.
 */
TextProp  *EZ_GetTextProperty MY_VARARGS(int,  the_args)
     MY_VARARGS_DECL
{
  int             font_id = -1, fg_set = 0, bg_set = 0, underline = 0, option;
  unsigned long   fg = 0, bg = 0;
  EZ_Bitmap       *bitmap = NULL;
  va_list         ap;
  TextProp        *textProp;
  

  option = MY_VARARGS_START(int, the_args, ap);
  while(option != 0)
    {
      switch(option)
	{
	case EZ_FONT_NAME:
	  {
	    char *fontname = va_arg(ap, char *);
	    if(fontname && *fontname)  font_id = EZ_LoadXFont(fontname);
	  }
	break;
	case EZ_FONT_ID:
	  font_id = va_arg(ap, int);
	  break;
	case EZ_FOREGROUND:
	  {
	    char *color = va_arg(ap, char *);	
	    if(color && *color) { fg = EZ_AllocateColorFromName(color); fg_set = 1;}
	  }
	break;
	case EZ_BACKGROUND:
	  {
	    char *color = va_arg(ap, char *);	
	    if(color && *color) { bg = EZ_AllocateColorFromName(color); bg_set = 1;}
	  }
	break;
	case EZ_UNDERLINE:
	  underline = va_arg(ap, int);
	  break;
	case EZ_PIXMAP_FILE:
	  {
	    char *file = va_arg(ap, char *);
	    bitmap = (file == (char *)NULL? (EZ_Bitmap *)NULL:
		      EZ_GetBitmapFromFile(file, 1));
	  }
	break;
	case EZ_IMAGE_FILE:
	  {
	    char *file = va_arg(ap, char *);
	    bitmap = (file == (char *)NULL? (EZ_Bitmap *)NULL:
		      EZ_GetImageFromFile(file, 1));
	  }
	break;
	case EZ_PIXMAP_DATA:
	  {
	    char **file = va_arg(ap, char **);
	    bitmap = (file == (char **)NULL? (EZ_Bitmap *)NULL:
		      EZ_GetPixmapFromData(file));
	  }
	break;
	case EZ_BITMAP_DATA:
	  {
	    char *data = va_arg(ap, char *);
	    int width = va_arg(ap,  int);
	    int height = va_arg(ap, int);
	    if(width > 0 && height > 0)
	      bitmap = EZ_GetBitmapFromData(data, 
					    (unsigned int)width,
					    (unsigned int)height);
	  }
	break;
	case EZ_X_PIXMAP:
	  {
	    Pixmap pixmap = va_arg(ap,Pixmap);
	    int    x = va_arg(ap,int);
	    int    y = va_arg(ap,int);
	    int width = va_arg(ap,int);
	    int height = va_arg(ap, int);
	    bitmap = EZ_GetPixmapFromPixmap(pixmap,x,y,width,height);
	  }
	break;
	case EZ_LABEL_PIXMAP:
	  {
	    char *tmp =  va_arg(ap, char *);
	    bitmap = (EZ_Bitmap *)tmp;
	    if(bitmap) bitmap->ref += 1;
	  }
	break;
	
	default:
	  break;
	}
      option = va_arg(ap,int);
    }
  textProp = EZ_GetTextPropertyFromID(font_id, fg_set, bg_set, underline,
				      fg, bg, bitmap);
  if(textProp) textProp->mark = 15;
  return(textProp);
}
/**************************************************************************/
void EZ_FreeTextProperty(tprop)
     TextProp *tprop;
{
  if(tprop) tprop->mark = 0;
}
/**************************************************************************/
void EZ_ClearTextKillBuffers()
{
  if(TextWidgetKillBuffer.data)
    {
      (void)my_free(TextWidgetKillBuffer.data);
      (void)my_free(TextWidgetKillBuffer.prop);      
      TextWidgetKillBuffer.data = (char *)NULL;
      TextWidgetKillBuffer.prop = (TextProp **)NULL;
      TextWidgetKillBuffer.width = 0;
      TextWidgetKillBuffer.space = 0;
      TextWidgetKillBuffer.length = 0;
    }
  if(TWKBufferA.data)
    {
      (void)my_free(TWKBufferA.data);
      (void)my_free(TWKBufferA.prop);      
      TWKBufferA.data = (char *)NULL;
      TWKBufferA.prop = (TextProp **)NULL;
      TWKBufferA.width = 0;
      TWKBufferA.space = 0;
      TWKBufferA.length = 0;
    }
}
/**************************************************************************/
ITextLine  *EZ_GetWrappedTextLine()
{
  ITextLine *tmp;
  if(WrappedTLinePool)
    {
      tmp = WrappedTLinePool;
      WrappedTLinePool = tmp->leader;
    }
  else
    {
      tmp = (ITextLine *)my_malloc(sizeof(ITextLine), _ITEXT_LINE_);
      if(!tmp) EZ_OutOfMemory("EZ_GetWrappedTextLine");
    }
  memset(tmp, 0, sizeof(ITextLine));
  return(tmp);
}
/**************************************************************************/
void  EZ_ClearTextPropHT() /* destroy all text properties */
{
  EZ_TPHTab   *entry, *prev, *tmp;
  TextProp    *tprop;
  int         i;

  for(i = 0; i < TEXT_PROP_HASH_SIZE; i++)
    {
      prev = &(TextPropHashTable[i]);
      entry = prev->next;
      while(entry)
	{
	  tprop = entry->textprop;
	  tmp = entry;
	  entry = prev->next = entry->next;
	  (void)my_free( (char *)tprop);
	  (void)my_free( (char *)tmp);
	}
    }
  if(lineWrapBitmap) XFreePixmap(EZ_Display,lineWrapBitmap);
  lineWrapBitmap = None;
  while(WrappedTLinePool)
    {
      ITextLine *tmp = WrappedTLinePool->leader;
      (void)my_free(WrappedTLinePool);
      WrappedTLinePool = tmp;
    }
}
/**************************************************************************/

#ifdef EZ_DEBUG
void EZ_DumpTextPropertyHashTable()
{
  int i;
  EZ_TPHTab   *entry;

  for(i = 0; i < TEXT_PROP_HASH_SIZE; i++)
    {
      entry = TextPropHashTable[i].next;
      if(entry)
	{
	  printf("%d==> ", i);
	  while(entry)
	    {
	      TextProp   *prop = entry->textprop;
	      if(prop)
		printf("<%d %d:%lx %d:%lx %d %s %d> ",
		       prop->font, prop->fgset,prop->fg,  prop->bgset,prop->bg, prop->underline,
		       EZ_TextPropertyBitmap(prop)? EZ_GetBitmapFileName(EZ_TextPropertyBitmap(prop)) :
		       " ", prop->mark);
	      else printf("<BAD>");
	      entry = entry->next;
	    }
	  printf("\n");
	}
    }
}
#endif
/*****************************************************************************************************/
void EZ_BreakITextLines(wptr, flag) EZ_Widget *wptr; int flag;
{
  int hsize = EZ_ITextHPageSize(wptr);
  int changed = 0, len, i, j;
  int nlines = EZ_ITextNLines(wptr);
  int wkind = EZ_ITextWrapText(wptr);
  int firstLine = EZ_ITextFirstLine(wptr);
  char  *str;
  ITextLine **line, *tline; 
  line = EZ_ITextData(wptr); 

  /* first unwrap lines */
  if(EZ_ITextWraped(wptr))
    {
      int decr = 0;
      while(firstLine > 0 && line[firstLine]->leader != NULL) firstLine--;
      for(i = 0; i < nlines; i++)
        {
          tline = line[i];
          tline->count = 0;
          if(tline->leader) /* is a wrapped line, remove */
            {
              line[i] = NULL;
              EZ_FreeTextLine(tline);
              changed++;
              if(firstLine > i) decr++;
            }
        }
      if(changed) /* wrapped lines are removed */
        {
          firstLine -= decr;
          i = 0; j = 0;
          while( j < nlines )
            {
              while(line[j] == NULL)  j++;
              while(j < nlines && line[j] != NULL)
                {
                  ITextLine *s = line[j];
                  line[j] = NULL;  /*  !!! */
                  line[i++] = s;
                  j++;
                }
            }
          EZ_ITextNLines(wptr) = i;
          EZ_ITextFirstLine(wptr) = firstLine;
          EZ_ITextYPosition(wptr) = firstLine;
        }
      EZ_ITextWraped(wptr) = 0;
    }
  if(flag == 0) return;

  nlines = EZ_ITextNLines(wptr);

  /* compute the dimension of all lines */
  changed = 0;
  for(i = 0; i < nlines; i++)
    {
      int width, height, adjust;
      tline = line[i];
      str = tline->data;
      len =  tline->length;	        
      EZ_ITextLineDimension(wptr, tline, 0, len, &width, &height, &adjust);      
      tline->width = width;
      if(width > hsize) changed++;
      tline->height = height;
    }
  if(changed)
    {
      EZ_ITextGrowBuffer(wptr,  (changed<<2)); 
      line = EZ_ITextData(wptr);       
      nlines = EZ_ITextNLines(wptr);
      for(i = 0; i < nlines; i++)
        {
          tline = line[i];
          if(tline->width > hsize)
            {
              int count=0;
              ITextLine *leader, *next, *wlines = EZ_WrapThisTextLine(wptr, tline, hsize, 
                                                                      (wkind>1), &count);
              if(count > 0)
                {
                  if(firstLine > i) firstLine += count;
                  leader = tline;
                  if(EZ_ITextGrowBuffer(wptr, count+1)) line = EZ_ITextData(wptr);       
                  for(j = nlines + count-1; j >= i+count; j--)  line[j] = line[j-count];
                  j = i+1;
                  while(wlines)
                    {
                      next = wlines->leader;
                      wlines->leader = leader;
                      line[j++] = wlines;
                      wlines = next;
                    }
                }
              i += count;
              EZ_ITextNLines(wptr) += count;
              nlines = EZ_ITextNLines(wptr);
            }
        }
      EZ_ITextWraped(wptr) = 1;
      EZ_ITextFirstLine(wptr) = firstLine;
      EZ_ITextYPosition(wptr) = firstLine;
    }
}

/*******************************************************************************************/

ITextLine *EZ_WrapThisTextLine(widget, lineIn, hsize, wword, c_ret)
     EZ_Widget *widget;
     ITextLine *lineIn;
     int       hsize, wword;
     int       *c_ret;
{
  ITextLine *sline=NULL, *cline = NULL, *tline = lineIn;
  TextProp  **sprop, **prop, *cprop;
  char      *str, *sstr;
  int       len, icount, totalLen, totalCount;
  int       twidth=0, whiteSpace;
  int       font_id, linecount = 0, wrapped = 0;
  XFontStruct *xfont = NULL, *defaultfont;
  EZ_Bitmap *bitmap;

  totalLen = len = tline->length;
  sstr  = tline->data;
  sprop = tline->prop;
  defaultfont = EZ_ITextFont(widget);
  totalCount = 0;
  
again:
  str = sstr + totalCount;
  prop = sprop + totalCount;
  cprop = NULL;
  xfont = defaultfont;
  bitmap = NULL;
  twidth = 0;
  icount = 0;
  whiteSpace = 0;
  
  while(totalCount < totalLen)
    {
      if(cprop != prop[icount])
        {
          cprop =  prop[icount];
          if(cprop) 
            {
              font_id = (int) EZ_TextPropertyFont(cprop);
              bitmap =  EZ_TextPropertyBitmap(cprop);
            }
          else { font_id = -1; bitmap = NULL;}
          if(font_id < 0) xfont = defaultfont;
          else xfont = EZ_GetFontFromId(font_id);
        }
      if(bitmap == NULL)
        {
          twidth += XTextWidth(xfont, str+icount, 1); 
          if(twidth > hsize)
            {
              if(wword && whiteSpace)
                {
                  int diff = icount - whiteSpace;
                  icount = whiteSpace;
                  totalCount -= diff;
                }
              if(wrapped == 0)
                {
                  lineIn->count = icount;
                  wrapped = 1;
                  goto again;
                }
              else
                {
                  ITextLine *tmpLine = EZ_GetWrappedTextLine();
                  tmpLine->data = str;
                  /*printf("[%s] %d %d\n", str, totalCount, icount);*/
                  tmpLine->prop = prop;
                  tmpLine->space = 0;
                  tmpLine->length = icount;
                  if(cline) cline->count = cline->length; /* mark cline is continued by tmpLine */
                  if(sline == NULL) sline = cline = tmpLine;
                  else cline->leader = tmpLine;
                  cline = tmpLine;
                  linecount++;
                  goto again;              
                }
            }
          else
            {
              char c =str[icount];
              if(c==' ' || c == '\t') whiteSpace = icount;
              icount++;
              totalCount++;
            }
        }
      else /* bitmap, special */
        {
          int tcount = totalCount;
          int ii = 0;
          TextProp *tmpP = sprop[tcount];
          /* skip over text with the same text property */
          while(tcount < totalLen && tmpP == sprop[tcount]) { tcount++; ii++; }
          
          twidth += bitmap->width;
          if(twidth > hsize)
            {
              if(icount > 0)
                {
                  if(wrapped == 0)
                    {
                      lineIn->count = icount;
                      wrapped = 1;
                      goto again;
                    }
                  else
                    {
                      ITextLine *tmpLine = EZ_GetWrappedTextLine();
                      tmpLine->data = str;
                      tmpLine->prop = prop;
                      tmpLine->space = 0;
                      tmpLine->length = icount;
                      if(cline) cline->count = cline->length;
                      if(sline == NULL) sline = cline = tmpLine;
                      else cline->leader = tmpLine;
                      cline = tmpLine;
                      linecount++;
                      goto again;
                    }
                }
              else /* image is too wide */
                {
                  if(wrapped == 0)
                    {
                      lineIn->count = ii;
                      wrapped = 1;
                      totalCount = tcount;
                      goto again;
                    }
                  else
                    {
                      ITextLine *tmpLine = EZ_GetWrappedTextLine();
                      tmpLine->data = str;
                      tmpLine->prop = prop;
                      tmpLine->space = 0;
                      tmpLine->length = ii;
                      if(cline) cline->count = cline->length;
                      if(sline == NULL) sline = cline = tmpLine;
                      else cline->leader = tmpLine;
                      cline = tmpLine;
                      linecount++;
                      totalCount = tcount;                      
                      goto again;
                    }
                }
            }
          else
            {
              icount += ii;
              totalCount = tcount;
            }
        }
    }
  /* the last line */
  if(icount > 0)
    {
      ITextLine *tmpLine = EZ_GetWrappedTextLine();
      tmpLine->data = str;
      tmpLine->prop = prop;
      tmpLine->space = 0;
      tmpLine->length = icount;
      if(cline) cline->count = cline->length;
      if(sline == NULL) sline = cline = tmpLine;
      else cline->leader = tmpLine;
      linecount++;
    }
  *c_ret = linecount;
  return(sline);
}

/***************************************************************************************************/
int EZ_DecodeTextProperty(tp, fg, bg, font, ul, bitmap)
     TextProp *tp; unsigned long *fg, *bg; int *font, *ul; EZ_Bitmap **bitmap;
{
  if(tp)
    {
      int rr = 1;
      if(fg && tp->fgset) {*fg = tp->fg; rr += 2;}
      if(bg && tp->bgset) {*bg = tp->fg; rr += 4;}
      if(font) *font = tp->font;
      if(ul) *ul = tp->underline;
      if(tp->bitmap){ *bitmap = tp->bitmap; rr |= 256;}
      return(rr);
    }
  return(0);
}
/***************************************************************************************************/
#undef _EZ_TEXT_C_
