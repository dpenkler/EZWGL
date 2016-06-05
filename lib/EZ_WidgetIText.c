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
/*****************************************************************
 ***                                                           ***
 ***              Widget Internal TextWidget                   ***
 ***                                                           ***
 *****************************************************************/
#define _EZ_WIDGET_ITEXT_C_

#include "EZ_Widget.h"

/*********************************************************************
 * 
 *  Functions implemented in this file:
 */
EZ_Widget        *EZ_CreateIText MY_ANSIARGS((EZ_Widget *widget, int editable));
void             EZ_DrawWidgetIText MY_ANSIARGS((EZ_Widget *widget));
void             EZ_ComputeWidgetITextSize MY_ANSIARGS((EZ_Widget *widget, int *w, int *h));
void             EZ_FreeWidgetITextData MY_ANSIARGS((EZ_Widget *widget));
void             EZ_ITextEventHandle MY_ANSIARGS((EZ_Widget *widget, XEvent *event));

/*********************************************************************/
int              EZ_ITextGetPosition MY_ANSIARGS((EZ_Widget *widget));
void             EZ_ITextGotoPosition MY_ANSIARGS((EZ_Widget *widget, int pos));
int              EZ_ITextSetMarker MY_ANSIARGS((EZ_Widget *widget));
char             *EZ_ITextGetRegionString MY_ANSIARGS((EZ_Widget *widget));
char             *EZ_ITextGetBufferString MY_ANSIARGS((EZ_Widget *widget));
/*********************************************************************/

void             EZ_ITextLoadFile MY_ANSIARGS((EZ_Widget *widget, char *file));
void             EZ_ITextLoadAnnotatedFile MY_ANSIARGS((EZ_Widget *widget, char *file));
void             EZ_ITextClear MY_ANSIARGS((EZ_Widget *widget));

void             EZ_ITextNextLine MY_ANSIARGS((EZ_Widget *widget));
void             EZ_ITextPreviousLine MY_ANSIARGS((EZ_Widget *widget));
void             EZ_ITextBackwardChar MY_ANSIARGS((EZ_Widget *widget));
void             EZ_ITextForwardChar MY_ANSIARGS((EZ_Widget *widget));
void             EZ_ITextForwardWord MY_ANSIARGS((EZ_Widget *widget));
void             EZ_ITextBackwardWord MY_ANSIARGS((EZ_Widget *widget));
void             EZ_ITextPreviousPage MY_ANSIARGS((EZ_Widget *widget));
void             EZ_ITextNextPage MY_ANSIARGS((EZ_Widget *widget));
void             EZ_ITextEndOfBuffer MY_ANSIARGS((EZ_Widget *widget));
void             EZ_ITextBeginningOfBuffer MY_ANSIARGS((EZ_Widget *widget));
void             EZ_ITextEndOfLine MY_ANSIARGS((EZ_Widget *widget));
void             EZ_ITextBeginningOfLine MY_ANSIARGS((EZ_Widget *widget));
void             EZ_ITextInsertChar MY_ANSIARGS((EZ_Widget *widget, char c));
void             EZ_ITextDeletePreviousChar MY_ANSIARGS((EZ_Widget *widget));
void             EZ_ITextDeleteNextChar MY_ANSIARGS((EZ_Widget *widget));
void             EZ_ITextInsertNewLine MY_ANSIARGS((EZ_Widget *widget, int flag));
void             EZ_ITextKillEndOfLine MY_ANSIARGS((EZ_Widget *widget));
void             EZ_ITextKillRegion MY_ANSIARGS((EZ_Widget *widget));
void             EZ_ITextCopyToKillBuffer MY_ANSIARGS((EZ_Widget *widget,int fx, int fy, int tx, int ty));
void             EZ_ITextPasteSelection MY_ANSIARGS((EZ_Widget *widget));
void             EZ_ITextReDisplay MY_ANSIARGS((EZ_Widget *widget));
void             EZ_ITextInsertString MY_ANSIARGS((EZ_Widget *widget, char *str));
void             EZ_ITextInsertAnnotatedString MY_ANSIARGS((EZ_Widget *widget, char *str));

int              EZ_ITextGrowBuffer MY_ANSIARGS((EZ_Widget *widget, int incr));
void             EZ_ITextInsertStringWithProperty MY_ANSIARGS((EZ_Widget *widget, char *str, 
							       EZ_TextProperty *prop));

void             EZ_ITextSetRegionFont MY_ANSIARGS((EZ_Widget *widget, char *fontName));
void             EZ_ITextSetRegionForeground MY_ANSIARGS((EZ_Widget *widget, char *clrName));
void             EZ_ITextSetRegionBackground  MY_ANSIARGS((EZ_Widget *widget, char *clrName));
void             EZ_ITextSetRegionUnderline  MY_ANSIARGS((EZ_Widget *widget, int flag));
void             EZ_ITextSetRegionProperty  MY_ANSIARGS((EZ_Widget *widget, EZ_TextProperty *prop));

void             EZ_ITextReplaceRegionWithBitmap MY_ANSIARGS((EZ_Widget *widget, char *file));
int              EZ_ITextSaveText MY_ANSIARGS((EZ_Widget *widget, char *file));
int              EZ_ITextSaveAnnotatedText MY_ANSIARGS((EZ_Widget *widget, char *file));
/*********************************************************************/
void             EZ_PasteSelection MY_ANSIARGS((EZ_Widget *widget, Window win,
						Atom prop, int del,
						void (*insertF) MY_ANSIARGS((EZ_Widget *widget, char *str)) ));
						
void             EZ_SetSelectionOwner MY_ANSIARGS((EZ_Widget *widget));
void             EZ_HandleSelectionRequest MY_ANSIARGS((EZ_Widget *widget, XEvent *event));

/*********************************************************************
 * 
 *  Local Functions.
 */
static void  EZ_ITextLoadTextFile MY_ANSIARGS((EZ_Widget *widget, char *file, int annot));
static void  EZ_InitITextWidget MY_ANSIARGS((EZ_Widget *widget));
static void  EZ_ITextInsertStringWork MY_ANSIARGS((EZ_Widget *widget, char *str, int len, EZ_TextProperty *prop));
static void  EZ_SetITextCursorPosition MY_ANSIARGS((EZ_Widget *widget, int x, int y, int redisp));
static void  EZ_ITextDisplayOneLine MY_ANSIARGS((EZ_Widget *widget, int lineNum));
static void  EZ_ITextYank MY_ANSIARGS((EZ_Widget *widget));
static void  update_itext_scrolls MY_ANSIARGS((EZ_Widget *widget));
static void  grab_next_quoted_string MY_ANSIARGS((char *from, char *to));
static void  EZ_ITextCopyToKBufferA MY_ANSIARGS((EZ_Widget *widget, int fx, int fy, int tx, int ty));

/*********************************************************************
 * 
 */
static EZ_WidgetHandle EZ_ITextHandle =
{
  EZ_ComputeWidgetITextSize,
  EZ_DrawWidgetIText,
  EZ_FreeWidgetITextData,
  EZ_ITextEventHandle,
};
/*********************************************************************/
extern ITextLine TextWidgetKillBuffer;
extern ITextLine TWKBufferA;
/*********************************************************************/

EZ_Widget  *EZ_CreateIText(parent, editable)
     EZ_Widget  *parent;     /* parent widget    */
     int        editable;
{
  EZ_Widget  *wptr;
  int        i;

  wptr = EZ_CreateNewWidget(parent);
  /*--------------------------------------------------
   * Register the handling functions for ...
   *  has to be done after  EZ_CreateNewWiget.
   *-------------------------------------------------*/
  EZ_WidgetHandlingFunctions[EZ_WIDGET_ITEXT] = &EZ_ITextHandle;

  EZ_SetWidgetTypeAndNames(wptr,EZ_WIDGET_ITEXT);
  EZ_WidgetBorderStyle(wptr)  = EZ_BORDER_DOWN;
  EZ_WidgetBorderWidth(wptr)  = 2;
  EZ_WidgetSepX(wptr) = 0;
  EZ_WidgetSepY(wptr) = 0;

  EZ_WidgetCursor(wptr) = EZ_GetCursor(EZ_C_XTERM);

  EZ_WidgetPadB(wptr) = 2;

  EZ_ITextFont(wptr) = EZ_GetFontFromId(EZ_TEXT_FONT);
  EZ_ITextTHeight(wptr)    = 10;
  EZ_ITextTextBG(wptr)     = ~0;
  EZ_ITextTextSelectionBG(wptr)= ~0;

  EZ_ITextHScroll(wptr) = (EZ_Widget *)NULL;
  EZ_ITextVScroll(wptr) = (EZ_Widget *)NULL;
  EZ_ITextTextWidget(wptr) = (EZ_Widget *)NULL;

  EZ_ITextData(wptr) = (ITextLine **)my_malloc( 256 * sizeof(ITextLine *), _ITEXT_D_);
  EZ_ITextSpace(wptr) = 256;
  for(i = 0; i < 256; i++)
    EZ_ITextData(wptr)[i] = (ITextLine *)NULL;

  EZ_InitITextWidget(wptr);
  EZ_ITextEditable(wptr) = (editable != 0);
  EZ_ITextHScrollInAction(wptr) = 0;
  EZ_ITextVSpacing(wptr) = 0;

  EZ_SetWidgetFocusableFlag(wptr);  

  return(wptr);
}

static void  EZ_InitITextWidget(wptr)
     EZ_Widget *wptr;
{
  EZ_ITextLastCmdIsKill(wptr) = 0;
  EZ_ITextMaxHSize(wptr) = 1;
  EZ_ITextHPageSize(wptr) = 1;
  EZ_ITextHStart(wptr) = 0;
  EZ_ITextXMark(wptr) = -1;
  EZ_ITextYMark(wptr) = -1;
  EZ_ITextXPosition(wptr) = 0;  /* initial cursor */
  EZ_ITextYPosition(wptr) = 0;
  EZ_ITextLXPosition(wptr) = 0;
  EZ_ITextSelectionYBgn(wptr)= -1;
  EZ_ITextSelectionYEnd(wptr)= -1;
  EZ_ITextSelectionXBgn(wptr)= -1;
  EZ_ITextSelectionXEnd(wptr)= -1;
  EZ_ITextSelectionFlag(wptr) = 0;
  EZ_ITextFirstLine(wptr)  = 0;
  EZ_ITextLastLine(wptr)  = 0;
  EZ_ITextNLines(wptr)  = 1;  
  EZ_ITextDirty(wptr) = 1;
  /*
   * We always add a new line at the end and this newline
   * cannot be deleted. We have to
   * strip it out when we write the buffer out.
   */
  {
    ITextLine *tline = EZ_ITextData(wptr)[0];
    if( tline == (ITextLine *)NULL)
      {
	EZ_GrowITextLine(&tline, 16);
	EZ_ITextData(wptr)[0] = tline;
      }
    tline->data[0] = '\n';  tline->data[1] = '\0';
    tline->prop[0] = (TextProp *)NULL;
    tline->length = 1;
  }
}
/******************************************************************************/
void  EZ_FreeWidgetITextData(widget)
     EZ_Widget *widget;
{
  ITextLine   **tmp = EZ_ITextData(widget);
  ITextLine   *tline = tmp[0]; /* the first line */

  EZ_ITextClear(widget);  /* leaves the first line unfreeed */
  EZ_FreeTextLine(tline); /* the first line */
  (void)my_free( (char *) tmp);
}
/********************************************************************
 *
 *  Figure out the dimension of an internal list box.
 */
void EZ_ComputeWidgetITextSize(widget, w, h)
     EZ_Widget *widget;
     int       *w, *h;
{
  int   height, cw, ch, padb;

  if(!(EZ_GetWidgetSizeComputedFlag(widget)))
    { 
      EZ_ITextTHeight(widget) = EZ_ITextFont(widget)->max_bounds.ascent +
	EZ_ITextFont(widget)->max_bounds.descent;
      EZ_SetWidgetSizeComputedFlag(widget);
    }
  height = EZ_ITextTHeight(widget);
  
  cw = EZ_WidgetPadX(widget) + EZ_WidgetBorderWidth(widget);
  ch = EZ_WidgetPadY(widget) + EZ_WidgetBorderWidth(widget);
  cw = cw + cw + height; 
  ch = ch + ch + height;

  padb = EZ_WidgetPadB(widget)<<1;
  *w = cw + padb;
  *h = ch + padb;
}

/****************************************************************************
 *
 *  Draw a IText widget
 */

void  EZ_DrawWidgetIText(wptr)
     EZ_Widget *wptr;
{
  int             w, h,x,y,xx, i, padb, padb2, wrapAdjust=0, doselection;
  int             totalheight, itmp, hsize = 0, padx, pady, bw, wrapText;
  Pixmap          pixmap;
  Window          win;
  GC              gc, hgc = (GC)NULL;
  unsigned long   bgpv;

  if(EZ_GetWidgetFreezedFlag(wptr)) return;
  
  win = EZ_WidgetWindow(wptr);
  w   = EZ_WidgetWidth(wptr);
  h   = EZ_WidgetHeight(wptr);
  padb = EZ_WidgetPadB(wptr);
  padb2 = padb + padb;
  pixmap = XCreatePixmap(EZ_Display, win, w, h, EZ_Depth);

  bw = EZ_WidgetBorderWidth(wptr);
  padx = EZ_WidgetPadX(wptr);
  pady = EZ_WidgetPadY(wptr);
  wrapText = EZ_ITextWrapText(wptr);

  /* wrapText only works in browsing mode ...
   * need to rewrite text widget !!!
   */
  if(EZ_ITextEditable(wptr)) wrapText = 0;
  if(wrapText) wrapAdjust = padx < 6? (6-padx): 0;

  totalheight = EZ_WidgetHeight(wptr) - pady -  padb2 - (bw<<1);
  if(EZ_ITextFirstLine(wptr) < 0) EZ_ITextFirstLine(wptr) = 0;

  /*  Set horizontal page size */
  EZ_ITextHPageSize(wptr) = EZ_WidgetWidth(wptr) - padb2 - - wrapAdjust - ((bw+padx)<<1);
  if(EZ_ITextHStart(wptr) > 0) EZ_ITextHStart(wptr) = 0;
  if(1/* EZ_ITextEditable(wptr) != 0 */)
    {
      itmp = EZ_ITextHPageSize(wptr) - EZ_ITextMaxHSize(wptr);
      if(itmp < 0 && EZ_ITextHStart(wptr) < itmp) EZ_ITextHStart(wptr) = itmp;
    }

  /* EZ_ITextNLines(wptr) may be modified ! */
  if(wrapText == 0 )
    {
      itmp = EZ_ITextHPageSize(wptr) - EZ_ITextMaxHSize(wptr);
      if(itmp < 0 && EZ_ITextHStart(wptr) < itmp) EZ_ITextHStart(wptr) = itmp;
      if(EZ_ITextDirty(wptr)) EZ_BreakITextLines(wptr, 0);
    }
  else if(EZ_ITextDirty(wptr))
    {
      EZ_ITextHStart(wptr) = 0;
      EZ_BreakITextLines(wptr, 1);
    }
  EZ_ITextDirty(wptr) = 0;  
  
  i = 0;
  if(EZ_ITextNLines(wptr) != 0)
    {
      int      len, tlen, tlen1, flag_last;
      int      wtmp, htmp, maxascent, junk, curX, curY;
      char     *str ;
      register ITextLine **line, *tline; 

      /*
       * First check to see if we need to scroll.
       */
      if(1/* EZ_ITextEditable(wptr) != 0 */)
	{
	  /* scroll horizontally if necessary */
	  int wjunk, hjunk, ajunk, accu, CY;
	  if(wrapText == 0 && EZ_ITextHScrollInAction(wptr) == 0)
	    {
	      tline = EZ_ITextData(wptr)[EZ_ITextYPosition(wptr)];
	      EZ_ITextLineDimension(wptr, tline, 0, 
				    EZ_ITextXPosition(wptr),
				    &wjunk, &hjunk, &ajunk);
	      wjunk += EZ_ITextHStart(wptr);
	      ajunk = w - ((bw+padx)<< 1) - padb2;
	      if(wjunk < 0 || wjunk > ajunk ) /* cursor is invisible, scroll horizontally */
		EZ_ITextHStart(wptr) += ( (ajunk >> 1) - wjunk);
	      if(EZ_ITextHStart(wptr) > 0) EZ_ITextHStart(wptr) = 0;
	    }
	  /* 
	   * scroll vertically if necessary. Bitmaps make life difficult,
	   * since they don't have a fixed size.
	   */
	  CY = EZ_ITextYPosition(wptr);
	  if(CY <= EZ_ITextFirstLine(wptr))
	    EZ_ITextFirstLine(wptr) = EZ_ITextYPosition(wptr);
	  else
	    {
	      int theight = totalheight + (int)EZ_ITextVSpacing(wptr) + 4;
	      line = EZ_ITextData(wptr)+ EZ_ITextFirstLine(wptr);
	      accu = bw + pady + padb;
	      for(i = EZ_ITextFirstLine(wptr); i <= CY; i++, line++)
		{
		  tline = *line;
		  if(tline == (ITextLine *)NULL) break;
		  str = tline->data;
		  len = tline->length;	  
		  EZ_ITextLineDimension(wptr, tline, 0, len, &wjunk, &hjunk, &ajunk);
		  tline->height = hjunk;
		  accu += hjunk;
		}
	      if( accu > theight)
		{
		  i = EZ_ITextFirstLine(wptr);
		  line = EZ_ITextData(wptr)+ i;
		  while(accu > totalheight)
		    {
		      if( (tline = *line) == NULL) break;
		      accu -= tline->height;
		      line++;
		      i++;
		    }
		  EZ_ITextFirstLine(wptr) = i;
		}
	    }
	}
    
      xx = EZ_ITextHStart(wptr);
      x = xx  + padx + bw + padb;
      y = bw + pady + padb;
      curX = EZ_ITextXPosition(wptr);
      curY = EZ_ITextYPosition(wptr);
      line = EZ_ITextData(wptr)+ EZ_ITextFirstLine(wptr);

      /*-----------------------------------------------------------
       *  11-28-96. Moved to here, for background tiling.
       *  Create a pixmap, draw into this pixmap in background and
       *  copy to the button window when finished.
       *----------------------------------------------------------*/
      {
	if(EZ_ITextTextBG(wptr)  != ~0)
	  {
	    bgpv = EZ_ITextTextBG(wptr);	
	    XSetForeground(EZ_Display, EZ_WRITABLEGC, bgpv);
	    gc = EZ_WRITABLEGC;
	  }
	else 
	  {
	    if(EZ_GetBackgroundGC(wptr, &gc, &bgpv, xx, 0))
	      {
		/* we cannot figure out exactly the TileOriginY in general,
		 * unless we calculte the y offset for the all the lines upto
		 * the first display line, which is quite expensive. Here we
		 * just do a simple approximation
		 */
                int tox, toy;
                if(EZ_WidgetTOSet(wptr))
                  EZ_GetWidgetTileOrigin(wptr, &tox, &toy); 
                else
                  {
                    int fl = EZ_ITextFirstLine(wptr);
                    int lw = EZ_ITextTHeight(wptr) + EZ_ITextVSpacing(wptr);
                    int yy = fl * lw;
                    tox = xx;
                    toy = -yy;
                  }
		XSetTSOrigin(EZ_Display, gc, tox, toy);
	      }
	  }
      }
      XFillRectangle(EZ_Display, pixmap, gc, padb,padb, w -padb2, h-padb2); 
      /* render the text */
      if(EZ_ITextSelectionFlag(wptr) != 0)
	{
	  unsigned long bg = EZ_ITextTextSelectionBG(wptr);
	  hgc = EZ_BD_DARKGC; 
	  if(bg == ~0) EZ_GetBrightBDpv(wptr, &bg);
	  XSetForeground(EZ_Display, hgc, bg);
	}
      flag_last = 0;
      doselection = EZ_ITextSelectionFlag(wptr);
      for(i = EZ_ITextFirstLine(wptr); i < EZ_ITextNLines(wptr); i++, line++)
	{
	  tline = *line;
	  if(tline == (ITextLine *)NULL) break;
	  str = tline->data;
	  len = tline->length;	  
	  EZ_ITextLineDimension(wptr, tline, 0, len, &wtmp, &htmp, &maxascent);
	  
	  if(htmp + y > totalheight )
	    {
	      if(flag_last) break;  /* make sure one line is displayed */
	      flag_last = 1;
	    }
	  if(hsize < wtmp) hsize = wtmp;    

	  /*
	   * highlight the selection if selection is active
	   */
          if(doselection)
	    {
	      int ybgn = EZ_ITextSelectionYBgn(wptr);
	      int yend =  EZ_ITextSelectionYEnd(wptr);
	      if(i >= ybgn && i <= yend )
		{
		  if(i == ybgn)     /* selection start at this line */
		    {
		      if( i == yend)/* selection end at this line   */
			{
			  EZ_ITextLineDimension(wptr, tline, 0, 
						EZ_ITextSelectionXBgn(wptr), &tlen, &junk, &junk);
			  EZ_ITextLineDimension(wptr, tline, EZ_ITextSelectionXBgn(wptr),
						EZ_ITextSelectionXEnd(wptr) - EZ_ITextSelectionXBgn(wptr),
						&tlen1, &junk, &junk);
			  XFillRectangle(EZ_Display, pixmap, hgc, 
					 x+tlen, y, tlen1+1, htmp- (int)EZ_ITextVSpacing(wptr));
			}
		      else  /* selection involves more than 1 line */
			{
			  EZ_ITextLineDimension(wptr, tline, 0, 
						EZ_ITextSelectionXBgn(wptr), &tlen, &junk, &junk);
			  XFillRectangle(EZ_Display, pixmap, hgc,
					 x+tlen,y, hsize-tlen, htmp- (int)EZ_ITextVSpacing(wptr)); 
			}
		    }		  
		  else if(i < yend) /* the whole line is selected */
		    {
		      XFillRectangle(EZ_Display, pixmap, hgc, 
				     x, y, hsize, htmp- (int)EZ_ITextVSpacing(wptr));  
		    }
		  else if(i == yend) /* selection end at this line */
		    {
		      EZ_ITextLineDimension(wptr, tline, 0, EZ_ITextSelectionXEnd(wptr), 
					    &tlen, &junk, &junk);
		      XFillRectangle(EZ_Display, pixmap, hgc,
				     x,y, tlen, htmp- (int)EZ_ITextVSpacing(wptr)); 
		    }
		}
	    }
	  /*
	   * set the bounding box of tline
	   */
	  tline->width = wtmp;
	  tline->height = htmp;
	  tline->x = x;
	  tline->y = y;
	  EZ_ITextRenderText(wptr, tline, x, y, htmp, maxascent, pixmap, i, curX, curY);
	  y += htmp;
	}
    }
  EZ_FixFocusHighlightBorder(wptr, pixmap, w, h, padb);
  EZ_DrawRectBorder(wptr, pixmap);
  XCopyArea(EZ_Display,pixmap,win, EZ_WRITABLEGC,0,0,w,h,0,0); 
  XFreePixmap(EZ_Display, pixmap); 
  EZ_ITextLastLine(wptr) = i - 1;
  EZ_ITextMaxHSize(wptr) = hsize;
  update_itext_scrolls(wptr);
}
/*************************************************************************/

static void  update_itext_scrolls(wptr)
     EZ_Widget *wptr;
{
  if(EZ_LookupWidgetFromAllHT(EZ_ITextHScroll(wptr)) != (EZ_Widget *)NULL)
    {
      EZ_UpdateScrollbarI(EZ_ITextHScroll(wptr),
			  EZ_ITextMaxHSize(wptr),
			  EZ_ITextHPageSize(wptr),
			  EZ_ITextHStart(wptr));
    }
  else
    {
      EZ_Widget *p = EZ_ITextTextWidget(wptr);
      EZ_ITextHScroll(wptr) = (EZ_Widget *)NULL;
      if(p) EZ_TextHScroll(p) = (EZ_Widget *)NULL;
    }
  if(EZ_LookupWidgetFromAllHT(EZ_ITextVScroll(wptr)) != (EZ_Widget *)NULL)
    {
      EZ_UpdateScrollbarI(EZ_ITextVScroll(wptr),
			  EZ_ITextNLines(wptr),
			  EZ_ITextPageSize(wptr),
			  EZ_ITextFirstLine(wptr));  
    }
  else
    {
      EZ_Widget *p = EZ_ITextTextWidget(wptr);
      EZ_ITextVScroll(wptr) = (EZ_Widget *)NULL;
      if(p) EZ_TextVScroll(p) = (EZ_Widget *)NULL;
    }
}
/*************************************************************************/
static void update_itext_selection(widget)
     EZ_Widget *widget;
{
  int tox, toy;

  tox = EZ_ITextXPosition(widget);
  toy = EZ_ITextYPosition(widget);
  if(EZ_ITextSelectionYBgn(widget) > toy)
    {
      EZ_ITextSelectionYBgn(widget) = toy;
      EZ_ITextSelectionXBgn(widget) = tox;
    }
  else if(EZ_ITextSelectionYEnd(widget) < toy)
    {
      EZ_ITextSelectionYEnd(widget) = toy;
      EZ_ITextSelectionXEnd(widget) = tox;
    }
  else 
    {
      int xbgn, xend, ybgn, yend;
      xbgn = EZ_ITextSelectionXBgn(widget);
      ybgn = EZ_ITextSelectionYBgn(widget);
      xend = EZ_ITextSelectionXEnd(widget);
      yend = EZ_ITextSelectionYEnd(widget);
      if(yend > ybgn)
	{
	  if(toy - ybgn > yend - toy)
	    {
	      EZ_ITextSelectionYEnd(widget) = toy;
	      EZ_ITextSelectionXEnd(widget) = tox;  
	    }
	  else
	    {
	      EZ_ITextSelectionYBgn(widget) = toy;
	      EZ_ITextSelectionXBgn(widget) = tox;
	    }
	}
      else
	{
	  if(tox - xbgn > xend - tox)  EZ_ITextSelectionXEnd(widget) = tox;  
	  else EZ_ITextSelectionXBgn(widget) = tox;
	}
    }
}
/*************************************************************************
 *
 *  Event handling
 */
#define ITEXT_BUFFER_SIZE 8
#define ITextCancelSelection(iwidget) { if(EZ_ITextSelectionFlag(iwidget)) \
					  {\
					     EZ_ITextSelectionFlag(iwidget) = 0;\
					     EZ_DrawWidget(iwidget); \
					 } }

void  EZ_ITextEventHandle(widget, event)
     EZ_Widget *widget;
     XEvent          *event;
{
  XEvent            xevent;
  int               x, y,fromx,tox,fromy,toy, oldX, oldY, done, save_X, save_Y;
  int               count, modifiers, wox,woy, highlighted = 0, editable;
  Window            junkwin;
  KeySym            keysym;
  XComposeStatus    compose; 
  char              tmpbuffer[ITEXT_BUFFER_SIZE];
  int               buffersize = ITEXT_BUFFER_SIZE;

  if(widget == (EZ_Widget *)NULL) return;
  if(event->type == Expose && event->xexpose.count == 0)
    EZ_DrawWidget(widget);      
  if( EZ_GetWidgetDisabledFlag(widget) )
    return;

  editable = EZ_ITextEditable(widget);

  switch(event->type)
    {
    case ConfigureNotify:
      EZ_ITextDirty(widget) = 1;
      if(editable == 0) ITextCancelSelection(widget);
      break;
    case MappingNotify:
      XRefreshKeyboardMapping((XMappingEvent *)event);
      break;
    case EnterNotify:
      break;
    case LeaveNotify:
      break;
    case SelectionNotify:
      EZ_ITextLastCmdIsKill(widget) = 0;	
      {
	XSelectionEvent *Sevent = (XSelectionEvent *)event;
	Window window = Sevent->requestor;
	Atom   property = Sevent->property;

	EZ_PasteSelection(widget, window,  property,  True, EZ_ITextInsertString);
      }
      break;
    case SelectionRequest:
      EZ_HandleSelectionRequest(widget, event);
      break;
    case SelectionClear:
      EZ_ITextSelectionFlag(widget) = 0;
      EZ_DrawWidget(widget);
      break;
    case ButtonPress:
      {
	Window fw; int rt;
	EZ_Widget *tmp;
	XGetInputFocus(EZ_Display, &fw, &rt);
	tmp = EZ_LookupWidgetFromMappedHT(fw);
	if(tmp != widget) EZ_SetFocusTo(widget);
      }
      EZ_ITextLastCmdIsKill(widget) = 0;
      /*=> */
      ITextCancelSelection(widget);
      /*<= */
      if(event->xbutton.button == EZ_Btn1)
	{
	  Time  time_stamp = event->xbutton.time;   
          int   elapsedtime = EZ_LastButton1ClickTime - time_stamp;
          EZ_LastButton1ClickTime = time_stamp;

	  done = 0;
	  x = event->xbutton.x;
	  y = event->xbutton.y;
	  EZ_SetITextCursorPosition(widget,x,y, 1);
	  EZ_ITextSetMarker(widget);
	  save_X = oldX = EZ_ITextXPosition(widget);
	  save_Y = oldY = EZ_ITextYPosition(widget);
          if(ABSV(elapsedtime) < DOUBLE_CLICK_TIME_INTERVAL)
            {
              int xp = EZ_ITextXPosition(widget);
              int yp = EZ_ITextYPosition(widget);
              ITextLine *tline = EZ_ITextData(widget)[yp];
              TextProp  **prop = tline->prop, *cprop;
              cprop = prop[xp];
	      {
		/* EZ_Widget *twidget = EZ_ITextTextWidget(widget);*/
		EZ_ExecuteWidgetCallBacks(widget);
		/* if(twidget) {EZ_ExecuteWidgetCallBacks(twidget);}*/
	      }
	      return;
            }
	  XTranslateCoordinates(EZ_Display, EZ_WidgetWindow(widget),
				RootWindow(EZ_Display, EZ_ScreenNum),
				0, 0,
				&wox,&woy, &junkwin);		  
	  while(!done)
	    {
	      XNextEvent(EZ_Display, &xevent);
	      if(EZ_FilterEvent(&xevent))
		EZ_InvokePrivateEventHandler(&xevent);
	      if(xevent.type == Expose || xevent.type == FocusIn || xevent.type == FocusOut)
		EZ_WidgetDispatchEvent(&xevent);
	      while(XPending(EZ_Display))
		{
		  if(xevent.type == ButtonRelease && xevent.xbutton.button == EZ_Btn1) break;
		  XNextEvent(EZ_Display, &xevent);
		  if(EZ_FilterEvent(&xevent))
		    EZ_InvokePrivateEventHandler(&xevent);
		  if(xevent.type == Expose || xevent.type == FocusIn || xevent.type == FocusOut )
		    EZ_WidgetDispatchEvent(&xevent);
		}
	      if(xevent.type == ButtonRelease && xevent.xbutton.button == EZ_Btn1)
		{
		  /* finish selection */
		  if(EZ_ITextSelectionFlag(widget))
		    {
		      /*=> *
			EZ_ITextSelectionFlag(widget) = 0;
			*<= */
		      EZ_ITextCopyToKillBuffer(widget, 
					       EZ_ITextSelectionXBgn(widget),
					       EZ_ITextSelectionYBgn(widget),
					       EZ_ITextSelectionXEnd(widget),
					       EZ_ITextSelectionYEnd(widget));
		      EZ_ReDisplayWidget(widget);
		    }
		  done = 1;
		}
	      if(!done)
		{
		  int             rx,ry,x,y;
		  unsigned int    mask;
		  Window          root,win;
		  XQueryPointer(EZ_Display, RootWindow(EZ_Display, EZ_ScreenNum),
				&root,
				&win, 
				&rx, &ry, 
				&x,&y, 
				&mask );
		  x = rx -wox;
		  y = ry -woy;
		  EZ_SetITextCursorPosition(widget,x,y, 0); 
		  tox = EZ_ITextXPosition(widget);
		  toy = EZ_ITextYPosition(widget);
		  if(EZ_ITextSelectionFlag(widget) == 0)
		    {
		      if(oldX != tox  || oldY != toy ) /* cursor has moved, start selection */
			EZ_ITextSelectionFlag(widget) = 1;
		    }
		  if(EZ_ITextSelectionFlag(widget) && (save_X != tox || save_Y != toy))
		    {
		      save_X = tox; save_Y = toy;
		      if(oldY < toy) { fromx = oldX; fromy = oldY;}
		      else if(oldY == toy)
			{
			  fromy = toy;
			  if(oldX < tox) { fromx = oldX; }
			  else { fromx = tox; tox = oldX; }
			}
		      else { fromx = tox; fromy = toy; tox = oldX; toy = oldY;}
		      EZ_ITextSelectionXBgn(widget) = fromx;
		      EZ_ITextSelectionXEnd(widget) = tox;
		      EZ_ITextSelectionYBgn(widget) = fromy;
		      EZ_ITextSelectionYEnd(widget) = toy;
		      EZ_DrawWidget(widget);
		    }
		}
	    }
	}
      else if(event->xbutton.button == EZ_Btn2 && editable)
	{
	  x = event->xbutton.x;
	  y = event->xbutton.y;
	  EZ_SetITextCursorPosition(widget,x,y, 1);
	  while(1)
	    {
	      XNextEvent(EZ_Display, &xevent);
	      if(EZ_FilterEvent(&xevent))
		EZ_InvokePrivateEventHandler(&xevent);
	      if(xevent.type == ButtonRelease && xevent.xbutton.button == EZ_Btn2) break;
	      if(xevent.xany.window != EZ_WidgetWindow(widget))	
		EZ_WidgetDispatchEvent(&xevent);
	    }
	  EZ_ITextPasteSelection(widget);
	}
      else if(event->xbutton.button == EZ_Btn3)
	{
	  /* update selection */
	  if(EZ_ITextSelectionYBgn(widget) == -1) /* no selection yet */
	    {
	      if(EZ_ITextYMark(widget) == -1)
		{
		  EZ_ITextSelectionYBgn(widget) = EZ_ITextSelectionYEnd(widget)
		    = EZ_ITextYPosition(widget);
		  EZ_ITextSelectionXBgn(widget) = EZ_ITextSelectionXEnd(widget)
		    = EZ_ITextXPosition(widget);
		}
	      else
		{
		  EZ_ITextSelectionYBgn(widget) = EZ_ITextSelectionYEnd(widget)
		    = EZ_ITextYMark(widget);
		  EZ_ITextSelectionXBgn(widget) = EZ_ITextSelectionXEnd(widget)
		    = EZ_ITextXMark(widget);
		}
	    }
	  x = event->xbutton.x;
	  y = event->xbutton.y;
	  EZ_SetITextCursorPosition(widget,x,y, 1); 
	  save_X = EZ_ITextXPosition(widget);
	  save_Y = EZ_ITextYPosition(widget);	  
	  update_itext_selection(widget);
	  EZ_ITextSelectionFlag(widget) = 1;
	  EZ_DrawWidget(widget);
	  done = 0;
	  XTranslateCoordinates(EZ_Display, EZ_WidgetWindow(widget),
				RootWindow(EZ_Display, EZ_ScreenNum),
				0, 0,
				&wox,&woy, &junkwin);

	  while(!done)
	    {
	      XNextEvent(EZ_Display, &xevent);
	      if(EZ_FilterEvent(&xevent))
		EZ_InvokePrivateEventHandler(&xevent);
	      if(xevent.type == Expose || xevent.type == SelectionClear || xevent.type == FocusIn || xevent.type == FocusOut)
		EZ_WidgetDispatchEvent(&xevent);
	      while(XPending(EZ_Display))
		{
		  if(xevent.type == ButtonRelease && xevent.xbutton.button == EZ_Btn3) break;
		  XNextEvent(EZ_Display, &xevent);
		  if(EZ_FilterEvent(&xevent))
		    EZ_InvokePrivateEventHandler(&xevent);
		  if(xevent.type == Expose || xevent.type == FocusIn || xevent.type == FocusOut)  
		    EZ_WidgetDispatchEvent(&xevent);
		}
	      if(xevent.type == ButtonRelease && xevent.xbutton.button == EZ_Btn3)
		{
		  /* finish selection */
		  EZ_ITextCopyToKillBuffer(widget, 
					   EZ_ITextSelectionXBgn(widget),
					   EZ_ITextSelectionYBgn(widget),
					   EZ_ITextSelectionXEnd(widget),
					   EZ_ITextSelectionYEnd(widget));
		  /*=>
		    EZ_ITextSelectionFlag(widget) = 0;
		    *<= */
		  EZ_DrawWidget(widget);
		  done = 1;
		}
	      if(!done)
		{
		  int             rx,ry,x,y;
		  unsigned int    mask;
		  Window          root,win;
		  XQueryPointer(EZ_Display, RootWindow(EZ_Display, EZ_ScreenNum),
				&root,
				&win, 
				&rx, &ry, 
				&x,&y, 
				&mask );
		  x = rx -wox;
		  y = ry -woy;
		  EZ_SetITextCursorPosition(widget,x,y, 0); 
		  tox = EZ_ITextXPosition(widget);
		  toy = EZ_ITextYPosition(widget);	  
		  if(tox != save_X || toy != save_Y)
		    {
		      save_X = tox; save_Y = toy;
		      update_itext_selection(widget);
		      EZ_DrawWidget(widget);
		    }
		}	  
	    }
	}
      break;
    case KeyPress:
      /* => */
      highlighted = EZ_ITextSelectionFlag(widget);
      /* <= */
      keysym = XLookupKeysym((XKeyEvent *)event, 0);
      /* tabing is reserved for now.  */
      if(keysym ==  XK_Tab 
#ifdef XK_KP_Tab
	 || keysym == XK_KP_Tab
#endif	 
	 )
	{
          EZ_FocusNextWidget(widget, (event->xkey.state & ShiftMask));
	  break;
	}
      modifiers = event->xkey.state & (ControlMask | Mod1Mask);
      xevent.xkey.state &= ~modifiers;
      count = XLookupString( &(event->xkey), tmpbuffer, buffersize, &keysym, &compose);
      if(count == 0 && (keysym == XK_Shift_L || keysym == XK_Shift_R)) break;
      tmpbuffer[count] = '\0';
      if(editable) ITextCancelSelection(widget);
      if(modifiers & ControlMask) /* cntrl-key takes precedence over Meta-key */
	{
	  switch(keysym)
	    {
	    case XK_A: case XK_a:   if(editable) EZ_ITextBeginningOfLine(widget);     break;
	    case XK_E: case XK_e:   if(editable) EZ_ITextEndOfLine(widget);           break;
	    case XK_B: case XK_b:   if(editable) EZ_ITextBackwardChar(widget);        break;
	    case XK_F: case XK_f:   if(editable) EZ_ITextForwardChar(widget);         break;	      
	    case XK_P: case XK_p:   EZ_ITextPreviousLine(widget);        break;
	    case XK_N: case XK_n:   EZ_ITextNextLine(widget);            break;
	    case XK_L: case XK_l:   EZ_ITextReDisplay(widget);           break;
	    case XK_V: case XK_v:   EZ_ITextNextPage(widget);            break;
	    case XK_D: case XK_d:   if(editable) EZ_ITextDeleteNextChar(widget);    break;
	    case XK_W: case XK_w:   if(editable) EZ_ITextKillRegion(widget);	       break;
	    case XK_Y: case XK_y:   if(editable) EZ_ITextPasteSelection(widget);     break;
	    case XK_M: case XK_m:   if(editable) EZ_ITextInsertNewLine(widget, 1);   break;
	    case XK_O: case XK_o:   if(editable) EZ_ITextInsertNewLine(widget, 0);   break;
	    case XK_at: case XK_space: if(editable) EZ_ITextSetMarker(widget);          break;
	    case XK_K: case XK_k:   if(editable) EZ_ITextKillEndOfLine(widget);     break;
	    default:                                            	       break;
	    }
	  if(keysym == XK_K || keysym == XK_k)     EZ_ITextLastCmdIsKill(widget) = 1;
	  else       EZ_ITextLastCmdIsKill(widget) = 0;
	}
      else if(modifiers & Mod1Mask)
	{
	  switch(keysym)
	    {
	    case XK_V:  case XK_v:  EZ_ITextPreviousPage(widget);        break;
	    case XK_less:           EZ_ITextBeginningOfBuffer(widget);   break;
	    case XK_greater:        EZ_ITextEndOfBuffer(widget);         break;
	    case XK_F: case XK_f:   if(editable) EZ_ITextForwardWord(widget);         break;
	    case XK_B: case XK_b:   if(editable) EZ_ITextBackwardWord(widget);        break;
	    default:                                                            break;
	    }
	  EZ_ITextLastCmdIsKill(widget) = 0;
	}
      else
	{
	  EZ_ITextLastCmdIsKill(widget) = 0;
	  switch(keysym)
	    {
	    case XK_Left: 
#ifdef XK_KP_Left
	    case XK_KP_Left: 
#endif
	      if(editable) EZ_ITextBackwardChar(widget);	break;
	    case XK_Right:    
#ifdef XK_KP_Right
	    case XK_KP_Right:  
#endif
	      if(editable) EZ_ITextForwardChar(widget);       	break;
	    case XK_Up: 
#ifdef XK_KP_Up
	    case XK_KP_Up:
#endif
              EZ_ITextPreviousLine(widget);       break;
	    case XK_Down: 
#ifdef XK_KP_Down
	    case XK_KP_Down:  
#endif
	      EZ_ITextNextLine(widget);           break;
            case XK_End:
#ifdef XK_KP_End
            case XK_KP_End:
#endif
              EZ_ITextEndOfBuffer(widget);
              break;
            case XK_Home:
#ifdef XK_KP_Home
            case XK_KP_Home:
#endif              
              EZ_ITextBeginningOfBuffer(widget);
              break;
            case XK_Page_Up:
#ifdef XK_KP_Page_Up
            case XK_KP_Page_Up:
#endif
              EZ_ITextPreviousPage(widget);       
              break;
            case XK_Page_Down:
#ifdef XK_KP_Page_Up
            case XK_KP_Page_Down:
#endif
              EZ_ITextNextPage(widget);       
              break;
	    case XK_BackSpace: 
	    case XK_Delete:
	      if(editable)
		{
		  if(highlighted) EZ_ITextKillRegion(widget);
		  else EZ_ITextDeletePreviousChar(widget); 
		}
	      break;
            case XK_Insert:
#ifdef XK_KP_Page_Insert
            case XK_KP_Page_Insert:
#endif
	      if(editable) EZ_ITextPasteSelection(widget);
              break;
	    case XK_Return:
#ifdef XK_KP_Enter
	    case XK_KP_Enter:
#endif
	    case XK_Linefeed: 
	      if(editable) EZ_ITextInsertNewLine(widget, 1);    break;
	    default:
	      if(count)
		{
                  if(editable)
                    {
                      if(highlighted) EZ_ITextKillRegion(widget);
                      EZ_ITextInsertChar(widget,tmpbuffer[0]);
                    }
                  else
                    {
                      if(tmpbuffer[0] == ' ')
                        EZ_ITextNextPage(widget);       
                    }
		}
	      break;
	    }
	}
      break;  /* KeyPress */
    default:
      break;
    }
}

/**************************************************************************
 *
 *   Editing functions.
 *
 *************************************************************************/
/*
 *
 *  skip over invisible chars. Those are the chars with bitmap 
 *  text properties.
 */
static int skip_over_invisible_chars(widget, tline, old_X, dir)
     EZ_Widget *widget; 
     ITextLine *tline;
     int       old_X;
     int       dir; /* 1 for forward, -1 for backward */
{
  TextProp  *cprop, **prop = tline->prop;
  int       length, count;

  count = 1;
  if(dir == 1) /* forward */
    {
      cprop = prop[old_X];
      if(cprop && EZ_TextPropertySpecial(cprop))
	{
	  length = tline->length;
	  while(old_X < length && prop[old_X] == cprop) { count++; old_X++;} count--;
	}
      return(count);
    }
  else
    {
      if(old_X > 0)
	{
	  old_X--;
	  cprop = prop[old_X];
	  if(cprop && EZ_TextPropertySpecial(cprop))
	    {
	      while(old_X >= 0 && prop[old_X] == cprop) { old_X--; count++;} count--;
	    }
	}
      return(count);
    }
}

/*****************************************************************************
 *  Insert a piece of string, possibly with a newline at the end, at
 *  the current position. Modify the current position.
 */
static void  EZ_ITextInsertStringWork(widget, str, length, prop)
     EZ_Widget *widget;
     char      *str;
     int       length;
     TextProp  *prop;
{
  int       XX, YY, has_newline, i;
  ITextLine *tline;
  char      *tstr;
  TextProp  **tprop;

  if(length <= 0) return;

  if(str[length-1] == '\n') { has_newline = 1; length -= 1; }
  else has_newline = 0;

  if(length > 0)
    {
      XX = EZ_ITextXPosition(widget);
      YY = EZ_ITextYPosition(widget);      
      tline = EZ_ITextData(widget)[YY];
      EZ_GrowITextLine(&tline, length);
      EZ_ITextData(widget)[YY] = tline;
      
      /*
       * first, move the piece to the right of point.
       */
      tstr = tline->data;
      tprop = tline->prop;
      for(i = tline->length; i >= XX; i--)
	{
	  tstr[i+length] = tstr[i];
	  tprop[i+length] = tprop[i];
	}
      /*
       * then, insert the string.
       */
      for(i = 0; i < length; i++)
	{
	  tstr[XX+i] = str[i];
	  tprop[XX+i] = prop;
	}
      tline->length += length;
      /*
       * and finally, update the cursor.
       */
      EZ_ITextXPosition(widget) += length;
      EZ_ITextLXPosition(widget) = EZ_ITextXPosition(widget);
      /*
       * update marker 
       */
      if(EZ_ITextYMark(widget) == YY)
	{
	  if(EZ_ITextXMark(widget) > XX)  EZ_ITextXMark(widget) += length;
	}
    }
  if(has_newline)  EZ_ITextInsertNewLine(widget, 1);
  EZ_ITextModified(widget) = 1;
}

/*********************************************************************************
 *
 * Break an arbitary string into segments terminated by '\n'.
 * Insert at the current position and modify current position.
 * str has no text property.
 */
void EZ_ITextInsertString(widget, str)
     EZ_Widget *widget;
     char      *str; 
{
  EZ_ITextInsertStringWithProperty(widget, str, NULL);
}
/*********************************************************************************
 *
 * Break an arbitary string into segments terminated by '\n'.
 * Insert at the current position and modify current position.
 * Str has text property prop.
 */
void EZ_ITextInsertStringWithProperty(widget, str, prop)
     EZ_Widget *widget;
     char      *str; 
     TextProp  *prop;
{
  int       i, count;
  int       str_length, lines_in_str;
  char      *strptr, *tmpstr;

  if(widget != (EZ_Widget *)NULL && EZ_WidgetType(widget) == EZ_WIDGET_ITEXT)
    {
      if(str == (char *)NULL) return;

      /* first, grow the buffer if necessary */
      EZ_CountNewLines(str, &str_length, &lines_in_str);
      EZ_ITextGrowBuffer(widget,  lines_in_str + 1);

      strptr = str;
      if(lines_in_str == 0 || (lines_in_str == 1 && str[str_length -1] == '\n'))
	EZ_ITextInsertStringWork(widget, str, str_length, prop);
      else
	{
	  i = 0;
	  tmpstr = strptr;
	  count = 0;
	  while( i < str_length)
	    {
	      if(*tmpstr == '\n')
		{
		  EZ_ITextInsertStringWork(widget, strptr, count+1, prop);
		  count = 0;
		  tmpstr++;
		  strptr = tmpstr;
		}
	      else
		{
		  tmpstr++;
		  count++;
		}
	      i++;
	      if(i == str_length)
		EZ_ITextInsertStringWork(widget, strptr, count, prop); 
	    }
	}
      EZ_ITextModified(widget) = 1;
    }
}
/*******************************************************************************
 *
 *  Load a text file. 
 */
void  EZ_ITextLoadFile(widget, file)
     EZ_Widget *widget;
     char      *file;
{
  EZ_ITextLoadTextFile(widget, file, 0);
}
void  EZ_ITextLoadAnnotatedFile(widget, file)
     EZ_Widget *widget;
     char      *file;
{
  EZ_ITextLoadTextFile(widget, file, 1);
}
/*******************************************************************************/
static void  EZ_ITextLoadTextFile(widget, file, annotation)
     EZ_Widget *widget;
     char      *file;
     int       annotation;
{
  FILE         *fp, *fopen();
  char         str[1024], qstr[128];
  TextProp     *tprop;
  int          length, special, newproperty;
  /* variables for text property */
  int            fg_set, bg_set, underline, font_id;
  unsigned long  fg, bg;
  EZ_Bitmap      *bitmap;

  if(file && widget && EZ_WidgetType(widget) == EZ_WIDGET_ITEXT)
    {
      EZ_ITextClear(widget);
      if( (fp = fopen(file,"r")) == (FILE *)NULL)
	{
	  sprintf(str, "Cannot open file \"%s\"",file);
	  EZ_ITextInsertString(widget, str);
	}
      else
	{
	  if(annotation == 0)
	    {
	      while(fgets(str,1024,fp))
		EZ_ITextInsertString(widget, str);
	    }
	  else
	    {
	      fg_set = 0;  bg_set = 0;  fg = 0; bg = 0;
	      underline = 0; font_id = -1;
	      newproperty = 1;
	      bitmap = NULL;
	      tprop = (TextProp *)NULL;
	      while(1)
		{
		  if(fgets(str,1024,fp))
		    {
		      special = 0;
		      length = strlen(str);
		      if(str[0] == '.')
			{
			  special = 1;
			  if(!strncmp(str, ".fg ", 4))
			    {
			      grab_next_quoted_string(str+4, qstr);
			      fg =  EZ_AllocateColorFromName(qstr);
			      fg_set = 1;
			      newproperty = 1;
			    }
			  else if(!strncmp(str, ".bg ", 4))
			    {
			      grab_next_quoted_string(str+4, qstr);
			      bg =  EZ_AllocateColorFromName(qstr);
			      bg_set = 1;
			      newproperty = 1;
			    }
			  else if(!strncmp(str, ".font ", 6))
			    {
			      grab_next_quoted_string(str+6, qstr);
			      font_id =  EZ_LoadXFont(qstr);
			      newproperty = 1;
			    }
			  else if(!strncmp(str, ".bitmap ", 8))
			    {
			      grab_next_quoted_string(str+8, qstr);
			      if((bitmap = EZ_GetAnyPixmapFromFile(qstr)))
				newproperty = 1;
			    }
			  else if(!strncmp(str, ".underline", 9))
			    {
			      underline = 1;
			      newproperty = 1;
			    }
			  else if(!strncmp(str, ".default", 8))
			    {
			      fg_set = 0;  bg_set = 0;  fg = 0; bg = 0;
			      underline = 0; font_id = -1; bitmap = NULL;
			      newproperty = 1;
			    }
			  else special = 0;
			}
		      if(special == 0)
			{
			  if(newproperty)
			    {
			      tprop = EZ_GetTextPropertyFromID(font_id, fg_set, bg_set, 
							       underline, fg, bg, bitmap);
			      newproperty = 0;
			    }
			  if(length > 1 && str[length -2] == '\\')
			    {
			      str[length-2] = '\0';
			      EZ_ITextInsertStringWork(widget, str, length-2, tprop); 
			    }
			  else EZ_ITextInsertStringWork(widget, str, length, tprop); 
			}
		    }
		  else break;
		}
	    }
	}
      EZ_ITextBeginningOfBuffer(widget);
      EZ_ITextModified(widget) = 0;
    }
}
/*****************************************************************************/
static void  grab_next_quoted_string(from, to)
     char *from, *to;
{
  while(*from && *from != '\"') from++;  /* " */
  if(*from) from++;


  while(*from && *from != '\"') *to++ = *from++; /* " */
  *to = '\0';
}
/*****************************************************************************/

void  EZ_ITextInsertAnnotatedString(widget, str_in)
     EZ_Widget *widget;
     char      *str_in;
{
  EZ_ITextInsertStringWithProperty(widget, str_in, NULL);
}

/*****************************************************************************/

void EZ_ITextClear(widget)
     EZ_Widget *widget;
{
  if(widget != (EZ_Widget *)NULL && EZ_WidgetType(widget) == EZ_WIDGET_ITEXT)  
    {
      int        i;
      ITextLine  *tline;

      for(i = 1; i < EZ_ITextSpace(widget); i++) 
	{
	  tline = EZ_ITextData(widget)[i];
	  EZ_FreeTextLine(tline);

	  EZ_ITextData(widget)[i] = (ITextLine *)NULL;
	}
      EZ_InitITextWidget(widget);
      EZ_TPHTCollectGarbage();
      if(EZ_WidgetMapped(widget)) EZ_DrawWidget(widget);
    }
}
/*****************************************************************************/
static int tline_is_not_special(tline)
     ITextLine *tline;
{
  TextProp **prop = tline->prop;
  int      i, length = tline->length;
  for(i = 0; i < length; i++)
    if(prop[i] && EZ_TextPropertySpecial(prop[i])) return(0);
  return(1);
}
/*****************************************************************************/
static void  EZ_SetITextCursorPosition(widget, x, y, display)
     EZ_Widget *widget;
     int       x,y, display;
{
  int        i, start, length, length1, error, itmp;
  int        hstart, w_ret, h_ret, a_ret, old_Y;
  ITextLine  *tline, **tlineptr = EZ_ITextData(widget);

  i = EZ_ITextFirstLine(widget);
  tline = tlineptr[i];
  if(y < tline->y && i > 0)
    {
      EZ_ITextFirstLine(widget) -= 1;
      y = tline->y + 2;
      EZ_DrawWidget(widget);
    }
  else
    {
      i = EZ_ITextLastLine(widget);
      tline = tlineptr[i];
      itmp = EZ_ITextNLines(widget) - i;
      if(y > tline->y + tline->height && itmp > 1)
	{
	  EZ_ITextFirstLine(widget) += 1;
	  y = tline->y + 1;
	  EZ_DrawWidget(widget);
	}
    }
  for(i = EZ_ITextFirstLine(widget); i <= EZ_ITextLastLine(widget); i++)
    {
      tline = tlineptr[i];
      if( y > tline->y && y <= tline->y + tline->height)
	{
          int totalCount;
	  hstart = EZ_ITextHStart(widget) + EZ_WidgetPadX(widget)+  EZ_WidgetPadB(widget)+
	    EZ_WidgetBorderWidth(widget); 
	  length = totalCount = tline->count>0? tline->count : tline->length;
	  if(tline_is_not_special(tline))
	    {
	      /* do a binary search to figure out the exact X position */
	      itmp = length >> 1;
	      length1 = totalCount - itmp;
	      length = itmp;
	      start = 0;
	      while(1)
		{
		  if(length > 0)
		    {
		      EZ_ITextLineDimension(widget, tline, start, length, &w_ret, &h_ret, &a_ret);
		      w_ret += hstart;
		      error = w_ret - x;
		      if(error > 0)  /* pointer is at the left of point */
			{
			  itmp = length >> 1;
			  length1 = length - itmp;
			  length = itmp;
			}
		      else if(error < 0) /* pointer is at the right */
			{
			  start += length;
			  hstart = w_ret;
			  length = length1 >> 1;
			  length1 = length1 - length;
			}
		      else
			{
			  start += length;
			  break;
			}
		    }
		  else
		    {
		      start += length;
		      break;
		    }
		} /* end while(1) */
	    }
	  else
	    {
	      /* tline contains special properties. Do a slow search */
	      x -= hstart;
	      while(--length > 0)
		{
		  EZ_ITextLineDimension(widget, tline, 0, length, &w_ret, &h_ret, &a_ret);
		  error = w_ret - x;
		  if(error <= 0) break;
		}
	      start = length;
	    }
	  EZ_ITextXPosition(widget) = start;
	  EZ_ITextLXPosition(widget) = start;
	  old_Y = EZ_ITextYPosition(widget);
	  EZ_ITextYPosition(widget) = i;
	  if(display && EZ_ITextEditable(widget))
	    {
	      if(old_Y != i) EZ_ITextDisplayOneLine(widget, old_Y);
	      EZ_ITextDisplayOneLine(widget, i);
	    }
	  break;
	}
    }
}
/*****************************************************************************/
int  EZ_ITextSetMarker(widget)
     EZ_Widget *widget;
{
  if(widget != (EZ_Widget *)NULL && EZ_WidgetType(widget) == EZ_WIDGET_ITEXT)
    {
      int x = EZ_ITextXPosition(widget);
      int y = EZ_ITextYPosition(widget);
      EZ_ITextXMark(widget) =  x;
      EZ_ITextYMark(widget) =  y;
      return( (x & 0xffff) | (y << 16));
    }
  return(0);
}
/*****************************************************************************/
int  EZ_ITextGetPosition(widget)
     EZ_Widget *widget;
{
  if(widget != (EZ_Widget *)NULL && EZ_WidgetType(widget) == EZ_WIDGET_ITEXT)  
    {
      int x = EZ_ITextXPosition(widget);
      int y = EZ_ITextYPosition(widget);
      return( (x & 0xffff) | (y << 16));
    }
  return(0);
}
void  EZ_ITextGotoPosition(widget, pos)
     EZ_Widget *widget;
     int pos;
{
  if(widget != (EZ_Widget *)NULL && EZ_WidgetType(widget) == EZ_WIDGET_ITEXT)  
    {
      int x = pos & 0xffff;
      int y = pos >> 16;
      EZ_ITextXPosition(widget) = x;
      EZ_ITextYPosition(widget) = y;
      EZ_ITextLXPosition(widget) = x;
      if(EZ_WidgetMapped(widget))  EZ_DrawWidget(widget);
    }
}
/*****************************************************************************/
char  *EZ_ITextGetRegionString(widget) 
     EZ_Widget *widget;
{
  if(widget != (EZ_Widget *)NULL && EZ_WidgetType(widget) == EZ_WIDGET_ITEXT)    
    {
      int markX, markY, XX,YY,length, nlines;
      ITextLine *tline;

      XX = EZ_ITextXPosition(widget);
      YY = EZ_ITextYPosition(widget);
      nlines = EZ_ITextNLines(widget);
      markX = EZ_ITextXMark(widget);
      markY = EZ_ITextYMark(widget);
      if(markY >= 0 && markY < nlines)
	{
	  tline = EZ_ITextData(widget)[markY];
	  length = tline->count>0? tline->count: tline->length;
          if(markX >= 0 && markX < length)  /* so mark is valid */
	    {
	      int fromx, fromy, tox,toy;

	      if(YY < markY) { fromx = XX; fromy = YY; tox = markX; toy = markY;}
	      else if(YY == markY)
		{
		  fromy = toy = YY; 
		  if(XX < markX) { fromx = XX; tox = markX;}
		  else { fromx = markX; tox = XX; }
		}
	      else { fromx = markX; fromy = markY; tox = XX; toy = YY; }
	
	      EZ_ITextCopyToKBufferA(widget, fromx, fromy, tox,toy);
	      return(TWKBufferA.data);
	    }
	}
    }
  return(NULL);
}
/********************************************************************************/
char  *EZ_ITextGetBufferString(widget) 
     EZ_Widget *widget;
{
  if(widget != (EZ_Widget *)NULL && EZ_WidgetType(widget) == EZ_WIDGET_ITEXT)    
    {
      int  nlines, length;
      ITextLine *tline;

      nlines = EZ_ITextNLines(widget);
      if(nlines > 0)
	{
	  tline = EZ_ITextData(widget)[nlines-1];
	  length = tline->length;      
	  if(length > 0)
	    {
	      EZ_ITextCopyToKBufferA(widget, 0, 0, length-1, nlines-1);
	      return(TWKBufferA.data);
	    }
	}
    }
  return(NULL);
}

/*****************************************************************************
 *
 *  Redisplay one line.
 *
 *****************************************************************************/
static void EZ_ITextDisplayOneLine(widget, Y)
     EZ_Widget *widget;
     int        Y;
{
  unsigned long   bgpv;

  if(EZ_WidgetMapped(widget) && (EZ_GetWidgetFreezedFlag(widget))==0)
    {
      if(Y >= EZ_ITextFirstLine(widget) && Y <= EZ_ITextLastLine(widget))
	{
	  if(Y ==  EZ_ITextLastLine(widget))
	    {
	      EZ_DrawWidget(widget); 
	      return;
	    }
	  else
	    {
	      Pixmap     pixmap;
	      GC         gc;
	      int        w, h, maxh, maxw, maxa, bwidth, CY;
	      ITextLine  *tline = EZ_ITextData(widget)[Y];
	      if(tline)
		{
		  EZ_ITextLineDimension(widget, tline, 0, tline->length, &maxw, &maxh, &maxa);
		  if(maxh != tline->height)  /* height has changed, redisplay the whole page */
		    {
		      EZ_DrawWidget(widget); 
		      return;
		    }
		  else
		    {
		      tline->height = maxh;
		      tline->width = maxw;
		    }
		  bwidth = EZ_WidgetBorderWidth(widget) + EZ_WidgetPadB(widget);
		  w = EZ_WidgetWidth(widget) - ( bwidth << 1);
		  CY = EZ_ITextYPosition(widget);
		  if(Y == CY)
		    {
		      int wjunk, hjunk, ajunk;
		      EZ_ITextLineDimension(widget, tline, 0, 
					    EZ_ITextXPosition(widget),
					    &wjunk, &hjunk, &ajunk);
		      wjunk += EZ_ITextHStart(widget);
		      ajunk = w - (EZ_WidgetPadX(widget) << 1);
		      if(wjunk < 0 || wjunk > ajunk ) /* cursor is invisible, scroll horizontally */
			{
			  EZ_ITextHStart(widget) += ( (ajunk >> 1) - wjunk);
			  EZ_DrawWidget(widget); 
			  return;
			}
		    }
		  h = maxh;
		  pixmap = XCreatePixmap(EZ_Display, EZ_WidgetWindow(widget), 
					 w, h, EZ_DepthForWidgets);
		  {
		    if(EZ_ITextTextBG(widget)  != ~0)
		      {
			XSetForeground(EZ_Display, EZ_WRITABLEGC, EZ_ITextTextBG(widget)); 
			gc = EZ_WRITABLEGC;
		      }
		    else 
		      {
			if(EZ_GetBackgroundGC(widget, &gc, &bgpv,0, 0))
			  {
                            int tox, toy, yy = -(tline->y);
                            if(EZ_WidgetTOSet(widget))
                              EZ_GetWidgetTileOrigin(widget, &tox, &toy); 
                            else
                              {
                                int fl = EZ_ITextFirstLine(widget);
                                int lw = EZ_ITextTHeight(widget) + EZ_ITextVSpacing(widget);
                                toy =  -fl * lw;
                                tox = (int)(EZ_ITextHStart(widget));
                              }
			    XSetTSOrigin(EZ_Display, gc, tox-bwidth, toy+yy);
			  }
		      }
		  }
		  XFillRectangle(EZ_Display, pixmap, gc, 0,0, w, h); 	      
		  EZ_ITextRenderText(widget, tline, tline->x - bwidth, 0,
				     maxh,maxa, pixmap, Y, 
				     EZ_ITextXPosition(widget),EZ_ITextYPosition(widget));
		  XCopyArea(EZ_Display,pixmap,EZ_WidgetWindow(widget), EZ_WRITABLEGC,
			    0,0,w,h, bwidth, tline->y);
		  XFreePixmap(EZ_Display, pixmap); 
		  if(EZ_ITextMaxHSize(widget) < maxw) EZ_ITextMaxHSize(widget)= maxw;
		  update_itext_scrolls(widget);
		}
	    }
	}
    }
}
/*****************************************************************************
 *
 *   Cursor movement. 
 *
 *****************************************************************************/
void EZ_ITextPreviousLine(widget)
     EZ_Widget *widget;
{
  if(widget != (EZ_Widget *)NULL && EZ_WidgetType(widget) == EZ_WIDGET_ITEXT)
    {
      int old_Y = EZ_ITextYPosition(widget);
      int old_LX = EZ_ITextLXPosition(widget);

      int old_first = EZ_ITextFirstLine(widget);
      int tmp, redisplay = 0, llen;
      ITextLine *tline;

      if(old_Y <= 0) {EZ_RingBell(); return;}
      if(old_Y <= old_first)
	{
	  int  half_page = (EZ_ITextPageSize(widget) >> 1);
	      
	  old_first -= half_page;
	  old_first = old_first >= 0 ? old_first : 0;
	  EZ_ITextFirstLine(widget) = old_first;
	  redisplay = 1;
	}
      tmp = old_Y - 1;
      EZ_ITextYPosition(widget) = tmp;
      tline = EZ_ITextData(widget)[tmp];
      llen = (tline->count > 0? tline->count: tline->length);
      if(llen -1 < old_LX) old_LX = llen -1;
      EZ_ITextXPosition(widget) = old_LX;

      if(EZ_WidgetMapped(widget)) 
	{
	  if(redisplay) EZ_DrawWidget(widget);
	  else if(EZ_ITextEditable(widget))
	    {
	      EZ_ITextDisplayOneLine(widget, old_Y);
	      EZ_ITextDisplayOneLine(widget, tmp);
	    }
	}
    }
}
/****************************************************************************/
void EZ_ITextNextLine(widget)
     EZ_Widget *widget;
{
  if(widget != (EZ_Widget *)NULL && EZ_WidgetType(widget) == EZ_WIDGET_ITEXT)
    {
      int old_Y = EZ_ITextYPosition(widget);
      int old_LX = EZ_ITextLXPosition(widget);
      int old_first = EZ_ITextFirstLine(widget);
      int old_last = EZ_ITextLastLine(widget);
      int nlines = EZ_ITextNLines(widget) - 1;
      int tmp, redisplay = 0, llen;
      ITextLine *tline;

      if(old_Y >= nlines) {EZ_RingBell(); return;}

      if(old_last < nlines)
	{
	  if(old_Y >= old_last -1)  /* up half page */
	    {
	      int  half_page = (EZ_ITextPageSize(widget) >> 1);
	      
	      old_first += half_page;
	      old_first = old_first <= nlines ? old_first : nlines;
	      EZ_ITextFirstLine(widget) = old_first;
	      redisplay = 1;
	    }
	}
      tmp = old_Y + 1;
      EZ_ITextYPosition(widget) = tmp;
      tline = EZ_ITextData(widget)[tmp];
      llen = tline->count > 0? tline->count : tline->length;
      if(llen -1 < old_LX) old_LX = llen-1;
      EZ_ITextXPosition(widget) = old_LX;

      if(EZ_WidgetMapped(widget)) 
	{
	  if(redisplay) EZ_DrawWidget(widget);
	  else if(EZ_ITextEditable(widget))
	    {
	      EZ_ITextDisplayOneLine(widget, old_Y);
	      EZ_ITextDisplayOneLine(widget, tmp);
	    }
	}
    }
}
/*****************************************************************************
 *
 *   page up. Since we are not using a fixed font, we have to compute in order
 *   to do an exact one page up.  Here, we just do a 3/4 page up.
 */
void EZ_ITextPreviousPage(widget)
     EZ_Widget *widget;
{
  if(widget != (EZ_Widget *)NULL && EZ_WidgetType(widget) == EZ_WIDGET_ITEXT)
    {
      int new_first, old_first = EZ_ITextFirstLine(widget);
      int padb = EZ_WidgetPadB(widget);
      int bw = EZ_WidgetBorderWidth(widget);
      int pady = EZ_WidgetPadY(widget);
      int totalheight = EZ_WidgetHeight(widget) - ((pady + padb + bw)<<1);
      int hh = 0;
      register ITextLine **lines, *tline;       

      if(old_first == 0) { EZ_RingBell(); return; }
      lines = EZ_ITextData(widget);
      new_first = old_first;
      while(new_first > 0 && hh < totalheight)
        {
	  int len, htmp,wtmp, junk;
          tline = lines[new_first];
          len =  tline->length;	  
	  EZ_ITextLineDimension(widget, tline, 0, len, &wtmp, &htmp, &junk);
          hh += htmp;
          new_first--;
        }
      new_first++;
      if(new_first == old_first) new_first--;
      if(new_first < 0) new_first = 0;
      
      EZ_ITextFirstLine(widget) = new_first;
      EZ_ITextYPosition(widget) = new_first;
      EZ_ITextXPosition(widget) = 0;

      if(EZ_WidgetMapped(widget))  EZ_DrawWidget(widget);
    }
}

/*****************************************************************************
 *
 * page down is simpler. Just make the last_line the first_line.
 */
void EZ_ITextNextPage(widget)
     EZ_Widget *widget;
{
  if(widget != (EZ_Widget *)NULL && EZ_WidgetType(widget) == EZ_WIDGET_ITEXT)
    {
      int tmp, tmp1;
      tmp = EZ_ITextLastLine(widget);
      tmp1 = EZ_ITextFirstLine(widget);
      if(tmp1 == tmp) tmp++;
      if(tmp == EZ_ITextNLines(widget) - 1) { EZ_RingBell(); return;}
      EZ_ITextFirstLine(widget) = tmp;

      EZ_ITextYPosition(widget) = tmp;
      EZ_ITextXPosition(widget) = 0;

      if(EZ_WidgetMapped(widget))  EZ_DrawWidget(widget);
    }
}
/*****************************************************************************
 *
 *   Forward a character.
 */
void EZ_ITextForwardChar(widget)
     EZ_Widget *widget;
{
  if(widget != (EZ_Widget *)NULL && EZ_WidgetType(widget) == EZ_WIDGET_ITEXT)
    {
      int old_X = EZ_ITextXPosition(widget);
      int old_Y = EZ_ITextYPosition(widget);
      int old_last, nlines, nchars;
      int redisplay = 0;
      ITextLine *tline;

      tline = EZ_ITextData(widget)[old_Y];
      nchars = skip_over_invisible_chars(widget,tline, old_X, 1);
      old_X += nchars;
      if(old_X < tline->length) 
	{
	  EZ_ITextXPosition(widget) = old_X; 
	  EZ_ITextLXPosition(widget) = old_X;
	}
      else
	{
	  old_last = EZ_ITextLastLine(widget);

	  if(old_Y >=  old_last)  /* scroll up */
	    {
	      nlines = EZ_ITextNLines(widget) - 1;
	      if(old_last == nlines) {EZ_RingBell(); return;}
	      else  /* up half page */
		{
		  int old_first = EZ_ITextFirstLine(widget);
		  int half_page = (EZ_ITextPageSize(widget) >> 1);
	      
		  old_first += half_page;
		  old_first = old_first <= nlines ? old_first : nlines;
		  EZ_ITextFirstLine(widget) = old_first;
		  redisplay = 1;
		}
	    }
	  EZ_ITextYPosition(widget) += 1;
	  EZ_ITextXPosition(widget) = 0;
	  EZ_ITextLXPosition(widget) = 0;
	}

      if(EZ_WidgetMapped(widget)) 
	{
	  if(redisplay) EZ_DrawWidget(widget);
	  else
	    {
	      int tmp = EZ_ITextYPosition(widget);
	      if( tmp != old_Y) EZ_ITextDisplayOneLine(widget, tmp);
	      EZ_ITextDisplayOneLine(widget, old_Y);
	    }
	}
    }
}

/*****************************************************************************
 *
 *   Backward a character.
 */
void EZ_ITextBackwardChar(widget)
     EZ_Widget *widget;
{
  if(widget != (EZ_Widget *)NULL && EZ_WidgetType(widget) == EZ_WIDGET_ITEXT)
    {
      int old_X = EZ_ITextXPosition(widget);
      int old_Y = EZ_ITextYPosition(widget);
      int old_first;
      int tmp, nchars, redisplay = 0;
      ITextLine *tline;

      tline = EZ_ITextData(widget)[old_Y];
      nchars = skip_over_invisible_chars(widget,tline, old_X, -1);
      old_X -= nchars;
      if(old_X >= 0)
	{
	  EZ_ITextXPosition(widget) = old_X; 
	  EZ_ITextLXPosition(widget) = old_X; 
	}
      else
	{
	  old_first = EZ_ITextFirstLine(widget);

	  if(old_Y <=  old_first)  /* down half page */
	    {
	      if(old_first == 0) {EZ_RingBell(); return;}
	      else 
		{
		  int half_page = (EZ_ITextPageSize(widget) >> 1);
	      
		  old_first -= half_page;
		  old_first = old_first >= 0 ? old_first : 0;
		  EZ_ITextFirstLine(widget) = old_first;
		  redisplay = 1;
		}
	    }
	  tmp = EZ_ITextYPosition(widget) - 1;
	  EZ_ITextYPosition(widget) = tmp; 
 	  tline = EZ_ITextData(widget)[tmp];
	  EZ_ITextXPosition(widget) = tline->length - 1;
	  EZ_ITextLXPosition(widget) = tline->length - 1;
	}

      if(EZ_WidgetMapped(widget)) 
	{
	  if(redisplay) EZ_DrawWidget(widget);
	  else
	    {
	      int tmp = EZ_ITextYPosition(widget);
	      if( tmp != old_Y) EZ_ITextDisplayOneLine(widget, tmp);
	      EZ_ITextDisplayOneLine(widget, old_Y);
	    }
	}
    }
}
/*****************************************************************************
 *
 *   Move to the beginning of the line.
 */
void EZ_ITextBeginningOfLine(widget)
     EZ_Widget *widget;
{
  if(widget != (EZ_Widget *)NULL && EZ_WidgetType(widget) == EZ_WIDGET_ITEXT)
    {
      int old_Y = EZ_ITextYPosition(widget);

      EZ_ITextXPosition(widget) = 0;
      EZ_ITextLXPosition(widget) = 0;

      if(EZ_WidgetMapped(widget))
	EZ_ITextDisplayOneLine(widget, old_Y);
    }
}

/*****************************************************************************
 *
 *   Move to the end of the line.
 */
void EZ_ITextEndOfLine(widget)
     EZ_Widget *widget;
{
  if(widget != (EZ_Widget *)NULL && EZ_WidgetType(widget) == EZ_WIDGET_ITEXT)
    {
      int old_Y = EZ_ITextYPosition(widget);
      ITextLine  *tline;

      tline = EZ_ITextData(widget)[old_Y];
      EZ_ITextXPosition(widget) = tline->length - 1;
      EZ_ITextLXPosition(widget) = tline->length - 1;

      if(EZ_WidgetMapped(widget))
	EZ_ITextDisplayOneLine(widget, old_Y);
    }
}
/*****************************************************************************
 *
 *   Move to the beginning of the buffer.
 */
void EZ_ITextBeginningOfBuffer(widget)
     EZ_Widget *widget;
{
  if(widget != (EZ_Widget *)NULL && EZ_WidgetType(widget) == EZ_WIDGET_ITEXT)
    {
      EZ_ITextFirstLine(widget) = 0;
      EZ_ITextYPosition(widget) = 0;
      EZ_ITextXPosition(widget) = 0;
      EZ_ITextLXPosition(widget) = 0;
      if(EZ_WidgetMapped(widget)) EZ_DrawWidget(widget);
    }
}

/*****************************************************************************
 *
 *   Move to the end of the buffer.
 */
void EZ_ITextEndOfBuffer(widget)
     EZ_Widget *widget;
{
  if(widget != (EZ_Widget *)NULL && EZ_WidgetType(widget) == EZ_WIDGET_ITEXT)
    {
      ITextLine *tline;
      int tmp, nlines = EZ_ITextNLines(widget);

      /*
      if(nlines >= 5) tmp = nlines - 5;
      else if(nlines >= 3) tmp = nlines - 3;
      else tmp = 0;
      */
      if(nlines)
	{
	  nlines -= 1;
	  tline = EZ_ITextData(widget)[nlines];
	  /* EZ_ITextFirstLine(widget) = tmp;*/
	  EZ_ITextYPosition(widget) = nlines;
	  tmp = tline->length -1;
	  EZ_ITextXPosition(widget) = tmp;
	  EZ_ITextLXPosition(widget) = tmp;
	  if(EZ_WidgetMapped(widget)) EZ_DrawWidget(widget);
	}
    }
}
/*****************************************************************************/ 	      
void EZ_ITextReDisplay(widget)
     EZ_Widget *widget;
{
  if(widget != (EZ_Widget *)NULL && EZ_WidgetType(widget) == EZ_WIDGET_ITEXT)  
    {
      if(EZ_ITextLastLine(widget) < EZ_ITextNLines(widget) - 1)
	{
	  int oldY = EZ_ITextYPosition(widget);
	  int halfPage = (EZ_ITextPageSize(widget)) >> 1;
	  int first = oldY - halfPage;
	  if(first < 0) first = 0;
	  EZ_ITextFirstLine(widget) = first;
	}
      if(EZ_WidgetMapped(widget)) EZ_DrawWidget(widget);
    }
}
/*****************************************************************************
 *
 *  character insertion
 *
 *****************************************************************************/
void EZ_ITextInsertChar(widget,theChar)
     EZ_Widget *widget;
     char      theChar;
{
  if(widget != (EZ_Widget *)NULL && EZ_WidgetType(widget) == EZ_WIDGET_ITEXT)
    {
      int       oldX, oldY, markX, markY, length, i;
      ITextLine *tline;
      char      *str;
      TextProp  **tprop;

      oldX = EZ_ITextXPosition(widget);
      oldY = EZ_ITextYPosition(widget);
      tline = EZ_ITextData(widget)[oldY];
      /*
       * first grow the line if necessary
       */
      EZ_GrowITextLine(&tline, 4);
      EZ_ITextData(widget)[oldY] = tline;

      /*
       * now insert the character.
       */
      length = tline->length;
      str   = tline->data;
      tprop = tline->prop;
      for(i = length; i >= oldX; i--)
	{
	  str[i+1] = str[i];
	  tprop[i+1] = tprop[i];
	}
      str[oldX] = theChar;
      if(oldX > 0)  /* rear sticky, but ignore speical properties */
	tprop[oldX] = EZ_CopyTextPropIgnoreSpecial(tprop[oldX-1]);
      else 
	{
	  if(oldY > 0)
	    {
	      ITextLine *ttline = EZ_ITextData(widget)[oldY -1];
	      TextProp  **ssprop = ttline->prop;
	      int       uulength = ttline->length - 1;
	      tprop[oldX] = EZ_CopyTextPropIgnoreSpecial(ssprop[uulength]);
	    }
	  else tprop[oldX] = (TextProp *)NULL;
	}
      EZ_ITextXPosition(widget) += 1;      
      EZ_ITextLXPosition(widget) = EZ_ITextXPosition(widget);
      tline->length += 1;
      /*
       * update mark
       */
      markY = EZ_ITextYMark(widget);
      if(markY == oldY)
	{
	  markX = EZ_ITextXMark(widget);
	  if(markX > oldX) EZ_ITextXMark(widget) += 1;
	}
      if(EZ_WidgetMapped(widget)) EZ_ITextDisplayOneLine(widget, oldY);
      EZ_ITextModified(widget) = 1;
    }
}
/*****************************************************************************/
void EZ_ITextInsertNewLine(widget, moveCur)
     EZ_Widget *widget;
     int       moveCur;
{
  if(widget != (EZ_Widget *)NULL && EZ_WidgetType(widget) == EZ_WIDGET_ITEXT)
    {
      int       oldX, oldY, markX, markY, length, tlength, afterpoint, i;
      ITextLine **linelist, *tline, *tmptline = NULL;
      char      *str, *tmpstr;
      TextProp  **tprop, **tmptprop;

      EZ_ITextGrowBuffer(widget, 16);
      oldX = EZ_ITextXPosition(widget);
      oldY = EZ_ITextYPosition(widget);
      tline = EZ_ITextData(widget)[oldY];
      length = tline->length;
      afterpoint = length - oldX;

      /*
       *  create a new line 
       */
      tlength = afterpoint + 4;
      EZ_GrowITextLine(&tmptline, tlength);
      str = tline->data;
      tprop = tline->prop;
      tmpstr = tmptline->data;
      tmptprop = tmptline->prop;
      for(i = oldX; i <= length; i++)
	{
	  tmpstr[i-oldX] = str[i];
	  tmptprop[i-oldX] = tprop[i];
	}
      tmptline->length = length - oldX;
      /*
       * fix the current line
       */
      str[oldX] = '\n';
      str[oldX+1] = '\0';
      tline->length = oldX + 1;
      /*
       * text property is rear sticky.
       * special properties are not sticky across newlines. !!!!!!!!
       */
      if(oldX > 0)
	{ 
	  tprop[oldX] = EZ_CopyTextPropIgnoreSpecial(tprop[oldX - 1]);
	}
      else if(oldY > 0)
	{
	  ITextLine *ttline = EZ_ITextData(widget)[oldY -1];
	  TextProp  **ssprop = ttline->prop;
	  int       uulength = ttline->length - 1;
	  tprop[oldX] = EZ_CopyTextPropIgnoreSpecial(ssprop[uulength]);
	}
      else tprop[oldX] = (TextProp *)NULL;
      
      /*
       * fix the buffer
       */
      linelist = EZ_ITextData(widget);
      for(i = EZ_ITextNLines(widget); i > oldY + 1; i--)
	linelist[i] = linelist[i - 1];
      linelist[oldY + 1] = tmptline;
      EZ_ITextNLines(widget) += 1;

      /*
       * update mark
       */
      markY = EZ_ITextYMark(widget);
      if(markY == oldY)
	{
	  markX = EZ_ITextXMark(widget);
	  if(markX > oldX)
	    {
	      EZ_ITextYMark(widget) += 1;
	      EZ_ITextXMark(widget) = markX - oldX;
	    }
	}
      else if(markY > oldY) EZ_ITextYMark(widget) += 1;
      
      /*
       * update the cursor
       */
      if(moveCur)
	{
	  EZ_ITextXPosition(widget) = 0;
	  EZ_ITextLXPosition(widget) = 0;
	  EZ_ITextYPosition(widget) += 1;
	}
      /*
       *  and redisplay the buffer
       */
      if(EZ_WidgetMapped(widget)) EZ_DrawWidget(widget);
      EZ_ITextModified(widget) = 1;
    }
}
/*****************************************************************************
 *
 *  Delete a character
 *
 *****************************************************************************/
void EZ_ITextDeletePreviousChar(widget)
     EZ_Widget *widget;
{
  if(widget != (EZ_Widget *)NULL && EZ_WidgetType(widget) == EZ_WIDGET_ITEXT)
    {
      int       newX, oldX, oldY, markX, markY, length, i, nchars, redisplay = 0;
      ITextLine *tline;
      char      *str;
      TextProp  **tprop;

      oldX = EZ_ITextXPosition(widget);
      oldY = EZ_ITextYPosition(widget);
      markY = EZ_ITextYMark(widget);
      tline = EZ_ITextData(widget)[oldY];
      length = tline->length;
      str   = tline->data;
      tprop = tline->prop;      
      if(oldX > 0)
	{
	  nchars = skip_over_invisible_chars(widget,tline, oldX, -1);
	  newX = oldX - nchars;  /* new cursor position if >= 0 */
	  for(i = oldX; i <= length; i++) /* copy the NULL char also */
	    {
	      str[i-nchars] = str[i];
	      tprop[i-nchars] = tprop[i];
	    }
	  EZ_ITextXPosition(widget) = newX;
	  EZ_ITextLXPosition(widget) = newX;
	  tline->length -= nchars; 
	  /*
	   * update mark
	   */
	  if(markY == oldY)
	    {
	      markX = EZ_ITextXMark(widget);
	      if(markX >= oldX) EZ_ITextXMark(widget) -= nchars;
	    }
	}
      else /* cursor is at the beginning of the line */
	{
	  if(oldY > 0)
	    {
	      ITextLine *tmptline, **linelist;
	      TextProp  **tmptprop;
	      char      *tmpstr;
	      int       itmp, tmplength;

	      tmptline = EZ_ITextData(widget)[oldY - 1];
	      tmplength = tmptline->length - 1;
	      itmp = length + 4;
	      EZ_GrowITextLine(&tmptline, itmp);
	      EZ_ITextData(widget)[oldY - 1] = tmptline;
	      tmpstr = tmptline->data;
	      tmptprop = tmptline->prop;

	      for(i = 0; i <= length; i++)
		{
		  tmpstr[i+tmplength] = str[i];
		  tmptprop[i+tmplength] = tprop[i];
		}
	      EZ_ITextXPosition(widget) = tmplength;	      
	      EZ_ITextLXPosition(widget) = tmplength;	      
	      EZ_ITextYPosition(widget) -= 1; 
	      tmptline->length += length - 1; 

	      EZ_FreeTextLine(tline);

	      itmp = EZ_ITextNLines(widget) -1;
	      linelist = EZ_ITextData(widget);
	      for(i = oldY; i < itmp; i++)
		linelist[i] = linelist[i+1];
	      linelist[itmp] = (ITextLine *)NULL;

	      EZ_ITextNLines(widget) -= 1;
	      redisplay = 1;
	      /*
	       * update mark
	       */
	      if(markY == oldY)
		{
		  markX = EZ_ITextXMark(widget);
		  EZ_ITextXMark(widget) += tmplength;
		  EZ_ITextYMark(widget) -= 1;
		}
	      else if(markY > oldY) EZ_ITextYMark(widget) -= 1;
	    }
	  else {EZ_RingBell(); return;}
	}
      if(EZ_WidgetMapped(widget))
	{
	  if(redisplay) EZ_DrawWidget(widget);
	  else EZ_ITextDisplayOneLine(widget, oldY);
	}
      EZ_ITextModified(widget) = 1;      
    }
}
/*****************************************************************************/
void EZ_ITextDeleteNextChar(widget)
     EZ_Widget *widget;
{
  if(widget != (EZ_Widget *)NULL && EZ_WidgetType(widget) == EZ_WIDGET_ITEXT)
    {
      int       newX, oldX, oldY, markX, markY, length, i, nchars, itmp, redisplay = 0;
      ITextLine *tline;
      char      *str;
      TextProp  **tprop;

      oldX = EZ_ITextXPosition(widget);
      oldY = EZ_ITextYPosition(widget);
      markY = EZ_ITextYMark(widget);
      tline = EZ_ITextData(widget)[oldY];
      nchars = skip_over_invisible_chars(widget,tline, oldX, 1);
      newX = oldX + nchars; /* new cursor position if < length */
      length = tline->length;
      str   = tline->data;
      tprop = tline->prop;

      if(newX <= length - 1) /* operate on one line */
	{
	  itmp = length - nchars;
	  for(i = oldX; i <= itmp; i++)
	    {
	      str[i] = str[i+nchars];
	      tprop[i] = tprop[i+nchars];
	    }
	  tline->length -= nchars;
	  /*
	   * update mark
	   */
	  if(markY == oldY)
	    {
	      markX = EZ_ITextXMark(widget);	      
	      if(markX > oldX) EZ_ITextXMark(widget) -= nchars;
	    }
	}
      else /* delete to end of line */
	{
	  if(oldY < EZ_ITextNLines(widget) - 1)
	    {
	      ITextLine *tmptline, **linelist;
	      TextProp  **tmptprop;
	      char      *tmpstr;
	      int       itmp, tmplength;

	      tmptline = EZ_ITextData(widget)[oldY + 1];
	      tmplength = tmptline->length;
	      tmpstr = tmptline->data;
	      tmptprop = tmptline->prop;
	      itmp = tmplength + 4;
	      EZ_GrowITextLine(&tline, itmp);
	      EZ_ITextData(widget)[oldY] = tline;

	      str   = tline->data;
	      tprop = tline->prop;      
	      for(i = 0; i <= tmplength; i++)
		{
		  str[i+oldX] = tmpstr[i];
		  tprop[i+oldX] = tmptprop[i];
		}
	      tline->length = oldX + tmplength;
	      EZ_FreeTextLine(tmptline);

	      linelist = EZ_ITextData(widget);
	      itmp =  EZ_ITextNLines(widget)-1;
	      for(i = oldY+1; i < itmp; i++)
		linelist[i] = linelist[i+1];
	      linelist[itmp] = (ITextLine *)NULL;

	      EZ_ITextNLines(widget) -= 1;
	      redisplay = 1;

	      /*
	       * update mark
	       */
	      if(markY == oldY + 1)
		{
		  markX = EZ_ITextXMark(widget); 
		  EZ_ITextXMark(widget) += oldX;
		  EZ_ITextYMark(widget) -= 1;
		}
	      else if(markY > oldY + 1)  EZ_ITextYMark(widget) -= 1;
	    }
	  else {EZ_RingBell(); return;}
	}
      if(EZ_WidgetMapped(widget))
	{
	  if(redisplay) EZ_DrawWidget(widget);
	  else EZ_ITextDisplayOneLine(widget, oldY);
	}
      EZ_ITextModified(widget) = 1;
    }
}

/*****************************************************************************/

void EZ_ITextKillEndOfLine(widget)
     EZ_Widget *widget;
{
  if(widget != (EZ_Widget *)NULL && EZ_WidgetType(widget) == EZ_WIDGET_ITEXT)    
    {
      int markX, markY, XX,YY,length;
      ITextLine *tline;
      char   *str;
      TextProp **tprop;

      XX = EZ_ITextXPosition(widget);
      YY = EZ_ITextYPosition(widget);
      tline = EZ_ITextData(widget)[YY];
      length = tline->length;

      /* copy to kill buffer first */
      if(XX < length -1 || YY < EZ_ITextNLines(widget) - 1)
	EZ_ITextCopyToKillBuffer(widget, XX, YY, 
				 (XX < length -1 ? length - 1: length),YY);
      /* now do the kill */
      if(XX < length - 1)
	{
	  str = tline->data;
	  tprop = tline->prop;

	  str[XX] = '\n';
	  str[XX+1] = '\0';
	  tprop[XX] = tprop[length-1];
	  tline->length = XX+1;
	  /*
	   * update mark
	   */
	  markY = EZ_ITextYMark(widget);
	  if(markY == YY)
	    {
	      markX = EZ_ITextXMark(widget);
	      if(markX > XX) EZ_ITextXMark(widget) = XX;
	    }
	  if(EZ_WidgetMapped(widget)) EZ_ITextDisplayOneLine(widget, YY);

	}
      else
	{
	  EZ_ITextDeleteNextChar(widget);
	}
      EZ_ITextModified(widget) = 1;
    }
}
/*****************************************************************************/
void EZ_ITextKillRegion(widget)
     EZ_Widget *widget;
{
  if(widget != (EZ_Widget *)NULL && EZ_WidgetType(widget) == EZ_WIDGET_ITEXT)    
    {
      int markX, markY, XX,YY,length, nlines, i;
      ITextLine *fline, *tline;

      XX = EZ_ITextXPosition(widget);
      YY = EZ_ITextYPosition(widget);
      nlines = EZ_ITextNLines(widget);
      markX = EZ_ITextXMark(widget);
      markY = EZ_ITextYMark(widget);
      if(markY >= 0 && markY < nlines)
	{
	  tline = EZ_ITextData(widget)[markY];
	  length = tline->length;
	  if(markX >= 0 && markX < length)  /* so mark is valid */
	    {
	      int fromx, fromy, tox,toy, tmp;
	      char   *str, *tmpstr;
	      TextProp **tprop, **tmptprop;
	      
	      if(YY < markY) { fromx = XX; fromy = YY; tox = markX; toy = markY;}
	      else if(YY == markY)
		{
		  fromy = toy = YY; 
		  if(XX < markX) { fromx = XX; tox = markX;}
		  else { fromx = markX; tox = XX; }
		}
	      else { fromx = markX; fromy = markY; tox = XX; toy = YY; }
	      /*
	       * update mark first.
	       */
	      EZ_ITextXMark(widget) = fromx;
	      EZ_ITextYMark(widget) = fromy;
	      EZ_ITextXPosition(widget) = fromx;
	      EZ_ITextLXPosition(widget) = fromx;
	      EZ_ITextYPosition(widget) = fromy;

	      EZ_ITextCopyToKillBuffer(widget, fromx, fromy, tox,toy);

	      /*
	       *  Combine the fromline and toline to fromline first.
	       */
	      fline = EZ_ITextData(widget)[fromy];
	      tline = EZ_ITextData(widget)[toy];
	      length = tline->length - tox + fromx - fline->length + 4;
	      EZ_GrowITextLine(&fline, length);
	      EZ_ITextData(widget)[fromy] = fline;
	      tline = EZ_ITextData(widget)[toy];    /* !!! */

	      str = fline->data;
	      tprop = fline->prop;
	      tmpstr = tline->data;
	      tmptprop = tline->prop;
	      tmp = fromx - tox;
	      for(i = tox; i <= tline->length; i++)
		{
		  str[i+tmp] = tmpstr[i];
		  tprop[i+tmp] = tmptprop[i];
		}
	      fline->length = tmp + tline->length;
	      /*
	       * kill all lines from fromy+1 to toy
	       * and fix the buffer.
	       */
	      tmp = toy - fromy;
	      if(tmp > 0)
		{
		  for(i = fromy + 1; i <= toy; i++)
		    EZ_FreeTextLine(EZ_ITextData(widget)[i]);
	      
		  for(i = toy + 1;  i < EZ_ITextNLines(widget); i++)
		    EZ_ITextData(widget)[i - tmp] = EZ_ITextData(widget)[i];

		  for(i = EZ_ITextNLines(widget) - tmp; i < EZ_ITextNLines(widget); i++)
		    EZ_ITextData(widget)[i] = (ITextLine *)NULL;

		  EZ_ITextNLines(widget) -= tmp;
		}
	      if(EZ_WidgetMapped(widget)) EZ_DrawWidget(widget);
	      return;
	    }
	}
      EZ_RingBell();
      EZ_ITextModified(widget) = 1;
    }
}
/*****************************************************************************/
int  EZ_ITextGrowBuffer(widget,  increment)
     EZ_Widget *widget;
     int       increment;
{
  if(widget != (EZ_Widget *)NULL && EZ_WidgetType(widget) == EZ_WIDGET_ITEXT)  
    {
      int nlines = EZ_ITextNLines(widget);
      int space = EZ_ITextSpace(widget);
      int i, tmp = nlines + increment + 1;

      if(space <= tmp)
	{
	  tmp = MAXV(512 + space, tmp);
	  EZ_ITextData(widget) =
	    (ITextLine **)my_realloc(EZ_ITextData(widget), tmp * sizeof(ITextLine *),  _ITEXT_D_);
	  if(!(EZ_ITextData(widget))) EZ_OutOfMemory("EZ_ITextGrowBuffer");
	  for(i = space; i < tmp; i++)
	    EZ_ITextData(widget)[i] = (ITextLine *)NULL;
	  EZ_ITextSpace(widget) = tmp;
          return(1);
	}
    }
  return(0);
}
/*****************************************************************************/

void EZ_ITextCopyToKillBuffer(widget, fromx, fromy, tox,toy)
     EZ_Widget *widget;
     int       fromx, fromy, tox, toy;
{
  int cpstart, cpend, i, j, tmp, newsize;
  ITextLine *tline, **lines = EZ_ITextData(widget);
  char      *str;
  TextProp  **tprop;

  newsize = 0;
  tmp = 0;
  for(i = fromy; i <= toy; i++)
    {
      if(lines[i]->count) newsize += lines[i]->count;
      else  newsize += lines[i]->length + 1;
    }
  if(EZ_ITextLastCmdIsKill(widget))
    {
      tmp = TextWidgetKillBuffer.length;
      newsize += tmp;
    }
  EZ_GrowITextKillBuffer(&TextWidgetKillBuffer,newsize);
  for(i = fromy; i <= toy; i++)
    {
      tline = EZ_ITextData(widget)[i];
      cpstart = 0; 
      cpend = tline->count>0 ? tline->count : tline->length;
      if(i == fromy)  cpstart = fromx; 
      if(i == toy)  cpend = tox;
      
      str = tline->data;
      tprop = tline->prop;
      for(j = cpstart; j < cpend; j++, tmp++)
	{
	  (TextWidgetKillBuffer.data)[tmp] = str[j];
	  (TextWidgetKillBuffer.prop)[tmp] = tprop[j];
	}
    }
  (TextWidgetKillBuffer.data)[tmp] = '\0';
  TextWidgetKillBuffer.length = tmp;
  TextWidgetKillBuffer.width = 1;
  EZ_SetSelectionOwner(widget);
}
/********************************************************************************/
static void EZ_ITextCopyToKBufferA(widget, fromx, fromy, tox,toy)
     EZ_Widget *widget;
     int       fromx, fromy, tox, toy;
{
  int cpstart, cpend, i, j, tmp, newsize;
  ITextLine *tline, **lines = EZ_ITextData(widget);
  char      *str;
  TextProp  **tprop;

  newsize = 0;
  tmp = 0;
  for(i = fromy; i <= toy; i++)
    {
      if(lines[i]->count) newsize += lines[i]->count;
      else  newsize += lines[i]->length + 1;
    }
  EZ_GrowITextKillBuffer(&TWKBufferA,newsize);
  for(i = fromy; i <= toy; i++)
    {
      tline = EZ_ITextData(widget)[i];
      cpstart = 0; 
      cpend = tline->count>0 ? tline->count : tline->length;
      if(i == fromy)  cpstart = fromx; 
      if(i == toy)  cpend = tox;
      
      str = tline->data;
      tprop = tline->prop;
      for(j = cpstart; j < cpend; j++, tmp++)
	{
	  (TWKBufferA.data)[tmp] = str[j];
	  (TWKBufferA.prop)[tmp] = tprop[j];
	}
    }
  (TWKBufferA.data)[tmp] = '\0';
  TWKBufferA.length = tmp;
  TWKBufferA.width = 1;
}
     

/*****************************************************************************/
static void EZ_ITextYank(widget)
     EZ_Widget *widget;
{
  ITextLine *tline = &TextWidgetKillBuffer;
  char *tmpstr, *str = tline->data;
  TextProp *prop, **tprop = tline->prop;
  int i, tlen, length = tline->length;
  int has_tprop = tline->width;

  EZ_FreezeWidget(widget);
  if(has_tprop) prop = tprop[0];
  else prop = (TextProp *)NULL;
  tmpstr = str;
  i = 0;
  while(1)
    {
      tlen = 0;
      while( i < length && (!has_tprop || tprop[i] == prop)  && str[i] != '\n') { i++; tlen++;}
      if( i == length) /* the last chunk */
	{
	  if(tlen > 0) EZ_ITextInsertStringWork(widget, tmpstr, tlen, prop); 
	  break;
	}
      else if(str[i] == '\n')
	{
	  EZ_ITextInsertStringWork(widget, tmpstr, tlen+1, prop);
	  i++;
	  tmpstr = str + i;
	  if(has_tprop) prop = tprop[i];
	}
      else
	{
	  EZ_ITextInsertStringWork(widget, tmpstr, tlen, prop);
	  tmpstr = str + i;
	  prop = tprop[i];
	}
    }
  EZ_UnFreezeWidget(widget);	  
  EZ_ITextModified(widget) = 1;
}
/***********************************************************************/

void EZ_ITextPasteSelection(widget)
     EZ_Widget *widget;
{
  Window owner;
  EZ_Widget *tmp;

  owner = XGetSelectionOwner(EZ_Display, XA_PRIMARY);
  tmp = EZ_LookupWidgetFromMappedHT(owner);
  if(tmp != (EZ_Widget *)NULL)
    EZ_ITextYank(widget);      
  else if(owner != None)
    {
      Atom property = XInternAtom(EZ_Display,"VT_SELECTION",False);
      /*------------------------------
       * have to go through the server
       *------------------------------*/
      XConvertSelection(EZ_Display,
			XA_PRIMARY,   /* the selection */
			XA_STRING,    /* the target    */
			property,     /* property      */
			EZ_WidgetWindow(widget),  /* the requestor */
			CurrentTime);
      /*------------------------------------------
       * Now return. The EventHandling procedure
       * will catch the SelectionNotify event and
       * act appropriately.
       *-----------------------------------------*/
    }
  else /* paste from CutBuffer */
    EZ_PasteSelection(widget, 
		      DefaultRootWindow(EZ_Display),
		      XA_CUT_BUFFER0,
		      False,
		      EZ_ITextInsertString);
  EZ_ITextModified(widget) = 1;
}


/**********************************************************/
void EZ_ITextSetRegionProperty(widget, property)
     EZ_Widget *widget;
     TextProp  *property;
{
  if(widget != (EZ_Widget *)NULL && EZ_WidgetType(widget) == EZ_WIDGET_ITEXT)
    {  
      int fromx, fromy, tox, toy, i, j, itmp, start, end;
      TextProp **prop;
      ITextLine *tline;

      fromx = EZ_ITextXMark(widget);
      fromy = EZ_ITextYMark(widget);
      if(fromy < 0 ) {EZ_RingBell(); return;}

      tox = EZ_ITextXPosition(widget);
      toy = EZ_ITextYPosition(widget);
      if(fromy > toy) 
	{
	  itmp = fromx; fromx = tox; tox = itmp;
	  itmp = fromy; fromy = toy; toy = itmp;
	}
      else if(fromy == toy)
	{
	  if(fromx > tox) {itmp = fromx; fromx = tox; tox = itmp;}
	}
      /*
       * set font for region [from, to].
       */
      for(i = fromy; i <= toy; i++)
	{
	  tline = EZ_ITextData(widget)[i];
	  prop = tline->prop;
	  start = 0; end = tline->length;
	  if(i == fromy) start = fromx;
	  if(i == toy)   end = tox;
	  for(j = start; j < end; j++)  prop[j] = property;
	}
      EZ_DrawWidget(widget);
      EZ_ITextModified(widget) = 1;
    }
}
/**********************************************************/
void EZ_ITextSetRegionFont(widget, font_name)
     EZ_Widget *widget;
     char      *font_name;
{
  if(widget != (EZ_Widget *)NULL && EZ_WidgetType(widget) == EZ_WIDGET_ITEXT 
     && font_name && *font_name)
    {  
      int fromx, fromy, tox, toy, i, j, itmp, start, end;
      int font_id;
      TextProp **prop, *cprop, *tprop;
      ITextLine *tline;

      fromx = EZ_ITextXMark(widget);
      fromy = EZ_ITextYMark(widget);
      if(fromy < 0 ) {EZ_RingBell(); return;}
      font_id = EZ_LoadXFont(font_name);
      tox = EZ_ITextXPosition(widget);
      toy = EZ_ITextYPosition(widget);
      if(fromy > toy) 
	{
	  itmp = fromx; fromx = tox; tox = itmp;
	  itmp = fromy; fromy = toy; toy = itmp;
	}
      else if(fromy == toy)
	{
	  if(fromx > tox) {itmp = fromx; fromx = tox; tox = itmp;}
	}
      /*
       * set font for region [from, to].
       */
      for(i = fromy; i <= toy; i++)
	{
	  tline = EZ_ITextData(widget)[i];
	  prop = tline->prop;
	  start = 0; end = tline->length;
	  if(i == fromy) start = fromx;
	  if(i == toy)   end = tox;
	  for(j = start; j < end; j++)
	    {
	      if( (cprop = prop[j]) != (TextProp *)NULL)
		tprop = EZ_GetTextPropertyFromID(font_id,
						 EZ_TextPropertyFGSet(cprop),
						 EZ_TextPropertyBGSet(cprop),
						 EZ_TextPropertyUnderLine(cprop),
						 EZ_TextPropertyForeground(cprop),
						 EZ_TextPropertyBackground(cprop),
						 EZ_TextPropertyBitmap(cprop));
	      else tprop = EZ_GetTextPropertyFromID(font_id, 0, 0, 0, 0, 0, NULL);
	      prop[j] = tprop;
	    }
	}
      EZ_DrawWidget(widget);
      EZ_ITextModified(widget) = 1;
    }
}
/**********************************************************/
void EZ_ITextSetRegionUnderline(widget, underline)
     EZ_Widget *widget;
     int       underline;
{
  if(widget != (EZ_Widget *)NULL && EZ_WidgetType(widget) == EZ_WIDGET_ITEXT)
    {  
      int fromx, fromy, tox, toy, i, j, itmp, start, end;
      TextProp **prop, *cprop, *tprop;
      ITextLine *tline;

      fromx = EZ_ITextXMark(widget);
      fromy = EZ_ITextYMark(widget);
      if(fromy < 0 ) {EZ_RingBell(); return;}

      tox = EZ_ITextXPosition(widget);
      toy = EZ_ITextYPosition(widget);
      if(fromy > toy) 
	{
	  itmp = fromx; fromx = tox; tox = itmp;
	  itmp = fromy; fromy = toy; toy = itmp;
	}
      else if(fromy == toy)
	{
	  if(fromx > tox) {itmp = fromx; fromx = tox; tox = itmp;}
	}
      /*
       * set font for region [from, to].
       */
      for(i = fromy; i <= toy; i++)
	{
	  tline = EZ_ITextData(widget)[i];
	  prop = tline->prop;
	  start = 0; end = tline->length;
	  if(i == fromy) start = fromx;
	  if(i == toy)   end = tox;
	  for(j = start; j < end; j++)
	    {
	      if( (cprop = prop[j]) != (TextProp *)NULL)
		tprop = EZ_GetTextPropertyFromID(EZ_TextPropertyFont(cprop),
						 EZ_TextPropertyFGSet(cprop),
						 EZ_TextPropertyBGSet(cprop),
						 underline,
						 EZ_TextPropertyForeground(cprop),
						 EZ_TextPropertyBackground(cprop),
						 EZ_TextPropertyBitmap(cprop));
	      else tprop = EZ_GetTextPropertyFromID(-1, 0,0,underline,0,0, NULL);
	      prop[j] = tprop;
	    }
	}
      EZ_DrawWidget(widget);
      EZ_ITextModified(widget) = 1;
    }
}
/**********************************************************/
void EZ_ITextSetRegionForeground(widget, color_name)
     EZ_Widget *widget;
     char      *color_name;
{
  if(widget != (EZ_Widget *)NULL && EZ_WidgetType(widget) == EZ_WIDGET_ITEXT 
     && color_name && *color_name)
    {  
      int fromx, fromy, tox, toy, i, j, itmp, start, end;
      unsigned long pixel_value;
      TextProp **prop, *cprop, *tprop;
      ITextLine *tline;

      fromx = EZ_ITextXMark(widget);
      fromy = EZ_ITextYMark(widget);
      if(fromy < 0 ) {EZ_RingBell(); return;}

      pixel_value = EZ_AllocateColorFromName(color_name);
      tox = EZ_ITextXPosition(widget);
      toy = EZ_ITextYPosition(widget);
      if(fromy > toy) 
	{
	  itmp = fromx; fromx = tox; tox = itmp;
	  itmp = fromy; fromy = toy; toy = itmp;
	}
      else if(fromy == toy)
	{
	  if(fromx > tox) {itmp = fromx; fromx = tox; tox = itmp;}
	}
      /*
       * set font for region [from, to].
       */
      for(i = fromy; i <= toy; i++)
	{
	  tline = EZ_ITextData(widget)[i];
	  prop = tline->prop;
	  start = 0; end = tline->length;
	  if(i == fromy) start = fromx;
	  if(i == toy)   end = tox;
	  for(j = start; j < end; j++)
	    {
	      if( (cprop = prop[j]) != (TextProp *)NULL)
		tprop = EZ_GetTextPropertyFromID(EZ_TextPropertyFont(cprop),
						 1,
						 EZ_TextPropertyBGSet(cprop),
						 EZ_TextPropertyUnderLine(cprop),
						 pixel_value,
						 EZ_TextPropertyBackground(cprop),
						 EZ_TextPropertyBitmap(cprop));
	      else tprop = EZ_GetTextPropertyFromID(-1, 1, 0, 0, pixel_value, 0, NULL);
	      prop[j] = tprop;
	    }
	}
      EZ_DrawWidget(widget);
      EZ_ITextModified(widget) = 1;
    }
}
/**********************************************************/
void EZ_ITextSetRegionBackground(widget, color_name)
     EZ_Widget *widget;
     char      *color_name;
{
  if(widget != (EZ_Widget *)NULL && EZ_WidgetType(widget) == EZ_WIDGET_ITEXT 
     && color_name && *color_name)     
    {  
      int fromx, fromy, tox, toy, i, j, itmp, start, end;
      unsigned long pixel_value;
      TextProp **prop, *cprop, *tprop;
      ITextLine *tline;

      fromx = EZ_ITextXMark(widget);
      fromy = EZ_ITextYMark(widget);
      if(fromy < 0 ) {EZ_RingBell(); return;}

      pixel_value = EZ_AllocateColorFromName(color_name);
      tox = EZ_ITextXPosition(widget);
      toy = EZ_ITextYPosition(widget);
      if(fromy > toy) 
	{
	  itmp = fromx; fromx = tox; tox = itmp;
	  itmp = fromy; fromy = toy; toy = itmp;
	}
      else if(fromy == toy)
	{
	  if(fromx > tox) {itmp = fromx; fromx = tox; tox = itmp;}
	}
      /*
       * set font for region [from, to].
       */
      for(i = fromy; i <= toy; i++)
	{
	  tline = EZ_ITextData(widget)[i];
	  prop = tline->prop;
	  start = 0; end = tline->length;
	  if(i == fromy) start = fromx;
	  if(i == toy)   end = tox;
	  for(j = start; j < end; j++)
	    {
	      if( (cprop = prop[j]) != (TextProp *)NULL)
		tprop = EZ_GetTextPropertyFromID(EZ_TextPropertyFont(cprop),
						 EZ_TextPropertyFGSet(cprop),
						 1,
						 EZ_TextPropertyUnderLine(cprop),
						 EZ_TextPropertyForeground(cprop),
						 pixel_value,
						 EZ_TextPropertyBitmap(cprop));
	      else  tprop = EZ_GetTextPropertyFromID(-1, 0, 1, 0, 0, pixel_value, NULL);
	      prop[j] = tprop;
	    }
	}
      EZ_DrawWidget(widget);
      EZ_ITextModified(widget) = 1;
    }
}
/**********************************************************************
 *
 * Hack. Replace region with a bitmap. Only replace the first line
 *       if the region involves more than one line.
 */
void EZ_ITextReplaceRegionWithBitmap(widget, file_name)
     EZ_Widget *widget;
     char      *file_name;
{
  if(widget != (EZ_Widget *)NULL && EZ_WidgetType(widget) == EZ_WIDGET_ITEXT && file_name)
    {  
      EZ_Bitmap *bitmap = EZ_GetAnyPixmapFromFile(file_name);
      if(bitmap)
	{
	  int fromx, fromy, tox, toy, i, j, itmp, start, end;
	  TextProp **prop, *cprop, *tprop;
	  ITextLine *tline;

	  fromx = EZ_ITextXMark(widget);
	  fromy = EZ_ITextYMark(widget);
	  if(fromy < 0 ) {EZ_RingBell(); return;}

	  tox = EZ_ITextXPosition(widget);
	  toy = EZ_ITextYPosition(widget);
	  if(fromy > toy) 
	    {
	      itmp = fromx; fromx = tox; tox = itmp;
	      itmp = fromy; fromy = toy; toy = itmp;
	    }
	  else if(fromy == toy)
	    {
	      if(fromx > tox) {itmp = fromx; fromx = tox; tox = itmp;}
	    }
	  /*
	   * set textproperty for region [from, to].
	   */
	  for(i = fromy; i <= toy; i++)
	    {
	      tline = EZ_ITextData(widget)[i];
	      prop = tline->prop;
	      start = 0; end = tline->length - 1; /* special property doesn't run over newlines */
	      if(i == fromy) start = fromx;
	      if(i == toy && tox < end) end = tox;

	      for(j = start; j < end; j++)
		{
		  if( (cprop = prop[j]) != (TextProp *)NULL)
		    tprop = EZ_GetTextPropertyFromID(EZ_TextPropertyFont(cprop),
						     EZ_TextPropertyFGSet(cprop),
						     EZ_TextPropertyBGSet(cprop),
						     EZ_TextPropertyUnderLine(cprop),
						     EZ_TextPropertyForeground(cprop),
						     EZ_TextPropertyBackground(cprop),
						     bitmap);
		  else  tprop = EZ_GetTextPropertyFromID(-1, 0, 0, 0, 0, 0, bitmap);
		  prop[j] = tprop;
		}
	    }
	  EZ_DrawWidget(widget);
	  EZ_ITextModified(widget) = 1;
	}
    }
}
/**********************************************************/
int  EZ_ITextSaveText(widget, fname)
     EZ_Widget *widget;
     char      *fname;
{
  int ok = 0;
  if(widget != (EZ_Widget *)NULL && EZ_WidgetType(widget) == EZ_WIDGET_ITEXT  && fname)
    {
      int i, j, nlines = EZ_ITextNLines(widget);
      ITextLine *tline;
      char      *str;
      FILE      *fp, *fopen();
      if(nlines > 0)
	{
	  if( (fp = fopen(fname, "w")))
	    {
	      for(i = 0; i < nlines; i++)
		{
		  tline = EZ_ITextData(widget)[i];
		  str = tline->data;
		  if(i == nlines -1)  /* takes of the newline inserted by EZ at the end */
		    {
		      j = tline->length;
		      if(j > 1)
			{
			  str[j-1] = '\0';
			  (void) fprintf(fp, "%s", str);
			  str[j-1] = '\n';
			}
		    }
		  else (void) fprintf(fp, "%s", str);
		}
	      (void)fclose(fp);
	      ok = 1;
	    }
	  else fprintf(stderr, "Cannot open %s\n", fname);
	}
      EZ_ITextModified(widget) = 0;
    }
  return(ok);
}
/**********************************************************/

static void OutPutTextProperty(tprop, fp)
     TextProp *tprop;
     FILE     *fp;
{
  (void)fprintf(fp,".default\n");
  if(tprop != (TextProp *)NULL)
    {
      int    itmp, rr,gg,bb;
      unsigned long pixel;

      itmp = EZ_TextPropertyFont(tprop);
      if( itmp >= 0) (void)fprintf(fp, ".font \"%s\"\n", EZ_GetFontNameFromId(itmp));
      if(EZ_TextPropertyUnderLine(tprop)) (void)fprintf(fp, ".underline\n");      
      if(EZ_TextPropertyBitmap(tprop)) 
	(void)fprintf(fp,".bitmap \"%s\"\n", EZ_GetBitmapFileName(EZ_TextPropertyBitmap(tprop)));
      
      if(EZ_Depth <= 8)
	{
          if(EZ_TextPropertyFGSet(tprop) || EZ_TextPropertyBGSet(tprop))
            {
              if(EZ_TextPropertyFGSet(tprop))
                {
                  pixel = EZ_TextPropertyForeground(tprop); 
                  EZ_PixelValue2RGB(pixel, &rr,&gg,&bb);
                  (void)fprintf(fp, ".fg \"#%02x%02x%02x\"\n", rr, gg, bb);
                }
              if(EZ_TextPropertyBGSet(tprop))
                {
                  pixel = EZ_TextPropertyBackground(tprop);		
                  EZ_PixelValue2RGB(pixel, &rr,&gg,&bb);
                  (void)fprintf(fp, ".bg \"#%02x%02x%02x\"\n", rr, gg, bb);
                }
            }
	}
      else if(EZ_Depth > 8)
	{
	  if(EZ_TextPropertyFGSet(tprop))
	    {
	      pixel = EZ_TextPropertyForeground(tprop);
	      EZ_PIXEL2RGB(pixel, rr,gg,bb);	      
	      (void)fprintf(fp, ".fg \"#%02x%02x%02x\"\n", rr, gg, bb);
	    }
	  if(EZ_TextPropertyBGSet(tprop))
	    {
	      pixel = EZ_TextPropertyBackground(tprop);
	      EZ_PIXEL2RGB(pixel, rr,gg,bb);	      	      
	      (void)fprintf(fp, ".bg \"#%02x%02x%02x\"\n", rr, gg, bb);
	    }
	}
    }
}

int EZ_ITextSaveAnnotatedText(widget,fname)
     EZ_Widget *widget;
     char      *fname;
{
  int ok = 1;
  if(fname && widget && EZ_WidgetType(widget) == EZ_WIDGET_ITEXT)
    {
      int i, j, length, nlines = EZ_ITextNLines(widget);
      ITextLine *tline;
      TextProp  **prop, *cprop, *tprop;
      char      *str;
      FILE      *fp, *fopen();
      if(nlines > 0)
	{
	  if( (fp = fopen(fname, "w")))
	    {
	      cprop = NULL;
	      for(i = 0; i < nlines; i++)
		{
		  tline = EZ_ITextData(widget)[i];
		  str = tline->data;
		  prop = tline->prop;
		  length = tline->length;
		  if(i == nlines - 1) length -= 1;
		  for(j = 0; j < length; j++)
		    {
		      tprop = prop[j];
		      if(tprop != cprop)
			{
			  putc('\\', fp);   putc('\n',fp);
			  OutPutTextProperty(tprop, fp);
			  cprop = tprop;
			}
		      putc(str[j],fp);
		    }
		}
	      (void)fclose(fp);
	      ok = 1;
	    }
	  else fprintf(stderr, "Cannot open %s\n", fname);
	}
      EZ_ITextModified(widget) = 0;
    }
  return(ok);
}

/*****************************************************************************
 *
 * General selection related functions.
 *
 ****************************************************************************/
/*
 *  This function is used in TextWidget and EntryWidget. Used to paste
 *  selection into the buffer.
 */
void EZ_PasteSelection(widget, window, property, del, insertF)
     EZ_Widget       *widget;
     Window          window;
     Atom            property;
     int             del;
     void            (*insertF) MY_ANSIARGS((EZ_Widget *widget, char *str));
{
  Atom   type;
  int    format;
  int    done = 0;
  unsigned char *data;
  unsigned long nbytes;
  unsigned long bytes_remaining, totalbytes;

  if(property == None) return;

  EZ_FreezeWidget(widget);
  totalbytes = 0;
  while(!done)
    {
      if(XGetWindowProperty(EZ_Display,
			    window,
			    property,
			    totalbytes/4,    /* 32-bit offset */
			    1024,
			    del,
			    AnyPropertyType,
			    &type,
			    &format,
			    &nbytes,
			    &bytes_remaining,
			    (unsigned char **)&data)  != Success)  return;

      if(type != XA_STRING) { XFree(data); return;}
      data[nbytes] = '\0';
      totalbytes += nbytes;
      (insertF)(widget, data);
      XFree(data);
      if(bytes_remaining <= 0) done = 1;
    }
  EZ_UnFreezeWidget(widget);
  EZ_DrawWidget(widget);
}
/****************************************************************************
 *
 *   Claim selection owner. If succesful, copy text into the CUT_BUFFER0
 *  The selected text from an EZ widget is saved in TextWidgetKillBuffer.data,
 *  the size is  TextWidgetKillBuffer.length. 
 */
void EZ_SetSelectionOwner(widget)
     EZ_Widget *widget;
{
  /* 
   * if this client owns the current selection. No SelectionClear event
   * will be sent by the server, so we have to un-highlight the current
   * selection ourself before change the owner window
   */
  Window oldOwner = XGetSelectionOwner(EZ_Display, XA_PRIMARY);
  if(oldOwner != None)
    {
      EZ_Widget *twidget = EZ_LookupWidgetFromMappedHT(oldOwner);
      if(twidget && twidget != widget && EZ_WidgetMapped(twidget))
	{
	  XSelectionClearEvent xevent;
	  xevent.type = SelectionClear;
	  xevent.send_event = True;
	  xevent.display = EZ_Display;
	  xevent.window = oldOwner;
	  xevent.time = CurrentTime;
	  xevent.selection = XA_PRIMARY;
	  XSendEvent(EZ_Display, oldOwner, False, 0L, (XEvent *)&xevent);
	}
    }
  XSetSelectionOwner(EZ_Display,   /* the  display    */
		     XA_PRIMARY,             /* selection atom  */
		     EZ_WidgetWindow(widget),/* owner window    */
		     CurrentTime
		     );
  if(XGetSelectionOwner(EZ_Display, XA_PRIMARY) != EZ_WidgetWindow(widget))
    (void) fprintf(stderr, "Set selection failed\n"); 
  else 
    XChangeProperty(EZ_Display,
		    DefaultRootWindow(EZ_Display),
		    XA_CUT_BUFFER0,
		    XA_STRING,
		    8,
		    PropModeReplace,
		    TextWidgetKillBuffer.data,
		    TextWidgetKillBuffer.length);
}
/*****************************************************************************
 *
 *  The selected text from an EZ widget is saved in TextWidgetKillBuffer.data,
 *  the size is  TextWidgetKillBuffer.length. 
 *  If TextWidgetKillBuffer.width != 0, it means there are internal EZ 
 *  text properties associated with the text. (Selected from a text widget)
 */
void EZ_HandleSelectionRequest(widget,xevent)
     EZ_Widget        *widget;
     XEvent           *xevent;
{
  XSelectionRequestEvent *event = (XSelectionRequestEvent *)xevent;
  XSelectionEvent reply;

  reply.type = SelectionNotify;
  reply.display =  event->display;
  reply.requestor = event->requestor;
  reply.selection = event->selection;
  reply.time = event->time;
  reply.target = event->target;
  reply.property =event->property;
  if(reply.property == None)   reply.property = event->target;
  XChangeProperty(reply.display,reply.requestor,reply.property,XA_STRING,8,
		  PropModeReplace, 
		  (unsigned char *)TextWidgetKillBuffer.data,
		  TextWidgetKillBuffer.length);
  
  XSendEvent(reply.display,reply.requestor,False,0L,(XEvent *)&reply);
  XFlush(reply.display);
}
/*****************************************************************************/  
void EZ_ITextForwardWord(widget)
     EZ_Widget *widget;
{
  if(widget != (EZ_Widget *)NULL && EZ_WidgetType(widget) == EZ_WIDGET_ITEXT)
    {
      char *data;
      int nlines, old_X, old_Y, save_Y;
      int freezed = EZ_GetWidgetFreezedFlag(widget);
      int old_last = EZ_ITextLastLine(widget);
      ITextLine *tline;

      nlines = EZ_ITextNLines(widget) - 1;
      if(freezed == 0) { EZ_SetWidgetFreezedFlag(widget);}
      save_Y = EZ_ITextYPosition(widget);
      /* skip punct chars first */
      while(1)
	{
	  old_X = EZ_ITextXPosition(widget);
	  old_Y = EZ_ITextYPosition(widget);
	  tline = EZ_ITextData(widget)[old_Y];

	  if(old_Y == nlines)
	    {
	      if(old_X >= tline->length-1) { EZ_RingBell(); break;}
	    }
	  data = tline->data;
	  if(isalnum(data[old_X])) break;
	  else  EZ_ITextForwardChar(widget);
	}
      /* go to the next punct char */
      while(1)
	{
	  old_X = EZ_ITextXPosition(widget);
	  old_Y = EZ_ITextYPosition(widget);
	  tline = EZ_ITextData(widget)[old_Y];

	  if(old_Y == nlines)
	    {
	      if(old_X == tline->length-1) break;
	    }
	  data = tline->data;
	  if( !(isalnum(data[old_X]))) break;
	  else EZ_ITextForwardChar(widget);
	}
      
      if(freezed == 0)
	{
	  EZ_ClearWidgetFreezedFlag(widget);
	  if(old_Y > old_last)  EZ_DrawWidget(widget);
	  else
	    {
	      if(save_Y != old_Y) EZ_ITextDisplayOneLine(widget, save_Y);
	      EZ_ITextDisplayOneLine(widget, old_Y);
	    }
	}
    }
}
void EZ_ITextBackwardWord(widget)
     EZ_Widget *widget;
{
  if(widget != (EZ_Widget *)NULL && EZ_WidgetType(widget) == EZ_WIDGET_ITEXT)
    {
      char *data;
      int old_X,old_Y, save_Y,freezed = EZ_GetWidgetFreezedFlag(widget);
      int old_first =  EZ_ITextFirstLine(widget);
      ITextLine *tline;

      if(freezed == 0) { EZ_SetWidgetFreezedFlag(widget);}
      save_Y = EZ_ITextYPosition(widget);
      EZ_ITextBackwardChar(widget);

      /* skip punct chars */
      while(1)
	{
	  old_X = EZ_ITextXPosition(widget);
	  old_Y = EZ_ITextYPosition(widget);

	  if(old_Y == 0)
	    {
	      if(old_X == 0) break;
	    }
	  tline = EZ_ITextData(widget)[old_Y];
	  data = tline->data;
	  if(isalnum(data[old_X])) break;
	  else EZ_ITextBackwardChar(widget);
	} 
      /* goto the previous punct chars */
      while(1)  
	{
	  old_X = EZ_ITextXPosition(widget);
	  old_Y = EZ_ITextYPosition(widget);

	  if(old_Y == 0)
	    {
	      if(old_X == 0) break;
	    }
	  tline = EZ_ITextData(widget)[old_Y];
	  data = tline->data;
	  if(!(isalnum(data[old_X])))
	    {
	      EZ_ITextForwardChar(widget);
	      break;
	    } 
	  else EZ_ITextBackwardChar(widget);
	} 
      if(freezed == 0)
	{
	  EZ_ClearWidgetFreezedFlag(widget);
	  if(old_Y < old_first) EZ_DrawWidget(widget);
	  else
	    {
	      if(save_Y != old_Y) EZ_ITextDisplayOneLine(widget, save_Y);
	      EZ_ITextDisplayOneLine(widget, old_Y);
	    }
	}
    }
}
/*******************************************************************/

#undef _EZ_WIDGET_ITEXT_C_
