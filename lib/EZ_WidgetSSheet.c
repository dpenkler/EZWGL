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
 ***              Widget Spread Sheet                          ***
 ***                                                           ***
 *****************************************************************/
#define _EZ_WIDGET_SSHEET_C_

#include "EZ_Widget.h"

/*********************************************************************
 * 
 *  Functions implemented in this file:
 */
EZ_Widget *EZ_CreateSpreadSheet MY_ANSIARGS((EZ_Widget *widget));
void EZ_SSheetGetSizes MY_ANSIARGS((EZ_Widget *widget,int *nrow, int *ncol));
void EZ_SSheetSetFirstCell MY_ANSIARGS((EZ_Widget *widget,int row, int col));
void EZ_SSheetSetTextProperty MY_ANSIARGS((EZ_Widget *widget, TextProp *prop));
void EZ_SSheetSetHScrollbar  MY_ANSIARGS((EZ_Widget *widget, EZ_Widget *hscroll));
void EZ_SSheetSetVScrollbar  MY_ANSIARGS((EZ_Widget *widget, EZ_Widget *vscroll));

void EZ_SSheetHideColumns   MY_ANSIARGS((EZ_Widget *widget, int r1, int r2));
void EZ_SSheetUnhideColumns   MY_ANSIARGS((EZ_Widget *widget, int r1, int r2));
void EZ_SSheetHideRows   MY_ANSIARGS((EZ_Widget *widget, int r1, int r2));
void EZ_SSheetUnhideRows   MY_ANSIARGS((EZ_Widget *widget, int r1, int r2));

void EZ_SSheetSwapRow     MY_ANSIARGS((EZ_Widget *widget, int r1, int r2));
void EZ_SSheetSwapColumn  MY_ANSIARGS((EZ_Widget *widget, int c1, int c2));
void EZ_SSheetSwapCell    MY_ANSIARGS((EZ_Widget *widget, int r1, int c1, int r2, int c2));
void EZ_SSheetClearRegion MY_ANSIARGS((EZ_Widget *wt, int r1, int c1, int r2, int c2)); 
void  EZ_SSheetCopyRegion MY_ANSIARGS((EZ_Widget *wt, int r1, int c1, int r2, int c2,
                                       int r0, int c0, int del));
void EZ_SSheetSortRegion MY_ANSIARGS((EZ_Widget *widget, int r1, int c1, int r2, int c2, int cc,
                                      int (*cmp)MY_ANSIARGS((CONST void *, CONST void *)) ));
void EZ_SSheetSortRegionR MY_ANSIARGS((EZ_Widget *widget, int r1, int c1, int r2, int c2, int rr,
                                      int (*cmp)MY_ANSIARGS((CONST void *, CONST void *)) ));
void EZ_SSheetSortColumns MY_ANSIARGS((EZ_Widget *widget, int r1, int c1, int r2, int c2, 
                                      int (*cmp)MY_ANSIARGS((CONST void *, CONST void *)) ));
void EZ_SSheetSortRows MY_ANSIARGS((EZ_Widget *widget, int r1, int c1, int r2, int c2, 
                                      int (*cmp)MY_ANSIARGS((CONST void *, CONST void *)) ));

int  EZ_SSheetFindCell MY_ANSIARGS((EZ_Widget *widget, char *str, int *r, int *c));
void  EZ_SSheetSetTabChar MY_ANSIARGS((EZ_Widget *widget, char tab, char newline));
char  *EZ_SSheetGetRegionString MY_ANSIARGS((EZ_Widget *wt, int r1, int c1, int r2, int c2));
char  *EZ_SSheetGetRegionStringWithProperty  MY_ANSIARGS((EZ_Widget *wt, int r1, int c1, int r2, int c2,
                                                          TextProp ***prop));
void  EZ_SSheetInsertRegionString MY_ANSIARGS((EZ_Widget *wt, int r1, int c1, char *str));
void  EZ_SSheetInsertRegionStringWithProperty MY_ANSIARGS((EZ_Widget *wt, int r1, int c1, char *str,
							   TextProp **tp));
char  **EZ_SSheetGetRegionItems  MY_ANSIARGS((EZ_Widget *wt, int r1, int c1, int r2, int c2));
char  **EZ_SSheetGetRegionItemsWithProperty  MY_ANSIARGS((EZ_Widget *wt, int r1, int c1, int r2, int c2,
							  TextProp ****tprop));

int   EZ_SSheetFindCursorCell MY_ANSIARGS((EZ_Widget *widget, int x, int y, int *cy, int *cx));
void  EZ_SSheetHighlightRegion MY_ANSIARGS((EZ_Widget *widget, int x1, int y1, int x2, int y2));
int   EZ_SSheetGetHighlightedRegion  MY_ANSIARGS((EZ_Widget *wt, int *r1, int *c1, int *r2, int *c2));

void  EZ_SSheetGetActiveCell MY_ANSIARGS((EZ_Widget *wt, int *row, int *col, char **str));
void  EZ_SSheetSetActiveCell MY_ANSIARGS((EZ_Widget *widget, int row, int col));
char  *EZ_SSheetGetCell MY_ANSIARGS((EZ_Widget *wt, int row, int col));
char  *EZ_SSheetGetCellWithProperty MY_ANSIARGS((EZ_Widget *wt, int row, int col, TextProp ***prop));
void  EZ_SSheetGetActiveCellWithProperty MY_ANSIARGS((EZ_Widget *wt, int *row, int *col, char **str,
                                                      TextProp ***prop));
void  EZ_SSheetDrawCell MY_ANSIARGS((EZ_Widget *wt, int row, int col,  Drawable d, 
                                     int x, int y, int w, int h));

void  EZ_SSheetInsertRegionItems  MY_ANSIARGS((EZ_Widget *wt, int r1, int c1, int r2, int c2, char **items));
void  EZ_SSheetInsertRegionItemsWithProperty  MY_ANSIARGS((EZ_Widget *wt, int r1, int c1, int r2, int c2, 
							   char **items, TextProp ***tprop));


void  EZ_SSheetInsertItem MY_ANSIARGS((EZ_Widget *widget, int row, int col, char *str));
void  EZ_SSheetInsertItemWithProperty MY_ANSIARGS((EZ_Widget *widget, int row, int col, char *str,
						   TextProp **p));
void  EZ_SSheetSetRegionJustification MY_ANSIARGS((EZ_Widget *widget, int r1, int c1, int r2, int c2, int just));
void  EZ_SSheetSetRegionProperty MY_ANSIARGS((EZ_Widget *widget, int r1, int c1, int r2, int c2, TextProp *p));

char  *EZ_SSheetIndexToString MY_ANSIARGS((int col, int row, char *str));
void  EZ_SSheetStringToIndex MY_ANSIARGS(( char *str, int *col, int *row));

int  EZ_SSheetSelectCellWork  MY_ANSIARGS((EZ_Widget *widget, Drawable d, int x, int y, int flag));
void  EZ_SSheetRedrawRegion MY_ANSIARGS((EZ_Widget *widget, Drawable d, int x, int y, 
					 int x1, int y1, int flag));
void  EZ_MarkSSheetUsedTextProperties MY_ANSIARGS((EZ_Widget *widget));
void  EZ_SSheetPasteSelection MY_ANSIARGS((EZ_Widget *widget));
void  EZ_DoScaleItem MY_ANSIARGS((EZ_Item *item, int x, int y,
                                     int cr, int x0, int y0, int w, int h));
int  EZ_MatchWord MY_ANSIARGS((char *buf, char *word, int blen, int wlen, int *pos_ret));

EZ_Item *EZ_SSheetMakeChart MY_ANSIARGS((EZ_Widget *widget, int r1, int c1, int r2, int c2,
                                         EZ_SChartHandle handle, int kind, void *data, int cmode));
void  EZ_SSheetDeleteChart MY_ANSIARGS((EZ_Widget *widget, EZ_Item *chart));

int   EZ_SSheetRedrawCharts MY_ANSIARGS((EZ_Widget *wptr, Drawable drawable));

void EZ_SSheetValidateCharts MY_ANSIARGS((EZ_Widget *widget, int r1, int c1, int r2, int c2));
EZ_Item * EZ_SSheetGetActiveChart MY_ANSIARGS((EZ_Widget *widget));
EZ_Item **EZ_SSheetGetAllCharts MY_ANSIARGS((EZ_Widget *widget, int *n ));

/*********************************************************************/
void    EZ_DrawWidgetSSheet MY_ANSIARGS((EZ_Widget *widget));
void    EZ_ComputeWidgetSSheetSize MY_ANSIARGS((EZ_Widget *widget, int *w, int *h));
void    EZ_FreeWidgetSSheetData MY_ANSIARGS((EZ_Widget *widget));
void    EZ_SSheetEventHandle MY_ANSIARGS((EZ_Widget *widget, XEvent *event));

void    EZ_FixSSheetHead MY_ANSIARGS((EZ_Widget *widget, int nrows, int ncolumns));

static EZ_SSItem *allocateNewSSItem MY_ANSIARGS((void));
static EZ_SSItem *locateCreateSSItem MY_ANSIARGS((EZ_Widget *w, int i, int j, int c, EZ_SSItem **prev));
static void  unmarkActiveItem MY_ANSIARGS((EZ_Widget *));
static void  redrawCell MY_ANSIARGS((EZ_Widget *widget, Drawable d, EZ_SSItem *item, 
				     int row, int col, int flag));
static void drawHead MY_ANSIARGS((EZ_Widget *w, Drawable d));
static void highlightHead MY_ANSIARGS((EZ_Widget *wptr, Drawable d, int ox, int oy, int flag));
static void EZ_SSheetInsertString MY_ANSIARGS(( EZ_Widget *widget, char *str));
static void copyToKillBuffer MY_ANSIARGS((EZ_Widget *widget));
static int moveBoundry MY_ANSIARGS((EZ_Widget *widget, int x, int y, int flag));
static int updateCharts MY_ANSIARGS((EZ_Widget *widget, int x1, int y1, int x2, int y2, int flag));
/*********************************************************************
 * 
 */
static EZ_WidgetHandle EZ_SSheetHandle =
{
  EZ_ComputeWidgetSSheetSize,
  EZ_DrawWidgetSSheet,
  EZ_FreeWidgetSSheetData,
  EZ_SSheetEventHandle,
};
/*********************************************************************/
#define CLEAR_REGION   0x1
#define START_REGION   0x2
#define UPDATE_HEAD    0x4
#define MARK_TAB       0x8
#define RED_BOX        0x10
#define HIGHLIGHT_R    0x20
#define UPDATE_CHART   0x40

#define DO_SCROLL      0x1
#define DO_EXTEND      0x2
/*********************************************************************/


EZ_Widget  *EZ_CreateSpreadSheet(parent)
     EZ_Widget  *parent;     /* parent widget    */
{
  EZ_Widget  *wptr;

  wptr = EZ_CreateNewWidget(parent);
  /*--------------------------------------------------
   * Register the handling functions for ...
   *  has to be done after  EZ_CreateNewWiget.
   *-------------------------------------------------*/
  EZ_WidgetHandlingFunctions[EZ_WIDGET_SPREAD_SHEET] = &EZ_SSheetHandle;

  EZ_SetWidgetTypeAndNames(wptr,EZ_WIDGET_SPREAD_SHEET);
  EZ_WidgetBorderStyle(wptr)  = EZ_BORDER_DOWN;
  EZ_WidgetBorderWidth(wptr)  = 2;
  EZ_WidgetCursor(wptr) = EZ_GetCursor(EZ_C_PLUS);
  EZ_WidgetPadB(wptr) = 2;
  EZ_WidgetSepX(wptr) = 4;
  EZ_WidgetSepY(wptr) = 3;
  EZ_SSheetFont(wptr) = EZ_GetFontFromId(EZ_LABEL_FONT);
  EZ_SSheetCellHeight(wptr)   = 24;
  EZ_SSheetCellWidth(wptr)    = 96;
  EZ_SSheetTWidth(wptr)       = 32;
  EZ_SSheetTHeight(wptr)      = 16;
  EZ_SSheetTextBG(wptr)     = ~0;
  EZ_SSheetSelectionBG(wptr)= EZ_ColorArray[YELLOW];
  EZ_SSheetSelectionFG(wptr)= EZ_ColorArray[RED];
  EZ_SetWidgetFocusableFlag(wptr);  
  EZ_SSheetNRows(wptr) = 0;
  EZ_SSheetNColumns(wptr) = 0;
  EZ_SSheetDirty(wptr) = 1;
  EZ_SSheetHMode(wptr) = 2;
  EZ_SSheetShowHead(wptr) = 1;
  EZ_SSheetNRows(wptr) = 0;
  EZ_SSheetNColumns(wptr) = 0;
  EZ_SSheetTabChar(wptr) = '\t';  
  EZ_SSheetNewlineChar(wptr) = '\n';  
  EZ_SSheetTextProperty(wptr)= EZ_GetTextProperty(EZ_FONT_ID, EZ_TEXT_FONT, 0);

  EZ_FixSSheetHead(wptr, 128, 52);

  return(wptr);
}

/******************************************************************************/
    
void  EZ_FreeWidgetSSheetData(widget)
     EZ_Widget *widget;
{
  int i, n;
  EZ_SSItem  *tmp, *next, *headrow, *data = EZ_SSheetData(widget);
  ITextLine  *tline;
  EZ_Item    **charts;

  headrow = EZ_SSheetHeadRow(widget);
  n = EZ_SSheetNColumns(widget);
  for(i = 0; i < n; i++)
    {
      tline = headrow[i].text;
      EZ_FreeTextLine(tline);
    }
  (void) my_free(headrow);

  n = EZ_SSheetNRows(widget);
  for(i = 0; i < n; i++)
    {
      tmp = data[i].next;
      while(tmp)
        {
          next = tmp->next;
          tline = tmp->text;
          EZ_FreeTextLine(tline);
          my_free(tmp);
          tmp = next;
        }
    }
  (void)my_free( (char *) data);
  charts = EZ_SSheetCharts(widget);
  if(charts)
    {
      n = EZ_SSheetNCharts(widget);
      for(i = 0; i < n; i++) EZ_DestroyItem(charts[i]);
      (void)my_free( (char *)charts);
    }
}
/********************************************************************
 *
 *  Figure out the dimension of an internal list box.
 */
void EZ_ComputeWidgetSSheetSize(widget, w, h)
     EZ_Widget *widget;
     int       *w, *h;
{
  int   width, height, cw, ch, padb, sepx, sepy, tmp, bw;
  unsigned long dfg, dbg;

  /* remember the default bg and fg for items */
  if(EZ_WidgetBackground(widget))  dbg = EZ_WidgetBackground(widget);
  else if(EZ_WidgetParentBG(widget) && *(EZ_WidgetParentBG(widget)))
    dbg = *(EZ_WidgetParentBG(widget));
  else dbg = EZ_ColorArray[EZ_NORMAL_GRAY1];
  dfg = EZ_WidgetForeground(widget);  
  EZ_SSheetDFG(widget) = dfg;
  EZ_SSheetDBG(widget) = dbg;

  sepx = EZ_WidgetSepX(widget);
  bw = EZ_WidgetBorderWidth(widget);
  padb = EZ_WidgetPadB(widget);
  EZ_SSheetBwPad(widget) = bw + padb;
  if(!(EZ_GetWidgetSizeComputedFlag(widget)))
    { 
      int i, nn;
      EZ_SSItem *head;
      EZ_SetWidgetSizeComputedFlag(widget);
      sepy = EZ_WidgetSepY(widget);  
      height = EZ_SSheetFont(widget)->max_bounds.ascent + 
	EZ_SSheetFont(widget)->max_bounds.descent;
      height = sepy + height + 2;
      if(EZ_SSheetCellHeight(widget) < height) 	EZ_SSheetCellHeight(widget) = height;
      if(EZ_SSheetTHeight(widget) > 0)
	{
	  if(EZ_SSheetTHeight(widget) < height)
	    EZ_SSheetTHeight(widget) = height;
	}
      if(EZ_SSheetTWidth(widget) > 0)
	{
	  tmp = sepx + 1;
	  tmp = tmp + tmp + XTextWidth(EZ_SSheetFont(widget),"XXXX", 4);
	  if(EZ_SSheetTWidth(widget) < tmp) EZ_SSheetTWidth(widget)= tmp;
	}
      if(EZ_SSheetTWidth(widget)> 0 || EZ_SSheetTHeight(widget)> 0)
	{ EZ_SSheetShowHead(widget) = 1;}
      else { EZ_SSheetShowHead(widget) = 0;}
      nn = EZ_SSheetNColumns(widget);
      head = EZ_SSheetHeadRow(widget);
      tmp = EZ_SSheetCellWidth(widget);
      for(i = 0; i < nn; i++) 
        { if(head[i].rowOrWidth >1 && head[i].rowOrWidth <tmp) head[i].rowOrWidth = tmp;}
      nn = EZ_SSheetNRows(widget);
      head = EZ_SSheetData(widget);
      tmp = EZ_SSheetCellHeight(widget);
      for(i = 0; i < nn; i++)
        { if( head[i].colOrHeight>1 && head[i].colOrHeight < tmp) head[i].colOrHeight = tmp;}
    }
  height = EZ_SSheetCellHeight(widget);
  width =  EZ_SSheetCellWidth(widget) + sepx;
  EZ_WidgetPadX(widget) = 0;
  EZ_WidgetPadY(widget) = 0;
 
  cw = width +padb + bw;
  ch = height + padb + bw;

  /* large enough to fit a 4x4 table */
  cw += cw; ch += ch;
  *w = cw + cw + padb;
  *h = ch + ch + padb;
}

/****************************************************************************
 * 
 */
static char *EZ_NumberToSSheetIndex(idx, alpha, str) int idx, alpha; char *str;
{
  if(alpha)
    {
      int tmp[10], i, cnt = 0;
      do { tmp[cnt++] = idx % 26; idx /= 26; } while(idx > 0);
      str[cnt--] = 0; if(cnt) tmp[cnt] -= 1;
      i = 0;
      while(cnt >= 0) { str[i] = (char) (tmp[cnt] + 'A'); i++; cnt--;}
    }
  else sprintf(str, "%d", idx);
  return(str);
}

char *EZ_SSheetIndexToString(col, row, str) int col, row; char *str;
{
  char tmp[10];
  EZ_NumberToSSheetIndex(col, 1, str);
  EZ_NumberToSSheetIndex(row, 0, tmp);
  strcat(str, tmp);
  return(str);
}
void EZ_SSheetStringToIndex(str, col, row) int *col, *row; char *str;
{
  int ii = 0;
  char *p, tmp[32];
  strcpy(tmp, str);
  p = tmp;
  while(isalpha(*p)) p++;
  *row = atoi(p);
  *p-- = '\0';  if(p != tmp) tmp[0] += 1;
  p = tmp;
  while(*p) { ii = ii * 26 + (int)(*p -'A'); p++;}
  *col = ii;
}
/***********************************************************************/
void makeSSHeadItem(wptr,ssitem, idx, rowflag) 
     EZ_Widget *wptr; EZ_SSItem *ssitem; int idx, rowflag;
{
  EZ_SSItem *ret = ssitem;
  char str[20];
  ITextLine *tline = NULL;
  int len;

  EZ_NumberToSSheetIndex(idx, rowflag, str);
  memset(ret, 0, sizeof(EZ_SSItem));
  ret->dirty = 1;

  if(rowflag) 
    {
      ret->rowOrWidth  = EZ_SSheetCellWidth(wptr);      
      ret->colOrHeight = EZ_SSheetTHeight(wptr);
    }
  else
    {
      ret->rowOrWidth  = EZ_SSheetTWidth(wptr);      
      ret->colOrHeight = EZ_SSheetCellHeight(wptr);
    }
  len = strlen(str);
  EZ_GrowITextLine(&tline, len);
  strcpy(tline->data, str);
  tline->length = len;
  memset(tline->prop, 0, tline->space * sizeof(TextProp *));
  ret->just = EZ_LEFT;
  ret->text = tline;
}

void EZ_FixSSheetHead(wptr, nrow, ncol)
     EZ_Widget *wptr; int nrow, ncol;
{
  int        i, rr, cc;
  EZ_SSItem  *data, *old, *row1, *trow;
  
  rr = EZ_SSheetNRows(wptr);
  cc = EZ_SSheetNColumns(wptr);

  if(rr < nrow) /* grow */
    {
      old = EZ_SSheetData(wptr);
      if(old) data = (EZ_SSItem *)my_realloc(old, (nrow+1) * sizeof(EZ_SSItem), _SSHEET_DATA_);
      else data = (EZ_SSItem *)my_malloc((nrow+1) * sizeof(EZ_SSItem), _SSHEET_DATA_);
      EZ_SSheetData(wptr) = data;
      EZ_SSheetNRows(wptr) = nrow;      
      for(i = rr; i <= nrow; i++) makeSSHeadItem(wptr, &(data[i]), i,0);

    }
  if(cc < ncol)
    {
      row1 = EZ_SSheetHeadRow(wptr);
      if(row1) trow = (EZ_SSItem *)my_realloc(row1, (ncol+1) * sizeof(EZ_SSItem),_SSHEET_DATA_);
      else trow = (EZ_SSItem *)my_malloc((ncol+1) * sizeof(EZ_SSItem),_SSHEET_DATA_);
      EZ_SSheetHeadRow(wptr) = trow;
      EZ_SSheetNColumns(wptr) = ncol;      
      for(i = cc; i <= ncol; i++) (void) makeSSHeadItem(wptr, &(trow[i]), i, 1);
    }
}
/****************************************************************************
 *
 */
int EZ_ComputeSSheetWindow(wptr, flag)
     EZ_Widget *wptr; int flag;
{
  int bwpad2 = (EZ_SSheetBwPad(wptr)) << 1;
  int w = EZ_WidgetWidth(wptr) - bwpad2;
  int h = EZ_WidgetHeight(wptr)- bwpad2;
  int cx = EZ_SSheetCx(wptr);
  int cy = EZ_SSheetCy(wptr);
  int x0 = EZ_SSheetX0(wptr);
  int y0 = EZ_SSheetY0(wptr);
  int cw, ch, aw, ah, i, ss;
  int nrows, ncolns, ok, ret = 0, fix = 0;
  int numRows=0, numCols=0;
  int scroll = flag & DO_SCROLL;
  EZ_SSItem *data, *row;
  ncolns = EZ_SSheetNColumns(wptr);
  nrows = EZ_SSheetNRows(wptr);  
  if(x0 >= ncolns || y0 >= nrows) 
    EZ_FixSSheetHead(wptr, x0+120, ncolns+60);
  
  row = EZ_SSheetHeadRow(wptr);
  data = EZ_SSheetData(wptr);

  ncolns = ss = EZ_SSheetNColumns(wptr);
  if(scroll)
    {
      if(x0 > cx) {x0 = EZ_SSheetX0(wptr) = cx; ret++;}
      if(y0 > cy) {y0 = EZ_SSheetY0(wptr) = cy; ret++;}
    }

  ok = scroll;
  do
    {
      aw = EZ_SSheetTWidth(wptr);

      for(i = x0;  i < ss;  i++)
        {
          aw += row[i].rowOrWidth;
          if(aw >= w)  break;
        }
      if(i == ss) { numCols = ncolns + 52; fix = 1; ok = 0;}
      else if(cx < i) ok = 0;
      else if(scroll) { x0 = EZ_SSheetX0(wptr) = x0+1; ret++;}
    } while(ok);

  EZ_SSheetX1(wptr) = i;
  for(cw=0, i = 0;  i < x0;  i++) cw += row[i].rowOrWidth;
  EZ_SSheetTx(wptr) = -cw;
  
  nrows = ss = EZ_SSheetNRows(wptr);  
  ok = scroll;
  do
    {
      ah = EZ_SSheetTHeight(wptr);
      for(i = y0; i < ss; i++)
        {
          ah += data[i].colOrHeight;
          if(ah >= h) break;
        }
      if(i == ss) { numRows = nrows + 128; fix = 1;ok=0;}
      else if(cy < i) ok = 0;
      else if(scroll) { y0 = EZ_SSheetY0(wptr) = y0+1; ret++;}
    } while(ok);
  EZ_SSheetY1(wptr) = i;  
  for(ch=0, i = 0;  i < y0;  i++) ch += data[i].colOrHeight;
  EZ_SSheetTy(wptr) = -ch;  
  if(fix && (flag & DO_EXTEND))
    {
      if(numRows > 32766) numRows = 32766;
      if(numCols > 32766) numCols = 32766;
      EZ_FixSSheetHead(wptr, numRows, numCols);
    }
  return(ret);
}
/****************************************************************************/
static void  update_ssheet_scrollbars(wptr)
     EZ_Widget *wptr;
{
  EZ_Widget *hscroll = EZ_SSheetHScroll(wptr);
  EZ_Widget *vscroll = EZ_SSheetVScroll(wptr);

  if(EZ_LookupWidgetFromAllHT(hscroll) != (EZ_Widget *)NULL)
    {
      int total = EZ_SSheetNColumns(wptr)-1;
      int start = -EZ_SSheetX0(wptr);
      int page = EZ_SSheetX1(wptr) + start+1;

      EZ_UpdateScrollbarI(hscroll, total, page,  start);
    }
  if(EZ_LookupWidgetFromAllHT(vscroll) != (EZ_Widget *)NULL)
    {
      int total = EZ_SSheetNRows(wptr)-1;
      int start = EZ_SSheetY0(wptr);
      int page = EZ_SSheetY1(wptr) - start+1;

      EZ_UpdateScrollbarI(vscroll, total, page, start);
    }
}

/****************************************************************************
 *
 *  Draw a SSheet widget
 */
void  EZ_DrawWidgetSSheet(wptr)
     EZ_Widget *wptr;
{
  int             w, h,x,y, i, padb, padb2, bw, bw2, tmp;
  int             xoff, yoff, x0, y0, x1, y1, lwidth, lheight, sepx;
  int             tw, th, cellW, cellH;
  Pixmap          pixmap;
  Window          win;
  GC              gc;
  unsigned long   bgpv, dfg, dbg;
  EZ_SSItem       *data, *row, *row0;
  XFontStruct     *defaultfont = EZ_WidgetFont(wptr);
  
  if(EZ_GetWidgetFreezedFlag(wptr)) return;
  
  win = EZ_WidgetWindow(wptr);
  w   = EZ_WidgetWidth(wptr);
  h   = EZ_WidgetHeight(wptr);
  bw = EZ_WidgetBorderWidth(wptr);
  sepx = EZ_WidgetSepX(wptr);
  bw2 = bw+bw;
  padb = EZ_WidgetPadB(wptr);
  padb2 = padb + padb;
  tmp = bw2 + padb2;
  pixmap = XCreatePixmap(EZ_Display, win, w, h, EZ_Depth);

  if(EZ_SSheetDirty(wptr))
    {
      EZ_ComputeSSheetWindow(wptr, DO_EXTEND);  
      EZ_SSheetDirty(wptr) = 0;
    }
  
  /* fill in the background */
  if(EZ_SSheetTextBG(wptr) != ~0)
    {
      gc = EZ_WRITABLEGC;
      XSetForeground(EZ_Display, gc, EZ_SSheetTextBG(wptr));	
    }
  else
    {
      if(EZ_GetBackgroundGC(wptr, &gc, &bgpv, 0, 0))
	{
	  int ox, oy;
	  EZ_GetWidgetTileOrigin(wptr, &ox, &oy);      
	  XSetTSOrigin(EZ_Display, gc, ox, oy);
	}
    }
  XFillRectangle(EZ_Display, pixmap, gc, padb,padb, w -padb2, h-padb2); 

  /* render the contents */
  gc = EZ_DARKGC2;
  x0 = EZ_SSheetX0(wptr);
  x1 = EZ_SSheetX1(wptr);
  y0 = EZ_SSheetY0(wptr);
  y1 = EZ_SSheetY1(wptr);

  cellW = EZ_SSheetCellWidth(wptr);
  cellH = EZ_SSheetCellHeight(wptr);
  tw = EZ_SSheetTWidth(wptr);
  th = EZ_SSheetTHeight(wptr);
  xoff = yoff= bw+padb;
  lwidth  = w - xoff;
  lheight = h - yoff;
  data = EZ_SSheetData(wptr);


  /* display the grid */
  row = EZ_SSheetHeadRow(wptr);
  x = xoff+tw;  y = yoff;
  for(i = x0; i<= x1; i++)
    {
      int tt = row[i].rowOrWidth; 
      XDrawLine(EZ_Display, pixmap, gc, x, y, x, lheight);
      row[i].colOrHeight = x;  /* remember starting X coor */
      x += tt; 
    }
  x = xoff; y=yoff + th;
  for(i = y0; i<= y1; i++)
    {
      XDrawLine(EZ_Display, pixmap, gc, x, y, lwidth, y);
      data[i].rowOrWidth = y;  /* remember starting Y coor */
      y += data[i].colOrHeight;      
    }

  dbg = EZ_SSheetDBG(wptr);
  dfg = EZ_SSheetDFG(wptr);


  /* render the cells */
  row0 = EZ_SSheetHeadRow(wptr);
  x = xoff; y = yoff + th;
  for(i = y0; i<= y1; i++)  
    {
      EZ_SSItem *ptr = data[i].next;
      if(data[i].colOrHeight <= 1) continue;
      while(ptr)
	{
	  int cidx = ptr->colOrHeight;
	  if(cidx > x1) break;
	  if(cidx >= x0 )
	    {
	      int cellWidth = row0[cidx].rowOrWidth;
              if(cellWidth > 1)
                {
                  int ascent, xx, yy;
                  int ridx = ptr->rowOrWidth;
                  int xstart = row0[cidx].colOrHeight;
                  int ystart = data[ridx].rowOrWidth;
                  int cellHeight =  data[ridx].colOrHeight;
                  ITextLine *tline = ptr->text;	      
                  int just = ptr->just;
                  if(ptr->dirty)
                    {
                      int ttw, tth, tta;
                      EZ_ITextLineDimensionWork(defaultfont, 0, tline, 0, tline->length, &ttw, &tth, &tta);
                      tline->width = ttw;
                      tline->height= tth;
                      tline->y = tta;
                      ptr->dirty = 0;
                    }
                  ascent = tline->y;
                  yy = (cellHeight - tline->height)/2;
                  if(just == EZ_RIGHT) { xx = cellWidth - tline->width - sepx;}
                  else if(just == EZ_CENTER) { xx = (cellWidth - tline->width)/2;}
                  else xx = sepx;
		  
                  EZ_ITextRenderTextWork(defaultfont, dfg, dbg, 0, 0, 0, tline,
                                         xstart +xx, ystart + yy, tline->height, ascent, pixmap, 0, 0, 0); 
                }
            }
	  ptr = ptr->next;
	}
    }
  /* highlight selection */
  if(EZ_SSheetSelectionMode(wptr))
    { 
      int sx0, sx1, sy0, sy1;
      sx0 = EZ_SSheetSx0(wptr);  sx1 = EZ_SSheetSx1(wptr);
      sy0 = EZ_SSheetSy0(wptr);  sy1 = EZ_SSheetSy1(wptr);
      if(sx1 >= 0) EZ_SSheetRedrawRegion(wptr, pixmap, sx0, sy0, sx1, sy1, HIGHLIGHT_R);
    }
  /* draw charts */
  EZ_SSheetRedrawCharts(wptr, pixmap);
  {
    int cx0=EZ_SSheetCx(wptr),  cy0=EZ_SSheetCy(wptr);
    EZ_SSheetSelectCellWork(wptr, pixmap, cx0, cy0, 0);
  }
  drawHead(wptr, pixmap);

  EZ_FixFocusHighlightBorder(wptr, pixmap, w, h, padb);
  EZ_DrawRectBorder(wptr, pixmap);
  XCopyArea(EZ_Display,pixmap,win, EZ_WRITABLEGC,0,0,w,h,0,0); 
  XFreePixmap(EZ_Display, pixmap); 
  update_ssheet_scrollbars(wptr);
}

/*************************************************************************/
static int doScroll(widget, x, y) EZ_Widget *widget; int *x, *y;
{
  int bwpad = EZ_SSheetBwPad(widget);
  int ww = EZ_WidgetWidth(widget) - bwpad;
  int hh = EZ_WidgetHeight(widget) - bwpad;
  int tw = EZ_SSheetTWidth(widget) + bwpad;
  int th = EZ_SSheetTHeight(widget) + bwpad;
  int redraw = 0;
  if(*x < tw && EZ_SSheetX0(widget) > 0 )
    {
      EZ_SSheetX0(widget) -= 1;
      *x =  tw+1;
      redraw = 1;
    }
  else if(*x > ww && EZ_SSheetX1(widget) < EZ_SSheetNColumns(widget)-1)
    {
      EZ_SSheetX0(widget) += 1;
      *x= ww -1;
      redraw = 1;
    }
  if(*y < th && EZ_SSheetY0(widget) > 0 )
    {
      EZ_SSheetY0(widget) -= 1;
      *y=th+1;
      redraw = 1;
    }
  else if(*y > hh && EZ_SSheetY1(widget) < EZ_SSheetNRows(widget)-1)
    {
      EZ_SSheetY0(widget) += 1;
      *y = hh -1;
      redraw = 1;
    }
  return(redraw);
}

static void updateHighlightedRegion(widget, drawable, xx0, yy0, xx1, yy1, ccx, ccy)
     EZ_Widget *widget; Drawable drawable;  int  ccx, ccy; int xx0, xx1, yy0, yy1; /* old region */
{
  int fixchart=0;
  if(ccx > xx1)
    {
      if(xx1 >= xx0)
        {
          EZ_SSheetRedrawRegion(widget,drawable, xx1, yy0, ccx, ccy, HIGHLIGHT_R);
          fixchart |= updateCharts(widget, xx1,yy0,ccx, ccy, 1);
        }
      else
        {
          EZ_SSheetRedrawRegion(widget,drawable, xx1, yy0, ccx-1, yy1, 0); 
          fixchart |= updateCharts(widget, xx1, yy0, ccx-1, yy1, 1); 
        }
    }
  else if(ccx < xx1)
    {
      if(xx1 > xx0)
        {
          EZ_SSheetRedrawRegion(widget,drawable, xx1, yy0, ccx+1, yy1, 0);
          fixchart |= updateCharts(widget, xx1, yy0, ccx+1, yy1, 1);
        }
      else
        {
          EZ_SSheetRedrawRegion(widget,drawable, xx1, yy0, ccx, ccy, HIGHLIGHT_R);
          fixchart |= updateCharts(widget,  xx1, yy0, ccx, ccy, 1);
        }          
    }
  if(ccy > yy1)
    {
      if(yy1 >= yy0)
        {
          EZ_SSheetRedrawRegion(widget,drawable, xx0, yy1, ccx, ccy,  HIGHLIGHT_R);
          fixchart |= updateCharts(widget,   xx0, yy1, ccx, ccy, 1);
        }
      else
        {
          EZ_SSheetRedrawRegion(widget,drawable, xx0, yy1, xx1, ccy-1, 0); 
          fixchart |= updateCharts(widget, xx0, yy1, xx1, ccy-1, 1); 
        }
    }
  else if(ccy < yy1)
    {
      if(yy1 > yy0)
        {
          EZ_SSheetRedrawRegion(widget,drawable, xx0, yy1, xx1, ccy+1, 0);
          fixchart |= updateCharts(widget,  xx0, yy1, xx1, ccy+1, 1);
        }
      else
        {
          EZ_SSheetRedrawRegion(widget,drawable, xx0, yy1, ccx, ccy, HIGHLIGHT_R); 
          fixchart |= updateCharts(widget, xx0, yy1, ccx, ccy, 1); 
        }
    }
  if(fixchart&1)
    {
      int fix = EZ_SSheetRedrawCharts(widget, drawable);
      if(fix & 65536) drawHead(widget, drawable);
      if(fix & ~65536)
        {
          EZ_FixFocusHighlightBorder(widget, drawable, EZ_WidgetWidth(widget),
                                     EZ_WidgetHeight(widget), EZ_WidgetPadB(widget));
          EZ_DrawRectBorder(widget, drawable);
        }
      XSync(EZ_Display, 0);
    }
}
/*************************************************************************
 *
 *  Event handling
 */
#define SSHEET_BUFFER_SIZE 32

void  EZ_SSheetEventHandle(widget, event)
     EZ_Widget *widget;
     XEvent          *event;
{
  XEvent            xevent;
  EZ_SSItem         *row, *data;

  if(widget == (EZ_Widget *)NULL) return;
  if(event->type == Expose && event->xexpose.count == 0)
    EZ_DrawWidget(widget);      
  if( EZ_GetWidgetDisabledFlag(widget) )
    return;

  switch(event->type)
    {
    case ConfigureNotify:
      EZ_SSheetDirty(widget) = 1;
      break;
    case MappingNotify:
      XRefreshKeyboardMapping((XMappingEvent *)event);
      break;
    case MotionNotify:
      {
        int x=event->xmotion.x;
        int y=event->xmotion.y;
        int onbdy = moveBoundry(widget, x, y, 1);
        if(onbdy) XDefineCursor(EZ_Display, EZ_WidgetWindow(widget),EZ_GetCursor(onbdy));
        else XDefineCursor(EZ_Display, EZ_WidgetWindow(widget), EZ_WidgetCursor(widget));                           
        XSync(EZ_Display, 0);
        while(XCheckTypedEvent(EZ_Display, MotionNotify, &xevent));
      }
      break;
    case ButtonPress:
      if(event->xbutton.button == EZ_Btn1)
        {
          int x = event->xbutton.x;
          int y = event->xbutton.y;
          int movebdy = 0, find, cx=-1, cy=-1;
          Window win = EZ_WidgetWindow(widget);

          /* get keyboard focus */
          { 
            Window fw; int rt;
            EZ_Widget *tmp;
            XGetInputFocus(EZ_Display, &fw, &rt);
            tmp = EZ_LookupWidgetFromMappedHT(fw);
            if(tmp != widget) EZ_SetFocusTo(widget);
          }
          
          movebdy = moveBoundry(widget, x, y, 0);

	  if(movebdy == 0)
	    {
              EZ_Item **charts = EZ_SSheetCharts(widget);
              int     ncharts =  EZ_SSheetNCharts(widget);
              EZ_Item *citem = EZ_FindPointerOnItemIWA(charts, ncharts, NULL, x,y);
              if(citem)
                {
                  Time  time_stamp = event->xbutton.time;   
                  int   elapsedtime = EZ_LastButton1ClickTime - time_stamp; 
                  EZ_LastButton1ClickTime = time_stamp;

		  if(ABSV(elapsedtime) < DOUBLE_CLICK_TIME_INTERVAL)
		    { 
		      EZ_HandleItemCallBacks(citem);
		      EZ_ExecuteWidgetCallBacks(widget);
		      return;
		    }	  
                  else
                    {
                      int i, bwpad =  EZ_SSheetBwPad(widget);
                      int Rx = bwpad + EZ_SSheetTWidth(widget);
                      int Ry = bwpad + EZ_SSheetTHeight(widget);
                      int Rw = EZ_WidgetWidth(widget) - (Rx+bwpad);
                      int Rh = EZ_WidgetHeight(widget) -(Ry+bwpad);
                      int hmode = EZ_SSheetHMode(widget);
                      unsigned long dbg = EZ_SSheetDBG(widget);
                      unsigned long dfg = EZ_SSheetDFG(widget);
                      EZ_Item *ocitem = EZ_SSheetSChart(widget);
                      EZ_SSheetSChart(widget) = citem;
                      if(ocitem && ocitem != citem) 
                        EZ_UpdateOneItem(widget, win, ocitem, Rx, Ry, Rw,Rh);
                      EZ_HighlightOneItem(widget, win, citem, dfg, dbg, Rx,Ry,Rw,Rh, hmode);
                      if(event->xbutton.state != 0)
                      {
                        int r1= EZ_FigItemR1(citem);
                        int c1= EZ_FigItemC1(citem);
                        int r2= EZ_FigItemR2(citem);
                        int c2= EZ_FigItemC2(citem);
                        EZ_SSheetHighlightRegion(widget, -1, 0, 0, 0); /* clear */
                        EZ_SSheetHighlightRegion(widget, r1,c1, r2, c2);
                        EZ_SSheetSx0(widget)=c1; EZ_SSheetSy0(widget)=r1;
                        EZ_SSheetSx1(widget)=c2; EZ_SSheetSy1(widget)=r2;
                        EZ_SSheetSelectionMode(widget) = 5; 
                        c1 = EZ_SSheetCx(widget);
                        r1 = EZ_SSheetCy(widget);
                        EZ_SSheetSelectCellWork(widget, win, c1, r1, 0);
                      }
                      EZ_IWorkAreaMoveOneItem(widget, citem, x, y, Rx, Ry, Rw, Rh);
                      for(i = 0; i < ncharts; i++) 
                        { 
                          if(charts[i] == citem)
                            { charts[i]=charts[ncharts-1];charts[ncharts-1]=citem;break;}
                        }
                      return;
                    }
                }
	      find = EZ_SSheetFindCursorCell(widget, x, y, &cy, &cx);
	      if(find)
		{
                  int xx0, xx1, yy0, yy1;
		  int ccx, ccy, tout=20;
		  unmarkActiveItem(widget);
		  EZ_SSheetSelectCellWork(widget, win, cx, cy, MARK_TAB|CLEAR_REGION|START_REGION|UPDATE_CHART);
		  xx0 = EZ_SSheetSx0(widget);
		  yy0 = EZ_SSheetSy0(widget);
		  xx1 = xx0;  yy1 = yy0;

		  while(1)
		    {
		      XNextEvent(EZ_Display, &xevent);
                      EZ_FilterEvent(&xevent);
                      EZ_InvokePrivateEventHandler(&xevent);
		      if(xevent.type == ButtonRelease) { break; }
		      else if(xevent.type == MotionNotify)
			{
			  x = xevent.xmotion.x;
			  y = xevent.xmotion.y;
			  if(doScroll(widget, &x, &y))
			    {
			      EZ_SSheetDirty(widget) =1; EZ_DrawWidget(widget); 
			      if(tout > 0) {EZ_SitForALittleBit(tout*3000);  tout--;}
			    }
                          XSync(EZ_Display, 0);
			  while(XCheckTypedEvent(EZ_Display, MotionNotify, &xevent));
                          
			  find = EZ_SSheetFindCursorCell(widget, x, y, &ccy, &ccx);		      
			  if(find && (ccx != xx1 || ccy != yy1)) /* optimization is needed here ! */
			    {
			      int ss = (xx0 == xx1 || xx0 == ccx || yy0 == yy1 || yy0 == ccy);
			      EZ_SSheetSx1(widget) = ccx;
			      EZ_SSheetSy1(widget) = ccy;
			      updateHighlightedRegion(widget, win, xx0, yy0, xx1, yy1, ccx, ccy);
			      xx1 = ccx; yy1 = ccy;
			      unmarkActiveItem(widget);
			      if(ss) EZ_SSheetSelectCellWork(widget, win, xx0, yy0, UPDATE_CHART);
			      EZ_SSheetSelectionMode(widget) = 3; /* a rectangle is selected */
			    }
			}
		      else if(xevent.type == Expose || xevent.type == FocusIn || xevent.type == FocusOut)
			EZ_WidgetDispatchEvent(&xevent); 
		    }
		}
	      else if(cx >= 0)
		{
		  int ddd, eee;
		  row = EZ_SSheetHeadRow(widget);
		  if(cx > 0)
		    {
		      eee = x - (row[cx-1].colOrHeight + row[cx-1].rowOrWidth);
		      if( eee >=0 && eee < 8) cx--;
		    }
		  ddd = (row[cx].colOrHeight + row[cx].rowOrWidth) -x;

		  if(ABSV(ddd) < 8)
		    {
		      int tmp =  EZ_SSheetBwPad(widget);
		      int xx0 =  row[cx].colOrHeight + row[cx].rowOrWidth;
		      int yy0 = tmp;
		      int yy1 = EZ_WidgetHeight(widget) - tmp;
		      int total = 0;
		      GC  gc = EZ_XORGC;
		      Window win = EZ_WidgetWindow(widget);
		      XDrawLine(EZ_Display, win, gc, xx0, yy0, xx0, yy1);
		      while(1)
			{
			  XNextEvent(EZ_Display, &xevent);
                          EZ_FilterEvent(&xevent);
                          EZ_InvokePrivateEventHandler(&xevent);
			  if(xevent.type == MotionNotify)
			    {
			      int mx = xevent.xmotion.x;
			      if( mx != xx0)
				{
				  XDrawLine(EZ_Display, win, gc, xx0, yy0, xx0, yy1);
				  total += mx - xx0;
				  xx0 = mx;
				  XDrawLine(EZ_Display, win, gc, xx0, yy0, xx0, yy1);
				}
			    }
			  else if(xevent.type == ButtonRelease) 
			    {
			      XDrawLine(EZ_Display, win, gc, xx0, yy0, xx0, yy1);
			      break;
			    }
			  else if(xevent.type == Expose || xevent.type == FocusIn || xevent.type == FocusOut)
			    EZ_WidgetDispatchEvent(&xevent);                    
			}

		      if(total)
			{
			  if(total + row[cx].rowOrWidth < 8) total = 8- row[cx].rowOrWidth;
			  row[cx].rowOrWidth += total;
			  EZ_SSheetDirty(widget) = 1;
			  EZ_DrawWidget(widget);
			}
		    }
		  else /* select columns */
		    {
		      int yy0 = EZ_SSheetY0(widget);
		      int last = EZ_SSheetNRows(widget)-1;
		      int cx2 = cx, done = 0;
		      int tout = 20;
		      EZ_SSheetHighlightRegion(widget, -1, 0, 0, 0); /* clear */
		      EZ_SSheetRedrawRegion(widget,win, cx, yy0, cx2, last, HIGHLIGHT_R);
		      EZ_SSheetSelectCellWork(widget, win, cx, yy0, START_REGION|UPDATE_CHART);
		      EZ_SSheetSx1(widget) = cx;
		      EZ_SSheetSy1(widget) = last;
		      unmarkActiveItem(widget);
		      while(!done)
			{
			  XNextEvent(EZ_Display, &xevent);
                          EZ_FilterEvent(&xevent);
                          EZ_InvokePrivateEventHandler(&xevent);
			  if(xevent.type == MotionNotify)
			    {
			      int mx = xevent.xmotion.x;
			      int my = EZ_SSheetTHeight(widget) + EZ_SSheetBwPad(widget)+1;
			      int ccx=-1, ccy;
			      if(doScroll(widget, &mx, &my))
				{
				  EZ_SSheetDirty(widget) =1 ; EZ_DrawWidget(widget); XSync(EZ_Display, 0);
				  if(tout > 0) {EZ_SitForALittleBit(tout*3000);  tout--;}
				}
			      EZ_SSheetFindCursorCell(widget, mx, 10, &ccy, &ccx);
			      if(ccx >= 0 && ccx != cx2)
				{
				  EZ_SSheetSx1(widget) = ccx;
				  EZ_SSheetSy1(widget) = last;
				  updateHighlightedRegion(widget, win, cx, yy0, cx2, last, ccx, last);
				  EZ_SSheetSelectCellWork(widget, win, cx, yy0, 0);
				  cx2 = ccx;
				}
			      while(XCheckTypedEvent(EZ_Display, MotionNotify, &xevent));
			    }
			  else if(xevent.type == ButtonRelease) done = 1;
			  else if(xevent.type == Expose || xevent.type == FocusIn || xevent.type == FocusOut)
			    EZ_WidgetDispatchEvent(&xevent);
			}
		      EZ_SSheetSelectionMode(widget) = 2; /* columns are selected */
		    }
		}
	      else if(cy >= 0)
		{
		  int ddd, eee;
		  data = EZ_SSheetData(widget);
		  if(cy > 0)
		    {
		      eee = y -(data[cy].colOrHeight + data[cy].rowOrWidth);
		      if(eee >= 0 && eee <= 8) cy--;
		    }
		  ddd = y -(data[cy].colOrHeight + data[cy].rowOrWidth);
		  if(ABSV(ddd)<=8)
		    {
		      int tmp =  EZ_SSheetBwPad(widget);
		      int xx0 = tmp;
		      int yy0 =  data[cy].colOrHeight + data[cy].rowOrWidth;
		      int xx1 = EZ_WidgetWidth(widget) - tmp;
		      int total = 0;
		      GC  gc = EZ_XORGC;
		      Window win = EZ_WidgetWindow(widget);
		      XDrawLine(EZ_Display, win, gc, xx0, yy0, xx1, yy0);
		      while(1)
			{
			  XNextEvent(EZ_Display, &xevent);
                          EZ_FilterEvent(&xevent);
                          EZ_InvokePrivateEventHandler(&xevent);
			  if(xevent.type == MotionNotify)
			    {
			      int my = xevent.xmotion.y;
			      if( my != yy0)
				{
				  XDrawLine(EZ_Display, win, gc, xx0, yy0, xx1, yy0);
				  total += my - yy0;
				  yy0 = my;
				  XDrawLine(EZ_Display, win, gc, xx0, yy0, xx1, yy0);
				}
			    }
			  else if(xevent.type == ButtonRelease) 
			    {
			      XDrawLine(EZ_Display, win, gc, xx0, yy0, xx1, yy0);
			      break;
			    }
			  else if(xevent.type == Expose || xevent.type == FocusIn || xevent.type == FocusOut)
			    EZ_WidgetDispatchEvent(&xevent);                    
			}

		      if(total)
			{
			  if(total + data[cy].colOrHeight < 8) total = 8-data[cy].colOrHeight;
			  data[cy].colOrHeight += total;
			  EZ_SSheetDirty(widget) = 1;
			  EZ_DrawWidget(widget);
			}		
		    }
		  else
		    {
		      int xx0 = EZ_SSheetX0(widget);
		      int last = EZ_SSheetNColumns(widget)-1;
		      int cy2 = cy, done = 0;
		      int tout = 20;
		      EZ_SSheetHighlightRegion(widget, -1, 0, 0, 0); /* clear */ 
		      EZ_SSheetRedrawRegion(widget,win, xx0, cy, last, cy2, HIGHLIGHT_R);
		      EZ_SSheetSelectCellWork(widget, win, xx0, cy, START_REGION|UPDATE_CHART);
		      EZ_SSheetSx1(widget) = last;
		      EZ_SSheetSy1(widget) = cy;
		      unmarkActiveItem(widget);
		      while(!done)
			{
			  XNextEvent(EZ_Display, &xevent);
                          EZ_FilterEvent(&xevent);
                          EZ_InvokePrivateEventHandler(&xevent);
			  if(xevent.type == MotionNotify)
			    {
			      int mx = EZ_SSheetTWidth(widget) + EZ_SSheetBwPad(widget)+1;
			      int my = xevent.xmotion.y;
			      int ccy=-1, ccx;
			      if(doScroll(widget, &mx, &my))
				{
				  EZ_SSheetDirty(widget) =1 ; EZ_DrawWidget(widget); XSync(EZ_Display, 0);
				  if(tout > 0) {EZ_SitForALittleBit(tout*3000);  tout--;}
				}
			      EZ_SSheetFindCursorCell(widget, 4, my, &ccy, &ccx);
			      if(ccy >= 0 && ccy != cy2)
				{
				  EZ_SSheetSx1(widget) = last;
				  EZ_SSheetSy1(widget) = ccy;
				  updateHighlightedRegion(widget, win, xx0, cy, last, cy2, last, ccy);
				  EZ_SSheetSelectCellWork(widget, win, xx0, cy, 0);
				  cy2 = ccy; 
				}
			      while(XCheckTypedEvent(EZ_Display, MotionNotify, &xevent));
			    }
			  else if(xevent.type == ButtonRelease) done = 1;
			  else if(xevent.type == Expose || xevent.type == FocusIn || xevent.type == FocusOut)
			    EZ_WidgetDispatchEvent(&xevent);
			}
		      EZ_SSheetSelectionMode(widget) = 1; /* rows are selected */
		    }
		}
	      else
		{
		  int bwpad = EZ_SSheetBwPad(widget);
		  int th = EZ_SSheetTHeight(widget);
		  int tw = EZ_SSheetTWidth(widget);
		  if( x > bwpad && y > bwpad && x <tw+bwpad && y < th+bwpad)
		    { 
		      int xlast = EZ_SSheetNColumns(widget)-1;
		      int ylast = EZ_SSheetNRows(widget)-1;
		      int xx0 = EZ_SSheetX0(widget);
		      int yy0 = EZ_SSheetY0(widget);
		      EZ_SSheetSx0(widget) = 0;
		      EZ_SSheetSy0(widget) = 0;
		      EZ_SSheetSx1(widget) = xlast;
		      EZ_SSheetSy1(widget) = ylast;
		      updateHighlightedRegion(widget, win, xx0, yy0, xx0, yy0, xlast, ylast);
		      EZ_SSheetSelectCellWork(widget, win, xx0, yy0, UPDATE_CHART);
		    }
		  EZ_SSheetSelectionMode(widget) = 4;
		}
	      if(EZ_SSheetSelectionMode(widget)) /* copy to kill buffer */
		copyToKillBuffer(widget);
	    }
	}
      else if(event->xbutton.button == EZ_Btn2) /* paste selection */
        {
          int x = event->xbutton.x;
          int y = event->xbutton.y;
          int cx=-1, cy=-1;
          int     ncharts =  EZ_SSheetNCharts(widget);
          EZ_Item **charts = EZ_SSheetCharts(widget);
          EZ_Item *citem = EZ_FindPointerOnItemIWA(charts, ncharts, NULL, x,y);
          if(citem == NULL && EZ_SSheetFindCursorCell(widget, x, y, &cy, &cx)) 
            {
	      unmarkActiveItem(widget);
              EZ_SSheetSelectCellWork(widget, EZ_WidgetWindow(widget), cx, cy,
                                      MARK_TAB|CLEAR_REGION|START_REGION);
              EZ_SSheetPasteSelection(widget);         
            }
        }
      else if(event->xbutton.button == EZ_Btn3)
	{
          EZ_HandleDnDEvents(widget, event);
	}
      break;
    case ButtonRelease:
      break;
    case KeyPress:
      {
        int               count, modifiers;

        KeySym            keysym;
        XComposeStatus    compose; 
        char              tmpbuffer[32];
        int               buffersize = 32, done=0;
        int               xx=0, yy=0, dx=0, dy=0, tmp; 
        
        keysym = XLookupKeysym((XKeyEvent *)event, 0);
	modifiers = event->xkey.state & (ControlMask | Mod1Mask);
	xevent.xkey.state &= ~modifiers;
	
        switch(keysym)
          {
          case XK_Left: 
#ifdef XK_KP_Left
          case XK_KP_Left: 
#endif
            dx =-1;
            break;
          case XK_Right:    
#ifdef XK_KP_Right
          case XK_KP_Right:  
#endif
            dx = 1;
	    break;
          case XK_Up: 
#ifdef XK_KP_Up
          case XK_KP_Up:
#endif
	    dy = -1;
            break;
          case XK_Down: 
#ifdef XK_KP_Down
          case XK_KP_Down:  
#endif
	    dy = 1;
            break;
          case XK_Page_Down:
#ifdef XK_KP_Page_Up
          case XK_KP_Page_Down:
#endif
            xx = EZ_SSheetCx(widget);
            yy =  EZ_SSheetCy(widget);
	    EZ_SSheetY0(widget) = EZ_SSheetY1(widget);
            EZ_SSheetCy(widget) = tmp =  EZ_SSheetY1(widget);
	    unmarkActiveItem(widget);
            EZ_ComputeSSheetWindow(widget, DO_SCROLL);
	    EZ_SSheetDirty(widget) = 1;
	    EZ_DrawWidget(widget);
            EZ_SSheetCy(widget) = yy;
            EZ_SSheetSelectCellWork(widget, EZ_WidgetWindow(widget), xx, tmp,
                                    MARK_TAB|CLEAR_REGION|START_REGION);
            done = 1;
            break;
          case XK_Page_Up:
#ifdef XK_KP_Page_Up
          case XK_KP_Page_Up:
#endif
            xx = EZ_SSheetCx(widget);
            yy =  EZ_SSheetCy(widget);
            dy =  EZ_SSheetY0(widget);
            while(EZ_SSheetY1(widget) != dy && EZ_SSheetY0(widget) > 0)
              {
                EZ_SSheetY0(widget) -= 1;
                EZ_ComputeSSheetWindow(widget, 0); 
              }
	    unmarkActiveItem(widget);
	    EZ_SSheetCy(widget) = dy;
	    EZ_ComputeSSheetWindow(widget, DO_SCROLL);
	    EZ_SSheetDirty(widget) = 1;
	    EZ_DrawWidget(widget);
	    EZ_SSheetCy(widget) = yy;
            EZ_SSheetSelectCellWork(widget, EZ_WidgetWindow(widget),  xx, dy,
                                    MARK_TAB|CLEAR_REGION|START_REGION);
            dy = 0;
            done = 1;
            break;
          case XK_Home:
#ifdef XK_KP_Home
          case XK_KP_Home:
#endif         
	    if(EZ_SSheetX0(widget) || EZ_SSheetY0(widget))
	      {
		EZ_SSheetX0(widget) = 0;
		EZ_SSheetY0(widget) = 0;
		unmarkActiveItem(widget);
		EZ_SSheetDirty(widget) = 1;
		EZ_DrawWidget(widget);
		EZ_SSheetSelectCellWork(widget, EZ_WidgetWindow(widget), 0, 0,
					MARK_TAB|CLEAR_REGION|START_REGION|UPDATE_CHART);
	      }
            done = 1;
            break;
          default:
            break;
          }
	if(dx | dy )
	  {
	    int ddd, ccc = UPDATE_CHART;
            xx = EZ_SSheetCx(widget);
            yy = EZ_SSheetCy(widget);            
	    dx += EZ_SSheetCx(widget);
	    dy += EZ_SSheetCy(widget);
	    if(dx < 0) dx = 0; if(dy < 0) dy = 0;
	    EZ_SSheetCx(widget) = dx;	    EZ_SSheetCy(widget) = dy;
	    unmarkActiveItem(widget);
            ddd = EZ_ComputeSSheetWindow(widget, DO_SCROLL);
            EZ_SSheetCx(widget) = xx;
            EZ_SSheetCy(widget) = yy;
	    if(ddd)
	      {
		EZ_SSheetDirty(widget) =1;
		EZ_DrawWidget(widget);
                ccc = 0;
	      }
            EZ_SSheetSelectCellWork(widget, EZ_WidgetWindow(widget), dx, dy,
                                    MARK_TAB|CLEAR_REGION|START_REGION|ccc);
            done = 1;
	  }

	/* editing an item ... */
	if(!done) /* not selection movement */
	  {
	    EZ_SSItem *currentItem=EZ_SSheetActiveItem(widget);
	    count = XLookupString( &(event->xkey), tmpbuffer, buffersize, &keysym, &compose);
	    if(count == 0 && (keysym == XK_Shift_L || keysym == XK_Shift_R)) break;
	    tmpbuffer[count] = '\0';

	    switch(keysym)
	      {
	      case XK_Return:
#ifdef XK_KP_Enter
	      case XK_KP_Enter:
#endif
	      case XK_Linefeed: 
		{
                  int ox = EZ_SSheetCx(widget);
                  int oy = EZ_SSheetCy(widget);
                  xx = EZ_SSheetTabX(widget);
                  if(EZ_SSheetSelectionMode(widget))
                    {
                      int xx0 = EZ_SSheetSx0(widget);
                      int yy0 = EZ_SSheetSy0(widget);
                      int xx1 = EZ_SSheetSx1(widget);
                      int yy1 = EZ_SSheetSy1(widget);
                      int tmp;
                      if(yy0 > yy1) { tmp=yy0; yy0=yy1; yy1=tmp;}
                      if(xx0 > xx1) { tmp=xx0; xx0=xx1; xx1=tmp;}

                      if(oy < yy1) { xx = ox; yy = oy+1;}
                      else if(ox < xx1) { xx = ox+1;  yy = yy0;}
                      else { xx = xx0; yy = yy0;}
                    }
                  else
                    { 
                      xx = ox;
                      yy = oy+1;
                    }
		  unmarkActiveItem(widget);
                  EZ_SSheetCx(widget) = xx;
                  EZ_SSheetCy(widget) = yy;
                  EZ_ComputeSSheetWindow(widget, DO_SCROLL);
                  EZ_SSheetCx(widget) = ox;
                  EZ_SSheetCy(widget) = oy;
                  EZ_SSheetDirty(widget) =1;
                  EZ_DrawWidget(widget);
                  EZ_SSheetSelectCellWork(widget, EZ_WidgetWindow(widget), xx, yy, UPDATE_CHART);
		}
              break;
	      case  XK_Tab:
#ifdef XK_KP_Tab
	      case XK_KP_Tab:
#endif	 
                {
                  int ox = EZ_SSheetCx(widget);
                  int oy = EZ_SSheetCy(widget);
                  if(EZ_SSheetSelectionMode(widget))
                    {
                      int xx0 = EZ_SSheetSx0(widget);
                      int yy0 = EZ_SSheetSy0(widget);
                      int xx1 = EZ_SSheetSx1(widget);
                      int yy1 = EZ_SSheetSy1(widget);
                      int tmp;
                      if(xx0 > xx1) { tmp=xx0; xx0=xx1; xx1=tmp;}
                      if(yy0 > yy1) { tmp=yy0; yy0=yy1; yy1=tmp;}
                      if(ox < xx1) { xx = ox+1;  yy = oy;}
                      else if(oy < yy1) { xx = xx0; yy = oy+1;}
                      else { xx = xx0; yy = yy0;}
                    }
                  else
                    { /* tab to the next cell */

                      xx = ox+1;
                      yy = oy;
                    }
                  unmarkActiveItem(widget);
                  EZ_SSheetCx(widget) = xx;
                  EZ_SSheetCy(widget) = yy;
                  EZ_ComputeSSheetWindow(widget, DO_SCROLL);
                  EZ_SSheetCx(widget) = ox;
                  EZ_SSheetCy(widget) = oy;
                  EZ_SSheetDirty(widget) =1;
                  EZ_DrawWidget(widget);
                  EZ_SSheetSelectCellWork(widget, EZ_WidgetWindow(widget), xx, yy, UPDATE_CHART);
                }
              break;
	      case XK_BackSpace: 
	      case XK_Delete:
                {
                  int cx = EZ_SSheetCx(widget);
                  int cy = EZ_SSheetCy(widget);
                  EZ_SSItem *currentItem = EZ_SSheetActiveItem(widget);
                  if(currentItem == NULL)
                    {
                      EZ_SSItem *prev = NULL;
                      currentItem = EZ_SSheetActiveItem(widget) = 
                        locateCreateSSItem(widget, cy, cx, 0, &prev);
                    }
                  if(currentItem)
                    {
                      ITextLine *tline =  currentItem->text;
                      char *str = tline->data;
                      int cursorx;
                      currentItem->dirty = 1;
                      currentItem->status = 1;
                      cursorx = EZ_SSheetCursorX(widget);

                      if(cursorx > 0)
                        {
                          str[cursorx-1] = '\0';
                          tline->length--;
                          EZ_SSheetCursorX(widget) -= 1;
                          redrawCell(widget, EZ_WidgetWindow(widget), currentItem, cy, cx, RED_BOX); 
                        }
                      else 
                        {
                          str[0] = 0;
                          tline->length = 0;
                          redrawCell(widget, EZ_WidgetWindow(widget), currentItem, cy, cx, RED_BOX); 
                        }
		      EZ_HandleMotionCallBacks(widget, EZ_WidgetMotionCallBack(widget));
                    }
                }
              break;
	      case XK_Insert:
#ifdef XK_KP_Page_Insert
	      case XK_KP_Page_Insert:
#endif
                EZ_SSheetPasteSelection(widget);  
                break;
	      default:
		if(modifiers == 0 && count) /* do insert */
		  {
		    int cx = EZ_SSheetCx(widget);
		    int cy = EZ_SSheetCy(widget);
		    if(currentItem == NULL)
		      {
			EZ_SSItem *prev = NULL;
			currentItem = EZ_SSheetActiveItem(widget) = 
			  locateCreateSSItem(widget, cy, cx, count, &prev);
			if(currentItem) 
			  { 
			    ITextLine *tline =  currentItem->text;			    
			    tline->length = 0;
                            EZ_SSheetCursorX(widget) = 0;
			  }
		      }
		    if(currentItem)
		      {
			ITextLine *tline =  currentItem->text;
			TextProp *prop, **oldp;
			char *str;
			int len=tline->length, cursorx;
			EZ_GrowITextLine(&tline, len+count);
			currentItem->text = tline;
			prop = EZ_SSheetTextProperty(widget);
			oldp = tline->prop;
			str = tline->data;
			currentItem->dirty = 1;
			currentItem->status = 1;
			cursorx = EZ_SSheetCursorX(widget);
			str[cursorx] = tmpbuffer[0]; str[cursorx+1] = 0;
			oldp[cursorx] = prop;
			tline->length++;
			EZ_SSheetCursorX(widget) += 1;
			redrawCell(widget, EZ_WidgetWindow(widget), currentItem, cy, cx, RED_BOX);
			EZ_HandleMotionCallBacks(widget, EZ_WidgetMotionCallBack(widget));
		      }
		  }
		break;
	      }          
	  }
      }
    case EnterNotify:
      break;
    case LeaveNotify:
      break;
    case SelectionNotify:
      {
	XSelectionEvent *Sevent = (XSelectionEvent *)event;
	Window window = Sevent->requestor;
	Atom   property = Sevent->property;

	EZ_PasteSelection(widget, window,  property,  True, EZ_SSheetInsertString); 
      }
      break;
    case SelectionRequest:
      EZ_HandleSelectionRequest(widget, event);
      break;
    case SelectionClear:
      break;
    }
}
/*******************************************************************/
static EZ_SSItem *locateCreateSSItem(wptr, i, j, cte, prev) 
     EZ_Widget *wptr; int i, j, cte; EZ_SSItem **prev;
{
  int nrows = EZ_SSheetNRows(wptr);
  int ncols = EZ_SSheetNColumns(wptr);
  EZ_SSItem *data, *ptr, *qtr, *retv=NULL;

  if(i < 0 || j < 0) return(NULL);
  if(i >= nrows || j >= ncols)  EZ_FixSSheetHead(wptr, i+i, j+j);
  /* the work */
  data = EZ_SSheetData(wptr);
  ptr = &(data[i]);
  qtr = ptr->next;  
  while(qtr)
    {
      int cidx = qtr->colOrHeight;
      if(cidx == j)
	{
	  *prev = ptr;
	  retv = qtr;
	  break;
	}
      else if(cidx > j)
	{
	  *prev = ptr;
	  retv = NULL;
	  break;
	}
      else
	{
	  ptr = qtr;
	  qtr = ptr->next;
	}
    }
  *prev = ptr;
  if(cte && retv == NULL)
    {
      EZ_SSItem *row0 = EZ_SSheetHeadRow(wptr);   
      EZ_SSItem *item = allocateNewSSItem();
      item->rowOrWidth  = i;
      item->colOrHeight = j;
      item->dirty = 1;
      item->just = row0[j].just;
      item->next = ptr->next;
      ptr->next = item;
      retv = item;
    }
  return(retv);
}
/*******************************************************************/ 

int EZ_SSheetFindCursorCell(wptr, x, y, cy, cx)
     EZ_Widget *wptr; int x, y, *cx, *cy;
{
  int x0 = EZ_SSheetX0(wptr);
  int x1 = EZ_SSheetX1(wptr);
  int y0 = EZ_SSheetY0(wptr);
  int y1 = EZ_SSheetY1(wptr);
  int i, find = 0;
  EZ_SSItem *row = EZ_SSheetHeadRow(wptr);
  EZ_SSItem *data = EZ_SSheetData(wptr);
  
  for(i = x0; i <= x1; i++)
    {
      int t = row[i].colOrHeight;
      if(x >= t && x < t + row[i].rowOrWidth)
	{ find = 1; *cx = i; break;}
    }
  for(i = y0; i <= y1; i++)
    {
      int t = data[i].rowOrWidth;
      if(y >= t && y <= t + data[i].colOrHeight)
	{ find++;  *cy = i; break; }
    }
  return((find == 2));
}
/*******************************************************************/
void EZ_SSheetSetActiveCell(wptr, row, col) EZ_Widget *wptr; int row,col;
{
  int k= EZ_SSheetSelectCellWork(wptr, EZ_WidgetWindow(wptr), col, row, 
			  CLEAR_REGION|MARK_TAB|START_REGION);
  EZ_SSheetSelectionMode(wptr) = 0; /* mark unselected */
  if(k==0 && EZ_WidgetMapped(wptr))
    {
      EZ_ComputeSSheetWindow(wptr, DO_SCROLL);
      EZ_DrawWidget(wptr);
    }
}
/*******************************************************************/
int EZ_SSheetSelectCellWork(wptr, drawable, cx, cy, flag) /* flag reset region */
     EZ_Widget *wptr; int cx, cy; int flag; Drawable drawable;
{
  int x0 = EZ_SSheetX0(wptr);
  int x1 = EZ_SSheetX1(wptr);
  int y0 = EZ_SSheetY0(wptr);
  int y1 = EZ_SSheetY1(wptr);
  int nrows = EZ_SSheetNRows(wptr);
  int ncols = EZ_SSheetNColumns(wptr);
  int drawit =1, drawchart = flag & UPDATE_CHART;
  if(EZ_WidgetMapped(wptr) == 0 || EZ_GetWidgetFreezedFlag(wptr))
    {
      EZ_SSheetDirty(wptr) = 1;
      drawit =0;
    }
  
  if(flag & CLEAR_REGION) /* clear old selection */
    {
      int occx = EZ_SSheetSx0(wptr);
      int occy = EZ_SSheetSy0(wptr); 
      int x2 = EZ_SSheetSx1(wptr);
      int y2 = EZ_SSheetSy1(wptr);
      if(drawit) EZ_SSheetRedrawRegion(wptr, drawable, occx,occy, x2, y2, drawchart);
      EZ_SSheetSelectionMode(wptr) = 0;
    }
  if(EZ_SSheetNCharts(wptr) == 0) drawchart = 0;
  if(cx < 0 || cx > ncols || cy < 0 || cy > nrows)
    {
      EZ_RingBell();
      cx = EZ_SSheetCx(wptr);
      cy = EZ_SSheetCy(wptr);
    }

  if(cx >= 0 && cx < ncols && cy >= 0 && cy < nrows)
    {
      int ocx = EZ_SSheetCx(wptr);
      int ocy = EZ_SSheetCy(wptr);
      if(flag & CLEAR_REGION) EZ_SSheetSx1(wptr) = -1;
      
      if(flag & START_REGION)
        {
          EZ_SSheetSx0(wptr) = cx;
          EZ_SSheetSy0(wptr) = cy;
        }
      if(flag & MARK_TAB)
	{
          EZ_SSheetTabX(wptr) = cx;
          EZ_SSheetTabY(wptr) = cy;
	}
      EZ_SSheetCx(wptr) = cx;
      EZ_SSheetCy(wptr) = cy;

      if(drawit && cx >= x0 && cx <= x1 && cy >= y0 && cy <= y1)
	{
          int hhh = 1;
          if(ocx != cx || ocy != cy) redrawCell(wptr, drawable, NULL, ocy, ocx, 0);
          redrawCell(wptr, drawable, NULL, cy, cx, RED_BOX);
          if(drawchart)
            {
              int fix = 0;
              fix |= updateCharts(wptr, ocx, ocy, ocx, ocy, 0);
              fix |= updateCharts(wptr, cx, cy, cx, cy, 0);
              if(fix)
                {
                  fix = EZ_SSheetRedrawCharts(wptr, drawable);
                  if(fix & 65536) { drawHead(wptr, drawable); hhh = 0;}
                  if(fix & ~65536)
                    {
                      EZ_FixFocusHighlightBorder(wptr, drawable, EZ_WidgetWidth(wptr),
                                                 EZ_WidgetHeight(wptr), EZ_WidgetPadB(wptr));
                      EZ_DrawRectBorder(wptr, drawable);
                    }
                  XSync(EZ_Display, 0);
                }
            }
          if(hhh) highlightHead(wptr, drawable, ocx, ocy, flag&UPDATE_HEAD);
        } else drawit = 0;

      if(ocx != cx || ocy != cy) 
	EZ_HandleMotionCallBacks(wptr, EZ_WidgetMotionCallBack(wptr));
    }
  return(drawit);
}
/*******************************************************************
 *
 * highlight region [x0 x1]x[y0 y1].
 * if x0 < 0, clear current region 
 */
void EZ_SSheetHighlightRegion(wptr, y0, x0, y1, x1) 
     EZ_Widget *wptr; int x0,y0,x1,y1;
{
  if(EZ_WidgetMapped(wptr))
    {
      int highlight = HIGHLIGHT_R;
      if(x0 < 0 || y0 < 0)
	{
	  x0 = EZ_SSheetSx0(wptr);
	  y0 = EZ_SSheetSy0(wptr); 
	  x1 = EZ_SSheetSx1(wptr);
	  y1 = EZ_SSheetSy1(wptr);
	  EZ_SSheetSx1(wptr) = -1;
	  highlight = 0;
	}
      EZ_SSheetRedrawRegion(wptr, EZ_WidgetWindow(wptr), x0, y0, x1, y1, highlight);
    }
}
/*******************************************************************/
void EZ_SSheetRedrawRegion(wptr, drawable, x1, y1, x2, y2, flag)
     EZ_Widget *wptr; Drawable drawable;  int x1, y1, x2, y2, flag;
{
  int xx0 = EZ_SSheetX0(wptr);
  int xx1 = EZ_SSheetX1(wptr);
  int yy0 = EZ_SSheetY0(wptr);
  int yy1 = EZ_SSheetY1(wptr);
  int tmp, fixborder=0, drawchart = flag & UPDATE_CHART;
  int highlight = flag & HIGHLIGHT_R;  	 

  if(x2 < 0) { x2 = x1; y2 = y1;}
  if(x1 > x2) { tmp=x1; x1=x2; x2=tmp;}
  if(y1 > y2) { tmp=y1; y1=y2; y2=tmp;}  

  if(x2 < xx0 || y2 < yy0 || x1 > xx1 || y1 > yy1) return;

  x1 = MAXV(x1, xx0); x2 = MINV(x2, xx1);
  y1 = MAXV(y1, yy0); y2 = MINV(y2, yy1);

  if(x2 == xx1 || y2 == yy1) fixborder = 1;
  if(x1 == x2 && y1 == y2) drawchart = 0;
  /* repaint [x1, x2]X[y1,y2] */
  {
    EZ_SSItem *row = EZ_SSheetHeadRow(wptr);
    EZ_SSItem *data = EZ_SSheetData(wptr);

    int sx, sy, x, y, w, h, sepx, tw, th, i, tmp, bwpad;
    unsigned long dfg, dbg, bgpv;
    XFontStruct *defaultfont = EZ_WidgetFont(wptr);
    GC gc;
    sepx = EZ_WidgetSepX(wptr);
    tw = EZ_SSheetTWidth(wptr);
    th = EZ_SSheetTHeight(wptr);
    bwpad = EZ_SSheetBwPad(wptr);
    
    if(highlight)
      {
	dfg = EZ_SSheetSelectionFG(wptr);
	dbg = EZ_SSheetSelectionBG(wptr);
      }
    else
      {
	dfg = EZ_SSheetDFG(wptr);
	dbg = EZ_SSheetDBG(wptr);
      } 
    
    /* 1: repaint bg */
    sx=x = row[x1].colOrHeight;
    w = row[x2].colOrHeight + row[x2].rowOrWidth - x;
    sy=y = data[y1].rowOrWidth;
    h = data[y2].rowOrWidth + data[y2].colOrHeight - y;
    if(highlight)
      {
	gc = EZ_WRITABLEGC;
	XSetForeground(EZ_Display, gc, EZ_SSheetSelectionBG(wptr));	
      }
    else
      {
	if(EZ_SSheetTextBG(wptr) != ~0)
	  {
	    gc = EZ_WRITABLEGC;
	    XSetForeground(EZ_Display, gc, EZ_SSheetTextBG(wptr));	
	  }
	else
	  {
	    if(EZ_GetBackgroundGC(wptr, &gc, &bgpv, 0, 0))
	      {
		int ox, oy;
		EZ_GetWidgetTileOrigin(wptr, &ox, &oy);      
		XSetTSOrigin(EZ_Display, gc, ox, oy);
	      }
	  }
      }
    XFillRectangle(EZ_Display, drawable, gc, x,y,w,h);

    /* 2. redraw grids */
    gc = EZ_DARKGC2;
    y = data[y1].rowOrWidth;
    tmp = data[y2].rowOrWidth + data[y2].colOrHeight;
    for(i = x1; i <= x2; i++)
      {
	x = row[i].colOrHeight;
        XDrawLine(EZ_Display, drawable, gc, x, y, x, tmp);
      }
    x = row[x1].colOrHeight;    
    tmp = row[x2].colOrHeight + row[x2].rowOrWidth;
    for(i = y1; i <= y2; i++)
      {
	y = data[i].rowOrWidth;
        XDrawLine(EZ_Display, drawable, gc, x, y, tmp, y);
      }

    /* 3: redraw cells */
    for(i = y1; i<= y2; i++)  
      {
	EZ_SSItem *ptr = data[i].next;
        if(data[i].colOrHeight <= 1) continue;
	while(ptr)
	  {
	    int cidx = ptr->colOrHeight;
	    if(cidx > x2) break;
	    if(cidx >= x1)
	      {
		int cellWidth = row[cidx].rowOrWidth;
                if(cellWidth > 1)
                  {
                    int ascent, xx, yy;
                    int ridx = ptr->rowOrWidth;
                    int xstart = row[cidx].colOrHeight;

                    int ystart = data[ridx].rowOrWidth;
                    int cellHeight =  data[ridx].colOrHeight;
                    ITextLine *tline = ptr->text;	      
                    int just = ptr->just;
                    if(ptr->dirty)
                      {
                        int ttw, tth, tta;
                        EZ_ITextLineDimensionWork(defaultfont, 0, tline, 0, tline->length, &ttw, &tth, &tta);
                        tline->width = ttw;
                        tline->height= tth;
                        tline->y = tta;
                        ptr->dirty = 0;
                      }
                    ascent = tline->y;
                    yy = (cellHeight - tline->height)/2;
                    if(just == EZ_RIGHT) { xx = cellWidth - tline->width - sepx;}
                    else if(just == EZ_CENTER) { xx = (cellWidth - tline->width)/2;}
                    else xx = sepx;
                    EZ_ITextRenderTextWork(defaultfont, dfg, dbg, 0, 0, 0, tline, xstart +xx, ystart + yy,
                                           tline->height, ascent, drawable, 0, 0, 0); 
                  }
              }
	    ptr = ptr->next;
	  }
      }
  }
  if(drawchart) fixborder += EZ_SSheetRedrawCharts(wptr, drawable);
  if(fixborder)
    {
      EZ_FixFocusHighlightBorder(wptr, drawable, EZ_WidgetWidth(wptr),
				 EZ_WidgetHeight(wptr), EZ_WidgetPadB(wptr));
      EZ_DrawRectBorder(wptr, drawable);
      XSync(EZ_Display, 0);
    }
}
/*******************************************************************/
static int updateCharts(wptr, x1, y1, x2, y2, flag)
     EZ_Widget *wptr; int x1, y1, x2, y2, flag;
{
  int ncharts = EZ_SSheetNCharts(wptr);
  if(ncharts > 0)
    {
      EZ_SSItem *row, *data; 
      EZ_Item **charts, *chart;
      int xx0 = EZ_SSheetX0(wptr);
      int xx1 = EZ_SSheetX1(wptr);
      int yy0 = EZ_SSheetY0(wptr);
      int yy1 = EZ_SSheetY1(wptr);
      int bwpad, headw, headh,w,h, tw, th;
      int ret = 0, i,tmp, tx, ty, wx, wy, ww, wh;
      if(x2 < 0) { x2 = x1; y2 = y1;}
      if(x1 > x2) { tmp=x1; x1=x2; x2=tmp;}
      if(y1 > y2) { tmp=y1; y1=y2; y2=tmp;}  
      if( x2 < xx0 || y2 < yy0 || x1 > xx1 || y1 > yy1) return(0);
      /*
      x1 = MAXV(x1, xx0); x2 = MINV(x2, xx1);
      y1 = MAXV(y1, yy0); y2 = MINV(y2, yy1);
      */
      row = EZ_SSheetHeadRow(wptr);
      data = EZ_SSheetData(wptr);      

      wx = row[x1].colOrHeight;
      ww = row[x2].colOrHeight + row[x2].rowOrWidth - wx;
      wy = data[y1].rowOrWidth;
      wh = data[y2].rowOrWidth + data[y2].colOrHeight - wy;
      tx = EZ_SSheetTx(wptr);
      ty = EZ_SSheetTy(wptr);
      bwpad = EZ_SSheetBwPad(wptr);
      headw = EZ_SSheetTWidth(wptr) + bwpad;
      headh = EZ_SSheetTHeight(wptr) + bwpad;
      w = EZ_WidgetWidth(wptr);
      h = EZ_WidgetHeight(wptr);
      tw = EZ_SSheetTWidth(wptr);
      th = EZ_SSheetTHeight(wptr);      
      charts = EZ_SSheetCharts(wptr);
      for(i = 0; i<ncharts; i++)
        {
          int cx, cy, cw, ch,rx,ry,rw,rh;
          chart = charts[i];
          cx = EZ_ItemAOriginX(chart) + tx +bwpad +tw;
          cy = EZ_ItemAOriginY(chart) + ty +bwpad +th;
          cw = EZ_ItemWidth(chart);
          ch = EZ_ItemHeight(chart);
          EZ_IntersectRectangles(wx,wy,ww,wh,cx,cy,cw,ch, &rx,&ry,&rw,&rh);
          if(rw) {if(flag) return(1); else ret |= 1; }
          EZ_IntersectRectangles(0,0,w,headh,cx,cy,cw,ch, &rx,&ry,&rw,&rh);
          if(rw) ret |= 65536;
          else
            {
              EZ_IntersectRectangles(0,0,headw,h,cx,cy,cw,ch, &rx,&ry,&rw,&rh);
              if(rw) ret |= 65536;
            }
        }
      return(ret);
    }
  return(0);
}
/*******************************************************************/
void EZ_SSheetInsertItem(wptr, row, col, str)
     EZ_Widget *wptr; int row, col; char *str;
{
  EZ_SSheetInsertItemWithProperty(wptr, row, col, str, NULL);
}
/*******************************************************************/
void EZ_SSheetInsertItemWithProperty(wptr, row, col, str, prop)
     EZ_Widget *wptr; int row, col; char *str; TextProp **prop;
{
  int docreate = (str != NULL && *str != '\0');
  EZ_SSItem *prev = NULL, *item = locateCreateSSItem(wptr, row, col, docreate, &prev);

  EZ_SSheetDirty(wptr) = 1;
  if(item)
    {
      int i, len = str? strlen(str) : 0;
      ITextLine *tline = item->text;
      TextProp **pptr;
      if(tline) tline->length = 0;
      EZ_GrowITextLine(&tline, len);
      item->text = tline;
      item->dirty = 1;
      if(len) strcpy(tline->data, str);
      else (tline->data)[0] = '\0';
      tline->length = len;
      if(len)
	{
	  memset(tline->prop, 0, len * sizeof(TextProp *));
	  if(prop)
	    {
	      pptr = tline->prop;
	      for(i = 0; i < len; i++) *pptr++ = *prop++;
	    }
	  else
	    {
	      TextProp *p = EZ_SSheetTextProperty(wptr);
	      pptr = tline->prop;
	      for(i = 0; i < len; i++) *pptr++ =p;
	    }
	}
      if(EZ_WidgetMapped(wptr) && EZ_GetWidgetFreezedFlag(wptr) == 0)
	{
	  int x0 = EZ_SSheetX0(wptr);
	  int x1 = EZ_SSheetX1(wptr);
	  int y0 = EZ_SSheetY0(wptr);
	  int y1 = EZ_SSheetY1(wptr);	  
	  if(col >= x0 && col <= x1 && row >= y0 && row <= y1)
	    {
	      int cx = EZ_SSheetCx(wptr);
	      int cy = EZ_SSheetCy(wptr);
	      int flag = 0;
	      if(row == cy && col == cx) flag = RED_BOX;
	      redrawCell(wptr, EZ_WidgetWindow(wptr), item, row, col, flag);
	    }
	}
      {
	int ocx = EZ_SSheetCx(wptr);
	int ocy = EZ_SSheetCy(wptr);
	if(ocx == col && ocy == row)
	  EZ_HandleMotionCallBacks(wptr, EZ_WidgetMotionCallBack(wptr));
      }
    }
}
/*******************************************************************/
static EZ_SSItem *allocateNewSSItem()
{
  ITextLine *tline;
  EZ_SSItem *item = (EZ_SSItem *)my_malloc(sizeof(EZ_SSItem), _SSHEET_DATA_);
  memset(item , 0, sizeof(EZ_SSItem));
  tline = item->text;
  EZ_GrowITextLine(&tline, 16);
  tline->length = 0;
  item->dirty = 1;
  item->text = tline;
  return(item);
}
/*******************************************************************/
static void  unmarkActiveItem(wptr) EZ_Widget *wptr;
{
  EZ_SSItem *item = EZ_SSheetActiveItem(wptr);
  if(item)
    {
      EZ_SSheetActiveItem(wptr) = NULL;
      EZ_SSheetCursorX(wptr) = 0;
      if(item->status)
        {
          int r = item->rowOrWidth;
          int c = item->colOrHeight;
          EZ_SSheetValidateCharts(wptr, r, c, r, c);
          item->status = 0;
        }
    }
}
/*******************************************************************/
static void  redrawCell(widget, drawable, sitem, row, col, flag)
     EZ_Widget *widget; int row, col, flag; Drawable drawable; EZ_SSItem *sitem;
{
  EZ_SSItem *jnk, *item = sitem;
  if(item == NULL) { item = locateCreateSSItem(widget, row, col, 0, &jnk);}
    {
      EZ_SSItem *row0 = EZ_SSheetHeadRow(widget); 
      EZ_SSItem *data = EZ_SSheetData(widget);
      int h = data[row].colOrHeight;
      int w = row0[col].rowOrWidth;
      if(w>1 && h>1)
        {
          int x = row0[col].colOrHeight;
          int y = data[row].rowOrWidth;

          unsigned long dfg = EZ_SSheetDFG(widget);
          unsigned long bgpv, dbg = EZ_SSheetDBG(widget);
          int sepx = EZ_WidgetSepX(widget);
          int bwpad = EZ_SSheetBwPad(widget);
          XFontStruct *defaultfont = EZ_WidgetFont(widget);
          GC gc;
          int fixborder = 0;

          if(EZ_SSheetTextBG(widget) != ~0)
            {
              gc = EZ_WRITABLEGC;
              XSetForeground(EZ_Display, gc, EZ_SSheetTextBG(widget));	
            }
          else
            {
              if(EZ_GetBackgroundGC(widget, &gc, &bgpv, 0, 0))
                {
                  int ox, oy;
                  EZ_GetWidgetTileOrigin(widget, &ox, &oy);      
                  XSetTSOrigin(EZ_Display, gc, ox, oy);
                }
            }
          XFillRectangle(EZ_Display, drawable, gc, x,y,w,h);
          gc = EZ_DARKGC2;
          XDrawLine(EZ_Display, drawable, gc, x, y, x, y+h);
          XDrawLine(EZ_Display, drawable, gc, x, y, x+w, y);
          if(item && item->rowOrWidth == row && item->colOrHeight == col)
            {
              int ascent, xx, yy;
              ITextLine *tline = item->text;	      
              int just = item->just;
              if(item->dirty)
                {
                  int ttw, tth, tta;
                  EZ_ITextLineDimensionWork(defaultfont, 0, tline, 0, tline->length, &ttw, &tth, &tta);
                  tline->width = ttw;
                  tline->height= tth;
                  tline->y = tta;
                  item->dirty = 0;
                }
              ascent = tline->y;
              yy = (h - tline->height)/2;
              if(just == EZ_RIGHT) { xx = w - tline->width - sepx;}
              else if(just == EZ_CENTER) { xx = (w - tline->width)/2;}
              else xx = sepx;
              EZ_ITextRenderTextWork(defaultfont, dfg, dbg, 0, 0, 0, tline, x +xx, y + yy,
                                     tline->height, ascent, drawable, 0, 0, 0); 
            }
          if(flag & RED_BOX)
            XDrawRectangle(EZ_Display, drawable, EZ_DARKREDGC, x+2,y+2,w-3,h-3);
          if(flag & UPDATE_CHART) fixborder = EZ_SSheetRedrawCharts(widget, drawable);      
          if(fixborder || x+w >EZ_WidgetWidth(widget) - bwpad || y+h > EZ_WidgetHeight(widget)-bwpad)
            {
              EZ_FixFocusHighlightBorder(widget, drawable, EZ_WidgetWidth(widget),
                                         EZ_WidgetHeight(widget), EZ_WidgetPadB(widget));
              EZ_DrawRectBorder(widget, drawable);
            }
          XSync(EZ_Display, 0);
        }
    }
}
/*******************************************************************/
EXTERN ITextLine TextWidgetKillBuffer;
void EZ_SSheetPasteSelection(widget) EZ_Widget *widget;
{
  Window owner;
  EZ_Widget *tmp;

  owner = XGetSelectionOwner(EZ_Display, XA_PRIMARY);
  tmp = EZ_LookupWidgetFromMappedHT(owner);
  if(tmp != (EZ_Widget *)NULL)
    {
      int col = EZ_SSheetCx(widget);
      int row = EZ_SSheetCy(widget);
      ITextLine *tline = &TextWidgetKillBuffer;
      char  *str = tline->data;
      TextProp **tprop = (TextWidgetKillBuffer.width ==0? NULL : tline->prop);
      EZ_SSheetInsertRegionStringWithProperty(widget, row, col, str, tprop);
    }
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
		      EZ_SSheetInsertString);
}
/*******************************************************************/
static void EZ_SSheetInsertString(widget, str) EZ_Widget *widget; char *str;
{
  int col = EZ_SSheetCx(widget);
  int row = EZ_SSheetCy(widget);
  EZ_SSheetInsertRegionStringWithProperty(widget, row, col, str, NULL);
}
/*******************************************************************/
void EZ_MarkSSheetUsedTextProperties(wptr) EZ_Widget *wptr;
{
  int nrows = EZ_SSheetNRows(wptr);
  int j;
  
  EZ_SSItem  *data, *ptr, *qtr;
  data = EZ_SSheetData(wptr);
  for(j = 0; j < nrows; j++)
    {
      ptr = &(data[j]);
      qtr = ptr->next;  
      while(qtr)
        {
          ITextLine *tline = qtr->text;
          TextProp *cprop, **prop = tline->prop;
          int i, len = tline->length;
          EZ_Bitmap *bitmap;
          for(i=0; i<len; i++)
            {
              cprop = prop[i];
              if(cprop)
                {
                  if(EZ_TextPropertyMark(cprop) == 0)
                    EZ_TextPropertyMark(cprop) = 1;
                  bitmap = EZ_TextPropertyBitmap(cprop);
                  if(bitmap) bitmap->special = 1;
                }
            }
          qtr = ptr->next;
        }
    }
}
/*******************************************************************/
void EZ_SSheetGetActiveCell(wptr, r, c, str)
     EZ_Widget *wptr; int *r, *c; char **str;
{
  EZ_SSheetGetActiveCellWithProperty(wptr, r, c, str,NULL);
}
void EZ_SSheetGetActiveCellWithProperty(wptr, r, c, str, prop)
     EZ_Widget *wptr; int *r, *c; char **str; TextProp ***prop;
{
  if(wptr && EZ_WidgetType(wptr) == EZ_WIDGET_SPREAD_SHEET)
    {
      *c = (int)(EZ_SSheetCx(wptr));
      *r = (int)(EZ_SSheetCy(wptr));
      if(str)
	{
	  EZ_SSItem *jnk, *item = locateCreateSSItem(wptr, *r, *c, 0, &jnk);
	  char *p = NULL;
	  if(item)
	    {
	      ITextLine *tline = item->text;
	      if(tline->length>0) {p = tline->data; if(prop) *prop = tline->prop;}
	    }
	  *str = p;
	}
    }
}
/*******************************************************************/
void  EZ_SSheetDrawCell(wptr, row, col, drawable, x, y, w, h)
     EZ_Widget *wptr; Drawable drawable; int row, col, x, y, w, h;
{
  if(row >= 0 && row < EZ_SSheetNRows(wptr) && col >=0 && col < EZ_SSheetNColumns(wptr))
    {
      EZ_SSItem *jnk, *item = locateCreateSSItem(wptr, row, col, 0, &jnk);  
      unsigned long dfg = EZ_SSheetDFG(wptr);
      unsigned long bgpv, dbg = EZ_SSheetDBG(wptr);
      GC gc;
      if(EZ_SSheetTextBG(wptr) != ~0)
	{
	  gc = EZ_WRITABLEGC;
	  XSetForeground(EZ_Display, gc, EZ_SSheetTextBG(wptr));	
	}
      else
	{
	  if(EZ_GetBackgroundGC(wptr, &gc, &bgpv, 0, 0))
	    {
	      int ox, oy;
	      EZ_GetWidgetTileOrigin(wptr, &ox, &oy);      
	      XSetTSOrigin(EZ_Display, gc, ox, oy);
	    }
	}
      if(item)
	{
	  XFontStruct *defaultfont = EZ_WidgetFont(wptr);
	  int ascent, yy;
	  ITextLine *tline = item->text;	      
	  if(item->dirty)
	    {
	      int ttw, tth, tta;
	      EZ_ITextLineDimensionWork(defaultfont, 0, tline, 0, tline->length, &ttw, &tth, &tta);
	      tline->width = ttw;
	      tline->height= tth;
	      tline->y = tta;
	      item->dirty = 0;
	    }
	  ascent = tline->y;
	  yy = (h - tline->height)/2;
	  XFillRectangle(EZ_Display, drawable, gc, x,y,w,h);
	  EZ_ITextRenderTextWork(defaultfont, dfg, dbg, 0, 0, 0, tline, x, y+yy,
				 tline->height, ascent, drawable, 0, 0, 0); 
	}
      else  XFillRectangle(EZ_Display, drawable, gc, x,y,w,h);
    }
}
/*******************************************************************/
int EZ_SSheetGetHighlightedRegion(widget, r1, c1, r2, c2)
     EZ_Widget *widget; int *r1, *c1, *r2, *c2;
{
  if(widget && EZ_WidgetType(widget) == EZ_WIDGET_SPREAD_SHEET &&
     EZ_SSheetSelectionMode(widget))
    {
      int x1,y1,x2,y2,tmp;
      x1 = EZ_SSheetSx0(widget);
      y1 = EZ_SSheetSy0(widget);
      x2 = EZ_SSheetSx1(widget);
      y2 = EZ_SSheetSy1(widget);
      if(x2 < 0) { x2 = x1; y2 = y1;}
      if(x2 < x1) { tmp = x1; x1 = x2; x2 = tmp;}
      if(y2 < y1) { tmp = y1; y1 = y2; y2 = tmp;}
      *r1 = y1; *c1 = x1; *r2 = y2; *c2 = x2;
      return(1);
    }
  return(0);
}
/*******************************************************************/
char *EZ_SSheetGetRegionString(widget, r1, c1, r2, c2)
     EZ_Widget *widget; int r1,c1,r2,c2;
{
  return(EZ_SSheetGetRegionStringWithProperty(widget, r1, c1, r2, c2, NULL));
}
char *EZ_SSheetGetRegionStringWithProperty(widget, r1, c1, r2, c2, pret)
     EZ_Widget *widget; int r1,c1,r2,c2; TextProp ***pret;
{
  if(widget && EZ_WidgetType(widget) == EZ_WIDGET_SPREAD_SHEET)
    {
      int i, j, k, len, tmp, goodtab;
      char *str, tabchar, newlinechar;
      TextProp **prop = NULL;
      EZ_SSItem *jnk, *item;

      tabchar = EZ_SSheetTabChar(widget);
      newlinechar = EZ_SSheetNewlineChar(widget);
      goodtab = (tabchar==newlinechar)?0:1;
      if(r2 < r1) { tmp = r1; r2 = r2; r2 = r1;}
      if(c2 < c1) { tmp=c1; c1=c2;c2 = c1;}
      len = 20 * (r2-r1+1)*(c2-c1+1);
      str = (char *)my_malloc(len * sizeof(char), _SSHEET_DATA_);
      if(pret) prop = (TextProp **)my_malloc(len * sizeof(TextProp *), _SSHEET_DATA_);
      tmp = 0;

      for(i = r1; i <= r2; i++)
	for(j = c1; j <= c2; j++)
	  {
	    item = locateCreateSSItem(widget, i, j, 0, &jnk);  
	    if(item)
	      {
		ITextLine *tline = item->text;
		int ll = tline->length;
		char *s =tline->data;
		if(tmp + ll >= len)
		  {
		    len += len + ll;
		    str = (char *)my_realloc(str, len * sizeof(char),  _SSHEET_DATA_);
		    if(pret)
		      prop = (TextProp **)my_realloc(prop, len * sizeof(TextProp *), _SSHEET_DATA_);
		  }
		if(ll)
		  {
		    TextProp **ppp = tline->prop;
		    strcpy(str+tmp, s);
		    if(prop) { for(k = 0; k < ll; k++) prop[tmp+k] = ppp[k];}
		    tmp += ll;
		  }
	      }
	    if(prop) {prop[tmp] = prop[tmp+1]= NULL;}
	    str[tmp+1] = '\0';  
	    if(j < c2) {if(goodtab) str[tmp++] = tabchar; }
            else  str[tmp++] = newlinechar;
	  }
      if(pret) *pret = prop;
      return(str);
    }
  return(NULL);
}
/*******************************************************************/
static char *nextItem(str, row, tabc, newlc) 
     char *str; int *row; char tabc, newlc;
{
  char *p = str;
  if(p && *p)
    {
      while(*p && *p != tabc && *p != newlc) p++;
      if(*p == newlc) { *row = 1; return(p+1);}
      else if(*p == tabc) { *row = 0; return(p+1);}
      else if(*p == '\0' && p !=str ) {*row = 0; return(p);}
    }
  return(NULL);
}

void EZ_SSheetInsertRegionString(widget, r, c, str)
     EZ_Widget *widget; int r, c; char *str;
{
  EZ_SSheetInsertRegionStringWithProperty(widget, r, c, str, NULL);
}

void EZ_SSheetInsertRegionStringWithProperty(widget, r, c, str, prop)
     EZ_Widget *widget; int r, c; char *str; TextProp **prop;
{
  if(widget && EZ_WidgetType(widget) == EZ_WIDGET_SPREAD_SHEET)
    {
      int freezed = EZ_GetWidgetFreezedFlag(widget);
      char *p = str, *q, *qq;
      char *s, *t, tstr[1024];
      TextProp **tprop = prop;
      int row = r, col = c, mcol = c, radv;
      char tabchar = EZ_SSheetTabChar(widget);
      char newlinechar = EZ_SSheetNewlineChar(widget);

      if(freezed == 0) {EZ_SetWidgetFreezedFlag(widget);}
      while(p)
	{
	  q = nextItem(p, &radv, tabchar, newlinechar);
	  s=p;
	  if(q)
	    {
	      t = tstr; *t = '\0';
	      qq = q-1;
	      while(p < qq && (*p == ' ' || *p == '\t')) p++;
	      while(p < qq) *t++ = *p++; *t = '\0';
              while(t != tstr && (*t == ' ' || *t == '\t')) {*t= '\0'; t--;}
	      EZ_SSheetInsertItemWithProperty(widget, row, col, tstr, tprop);
	      if(radv) {row++; if(mcol < col) mcol = col; col = c;}
	      else  col++;
	    }
	  if(prop) {tprop += (int)(q-s);}
	  p = q;
	}
      EZ_SSheetValidateCharts(widget, r, c, row, mcol);
      if(freezed == 0)
        {
          EZ_ClearWidgetFreezedFlag(widget);
          if(EZ_WidgetMapped(widget)) EZ_DrawWidget(widget);
        }
    }
}
/*******************************************************************/
char **EZ_SSheetGetRegionItems(widget, r1,c1,r2,c2)
     EZ_Widget *widget; int r1, c1,r2,c2;
{
  return(EZ_SSheetGetRegionItemsWithProperty(widget, r1,c1,r2,c2, NULL));
}
char **EZ_SSheetGetRegionItemsWithProperty(widget, r1,c1,r2,c2, pret)
     EZ_Widget *widget; int r1, c1,r2,c2; TextProp ****pret;
{
  if(widget && EZ_WidgetType(widget) == EZ_WIDGET_SPREAD_SHEET)
    {
      int tmp, i, j, len;
      EZ_SSItem *jnk, *item;
      TextProp **pp, ***ppp, ***prop = NULL;
      char *str, **p, **ret;
      if(r2 < r1) { tmp = r1; r2 = r2; r2 = r1;}
      if(c2 < c1) { tmp=c1; c1=c2;c2 = c1;}
      len = (r2-r1+1)*(c2-c1+1);
      ret = (char **)my_malloc(len * sizeof(char *), _SSHEET_DATA_);
      memset(ret, 0, len *sizeof(char*));
      if(pret) 
	{
	  prop = (TextProp ***)my_malloc(len * sizeof(TextProp **), _SSHEET_DATA_);
	  memset(prop, 0, len *sizeof(TextProp ***));
	}
      p = ret;
      ppp = prop;
      for(i = r1; i <= r2; i++)
	for(j = c1; j <= c2; j++)
	  {
	    item = locateCreateSSItem(widget, i, j, 0, &jnk);  
	    str = NULL;
	    pp = NULL;
	    if(item)
	      {
		ITextLine *tline = item->text;
		int ll = tline->length;
		if(ll > 0) { str = tline->data; if(pret) pp = tline->prop;}
	      }
	    *p++ = str;
	    if(pret) *ppp++ = pp;
	  }
      if(pret) *pret = prop;
      return(ret);
    }
  return(NULL);
}
/*******************************************************************/
void EZ_SSheetInsertRegionItems(widget, r1,c1,r2,c2, items)
     EZ_Widget *widget; int r1,c1,r2,c2; char **items;
{
  EZ_SSheetInsertRegionItemsWithProperty(widget, r1,c1,r2,c2, items, NULL);
}
void EZ_SSheetInsertRegionItemsWithProperty(widget, r1,c1,r2,c2, items, prop)
     EZ_Widget *widget; int r1,c1,r2,c2; char **items; TextProp ***prop;
{
  if(widget && EZ_WidgetType(widget) == EZ_WIDGET_SPREAD_SHEET)
    {
      int freezed = EZ_GetWidgetFreezedFlag(widget);
      char **p = items;
      TextProp ***tprop = prop;
      int i,j, tmp;
      if(freezed == 0) {EZ_SetWidgetFreezedFlag(widget);}
      if(r2 < r1) { tmp = r1; r2 = r2; r2 = r1;}
      if(c2 < c1) { tmp=c1; c1=c2;c2 = c1;}

      for(i = r1; i <= r2; i++) for(j = c1; j <=c2; j++)
	{
	  TextProp **tp = NULL;
	  if(prop) tp = *tprop;
	  EZ_SSheetInsertItemWithProperty(widget, i, j, *p, tp);
	  p++;
	  if(prop) tprop++;
	}
      EZ_SSheetValidateCharts(widget, r1, c1, r2, c2);
      if(freezed == 0)
        {
          EZ_ClearWidgetFreezedFlag(widget);
          if(EZ_WidgetMapped(widget)) EZ_DrawWidget(widget);
        }
    }
}
/*******************************************************************/
static void copyToKillBuffer(widget) EZ_Widget *widget;
{
  int len, tmp;
  int x1 = EZ_SSheetSx0(widget);
  int y1 = EZ_SSheetSy0(widget);
  int x2 = EZ_SSheetSx1(widget);
  int y2 = EZ_SSheetSy1(widget);
  char *str;
  TextProp **pret;
  if(x2 < 0) { x2 = x1; y2 = y1;}
  if(x2 < x1) { tmp = x1; x1 = x2; x2 = tmp;}
  if(y2 < y1) { tmp = y1; y1 = y2; y2 = tmp;}
  str = EZ_SSheetGetRegionStringWithProperty(widget, y1, x1, y2, x2, &pret);
  if(str)
    {
      int i;
      len = strlen(str);
      EZ_GrowITextKillBuffer(&TextWidgetKillBuffer, len+1);
      for(i = 0; i < len; i++)
	{
	  (TextWidgetKillBuffer.data)[i] = str[i];
	  (TextWidgetKillBuffer.prop)[i] = pret[i];
	}
      (TextWidgetKillBuffer.data)[i] = '\0';
      TextWidgetKillBuffer.length = len;
      TextWidgetKillBuffer.width = 1;
      EZ_SetSelectionOwner(widget);
      (void) my_free(str);
      (void) my_free(pret);
    }
}
/*******************************************************************/
void  EZ_SSheetSetRegionJustification(widget, r1,c1,r2,c2, just)
     EZ_Widget *widget; int r1,c1,r2,c2, just; 
{
  if(widget && EZ_WidgetType(widget) == EZ_WIDGET_SPREAD_SHEET)
    {
      int tmp, i, j, freezed = EZ_GetWidgetFreezedFlag(widget);
      int selmode = EZ_SSheetSelectionMode(widget);
      if(freezed == 0) {EZ_SetWidgetFreezedFlag(widget);}
      if(r2 < r1) { tmp = r1; r2 = r2; r2 = r1;}
      if(c2 < c1) { tmp=c1; c1=c2;c2 = c1;}
      if(selmode == 2) /* columns */
        {
          EZ_SSItem *row = EZ_SSheetHeadRow(widget);
          for(i = c1; i <= c2; i++) row[i].just = just;
        }            
      for(i = r1; i <= r2; i++)
        for(j = c1; j <= c2; j++) 
          {
              EZ_SSItem *jnk, *item;
              item = locateCreateSSItem(widget, i, j, 0, &jnk); 
              if(item)
                {
                  item->just = just;
                  item->dirty = 1;
                }
          }
      if(freezed == 0) {EZ_ClearWidgetFreezedFlag(widget);}
      if(EZ_WidgetMapped(widget)) EZ_DrawWidget(widget);
    }
}      
/*******************************************************************/
void  EZ_SSheetSetRegionProperty(widget, r1,c1,r2,c2, prop)
     EZ_Widget *widget; int r1,c1,r2,c2; TextProp *prop;
{
  if(widget && EZ_WidgetType(widget) == EZ_WIDGET_SPREAD_SHEET)
    {
      int tmp, i, j, k, freezed = EZ_GetWidgetFreezedFlag(widget);
      if(freezed == 0) {EZ_SetWidgetFreezedFlag(widget);}
      if(r2 < r1) { tmp = r1; r2 = r2; r2 = r1;}
      if(c2 < c1) { tmp=c1; c1=c2;c2 = c1;}
      for(i = r1; i <= r2; i++) for(j = c1; j <= c2; j++) 
	{
	  EZ_SSItem *jnk, *item;
	  item = locateCreateSSItem(widget, i, j, 0, &jnk); 
	  if(item)
	    {
	      ITextLine *tline = item->text;
	      TextProp **tprop = tline->prop;
	      int len = tline->length;
	      for(k = 0; k < len;  k++) tprop[k] = prop;
	      item->dirty = 1;
	    }
	}
      if(freezed == 0) {EZ_ClearWidgetFreezedFlag(widget);}
      if(EZ_WidgetMapped(widget)) EZ_DrawWidget(widget);
    }
} 
/*******************************************************************/
void EZ_SSheetClearRegion(widget, r1,c1,r2,c2)
     EZ_Widget *widget; int r1,c1,r2,c2; 
{
  if(widget && EZ_WidgetType(widget) == EZ_WIDGET_SPREAD_SHEET)
    {
      int tmp, i, j, freezed = EZ_GetWidgetFreezedFlag(widget);
      int ncols = EZ_SSheetNColumns(widget);
      int nrows = EZ_SSheetNRows(widget);
      if(freezed == 0) {EZ_SetWidgetFreezedFlag(widget);}
      if(r2 < r1) { tmp = r1; r2 = r2; r2 = r1;}
      if(c2 < c1) { tmp=c1; c1=c2;c2 = c1;}
      if(r2 >= nrows) r2 = nrows -1; if(c2 >= ncols) c2 = ncols-1;
      for(i = r1; i <= r2; i++) for(j = c1; j <= c2; j++) 
	{
	  EZ_SSItem *jnk, *item;
	  item = locateCreateSSItem(widget, i, j, 0, &jnk); 
	  if(item)
	    {
	      ITextLine *tline = item->text;
	      tline->length = 0;
	      (tline->data)[0] = '\0';
	      item->dirty = 1;
	    }
	}
      EZ_SSheetValidateCharts(widget, r1, c1, r2, c2);
      if(freezed == 0) 
        {
          EZ_ClearWidgetFreezedFlag(widget);
          if(EZ_WidgetMapped(widget)) EZ_DrawWidget(widget);
        }
    }
}

void EZ_SSheetCopyRegion(widget, r1,c1,r2,c2, r0, c0, del )
     EZ_Widget *widget; int r1,c1,r2,c2, r0,c0, del;  
{
  if(widget && EZ_WidgetType(widget) == EZ_WIDGET_SPREAD_SHEET)
    {
      char *rstr; TextProp **tprop;
      int freezed = EZ_GetWidgetFreezedFlag(widget);
      if(freezed == 0) {EZ_SetWidgetFreezedFlag(widget);}
      rstr = EZ_SSheetGetRegionStringWithProperty(widget, r1, c1, r2, c2, &tprop);
      if(rstr)
        {
          if(del) EZ_SSheetClearRegion(widget, r1,c1,r2,c2);
          EZ_SSheetInsertRegionStringWithProperty(widget, r0, c0, rstr, tprop);
          (void) my_free(rstr);
          (void) my_free(tprop);          
        }
      if(del) EZ_SSheetValidateCharts(widget, r1, c1, r2, c2);
      EZ_SSheetValidateCharts(widget, r0, c0, r0+r2-r1, c0+c2-c1);
      if(freezed == 0) 
        {
          EZ_ClearWidgetFreezedFlag(widget);
          if(EZ_WidgetMapped(widget)) EZ_DrawWidget(widget);
        }
    }
}
/*******************************************************************/

EZ_Item *EZ_SSheetMakeChart(widget, r1, c1, r2, c2, chandle, kind, vdata, cmode)
     EZ_Widget *widget;  int r1, c1, r2,  c2, kind, cmode; 
     EZ_SChartHandle chandle; void *vdata;
{
  if(widget && EZ_WidgetType(widget) == EZ_WIDGET_SPREAD_SHEET)
    {
      EZ_SSItem *row0 = EZ_SSheetHeadRow(widget);
      EZ_SSItem *data = EZ_SSheetData(widget);
      int i, tmp, ncharts = EZ_SSheetNCharts(widget);
      int acharts = EZ_SSheetACharts(widget);
      EZ_Item *item, **charts = EZ_SSheetCharts(widget);
      EZ_SSChartJunk *sjunk;

      for(i=0; i< ncharts; i++)
        {
          item=charts[i];
          if(EZ_FigItemR1(item) == r1 && EZ_FigItemC1(item) == c1 &&
             EZ_FigItemR2(item) == r2 && EZ_FigItemC2(item) == c2 &&
             EZ_FigItemSKind(item) == kind && EZ_FigItemSCHandle(item) == chandle &&
             EZ_FigItemSCData(item) == vdata)
            return(NULL);
        }
      sjunk = (EZ_SSChartJunk *)my_malloc(sizeof(EZ_SSChartJunk), _SSHEET_DATA_);
      item = EZ_CreateFigItem();
      EZ_ItemBorderWidth(item) = 1;
      EZ_ItemBorderType(item) = EZ_BORDER_FLAT;
      EZ_ItemBackground(item) = EZ_SSheetTextBG(widget);
      EZ_GetAndUseItemResources(item);
      memset(sjunk, 0, sizeof(EZ_SSChartJunk));
      EZ_FigItemSSChartJunk(item) = sjunk;
      if(ncharts+1 >= acharts) 
        {
          acharts += 10;
          if(charts) charts = (EZ_Item **)my_realloc(charts, acharts * sizeof(EZ_Item *), _SSHEET_DATA_);
          else  charts = (EZ_Item **)my_malloc(acharts * sizeof(EZ_Item *), _SSHEET_DATA_);
          EZ_SSheetCharts(widget) = charts;
          EZ_SSheetACharts(widget) = acharts;
        }
      charts[ncharts++] = item;
      
      if(r2<r1){ tmp=r1; r1=r2; r2=tmp;}
      if(c2<c1){ tmp=c1; c1=c2; c2=tmp;}

      EZ_FigItemR1(item) = r1;
      EZ_FigItemC1(item) = c1;
      EZ_FigItemR2(item) = r2;
      EZ_FigItemC2(item) = c2;
      EZ_FigItemSCHandle(item) = chandle;
      EZ_FigItemSCData(item) = vdata;
      EZ_FigItemSKind(item) = kind;

      if(cmode) EZ_FigItemSy(item) = -1.0;

      tmp =  row0[c1].colOrHeight - EZ_SSheetTx(widget);
      EZ_ItemAOriginX(item) = tmp;
      tmp =  data[r1].rowOrWidth - EZ_SSheetTy(widget);      
      EZ_ItemAOriginY(item) = tmp;

      EZ_SSheetDirty(widget) = 1;  
      if(chandle) (chandle)(widget, item, r1,c1,r2,c2,kind, vdata);
      EZ_SSheetNCharts(widget) += 1;
      if(EZ_WidgetMapped(widget)) EZ_DrawWidget(widget);
      return(item);
    }
  return(NULL);
}
/*******************************************************************/ 
int EZ_SSheetRedrawCharts(wptr, drawable) EZ_Widget *wptr; Drawable drawable;
{
  int i, x, y, hmode, ncharts = EZ_SSheetNCharts(wptr);
  int w, h, tw, th, bwpad, Rx, Ry, Rw, Rh, tmpw, tmph, ret = 0;
  EZ_Item *chart, **charts = EZ_SSheetCharts(wptr);

  if(ncharts == 0) return(0);

  tw = EZ_SSheetTWidth(wptr);
  th = EZ_SSheetTHeight(wptr);
  bwpad = EZ_SSheetBwPad(wptr);
  w = EZ_WidgetWidth(wptr);
  h = EZ_WidgetHeight(wptr);

  Rx = bwpad + tw;
  Ry = bwpad + th;

  x = EZ_SSheetTx(wptr) + Rx;
  y = EZ_SSheetTy(wptr) + Ry;

  Rw = w -(bwpad + Rx);
  Rh = h -(bwpad + Ry);

  /* compute the geomtry of charts */
  for(i = 0; i < ncharts; i++)
    {
      chart = charts[i];
      if(EZ_LookupItemFromHT(chart))
        {
          EZ_ComputeItemSize(chart, &tmph, &tmpw);
        }
    }
  for(i = 0; i < ncharts; i++)
    {
      chart = charts[i];
      if(EZ_LookupItemFromHT(chart))
        {
          int x0 = x+EZ_ItemAOriginX(chart);
          int y0 = y+EZ_ItemAOriginY(chart);
          int x1 = x0 + EZ_ItemWidth(chart);
          int y1 = y0 + EZ_ItemHeight(chart);
          int tmp;
          if(x1 < 0 || x0 > w || y1 < 0 || y0 > h) tmp = 0;
          else if( x0 > bwpad && x1 < w - bwpad && y0 > bwpad && y1 < h-bwpad) tmp = 0;
          else tmp = 1;
          if((x0 < Rx && x1 > Rx)  ||  (y0 < Ry && y1 > bwpad)) ret |= 65536;
          ret += tmp;
          EZ_DisplayOneItem(wptr, drawable, x0, y0, chart, Rx, Ry, Rw,Rh);
        }
    }
  /* highlight selection */
  hmode = EZ_SSheetHMode(wptr);
  chart = EZ_SSheetSChart(wptr);
  if(EZ_LookupItemFromHT(chart))
    { 
      unsigned long dbg = EZ_SSheetDBG(wptr);
      unsigned long dfg = EZ_SSheetDFG(wptr);
      EZ_HighlightOneItem(wptr, drawable, chart, dfg, dbg, Rx,Ry,Rw,Rh, hmode);
    }
  return(ret);
}
/*****************************************************************************/
static void drawHead(wptr, pixmap) EZ_Widget *wptr; Drawable pixmap;
{
  /* render the head row and column */
  if(EZ_SSheetShowHead(wptr)) /* display head row */
    {
      EZ_SSItem *row = EZ_SSheetHeadRow(wptr);
      EZ_SSItem *data =EZ_SSheetData(wptr);
      int sx0=EZ_SSheetCx(wptr); int sy0=EZ_SSheetCy(wptr);
      int bd, th, tw, w, h, tmp, bwpad, x0, x1, y0, y1, x, y, i; 
      unsigned long fg, bg, bgpv;
      unsigned long dfg = EZ_SSheetDFG(wptr);
      unsigned long dbg = EZ_SSheetDBG(wptr);
      unsigned long hfg = EZ_SSheetSelectionFG(wptr);
      unsigned long hbg = EZ_SSheetSelectionBG(wptr);
      XFontStruct     *defaultfont = EZ_WidgetFont(wptr);
      GC gc;

      x0 = EZ_SSheetX0(wptr);
      x1 = EZ_SSheetX1(wptr);
      y0 = EZ_SSheetY0(wptr);
      y1 = EZ_SSheetY1(wptr);

      th = EZ_SSheetTHeight(wptr);
      tw = EZ_SSheetTWidth(wptr);
      w = EZ_WidgetWidth(wptr);
      h = EZ_WidgetHeight(wptr);
      bwpad = EZ_SSheetBwPad(wptr);
      tmp = bwpad + bwpad;
      /* render head row/column in a darker background */
      if(EZ_WidgetBGPixmapB(wptr) != NULL)
	{
	  EZ_Bitmap *bitmap = EZ_WidgetBGPixmapB(wptr);
	  gc = EZ_NTILEGC2;
	  XSetTile(EZ_Display,EZ_NTILEGC2,bitmap->pixmap);
	  XSetTSOrigin(EZ_Display, EZ_NTILEGC2, 0, 0);
	}
      else if(EZ_WidgetParentBGPixmapB(wptr) != NULL)
	{
	  EZ_Bitmap *bitmap = EZ_WidgetParentBGPixmapB(wptr);
	  gc = EZ_NTILEGC2;
	  XSetTile(EZ_Display,EZ_NTILEGC2,bitmap->pixmap);
	  XSetTSOrigin(EZ_Display, EZ_NTILEGC2, 0, 0);
	}
      else
	{
	  int tile = EZ_GetBackgroundGC(wptr,&gc, &bgpv, 0, 0);
	  if(tile) 
	    {
	      int ox, oy;
	      EZ_GetWidgetTileOrigin(wptr, &ox, &oy);      
	      XSetTSOrigin(EZ_Display, gc, ox, oy);
	    }
	}  
      XFillRectangle(EZ_Display, pixmap, gc, bwpad, bwpad, w-tmp,  th);
      XFillRectangle(EZ_Display, pixmap, gc, bwpad, th+bwpad, tw, h-(th+tmp));
      
      /* the UL corner cell */
      if(th && tw)
	EZ_DrawRectBorderWithSize(wptr, pixmap, bwpad,bwpad,tw+1, th, 1, EZ_BORDER_UP, 0,0,1);
      
      /* render the labels */
      if(th > 0)
	{
	  x = bwpad + tw; y = bwpad;
	  for(i = x0; i<= x1; i++) /* head row */
	    {
              if(row[i].rowOrWidth > 1)
                {
                  int maxh, ascent, xx, yy;
                  ITextLine *tline = row[i].text;
                  if(row[i].dirty)
                    {
                      int tth, ttw, tta;
                      EZ_ITextLineDimensionWork(defaultfont, 0, tline, 0, tline->length, &ttw, &tth, &tta);
                      tline->width = ttw;
                      tline->height= tth;
                      tline->y = tta;
                      row[i].dirty = 0;
                    }
                  if(i == sx0) { bd = EZ_BORDER_DOWN; fg = hfg; bg = hbg; }
                  else { bd = EZ_BORDER_UP; fg = dfg; bg = dbg;}
                  EZ_DrawRectBorderWithSize(wptr, pixmap, x+1,y,row[i].rowOrWidth, th, 1, bd, 0,0,1);
                  ascent = tline->y;
                  maxh = tline->height;
                  xx = (row[i].rowOrWidth - tline->width)/2;
                  yy = (th - tline->height)/2;
                  EZ_ITextRenderTextWork(defaultfont, fg, bg, 0, 0, 0,
                                         tline, x+xx, y+yy, maxh, ascent, pixmap, 0, 0, 0);
                }
              x += row[i].rowOrWidth;
            }
	}
      if(tw > 0)
	{
	  x = bwpad; y = bwpad + th;
	  for(i = y0; i<= y1; i++) /* head column */
	    {
              if(data[i].colOrHeight > 1)
                {
                  int maxh, ascent, xx, yy;
                  ITextLine *tline = data[i].text;
                  if(data[i].dirty)
                    {
                      int tth, ttw, tta;
                      EZ_ITextLineDimensionWork(defaultfont, 0, tline, 0, tline->length, &ttw, &tth, &tta);
                      tline->width = ttw;
                      tline->height= tth;
                      tline->y = tta;
                      data[i].dirty = 0;
                    }
                  if(i == sy0) { bd = EZ_BORDER_DOWN; fg = hfg; bg = hbg; }
                  else { bd = EZ_BORDER_UP; fg = dfg; bg = dbg;}
                  EZ_DrawRectBorderWithSize(wptr, pixmap, x,y,tw, data[i].colOrHeight, 1, bd, 0,0,1);
                  ascent = tline->y;
                  maxh = tline->height;
                  xx = (tw - tline->width)/2;
                  yy = (data[i].colOrHeight - tline->height)/2;
                  EZ_ITextRenderTextWork(defaultfont, fg, bg, 0, 0, 0,
                                         tline, x+xx, y+yy, maxh, ascent, pixmap, 0, 0, 0);
                }
              y += data[i].colOrHeight;
            }
	}
    }
}
static void highlightHead(wptr, drawable, ox, oy, flag) 
     EZ_Widget *wptr; Drawable drawable; int ox, oy, flag;
{
  if(EZ_SSheetShowHead(wptr)) /* display head row and column */
    {
      int cx = EZ_SSheetCx(wptr);
      int cy = EZ_SSheetCy(wptr);
      if(flag == 0 && cx == ox && cy == oy) return;
      else
        {
          int i, j, x, y;
          unsigned long dfg =0, dbg=0;
          int tw = EZ_SSheetTWidth(wptr);
          int th = EZ_SSheetTHeight(wptr);
          int bwpad = EZ_SSheetBwPad(wptr);
          EZ_SSItem *row = EZ_SSheetHeadRow(wptr);
          EZ_SSItem *data = EZ_SSheetData(wptr);
          XFontStruct  *defaultfont = EZ_WidgetFont(wptr);
	  GC gc;
	  unsigned long bgpv;

	  /* render head row/column in a darker background */
	  if(EZ_WidgetBGPixmapB(wptr) != NULL)
	    {
	      EZ_Bitmap *bitmap = EZ_WidgetBGPixmapB(wptr);
	      gc = EZ_NTILEGC2;
	      XSetTile(EZ_Display,EZ_NTILEGC2,bitmap->pixmap);
	      XSetTSOrigin(EZ_Display, EZ_NTILEGC2, 0, 0);
	    }
	  else if(EZ_WidgetParentBGPixmapB(wptr) != NULL)
	    {
	      EZ_Bitmap *bitmap = EZ_WidgetParentBGPixmapB(wptr);
	      gc = EZ_NTILEGC2;
	      XSetTile(EZ_Display,EZ_NTILEGC2,bitmap->pixmap);
	      XSetTSOrigin(EZ_Display, EZ_NTILEGC2, 0, 0);
	    }
	  else
	    {
	      int tile = EZ_GetBackgroundGC(wptr,&gc, &bgpv, 0, 0);
	      if(tile) 
		{
		  int tx, ty;
		  EZ_GetWidgetTileOrigin(wptr, &tx, &ty);      
		  XSetTSOrigin(EZ_Display, gc, tx, ty);
		}
	    }  

	  if(th > 0 && (flag || ox != cx))
            {
              y = bwpad;
              for(j = 0, i = ox; j < 2; j++, i = cx)
                {
                  int maxh, ascent, xx, yy;
                  int bd=0;
                  if(i == ox)
                    {
                      dfg = EZ_SSheetDFG(wptr); dbg = EZ_SSheetDBG(wptr);
                      bd = EZ_BORDER_UP;
                    }
                  if(i == cx)
                    {
                      dfg = EZ_SSheetSelectionFG(wptr);
                      dbg = EZ_SSheetSelectionBG(wptr);
                      bd = EZ_BORDER_DOWN; 
                    }
                  if(row[i].rowOrWidth > 1)
                    {
                      ITextLine *tline = row[i].text;
                      x =  row[i].colOrHeight;
                      if(row[i].dirty)
                        {
                          int tth, ttw, tta;
                          EZ_ITextLineDimensionWork(defaultfont, 0, tline, 0, tline->length, 
                                                    &ttw, &tth, &tta);
                          tline->width = ttw;
                          tline->height= tth;
                          tline->y = tta;
                          row[i].dirty = 0;
                        }
                      XFillRectangle(EZ_Display, drawable, gc, x+1,y, row[i].rowOrWidth, th);
                      EZ_DrawRectBorderWithSize(wptr, drawable, x+1,y,row[i].rowOrWidth, th, 1, bd,
                                                0,0,1);
                      ascent = tline->y;
                      maxh = tline->height;
                      xx = (row[i].rowOrWidth - tline->width)/2;
                      yy = (th - tline->height)/2;
                      EZ_ITextRenderTextWork(defaultfont, dfg, dbg, 0, 0, 0,
                                             tline, x+xx, y+yy, maxh, ascent, drawable, 0, 0, 0);
                    }
                }
            }
          if(tw > 0 && (flag || oy != cy))
            {
              x = bwpad;
              for(j=0, i=oy; j < 2; j++, i=cy) /* head column */
                {
                  int maxh, ascent, xx, yy;
                  int bd=0;
                  if(i == oy)
                    {
                      dfg = EZ_SSheetDFG(wptr); dbg = EZ_SSheetDBG(wptr);
                      bd = EZ_BORDER_UP; 
                    }
                  if( i == cy)
                    {
                      dfg = EZ_SSheetSelectionFG(wptr);
                      dbg = EZ_SSheetSelectionBG(wptr);
                      bd = EZ_BORDER_DOWN;
                    }
                  if(data[i].colOrHeight>1)
                    {
                      ITextLine *tline = data[i].text;
                      y = data[i].rowOrWidth;
                      if(data[i].dirty)
                        {
                          int tth, ttw, tta;
                          EZ_ITextLineDimensionWork(defaultfont, 0, tline, 0, tline->length, &ttw, &tth, &tta);
                          tline->width = ttw;
                          tline->height= tth;
                          tline->y = tta;
                          data[i].dirty = 0;
                        }
                      XFillRectangle(EZ_Display, drawable, gc, x,y, tw, data[i].colOrHeight);
                      EZ_DrawRectBorderWithSize(wptr, drawable, x,y,tw, data[i].colOrHeight, 1, bd,
                                                0,0,1);
                      ascent = tline->y;
                      maxh = tline->height;
                      xx = (tw - tline->width)/2;
                      yy = (data[i].colOrHeight - tline->height)/2;
                      EZ_ITextRenderTextWork(defaultfont, dfg, dbg, 0, 0, 0,
                                             tline, x+xx, y+yy, maxh, ascent, drawable, 0, 0, 0);
                    }
                }
            }
        }
    }
}
/*********************************************************************************************/
static void drawOutline(widget, win, mode, x0, y0, x1, y1)
     EZ_Widget *widget; Drawable win; int mode, x0, x1, y0, y1;
{
  EZ_SSItem *row = EZ_SSheetHeadRow(widget);
  EZ_SSItem *data = EZ_SSheetData(widget);
  int x,y,xx,yy, bwpad;
  switch(mode)
    {
    case 1: /* rows */
      bwpad = EZ_SSheetBwPad(widget);
      x = bwpad + EZ_SSheetTWidth(widget);
      xx = EZ_WidgetWidth(widget) - bwpad;
      y = data[y0].rowOrWidth;
      yy = data[y1].rowOrWidth + data[y1].colOrHeight;
      XDrawLine(EZ_Display, win, EZ_DNDXORGC, x, y, xx, y);
      XDrawLine(EZ_Display, win, EZ_DNDXORGC, x, yy, xx, yy);
      break;
    case 2:
      bwpad = EZ_SSheetBwPad(widget);
      y = bwpad + EZ_SSheetTHeight(widget);
      yy = EZ_WidgetHeight(widget) - bwpad;
      x = row[x0].colOrHeight;
      xx = row[x1].colOrHeight + row[x1].rowOrWidth;
      XDrawLine(EZ_Display, win, EZ_DNDXORGC, x, y, x, yy);
      XDrawLine(EZ_Display, win, EZ_DNDXORGC, xx, y, xx, yy);
      break;      
    case 3:
      x = row[x0].colOrHeight;
      xx =row[x1].colOrHeight - x +  row[x1].rowOrWidth;
      y = data[y0].rowOrWidth;
      yy= data[y1].rowOrWidth -y + data[y1].colOrHeight;
      XDrawRectangle(EZ_Display, win, EZ_DNDXORGC, x, y, xx, yy);
      break;
    default:
      break;
    }
}
/*********************************************************************************************/
void EZ_DoScaleItem(fitem, corner, x, y, x0, y0, w, h)
     EZ_Item *fitem; int corner;
     int x0, y0, w, h; /* the current geometry of item */
     int x, y;         /* pointer coor  */
     /* coor is measured from the UL corner of the ambient window */
{
  int     x1,y1,x2,y2;
  int     dx,dy, rootx, rooty, oldx, oldy;
  int     dflag = 0;
  int     rx, ry,rw,rh, rx0, ry0;
  XEvent  xevent;
  Window  root, child, win;
  unsigned int    mask;
  GC       gc = EZ_DNDXORGC;

  win = RootWindow(EZ_Display, EZ_ScreenNum);
  XQueryPointer(EZ_Display, RootWindow(EZ_Display, EZ_ScreenNum),
                &root,       /* root return win */
                &child,      /* child ret win   */
                &rootx, &rooty,   /* x, y in root    */
                &rx,&ry,            /* x, y in win     */
                &mask);  
  oldx = rootx; oldy = rooty;
  x0 = rootx+x0-x; y0 = rooty +y0-y;
  switch(corner)
    {
    case XC_top_left_corner:
      x1 = x0; y1 = y0; 
      x2 = x0 +w; y2 = y0 +h;
      break;
    case XC_top_right_corner:
      x1 = x0 +w; y1 = y0;
      x2 = x0; y2 = y0 +h;
      break;
    case XC_bottom_right_corner:
      x2 = x0; y2 = y0; 
      x1 = x0 +w; y1 = y0 +h;
      break;
    case XC_bottom_left_corner:
      x2 = x0 +w; y2 = y0;
      x1 = x0; y1 = y0 +h;			  
      break;
    default:
      { x1=x2=y1=y2 = 0;}
      break;
    }

  rw = x1 - x2; if(rw<0) rw = -rw;
  rh = y1 - y2; if(rh<0) rh = -rh;
  rx0=rx = x2 < x1 ? x2: x1;
  ry0=ry = y2 < y1 ? y2: y1;
  XDrawRectangle(EZ_Display, win, gc, rx,ry,rw, rh);
  dflag = 1;
  while(1)
    {
      XNextEvent(EZ_Display, &xevent); 
      EZ_FilterEvent(&xevent);
      EZ_InvokePrivateEventHandler(&xevent);
      if(xevent.type == Expose) EZ_WidgetDispatchEvent(&xevent);
      if(xevent.type == MotionNotify)
        {
          if(dflag) 
            {
              XDrawRectangle(EZ_Display, win, gc, rx,ry,rw, rh);
              dflag = 0;
            }
          while (XCheckTypedEvent(EZ_Display, MotionNotify, &xevent)) EZ_FilterEvent(&xevent);
          XQueryPointer(EZ_Display, RootWindow(EZ_Display, EZ_ScreenNum),
                        &root,       /* root return win */
                        &child,      /* child ret win   */
                        &rootx, &rooty,   /* x, y in root    */
                        &x,&y,            /* x, y in win     */
                        &mask);  
          dx = rootx - oldx;  dy = rooty - oldy;
          oldx = rootx; oldy = rooty;

          x1 += dx; y1 += dy;
          rw = x1 - x2; if(rw<0) rw = -rw;
          rh = y1 - y2; if(rh<0) rh = -rh;
          rx = x2 < x1 ? x2: x1;
          ry = y2 < y1 ? y2: y1;
          XDrawRectangle(EZ_Display, win, gc, rx,ry,rw, rh);
          dflag = 1;
        }
      else if(xevent.type == ButtonRelease)
        {
          if(dflag) XDrawRectangle(EZ_Display, win, gc, rx,ry,rw, rh);
          rw = (rw > 0 ? rw : 10);
          rh = (rh > 0 ? rh : 10);
          break;
        }
    }
  /* scale item */
  if(rx0 != rx || ry0 != ry || rw != w || rh != h)
    {
      int   type = EZ_ItemType(fitem);
      
      EZ_ItemAOriginX(fitem) += rx-rx0;
      EZ_ItemAOriginY(fitem) += ry-ry0;
      if(type == EZ_FIG_ITEM)
        {
          float sxf, syf;
          sxf = (float)(rw)/(float)w;
          syf = (float)(rh)/(float)h;
          EZ_ScaleFigItem(fitem, sxf, syf, 1);
          EZ_ItemDirty(fitem) = 1;
        }
      else if(type == EZ_LABEL_ITEM)
        {
          EZ_ItemAWidth(fitem) = rw;
          EZ_ItemAHeight(fitem) = rh;
        }
      else
        {
	  int tw, th, itmpX, itmpY;
	  EZ_Widget *widget = EZ_WidgetItemWidget(fitem);
	  itmpX = (int)EZ_ItemBorderWidth(fitem);
	  itmpY = itmpX + (int) EZ_ItemPadY(fitem) + 2;
	  itmpX = itmpX + (int) EZ_ItemPadX(fitem) + 2;
	  tw =  rw - (itmpX << 1);
	  th =  rh - (itmpY << 1);
          EZ_ItemAWidth(fitem) = rw;
          EZ_ItemAHeight(fitem) = rh;
          EZ_ItemWidth(fitem) = rw;
          EZ_ItemHeight(fitem) = rh;
          EZ_ResizeWidgetWindow(widget, tw,th);
        }
    }
}
/*************************************************************************************************/
static int moveBoundry(widget, x, y, flg)
     EZ_Widget *widget; int x, y; int flg; /* motion notify */
{
  int ncharts, smode;
  int bwpad, tw, th, w, h;
  bwpad = EZ_SSheetBwPad(widget);
  w = EZ_WidgetWidth(widget);
  h = EZ_WidgetHeight(widget);
  tw =EZ_SSheetTWidth(widget);
  th = EZ_SSheetTHeight(widget);  

  if(x <= bwpad+tw || x >= w-bwpad || y <= bwpad+th || y >= h-bwpad) return(0);

  ncharts = EZ_SSheetNCharts(widget);
  if(ncharts > 0)
    {
      EZ_Item **charts = EZ_SSheetCharts(widget), *chart, *find=NULL;
      int bwpad = EZ_SSheetBwPad(widget);
      int tx = EZ_SSheetTx(widget) + EZ_SSheetTWidth(widget)+ bwpad;
      int ty = EZ_SSheetTy(widget) + EZ_SSheetTHeight(widget)+bwpad;
      int corner=0, i, x0=0,y0=0, w=0, h=0;
      for(i=ncharts-1; i>=0; i--)
        {
          chart = charts[i];
          x0 = tx + EZ_ItemAOriginX(chart);
          y0 = ty + EZ_ItemAOriginY(chart);
          w = EZ_ItemWidth(chart);
          h = EZ_ItemHeight(chart);
          corner = EZ_XYIsAtCorner(x-x0,y-y0,w,h);
          if(corner) {find = chart; break;}
          else if(flg) { if(x > x0 && y > y0 && x<x0+w && y<y0+h) return(XC_fleur);}
        }
      if(flg && corner) return(corner);
      if(find)
        {
          EZ_DoScaleItem(find, corner,x,y,x0,y0,w,h);
          EZ_DrawWidget(widget);
          return(1);
        }
    }
  
  smode = EZ_SSheetSelectionMode(widget);
  if(smode != 0 && smode < 4) /* try move region */
    {
      int cx, cy;
      int ofind = EZ_SSheetFindCursorCell(widget, x, y, &cy, &cx);
      if(ofind)
        {
          int X0 =  EZ_SSheetX0(widget);
          int Y0 =  EZ_SSheetY0(widget);
          int xx0 = EZ_SSheetSx0(widget);
          int yy0 = EZ_SSheetSy0(widget);      
          int xx1 = EZ_SSheetSx1(widget);
          int yy1 = EZ_SSheetSy1(widget);      
          int onbdy, tmp, t1, t2, t3, t4;
          EZ_SSItem *row0, *data;

          if(xx1 < xx0){ tmp=xx0; xx0=xx1; xx1=tmp;}
          if(yy1 < yy0){ tmp=yy0; yy0=yy1; yy1=tmp;}
          
          row0 = EZ_SSheetHeadRow(widget);
          data = EZ_SSheetData(widget);
          t1 = row0[xx0].colOrHeight -x;
          t3 = row0[xx1].colOrHeight + row0[xx1].rowOrWidth -x;
          t2 = data[yy0].rowOrWidth - y;
          t4 = data[yy1].rowOrWidth + data[yy1].colOrHeight - y;
          onbdy = 0;
          switch(smode)
            {
            case 1: /* rows were highlighted */
              onbdy = (ABSV(t2)<= 3) | (ABSV(t4)<= 3);
              break;
            case 2: /* columns were highlighted */
              onbdy = (ABSV(t1)<= 3) | (ABSV(t3)<= 3);
              break;
            case 3: /* a rectangle is selected */
              onbdy = (((ABSV(t2)<= 3) | (ABSV(t4)<= 3)) && (t1 <=0 && t3>=0 ) ) ||
                (((ABSV(t1)<= 3) | (ABSV(t3)<= 3)) && (t2<=0 && t4 >=0));
              break;
            default:
              break;
            }
          if(flg && onbdy) return(XC_left_ptr);
          if(onbdy)
            {
              XEvent xevent;
              Window win = EZ_WidgetWindow(widget);
              int done = 0, ncx, ncy, find, x0=xx0, y0=yy0, x1=xx1, y1=yy1;
              drawOutline(widget, win, smode, x0, y0, x1, y1);
              
              while(!done)
                {
                  XNextEvent(EZ_Display, &xevent);
                  EZ_FilterEvent(&xevent);
                  EZ_InvokePrivateEventHandler(&xevent);
                  if(xevent.type == MotionNotify)
                    {
                      x=xevent.xmotion.x;  y=xevent.xmotion.y;
                      if(x <= bwpad+tw || x >= w-bwpad || y <= bwpad+th || y >= h-bwpad) continue;
                      find = EZ_SSheetFindCursorCell(widget, x, y, &ncy, &ncx);
                      if(find)
                        {
                          int dx = ncx -cx, dy = ncy -cy;
                          if((smode ==1 && dy == 0) || (smode ==2 && dx == 0)) continue;
                          else if((dx | dy) && (x0+dx) >= X0 && (y0+dy)>= Y0)
                            {
                              cx = ncx; cy = ncy;
                              drawOutline(widget, win, smode, x0, y0, x1, y1);
                              x0 += dx; x1 += dx; y0 += dy; y1 += dy;
                              drawOutline(widget, win, smode, x0, y0, x1, y1);
                            }
                        }
                      XSync(EZ_Display, 0);
                      while(XCheckTypedEvent(EZ_Display, MotionNotify, &xevent));
                    }
                  else if(xevent.type == ButtonRelease)
                    {
                      drawOutline(widget, win, smode, x0, y0, x1, y1);
                      done = 1;
                    }
                  else if(xevent.type == Expose || xevent.type == FocusIn || xevent.type == FocusOut)
                    EZ_WidgetDispatchEvent(&xevent); 
                }
              if(x0 != xx0 || y0 != yy0 )
                {
                  if(smode == 1) /* rows are selected */
                    {
                      EZ_SSheetSy0(widget) = y0; EZ_SSheetSy1(widget) = y1; 
                      EZ_SSheetCy(widget) += y0-yy0;
                      x0=xx0 = 0; 
                    }   
                  else if(smode == 2) /* columna are selected */
                    {
                      EZ_SSheetSx0(widget) = x0;  EZ_SSheetSx1(widget) = x1; 
                      EZ_SSheetCx(widget) += x0-xx0;
                      y0=yy0 = 0;
                    }
                  else
                    {
                      EZ_SSheetCx(widget) += x0-xx0;  EZ_SSheetCy(widget) += y0-yy0;
                      EZ_SSheetSx0(widget) = x0;  EZ_SSheetSy0(widget) = y0; 
                      EZ_SSheetSx1(widget) = x1; EZ_SSheetSy1(widget) = y1; 
                    }
                  EZ_SSheetCopyRegion(widget, yy0, xx0, yy1, xx1, y0, x0, 1);
                }
              return(1);
            }
	}
    }
  return(0);
}
/*******************************************************************/
void EZ_SSheetSetTabChar(widget, tab, newline)
     EZ_Widget *widget; char tab,newline;
{
  if(widget && EZ_WidgetType(widget) == EZ_WIDGET_SPREAD_SHEET)
    {
      EZ_SSheetTabChar(widget) = tab;
      EZ_SSheetNewlineChar(widget) = newline;      
    }
}
/*******************************************************************/
void EZ_SSheetGetSizes(widget, nrow_r, ncol_r)
     EZ_Widget *widget; int *nrow_r, *ncol_r;
{
  if(widget && EZ_WidgetType(widget) == EZ_WIDGET_SPREAD_SHEET)
    {
      EZ_SSItem *data = EZ_SSheetData(widget);
      int i, nrows, cols=0, rows;
      nrows = (int)EZ_SSheetNRows(widget);
      for(rows = nrows; rows >=0;  rows--) {if(data[rows].next != NULL) break;}
      for(i=0; i<= rows; i++)
        { 
          EZ_SSItem *item=data[i].next;
          while(item && item->next) item=item->next; 
          if(item && item->colOrHeight > cols) cols = item->colOrHeight;
        }
      *nrow_r = rows+1;
      *ncol_r = cols+1;
    }
}

void EZ_SSheetSetFirstCell(widget, row, col)
     EZ_Widget *widget; int row, col;
{
  if(widget && EZ_WidgetType(widget) == EZ_WIDGET_SPREAD_SHEET)
    {
      int nrow = (int)EZ_SSheetNRows(widget);
      int ncol = (int)EZ_SSheetNColumns(widget);
      if(row >= 0 && row < nrow && col >=0 && col < ncol)
        {
          EZ_SSheetDirty(widget) = 1;
          EZ_SSheetX0(widget) = col;
          EZ_SSheetY0(widget) = row;
          EZ_SSheetCx(widget) = col;
          EZ_SSheetCy(widget) = row;
          if(EZ_WidgetMapped(widget)) EZ_DrawWidget(widget);
        }
    }
}

void EZ_SSheetSetHScrollbar(widget, hscroll)
     EZ_Widget *widget, *hscroll;
{
  if(widget && EZ_WidgetType(widget) == EZ_WIDGET_SPREAD_SHEET)
    {
      EZ_SSheetHScroll(widget) = hscroll;
      if(hscroll) EZ_ScrollObject(hscroll) = widget;
    }
}
void EZ_SSheetSetVScrollbar(widget, vscroll)
     EZ_Widget *widget, *vscroll;
{
  if(widget && EZ_WidgetType(widget) == EZ_WIDGET_SPREAD_SHEET)
    {
      EZ_SSheetVScroll(widget) = vscroll;
      if(vscroll) EZ_ScrollObject(vscroll) = widget;
    }
}
/*******************************************************************/
void EZ_SSheetValidateCharts(wptr, r1,c1,r2,c2)
     EZ_Widget *wptr; int r1, c1, r2, c2;
{
  int i,dirty=0, ncharts = EZ_SSheetNCharts(wptr);
  EZ_Item  *item, **charts = EZ_SSheetCharts(wptr);

  for(i = 0; i < ncharts; i++)
    {
      item = charts[i];
      if(EZ_ItemType(item) == EZ_FIG_ITEM)
        {
          int rr1, rr2, cc1, cc2;
          rr1 = EZ_FigItemR1(item);
          cc1 = EZ_FigItemC1(item);
          rr2 = EZ_FigItemR2(item);
          cc2 = EZ_FigItemC2(item);

          if(r2 < rr1 || r1 > rr2 || c1 > cc2 || c2 < cc1) return;
          dirty = 1;
          EZ_ItemDirty(item) = 1;
          if(EZ_FigItemSCHandle(item))
            {
              int kind = EZ_FigItemSKind(item);
              void *data = EZ_FigItemSCData(item);
              (EZ_FigItemSCHandle(item))(wptr, item, rr1,cc1,rr2,cc2, kind, data);
            }
        }
    }
  if(dirty && EZ_WidgetMapped(wptr)) EZ_DrawWidget(wptr);
}

void EZ_SSheetDeleteAllCharts(wptr) EZ_Widget *wptr;
{
  if(wptr && EZ_WidgetType(wptr) == EZ_WIDGET_SPREAD_SHEET)
    {
      int i, ncharts = EZ_SSheetNCharts(wptr);
      if(ncharts > 0)
        {
          EZ_Item **charts = EZ_SSheetCharts(wptr);
          for(i = ncharts-1; i >= 0; i++) EZ_DestroyItem(charts[i]);
          EZ_SSheetNCharts(wptr) = 0;
          if(EZ_WidgetMapped(wptr)) EZ_DrawWidget(wptr);
        }
    }
}
void EZ_SSheetDeleteChart(wptr, chart)
     EZ_Widget *wptr; EZ_Item *chart;
{
  if(wptr && EZ_WidgetType(wptr) == EZ_WIDGET_SPREAD_SHEET)
    {
      int i, deleted = 0, ncharts = EZ_SSheetNCharts(wptr);
      EZ_Item **charts = EZ_SSheetCharts(wptr);
      for(i = ncharts-1; i >= 0; i++)
        {
          if(charts[i] == chart)
            {
              if(i != ncharts -1)
                charts[i] = charts[ncharts-1];
              EZ_DestroyItem(chart);
              deleted = 1;
              break;
            }
        }
      if(deleted)
        {
          if(ncharts > 0) {EZ_SSheetNCharts(wptr) -= 1;}
          if(EZ_WidgetMapped(wptr)) EZ_DrawWidget(wptr);
        }
    }
}
EZ_Item *EZ_SSheetGetActiveChart(widget) EZ_Widget *widget;
{
  if(widget && EZ_WidgetType(widget) == EZ_WIDGET_SPREAD_SHEET)
    {
      EZ_Item *chart = EZ_SSheetSChart(widget);
      return(EZ_LookupItemFromHT(chart));
    }
  return(NULL);
}
EZ_Item **EZ_SSheetGetAllCharts(widget, cnt) EZ_Widget *widget; int *cnt;
{
  if(widget && EZ_WidgetType(widget) == EZ_WIDGET_SPREAD_SHEET)
    {
      int ncharts =EZ_SSheetNCharts(widget);
      if(cnt) *cnt = ncharts;
      if(ncharts) return(EZ_SSheetCharts(widget));
    }
  return(NULL);
}
/*******************************************************************/
void EZ_SSheetSetTextProperty(widget, prop) EZ_Widget *widget; TextProp *prop;
{
  if(widget && EZ_WidgetType(widget) == EZ_WIDGET_SPREAD_SHEET)
    {EZ_SSheetTextProperty(widget) = prop; }
}
char *EZ_SSheetGetCell(widget, row, col)
     EZ_Widget *widget; int row, col;
{
  return(EZ_SSheetGetCellWithProperty(widget, row, col, NULL));
}
char *EZ_SSheetGetCellWithProperty(widget, row, col, prop)
     EZ_Widget *widget; int row, col; TextProp ***prop;
{
  if(widget && EZ_WidgetType(widget) == EZ_WIDGET_SPREAD_SHEET)
    {
      EZ_SSItem *prev, *item = locateCreateSSItem(widget, row, col, 0, &prev);
      if(item)
        {
          ITextLine *tline = item->text;
          if(prop) *prop = tline->prop;
          return(tline->data);
        }
    }
  return(NULL);
}
/*******************************************************************/
void EZ_SSheetSwapRow(widget, r1, r2) EZ_Widget *widget; int r1, r2;
{
  if(widget && EZ_WidgetType(widget) == EZ_WIDGET_SPREAD_SHEET)
    {
      int tmp, nrows = EZ_SSheetNRows(widget);
      if(r1 > r2) {tmp=r1; r1 = r2;r2=tmp;}
      if(r1 >= 0 && r1 != r2 && r2 < nrows)
        {
          EZ_SSItem *data = EZ_SSheetData(widget);
          EZ_SSItem *save = data[r1].next;
          data[r1].next = data[r2].next;
          data[r2].next = save;
          while(save) {save->rowOrWidth = r2; save=save->next;}
          save=data[r1].next; while(save) {save->rowOrWidth = r1; save=save->next;}
          EZ_SSheetDirty(widget) =1;
          {
            int freezed = EZ_GetWidgetFreezedFlag(widget);
            if(freezed == 0)
              {
                EZ_SetWidgetFreezedFlag(widget);
                unmarkActiveItem(widget);
                EZ_SSheetValidateCharts(widget, r1,0, r1,32766);
                EZ_SSheetValidateCharts(widget, r2,0, r2,32766);
                EZ_ClearWidgetFreezedFlag(widget);
                if(EZ_WidgetMapped(widget)) EZ_DrawWidget(widget);
              }
          }
        }
    }
}
void EZ_SSheetSwapColumn(widget, c1, c2) EZ_Widget *widget; int c1, c2;
{
  if(widget && EZ_WidgetType(widget) == EZ_WIDGET_SPREAD_SHEET)
    {
      int tmp, ncols = EZ_SSheetNColumns(widget);
      if(c1 > c2) {tmp=c1; c1 = c2;c2=tmp;}
      if(c1 >= 0 && c1 != c2 && c2 < ncols)
        {
          int i, nrows = EZ_SSheetNRows(widget);
          EZ_SSItem *data = EZ_SSheetData(widget);
          EZ_SSItem *prev, *next;
          for(i = 0; i < nrows; i++)
            {
              EZ_SSItem *item1=NULL, *item2=NULL;
              prev = &(data[i]); next=prev->next;
              while(prev && next)
                {
                  if(next->colOrHeight == c1) 
                    {
                      item1 = next; item1->colOrHeight=c2;
                      prev->next = item1->next;
                    }
                  else if(next->colOrHeight == c2)
                    {
                      item2 = next; item2->colOrHeight =c1;
                      prev->next = item2->next;
                    }
                  else prev = next;
                  if(prev) next = prev->next;
                }
              if(item1)
                {
                  prev = &(data[i]); next=prev->next;
                  while(prev && next)
                    {
                      if(next->colOrHeight > c1) break;
                      prev = next;
                      next = prev->next;
                    }
                  item1->next = next;
                  prev->next = item1;
                }
              if(item2)
                {
                  prev = &(data[i]); next=prev->next;
                  while(prev && next)
                    {
                      if(next->colOrHeight > c2) break;
                      prev = next;
                      next = prev->next;
                    }
                  item2->next = next;
                  prev->next = item2;
                }
            }
          EZ_SSheetDirty(widget) =1;
          {
            int freezed = EZ_GetWidgetFreezedFlag(widget);
            if(freezed == 0) 
              {
                EZ_SetWidgetFreezedFlag(widget);
                unmarkActiveItem(widget);
                EZ_SSheetValidateCharts(widget, 0,c1,32766,c1);
                EZ_SSheetValidateCharts(widget, 0,c2,32766,c2);           
                EZ_ClearWidgetFreezedFlag(widget);
                if(EZ_WidgetMapped(widget)) EZ_DrawWidget(widget);
              }
          }
        }
    }
}
void EZ_SSheetSwapCell(widget, r1, c1, r2, c2)
     EZ_Widget *widget; int r1,c1,r2,c2;
{
  if(widget && EZ_WidgetType(widget) == EZ_WIDGET_SPREAD_SHEET)
    {
      EZ_SSItem *prev1, *item1,*prev2, *item2;
      EZ_SSItem *data = EZ_SSheetData(widget);
      item1 = locateCreateSSItem(widget, r1, c1, 0, &prev1);
      if(item1) {prev1->next = item1->next; item1->rowOrWidth = r2;item1->colOrHeight = c2;}
      item2 = locateCreateSSItem(widget, r2, c2, 0, &prev2);
      if(item2) {prev2->next = item2->next; item2->rowOrWidth = r1;item2->colOrHeight = c1;}
      if(item1)
        {
          EZ_SSItem *prev = &(data[r2]), *next=prev->next;
          while(prev && next)
            {
              if(next->colOrHeight > c2) break;
              prev = next;
              next = prev->next;
            }
          item1->next = next;
          prev->next = item1;
        }
      if(item2)
        {
          EZ_SSItem *prev = &(data[r1]), *next=prev->next;
          while(prev && next)
            {
              if(next->colOrHeight > c1) break;
              prev = next;
              next = prev->next;
            }
          item2->next = next;
          prev->next = item2;
        }          
      EZ_SSheetDirty(widget) =1;
      {
        int freezed = EZ_GetWidgetFreezedFlag(widget);
        if(freezed == 0)
          {
            EZ_SetWidgetFreezedFlag(widget);
            unmarkActiveItem(widget);
            EZ_SSheetValidateCharts(widget, r1,c1, r1,c1);
            EZ_SSheetValidateCharts(widget, r2,c2, r2,c2);
            EZ_ClearWidgetFreezedFlag(widget);
            if(EZ_WidgetMapped(widget)) EZ_DrawWidget(widget);
          }
      }
    }
}
/*******************************************************************/
static  int (*ssheetSorter)MY_ANSIARGS((CONST void *, CONST void *));

static int cmpSSItems(va, vb) void *va, *vb;
{
  EZ_SSItem **nap = (EZ_SSItem **)va;
  EZ_SSItem **nbp = (EZ_SSItem **)vb;
  EZ_SSItem *na = *nap;
  EZ_SSItem *nb = *nbp;

  if(na == NULL) return (1);
  else if(nb == NULL) return (-1);
  else
    {
      ITextLine *a = na->text;
      ITextLine *b = nb->text;
      char *astr, *bstr;

      if(a->length == 0) astr = NULL;
      else astr = a->data;
      if(b->length == 0) bstr = NULL;
      else bstr = b->data;

      return(ssheetSorter( &astr, &bstr));
    }
}

static void ssheetPurge(widget) EZ_Widget *widget;
{
  int i, j, nrows = EZ_SSheetNRows(widget);
  EZ_SSItem *rr, *prev, *next, *data = EZ_SSheetData(widget);
  for(j=0, i =0; i < nrows; i++)
    {
      prev= &(data[i]); next = prev->next;
      while(prev && next)
        {
          ITextLine *tline = next->text;
          if(tline->length == 0)
            {
              rr = next;
              next = next->next;
              EZ_FreeTextLine(tline);
              my_free(rr);
              prev->next = next;
              j++;
            }
          else { prev= next; next=prev->next;}
        }
    }
  if(j) EZ_SSheetDirty(widget) = 1;
}
/*******************************************************************/
void EZ_SSheetSortRegion(widget,r1,c1,r2,c2,cc, cmp)
     EZ_Widget *widget; int r1,c1,r2,c2,cc;
     int (*cmp)MY_ANSIARGS((CONST void *, CONST void *));
{
  if(widget && EZ_WidgetType(widget) == EZ_WIDGET_SPREAD_SHEET)
    {
      EZ_SSItem **prevs, **nexts, **data;
      int i, j, nrows, ncols, itmp;
      EZ_SSheetGetSizes(widget, &nrows, &ncols);
      if(r2 < r1) {itmp=r2; r2=r1; r1=itmp;}
      if(c2 < c1) {itmp=c2; c2=c1; c1=itmp;}
      if(r1<0)r1=0; if(r2 > nrows-1) r2 = nrows-1;
      if(r2 < r1) {itmp=r2; r2=r1; r1=itmp;}

      if(c1<0 || c2>ncols ||cc<c1 || cc>c2 || r1<0) return;

      if(cc > c2) cc=c2; else if(cc<c1) cc=c1;
      nrows = r2-r1+1;      
      prevs = (EZ_SSItem **)my_malloc( (nrows+1)*sizeof(EZ_SSItem*), _SSHEET_DATA_);
      nexts = (EZ_SSItem **)my_malloc( (nrows+1)*sizeof(EZ_SSItem*), _SSHEET_DATA_);
      data  = (EZ_SSItem **)my_malloc( 3*(nrows+1)*sizeof(EZ_SSItem*), _SSHEET_DATA_);
      if(cmp) ssheetSorter = cmp;
      else  ssheetSorter = (int (*) MY_ANSIARGS((CONST void *,CONST void *))) EZ_CompareStr;
      if(c1 == c2)
        {
          for(i=r1; i<=r2; i++)
            {
              EZ_SSItem *stmp, *pp;
              j = i -r1;
              stmp = locateCreateSSItem(widget, i, c1, 1, &pp);
              prevs[j]=pp; nexts[j]=stmp->next; data[j]=stmp;
            }
          qsort(data, nrows, sizeof(EZ_SSItem *), cmpSSItems);
          for(i = 0; i < nrows; i++) 
            {
              prevs[i]->next = data[i];
              data[i]->next = nexts[i];
              data[i]->rowOrWidth=i+r1;
            }
        }
      else
        {
          EZ_SSItem *tmp, *stmp, *ttmp, *utmp, *pp;
          int k, dd =(cc != c1 && cc != c2);
          for(i=r1; i<=r2; i++)
            {
              k = i-r1;
              j=k+k+k;
              stmp=locateCreateSSItem(widget, i, c1, 1, &pp);
              prevs[k] = pp; data[j+1] = stmp;
              if(dd)
                {
                  tmp=locateCreateSSItem(widget, i, cc, 1, &pp);
                  data[j] = tmp;
                }
              ttmp=locateCreateSSItem(widget, i, c2, 1, &utmp);
              data[j+2]=ttmp; nexts[k] = ttmp->next;
              if(!dd)
                {
                  if(cc == c1) data[j] =stmp;
                  else data[j] = ttmp;
                }
            }
          qsort(data, nrows, 3*sizeof(EZ_SSItem *), cmpSSItems);
      
          for(i = 0; i < nrows; i++) 
            {
              EZ_SSItem *tmp, *stmp;
              int  idx = i+r1;
              j = i+i+i;
              prevs[i]->next = data[j+1];
              data[j+2]->next = nexts[i];
              tmp= data[j+1]; stmp = nexts[i];
              while(tmp && tmp != stmp) { tmp->rowOrWidth=idx; tmp=tmp->next;}
            }
        }
      my_free(prevs); my_free(nexts); my_free(data);
      ssheetPurge(widget);
      EZ_SSheetDirty(widget) = 1;
    }
}
/****************************************************************************/
void EZ_SSheetSortColumns(EZ_Widget *widget, int r1, int c1, int r2, int c2, 
			  int (*cmp)MY_ANSIARGS((CONST void *, CONST void *)))
{
  if(widget && EZ_WidgetType(widget) == EZ_WIDGET_SPREAD_SHEET)
    {
      int i, nrows, ncols, itmp;
      EZ_SSheetGetSizes(widget, &nrows, &ncols);
      if(r2 < r1) {itmp=r2; r2=r1; r1=itmp;}
      if(c2 < c1) {itmp=c2; c2=c1; c1=itmp;}
      if(r1<0)r1=0; if(r2 > nrows-1) r2 = nrows-1;
      if(r2 < r1) {itmp=r2; r2=r1; r1=itmp;}
      for(i = c1; i <=c2; i++) EZ_SSheetSortRegion(widget, r1, i, r2, i, i, cmp);
    }
}
/******************************************************************************************/
void EZ_SSheetSortRegionR(EZ_Widget *widget, int r1, int c1, int r2, int c2, int rr,
			  int (*cmp)MY_ANSIARGS((CONST void *, CONST void *)))
{
  if(widget && EZ_WidgetType(widget) == EZ_WIDGET_SPREAD_SHEET)
    {
      EZ_SSItem **prevs, **nexts, **data;
      int i, j, nrows, ncols, itmp;
      EZ_SSheetGetSizes(widget, &nrows, &ncols);
      if(r2 < r1) {itmp=r2; r2=r1; r1=itmp;}
      if(c2 < c1) {itmp=c2; c2=c1; c1=itmp;}
      if(c1<0)c1=0; if(c2>ncols-1)c2=ncols-1;
      if(c2 < c1) {itmp=c2; c2=c1; c1=itmp;}
      if(r1<0 || r2> nrows ||rr<r1 || rr>r2 ||c1==c2) return;

      ncols = c2-c1+1;      
      nrows = r2-r1+1;
      prevs = (EZ_SSItem **)my_malloc( (nrows+2)*sizeof(EZ_SSItem*), _SSHEET_DATA_);
      nexts = (EZ_SSItem **)my_malloc( (nrows+2)*sizeof(EZ_SSItem*), _SSHEET_DATA_);
      data  = (EZ_SSItem **)my_malloc( (ncols+1)*(nrows+2)*sizeof(EZ_SSItem*), _SSHEET_DATA_);
      if(cmp) ssheetSorter = cmp;
      else  ssheetSorter = (int (*) MY_ANSIARGS((CONST void *,CONST void *))) EZ_CompareStr;
      if(r1 == r2)
        {
          for(i=c1; i<=c2; i++)
            {
              EZ_SSItem *stmp, *pp;
              j = i -c1;
              stmp = locateCreateSSItem(widget, r1, i, 1, &pp);
	      if(i==c1) prevs[0]=pp; else if(i==c2) nexts[0]=stmp->next;
	      data[j]=stmp;
            }
          qsort(data, ncols, sizeof(EZ_SSItem *), 
		(int (*) MY_ANSIARGS((CONST void *,CONST void *)))cmpSSItems);
	  prevs[0]->next = data[0];
          for(i = 0; i < ncols; i++) 
            {
              data[i]->colOrHeight=i+c1;
	      if(i==ncols-1) data[i]->next = nexts[0];
	      else  data[i]->next = data[i+1];
            }
        }
      else
        {
          EZ_SSItem *stmp, *pp;
          int ii, jj;
          for(i=r1; i<=r2; i++)
            {
	      ii=i-r1;
	      for(jj=1, j=c1; j<=c2; j++, jj += nrows+1)
		{
		  stmp=locateCreateSSItem(widget, i, j, 1, &pp);
		  if(j==c1) prevs[ii] = pp; else if(j==c2) nexts[ii] = stmp->next;
		  if(i==rr) data[(j-c1)*(nrows+1)] = stmp;
		  data[jj+ii] = stmp;
		}
            }
          qsort(data, ncols, (nrows+1)*sizeof(EZ_SSItem *), 
		(int (*) MY_ANSIARGS((CONST void *,CONST void *)))cmpSSItems);

	  for(i = 0; i < nrows; i++)  for(jj=1, j = 0; j < ncols; j++, jj+=nrows+1) 
	    {
	      int kk =jj+i;
	      data[kk]->colOrHeight=c1+j;
	      data[kk]->rowOrWidth=r1+i;
	      if(j==0) prevs[i]->next= data[kk];
	      if(j==ncols-1) data[kk]->next=nexts[i];
	      else data[kk]->next = data[kk+nrows+1];
	    }
        }
      my_free(prevs); my_free(nexts); my_free(data);
      ssheetPurge(widget);
      EZ_SSheetDirty(widget) = 1;
    }
}
/******************************************************************************************/
void EZ_SSheetSortRows(EZ_Widget *widget, int r1, int c1, int r2, int c2, 
		       int (*cmp)MY_ANSIARGS((CONST void *, CONST void *)))
{
  if(widget && EZ_WidgetType(widget) == EZ_WIDGET_SPREAD_SHEET)
    {
      int i, nrows, ncols, itmp;
      EZ_SSheetGetSizes(widget, &nrows, &ncols);
      if(r2 < r1) {itmp=r2; r2=r1; r1=itmp;}
      if(c2 < c1) {itmp=c2; c2=c1; c1=itmp;}
      if(c1<0)c1=0; if(c2>ncols-1)c2=ncols-1;
      if(c2 < c1) {itmp=c2; c2=c1; c1=itmp;}
      for(i = r1; i <=r2; i++) EZ_SSheetSortRegionR(widget, i, c1, i, c2, i, cmp);
    }
}

/*******************************************************************/
int  EZ_SSheetFindCell(widget, str, row,col)
     EZ_Widget *widget; char *str; int *row, *col;
{
  if(str && widget && EZ_WidgetType(widget) == EZ_WIDGET_SPREAD_SHEET)
    {
      int i, len, nn = EZ_SSheetNRows(widget);
      EZ_SSItem *tmp, *data = EZ_SSheetData(widget);
      len = strlen(str); 
      for(i=0; i<nn; i++)
        {
          int j = 0;
          tmp= data[i].next;

          while(tmp)
            {
              ITextLine *tline = tmp->text;
              int tlen = tline->length;
              if(tlen >= len)
                { if(EZ_MatchWord(tline->data, str,tlen, len, NULL)) { *row=i; *col=j; return(1);} }
              tmp=tmp->next; j++;
            }
        }
    } 
  return(0);
}
/*******************************************************************/
static void hideRowsOrCols(widget, s1, s2, row, hu)
     EZ_Widget *widget; int s1, s2, row, hu;
{
  int i; 
  if(s1 > s2) {i=s1; s1=s2; s2=i;}  
  if(s1 < 0) return;
  if(row)
    {
      EZ_SSItem *data = EZ_SSheetData(widget);
      int n = EZ_SSheetNRows(widget);
      if(s2 >= n) return;
      if(hu)
        {
          for(i = s1; i <= s2; i++)
            {
              data[i].unused = (unsigned char)data[i].colOrHeight;
              data[i].colOrHeight = 1;
            }
        }
      else
        {
          int tt = EZ_SSheetCellHeight(widget);
          for(i = s1; i <= s2; i++)
            {
              if(data[i].unused)
                { data[i].colOrHeight = data[i].unused; data[i].unused = 0;}
              else data[i].colOrHeight = tt;
            }
        }
    }
  else
    {
      EZ_SSItem *data = EZ_SSheetHeadRow(widget);
      int n = EZ_SSheetNColumns(widget);
      if(s2 >= n) return;
      if(hu)
        {
          for(i = s1; i <=s2; i++)
            {
              data[i].unused = (unsigned char)data[i].rowOrWidth;
              data[i].rowOrWidth = 1;
            }
        }
      else
        {
          int tt = EZ_SSheetCellWidth(widget);
          for(i = s1; i <= s2; i++)
            {
              if(data[i].unused)
                { data[i].rowOrWidth = data[i].unused; data[i].unused = 0;}
              else data[i].rowOrWidth = tt;
            }
        }
    }
  EZ_SSheetDirty(widget) = 1;
  if(EZ_WidgetMapped(widget)) 
    {
      EZ_ComputeSSheetWindow(widget, DO_SCROLL);
      EZ_DrawWidget(widget);
    }
}

void EZ_SSheetHideColumns(widget, c1, c2) EZ_Widget *widget; int c1, c2;
{
  hideRowsOrCols(widget, c1, c2, 0, 1);
}
void EZ_SSheetHideRows(widget, r1, r2) EZ_Widget *widget; int r1, r2;
{
  hideRowsOrCols(widget, r1, r2, 1, 1);
}
void EZ_SSheetUnhideColumns(widget, c1, c2) EZ_Widget *widget; int c1, c2;
{
  hideRowsOrCols(widget, c1, c2, 0, 0);
}
void EZ_SSheetUnhideRows(widget, r1, r2) EZ_Widget *widget; int r1, r2;
{
  hideRowsOrCols(widget, r1, r2, 1, 0);
}
/*******************************************************************/
int EZ_MatchWord(buf, word, blen, wlen, pos_ret) 
     char *buf; char *word; int blen, wlen; int *pos_ret;
{
  int pfxS[256], *pfx = pfxS;
  if(buf == NULL || word == NULL) return(0);
  if(blen < 0)blen = strlen(buf);
  if(wlen < 0)wlen = strlen(word);
  if(blen < wlen) return(0);
  if(wlen > 250)  pfx = (int *)my_malloc( (wlen + 4) * sizeof(int), _MISC_DATA_);
  
  /* compute the prefix function */
  {
    int m = wlen;
    int i,j;
    pfx[0] = -2;
    pfx[1] = -1;
    for(i = 2; i < m; i++)
      {
        j = pfx[i-1] + 1;
        while(word[i-1] != word[j] && j >= 0) j = pfx[j]+1;
        pfx[i] = j;
      }
  }
  /* match */
  {
    int n = blen;
    int m = wlen;
    int i=0, j=0;

    while(i < n)
      {
        if(word[j] == buf[i]) {j++; i++;}
        else 
          {
            j = pfx[j] + 1;
            if(j < 0) {j = 0; i++;}
          }
        if(j == m)
          {
            if(pos_ret) *pos_ret = i-m;
            if(pfx != pfxS) free(pfx);
            return(1);
          }
      }
  }
  if(pfx != pfxS) free(pfx);
  return(0);
}
/*******************************************************************/
#undef _EZ_WIDGET_SSHEET_C_
          
