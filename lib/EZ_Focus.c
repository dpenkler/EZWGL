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
/*******************************************************************
 *  
 *  Set Keyboard Focusing. 
 *              
 */

#define _EZ_FOCUS_C_
#include "EZ_Widget.h"

/*******************************************************************
 *
 * Functions implemented in this file:
 */
int  EZ_WidgetIsFocusable MY_ANSIARGS(( EZ_Widget *widget));
int  EZ_FocusNextWidget MY_ANSIARGS(( EZ_Widget *widget, int dir));
void EZ_SetFocusTo MY_ANSIARGS(( EZ_Widget *widget));
void EZ_ResetFocus MY_ANSIARGS((EZ_Widget *widget, int flag));
EZ_Widget *EZ_FindFocusingWidget MY_ANSIARGS(( EZ_Widget *widget));
EZ_Widget *EZ_GetFocus MY_ANSIARGS((void));
void EZ_ClearFocusRecord MY_ANSIARGS((void));
/*******************************************************************
 *
 * Global Variables:
 */
EZ_Widget *EZ_CurrentFocusingWidget = (EZ_Widget *)NULL; 
EZ_Widget *EZ_PendingFocus =  (EZ_Widget *)NULL; 
int        EZ_WeAreInFocus = 0;
/*******************************************************************
 *
 *  For manipulating keyboard focus. For each toplevel widget, 
 *  there is an entry in the list.
 */
typedef struct  _EZ_FocusRecord_ {
  EZ_Widget *toplevel;
  EZ_Widget *focus;
  struct  _EZ_FocusRecord_ *next;
} EZ_FocusRecord;

static  EZ_FocusRecord *EZ_FocusList = (EZ_FocusRecord *)NULL;

/*******************************************************************
 *
 * Local Functions.
 */
static void ExtractFocusableWidgets MY_ANSIARGS((EZ_Widget *widget,
						 EZ_Widget *root,
						 EZ_Widget **list,
						 int       *nwidgets, 
						 int       *mark, 
						 int       *markA,
						 EZ_FocusRecord *finfo,
                                                 int x0,int y0,int w,int h));
static EZ_FocusRecord *findOrCreateFocusRecord 
                      MY_ANSIARGS(( EZ_Widget *root, EZ_Widget *widget));
static void EZ_SetInputFocus
                          MY_ANSIARGS(( EZ_Widget *root, EZ_Widget *widget,
						 EZ_FocusRecord *finfo, int wtime));
/*******************************************************************
 * Each time this function is called, it builds a list of all widgets
 * on the current widget tree. ( it's really not necessary !). It then
 * find a focusable widget on the list.
 */
int EZ_FocusNextWidget(EZ_Widget *widget, int direction)
     /* EZ_Widget *widget; int direction; 0 ==>, !=0  <== */
{
  if(widget && EZ_WidgetExist(widget))
    {
      EZ_Widget *wlist[256];
      int        nwidgets = 0, mark = -1, markA = -1;
      EZ_Widget *root = widget;
      EZ_FocusRecord *finfo = NULL;
      
      /* find the root */
      while(EZ_WidgetParent(root))
	root = EZ_WidgetParent(root);

      /* make a list */
      if(EZ_WidgetMapped(root))
	{

	  finfo = findOrCreateFocusRecord(root, widget); 
	  ExtractFocusableWidgets(widget, root, wlist, &nwidgets, &mark, &markA, finfo,
                                  0, 0, EZ_WidgetWidth(root), EZ_WidgetHeight(root));
	}
      
      if(nwidgets == 0) return(0); /* something is wrong ! */
      else
	{
	  int       i, j;
	  EZ_Widget *tmp;

	  if(mark == -1 && markA >= 0) mark = markA;
	  if(direction == 0)  /* move forward */
	    {
	      for(i=0, j = mark+1; i < nwidgets && j != mark;  i++, j++)
		{
		  if(j == nwidgets) j = 0;
		  tmp = wlist[j];
		  if(EZ_WidgetMapped(tmp))
		    {
		      EZ_SetInputFocus(root, tmp, finfo, 0);
		      break;
		    }
		}
	    }
	  else
	    {
	      for(i=0, j = mark-1; i < nwidgets && j != mark;  i++, j--)
		{
		  if(j < 0 ) j = nwidgets - 1;
		  tmp = wlist[j];
		  if(EZ_WidgetMapped(tmp))
		    {
		      EZ_SetInputFocus(root, tmp, finfo,0);
		      break;
		    }
		}
	    }
	}
    }
  return(1);
}

/*******************************************************************
 *
 *  Extract the focusable widgets on the current widget tree.
 *  root is a toplevel widget, widget is a descendant of root.
 *
 */
static void ExtractFocusableWidgets(EZ_Widget *widget, EZ_Widget *root, EZ_Widget **list,
				    int *nwidgets, int *mark, int *markA,
				    EZ_FocusRecord *finfo,int x0, int y0, int wi,int hi)
{
  if(EZ_WidgetIsViewable(root) && EZ_GetWidgetDisabledFlag(root)==0)
    {
      Window rootWin = EZ_WidgetWindow(root);
      Window root_return;
      Window parent_return;
      unsigned int nchildren_return;
      Window *children_return = (Window *)NULL;
      
      if(finfo == NULL) finfo = findOrCreateFocusRecord(root, widget); 
      
      if( (EZ_GetWidgetFocusableFlag(root) || EZ_GetWidgetEmbededFlag(root))
	  && EZ_WidgetPadB(root) > 0)
	{
          int w = EZ_WidgetWidth(root);
          int h = EZ_WidgetHeight(root);
          
          if(x0 >=0 && y0 >= 0 && x0+w <= wi && y0+h <= hi)
            {
              if(root == widget) *markA = *nwidgets;
              else { if(root == finfo->focus) *mark = *nwidgets; }
              list[*nwidgets] = root;
              *nwidgets += 1;
              if(*nwidgets >= 256) *nwidgets = 0;
            }
	}  

      if(XQueryTree(EZ_Display,rootWin, &root_return, &parent_return,
		    &children_return, &nchildren_return))
	{
	  int i;
	  for(i = 0; i < nchildren_return; i++)
	    {
	      EZ_Widget *child = EZ_LookupWidgetFromMappedHT(children_return[i]);
	      if(child)
		{
		  int x = x0+EZ_WidgetOriginX(child);
		  int y = y0+EZ_WidgetOriginY(child);
                  ExtractFocusableWidgets(widget, child, list, nwidgets, mark, markA, finfo,
                                          x, y, wi,hi);
		}
	    }
	  if(children_return != (Window *)NULL) XFree(children_return);
	}
    }
}
/***************************************************************
 *
 * The calling function has to find the root of widget and verify 
 * that the windows are viewable.
 */
extern Time EZ_LastEventTime, EZ_LastResetFocusTime;

static void EZ_SetInputFocus(EZ_Widget *root, EZ_Widget *widget,
			     EZ_FocusRecord *finfo,   int wtime)
{
  if(root && EZ_WidgetExist(root))
    {
      int    (*OldErrorHandler)MY_ANSIARGS((Display *, XErrorEvent *));
      /* if widget has the focus, return */
      Window fw; int rt;
      EZ_Widget *tmp;
      XSync(EZ_Display, False);      
      XGetInputFocus(EZ_Display, &fw, &rt);
      tmp = EZ_LookupWidgetFromMappedHT(fw);
      if(tmp == widget) 
        {
          EZ_CurrentFocusingWidget = widget; 
          EZ_WeAreInFocus = 1; 
          EZ_DrawFocusIndicator(widget, None, FocusIn); 
          return;
        }
      if(EZ_LastEventTime - EZ_LastResetFocusTime > 8) /* kluge */
        {      
          if(finfo == NULL) finfo = findOrCreateFocusRecord(root, widget);
          EZ_XErrorCode = 0;
          OldErrorHandler = XSetErrorHandler(EZ_XErrorHandler);
          finfo->focus = widget;
          XSetInputFocus(EZ_Display, EZ_WidgetWindow(widget), RevertToParent, 
                         (wtime == 0? EZ_LastEventTime: CurrentTime));      
          XSync(EZ_Display, False);
          EZ_LastResetFocusTime = EZ_LastEventTime;
          XSetErrorHandler(OldErrorHandler);  
          if(EZ_XErrorCode == 0)
            {
              EZ_CurrentFocusingWidget = widget; 
              if(wtime != 0) EZ_PendingFocus = widget;
            }
          else {  EZ_CurrentFocusingWidget = NULL;}
        }
    }
}
/***************************************************************/
EZ_Widget *EZ_FindFocusingWidget(    EZ_Widget *widget)
{
  if(widget)
    {
      EZ_FocusRecord *finfo = findOrCreateFocusRecord(NULL, widget);
      return(finfo->focus);
    }
  return(NULL);
}
/***************************************************************/
void EZ_SetFocusTo(     EZ_Widget *widget)
{
  if(EZ_WidgetExist(widget) && EZ_GetWidgetDisabledFlag(widget)== 0 && EZ_WidgetIsViewable(widget))
    {
      EZ_FocusRecord *finfo;
      EZ_Widget *root = widget;
      
      while(EZ_WidgetParent(root)) root = EZ_WidgetParent(root);
      finfo = findOrCreateFocusRecord(root, widget);

      EZ_SetInputFocus(root, widget, finfo, 0);
    }
}
/*******************************************************************/
extern EZ_Widget *EZ_GrabedWidget;

void EZ_ResetFocus( EZ_Widget *widget, int flag)
{
  EZ_FocusRecord *finfo;
  EZ_Widget *root, *tmp;
  int reset = 1;
  if(EZ_GrabedWidget && EZ_WidgetExist(EZ_GrabedWidget) && EZ_WidgetIsViewable(EZ_GrabedWidget))
    {
      root = EZ_GrabedWidget;
      while(EZ_WidgetParent(root)) root = EZ_WidgetParent(root);
      finfo = findOrCreateFocusRecord(root, widget);
      tmp = finfo->focus;
      if(tmp != NULL && EZ_WidgetIsViewable(tmp))
	{
	  reset = 0;
	  EZ_SetInputFocus(root, tmp, finfo, flag); 
	}
    }
  if(reset)
    {
      root = widget;
      while(EZ_WidgetParent(root)) root = EZ_WidgetParent(root);
      finfo = findOrCreateFocusRecord(root, widget);
      tmp = finfo->focus;
      if(tmp != NULL && EZ_WidgetIsViewable(tmp))
	EZ_SetInputFocus(root, tmp, finfo, flag);
      else 
	{
	  /*
	   * This part is used only when pointer enters an icon/toplevel
	   * window(toplevel windows that are invisible by the window managger),
	   * and the toplevel window is supposed to take focus. We have to
	   * handle two situations.
	   *  1. the widget has children widgets and focus has previously
	   *     set to one of them. This case is handled normally above.
	   *  2. the widget is a leaf widget or no explicit focus has been
	   *     set to its children widgets. We set focus to the toplevel
	   *     in this case. The next line takes care of this situation.
	   */
	  if(flag == 100 && EZ_WidgetIsViewable(widget))
	    EZ_SetInputFocus(root, widget, finfo, 0);
	}
    }
}

/*******************************************************************/
void EZ_RestGVFocusC() 						 
{
  EZ_CurrentFocusingWidget = (EZ_Widget *)NULL;
  EZ_PendingFocus =  (EZ_Widget *)NULL;
  EZ_FocusList = (EZ_FocusRecord *)NULL;
  EZ_ClearFocusRecord();
}
/*******************************************************************/
        
static EZ_FocusRecord *findOrCreateFocusRecord( EZ_Widget *root, EZ_Widget *widget)
{
  EZ_FocusRecord *ptr= EZ_FocusList;
  EZ_Widget *toplevel = root;

  if(toplevel == NULL && EZ_WidgetExist(toplevel))
    {
      toplevel = widget;
      while(EZ_WidgetParent(toplevel))
	toplevel= EZ_WidgetParent(toplevel);
    } else { toplevel = NULL;}

  while(ptr != NULL)
    {
      if(ptr->toplevel == toplevel) return(ptr);
      else ptr = ptr->next;
    }
  /* so we don't have it, create a new entry */
  ptr = (EZ_FocusRecord *)my_malloc(sizeof(EZ_FocusRecord),_FOCUS_RECORD_);
  ptr->toplevel = toplevel;
  ptr->focus = NULL;
  ptr->next = EZ_FocusList;
  EZ_FocusList = ptr;

  return(ptr);
}
/*******************************************************************/
void EZ_ClearFocusRecord(void)
{
  EZ_FocusRecord *tmp, *ptr= EZ_FocusList;
  while(ptr != NULL)
    {
      tmp = ptr;
      ptr = ptr->next;
      (void)my_free(tmp);
    }
  EZ_FocusList = NULL;
  EZ_WeAreInFocus = 0;
}
EZ_Widget *EZ_GetFocus()
{
  Window fw; int rt;
  XSync(EZ_Display, False);      
  XGetInputFocus(EZ_Display, &fw, &rt);
  return(EZ_LookupWidgetFromMappedHT(fw));
}
/*******************************************************************/
int  EZ_WidgetIsFocusable( EZ_Widget *widget)
{ if(widget) return((EZ_GetWidgetFocusableFlag(widget))); return(0);}
/*******************************************************************/
#undef _EZ_FOCUS_C_
