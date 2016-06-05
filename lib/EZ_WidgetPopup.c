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
 ***              Widget Popup Menu                            ***
 ***                                                           ***
 *****************************************************************/
#define _EZ_WIDGET_POPUP_C_

#include "EZ_Widget.h"

/*********************************************************************
 * 
 *  Functions implemented in this file:
 */
EZ_Widget        *EZ_CreatePopupMenu MY_ANSIARGS((char *title));
EZ_Widget        *EZ_CreateMenu MY_ANSIARGS((char *title));
EZ_Widget        *EZ_CreatePopupMenuInternal MY_ANSIARGS((char *title));
void              EZ_ComputeWidgetPopupSize MY_ANSIARGS((EZ_Widget *widget, int *w, int *h));
void              EZ_FreePopupData MY_ANSIARGS((EZ_Widget *widget));

EZ_Widget        *EZ_GetPopupMenuTitleWidget MY_ANSIARGS((EZ_Widget *widget));
EZ_Widget         *EZ_GetSelectedMenuItem MY_ANSIARGS((EZ_Widget *widget));
int               EZ_DoPopup MY_ANSIARGS((EZ_Widget *widget, int where));
int               EZ_DoPopupAt MY_ANSIARGS((EZ_Widget *widget, int x, int y));
int               EZ_DoPopupX MY_ANSIARGS((EZ_Widget *widget, EZ_Widget *menu, int where));
int               EZ_DoPopupAtX MY_ANSIARGS((EZ_Widget *widget, EZ_Widget *menu, int x, int y));

void              EZ_SetMenuSetupProcedure MY_ANSIARGS((EZ_Widget *menu, EZ_CallBack p, void *data));

void              EZ_DisplayPopup MY_ANSIARGS((EZ_Widget *widget, int x, int y));
void              EZ_DisplayPopupSubMenuOf MY_ANSIARGS((EZ_Widget *widget));
void              EZ_HideCurrentPopupMenu MY_ANSIARGS((void));
void              EZ_HideCurrentPopupMenuA MY_ANSIARGS((void));
void              EZ_HidePopupMenu MY_ANSIARGS((EZ_Widget *widget));
void              EZ_HidePopupMenuA MY_ANSIARGS((EZ_Widget *widget));
void              EZ_HideSubMenuOf MY_ANSIARGS((EZ_Widget *widget));
void              EZ_HideSubMenuOfA MY_ANSIARGS((EZ_Widget *widget));

void              EZ_HandlePopupCallBack MY_ANSIARGS((EZ_Widget *widget));
void              EZ_PopupEventHandle MY_ANSIARGS((EZ_Widget *widget, XEvent *event));
EZ_Widget         *EZ_FindMenuItemOnMenu MY_ANSIARGS((EZ_Widget *menu, EZ_Widget *item, int dir));
EZ_Widget         *EZ_MoveMenuSelection MY_ANSIARGS((EZ_Widget *menu, EZ_Widget *item, int dir));
void               EZ_SetupPopup  MY_ANSIARGS((EZ_Widget *menu));
void              EZ_PropagateBgInfo2Widget MY_ANSIARGS((EZ_Widget *widget, EZ_Widget *menu));
EZ_Widget         *EZ_CreateMenuFromList MY_ANSIARGS((EZ_MenuEntry *list));
/*********************************************************************
 *
 *  Global variables.
 */
EZ_Widget *EZ_CurrentPopupMenu = (EZ_Widget *)NULL;

/*********************************************************************
 * 
 *  Local Variables.
 */
static EZ_WidgetHandle EZ_PopupMenuHandle =
{ 
  EZ_ComputeWidgetPopupSize,
  EZ_DrawWidgetUnknown,
  EZ_FreePopupData,
  EZ_PopupEventHandle,
};
static void wait_for_selection MY_ANSIARGS((EZ_Widget *widget));
/*********************************************************************/

/* 12-28-97 */

void EZ_FreePopupData(widget) EZ_Widget *widget;
{
  if(EZ_PopupIsTearOff(widget) == 0)
    {
      EZ_Widget *other = EZ_PopupCompanion(widget);
      EZ_PopupCompanion(widget) = NULL; 
      if(other && other == EZ_LookupWidgetFromAllHT(other))
	{
	  EZ_PopupCompanion(other) = NULL;
	  EZ_DestroyWidget(other);
	}
    }
}
/*********************************************************************/

void EZ_PopupEventHandle(widget, event)
     EZ_Widget *widget;
     XEvent          *event;
{
  if(widget == (EZ_Widget *)NULL)   return;

  if(event->type == Expose)  EZ_DrawWidget(widget);      
  else  if(event->type == EnterNotify)
    {
      if(EZ_WidgetParent(widget) == NULL && EZ_WidgetMapped(widget))
	{
	  if(event->xcrossing.focus == False) EZ_SetFocusTo(widget);
	}
    }
  else if(event->type == KeyPress || (event->type == ButtonPress &&
				      event->xbutton.button == EZ_Btn1))
    EZ_HandleToplevelMenuEvents(widget, event);   
}
/**********************************************************************
 *
 *   popup a menu.
 */
int EZ_DoPopup( widget, where)
     EZ_Widget *widget; int where;
{ return(EZ_DoPopupX(NULL, widget, where)); }

int EZ_DoPopupX(initiator, widget, where)
     EZ_Widget *initiator, *widget; int where;
{
  EZ_Widget *the_popup;

  if(widget == (EZ_Widget *)NULL ||
     EZ_WidgetType(widget) != EZ_WIDGET_POPUP_MENU) 
    return(0);

  /*-------------------------------------------------
   * If there is an active popup menu, stop.
   *------------------------------------------------*/
  if(EZ_CurrentPopupMenu != (EZ_Widget *)NULL)
    return(0);

  EZ_PopupInitiator(widget) = NULL;
  EZ_PopupXoff(widget) = 0;
  EZ_PopupYoff(widget) = 0;
  EZ_SetupPopup(widget);
  if(initiator) EZ_PropagateBgInfo2Widget(initiator, widget);

  /*--------------------
   * Now do the popup.
   *-------------------*/
  {
    int             rx,ry,x,y;
    unsigned int    mask;
    Window          root,win;
    int             ww,hh;
    
    XQueryPointer(EZ_Display, RootWindow(EZ_Display, EZ_ScreenNum),
		                                            /* dpy and win     */
		  &root,                                    /* root return win */
		  &win,                                     /* child ret win   */
		  &rx, &ry,                                 /* x, y in root    */
		  &x,&y,                                    /* x, y in win     */
		  &mask
		  );

    if( !(EZ_GetWidgetSizeComputedFlag(widget)) )
      {
	EZ_MarkAllChildrenWidgetSizeChanged(widget,0);
	EZ_ComputeWidgetWindowSize(widget, &ww, &hh, NULL);
      }
    else
      {
	ww = EZ_WidgetWidth(widget);
	hh = EZ_WidgetHeight(widget);
      }
    switch(where)
      {
      case EZ_RIGHT:
	ry -= (hh/2);
	break;
      case EZ_LEFT:
	rx -= ww;
	ry -= (hh/2);
	break;
      case EZ_BOTTOM:
	rx -= (ww/2);
	break;
      case EZ_TOP:
	rx -= (ww/2);
	ry -= hh;
	break;
      case EZ_BOTTOM_LEFT:
	rx -= ww;
	break;
      case EZ_TOP_RIGHT:
	ry -= hh;
	break;
      case EZ_TOP_LEFT:
	ry -= hh;
	rx -= ww;
	break;
      case EZ_BOTTOM_RIGHT:
	break;
      default:
	{
	  EZ_Widget *child = EZ_WidgetChildren(widget);
	  if(child && EZ_WidgetType(child) == EZ_WIDGET_LABEL)
	    { ry -= EZ_WidgetHeight(child) + 4; }
	}
	break;
      }

    if(rx < 0) rx = 0;
    else if(rx + ww > EZ_XDisplayWidth)  rx = (int)EZ_XDisplayWidth - ww;
    if(ry < 0) ry = 0;
    else if(ry + hh > EZ_XDisplayHeight) ry = (int)EZ_XDisplayHeight - hh;

    EZ_DisplayPopup(widget,rx,ry); /* this call initializes the ret/link data also */
  }
  the_popup = EZ_CurrentPopupMenu;
  wait_for_selection(the_popup);
  EZ_HideCurrentPopupMenu();
  EZ_HandlePopupCallBack(the_popup);
  return(EZ_WidgetRetData(the_popup));
}

/*****************************************************************/
int EZ_DoPopupAt(widget, x0,y0) EZ_Widget *widget; int x0,y0;
{ return(EZ_DoPopupAtX(NULL, widget, x0,y0));}

int EZ_DoPopupAtX(initiator, widget, x0,y0)
     EZ_Widget *initiator, *widget; int x0,y0;
{
  EZ_Widget *the_popup;

  if(widget == (EZ_Widget *)NULL ||
     EZ_WidgetType(widget) != EZ_WIDGET_POPUP_MENU) 
    return(0);

  /*-------------------------------------------------
   * If there is an active popup menu, stop.
   *------------------------------------------------*/
  if(EZ_CurrentPopupMenu != (EZ_Widget *)NULL)
    return(0);

  EZ_PopupInitiator(widget) = NULL;
  EZ_PopupXoff(widget) = 0;
  EZ_PopupYoff(widget) = 0;
  EZ_SetupPopup(widget);
  if(initiator) EZ_PropagateBgInfo2Widget(initiator, widget);
  /*--------------------
   * Now do the popup.
   *-------------------*/
  {
    int             rx,ry;
    int             ww,hh;

    if( !(EZ_GetWidgetSizeComputedFlag(widget)) )
      {
	EZ_MarkAllChildrenWidgetSizeChanged(widget,0);
	EZ_ComputeWidgetWindowSize(widget, &ww, &hh, NULL);
      }
    else
      {
	ww = EZ_WidgetWidth(widget);
	hh = EZ_WidgetHeight(widget);
      }
    rx = x0; 
    ry = y0; 
    if(rx < 0) rx = 0;
    else if(rx + ww > EZ_XDisplayWidth)  rx = (int)EZ_XDisplayWidth - ww;
    if(ry < 0) ry = 0;
    else if(ry + hh > EZ_XDisplayHeight) ry = (int)EZ_XDisplayHeight - hh;
    EZ_DisplayPopup(widget,rx,ry);
  }
  the_popup = EZ_CurrentPopupMenu;
  wait_for_selection(the_popup);
  EZ_HideCurrentPopupMenu();
  EZ_HandlePopupCallBack(the_popup);
  return(EZ_WidgetRetData(the_popup));
}

/*-----------------------------------------------------
 * Handle popup callback.
 * The last selected item, if any, is propagated to
 * EZ_PopupLSItem(the_popup).
 *  If it has a callback, do it first.
 *  If its parent has a callback, do it the second.
 *
 *  The real reason for doing callbacks here is that 
 *  we want to hide the popup menu first before calling
 *  the callback (in case the callback takes a while).
 *-----------------------------------------------------*/
void EZ_HandlePopupCallBack(the_popup)
     EZ_Widget *the_popup;
{
  EZ_Widget *item, *parent;

  if(the_popup != (EZ_Widget *)NULL)
    {
      item = EZ_PopupLSItem(the_popup);
      if(item != (EZ_Widget *)NULL)
	{
	  int pp = 0;
	  parent = EZ_WidgetParent(item);
	  if(parent && EZ_WidgetType(parent) == EZ_WIDGET_POPUP_MENU)
	    {
	      EZ_Widget *cpn = EZ_PopupCompanion(parent);
	      if(cpn && EZ_WidgetMapped(cpn))
		{
		  int ms = EZ_WidgetMapped(cpn);
		  EZ_WidgetMapped(cpn) = 5; /* don't raise the window */
		  EZ_DisplayWidget(cpn);
		  EZ_WidgetMapped(cpn) = ms;
		}
	      pp = 1;
	    }
	  { EZ_ExecuteWidgetCallBacks(item);} /* call back of selected item, if any.*/
	  if(pp && EZ_WidgetCallBackFunc(parent) != NULL)
	    {
	      EZ_PopupLSItem(parent) = item;
	      EZ_WidgetRetData(parent) = EZ_WidgetRetData(item);
	      { EZ_ExecuteWidgetCallBacks(parent);}
	    }
	} 
    }
}
/**********************************************************************
 *
 *  Display a menu.
 */
void  EZ_DisplayPopup(widget,x,y)
     EZ_Widget *widget;  /* the popup menu    */
     int             x,y;      /* position to popup */
{
  if(widget == (EZ_Widget *)NULL) return;
  if(EZ_WidgetType(widget) != EZ_WIDGET_POPUP_MENU) return;

  EZ_CurrentPopupMenu = widget;   

  EZ_WidgetOriginX(widget) = x;
  EZ_WidgetOriginY(widget) = y;
  /*-------------------------------
   * Initial the return value.
   *------------------------------*/
  EZ_WidgetRetData(widget) = -1;   
  EZ_PopupLSItem(widget) = (EZ_Widget *)NULL;
  EZ_PopupLink1(widget) =  (EZ_Widget *)NULL;
  EZ_DisplayWidget(widget);
}

/********************************************************************
 *
 *  Display the submenu of a Menu-submenu widget
 */
void  EZ_DisplayPopupSubMenuOf(widget)
     EZ_Widget *widget;
{
  int             rx,ry,cx,cy,x,y;
  unsigned int    mask;
  Window          root, win;
  EZ_Widget       *parent, *submenu;
  
  if((widget == (EZ_Widget *)NULL) ||
     (EZ_SubMenuTheMenu(widget) == (EZ_Widget *)NULL))
    return;

  if( EZ_WidgetWindow(widget) == (Window )NULL ||  EZ_WidgetMaped(widget) == 0)
    return;

  parent = EZ_WidgetParent(widget);
  EZ_HideSubMenuOfA(parent);
  XQueryPointer(EZ_DisplayForWidgets, EZ_WidgetWindow(widget),      /* dpy and win     */
		&root,                                    /* root return win */
		&win,                                     /* child ret win   */
		&rx, &ry,                                 /* x, y in root    */
		&cx,&cy,                                  /* x, y in win     */
		&mask
		);
  /*
   * this is the position to popup the submenu
   */
  x = rx + EZ_WidgetWidth(widget) - cx;
  y = ry - cy;

  submenu = EZ_SubMenuTheMenu(widget);
  EZ_SetupPopup(submenu);
  /* first set the background ptr */
  EZ_PropagateBgInfo2Widget(widget, submenu);
  
  /* 1-7-97 */
  {
    int ww, hh;
    if( !(EZ_GetWidgetSizeComputedFlag(submenu)) )
      {
	EZ_MarkAllChildrenWidgetSizeChanged(submenu,0);
	EZ_ComputeWidgetWindowSize(submenu, &ww, &hh, NULL);
      }
    else
      {
	ww = EZ_WidgetWidth(submenu);
	hh = EZ_WidgetHeight(submenu);
      }
    if(x + ww > EZ_XDisplayWidth)  x = (int)EZ_XDisplayWidth - ww;
    if(y + hh > EZ_XDisplayHeight) y = (int)EZ_XDisplayHeight - hh;
  }
  EZ_WidgetOriginX(submenu) = x;
  EZ_WidgetOriginY(submenu) = y;
  EZ_PopupInitiator(submenu) = parent;
  EZ_PopupXoff(submenu) =  x - (EZ_WidgetOriginX(parent));
  EZ_PopupYoff(submenu) =  y - (EZ_WidgetOriginY(parent));
  EZ_DisplayWidget(submenu);
}

void EZ_HideCurrentPopupMenu()
{
  EZ_HidePopupMenu(EZ_CurrentPopupMenu);
  EZ_CurrentPopupMenu = (EZ_Widget *)NULL;
  XSync(EZ_Display, False);
}

void EZ_HideCurrentPopupMenuA()
{
  EZ_HidePopupMenuA(EZ_CurrentPopupMenu);
  EZ_CurrentPopupMenu = (EZ_Widget *)NULL;
  XSync(EZ_Display, False);
}

/**********************************************************
 *
 *  Hide a popup menu and all poped submenu by menu.
 */

void EZ_HidePopupMenu(menu)
     EZ_Widget *menu;
{
  if((menu == (EZ_Widget *)NULL) ) return;

  EZ_HideSubMenuOf(menu);
  EZ_WidgetParentBG(menu) = NULL;
  if(EZ_WidgetMaped(menu) != 0)
    {
      EZ_WidgetMaped(menu) = 0;
      XUnmapWindow(EZ_DisplayForWidgets, EZ_WidgetWindow(menu));
    }
  EZ_PopupLink1(menu) =  (EZ_Widget *)NULL;
  {
    EZ_Widget *child = EZ_WidgetChildren(menu);
    while(child)
      {
	if(EZ_WidgetType(child) != EZ_WIDGET_LABEL)
	  {
	    EZ_WidgetBorderStyle(child) = EZ_BORDER_NONE; 
	    EZ_ClearWidgetHighlightFlag(child);
            { EZ_UninstallHighlightColor(child); } /*4/29/99*/
	  }
	child = EZ_WidgetSibling(child);
      }
  }
}

void EZ_HidePopupMenuA(menu)
     EZ_Widget *menu;
{
  if((menu == (EZ_Widget *)NULL) ) return;

  EZ_HideSubMenuOfA(menu);
  EZ_WidgetParentBG(menu) = NULL;
  if(EZ_WidgetMaped(menu) != 0)
    {
      EZ_WidgetMaped(menu) = 0;
      XUnmapWindow(EZ_DisplayForWidgets, EZ_WidgetWindow(menu));
    }
  EZ_PopupLink1(menu) =  (EZ_Widget *)NULL;
  {
    EZ_Widget *child = EZ_WidgetChildren(menu);
    while(child)
      {
	if(EZ_WidgetType(child) != EZ_WIDGET_LABEL)
	  {
	    EZ_ClearWidgetHighlightFlag(child);
            { EZ_UninstallHighlightColor(child); } /*4/29/99*/
	    EZ_WidgetBorderStyle(child) = EZ_BORDER_NONE;
	  }
	child = EZ_WidgetSibling(child);
      }
  }
}

/*******************************************************
 *
 *  Hide all submenus of menu.
 */
void EZ_HideSubMenuOf(menu)
     EZ_Widget *menu;
{
  EZ_Widget  *children;

  if((menu == (EZ_Widget *)NULL) ) return;

  children = EZ_WidgetChildren(menu);
  while(children)
    {
      if(EZ_WidgetType(children) == EZ_WIDGET_MENU_SUBMENU)
	EZ_HidePopupMenu(EZ_SubMenuTheMenu(children));
      children = EZ_WidgetSibling(children);
    }
}
void EZ_HideSubMenuOfA(menu)
     EZ_Widget *menu;
{
  EZ_Widget  *children;

  if((menu == (EZ_Widget *)NULL) ) return;

  children = EZ_WidgetChildren(menu);
  while(children)
    {
      int type = EZ_WidgetType(children);
      if(type != EZ_WIDGET_LABEL)
	{
	  EZ_ClearWidgetHighlightFlag(children);
          { EZ_UninstallHighlightColor(children); } /*4/29/99*/
	  EZ_WidgetBorderStyle(children) = EZ_BORDER_NONE;      
	}
      if(type == EZ_WIDGET_MENU_SUBMENU)
	EZ_HidePopupMenuA(EZ_SubMenuTheMenu(children));
      children = EZ_WidgetSibling(children);
    }
}

/*********************************************************************
 *
 *  A popup menu is always a toplevel widget.
 */
EZ_Widget  *EZ_CreateMenu(title)
     char *title;
{
  return(EZ_CreatePopupMenu(title));
}

EZ_Widget  *EZ_CreatePopupMenu(title)
     char *title;
{
  EZ_Widget *menu, *toBar;
  menu = EZ_CreatePopupMenuInternal(title);
  toBar = EZ_CreateMenuTearOffItem(menu);
  /*
  EZ_WidgetGBX(toBar) = 0; 
  EZ_WidgetGBY(toBar) = 1;
  EZ_WidgetGBW(toBar) = 16;
  EZ_WidgetGBH(toBar) = 1;
  EZ_WidgetGBFill(toBar)  = EZ_FILL_HORIZONTALLY;
  */
  /* EZ_SetWidgetNonActiveFlag(toBar); */
  return(menu);
}
EZ_Widget  *EZ_CreatePopupMenuInternal(title)
     char *title;
{
  EZ_Widget  *tmp;

  tmp = EZ_CreateNewWidget((EZ_Widget *)NULL);
  /*--------------------------------------------------
   * Register the handling functions for CButton.
   *  has to be done after  EZ_CreateNewWiget.
   *-------------------------------------------------*/
  EZ_WidgetHandlingFunctions[EZ_WIDGET_POPUP_MENU] = &EZ_PopupMenuHandle;

  EZ_WidgetType(tmp) = EZ_WIDGET_POPUP_MENU;
  EZ_SetWidgetTypeAndNames(tmp, EZ_WIDGET_POPUP_MENU);
  /* create the title label */
    {
      EZ_Widget *titlewidget;
      titlewidget = EZ_CreateLabel(tmp,title);
      EZ_GetAndUseWidgetResources(titlewidget);
      EZ_LabelAlwaysHighlight(titlewidget) = 1;
      EZ_WidgetBorderStyle(titlewidget) = EZ_BORDER_FRAMED_DOWN;
      EZ_WidgetBorderWidth(titlewidget) = 2;
      EZ_WidgetPadY(titlewidget) = 2;
      EZ_LabelFont(titlewidget) = EZ_GetFontFromId(EZ_MENU_TITLE_FONT);
      EZ_LabelIsMenuTitle(titlewidget) = 1;
      if(title == (char *)NULL) EZ_SetWidgetNonActiveFlag(titlewidget);
      /*
      EZ_WidgetGBX(titlewidget) = 0;
      EZ_WidgetGBY(titlewidget) = 0;
      EZ_WidgetGBW(titlewidget) = 16;
      EZ_WidgetGBH(titlewidget) = 1;
      EZ_WidgetGBFill(titlewidget)  = EZ_FILL_HORIZONTALLY;
      */
      EZ_SetRowColMaxSize(tmp, EZ_XDisplayHeight-100);
      EZ_GMType(EZ_WidgetGManager(tmp)) = EZ_COLUMN_GEOM_MANAGER;
    }
  EZ_WidgetPadX(tmp) = 0;
  EZ_WidgetPadY(tmp) = 0; 
  EZ_WidgetSepX(tmp) = 8;
  EZ_WidgetStacking(tmp)  = EZ_VERTICAL;
  EZ_WidgetFillMode(tmp) = EZ_FILL_BOTH;
  EZ_WidgetAlignment(tmp)= EZ_LEFT_ALIGNED;
  EZ_WidgetLabelPosition(tmp) = EZ_LEFT;
  EZ_WidgetBorderWidth(tmp)  = 2;
  EZ_WidgetBorderStyle(tmp)  = EZ_BORDER_UP;
  EZ_WidgetCursor(tmp) = EZ_GetCursor(EZ_C_RIGHT_PTR);

  EZ_PopupLSItem(tmp) = (EZ_Widget *)NULL;
  EZ_PopupIsTearOff(tmp) = 0;
  EZ_PopupIsTearLink(tmp) = (EZ_Widget *)NULL;
  EZ_PopupCompanion(tmp) =  (EZ_Widget *)NULL;
  EZ_PopupLink1(tmp) = NULL;
  EZ_PopupLink2(tmp) = NULL;

  EZ_SetWidgetTransientFlag(tmp); 
  EZ_ClearWidgetFocusableFlag(tmp);

  EZ_PopupSetupF(tmp) = NULL;
  EZ_PopupSetupFD(tmp) = NULL;
  return(tmp);
}

/******************************************************
 *
 *  A popup menu need no space for private things.
 */
void  EZ_ComputeWidgetPopupSize(widget, w,h)
     EZ_Widget *widget;
     int             *w, *h;
{
  *w = 0;
  *h = 0;
}

/*************************************************************************/

EZ_Widget *EZ_GetSelectedMenuItem(widget)
     EZ_Widget *widget;
{
  if(widget == (EZ_Widget *)NULL ||
     (EZ_WidgetType(widget) != EZ_WIDGET_POPUP_MENU &&
      EZ_WidgetType(widget) != EZ_WIDGET_MENU_BUTTON))
    return((EZ_Widget *)NULL);
  if(EZ_WidgetType(widget) == EZ_WIDGET_POPUP_MENU)
    return(EZ_PopupLSItem(widget));
  else
    return(EZ_MButtonLSItem(widget));
}
/******************************************************************************************/
void EZ_ResetGVPopupC()
{
  EZ_CurrentPopupMenu = (EZ_Widget *)NULL;
}

/******************************************************************************************
 *
 *               interface for building simple menus.
 *
 ******************************************************************************************/

typedef struct simpleMenu_ {
  char label[256];
  char cname[128];
  char iname[128];
  char shortcut[128];
  EZ_CallBack callBack;
  void *cdata;
  void *subMenu;
  void *pdata;
  int  idata;
  int  retValue;    
  int  isSeparator;
  int  underline;
  int  btype;
  int  v1,v2,v3;
  struct simpleMenu_ *next;
} simpleMenu, simpleMenuItem;

/*********************************************************************/
static  simpleMenu *newSimpleMenu MY_ANSIARGS((void));
/**************************************************************/
static simpleMenu *newSimpleMenu()
{
  simpleMenu *tmp = (simpleMenu *)my_malloc(sizeof(simpleMenu), _SIMPLE_MENU);
  if(tmp == (simpleMenu *)NULL)
    exit(0);
  (tmp->label)[0] = 0;
  (tmp->cname)[0] = 0;
  (tmp->iname)[0] = 0;
  (tmp->shortcut)[0] = 0;
  (tmp->callBack) = NULL;
  (tmp->cdata) = NULL;
  (tmp->pdata) = NULL;
  (tmp->idata) = 0;
  (tmp->subMenu) = NULL;
  (tmp->retValue) = 0;
  (tmp->isSeparator) = 0;
  (tmp->underline) = -1;
  (tmp->btype) = 0;
  (tmp->next) = NULL;
  return(tmp);
}
/**************************************************************/
static int extractId(str, ret) char *str, *ret;
{
  int i = 0, count = 0, c;
  /* skip white spaces */
  while(str[i] && (str[i] == ' ' || str[i] =='\t')) i++;

  for(c = str[i]; c != 0 && c != '%' && c != '|'; )
    {
      if(count < 127) { ret[count] = str[i]; count++;}
      i++;
      c = str[i];
    }
  /* strip out tailing white spaces */
  while(count > 0 && (ret[count-1] == ' ' || ret[count-1] == '\t')) count--;
  ret[count] = 0;
  /* printf("Id is [%s]\n",ret);*/
  return(i);
}
/**************************************************************/
EXTERN void EZ_SetKeyShortcut MY_ANSIARGS((EZ_Widget *widget, char *str));
/**************************************************************/
static EZ_Widget *makeMenuFromDescription(sMenu) simpleMenu *sMenu;
{
    simpleMenu  *tmp1, *tmp = sMenu;
    EZ_Widget   *menu = NULL, *item;
    
    menu = EZ_CreateMenu((tmp->label)[0] == 0? NULL : tmp->label);
    EZ_SetWidgetCINames(menu, tmp->cname, tmp->iname);
    EZ_GetAndUseWidgetResources(menu);    
    EZ_SetWidgetClientData(menu, tmp->idata, tmp->pdata);

    if(tmp->callBack)
      EZ_AddWidgetCallBack(menu, EZ_CALLBACK,tmp->callBack,tmp->cdata, 0);
    tmp1 = tmp->next;
    (void)my_free((char *)tmp);
    tmp = tmp1;
    while(tmp)
      {
	if(tmp->next) /* the last node is useless */
	  {
	    if(tmp->isSeparator)
	      {
		item = EZ_CreateMenuSeparator(menu);
		EZ_SetWidgetCINames(item, tmp->cname, tmp->iname);
		EZ_GetAndUseWidgetResources(item);
	      }
	    else
	      {
		if(tmp->subMenu)
		  {
		    if((tmp->label)[0] == 0)
		      sprintf(tmp->label, "Submenu %d", tmp->retValue);
		    item = EZ_CreateMenuSubMenu(menu, tmp->label, tmp->underline);
		    if(tmp->shortcut[0] != 0) EZ_SetKeyShortcut(item, tmp->shortcut);
		    EZ_SetWidgetCINames(item, tmp->cname, tmp->iname);
		    EZ_GetAndUseWidgetResources(item);
		    EZ_SetSubMenuMenu(item, tmp->subMenu);
		    EZ_SetWidgetClientData(item, tmp->idata, tmp->pdata);
		  }
		else
		  {
		    if((tmp->label)[0] == 0)
		      sprintf(tmp->label, "Item %d", tmp->retValue);
                    if(tmp->btype == EZ_WIDGET_MENU_RADIO_BUTTON)
                      {
                        item = EZ_CreateMenuRadioButton(menu,tmp->label, tmp->underline,
                                                        tmp->v1, tmp->v2, tmp->retValue);
                        EZ_SetRadioButtonGroupVariableValue(item, tmp->v3);
                      }
                    else if(tmp->btype == EZ_WIDGET_MENU_CHECK_BUTTON)
                      item = EZ_CreateMenuCheckButton(menu,tmp->label,tmp->underline,
                                                      tmp->v1, tmp->v2, tmp->v3,
                                                      tmp->retValue);
                    else
                      item = EZ_CreateMenuNormalButton(menu,tmp->label,
                                                       tmp->underline, tmp->retValue);
		    if(tmp->shortcut[0] != 0) EZ_SetKeyShortcut(item, tmp->shortcut);
		    EZ_SetWidgetCINames(item, tmp->cname, tmp->iname);
		    EZ_GetAndUseWidgetResources(item);
		    if(tmp->callBack)
		      EZ_AddWidgetCallBack(item,EZ_CALLBACK,tmp->callBack, tmp->cdata, 0);
		    EZ_SetWidgetClientData(item, tmp->idata, tmp->pdata);
		  }
	      }
	  }
	tmp1 = tmp->next;
	(void)my_free((char *)tmp);
	tmp = tmp1;
      }
    return(menu);
}

/**************************************************************/
EZ_Widget *EZ_CreateSimpleMenu MY_VARARGS(char *,  the_args)
     MY_VARARGS_DECL
{
  int          i, itemCount, bar;
  char         *str;
  simpleMenu   *sMenu, *sMenuItem;
  EZ_CallBack  callBack;
  void         *cdata;
  void         *subMenu;
  int          retValue;    
  int          isSeparator;
  int          underline;
  void         *pdata;
  int          idata;
  va_list      ap;

  sMenu = newSimpleMenu();
  sMenu->next = sMenuItem = newSimpleMenu();
  str = (char *)(MY_VARARGS_START(char *, the_args, ap));

  for(bar=0, itemCount = 1, i = 0; str[i]; )
    {
      char buf[256];
      int isTitle = 0;
      int increment = 1;
      int bufCount = 0;
      int cmdset = 0;
      int tstrset = 0;
      int btype = 0;
      int v1=0, v2=0, v3=0;
      callBack = NULL;
      cdata = NULL;
      subMenu = NULL;
      retValue = itemCount;
      isSeparator = 0;
      underline = -1;
      pdata = NULL;
      idata = 0;
      
      while( str[i] && str[i] != '|')
	{
	  if(str[i] == '%')
	    {
	      i++;
	      switch(str[i])
		{
		case 'c': /* class name */
		  i++;
		  if(bar != 0)    i += extractId(&(str[i]), sMenuItem->cname);
		  else     i += extractId(&(str[i]), sMenu->cname);
		  break;
		case 'n': /* instance name */
		  i++;
		  if(bar != 0)    i += extractId(&(str[i]), sMenuItem->iname);
		  else     i += extractId(&(str[i]), sMenu->iname);
		  break;
		case 't':
		case 'T':
		  isTitle = 1;
		  tstrset = 1;
		  i++;
		  break;
		case 'f': 
		  callBack = va_arg(ap, EZ_CallBack);
		  cdata =  va_arg(ap, void *);
		  cmdset = 1;
		  i++;
		  break;
		case 'F':
		  callBack = va_arg(ap, EZ_CallBack);
		  cdata =  va_arg(ap, void *);
		  cmdset = 1;
		  isTitle = 1;
		  i++;
		  break;
		case 'm':
		  subMenu =  va_arg(ap, void *);
		  i++;
		  break;
		case 'x':
		  {
		    char *nptr, *endptr;

		    i++;
		    nptr = str + i;
		    retValue = (int)strtol(nptr, &endptr, 0);
		    i += endptr - nptr;
		  }
		  break;
		case 'u':
		  {
		    char *nptr, *endptr;

		    i++;
		    nptr = str + i;
		    underline = (int)strtol(nptr, &endptr, 0);
		    i += endptr - nptr;
		    /*printf("underline = %d\n", underline);*/
		  }
  		  break;
		case 's':
		  i++;
		  i += extractId(&(str[i]), sMenuItem->shortcut);
		  /*printf("shortcut = %s\n", sMenuItem->shortcut);*/
		  break;
		case 'l':
		  isSeparator = 1;
		  increment = 0;
		  i++;
		  break;
		case 'i':
		  idata = va_arg(ap, int);
		  break;
		case 'p':
		  pdata = va_arg(ap, void *);
		  break;
                case 'r':
                case 'k':
                  {
		    char *nptr, *endptr;
                    char cc = str[i];
		    i+=2;
		    nptr = str + i;
		    v1 = (int)strtol(nptr, &endptr, 0);
		    i += endptr - nptr;
                    i++;
		    nptr = str + i;
		    v2 = (int)strtol(nptr, &endptr, 0);
		    i += endptr - nptr;                    
                    i++;
		    nptr = str + i;
		    v3 = (int)strtol(nptr, &endptr, 0);
		    i += endptr - nptr;                    
                    i++;
                    if(cc == 'r') btype = EZ_WIDGET_MENU_RADIO_BUTTON;
                    else btype = EZ_WIDGET_MENU_CHECK_BUTTON;
                  }
                break;
		default:
		  if(bufCount < 255)
		    {
		      buf[bufCount++] = '%';	
		      buf[bufCount++] = str[i];	
		    } 
		  i++;
		  break;
		}
	    }
	  else
	    {
	      if(bufCount < 256)
		buf[bufCount++] = str[i];
	      i++;
	    }
	}
      if(str[i] == '|') { i++; bar++;}
      buf[bufCount] = '\0';
      /* now record this item */

      if(isTitle == 0)
	{
	  int i = 0; while(buf[i] == ' ' || buf[i] == '\t') i++;
	  (void)strcpy(sMenuItem->label,buf+i);
	  sMenuItem->callBack = callBack;
	  sMenuItem->cdata = cdata;
	  sMenuItem->retValue = retValue;
	  sMenuItem->subMenu = subMenu;
	  sMenuItem->isSeparator = isSeparator;
	  sMenuItem->underline = underline;
	  sMenuItem->btype = btype;
	  sMenuItem->v1 = v1;
	  sMenuItem->v2 = v2;
	  sMenuItem->v3 = v3;

	  sMenuItem->pdata = pdata;
	  sMenuItem->idata = idata;
	  
	  itemCount += increment;
	  sMenuItem->next = newSimpleMenu();
	  sMenuItem = sMenuItem->next;
	}
      else /*  title and callback */
	{
	  if(tstrset) (void)strcpy(sMenu->label, buf);
	  if(cmdset)
	    {
	      sMenu->callBack = callBack;
	      sMenu->cdata = cdata;
	      sMenu->pdata = pdata;
	      sMenu->idata = idata;
	    }
	}
    }
  /*
   * Done parsing the definitions. Now create menus.
   */
  return(makeMenuFromDescription(sMenu));
}
/*******************************************************************************************/
static EZ_Widget *makeMenuFromList(list, forward) EZ_MenuEntry *list; int *forward;
{
  int count = 0;
  EZ_MenuEntry *tlist = list;
  simpleMenu   *sMenu, *sMenuItem;
  if(tlist == NULL || tlist->type == 0) { *forward = 0; return(NULL);}
  sMenu = newSimpleMenu();
  sMenuItem = sMenu;
  while(tlist && tlist->type != 0)
    {
      sMenuItem->next = newSimpleMenu();
      sMenuItem = sMenuItem->next;
      
      sMenuItem->callBack = tlist->callback;
      sMenuItem->cdata = tlist->clientData;
      sMenuItem->retValue = tlist->returnValue;
      sMenuItem->isSeparator = (tlist->type == EZ_WIDGET_MENU_SEPARATOR);
      sMenuItem->underline = tlist->underline;
      sMenuItem->btype = tlist->type;
      sMenuItem->v1 = tlist->v1;
      sMenuItem->v2 = tlist->v2;
      sMenuItem->v3 = tlist->v3;
      if(tlist->name)  strcpy(sMenuItem->iname, tlist->name);
      if(tlist->label) strcpy(sMenuItem->label, tlist->label);
      if(tlist->accelerator) strcpy(sMenuItem->shortcut, tlist->accelerator);
      if(tlist->type == EZ_WIDGET_MENU_SUBMENU)
        {
          int itmp = 0;
          tlist++; count++;
          sMenuItem->subMenu = makeMenuFromList(tlist, &itmp);
          count += itmp;
          tlist += itmp;
        }
      else
        {
          sMenuItem->subMenu = NULL;
          tlist++; count++;
        }
    }
  *forward = count+1;
  return(makeMenuFromDescription(sMenu));
}
EZ_Widget *EZ_CreateMenuFromList(list) EZ_MenuEntry *list;
{
  int a=0;
  return(makeMenuFromList(list, &a));
}
/*******************************************************************************************/
extern void EZ_list_items MY_ANSIARGS(( EZ_Widget *place[], EZ_Widget *menu, int *cidx));
extern int EZ_find_item_in_array MY_ANSIARGS(( EZ_Widget *ilst[], EZ_Widget *item, int bd));

static void wait_for_selection(the_popup)
     EZ_Widget *the_popup;
{
  EZ_Widget    *cmenu, *sitem, *items[1024];
  Window       parent_window;
  int          nitems = 0;

  if(the_popup == NULL) return;
  parent_window = EZ_WidgetWindow(the_popup);
  EZ_list_items(items, the_popup, &nitems);
  cmenu = the_popup;

  XGrabPointer(EZ_Display,
	       parent_window,
	       True,
	       ButtonPressMask|ButtonReleaseMask|ButtonMotionMask|PointerMotionMask,
	       GrabModeAsync,
	       GrabModeAsync, 
	       None,
	       EZ_GetCursor(EZ_C_RIGHT_PTR),
	       CurrentTime);
  XGrabKeyboard(EZ_Display,
		parent_window,
		True,
		GrabModeAsync,
		GrabModeAsync, 
		CurrentTime);
  /* wait for a selection */
  {
    XEvent        xevent;
    Window        event_window;
    EZ_Widget     *tmp, *shortcutItem, *out, *CurrentItem;
    int           done = 0;
    
    CurrentItem = NULL;
    while(!done)
      {
	sitem = NULL;  shortcutItem=NULL; out=NULL;
	EZ_TFEvents();
	XNextEvent(EZ_Display, &xevent);
	EZ_FilterEvent(&xevent);
        EZ_InvokePrivateEventHandler(&xevent);
	event_window = xevent.xany.window;
	tmp = EZ_LookupWidgetFromMappedHT(event_window);

	/*---------------------------------------------------------
	 * Handle KeyPress first.
	 *--------------------------------------------------------*/
	if(xevent.type == KeyPress)
	  {
#define TEMP_BUFFER_SIZE    32
	    int               count,modifiers;
	    KeySym            keysym;
	    XComposeStatus    compose; 
	    char              tmpbuffer[TEMP_BUFFER_SIZE];
	    int               buffersize = TEMP_BUFFER_SIZE;
	    int               x_move, y_move, kx, ky;
#undef TEMP_BUFFER_SIZE
	    shortcutItem = NULL;
	    out = NULL;
	    modifiers = xevent.xkey.state & (ShiftMask | ControlMask | Mod1Mask);
    
	    xevent.xkey.state &= ~modifiers;
	    count = XLookupString(&(xevent.xkey), tmpbuffer, buffersize, &keysym, &compose);
	    tmpbuffer[count] = '\0'; 
			
	    if(modifiers == 0 || count == 0 ||
	       (EZ_LookForPopupShortCuts(EZ_CurrentPopupMenu,
					 modifiers, tmpbuffer,&x_move, &y_move,
					 &shortcutItem) == 0 &&
		EZ_GetGlobalKeyEvent(modifiers, tmpbuffer, NULL, &out, &kx, &ky) == 0))
	      {
		switch(keysym)
		  {
		  case XK_Up: case XK_k:  case XK_p: case XK_K:  case XK_P: 
#ifdef XK_KP_Up
		  case XK_KP_Up:
#endif
		    CurrentItem = EZ_MoveMenuSelection(cmenu, CurrentItem, 1);
		    EZ_PopupLink2(cmenu) = CurrentItem;
		    break;
		  case XK_Down:  case XK_n: case XK_j: case XK_N: case XK_J: 
#ifdef XK_KP_Down
		  case XK_KP_Down:
#endif
		    CurrentItem = EZ_MoveMenuSelection(cmenu, CurrentItem, -1);
		    EZ_PopupLink2(cmenu) = CurrentItem;
		    break;
		  case XK_Tab: 
#ifdef XK_KP_Tab
		  case XK_KP_Tab:
#endif
		    CurrentItem = EZ_MoveMenuSelection(cmenu, CurrentItem, 
						       (modifiers&ShiftMask)==0? -1: 1);
		    EZ_PopupLink2(cmenu) = CurrentItem;
		    break;
		  case XK_Left: case XK_b:  case XK_h: case XK_B:  case XK_H: 
#ifdef XK_KP_Left
		  case XK_KP_Left:
#endif
		    {
		      EZ_Widget *mt = EZ_PopupLink1(cmenu);
		      if(mt)
			{
			  EZ_DeselectCurrentMenuItem(CurrentItem);
			  cmenu = mt;
			  CurrentItem = EZ_MoveMenuSelection(cmenu, EZ_PopupLink2(cmenu), 0);
			}
		    }
		  break;
		  case XK_Right: case XK_f: case XK_l: case XK_F: case XK_L: 
#ifdef XK_KP_Right
		  case XK_KP_Right:
#endif
		    if(CurrentItem && EZ_WidgetType(CurrentItem) == EZ_WIDGET_MENU_SUBMENU)
		      {
			EZ_Widget *tmenu = EZ_SubMenuTheMenu(CurrentItem);
			if(tmenu )
			  {
			    if(! EZ_WidgetMapped(tmenu)) EZ_DisplayPopupSubMenuOf(CurrentItem);
			    EZ_DeselectCurrentMenuItem(CurrentItem);
			    EZ_PopupLink1(tmenu) = cmenu;
			    cmenu = tmenu;
			    CurrentItem = EZ_MoveMenuSelection(cmenu, EZ_PopupLink2(cmenu), 0);
			    EZ_PopupLink2(cmenu) = CurrentItem;			    
			  }
		      }
		    break;
		  case XK_Return: case XK_Linefeed: case XK_space: 
#ifdef XK_KP_Enter
		  case XK_KP_Enter:
#endif
		    if(CurrentItem && EZ_WidgetType(CurrentItem) == EZ_WIDGET_MENU_SUBMENU)
		      {
			EZ_Widget *tmenu = EZ_SubMenuTheMenu(CurrentItem);
			if(tmenu)
			  {
			    if(! EZ_WidgetMapped(tmenu)) EZ_DisplayPopupSubMenuOf(CurrentItem);
			    EZ_PopupLink1(tmenu) = cmenu;
			    cmenu = tmenu;
			    CurrentItem = EZ_MoveMenuSelection(cmenu, EZ_PopupLink2(cmenu), 0);
			    EZ_PopupLink2(cmenu) = CurrentItem;			    
			  }
		      }
		    else
		      {
			sitem = CurrentItem;
			done = 1;
		      }
		    break;		    
		  default:
		    break;
		  }
	      }
	    if(shortcutItem)
	      {
		EZ_DeselectCurrentMenuItem(CurrentItem);
		CurrentItem = shortcutItem;
		sitem = shortcutItem;
	      }

	    if(sitem)
	      {
		done = 1;
		if(EZ_GetWidgetDisabledFlag(sitem) == 0)
		  {
		    int doit = 1;
		    
		    switch(EZ_WidgetType(sitem))
		      {
		      case EZ_WIDGET_MENU_CHECK_BUTTON:
			if( EZ_CButtonOn(sitem) )
			  {  EZ_CButtonSetOffValue(sitem);}
			else { EZ_CButtonSetOnValue(sitem);}
			break;
		      case EZ_WIDGET_MENU_RADIO_BUTTON:	  
			if( !( EZ_RButtonOn(sitem)) )
			  {
			    EZ_RButtonList  *friends;
			    EZ_Widget *old = (EZ_Widget *)NULL;
			    friends = EZ_RButtonGroup(sitem)->list;
			    while(friends)
			      {
				if( EZ_RButtonOn( friends->rbutton) )
				  {
				    old = friends->rbutton;  /* currently checked  */
				    break;
				  }
				friends = friends->next;
			      }
			    EZ_RButtonSetValue(sitem);
			    if(old && EZ_WidgetMapped(old)) EZ_DrawWidget(old);
			  }
			break;
		      case EZ_WIDGET_MENU_NORMAL_BUTTON:
		      case EZ_WIDGET_MENU_TEAR_OFF_BAR:
			break;
		      case EZ_WIDGET_MENU_SUBMENU:
			EZ_DisplayPopupSubMenuOf(sitem);
			doit = 0;
			done = 0;
			break;
		      default:
			doit = 0;
			break;
		      }
		    if(doit)
		      {
			EZ_DeselectCurrentMenuItem(sitem);
			if(EZ_CurrentPopupMenu != (EZ_Widget *)NULL)
			  {
			    EZ_WidgetRetData(EZ_CurrentPopupMenu) = EZ_WidgetRetData(sitem); /* return value */
			    EZ_PopupLSItem(EZ_CurrentPopupMenu) = sitem;   /* this is the selected item     */
			  }
		      }
		  }
	      }
	    else if(out) /* get out */
	      {
		XButtonEvent xbevent;
		xbevent.type = ButtonRelease;
		xbevent.button = EZ_Btn1;
		xbevent.display = EZ_Display;
		xbevent.time = CurrentTime;
		xbevent.window = EZ_WidgetWindow(out);
		xbevent.x=0; xbevent.y=0;
		XSendEvent(EZ_Display, EZ_WidgetWindow(out), 
			   False, ButtonReleaseMask, (XEvent *)&xbevent);
		EZ_HandleGlobalKeyEvent(out, 0, 0);
		done = 1;
	      }
	  } /* KeyPress */
	else /* ptr and other events */
	  {
	    int etype;
	    if(tmp == (EZ_Widget *)NULL) continue;
	    etype = xevent.type;
	    
	    if( etype == Expose ||etype == FocusIn ||etype == FocusOut || etype == LeaveNotify)
	      EZ_HandleWidgetWindowEvent(tmp, &xevent);
	    else
	      {
		if(EZ_find_item_in_array(items,tmp, nitems))
		  {
		    int type = EZ_WidgetType(tmp);
		    if(type == EZ_WIDGET_MENU_TEAR_OFF_BAR ||
		       (type >= EZ_WIDGET_MENU_SEPARATOR && type <= EZ_WIDGET_MENU_RADIO_BUTTON))
		      {
			if(xevent.type == EnterNotify) 
			  {
			    if(xevent.xcrossing.mode != NotifyGrab  &&
			       xevent.xcrossing.mode != NotifyUngrab)
			      {
				EZ_DeselectCurrentMenuItem(CurrentItem);
				CurrentItem = tmp;
			      }
			  }
			else if(xevent.type == LeaveNotify)
			  {
			    if(xevent.xcrossing.mode != NotifyGrab  &&
			       xevent.xcrossing.mode != NotifyUngrab)
			      {
				EZ_DeselectCurrentMenuItem(CurrentItem);
				CurrentItem = NULL;
			      }
			  }
			EZ_HandleWidgetWindowEvent(tmp, &xevent);
		      }
		  }
	      }
	    if(xevent.type == ButtonPress || xevent.type == ButtonRelease)
	      {
		if(xevent.type == ButtonRelease &&
		   (EZ_WidgetType(tmp) != EZ_WIDGET_MENU_SUBMENU)) done = 1;
	      }
	  } 
	if(done) break;
      }
    XUngrabPointer(EZ_Display, CurrentTime); 
    XUngrabKeyboard(EZ_Display,CurrentTime); 
  }
}
/*************************************************************************/
EZ_Widget *EZ_FindMenuItemOnMenu(menu, item, dir)
     EZ_Widget *menu, *item; int dir; /* >0 for upwards */
{
  EZ_Widget *tmp, *children, *front, *tmpa;
  int found = 0;

  if(menu == NULL) return(NULL);
  children = EZ_WidgetChildren(menu);
  if(children && EZ_WidgetType(children) == EZ_WIDGET_LABEL)
    children = EZ_WidgetSibling(children);
  if(children == NULL) return(NULL);

  if(item == NULL) found = -1;
  tmp = children;
  front = NULL;
  while(found == 0 && tmp)
    {
      if(tmp == item){  found = 2;  break;}
      front = tmp;
      tmp = EZ_WidgetSibling(front);
    }
  if(found <= 0) 
    {
      if(dir <= 0) /* down */
	{
	  tmp = children;
	  while(tmp && (EZ_GetWidgetDisabledFlag(tmp) ||
			EZ_GetWidgetNonActiveFlag(tmp) ||
			EZ_WidgetType(tmp) == EZ_WIDGET_MENU_SEPARATOR))
	    {
	      if(EZ_WidgetSibling(tmp) == NULL) break;
	      else tmp = EZ_WidgetSibling(tmp);
	    }
	  return(tmp);
	}
      else
	{
	  tmpa = tmp = children;
	  while(tmp)
	    {
	      if(EZ_GetWidgetDisabledFlag(tmp) == 0 &&
		 EZ_GetWidgetNonActiveFlag(tmp) == 0 &&
		 EZ_WidgetType(tmp) != EZ_WIDGET_MENU_SEPARATOR)
		tmpa = tmp;
	      tmp = EZ_WidgetSibling(tmp);
	    }
	  return(tmpa);
	}
    }
  else /* found the item */
    {
      if(dir > 0) /* up wards */
	{
	  if(front == NULL)
	    {
	      tmpa = tmp = children;
	      while(tmp)
		{
		  if(EZ_GetWidgetDisabledFlag(tmp) == 0 &&
		     EZ_GetWidgetNonActiveFlag(tmp) == 0 &&
		     EZ_WidgetType(tmp) != EZ_WIDGET_MENU_SEPARATOR)
		    tmpa = tmp;
		  tmp = EZ_WidgetSibling(tmp);
		}
	      return(tmpa);
	    }
	  else
	    {
	      tmpa = tmp = children;
	      while(tmp)
		{
		  if(EZ_GetWidgetDisabledFlag(tmp) == 0 &&
		     EZ_GetWidgetNonActiveFlag(tmp) == 0 &&
		     EZ_WidgetType(tmp) != EZ_WIDGET_MENU_SEPARATOR)
		    tmpa = tmp;
		  tmp = EZ_WidgetSibling(tmp);
		  if(tmp == item) break;
		}
	      return(tmpa);	      
	    }
	}
      else if(dir < 0)/* down wards */
	{
	  tmp = EZ_WidgetSibling(item);
	  if(tmp == NULL) tmp = children;
	  while(tmp)
	    {
	      if(EZ_GetWidgetDisabledFlag(tmp) == 0 &&
		     EZ_GetWidgetNonActiveFlag(tmp) == 0 &&
		 EZ_WidgetType(tmp) != EZ_WIDGET_MENU_SEPARATOR)
		{ return(tmp);}
	      else tmp = EZ_WidgetSibling(tmp);
	    }
	  if(tmp == NULL) return(children);
	}
      else return(item);
    }
  return(NULL);
}
EZ_Widget *EZ_MoveMenuSelection(menu, citem, dir)
     EZ_Widget *menu, *citem; int dir; /* >0 for moving upwards */
{
  EZ_Widget *tmp = EZ_FindMenuItemOnMenu(menu, citem, dir);
  if(tmp) 
    {
      XFocusChangeEvent xfc;
      if(citem != NULL && EZ_WidgetMapped(citem) )
	{
	  xfc.type = FocusOut;
	  xfc.display = EZ_Display;
	  xfc.window = EZ_WidgetWindow(citem);
	  XSendEvent(EZ_Display, EZ_WidgetWindow(citem),
		     False, FocusChangeMask,
		     (XEvent *) &xfc);
	}
      if(EZ_WidgetMapped(tmp))
	{
	  xfc.type = FocusIn;
	  xfc.display = EZ_Display;
	  xfc.window = EZ_WidgetWindow(tmp);
	  XSendEvent(EZ_Display, EZ_WidgetWindow(tmp),
		     False, FocusChangeMask,
		     (XEvent *) &xfc);
	}
      return(tmp);
    }
  else return(citem);
}
/****************************************************************************************/
EZ_Widget *EZ_GetPopupMenuTitleWidget(menu)
     EZ_Widget *menu;
{
  if(menu && EZ_WidgetType(menu) == EZ_WIDGET_POPUP_MENU)
    {
      EZ_Widget *child = EZ_WidgetChildren(menu);
      if(EZ_WidgetType(child) == EZ_WIDGET_LABEL)
	return(child);
    }
  return(NULL);
}
/****************************************************************************************/
void EZ_SetMenuSetupProcedure(menu, f, data)
     EZ_Widget *menu; EZ_CallBack f; void *data;
{
  if(menu && EZ_WidgetType(menu) == EZ_WIDGET_POPUP_MENU)
    {
      EZ_PopupSetupF(menu) = f;
      EZ_PopupSetupFD(menu) = data;
    }
}
void EZ_SetupPopup(menu)
     EZ_Widget *menu;
{
  if(menu && EZ_WidgetType(menu) == EZ_WIDGET_POPUP_MENU && EZ_PopupSetupF(menu))
    { (EZ_PopupSetupF(menu))(menu, EZ_PopupSetupFD(menu)); }
}
/****************************************************************************************/
void EZ_PropagateBgInfo2Widget(widget, mmenu) EZ_Widget *widget, *mmenu;
{
  { EZ_UninstallHighlightColor(widget); } /*4/29/99*/
  if(EZ_WidgetBackground(widget)) EZ_WidgetParentBG(mmenu) =  &(EZ_WidgetBackground(widget));
  else   EZ_WidgetParentBG(mmenu) = EZ_WidgetParentBG(widget);
  if(EZ_WidgetBGPixmap(widget))   EZ_WidgetParentBGPixmap(mmenu) = EZ_WidgetBGPixmap(widget);
  else EZ_WidgetParentBGPixmap(mmenu) = EZ_WidgetParentBGPixmap(widget);
  if(EZ_WidgetBGPixmap2(widget))  EZ_WidgetParentBGPixmap2(mmenu) = EZ_WidgetBGPixmap2(widget);
  else EZ_WidgetParentBGPixmap2(mmenu) = EZ_WidgetParentBGPixmap2(widget);      
}
/****************************************************************************************/
#undef _EZ_WIDGET_POPUP_C_

