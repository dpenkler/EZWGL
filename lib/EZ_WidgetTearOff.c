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
 ***              Misc Items for popupmenu                     ***
 ***                                                           ***
 *****************************************************************/
#define _EZ_WIDGET_TEAR_OFF_C_

#include "EZ_Widget.h"

/*********************************************************************
 * 
 *  Functions implemented in this file:
 */
EZ_Widget        *EZ_CreateMenuTearOffItem MY_ANSIARGS((EZ_Widget *parent));
void             EZ_DrawTearOffItem MY_ANSIARGS((EZ_Widget *widget));
void             EZ_ComputeTearOffItemSize MY_ANSIARGS((EZ_Widget *widget, int *w, int *h));
void             EZ_TearOffItemEventHandle MY_ANSIARGS((EZ_Widget *widget, XEvent *event));
void             EZ_HandleToplevelMenuEvents  MY_ANSIARGS((EZ_Widget *widget, XEvent *ev));
int              EZ_ConfigureTearOffMenu  MY_ANSIARGS((EZ_Widget *menu, int onoff));
void 		 EZ_DeselectCurrentMenuItem  MY_ANSIARGS((EZ_Widget *item));
void             EZ_RedisplayTearOffMenu MY_ANSIARGS((EZ_Widget *menu));
EZ_Widget        *EZ_GetMenuCompanion  MY_ANSIARGS((EZ_Widget *item));
/*********************************************************************
 * 
 *  Local Variables.
 */
static EZ_WidgetHandle EZ_MenuTearOffItemHandle =
{
  EZ_ComputeTearOffItemSize,
  EZ_DrawTearOffItem,
  EZ_FreeUnknownData,
  EZ_TearOffItemEventHandle,    /* response only to Expose */
};

static void TearOffItemCallback MY_ANSIARGS((EZ_Widget *widget, void *data));
/*********************************************************************/
EZ_Widget  *EZ_CreateMenuTearOffItem(parent)
     EZ_Widget  *parent;     /* parent widget    */
{
  EZ_Widget  *wptr;

  wptr = EZ_CreateNewWidget(parent);
  /*--------------------------------------------------
   * Register the handling functions for CButton.
   *  has to be done after  EZ_CreateNewWiget.
   *-------------------------------------------------*/
  EZ_WidgetHandlingFunctions[EZ_WIDGET_MENU_TEAR_OFF_BAR] = &EZ_MenuTearOffItemHandle;

  EZ_SetWidgetTypeAndNames(wptr, EZ_WIDGET_MENU_TEAR_OFF_BAR);

  EZ_WidgetBorderStyle(wptr)  = EZ_BORDER_NONE;
  EZ_WidgetBorderWidth(wptr)  = 2;
  EZ_WidgetPadB(wptr)  = 0;
  EZ_WidgetWidth(wptr)  = 10;    /* default size */
  EZ_WidgetHeight(wptr) = 10;
  EZ_WidgetRetData(wptr) = -(1<<30); 
  EZ_SetWidgetHeightSetFlag(wptr);
  EZ_MenuTearOffBarMenu(wptr) = (EZ_Widget *)NULL;
  EZ_AddWidgetCallBackInternal(wptr,  EZ_CALLBACK, 
			       (EZ_CallBack)TearOffItemCallback, NULL,0);
  return(wptr);
}

void EZ_ComputeTearOffItemSize(widget, w, h)
    EZ_Widget *widget;
     int             *w, *h;
{
  EZ_SetWidgetSizeComputedFlag(widget);

  *w = 10;
  *h = 10;
}

void  EZ_DrawTearOffItem(wptr)
     EZ_Widget *wptr;
{
  int             w, h,x, x1;
  Pixmap          pixmap;
  Window          win;
  GC              gc, gc1, junkgc;
  unsigned long   bgpv;
  int             ox=0, oy=0,tile;
  win = EZ_WidgetWindow(wptr);
  w   = EZ_WidgetWidth(wptr);
  h   = EZ_WidgetHeight(wptr);

  /*-----------------------------------------------------------
   *  Create a pixmap, draw into this pixmap in background and
   *  copy to the button window when finished.
   *----------------------------------------------------------*/
  pixmap = XCreatePixmap(EZ_DisplayForWidgets, win, w, h, EZ_Depth);   
  /*----------------------------------------------------------
   *       Fill the background 
   *---------------------------------------------------------*/
  if((tile =EZ_GetBackgroundGC(wptr, &gc, &bgpv, 1, 0)))
    {
      EZ_GetWidgetTileOrigin(wptr, &ox, &oy);      
      XSetTSOrigin(EZ_Display, gc, ox, oy);      
    }
  XFillRectangle(EZ_DisplayForWidgets, pixmap, gc, 0,0, w, h); 
  if(EZ_GetDarkBrightNormalBDGC(wptr, &gc, &gc1, &junkgc))  /* dark, bright */
    {
      if(tile == 0) EZ_GetWidgetTileOrigin(wptr, &ox, &oy); 
      XSetTSOrigin(EZ_Display, gc, ox, oy);            
      XSetTSOrigin(EZ_Display, gc1, ox, oy);            
    }
  for(x = 0, x1 = 6; x < w;)
    {
      XDrawLine(EZ_DisplayForWidgets,pixmap, gc,   x, 5, x1, 5);
      XDrawLine(EZ_DisplayForWidgets,pixmap, gc1,  x, 4, x1, 4);
      x += 9;
      x1 += 9;
    }
  EZ_DrawRectBorder(wptr, pixmap);
  XCopyArea(EZ_DisplayForWidgets,pixmap,win, EZ_WRITABLEGC,0,0,w,h,0,0); 
  XFreePixmap(EZ_DisplayForWidgets, pixmap);                             
}

void EZ_TearOffItemEventHandle(widget, event)
     EZ_Widget *widget;
     XEvent          *event;
{
  if(widget == (EZ_Widget *)NULL)   return;

  switch(event->type)
    {
    case Expose:
      EZ_DrawWidget(widget);      
      break;
    case ButtonPress:
      EZ_WidgetBorderStyle(widget) = EZ_BORDER_DOWN;
      EZ_DrawWidget(widget);
      if(EZ_CurrentPopupMenu == (EZ_Widget *)NULL)
	EZ_HandleToplevelMenuEvents(widget, event);
      break;
    case ButtonRelease:
      EZ_SetWidgetHighlightFlag(widget);
      { EZ_InstallHighlightColor(widget); } /*4/29/99*/
      EZ_WidgetBorderStyle(widget) = EZ_BORDER_NONE;
      EZ_DrawWidget(widget);
      if(EZ_CurrentPopupMenu != (EZ_Widget *)NULL)
	{
	  EZ_WidgetRetData(EZ_CurrentPopupMenu) = EZ_WidgetRetData(widget); /* return value */;
	  EZ_PopupLSItem(EZ_CurrentPopupMenu) = widget; 
	}
      break;
    case EnterNotify:
    case FocusIn:
      EZ_SetWidgetHighlightFlag(widget);
      { EZ_InstallHighlightColor(widget); } /*4/29/99*/
      EZ_WidgetBorderStyle(widget) = EZ_BORDER_UP;
      EZ_DrawWidget(widget);
      EZ_HideSubMenuOfA(EZ_WidgetParent(widget));
      break;
    case LeaveNotify:
    case FocusOut:
      EZ_ClearWidgetHighlightFlag(widget);
      { EZ_UninstallHighlightColor(widget); } /*4/29/99*/
      EZ_WidgetBorderStyle(widget) = EZ_BORDER_NONE;
      EZ_DrawWidget(widget);
      break;
    case KeyPress:
      EZ_HandleToplevelMenuEvents(widget, event);
    default:
      break;
    }
}
/**************************************************************************************/
EZ_Widget *EZ_GetMenuCompanion(widget)
     EZ_Widget *widget;
{
  if(widget && EZ_WidgetType(widget) == EZ_WIDGET_POPUP_MENU)
    return(EZ_PopupCompanion(widget));
  return(NULL);
}
/**************************************************************************************/
static void CloneLabels MY_ANSIARGS((EZ_Widget *parent, EZ_Widget *child));

static void TearOffItemCallback(widget, data)
     EZ_Widget *widget;
     void *data;
{
  if(widget && EZ_WidgetType(widget) == EZ_WIDGET_MENU_TEAR_OFF_BAR)
    {
      char *mtitle=NULL;
      EZ_Widget *tearoff;
      EZ_Widget *parent = EZ_WidgetParent(widget);

      if(EZ_GetWidgetIsCloneFlag(parent) != 0)
	{
	  (void)fprintf(stderr, "Warning: cannot tear off a cloned menu.\n");
	    return;
	}
      tearoff = EZ_MenuTearOffBarMenu(widget);

      if(EZ_LookupWidgetFromAllHT(tearoff) == (EZ_Widget *)NULL)
	{
	  EZ_Widget *menu, *children, *tmp;
	  menu = tearoff = EZ_CreatePopupMenuInternal(NULL);

	  EZ_MenuTearOffBarMenu(widget) = menu;
	  memcpy(menu, parent, sizeof(EZ_Widget));
          EZ_PopupInitiator(menu) = NULL;
          EZ_PopupXoff(menu) = 0;
          EZ_PopupYoff(menu) = 0;
	  EZ_SetWidgetIsCloneFlag(menu);
          EZ_PopupIsTearOff(menu) = 1;	  
	  EZ_PopupIsTearLink(menu) = widget;
	  EZ_PopupCompanion(menu) =  parent;
	  EZ_PopupCompanion(parent) = menu;

	  EZ_WidgetNumChildren(menu)  = 0;
	  EZ_WidgetChildren(menu) = NULL;
	  EZ_WidgetSibling(menu)  = (EZ_Widget *)NULL;
	  EZ_WidgetMapped(menu) = 0;
	  EZ_WidgetWindow(menu) = (Window)NULL;
	  EZ_ClearWidgetSizeComputedFlag(menu);
	  EZ_ClearWidgetTransientFlag(menu); 
	  children = EZ_WidgetChildren(parent);
	  while(children != NULL)
	    {
	      if(children != widget)
		{
		  int type = EZ_WidgetType(children);
		  if(type == EZ_WIDGET_MENU_RADIO_BUTTON)
		    {
		      EZ_RButtonGpStruct *group;
		      int id = EZ_RButtonGroup(children)->id;
		      id = id << 16;
		      tmp = EZ_CreateMenuRadioButton(menu, NULL, -1, id, -1, -1);
		      (void)my_free((char *)EZ_RButtonLabel(tmp));
		      group = EZ_RButtonGroup(tmp);
		      memcpy(tmp, children, sizeof(EZ_Widget));	
		      EZ_RButtonGroup(tmp) = group;
		    }
		  else if(type == EZ_WIDGET_LABEL)
		    {
		      tmp = NULL;
		      if(EZ_GetWidgetNonActiveFlag(children) == 0)
			mtitle = EZ_LabelString(children);
		    }
		  else if(type >= EZ_WIDGET_MENU_SEPARATOR &&
			  type <= EZ_WIDGET_MENU_RADIO_BUTTON)
		    {
		      tmp = EZ_CreateNewWidget(menu);
		      memcpy(tmp, children, sizeof(EZ_Widget));
		    }
		  else tmp = NULL;

		  if(tmp)
		    {
		      EZ_SetWidgetIsCloneFlag(tmp);
		      EZ_WidgetParent(tmp) = menu;
		      EZ_WidgetSibling(tmp) = NULL;
		      EZ_WidgetMapped(tmp) = 0;
		      EZ_WidgetWindow(tmp) = (Window)NULL;
		      EZ_ClearWidgetSizeComputedFlag(tmp);
		      EZ_WidgetNumChildren(tmp) = 0;
		      if(EZ_WidgetChildren(tmp) != NULL)
			{
			  EZ_Widget *w = EZ_WidgetChildren(tmp);
			  EZ_WidgetChildren(tmp) = NULL;
			  CloneLabels(tmp, w);
			}
		      /* register key shortcuts */
		      switch(type)
			{
			case EZ_WIDGET_MENU_RADIO_BUTTON:
			  EZ_RButtonCopy(tmp) = children;
			  EZ_RButtonCopy(children) = tmp;
			  if( EZ_RButtonShortcut(tmp) != NULL)
			    EZ_InsertGlobalKeyPressEvent(tmp, 
							 EZ_RButtonModifiers(tmp),
							 EZ_RButtonShortcut(tmp));
			  break;
			case EZ_WIDGET_MENU_CHECK_BUTTON:
			  EZ_CButtonCopy(tmp) = children;
			  EZ_CButtonCopy(children) = tmp;
			  if( EZ_CButtonShortcut(tmp) != NULL)
			    EZ_InsertGlobalKeyPressEvent(tmp, 
							 EZ_CButtonModifiers(tmp),
							 EZ_CButtonShortcut(tmp));
			  break;
			case EZ_WIDGET_MENU_NORMAL_BUTTON:
			  EZ_ButtonCopy(tmp) = children;
			  EZ_ButtonCopy(children) = tmp;
			  if( EZ_ButtonShortcut(tmp) != NULL)
			    EZ_InsertGlobalKeyPressEvent(tmp, 
							 EZ_ButtonModifiers(tmp),
							 EZ_ButtonShortcut(tmp));
			  break;
			case EZ_WIDGET_MENU_SUBMENU:
			  EZ_SubMenuCopy(tmp) = children;
			  EZ_SubMenuCopy(children) = tmp;
			  if( EZ_SubMenuShortcut(tmp) !=  NULL)
			    EZ_InsertGlobalKeyPressEvent(tmp, 
							 EZ_SubMenuModifiers(tmp),
							 EZ_SubMenuShortcut(tmp));
			  break;
			case EZ_WIDGET_MENU_SEPARATOR:
			  EZ_MenuSeparatorCopy(tmp) = children;
			  EZ_MenuSeparatorCopy(children) = tmp;
			  break;
			default: break;
			}
		    }
		}
	      children = EZ_WidgetSibling(children);
	    }
	}
      if(EZ_WidgetMapped(tearoff))
	{
	  int             rx,ry,x,y;
	  unsigned int    mask;
	  Window          root,win;

	  
	  XQueryPointer(EZ_Display, RootWindow(EZ_Display, EZ_ScreenNum),
			&root,  &win,  &rx, &ry,  &x,&y, &mask );

	  EZ_WidgetOriginX(tearoff) = rx - 32;
	  EZ_WidgetOriginY(tearoff) = ry ;
	  EZ_WidgetMapped(tearoff) = 3;
	}
      EZ_DisplayWidget(tearoff);
      XStoreName(EZ_Display, EZ_WidgetWindow(tearoff), mtitle==NULL? "No name": mtitle);
      {
        int x,y;
        Window junkwin, win = EZ_WidgetWindow(tearoff);
        XTranslateCoordinates(EZ_Display, win,
                              RootWindow(EZ_Display, EZ_ScreenNum),
                              0,0, &x,&y, &junkwin); 
        if(x != EZ_WidgetOriginX(tearoff) || y != EZ_WidgetOriginY(tearoff))
          {
            EZ_WidgetOriginX(tearoff) = x;
            EZ_WidgetOriginY(tearoff) = y;            
            EZ_RedisplayTearOffMenu(tearoff);
          }
      }
    }
}

/* clone only labels */
static void CloneLabels(parent, clst)
     EZ_Widget *parent,*clst;
{
  EZ_Widget *child = clst, *tmp;

  while(child != NULL)
    {
      int type = EZ_WidgetType(child);
      if(type == EZ_WIDGET_LABEL || type == EZ_WIDGET_NW_LABEL) 
	{
	  tmp = EZ_CreateNewWidget(parent);
	  memcpy(tmp, child, sizeof(EZ_Widget));
	  EZ_SetWidgetIsCloneFlag(tmp);
	  EZ_WidgetParent(tmp) = parent;
	  EZ_WidgetSibling(tmp) = NULL;
	  EZ_WidgetMapped(tmp) = 0;
	  EZ_WidgetWindow(tmp) = (Window)NULL;
	  EZ_ClearWidgetSizeComputedFlag(tmp);
	  EZ_WidgetNumChildren(tmp) = 0;
	  EZ_LabelCopy(tmp) = child;
	  EZ_LabelCopy(child) = tmp;
	  if(EZ_WidgetChildren(tmp) != NULL)
	    {
	      EZ_Widget *w = EZ_WidgetChildren(tmp);
	      EZ_WidgetChildren(tmp) = NULL;
	      CloneLabels(tmp, w);
	    }
	}
      child = EZ_WidgetSibling(child);
    }
}
/**********************************************************************************/
void EZ_list_items(place,menu,cidx)
     EZ_Widget *menu, *place[]; int *cidx;
{
  EZ_Widget *children;

  if(menu != (EZ_Widget *)NULL)
    {
      children = EZ_WidgetChildren(menu);
      while(children != (EZ_Widget *)NULL)
	{
	  if( *cidx >= 1024) return;
	  place[*cidx] = children; *cidx += 1;
	  if(EZ_WidgetType(children) == EZ_WIDGET_MENU_SUBMENU)
	    {
	      EZ_Widget *sm = EZ_SubMenuTheMenu(children);
	      if( sm != NULL)  EZ_list_items(place,sm,cidx);
	    }
	  children = EZ_WidgetSibling(children);
	}
    }
}
int EZ_find_item_in_array(ilist,item, cnt)
     EZ_Widget *ilist[], *item; int cnt;
{
  int i;
  for(i = 0; i < cnt; i++) {  if(ilist[i] == item) return(1);}
  return(0);
}

/**************************************************************************************
 *
 * This routine is invoked when a button press occurs at a menu item of
 * a toplevel menu (e.g. a tearoff menu)
 */
void  EZ_HandleToplevelMenuEvents(widget, event)
     EZ_Widget *widget;
     XEvent    *event;
{
  EZ_Widget    *sitem, *the_popup, *cmenu, *items[1024];
  Window       parent_window;
  int          nitems, keyEvent = 1;

  if(EZ_CurrentPopupMenu != (EZ_Widget *)NULL)
    { return;}

  if(event->type == KeyPress && (event->xkey.state & (ShiftMask | ControlMask | Mod1Mask)))
    return;
  /* thaw the passive grab */
  if(event->type == ButtonPress)
    { keyEvent = 0; XUngrabPointer(EZ_Display, CurrentTime);}
  
  /* init the current popup menu */
  if(EZ_WidgetType(widget) == EZ_WIDGET_POPUP_MENU)
    { the_popup = widget; widget = EZ_WidgetChildren(widget);}
  else the_popup = EZ_WidgetParent(widget);
  if(EZ_WidgetType(the_popup) != EZ_WIDGET_POPUP_MENU) return;
  EZ_CurrentPopupMenu = the_popup;
  cmenu = the_popup;
  EZ_PopupLink1(cmenu) = NULL;
  EZ_PopupLSItem(cmenu) = (EZ_Widget *)NULL;
  EZ_WidgetRetData(cmenu) = -1;
  parent_window = EZ_WidgetWindow(the_popup);

  nitems = 0;
  EZ_list_items(items, the_popup, &nitems);

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
    
    CurrentItem = widget;

    while(!done)
      {
	sitem = NULL;  shortcutItem=NULL; out=NULL;
	if(keyEvent)
	  {
	    keyEvent = 0; 
	    memcpy(&xevent, event, sizeof(XEvent));
	    xevent.xkey.window = parent_window;
	  }
	else 
	  {
	    XNextEvent(EZ_Display, &xevent);
	    EZ_FilterEvent(&xevent);	    
            EZ_InvokePrivateEventHandler(&xevent);
	  }
	
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
		  case XK_Down: case XK_n: case XK_j: case XK_N: case XK_J: 
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
		      EZ_Widget *tmpa = EZ_PopupLink1(cmenu); /* walk back */
		      if(tmpa)
			{
			  cmenu = tmpa;
			  EZ_DeselectCurrentMenuItem(CurrentItem);
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
			if(tmenu)
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
			    EZ_DeselectCurrentMenuItem(CurrentItem);
			    CurrentItem = EZ_MoveMenuSelection(cmenu, NULL, 0);
			    EZ_PopupLink2(cmenu) = CurrentItem;			    
			  }
		      }		    
		    else
		      {
			sitem = CurrentItem;
			done = 1;
		      }
		    break;	
                  case XK_Escape:
                    sitem = NULL;
                    done = 1;    
                    break;
		  default:
		    break;
		  }
	      }
	    if(shortcutItem)
	      {
		EZ_DeselectCurrentMenuItem(CurrentItem);
                if(EZ_WidgetType(shortcutItem) == EZ_WIDGET_MENU_SUBMENU)
                  {
                    EZ_Widget *tmenu = EZ_SubMenuTheMenu(shortcutItem);
                    if(tmenu )
                      {
                        if(!EZ_WidgetMapped(tmenu)) EZ_DisplayPopupSubMenuOf(shortcutItem);
                        EZ_DeselectCurrentMenuItem(shortcutItem);
                        EZ_PopupLink1(tmenu) = cmenu;
                        cmenu = tmenu;
                        CurrentItem = EZ_MoveMenuSelection(cmenu, EZ_PopupLink2(cmenu), 0);
                        EZ_PopupLink2(cmenu) = CurrentItem;			    
                      }
                  }
		else sitem = shortcutItem;
	      }

	    if(sitem)
	      {
		if(EZ_WidgetType(sitem) != EZ_WIDGET_MENU_SUBMENU) done = 1;
		else 
		  {
		    EZ_DisplayPopupSubMenuOf(sitem);
		  }
	      }
	    else if(out) /* get out */
	      {
		XButtonEvent xbevent;
		xbevent.type = ButtonRelease;
		xbevent.button = EZ_Btn1;
		xbevent.display = EZ_Display;
		xbevent.window = EZ_WidgetWindow(out);
		xbevent.time = CurrentTime;
		xbevent.x=0; xbevent.y=0;
		XSendEvent(EZ_Display, EZ_WidgetWindow(out), 
			   False, ButtonReleaseMask, (XEvent *)&xbevent);
		EZ_HandleGlobalKeyEvent(out, 0, 0);
		done = 1;
	      }
	  } /* KeyPress */
	else /* non key events */
	  {
	    if(tmp == (EZ_Widget *)NULL) continue;

	    if((xevent.type == Expose) ||
	       (xevent.type == FocusIn || xevent.type == FocusOut))
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
			    if( xevent.xcrossing.mode != NotifyGrab  &&
			       xevent.xcrossing.mode != NotifyUngrab)
			      {
				EZ_DeselectCurrentMenuItem(CurrentItem);
				CurrentItem = tmp;
				EZ_HandleWidgetWindowEvent(tmp, &xevent);
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
			    EZ_HandleWidgetWindowEvent(tmp, &xevent);
			  }
		      }
		  }
	      }
	    if(xevent.type == ButtonPress || xevent.type == ButtonRelease)
	      {
		if(xevent.type == ButtonRelease &&
		   (EZ_WidgetType(tmp) != EZ_WIDGET_MENU_SUBMENU))
		  {
		    sitem = CurrentItem;
		    done = 1;
		  }
	      }
	  } 
	if(done) break;
      }
    if(sitem !=NULL)
      {
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
		doit = 0;
		break;
	      default:
		doit = 0;
		break;
	      }
	    if(doit)
	      {
		EZ_ClearWidgetHighlightFlag(sitem);
                { EZ_UninstallHighlightColor(sitem); } /*4/29/99*/
		EZ_WidgetBorderStyle(sitem) = EZ_BORDER_NONE;
		EZ_DrawWidget(sitem);		
		if(EZ_CurrentPopupMenu != (EZ_Widget *)NULL)
		  { /* return value */
		    EZ_WidgetRetData(EZ_CurrentPopupMenu) = EZ_WidgetRetData(sitem);
		    /* this is the selected item     */
		    EZ_PopupLSItem(EZ_CurrentPopupMenu) = sitem;  
		  }
	      }
	  }
      }
    
    XUngrabPointer(EZ_Display, CurrentTime); 
    XUngrabKeyboard(EZ_Display,CurrentTime); 

    EZ_HideSubMenuOfA(the_popup);
    EZ_DisplayWidget(the_popup);
    EZ_HandlePopupCallBack(the_popup);
    EZ_CurrentPopupMenu = (EZ_Widget *)NULL;
  }
}

int EZ_ConfigureTearOffMenu(menu, onoff)
     EZ_Widget *menu; int onoff;
{
  if(menu)
    {
      EZ_Widget *children;
      children = EZ_WidgetChildren(menu);
      while(children)
	{
	  if(EZ_WidgetType(children) == EZ_WIDGET_MENU_TEAR_OFF_BAR)
	    break;
	  children = EZ_WidgetSibling(children);
	}
      if(onoff == 0) 
	{ 
	  if(children)
	    {
	      if(EZ_GetWidgetNonActiveFlag(children) == 0)
		{EZ_DeActivateWidget(children); return(1);} 
	    }
	}
      else
	{
	  if(children == NULL)
	    {
	      EZ_Widget *bar;
	      children = EZ_WidgetChildren(menu);
	      if(children && EZ_WidgetType(children) == EZ_WIDGET_LABEL)
		children = EZ_WidgetSibling(children);
	      bar = EZ_CreateMenuTearOffItem(menu);
	      if(children) EZ_InsertWidgetBefore(bar,children);
	      return(1);
	    }
	  else 
	    {
	     if(EZ_GetWidgetNonActiveFlag(children) != 0)
	       {
		 EZ_ActivateWidget(children);
		 return(1);
	       }
	    }
	}
    }
  return(0);
}
/**************************************************************************************/
void EZ_DeselectCurrentMenuItem(item)
     EZ_Widget *item;
{
  if(item)
    {
      EZ_ClearWidgetHighlightFlag(item);
      { EZ_UninstallHighlightColor(item); } /*4/29/99*/
      EZ_WidgetBorderStyle(item) = EZ_BORDER_NONE;
      if(EZ_WidgetMapped(item)) EZ_DrawWidget(item); 
    }
}
/**************************************************************************************/
void EZ_RedisplayTearOffMenu(wptr) EZ_Widget *wptr;
{
  if(wptr && EZ_WidgetBackground(wptr) == 0 &&
     EZ_WidgetBGPixmap(wptr) == NULL &&
     (EZ_WidgetParentBG(wptr) == NULL || *(EZ_WidgetParentBG(wptr)) == 0) &&
     EZ_WidgetParentBGPixmap(wptr) != NULL)
    {
      EZ_DisplayWidget(wptr);
    }
}
/**************************************************************************************/
#undef _EZ_WIDGET_MENU_TEAR_OFF_C_
