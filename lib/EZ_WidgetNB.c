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
 ***                     Widget NoteBook(hack)                       ***
 ***     Composite widget, consists of 3 frames.                     ***
 ***                                                                 ***
 ***********************************************************************/
#define _EZ_WIDGET_NB_C_
#include "EZ_Widget.h"

/*********************************************************************
 * 
 *  Functions implemented in this file:
 */
EZ_Widget   *EZ_CreateNoteBook  MY_ANSIARGS((EZ_Widget *parent));
int          EZ_SetNoteBookOrientation MY_ANSIARGS((EZ_Widget *widget, int value));
int          EZ_SetNoteBookOrientationA MY_ANSIARGS((EZ_Widget *widget, int value));
void         EZ_ComputeNoteBookSize MY_ANSIARGS((EZ_Widget *widget, int *w, int *h));

static EZ_WidgetHandle EZ_NoteBookHandle =
{ 
  EZ_ComputeNoteBookSize,
  EZ_DrawWidgetUnknown,
  EZ_FreeUnknownData,
  EZ_UnkownEventHandle,
};

/*********************************************************************/
void  EZ_ComputeNoteBookSize(widget, w, h)
     EZ_Widget *widget; int *w, *h;
{
  int cw, ch;
  EZ_SetWidgetSizeComputedFlag(widget);
  cw = EZ_WidgetPadX(widget) + EZ_WidgetBorderWidth(widget);
  ch = EZ_WidgetPadY(widget) + EZ_WidgetBorderWidth(widget);
  cw = cw + cw + 64;
  ch = ch + ch + 32;

  *w = cw;
  *h = ch;
}
/*********************************************************************/
EZ_Widget *EZ_CreateNoteBook(parent)
     EZ_Widget *parent;
{
  EZ_Widget  *tmp, *tframe, *bframe;

  tmp = EZ_CreateNewWidget(parent);

  /* use the default widget handle */
  EZ_WidgetHandlingFunctions[EZ_WIDGET_NOTE_BOOK] = &EZ_NoteBookHandle;
  EZ_SetWidgetTypeAndNames(tmp,EZ_WIDGET_NOTE_BOOK);
  EZ_WidgetBorderWidth(tmp) = 0;
  EZ_WidgetBorderStyle(tmp)  = EZ_BORDER_NONE;
  EZ_WidgetPixmap(tmp) = (EZ_Bitmap *)NULL;

  EZ_WidgetFont(tmp) = EZ_GetFontFromId(EZ_BUTTON_FONT);
  EZ_WidgetStacking(tmp) = EZ_VERTICAL_TOP;
  EZ_WidgetAlignment(tmp) = EZ_LEFT_ALIGNED;
  EZ_WidgetFillMode(tmp) = EZ_FILL_BOTH;
  EZ_WidgetAlignment(tmp) = EZ_LEFT;
  EZ_WidgetPadX(tmp) = 0;
  EZ_WidgetPadY(tmp) = 0; 
  EZ_WidgetPadB(tmp) = 0; 

  /* top frame, holds page stubs (buttons) */
  tframe = EZ_CreateFrame(tmp,NULL);
  EZ_WidgetGroupLeader(tframe) = tmp;
  EZ_FrameUndockable(tframe)  = 1;
  EZ_FrameInternal(tframe) = 1;
  EZ_WidgetPadX(tframe) = 0;
  EZ_WidgetPadY(tframe) = 0;
  EZ_WidgetPadB(tframe) = 0;
  EZ_WidgetSepX(tframe) = 0; 
  EZ_WidgetStacking(tframe) = EZ_HORIZONTAL_LEFT;
  EZ_WidgetFillMode(tframe) = EZ_FILL_VERTICALLY;
  EZ_SetWidgetHeightSetFlag(tframe);
  EZ_SetWidgetWidthSetFlag(tframe);
  EZ_WidgetBorderWidth(tframe) = 0;
  EZ_WidgetBorderStyle(tframe)  = EZ_BORDER_NONE;
  EZ_SetWidgetCINames(tframe, NULL, "topFrame");

  /* bottom frame */
  bframe = EZ_CreateFrame(tmp,NULL);
  EZ_WidgetGroupLeader(bframe) = tmp;
  EZ_FrameUndockable(bframe)  = 1;
  EZ_FrameInternal(bframe) = 1;
  EZ_WidgetBorderStyle(bframe) = EZ_BORDER_NONE;
  EZ_WidgetBorderWidth(bframe) = 0;
  EZ_WidgetPadX(bframe) = 0;
  EZ_WidgetPadY(bframe) = 0;
  EZ_WidgetPadB(bframe) = 0; 
  EZ_WidgetFillMode(bframe) = EZ_FILL_BOTH;
  EZ_SetWidgetCINames(bframe, NULL, "bottomFrame");

  EZ_NoteBookFrame(tmp) = tmp;
  EZ_NoteBookTFrame(tmp) = tframe;
  EZ_NoteBookBFrame(tmp) = bframe;
  EZ_NoteBookActivePage(tmp) = NULL;
  EZ_NoteBookMaxMinWidth(tmp) = 0;
  EZ_NoteBookMaxMinHeight(tmp) = 0;

  return(tmp);
}
/*********************************************************************/
int  EZ_SetNoteBookOrientation(widget, orien)
     EZ_Widget *widget; int orien;
{
  int changed =  EZ_SetNoteBookOrientationA(widget, orien);
  if(changed && EZ_WidgetMapped(widget))
    EZ_DisplayWidget(widget);
  return(changed);
}

int  EZ_SetNoteBookOrientationA(widget, orien)
     EZ_Widget *widget; int orien;
{
  if(widget && EZ_WidgetType(widget) == EZ_WIDGET_NOTE_BOOK)
    {
      int old = EZ_WidgetStacking(widget);
      if(old != orien)
	{
	  EZ_Widget *child, *tmp, *stub, *tframe = EZ_NoteBookTFrame(widget);
	  int set = 1;
	  switch(orien)
	    {
	    case EZ_HORIZONTAL:
	    case EZ_HORIZONTAL_LEFT:
	    case EZ_HORIZONTAL_CENTER:
	      EZ_WidgetStacking(widget) = EZ_HORIZONTAL_LEFT;
	      EZ_WidgetAlignment(widget) = EZ_BOTTOM_ALIGNED;
	      EZ_WidgetStacking(tframe) = EZ_VERTICAL_BOTTOM;
	      EZ_WidgetFillMode(tframe) = EZ_FILL_HORIZONTALLY;
	      EZ_ClearWidgetSizeComputedFlag(widget);
	      EZ_ClearWidgetSizeComputedFlag(tframe);
	      stub = EZ_WidgetChildren(tframe);
	      while(stub)
		{
		  EZ_ClearWidgetSizeComputedFlag(stub);
		  EZ_WidgetBorderStyle(stub) = EZ_BORDER_NB_UP_L;
		  stub = EZ_WidgetSibling(stub);
		}
	      child = EZ_WidgetChildren(widget);
	      if(child != tframe)
		{
		  tmp = EZ_WidgetSibling(child);
		  EZ_WidgetSibling(child) = NULL;
		  EZ_WidgetChildren(widget) = tmp;
		  EZ_WidgetSibling(tmp) = child;
		}
	      break;
	    case  EZ_HORIZONTAL_RIGHT:
	      EZ_WidgetStacking(widget) = EZ_HORIZONTAL_RIGHT;
	      EZ_WidgetAlignment(widget) = EZ_TOP_ALIGNED;
	      EZ_WidgetStacking(tframe) = EZ_VERTICAL_TOP;
	      EZ_WidgetFillMode(tframe) = EZ_FILL_HORIZONTALLY;
	      EZ_ClearWidgetSizeComputedFlag(widget);
	      EZ_ClearWidgetSizeComputedFlag(tframe);
	      stub = EZ_WidgetChildren(tframe);
	      while(stub)
		{
		  EZ_ClearWidgetSizeComputedFlag(stub);
		  EZ_WidgetBorderStyle(stub) = EZ_BORDER_NB_UP_R;
		  stub = EZ_WidgetSibling(stub);
		}
	      child = EZ_WidgetChildren(widget);
	      if(child == tframe)
		{
		  tmp = EZ_WidgetSibling(tframe);
		  EZ_WidgetSibling(tframe) = NULL;
		  EZ_WidgetChildren(widget) = tmp;
		  EZ_WidgetSibling(tmp) = tframe;
		}
	      break;	      
	    case EZ_VERTICAL:
	    case EZ_VERTICAL_TOP:
	    case  EZ_VERTICAL_CENTER:
	      EZ_WidgetStacking(widget) = EZ_VERTICAL_TOP;
	      EZ_WidgetAlignment(widget) = EZ_LEFT_ALIGNED;
	      EZ_WidgetStacking(tframe) = EZ_HORIZONTAL_LEFT;
	      EZ_WidgetFillMode(tframe) = EZ_FILL_VERTICALLY;
	      EZ_ClearWidgetSizeComputedFlag(widget);
	      EZ_ClearWidgetSizeComputedFlag(tframe);
	      stub = EZ_WidgetChildren(tframe);
	      while(stub)
		{
		  EZ_ClearWidgetSizeComputedFlag(stub);
		  EZ_WidgetBorderStyle(stub) = EZ_BORDER_NB_UP;
		  stub = EZ_WidgetSibling(stub);
		}
	      child = EZ_WidgetChildren(widget);
	      if(child != tframe)
		{
		  tmp = EZ_WidgetSibling(child);
		  EZ_WidgetSibling(child) = NULL;
		  EZ_WidgetChildren(widget) = tmp;
		  EZ_WidgetSibling(tmp) = child;
		}
	      break;
	    case  EZ_VERTICAL_BOTTOM:
	      EZ_WidgetStacking(widget) = EZ_VERTICAL_BOTTOM;
	      EZ_WidgetAlignment(widget) = EZ_RIGHT_ALIGNED;
	      EZ_WidgetStacking(tframe) = EZ_HORIZONTAL_RIGHT;
	      EZ_WidgetFillMode(tframe) = EZ_FILL_VERTICALLY;
	      EZ_ClearWidgetSizeComputedFlag(widget);
	      EZ_ClearWidgetSizeComputedFlag(tframe);
	      stub = EZ_WidgetChildren(tframe);
	      while(stub)
		{
		  EZ_ClearWidgetSizeComputedFlag(stub);
		  EZ_WidgetBorderStyle(stub) = EZ_BORDER_NB_UP_B;
		  stub = EZ_WidgetSibling(stub);
		}
	      child = EZ_WidgetChildren(widget);
	      if(child == tframe)
		{
		  tmp = EZ_WidgetSibling(tframe);
		  EZ_WidgetSibling(tframe) = NULL;
		  EZ_WidgetChildren(widget) = tmp;
		  EZ_WidgetSibling(tmp) = tframe;
		}
	      break;
	    default:
	      set = 0;
	      break;
	    }
	  return(set);
	}
    }
  return(0);
}
/*********************************************************************/
#undef _EZ_WIDGET_NB_C_
