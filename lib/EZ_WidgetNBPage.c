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
 *  April 1997.  Release Version 1.1
 */
/***********************************************************************
 ***                                                                 ***
 ***                     NoteBook Page                               ***
 ***                                                                 ***
 ***********************************************************************/
#define _EZ_WIDGET_NBPAGE_C_
#include "EZ_Widget.h"

/*********************************************************************
 * 
 *  Functions implemented in this file:
 */
EZ_Widget   *EZ_CreateNoteBookPage  MY_ANSIARGS((EZ_Widget *parent, char *lab, int udl));
void        EZ_ComputeWidgetNBPageSize MY_ANSIARGS((EZ_Widget *widget, int *w, int *h));
void        EZ_DrawWidgetNBPage MY_ANSIARGS((EZ_Widget *widget));
void        EZ_ActivateNoteBookPage MY_ANSIARGS((EZ_Widget *widget));

EZ_Widget   *EZ_GetNoteBookPageTab MY_ANSIARGS((EZ_Widget *widget));
/*********************************************************************/
static void  EZ_ActivateNoteBookPageWork MY_ANSIARGS((EZ_Widget *widget, void *data));
/*********************************************************************
 * 
 *  Local Variables.
 */
static EZ_WidgetHandle EZ_NBPageHandle =
{ 
  EZ_ComputeWidgetUnknownSize,
  EZ_DrawWidgetNBPage,
  EZ_FreeUnknownData,
  EZ_UnkownEventHandle,
};

/*********************************************************************/
EZ_Widget *EZ_CreateNoteBookPage(parent, label, underline)
     EZ_Widget *parent;
     char      *label;
     int       underline;
{
  if(parent && EZ_WidgetType(parent) == EZ_WIDGET_NOTE_BOOK)
    {
      EZ_Widget *tmp, *tframe, *bframe, *button;
      
      tframe = EZ_NoteBookTFrame(parent);          /* top frame, hold stubs    */
      bframe = EZ_NoteBookBFrame(parent);          /* bottom frame, hold notes */

      /*
       * the stub 
       */
      button = EZ_CreateButton(tframe, label, underline); 
      EZ_WidgetGroupLeader(button) = parent;
      EZ_WidgetPadX(button) = 6;
      EZ_WidgetPadY(button) = 3;
      EZ_WidgetPadB(button) = 2;
      EZ_WidgetBorderWidth(button) = 2;
      EZ_ButtonNoHighlight(button) = 1;
      EZ_WidgetFont(button) = EZ_WidgetFont(parent);
      EZ_WidgetForeground(button) = EZ_WidgetForeground(parent);

      /*
       * the page 
       */
      tmp = EZ_CreateNewWidget(bframe); 
      EZ_WidgetHandlingFunctions[EZ_WIDGET_NB_PAGE] = &EZ_NBPageHandle;

      EZ_SetWidgetTypeAndNames(tmp,EZ_WIDGET_NB_PAGE);
      EZ_WidgetGroupLeader(tmp) = parent;

      EZ_WidgetBorderWidth(tmp) = 2;
      EZ_WidgetBorderStyle(tmp)= EZ_BORDER_UP;
      EZ_WidgetPadB(tmp) = 0; 
      EZ_WidgetPixmap(tmp) = (EZ_Bitmap *)NULL;
      EZ_WidgetFont(tmp) = EZ_WidgetFont(parent);
      EZ_NBPageNoteBook(tmp) = parent;

      /* set border type for stub */
      {
	int orien = EZ_WidgetStacking(parent);
	switch(orien)
	  {
	  case EZ_HORIZONTAL_LEFT:
	    EZ_WidgetBorderStyle(button)  = EZ_BORDER_NB_UP_L;
	    break;
	  case EZ_HORIZONTAL_RIGHT:	    
	    EZ_WidgetBorderStyle(button)  = EZ_BORDER_NB_UP_R;	    
	    break;
	  case EZ_VERTICAL_TOP:
	    EZ_WidgetBorderStyle(button)  = EZ_BORDER_NB_UP;
	    break;
	  case EZ_VERTICAL_BOTTOM:
	    EZ_WidgetBorderStyle(button)  = EZ_BORDER_NB_UP_B;
	    break;
	  default:
	    break;
	  }
      }
      /*
       * we keep the dim of the maximal min size of all pages under 
       * parent. And this size will be used as the size of all pages
       * if a fill mode is not set on parent.
       */
      EZ_SetWidgetWidthHintFlag(tmp);     
      EZ_SetWidgetHeightHintFlag(tmp);

      /*
       * hook stub and page together.
       */
      EZ_NBPageCompanion(tmp) = button;
      EZ_ButtonCompanion(button) = tmp;

      EZ_SetWidgetNonActiveFlag(tmp);
      if(EZ_NoteBookActivePage(parent) == (EZ_Widget *)NULL)
	EZ_ActivateNoteBookPage(button);
      EZ_AddWidgetCallBackInternal(button,  EZ_CALLBACK, 
				   (EZ_CallBack)EZ_ActivateNoteBookPageWork, tmp,0);

      return(tmp);
    }
  else
    {
      fprintf(stderr, "EZ_NoteBookCreatePage(): parent is not a NoteBook widget\n");
      return(NULL);
    }
}

/*********************************************************************/
void  EZ_DrawWidgetNBPage(wptr)
     EZ_Widget *wptr;
{
  int             w, h, ox,oy,tile;
  Pixmap          pixmap;
  Window          win;
  GC              gc;
  unsigned long   bgpv;

  win = EZ_WidgetWindow(wptr);
  w   = EZ_WidgetWidth(wptr);
  h   = EZ_WidgetHeight(wptr);

  pixmap = XCreatePixmap(EZ_DisplayForWidgets, win, w, h, EZ_DepthForWidgets); 
  if((tile = EZ_GetBackgroundGC(wptr, &gc, &bgpv, 0 ,0)))
    {
      EZ_GetWidgetTileOrigin(wptr, &ox, &oy);      
      XSetTSOrigin(EZ_Display, gc, ox, oy);
    }
  XFillRectangle(EZ_DisplayForWidgets, pixmap, gc, 0,0, w, h); 

  EZ_DrawRectBorder(wptr, pixmap); 

  /* fix the border */
  if(EZ_WidgetBorderStyle(wptr) != EZ_BORDER_NONE && EZ_WidgetBorderWidth(wptr) > 0)
    {
      EZ_Widget *prnt, *companion = EZ_NBPageCompanion(wptr); 
      int itmp, xtmp, ytmp, bw = EZ_WidgetBorderWidth(wptr);
      int cpb, c_bw, c_w, cbd = EZ_WidgetBorderStyle(companion);
      
      cpb = EZ_WidgetPadB(companion);	  
      c_bw = EZ_WidgetBorderWidth(companion) + cpb;

      if(EZ_GetBackgroundGC(wptr, &gc, &bgpv,0,0))
	{
	  if(!tile) EZ_GetWidgetTileOrigin(wptr, &ox, &oy);      
	  XSetTSOrigin(EZ_Display, gc, ox, oy);
	}
      switch(cbd)
	{
	case EZ_BORDER_NB_UP:
	case EZ_BORDER_NB_DOWN:	
	  c_w = EZ_WidgetWidth(companion) - c_bw - cpb;
	  xtmp = EZ_WidgetOriginX(companion) + c_bw;
	  XFillRectangle(EZ_Display, pixmap, gc, 
			 xtmp, 0, 
			 c_w, bw+1);
	  break;
	case EZ_BORDER_NB_UP_B:
	case EZ_BORDER_NB_DOWN_B:
	  prnt = EZ_WidgetParent(companion);
	  itmp = EZ_WidgetWidth(wptr) - EZ_WidgetWidth(prnt);
	  c_w = EZ_WidgetWidth(companion) - c_bw - cpb;
	  xtmp = EZ_WidgetOriginX(companion)+cpb;
	  XFillRectangle(EZ_Display, pixmap, gc, 
			 xtmp + itmp, EZ_WidgetHeight(wptr) - bw,
			 c_w, bw+1);
	  break;
	case EZ_BORDER_NB_UP_L:
	case EZ_BORDER_NB_DOWN_L:
	  prnt = EZ_WidgetParent(companion);
	  itmp = EZ_WidgetHeight(wptr) - EZ_WidgetHeight(prnt);
	  c_w = EZ_WidgetHeight(companion) - c_bw -  cpb;
	  ytmp = EZ_WidgetOriginY(companion) + c_bw;
	  XFillRectangle(EZ_Display, pixmap, gc, 
			 0, ytmp+itmp, 
			 bw+1,c_w);
	  break;
	case EZ_BORDER_NB_UP_R:
	case EZ_BORDER_NB_DOWN_R:
	  c_w = EZ_WidgetHeight(companion) - c_bw -  cpb;
	  ytmp = EZ_WidgetOriginY(companion)+cpb;
	  xtmp = EZ_WidgetWidth(wptr) -bw;
	  XFillRectangle(EZ_Display, pixmap, gc, 
			 xtmp, ytmp, bw+1,c_w);
	  break;
	default:
	  break;
	}
    }
  XCopyArea(EZ_Display,pixmap,win, EZ_WRITABLEGC,0,0,w,h,0,0); 
  XFreePixmap(EZ_Display, pixmap); 
}

/****************************************************************************/
void EZ_ActivateNoteBookPage(page)
     EZ_Widget *page;    /* page is a button or a NBPage */
{
  EZ_ActivateNoteBookPageWork(page, NULL);
}
/****************************************************************************/
static void EZ_ActivateNoteBookPageWork(widget, cdata)
     EZ_Widget *widget;    /* widget is a button or a NBPage */
     void *cdata;
{
  if(widget)
    {
      EZ_Widget *tab, *notebook = NULL, *pageframe=NULL, *activepage;
      int wtype = EZ_WidgetType(widget);
      if( wtype ==  EZ_WIDGET_NB_PAGE)
	{
	  notebook = EZ_NBPageNoteBook(widget);
	  tab = EZ_NBPageCompanion(widget);
	  pageframe = widget;
	}
      else if(wtype ==  EZ_WIDGET_NORMAL_BUTTON || wtype ==  EZ_WIDGET_ARROW_BUTTON)
	{
	  tab = widget;
	  pageframe = EZ_ButtonCompanion(widget);
	  if(pageframe) notebook = EZ_NBPageNoteBook(pageframe);
	}
      if(notebook)
	{
	  activepage = EZ_NoteBookActivePage(notebook);

	  if(activepage != pageframe)
	    {
	      EZ_FreezeWidget(notebook);
	      EZ_DeActivateWidget(activepage);
	      EZ_NoteBookActivePage(notebook) = pageframe;
	      EZ_ActivateWidget(pageframe);
	      EZ_UnFreezeWidget(notebook);
	    }
	}
    }
}
/****************************************************************************/
EZ_Widget   *EZ_GetNoteBookPageTab(widget)
     EZ_Widget *widget;
{
  if(widget && EZ_WidgetType(widget) == EZ_WIDGET_NB_PAGE)
    return(EZ_NBPageCompanion(widget));
  return(NULL);
}
/****************************************************************************/
#undef _EZ_WIDGET_NBPAGE_C_
