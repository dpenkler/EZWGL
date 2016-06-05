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
 ***              Widget Optional Entry                        ***
 ***                                                           ***
 ***                                                           ***
 *****************************************************************/
#define _EZ_WIDGET_OPT_INPUT_C_

#include "EZ_Widget.h"

/*********************************************************************
 * 
 *  Functions implemented in this file:
 */
EZ_Widget        *EZ_CreateOptionalEntry MY_ANSIARGS((EZ_Widget *parent, char *str, int hist, int editable));
void              EZ_SetOptionalEntryString MY_ANSIARGS((EZ_Widget *widget, char *str));
char             *EZ_GetOptionalEntryString MY_ANSIARGS((EZ_Widget *widget));
void              EZ_GetOptionalEntryWidgetComponents MY_ANSIARGS((EZ_Widget *widget, 
                                                                   EZ_Widget **entry, EZ_Widget **listbox, 
                                                                   EZ_Widget **btn ));
void  EZ_OptionalEntryClearOptions     MY_ANSIARGS((EZ_Widget *widget));
void  EZ_SetOptionalEntryOptions       MY_ANSIARGS((EZ_Widget *widget, char **items, int count));
void  EZ_OptionalEntryAppendOption     MY_ANSIARGS((EZ_Widget *widget, char *item));
void  EZ_OptionalEntryInsertOption     MY_ANSIARGS((EZ_Widget *widget, char *str, int idx));
void  EZ_OptionalEntryDeleteOption     MY_ANSIARGS((EZ_Widget *widget, char *str));
void  EZ_SetOptionalEntryStyle         MY_ANSIARGS((EZ_Widget *widget, int style));

void  EZ_OptionalEntryInsertListOption MY_ANSIARGS((EZ_Widget *widget, char *str, int idx));
void  EZ_SetOptionalEntryOptionalItems MY_ANSIARGS((EZ_Widget *widget, char **items, int count));
/*********************************************************************
 * 
 * Local functions.
 */
static void EZ_OptEntryListboxMotionCallBack MY_ANSIARGS((EZ_Widget *widget, void *data));
static void EZ_OptEntryListboxDone MY_ANSIARGS((EZ_Widget *widget, void *data));
static void EZ_OptEntryEntryCallBack MY_ANSIARGS((EZ_Widget *widget, void *data));

/*********************************************************************/

EZ_Widget  *EZ_CreateOptionalEntry(parent, init_str, remhist, editable)
     EZ_Widget  *parent;     /* parent widget    */
     char       *init_str;
     int        remhist;
     int        editable;
{
  EZ_Widget  *frame, *btn, *listbox, *entry, *icon;

  frame = EZ_CreateNewWidget(parent);
  /* use the default widget handle */
  
  EZ_SetWidgetTypeAndNames(frame,EZ_WIDGET_OPTIONAL_ENTRY);
  EZ_WidgetPadX(frame) = 0;
  EZ_WidgetPadY(frame) = 0;
  EZ_WidgetSepX(frame) = 2;
  EZ_WidgetSepY(frame) = 0;
  EZ_WidgetBorderWidth(frame) = 0;
  EZ_WidgetStacking(frame) = EZ_HORIZONTAL;
  EZ_WidgetFillMode(frame) = EZ_FILL_HORIZONTALLY;
  EZ_SetWidgetHeightSetFlag(frame);

  entry = EZ_CreateEntry(frame, init_str);
  EZ_WidgetGroupLeader(entry) = frame;
  EZ_WidgetPadX(entry) = 0;
  EZ_WidgetPadY(entry) = 0;
  EZ_EntryLineLength(entry) = 15;
  EZ_EntryRemberHistory(entry) = (remhist != 0 ? 1 : 0);
  if(editable == 0) EZ_EntryEditable(entry) = 0;
  
  btn = EZ_CreateArrowButton(frame, EZ_DOWN_ARROW_BAR);
  EZ_WidgetGroupLeader(btn) = frame;
  EZ_WidgetPadB(btn) = 1;
  EZ_WidgetPadX(btn) = 1;
  EZ_WidgetPadY(btn) = 2;
  EZ_WidgetWidth(btn) = 10;
  EZ_SetWidgetWidthSetFlag(btn);
  
  icon = EZ_CreateIToplevelFrame(NULL, NULL);
  EZ_SetWidgetCINames(icon, "Popup", "popup");
  EZ_LabelMovable(icon) = 0; 
  EZ_WidgetPadX(icon) = 3;
  EZ_WidgetPadY(icon) = 3;
  EZ_WidgetFillMode(icon) = EZ_FILL_BOTH;
  EZ_WidgetBorderWidth(icon) = 2;
  EZ_WidgetBorderStyle(icon) = EZ_BORDER_UP;
  EZ_LabelDontTakeFocus(icon) = 1;

  listbox = EZ_CreateListBox(icon,1,1);
  EZ_SetWidgetTransientFlag(listbox);
  EZ_ListBoxCallBackOnRelease(listbox) = 1;
  EZ_AddWidgetCallBackInternal(listbox, EZ_MOTION_CALLBACK,
			       (EZ_CallBack) EZ_OptEntryListboxMotionCallBack, 
			       frame, 0);
  EZ_AddWidgetCallBackInternal(listbox, EZ_CALLBACK, (EZ_CallBack)EZ_OptEntryListboxDone, listbox,0);

  EZ_AddWidgetCallBackInternal(entry,  EZ_CALLBACK, (EZ_CallBack)EZ_OptEntryEntryCallBack, 
			       EZ_ListBoxIListBox(listbox), 0);

  /*
   * link widgets together
   */
  EZ_OptEntryEntry(frame) = entry;
  EZ_OptEntryButton(frame) = btn;
  EZ_OptEntryListbox(frame) = listbox;
  EZ_OptEntryPopup(frame) = icon;
  EZ_IToplevelInitiator(icon) = entry;

  EZ_EntryOptEntry(entry) = frame;
  EZ_ListBoxOptEntry(listbox) = frame;
  EZ_ArrowButtonOptEntry(btn) = frame;

  return(frame);
}
/*************************************************************/
static void EZ_OptEntryListboxDone(confused, data)
     EZ_Widget *confused;  /* data is a LISTBOX */
     void *data;
{
  EZ_Widget *widget = (EZ_Widget *) data;
  EZ_Widget *optEntry = EZ_ListBoxOptEntry(widget);
  EZ_Widget *entry = EZ_OptEntryEntry(optEntry);
  EZ_Widget *parent = EZ_OptEntryPopup(optEntry);
  EZ_HideWidget(parent);

      /* remove highlighting on entry */
  EZ_ClearWidgetHighlightFlag(entry);
  EZ_DrawWidget(entry);	  
  /* call the entry call back */
  if(EZ_IListBoxSLine(widget) >= 0)
    {
      EZ_ExecuteWidgetCallBacks(entry);
      EZ_ExecuteWidgetCallBacks(optEntry);
    }
}
/*************************************************************/
static void EZ_OptEntryListboxMotionCallBack(widget, data)
     EZ_Widget *widget;
     void *data;
{
  char *item = EZ_GetListBoxSelectedItem(widget);
  if(item)
    {
      EZ_Widget *optEntry = (EZ_Widget *)data;
      EZ_Widget *entry = EZ_OptEntryEntry(optEntry);
      EZ_SetEntryString(entry, item);
    }
}
/*************************************************************/
static void EZ_OptEntryEntryCallBack(widget, data)
     EZ_Widget *widget;
     void *data;
{
  if(widget && data)
    {
      char    *str = EZ_GetEntryString(widget);
      if(str && *str ) /* ignore NULL */
	{
	  listboxentry *item;
	  int     i, insert = 1,  nitems;
	  EZ_Widget *ilistbox = (EZ_Widget *)data;
	  EZ_GetIListBoxData(ilistbox, &item, &nitems);
	  for(i = 0; i < nitems; i++)
	    {
	      if(!strcmp(str, (item +i)->data)) 
		{
		  int k, len;
		  char *data;
		  listboxentry *tmp; 
		  insert = 0; 
		  tmp = item + i;
		  len = tmp->length;
		  data = tmp->data;
		  for(k = i; k > 0; k--)
		    {
		      item[k].data = item[k-1].data;
		      item[k].length = item[k-1].length;
		    }
		  item[0].data = data;
		  item[0].length = len;
		  break;
		}
	    }
	  if(insert && EZ_EntryRemberHistory(widget))
	    EZ_InsertIListBoxItem(ilistbox, str, 0);
	  EZ_IListBoxSLine(ilistbox) = EZ_IListBoxFirstLine(ilistbox) = 0;  
	  EZ_IListBoxSLineEnd(ilistbox) = 0;
	}
    }  
}
/**************************************************************************************/
void   EZ_SetOptionalEntryString(widget, str)
     EZ_Widget *widget;
     char      *str;
{
  if(widget && EZ_WidgetType(widget) == EZ_WIDGET_OPTIONAL_ENTRY)
    {
      EZ_Widget *entry = EZ_OptEntryEntry(widget);
      EZ_Widget *listbox = EZ_OptEntryListbox(widget);
      EZ_SetEntryString(entry, str);
      EZ_OptEntryEntryCallBack(entry, EZ_ListBoxIListBox(listbox));
    }
}
/**************************************************************************************/
char *EZ_GetOptionalEntryString(widget)
     EZ_Widget *widget;
{
  return (EZ_GetEntryString(widget));
}
/**************************************************************************************/
void EZ_SetOptionalEntryOptions(widget, items, nitems)
     EZ_Widget *widget;
     char      **items;
     int       nitems;
{
  if(widget)
    {
      EZ_Widget *optEntry;
      switch(EZ_WidgetType(widget))
	{
	case EZ_WIDGET_OPTIONAL_ENTRY:
	  optEntry = widget;
	  break;
	case EZ_WIDGET_ENTRY:
	  optEntry = EZ_EntryOptEntry(widget);
	  break;
	default:
	  optEntry = NULL;
	  break;
	}
      if(optEntry)
	{
	  EZ_Widget *listbox = EZ_OptEntryListbox(optEntry);
	  if(listbox)
	    {
	      EZ_Widget *ilistbox = EZ_ListBoxIListBox(listbox);
	      if(items && nitems > 0)
		EZ_SetIListBoxData(ilistbox, items, nitems);
	      else EZ_ClearIListBox(ilistbox);
	    }
	}
    }
}
/**************************************************************************************/
void  EZ_GetOptionalEntryWidgetComponents(widget, entry_ret, listbox_ret, btn_ret)
     EZ_Widget *widget;
     EZ_Widget **entry_ret, **listbox_ret, **btn_ret;
{
  if(widget && EZ_WidgetType(widget) == EZ_WIDGET_OPTIONAL_ENTRY)
    {
      *entry_ret = EZ_OptEntryEntry(widget);
      *listbox_ret = EZ_OptEntryListbox(widget);
      *btn_ret = EZ_OptEntryButton(widget);
    }
}
/**************************************************************************************/

void  EZ_OptionalEntryClearOptions(widget) EZ_Widget *widget;
{ 
  EZ_Widget *optEntry = NULL;
  if(widget)
    {
      switch(EZ_WidgetType(widget))
	{
	case EZ_WIDGET_OPTIONAL_ENTRY:	  optEntry = widget; 	  break;
	case EZ_WIDGET_ENTRY:	  optEntry = EZ_EntryOptEntry(widget);	  break;
	default:        break;
	}
    }
  if(optEntry)
    {
      EZ_Widget *listbox = EZ_OptEntryListbox(optEntry);
      if(listbox)
	{
	  EZ_Widget *ilistbox = EZ_ListBoxIListBox(listbox);
	  EZ_ClearIListBox(ilistbox);
	}
    }
}

void  EZ_OptionalEntryAppendOption(widget,item)
     EZ_Widget *widget; char *item;
{
  EZ_Widget *optEntry = NULL;
  if(widget)
    {
      switch(EZ_WidgetType(widget))
	{
	case EZ_WIDGET_OPTIONAL_ENTRY:	  optEntry = widget; 	  break;
	case EZ_WIDGET_ENTRY:	  optEntry = EZ_EntryOptEntry(widget);	  break;
	default:        break;
	}
    }
  if(optEntry)
    {
      EZ_Widget *listbox = EZ_OptEntryListbox(optEntry);
      if(listbox)
	{
	  EZ_Widget *ilistbox = EZ_ListBoxIListBox(listbox);
	  EZ_AppendIListBoxItem(ilistbox, item);
	}
    }
}

void  EZ_OptionalEntryInsertOption(widget,item, idx)
     EZ_Widget *widget; char *item; int idx;
{
  EZ_Widget *optEntry = NULL;
  if(widget)
    {
      switch(EZ_WidgetType(widget))
	{
	case EZ_WIDGET_OPTIONAL_ENTRY:	  optEntry = widget; 	  break;
	case EZ_WIDGET_ENTRY:	  optEntry = EZ_EntryOptEntry(widget);	  break;
	default:        break;
	}
    }
  if(optEntry)
    {
      EZ_Widget *listbox = EZ_OptEntryListbox(optEntry);
      if(listbox)
	{
	  EZ_Widget *ilistbox = EZ_ListBoxIListBox(listbox);
	  EZ_InsertIListBoxItem(ilistbox, item, idx);
	}
    }
}
void  EZ_OptionalEntryDeleteOption(widget,item)
     EZ_Widget *widget; char *item;
{
  EZ_Widget *optEntry = NULL;
  if(widget)
    {
      switch(EZ_WidgetType(widget))
	{
	case EZ_WIDGET_OPTIONAL_ENTRY:	  optEntry = widget; 	  break;
	case EZ_WIDGET_ENTRY:	  optEntry = EZ_EntryOptEntry(widget);	  break;
	default:        break;
	}
    }
  if(optEntry)
    {
      EZ_Widget *listbox = EZ_OptEntryListbox(optEntry);
      if(listbox)
	{
	  EZ_Widget *ilistbox = EZ_ListBoxIListBox(listbox);
	  EZ_DeleteIListBoxString(ilistbox, item);
	}
    }
}
/**************************************************************************************/
void  EZ_SetOptionalEntryStyle(oentry,flag) EZ_Widget *oentry; int flag;
{
  if(oentry && EZ_WidgetType(oentry) == EZ_WIDGET_OPTIONAL_ENTRY)
    {
      EZ_Widget *entry = EZ_OptEntryEntry(oentry);
      EZ_Widget *btn =   EZ_OptEntryButton(oentry);
      if(flag)
        {
          if(EZ_WidgetParent(btn) != entry)
            EZ_ReparentWidget(btn, entry);
        }
      else
        {
          if(EZ_WidgetParent(btn) == entry)
            EZ_ReparentWidget(btn, oentry);
        }
    }
}
int  EZ_GetOptionalEntryStyle(oentry)
     EZ_Widget *oentry;
{
  if(oentry && EZ_WidgetType(oentry) == EZ_WIDGET_OPTIONAL_ENTRY)
    {
      EZ_Widget *entry = EZ_OptEntryEntry(oentry);
      EZ_Widget *btn =   EZ_OptEntryButton(oentry);
      if(EZ_WidgetParent(btn) == entry) return(1);
      else return(0);
    }
  return(-1);
}
/**************************************************************************************/
void  EZ_OptionalEntryInsertListOption(widget,item, idx)
     EZ_Widget *widget; char *item; int idx;
{ EZ_OptionalEntryInsertOption(widget,item, idx); }
void EZ_SetOptionalEntryOptionalItems(widget, items, nitems)
     EZ_Widget *widget;     char      **items;     int       nitems;
{  EZ_SetOptionalEntryOptions(widget, items, nitems); }
/**************************************************************************************/
#undef _EZ_WIDGET_OPT_INPUT_C_
