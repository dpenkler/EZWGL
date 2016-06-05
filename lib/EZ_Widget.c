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
 *  EZ Widget library. 
 */
#define _EZ_WIDGET_C_

#include "EZ_Widget.h"

/*******************************************************************
 *
 * Functions implemented in this file:
 */
EZ_Widget             *EZ_CreateNewWidget MY_ANSIARGS((EZ_Widget *parent));
void                  EZ_DestroyWidget MY_ANSIARGS((EZ_Widget *widget));
void                  EZ_DestroyCloneWidget MY_ANSIARGS((EZ_Widget *widget, int level));
void                  EZ_RemoveWidget MY_ANSIARGS((EZ_Widget *widget));
void                  EZ_DestroyWidgetReal MY_ANSIARGS((EZ_Widget *widget, int level));
void                  EZ_DestroyWidgetFinal MY_ANSIARGS((EZ_Widget *widget));
void                  EZ_GetWidgetLabelPosition MY_ANSIARGS((EZ_Widget *widget, int *x, int *y));
void                  EZ_MarkAllChildrenWidgetSizeChanged MY_ANSIARGS((EZ_Widget *widget, int flag));

void                  EZ_FigureLabelExtent MY_ANSIARGS((XFontStruct  *font_info,
							char         *string,
							int          length, int linelength,
							int          *numlines, int *twidth
							));
void                  EZ_FigureLabelExtentWork MY_ANSIARGS((XFontStruct  *font_info,
							    char         *string,
							    int          length, int linelength,
							    int          *numlines, int *twidth,
							    char         **idxes
							));
void                  EZ_RenderLabelText MY_ANSIARGS((Drawable drawable,
						      GC       gc,
						      int      x, int y,
						      int height, 
						      char     *string,
						      int length,
						      int linelength, int nlines,
						      int twidth, int justification,
						      XFontStruct  *font_info, int ss
						      ));
void                   EZ_RenderLabelTextDisabled  MY_ANSIARGS((Drawable drawable,
								GC       BTgc, GC DKgc, GC Ngc,
								int      x, int y,
								int height, 
								char     *string,
								int length,
								int linelength, int nlines,
								int twidth, int justification,
								XFontStruct  *font_info
								));

void                  EZ_RenderPixmapLabel MY_ANSIARGS((EZ_Widget *wptr,
							EZ_Bitmap *bitmap,
							Drawable  drawable,
							int       x,int y
							));
void                  EZ_RenderPixmapLabelWithGC  MY_ANSIARGS((EZ_Bitmap *bitmap,
							       Drawable  drawable,
							       GC        gc,
							       int       x,int y
							       ));

void                  EZ_MoveWidgetWindow MY_ANSIARGS((EZ_Widget *widget, int x, int y));
void                  EZ_ResizeWidgetWindow MY_ANSIARGS((EZ_Widget *widget, int w, int h));
void                  EZ_MoveResizeWidgetWindow MY_ANSIARGS((EZ_Widget *widget, 
                                                             int x, int y, int w, int h));
void                  EZ_SetWidgetPosition MY_ANSIARGS((EZ_Widget *widget, int x, int y));
void                  EZ_SetWidgetWidth MY_ANSIARGS((EZ_Widget *widget, int width));
void                  EZ_SetWidgetHeight MY_ANSIARGS((EZ_Widget *widget, int height));
void                  EZ_GetWidgetPosition MY_ANSIARGS((EZ_Widget *widget, int *x, int *y));
int                   EZ_GetWidgetWidth MY_ANSIARGS((EZ_Widget *widget));
int                   EZ_GetWidgetHeight MY_ANSIARGS((EZ_Widget *widget));
int                   EZ_GetWidgetMinWidth MY_ANSIARGS((EZ_Widget *widget));
int                   EZ_GetWidgetMinHeight MY_ANSIARGS((EZ_Widget *widget));
int                   EZ_GetWidgetBorderWidth MY_ANSIARGS((EZ_Widget *widget));
void                  EZ_GetWidgetPaddings MY_ANSIARGS((EZ_Widget *wg, int *px, int *py, int *pb));
void                  EZ_GetWidgetBorder MY_ANSIARGS((EZ_Widget *wg, int *bw, int *bs));
void                  EZ_GetWidgetDimension MY_ANSIARGS((EZ_Widget *widget, int *w, int *h));
void                  EZ_GetWidgetSize MY_ANSIARGS((EZ_Widget *widget, int *w, int *h));
void                  EZ_GetWidgetGeometry MY_ANSIARGS((EZ_Widget *widget, int *x,int *y, int *w, int *h));
void                  EZ_GetWidgetAbsoluteGeometry MY_ANSIARGS((EZ_Widget *widget, int *x,int *y, int *w, int *h));
void                  EZ_GetWidgetMinDimension MY_ANSIARGS((EZ_Widget *widget, int *w, int *h));
void                  EZ_DisableWidget MY_ANSIARGS((EZ_Widget *widget));
void                  EZ_DisableWidgetTree MY_ANSIARGS((EZ_Widget *widget));
void                  EZ_EnableWidgetTree MY_ANSIARGS((EZ_Widget *widget));
void                  EZ_EnableWidget MY_ANSIARGS((EZ_Widget *widget));
int                   EZ_SetWidgetLabelString MY_ANSIARGS((EZ_Widget *widget, char *str));
void                  EZ_SetWidgetLabelPixmap MY_ANSIARGS((EZ_Widget *widget, EZ_Bitmap *str));
char                  *EZ_GetWidgetLabelString MY_ANSIARGS((EZ_Widget *widget));
int                   EZ_GetWidgetIntData MY_ANSIARGS((EZ_Widget *widget));
void                  *EZ_GetWidgetPtrData MY_ANSIARGS((EZ_Widget *widget));
void                  EZ_GetWidgetClientData MY_ANSIARGS((EZ_Widget *widget, int *i, void **p));
int                   EZ_GetWidgetReturnedData MY_ANSIARGS((EZ_Widget *widget));
int                   EZ_GetWidgetReturnData MY_ANSIARGS((EZ_Widget *widget));
void                  EZ_SetWidgetIntData MY_ANSIARGS((EZ_Widget *widget, int idata));
void                  EZ_SetWidgetPtrData MY_ANSIARGS((EZ_Widget *widget, void *ptr));
void                  EZ_SetWidgetClientData MY_ANSIARGS((EZ_Widget *widget, int i, void *p));
void                  EZ_HideWidget MY_ANSIARGS((EZ_Widget *widget));
void                  EZ_HideWidgetWork MY_ANSIARGS((EZ_Widget *widget, int rec));
void                  EZ_DeActivateWidget MY_ANSIARGS((EZ_Widget *widget));
void                  EZ_ActivateWidget MY_ANSIARGS((EZ_Widget *widget));
void                  EZ_DeActivateChildren MY_ANSIARGS((EZ_Widget *widget));
void                  EZ_ActivateChildren MY_ANSIARGS((EZ_Widget *widget));
void                  EZ_DeActivateSibling MY_ANSIARGS((EZ_Widget *widget));
void                  EZ_ActivateSibling MY_ANSIARGS((EZ_Widget *widget));
void                  EZ_FreezeWidget MY_ANSIARGS((EZ_Widget *widget));
void                  EZ_UnFreezeWidget MY_ANSIARGS((EZ_Widget *widget));

void                  EZ_HighlightWidget MY_ANSIARGS((EZ_Widget *widget, int flag));
int                   EZ_GetBackgroundGC MY_ANSIARGS((EZ_Widget *widget,
						      GC *gc, unsigned long *pv,
						      int highlight, int specialhighlight));
int                   EZ_GetBackgroundTileGC MY_ANSIARGS((EZ_Widget *widget,
                                                          GC *gc, unsigned long *pv,
                                                          int highlight, int specialhighlight));

int                   EZ_GetParentBgGC MY_ANSIARGS((EZ_Widget *widget,
                                                     GC *gc, unsigned long *pv));
int                   EZ_GetParentBgTileGC MY_ANSIARGS((EZ_Widget *widget,
                                                        GC *gc, unsigned long *pv));

void                  EZ_GetBackgroundPV MY_ANSIARGS((EZ_Widget *widget,
						      unsigned long *pv, 
						      int hlight, int specialhlight));
int                   EZ_GetDarkBDGC MY_ANSIARGS((EZ_Widget *widget, GC *gc));
int                   EZ_GetBrightBDGC MY_ANSIARGS((EZ_Widget *widget, GC *gc));
int                   EZ_GetDarkBrightNormalBDGC MY_ANSIARGS((EZ_Widget *widget, GC *dk, GC *bt, GC *ngc));

void                  EZ_GetBrightBDpv MY_ANSIARGS((EZ_Widget *widget, unsigned long *pv));
void                  EZ_GetDarkBDpv MY_ANSIARGS((EZ_Widget *widget, unsigned long *pv));
void                  EZ_GetWidgetTileOrigin MY_ANSIARGS((EZ_Widget *widget, int *x, int *y));

int                   EZ_GetLabel2XPosition MY_ANSIARGS((EZ_Widget *widget,int l2width, int offset));
int                   EZ_GetWidgetOrientation MY_ANSIARGS((EZ_Widget *widget));

void                  EZ_SetWidgetCallBack MY_ANSIARGS((EZ_Widget *widget, EZ_CallBack callback, void *data));
void                  EZ_AddWidgetCallBack MY_ANSIARGS((EZ_Widget *widget, int wlist,
							EZ_CallBack callback, void *data,int where));
void                  EZ_AddWidgetCallBackInternal MY_ANSIARGS((EZ_Widget *widget, int wlist,
								EZ_CallBack callback, void *data,int where));
void                  EZ_RemoveWidgetCallBack MY_ANSIARGS((EZ_Widget *widget, int wlist,
							   EZ_CallBack callback, void *data));
void                  EZ_RemoveAllWidgetCallBacks MY_ANSIARGS((EZ_Widget *widget, int wlist));

void                  EZ_AddCallBackWork MY_ANSIARGS((EZ_CallBackStruct **list, EZ_CallBack callback,
						      void *data, int where, int flag));
void                  EZ_RemoveCallBackWork MY_ANSIARGS((EZ_CallBackStruct **list, EZ_CallBack callback, void *data));
void                  EZ_RemoveAllCallBacksWork MY_ANSIARGS((EZ_CallBackStruct **list ));

void                  EZ_AddEventHandler MY_ANSIARGS((EZ_Widget *widget, EZ_EventHandler handler,
						      void *data, int where));
void                  EZ_AddEventHandlerAll MY_ANSIARGS((EZ_Widget *widget, EZ_EventHandler handler,
                                                       void *data, int where));
void                  EZ_RemoveEventHandler MY_ANSIARGS((EZ_Widget *widget, EZ_EventHandler handler, void *data));

void                  EZ_RemoveAllEventHandlers MY_ANSIARGS((EZ_Widget *widget));

void                  EZ_AddWidgetCallBack00 MY_ANSIARGS((EZ_Widget *widget, int wlist,
							EZ_CallBack callback, void *data,int where));
void                  EZ_RemoveWidgetCallBack00 MY_ANSIARGS((EZ_Widget *widget, int wlist,
                                                               EZ_CallBack callback, void *data));
void                  EZ_RemoveAllWidgetCallBacks00 MY_ANSIARGS((EZ_Widget *widget, int wlist));

void                  EZ_AddEventHandler00 MY_ANSIARGS((EZ_Widget *widget, EZ_EventHandler handler,
                                                          void *data, int where));
void                  EZ_RemoveEventHandler00 MY_ANSIARGS((EZ_Widget *widget, EZ_EventHandler handler, void *data));
void                  EZ_RemoveAllEventHandlers00 MY_ANSIARGS((EZ_Widget *widget));

EZ_Widget             *EZ_GetParentWidget MY_ANSIARGS((EZ_Widget *widget));
EZ_Widget             *EZ_GetChildrenWidget MY_ANSIARGS((EZ_Widget *widget));
EZ_Widget             *EZ_GetSiblingWidget MY_ANSIARGS((EZ_Widget *widget));

unsigned long         EZ_GetForegroundPV MY_ANSIARGS((EZ_Widget *widget));
EZ_Bitmap             *EZ_GetWidgetPixmap MY_ANSIARGS((EZ_Widget *widget));
void                  EZ_GetDetachedInferior  MY_ANSIARGS((EZ_Widget *widget, int sib, int *idx,
							   EZ_Widget **sto));
EZ_CallBackStruct     **EZ_GetCallbackType MY_ANSIARGS((EZ_Widget *widget, int which));
EZ_EventHandlerStruct **GetWidgetEventHandlers MY_ANSIARGS((EZ_Widget *widget));
/*************************************************************************/
int                   EZ_GetWidgetUnknownData MY_ANSIARGS((EZ_Widget *widget, int idx,
							   EZ_UnknownDataType  *ret));
void                  EZ_SetWidgetUnknownData  MY_ANSIARGS((EZ_Widget *widget, int idx,
							    EZ_UnknownDataType data));
EZ_UnknownDataType   *EZ_GetWidgetUnknownDataPtr MY_ANSIARGS((EZ_Widget *widget));
/*************************************************************************/

/*************************************************************************
 ***                                                                   ***
 ***  Create a Widget data structure and initialize the common fields. ***
 ***  All EZ widget creation routines call this function.              ***
 ***                                                                   ***
 *************************************************************************/
static char *defaultAv[] = {"EZWGL: NoNameApp", NULL};

EZ_Widget *EZ_CreateNewWidget(parent)
     EZ_Widget  *parent;     /* parent widget    */
{
  EZ_Widget *tmp, *ptr;  

  /*----------------------------------------------------
   * Make sure that the widget lib has been initialized.
   *---------------------------------------------------*/
  if(EZ_Initialize(1, defaultAv, 1) < 0) return(NULL);
  
  /*-------------------------------------------
   * allocate a new widget and initialize it.
   *-----------------------------------------*/
  tmp = (EZ_Widget *)my_malloc(sizeof(EZ_Widget), _NEW_WIDGET_);
  if(!tmp)  EZ_OutOfMemory("EZ_CreateNewWidget");
  (void) memset(tmp,0, sizeof(EZ_Widget));

  EZ_WidgetGroupLeader(tmp) = tmp;
  EZ_WidgetWidth(tmp)        = 1;
  EZ_WidgetHeight(tmp)       = 1;
  EZ_WidgetParent(tmp)       = parent;
  EZ_WidgetForeground(tmp)   = EZ_DefaultForeground;
  EZ_WidgetCursor(tmp)       = EZ_GetCursor(EZ_C_LEFT_PTR);
  EZ_WidgetPadX(tmp)         = EZ_WIDGET_DEFAULT_PADX;
  EZ_WidgetPadY(tmp)         = EZ_WIDGET_DEFAULT_PADY;
  EZ_WidgetStacking(tmp)     = EZ_HORIZONTAL;
  EZ_WidgetAlignment(tmp)    = EZ_CENTER_ALIGNED;
  EZ_WidgetLabelPosition(tmp)= EZ_CENTER;
  EZ_WidgetJustification(tmp)= EZ_LEFT;
  EZ_WidgetBorderWidth(tmp)  = 2;
  EZ_WidgetBorderStyle(tmp)  = EZ_BORDER_NONE;
  EZ_WidgetGBW(tmp) = 1;
  EZ_WidgetGBH(tmp) = 1;

  /*------------------------------------------
   * Insert it into its parent's children list
   *-----------------------------------------*/  
  if(parent != (EZ_Widget *)NULL)
    {
      EZ_ClearWidgetSizeComputedFlag(parent);  
      EZ_WidgetNumChildren(parent)  += 1;
      ptr = EZ_WidgetChildren(parent);
      /*------------------------------------
       * tmp is the first widget of parent
       *-----------------------------------*/
      if(ptr == (EZ_Widget *)NULL)
	EZ_WidgetChildren(parent) = tmp;
      else
	{
	  /*-------------------------------
	   * insert tmp at the end of 
	   * children's sibling list.
	   *------------------------------*/
	  while(EZ_WidgetSibling(ptr)) ptr = EZ_WidgetSibling(ptr);
	  EZ_WidgetSibling(ptr) = tmp;
	}
    }  
  /*-----------------------------------------
   * insert tmp into the Widget hash table 
   *----------------------------------------*/
  EZ_InsertWidgetToAllHT(tmp);
  return(tmp);
}

/*******************************************************************************
 ***                                                                         ***
 ***  Destroy a widget. We do this in two steps.                             ***
 ***  [1].  ReChain its parent' children list, which is on the sibling       ***
 ***        list of its parent' first child widget.                          ***
 ***  [2].  Disconnect widget from its parent and sibling list and do        ***
 ***        a recursive destruction.                                         ***
 ***                                                                         ***
 *******************************************************************************/
void  EZ_DestroyWidget(widget)
     EZ_Widget *widget;
{
  int WType;

  if(EZ_LookupWidgetFromAllHT(widget) == (EZ_Widget *)NULL) return;
  WType = EZ_WidgetType(widget);

  /*----------------------------------------------------------------
   * special case first. NoteBookPage has a special button companion
   *----------------------------------------------------------------*/
  if(WType == EZ_WIDGET_NORMAL_BUTTON || WType == EZ_WIDGET_ARROW_BUTTON)
    {
      EZ_Widget *companion = EZ_ButtonCompanion(widget);
      if(companion) EZ_NBPageCompanion(companion) = NULL;
    }
  else if(WType == EZ_WIDGET_NB_PAGE)
    {
      EZ_Widget *companion = EZ_NBPageCompanion(widget);
      EZ_Widget *notebook  = EZ_NBPageNoteBook(widget);
      EZ_Widget *actpage = EZ_NoteBookActivePage(notebook);
      if(actpage == widget) EZ_NoteBookActivePage(notebook) = NULL;
      EZ_DestroyWidget(companion);
    }

  /*----------------------------------
   *    Fix its parent
   *---------------------------------*/ 
  EZ_RemoveWidget(widget);
  /*---------------------------------
   * Now, the real destruction
   *--------------------------------*/
  EZ_WidgetSibling(widget) = (EZ_Widget *)NULL;
  EZ_WidgetParent(widget) = (EZ_Widget *)NULL; 
  if(EZ_GetWidgetIsCloneFlag(widget))  EZ_DestroyCloneWidget(widget, 0);
  else 
    {
      EZ_Widget *clone = EZ_GetMenuItemCompaion(widget);
      EZ_DestroyWidgetReal(widget, 0);
      if(clone)  EZ_DestroyWidget(clone);
    }
}

void EZ_DestroyWidgetReal(widget, level)
     EZ_Widget *widget; int level;
{
  EZ_Widget *children, *sibling;
  int WType;

  if(widget == (EZ_Widget *)NULL) return;  
  WType = EZ_WidgetType(widget);

  children = EZ_WidgetChildren(widget);
  sibling = EZ_WidgetSibling(widget);
  EZ_DestroyWidgetReal(children, level+1);
  EZ_DestroyWidgetReal(sibling, level);
  
  /*-------------------------------------------------
   * remove it fm the widget Hash tables
   *------------------------------------------------*/
  EZ_RemoveWidgetFromAllHT(widget);  
  EZ_RemoveWidgetFromMappedHT(widget);  
  EZ_RemveWidgetFromDnDList(widget);
  EZ_DeleteWidgetWMHints(widget);
  /*
   * destroy embeded application. This is a simple minded
   * solution. We cannot just destroy widget or leave the
   * windows unattended. Both destroy the window at the time
   * the applicatione exists. This leave the embeded window
   * be destroyed, which causes X errors ...
   * Ths soln here is to reparent the embeded window to 
   * ROOTWINDOW, send a destroy message to the embeded 
   * application, and just let it handle itself.
   */
  if(WType == EZ_WIDGET_EMBEDER || WType == EZ_WIDGET_EXECUTOR)
    {
      if(EZ_EmbederConfirmed(widget) > 0)  /* embeding is alive */
	{
	  Window commWin = EZ_EmbederCommuWin(widget);
	  Window eWin = EZ_EmbederWin(widget);
	  unsigned long id = EZ_EmbederWidgetAddr(widget);
	  
	  EZ_GrabServer(); /* have to grab server */
	  if(EZ_EmbederId(widget) == EZ_VerifyExistence(commWin) && EZ_WindowExist(eWin))
	    {
	      int x = EZ_WidgetOriginX(widget); /* absolute coor ?? */
	      int y = EZ_WidgetOriginY(widget); 
	      int w = EZ_WidgetWidth(widget);
	      int h = EZ_WidgetHeight(widget);
	      EZ_SetWMHintForWindow(eWin, x,y,w,h);
	      XReparentWindow(EZ_Display, eWin, 
			      RootWindow(EZ_Display, EZ_ScreenNum),
			      x,y);  /* approximate location */
	    }
	  EZ_UngrabServer();

	  EZ_EmbederConfirmed(widget) = 0;
	  EZ_SendEmbedingMessage(EZ_EMBEDING_DESTROY_YOUSELF_C,
				 EZ_DummyWindow, EZ_WidgetWindow(widget),(unsigned long)widget,
				 commWin,eWin, id, EZ_EmbederId(widget), 0,0,0,0, 0, 0);
	}
    }
  /* destroy Widget Window */
  {
    Window win = EZ_WidgetWindow(widget);
    if(EZ_WindowExist(win))
      {
	EZ_RemoveFromDnDWindowList(widget); /* 5-20-97 */
	if(level == 0) XDestroyWindow(EZ_Display,win);
      }
    EZ_WidgetWindow(widget) = None;
  }
  
  if(EZ_GetWidgetEmbededFlag(widget))
    {
      /* widget is embeded in another application 
       * send a message to the embeder.
       */
      Window commWin, win;
      
      commWin = EZ_WidgetDnDApplName(widget);
      win = EZ_WidgetDnDParent(widget);
      if(EZ_VerifyExistence(commWin) && EZ_WindowExist(win))
	{
	  EZ_SendEmbedingMessage(EZ_EMBEDING_CLIENT_DIED_S,
				 EZ_DummyWindow, EZ_WidgetWindow(widget), (unsigned long)widget,
				 commWin, win, 0, EZ_ApplicationSetupTime, 0,0,0,0, 0, 0);
	}
    }
  {
    EZ_ResourceHandle *rhandle = EZ_WidgetResourceHandle(widget);
    while(rhandle)
      {
        EZ_ResourceHandle *tmp = rhandle->next;
        if(rhandle->mark == 0 && rhandle->resources)
          (void) my_free(rhandle->resources);
        (void) my_free( (void *)rhandle);
        rhandle = tmp;
      }
    EZ_WidgetResourceHandle(widget) = NULL;
  }

  if(EZ_WidgetBlocked(widget)<= 0)  EZ_DestroyWidgetFinal(widget);
  else
    {
      EZ_WidgetIsDestroyed(widget) = 1; /* mark it's been destroyed */
      EZ_InsertObjToGarbageList(EZ_GARBAGE_IS_WIDGET, (void *)widget);
    }
}

void EZ_DestroyWidgetFinal(widget)
     EZ_Widget *widget;
{
  int WType = EZ_WidgetType(widget);

  if(EZ_GetWidgetIsCloneFlag(widget)) /* cloned widget */
    {
      /*-------------------------------------------------------
       * RADIO buttons are grouped with others. Fix its group
       *------------------------------------------------------*/  
      if(WType == EZ_WIDGET_MENU_RADIO_BUTTON || WType == EZ_WIDGET_RADIO_BUTTON)
	EZ_RemoveRBtnFromItsGroup(widget);
      /*-------------------------------------------------
       * and finally, free the widget.
       *------------------------------------------------*/  
      (void) my_free( (char *)widget);
      return;
    }
  /*-------------------------------------------------
   * If there is a bubble help string, release it
   *------------------------------------------------*/  
  if(EZ_WidgetBubbleString(widget))
    {
      (void)my_free(EZ_WidgetBubbleString(widget));
      EZ_WidgetBubbleString(widget) = (char *)NULL;
    }
  if(EZ_WidgetDnDTargetHelp(widget))
    {
      (void)my_free(EZ_WidgetDnDTargetHelp(widget));
      EZ_WidgetDnDTargetHelp(widget) = (char *)NULL;
    }

  /* free geometry manager data */
  if(EZ_WidgetGManager(widget) != (EZ_GManager *)NULL)
    {
      EZ_DestroyGManager(EZ_WidgetGManager(widget));
      EZ_WidgetGManager(widget) = (EZ_GManager *)NULL;
    }
  /*-------------------------------------------------
   * remove the callback lists and event handlers
   *------------------------------------------------*/  
  {
    EZ_CallBackStruct *next, *funcList = EZ_WidgetCallBackFunc(widget);
    while(funcList)
      {
	next = funcList->next;
	(void)my_free(funcList);      
	funcList = next;
      }
    EZ_WidgetCallBackFunc(widget) = (EZ_CallBackStruct *)NULL;

    funcList = EZ_WidgetMotionCallBack(widget);
    while(funcList)
      {
	next = funcList->next;
	(void)my_free(funcList);      
	funcList = next;
      }
    EZ_WidgetMotionCallBack(widget) = (EZ_CallBackStruct *)NULL;
  }
  {
    EZ_EventHandlerStruct *next, *funcList = EZ_WidgetEventHandlers(widget);
    while(funcList)
      {
	next = funcList->next;
	(void)my_free(funcList);      
	funcList = next;
      }    
    EZ_WidgetEventHandlers(widget) = (EZ_EventHandlerStruct *)NULL;      
  }
  /*-------------------------------------------------
   * remove DnD message En(De)coders
   *------------------------------------------------*/  
  {
    EZ_DnDDataEncoder *next, *tmp = EZ_WidgetDnDDataEncoders(widget);
    while(tmp)
      {
	next = tmp->next;
	(void)my_free((char *)tmp);
	tmp = next;
      }
    EZ_WidgetDnDDataEncoders(widget) = (EZ_DnDDataEncoder *)NULL;
  }
  {
    EZ_DnDDataDecoder *next, *tmp = EZ_WidgetDnDDataDecoders(widget);
    while(tmp)
      {
	next = tmp->next;
	(void)my_free((char *)tmp);
	tmp = next;
      }
    EZ_WidgetDnDDataDecoders(widget) = (EZ_DnDDataDecoder *)NULL;
  }
  /*-------------------------------------------------
   * free background pixmaps
   *------------------------------------------------*/  
  if(EZ_WidgetBGPixmap(widget))
    {
      EZ_FreeLabelPixmap(EZ_WidgetBGPixmap(widget));
      EZ_WidgetBGPixmap(widget) = NULL;
    }
  if(EZ_WidgetBGPixmapB(widget))
    {
      EZ_FreeLabelPixmap(EZ_WidgetBGPixmapB(widget));
      EZ_WidgetBGPixmapB(widget) = NULL;
    }
  
  /*-------------------------------------------------
   * Call user-defined  DestroyCallbacks
   *------------------------------------------------*/  
  if(EZ_WidgetDestroyCallback(widget))
    {
      EZ_CallBackStruct *next, *funcList = EZ_WidgetDestroyCallBack(widget); 
      if(funcList)
	{
	  EZ_WidgetDestroyCallBack(widget) = (EZ_CallBackStruct *)NULL;
	  while(funcList)
	    {
	      if(funcList->callback) (funcList->callback)(widget,funcList->data);
	      next = funcList->next;
	      (void)my_free((char *)funcList);
	      funcList = next;
	    }
	}
    }
  /*-------------------------------------------------
   * free widget specific data.
   *------------------------------------------------*/  
  (EZ_WidgetHandlingFunctions[WType]->FreeData)(widget);

  /* free allocated storage for general clientdata */
  if(EZ_WidgetPValues(widget)) (void) my_free((char *)EZ_WidgetPValues(widget));
  if(EZ_WidgetResourceString(widget)) (void)my_free((char *)EZ_WidgetResourceString(widget));
  /*-------------------------------------------------
   * and finally, free the widget.
   *------------------------------------------------*/  
  (void) my_free( (char *)widget);
}

/******************************************************
 * used only for destroying tear off menu
 */
void EZ_DestroyCloneWidget(widget, level)
     EZ_Widget *widget; int level;
{
  EZ_Widget *children, *sibling;
  int WType;

  if(widget == (EZ_Widget *)NULL) return;  
  WType = EZ_WidgetType(widget);

  children = EZ_WidgetChildren(widget);
  sibling = EZ_WidgetSibling(widget);
  EZ_DestroyCloneWidget(children, level+1);
  EZ_DestroyCloneWidget(sibling, level);
  
  /*-------------------------------------------------
   * remove it fm the widget HT and close its window.
   *------------------------------------------------*/
  EZ_RemoveWidgetFromAllHT(widget);  
  EZ_RemoveWidgetFromMappedHT(widget);  
  EZ_RemveWidgetFromDnDList(widget);

  /* destroy widget window */
  {
    Window win = EZ_WidgetWindow(widget);
    if(EZ_WindowExist(win))
      {
	EZ_RemoveFromDnDWindowList(widget); /* 5-20-97 */
	if(level == 0) XDestroyWindow(EZ_Display,win);
      }
    EZ_WidgetWindow(widget) = None;
  }
  
  if(EZ_GetWidgetEmbededFlag(widget))
    {
      /* widget is embeded in another application 
       * send a message to the embeder.
       */
      Window commWin, win;
      
      commWin = EZ_WidgetDnDApplName(widget);
      win = EZ_WidgetDnDParent(widget);
      if(EZ_VerifyExistence(commWin) && EZ_WindowExist(win))
	{
	  EZ_SendEmbedingMessage(EZ_EMBEDING_CLIENT_DIED_S,
				 EZ_DummyWindow, EZ_WidgetWindow(widget), (unsigned long)widget,
				 commWin, win, 0, EZ_ApplicationSetupTime, 0,0,0,0, 0, 0);
	}
    }
  if(EZ_WidgetBlocked(widget) <= 0) EZ_DestroyWidgetFinal(widget);
  else EZ_InsertObjToGarbageList(EZ_GARBAGE_IS_WIDGET, (void *)widget);
}

/****************************************************************/
void EZ_RemoveWidget(widget)
     EZ_Widget *widget;
{
  if(widget)
    {
      EZ_Widget *parent = NULL;
      if( (parent = EZ_WidgetParent(widget)) != (EZ_Widget *)NULL)
	{
	  EZ_Widget *wptr, *children = EZ_WidgetChildren(parent);
      
	  if(children == widget)
	    { 
	      EZ_WidgetChildren(parent) = EZ_WidgetSibling(widget);
	    }
	  else
	    {
	      wptr = children;
	      while(wptr && EZ_WidgetSibling(wptr) != widget)
		{
		  wptr =  EZ_WidgetSibling(wptr);
		}
	      if(wptr)
		{
		  children = EZ_WidgetSibling(wptr);
		  EZ_WidgetSibling(wptr) = EZ_WidgetSibling(children);
		}
	    }
	  EZ_ClearWidgetSizeComputedFlag(parent);  
	  EZ_WidgetNumChildren(parent) -= 1;
	}
    }
}

/**************************************************************
 *
 *  Extract the displacement for placing  LABEL
 *  on a widget.
 */
void  EZ_GetWidgetLabelPosition(wptr, x,y)
     EZ_Widget *wptr;
     int *x, *y;
{
  int  fillx, filly;

  if(wptr == (EZ_Widget *)NULL)
    {
      *x = 0; *y = 0;
      return;
    }
  fillx = filly = 0;
  switch(EZ_WidgetLabelPosition(wptr))
    {
    case EZ_LEFT:
    case EZ_LEFT_2_RIGHT:
      fillx = - EZ_WidgetXOffset(wptr);
      break;
    case EZ_RIGHT:
      fillx =  EZ_WidgetXOffset(wptr);
      break;
    case EZ_TOP:
      filly = - EZ_WidgetYOffset(wptr);
      break;
    case EZ_BOTTOM:
      filly = EZ_WidgetYOffset(wptr);
      break;
    case EZ_TOP_LEFT:
      filly = - EZ_WidgetYOffset(wptr);
      fillx = - EZ_WidgetXOffset(wptr);
      break;
    case EZ_TOP_RIGHT:
      filly = - EZ_WidgetYOffset(wptr);
      fillx = EZ_WidgetXOffset(wptr);
      break;
    case EZ_BOTTOM_LEFT:
      filly = EZ_WidgetYOffset(wptr);
      fillx = -EZ_WidgetXOffset(wptr);
      break;
    case EZ_BOTTOM_RIGHT:
      filly = EZ_WidgetYOffset(wptr);
      fillx = EZ_WidgetXOffset(wptr);
      break;
    case EZ_CENTER:
    default:
      break;
    }
  *x = fillx;
  *y = filly;
}

int EZ_GetLabel2XPosition(wptr, l2width, offset)
     EZ_Widget *wptr;
     int l2width, offset;
{
  int w = EZ_WidgetWidth(wptr);
  int bwpdx = EZ_WidgetPadX(wptr) + EZ_WidgetBorderWidth(wptr) + EZ_WidgetPadB(wptr);
  int tmpx;

  tmpx = w - bwpdx -l2width + offset;
  if(EZ_WidgetChildren(wptr) == NULL &&
     EZ_WidgetLabelPosition(wptr) == EZ_CENTER)
    tmpx -= EZ_WidgetXOffset(wptr);

  return(tmpx);
}
/********************************************************************************
 ***                                                                          ***
 ***  The Next two functions compute/display a lable string on a button       ***
 ***  and/or label.  They use the same simple algorithm to break long         ***
 ***  labels into pieces. Both should be modified if we want to change to a   ***
 ***  better algorithm !                                                      ***
 ***                                                                          ***
 ********************************************************************************/

/*
 *  Figure out the text extent of label.
 */
void EZ_FigureLabelExtent(font_info, string, length, linelength, numlines, twidth)
     XFontStruct  *font_info;
     char         *string;
     int          length, linelength;
     int          *numlines, *twidth;
{
  EZ_FigureLabelExtentWork(font_info, string, length, linelength, numlines, twidth, NULL);
}

void EZ_FigureLabelExtentWork(font_info, string, length, linelength, numlines, twidth, idxes)
     XFontStruct  *font_info;
     char         *string;
     int          length, linelength;
     int          *numlines, *twidth;
     char         **idxes;
{
  char   *str, *tmp, *tmpa = (char *)NULL;
  int    i, j, itmp, nlines, slen, llen, trylen, lookahead, maxwidth;

  if(string == NULL || *string== '\0' || length == 0)
    {
      *numlines = 0; *twidth = 0;
      return;
    }
  str = string;
  slen = length;
  llen = (linelength <= 0? 16: linelength);

  if(llen < 8)
    {
      trylen = (llen >>1) + 1;
      lookahead = llen - trylen;
    }
  else
    {
      trylen = llen - 8;
      lookahead = 8;
    }
  maxwidth = 0;
  nlines = 0;
  if(idxes) idxes[nlines] = str;
  while(slen > 0)
    {
      if(slen > llen)
	{
	  itmp = -1;
	  for(i = 0; i < trylen + lookahead; i++)
	    {
	      if( *(str + i) == '\n')
		{
		  itmp = i+1;
		  tmpa = str + i+1;
		  j = XTextWidth(font_info, str, itmp);
		  maxwidth = MAXV(maxwidth, j);
		  str = tmpa;
		  slen = slen - itmp;
		  if(idxes) idxes[nlines] = str;
		  nlines++;
		  break;
		}
	    }
	  if( itmp == -1) 
	    {
	      tmp = str + trylen;
	      for(i = 0; i < lookahead; i++)
		{
		  if(IS_END_OF_WORD(*tmp))
		    {
		      tmpa = tmp+1;
		      itmp = i+1;
		    }
		  tmp++;
		}
	      if(itmp == -1) 
		{
		  itmp = lookahead;
		  tmpa = tmp;
		}
	      i = XTextWidth(font_info, str, trylen + itmp);
	      maxwidth = MAXV(maxwidth, i);
	      if(idxes) idxes[nlines] = str;
	      nlines++;
	      str = tmpa;
	      slen = slen - (trylen + itmp);
	    }
	}
      else
	{
	  itmp = -1;
	  for(i = 0; i < slen; i++)
	    {
	      if( *(str + i) == '\n')
		{
		  j = XTextWidth(font_info, str, i);
		  maxwidth = MAXV(maxwidth, j); 
		  if(idxes) idxes[nlines] = str;
		  str = str + i+1;
		  slen = slen - (i+1); 
		  nlines++;
		  itmp = 0;
		  break;
		}
	    }
	  if(itmp == -1)
	    {
	      i = XTextWidth(font_info, str, slen);
	      slen = 0;
	      maxwidth = MAXV(maxwidth, i); 
	      if(idxes) idxes[nlines] = str;
	      nlines++;
	    }
	}
    }
  *numlines = nlines;
  *twidth = maxwidth;
}

/*
 *  Draw the label string on widgets.
 */
void EZ_RenderLabelText(drawable,gc,x,y,height,string,length, linelength, nlines,
			twidth, justification, font_info, shadow)
     Drawable drawable;
     GC       gc;
     int      x,y,height,length,linelength, nlines, twidth, justification, shadow;
     char     *string;
     XFontStruct  *font_info;     
{

  char   *str, *tmp, *tmpa = (char *)NULL;
  int    i, itmp, slen, llen, trylen, lookahead, offset;
  int    shadowx, shadowy;

  str = string;
  if(str[0] == '\0') return;

  slen = length;
  llen = (linelength <=0 ? 16: linelength);;

  {shadowx = shadowy = shadow;}
  if(shadow) 
    {
      XSetFont(EZ_Display, EZ_SHADOWGC, font_info->fid);
      if(shadow &0xf0)
        {
          shadowx = (shadow & 0xf0) >> 4;
          shadowy = (shadow & 0x0f);
        }
      if(shadowx & 8) shadowx= (8-shadowx);
      if(shadowy & 8) shadowy= (8-shadowy);
    }
  if(nlines == 1)
    {
      if(shadow)
        XDrawString(EZ_Display, drawable, EZ_SHADOWGC, x+shadowx, y+shadowy, string, slen);
      XDrawString(EZ_Display, drawable, gc, x, y, string, slen);
      return;
    }
  
  if(llen < 8)
    {
      trylen = (llen >>1) + 1;
      lookahead = llen - trylen;
    }
  else
    {
      trylen = llen - 8;
      lookahead = 8;
    }
	  
  while(slen > 0)
    {
      if(slen > llen)
	{
	  itmp = -1;
	  for(i = 0; i < trylen + lookahead; i++)
	    {
	      if( *(str + i) == '\n')
		{
		  itmp = i+1;
		  tmpa = str + i+1;
		  if(justification == EZ_LEFT) offset = 0;
		  else
		    { 
		      int tlen = XTextWidth(font_info, str, itmp);
		      offset = twidth - tlen;
		      if(justification == EZ_CENTER) offset = offset >> 1;
		    }
                  if(shadow) 
                    XDrawString(EZ_Display, drawable, EZ_SHADOWGC, x + offset+shadowx,y+shadowy, str, itmp);
		  XDrawString(EZ_DisplayForWidgets, drawable, gc, x + offset,y, str, itmp);
		  str = tmpa;
		  slen = slen - itmp;
		  break;
		}
	    }
	  if(itmp == -1)
	    {
	      tmp = str + trylen;
	      for(i = 0; i < lookahead; i++)
		{
		  if(IS_END_OF_WORD(*tmp))
		    {
		      tmpa = tmp+1;
		      itmp = i+1;
		    }
		  tmp++;
		}
	      if(itmp == -1) 
		{
		  itmp = lookahead;
		  tmpa = tmp;
		}
	      i = trylen + itmp;
	      if(justification == EZ_LEFT) offset = 0;
	      else
		{ 
		  int tlen = XTextWidth(font_info, str, i);
		  offset = twidth - tlen;
		  if(justification == EZ_CENTER) offset = offset >> 1;
		}
              if(shadow)
                XDrawString(EZ_Display, drawable, EZ_SHADOWGC, x + offset+shadowx, y+shadowy, str, i);
              XDrawString(EZ_DisplayForWidgets, drawable, gc, x + offset, y, str, i);
	      str = tmpa;
	      slen = slen - i;
	    }
	}
      else
	{
	  itmp = -1;
	  for(i = 0; i < slen; i++)
	    {
	      if( *(str + i) == '\n')
		{
		  if(justification == EZ_LEFT) offset = 0;
		  else
		    { 
		      int tlen = XTextWidth(font_info, str, i+1);
		      offset = twidth - tlen;
		      if(justification == EZ_CENTER) offset = offset >> 1;
		    }
                  if(shadow)
                    XDrawString(EZ_Display, drawable, EZ_SHADOWGC, x + offset+shadowx, y+shadowy, str, i+1);
		  XDrawString(EZ_DisplayForWidgets, drawable, gc, x + offset, y, str, i+1);
		  str = str + i+1;
		  slen = slen - (i+1); 
		  itmp = 0;
		  break;
		}
	    }
	  if(itmp == -1)
	    {
	      if(justification == EZ_LEFT) offset = 0;
	      else
		{ 
		  int tlen = XTextWidth(font_info, str, slen);
		  offset = twidth - tlen;
		  if(justification == EZ_CENTER) offset = offset >> 1;
		}
              if(shadow)
                XDrawString(EZ_Display, drawable, EZ_SHADOWGC, x+offset+shadowx, y+shadowy, str, slen);
              XDrawString(EZ_DisplayForWidgets, drawable, gc, x+offset, y, str, slen);
	      slen = 0;
	    }
	}
      y += height;
    }
}


/*
 * Disabled widget label is displayed differently.
 */
void EZ_RenderLabelTextDisabled(drawable,BTgc,DKgc,Ngc, x,y,height,string,length, linelength, nlines,
				twidth, justification, font_info)
     Drawable drawable;
     GC       BTgc, DKgc, Ngc;
     int      x,y,height,length,linelength, nlines, twidth, justification;
     char     *string;
     XFontStruct  *font_info;     

{
  EZ_RenderLabelText(drawable,BTgc, x+1,y+1,height,string,length, linelength, nlines,
		     twidth, justification, font_info, 0);
  EZ_RenderLabelText(drawable,DKgc, x-1,y-1,height,string,length, linelength, nlines,
		     twidth, justification, font_info, 0);
  EZ_RenderLabelText(drawable,Ngc, x,y,height,string,length, linelength, nlines,
		     twidth, justification, font_info, 0);
}

/*
 * render a pixmap label.
 */
void EZ_RenderPixmapLabel(wptr, bitmap, drawable, x, y)
     EZ_Widget *wptr;
     EZ_Bitmap *bitmap;  /* the bitmap   */
     Drawable  drawable; /* the drawable */
     int       x,y;      /* UL corner    */
{
  XSetForeground(EZ_Display, EZ_WRITABLEGC, EZ_WidgetForeground(wptr));
  EZ_RenderPixmapLabelWithGC(bitmap, drawable, EZ_WRITABLEGC, x,y);
}

void EZ_RenderPixmapLabelWithGC(bitmap, drawable, gc, x,y)
     EZ_Bitmap *bitmap;
     Drawable drawable;
     GC       gc;
     int      x,y;
{
  if(EZ_BitmapType(bitmap) == EZ_BITMAP_IS_BITMAP)
    {
      int width = bitmap->width;
      int height = bitmap->height;
      if(bitmap->htpixmap == None)
	bitmap->htpixmap = XCreatePixmap(EZ_Display, drawable, width, height, EZ_Depth);    
      XCopyPlane(EZ_DisplayForWidgets,
		 bitmap->pixmap,
		 bitmap->htpixmap,
		 gc,
		 0,0, width, height,
		 0,0,
		 1);
      XSetClipMask(EZ_Display, gc, bitmap->pixmap);
      XSetClipOrigin(EZ_Display, gc,x,y);
      XCopyArea(EZ_DisplayForWidgets,
		bitmap->htpixmap,
		drawable,
		gc,
		0,0, width, height,
		x,y);
      XSetClipMask(EZ_Display, gc, None);
    }
  else 
    {
      if(bitmap->shape != (Pixmap)NULL)
	{
	  XSetClipMask(EZ_Display, gc, bitmap->shape);
	  XSetClipOrigin(EZ_Display, gc,x,y);
	}
      XCopyArea(EZ_DisplayForWidgets,
		bitmap->pixmap,
		drawable,
		gc,
		0,0,
		(unsigned int)(bitmap->width),
		(unsigned int)(bitmap->height),
		x,y);
      if(bitmap->shape != (Pixmap)NULL) XSetClipMask(EZ_Display, gc, None);
    }
}

int  EZ_GetBackgroundTileGC(wptr, gcptr, pv, hlight, specialhlight)
     EZ_Widget *wptr;
     GC        *gcptr;
     unsigned long *pv;
     int       hlight, specialhlight;
{
  int tile =  EZ_GetBackgroundGC(wptr, gcptr, pv, hlight, specialhlight);
  if(tile)
    {
      int ox, oy;
      EZ_GetWidgetTileOrigin(wptr, &ox, &oy);      
      XSetTSOrigin(EZ_Display, *gcptr, ox, oy);
    }
  return(tile);
}
/*
 *  set the GC for rendering background
 */
int  EZ_GetBackgroundGC(wptr, gcptr, pv, hlight, specialhlight)
     EZ_Widget *wptr;
     GC        *gcptr;
     unsigned long *pv;
     int       hlight, specialhlight;
{
  int kind = 0;
  if(wptr)
    {
      if((hlight && EZ_GetWidgetHighlightFlag(wptr)) || specialhlight)
	{
	  if(EZ_WidgetBackground(wptr) != 0)
	    {
	      *pv = EZ_GetHighlightColor(EZ_WidgetBackground(wptr));
	      XSetForeground(EZ_Display, EZ_WRITABLEGC, *pv);
	      *gcptr = EZ_WRITABLEGC;
	    }
	  else if(EZ_WidgetBGPixmap(wptr) != NULL)
	    {
	      EZ_Bitmap *bitmap = EZ_WidgetBGPixmap(wptr);
	      XSetTile(EZ_Display,EZ_NTILEGC, bitmap->htpixmap);
              *pv = EZ_ColorArray[EZ_NORMAL_GRAY2];
	      *gcptr = EZ_NTILEGC;
	      kind = 1;
	    }
	  else if(EZ_WidgetParentBG(wptr) && *(EZ_WidgetParentBG(wptr)))
	    {
	      *pv = EZ_GetHighlightColor(*(EZ_WidgetParentBG(wptr)));
	      XSetForeground(EZ_Display, EZ_WRITABLEGC, *pv);
	      *gcptr = EZ_WRITABLEGC;
	    }
	  else if(EZ_WidgetParentBGPixmap(wptr) != NULL)
	    {
	      EZ_Bitmap *bitmap = EZ_WidgetParentBGPixmap(wptr);
	      XSetTile(EZ_Display,EZ_NTILEGC, bitmap->htpixmap);
              *pv = EZ_ColorArray[EZ_NORMAL_GRAY2];
	      *gcptr = EZ_NTILEGC;
	      kind = 2;
	    }
	  else 
	    {
	      *pv = EZ_ColorArray[EZ_NORMAL_GRAY2];
	      *gcptr = EZ_NORMALGC2;
	    }
	}
      else if(EZ_WidgetBackground(wptr) != 0)
	{
	  *pv = EZ_WidgetBackground(wptr);
	  XSetForeground(EZ_Display, EZ_WRITABLEGC, *pv);
	  *gcptr = EZ_WRITABLEGC;
	}
      else if(EZ_WidgetBGPixmap(wptr) != NULL)
	{
	  EZ_Bitmap *bitmap = EZ_WidgetBGPixmap(wptr);
	  XSetTile(EZ_Display,EZ_NTILEGC, bitmap->pixmap);
	  *pv = EZ_ColorArray[EZ_NORMAL_GRAY1];
	  *gcptr = EZ_NTILEGC;
	  kind = 1;
	}
      else if(EZ_WidgetParentBG(wptr) && *(EZ_WidgetParentBG(wptr)))
	{
	  *pv = *(EZ_WidgetParentBG(wptr));
	  XSetForeground(EZ_Display, EZ_WRITABLEGC, *pv);
	  *gcptr = EZ_WRITABLEGC;
	}
      else if(EZ_WidgetParentBGPixmap(wptr) != NULL)
	{
	  EZ_Bitmap *bitmap = EZ_WidgetParentBGPixmap(wptr);
	  XSetTile(EZ_Display,EZ_NTILEGC, bitmap->pixmap);
	  *pv = EZ_ColorArray[EZ_NORMAL_GRAY1];
	  *gcptr = EZ_NTILEGC;
	  kind = 2;
	}       
      else 
	{
	  *pv = EZ_ColorArray[EZ_NORMAL_GRAY1];
	  *gcptr = EZ_NORMALGC1;
	}
    }
  return(kind);
}

int EZ_GetParentBgGC(wptr, gcptr, pv)
     EZ_Widget *wptr;
     GC        *gcptr;
     unsigned long *pv;
{
  int kind = 0;
  if(wptr)
    {
      EZ_Widget *tmp = EZ_WidgetParent(wptr);
      if(tmp) wptr = tmp;
      if(EZ_WidgetBackground(wptr) != 0)
	{
	  *pv = EZ_WidgetBackground(wptr); 
	  XSetForeground(EZ_Display, EZ_WRITABLEGC2, *pv);
	  *gcptr = EZ_WRITABLEGC2;
	}
      else if(EZ_WidgetBGPixmap(wptr))
	{
	  EZ_Bitmap *bitmap = EZ_WidgetBGPixmap(wptr);
	  XSetTile(EZ_Display,EZ_NTILEGC,bitmap->pixmap);	  
	  if(tmp) kind = 2; else kind = 1;
	  *gcptr = EZ_NTILEGC;
	  *pv = EZ_ColorArray[EZ_NORMAL_GRAY1];
	}
      else if(EZ_WidgetParentBG(wptr) && *(EZ_WidgetParentBG(wptr)))
	{
	  *pv = *(EZ_WidgetParentBG(wptr));
	  XSetForeground(EZ_Display, EZ_WRITABLEGC2, *pv);
	  *gcptr = EZ_WRITABLEGC2;
	}
      else if(EZ_WidgetParentBGPixmap(wptr))
	{
	  EZ_Bitmap *bitmap = EZ_WidgetParentBGPixmap(wptr);
	  XSetTile(EZ_Display,EZ_NTILEGC,bitmap->pixmap);	  
	  if(tmp) kind = 2; else kind = 1;
	  *gcptr = EZ_NTILEGC;
	  *pv = EZ_ColorArray[EZ_NORMAL_GRAY1];
	}
      else 
	{
	  *pv = EZ_ColorArray[EZ_NORMAL_GRAY1];
	  XSetForeground(EZ_Display, EZ_WRITABLEGC2, *pv);
	  *gcptr = EZ_WRITABLEGC2;
	}
    }
  return(kind);
}

int EZ_GetParentBgTileGC(wptr, gcptr, pv)
     EZ_Widget *wptr;
     GC        *gcptr;
     unsigned long *pv;
{
  int tile = EZ_GetParentBgGC(wptr, gcptr, pv);
  if(tile)
    {
      int tx, ty;
      EZ_Widget *p = EZ_WidgetParent(wptr);
      if(p == NULL) p = wptr;
      EZ_GetWidgetTileOrigin(p, &tx, &ty);
      if(tile > 1)
        { tx -= EZ_WidgetOriginX(wptr);  ty -= EZ_WidgetOriginY(wptr); }
      XSetTSOrigin(EZ_Display, *gcptr, tx, ty);
    }
  return(tile);
}

int EZ_GetDarkBrightNormalBDGC(widget, darkgc, brightgc, normalgc)
     EZ_Widget *widget;
     GC        *darkgc, *brightgc, *normalgc;
{
  unsigned long pixel, dbg;
  int kind = 0;
  if(EZ_WidgetBackground(widget))
    {
      dbg = EZ_WidgetBackground(widget);
      pixel = EZ_GetDarkColor(dbg);
      XSetForeground(EZ_Display, EZ_BD_DARKGC, pixel);
      *darkgc = EZ_BD_DARKGC;
      pixel = EZ_GetBrightColor(dbg);
      XSetForeground(EZ_Display, EZ_BD_BRIGHTGC, pixel);
      *brightgc = EZ_BD_BRIGHTGC;      
      XSetForeground(EZ_Display, EZ_BD_NORMALGC, dbg);
      *normalgc = EZ_BD_NORMALGC; 
    }
  else if(EZ_WidgetBGPixmap(widget))
    {
      EZ_Bitmap *bitmap = EZ_WidgetBGPixmap(widget);
      *darkgc = EZ_DTILEGC1;
      *brightgc = EZ_BTILEGC1;
      *normalgc = EZ_NTILEGC1;
      XSetTile(EZ_Display,EZ_NTILEGC1,bitmap->pixmap);	        
      XSetTile(EZ_Display,EZ_DTILEGC1,bitmap->dkpixmap);	        
      XSetTile(EZ_Display,EZ_BTILEGC1,bitmap->btpixmap); 
      kind = 1;
    }
  else if(EZ_WidgetParentBG(widget) && *(EZ_WidgetParentBG(widget)))
    {
      dbg = *(EZ_WidgetParentBG(widget));
      pixel = EZ_GetDarkColor(dbg);
      XSetForeground(EZ_Display, EZ_BD_DARKGC, pixel);
      *darkgc = EZ_BD_DARKGC;
      pixel = EZ_GetBrightColor(dbg);
      XSetForeground(EZ_Display, EZ_BD_BRIGHTGC, pixel);
      *brightgc = EZ_BD_BRIGHTGC;
      XSetForeground(EZ_Display, EZ_BD_NORMALGC, dbg);
      *normalgc = EZ_BD_NORMALGC;      
    }
  else if(EZ_WidgetParentBGPixmap(widget))
    {
      EZ_Bitmap *bitmap = EZ_WidgetParentBGPixmap(widget);
      *darkgc = EZ_DTILEGC1;
      *brightgc = EZ_BTILEGC1;
      *normalgc = EZ_NTILEGC1;
      XSetTile(EZ_Display,EZ_NTILEGC1,bitmap->pixmap);	        
      XSetTile(EZ_Display,EZ_DTILEGC1,bitmap->dkpixmap);	        
      XSetTile(EZ_Display,EZ_BTILEGC1,bitmap->btpixmap); 
      kind = 2;
    }
  else
    {
      *darkgc = EZ_DARKGC2;
      *brightgc = EZ_BRIGHTGC1;
      *normalgc = EZ_NORMALGC1;
    }
  return(kind);
}

int EZ_GetDarkBDGC(widget, gcptr)
     EZ_Widget *widget;
     GC        *gcptr;
{
  unsigned long pixel;
  int kind = 0;
  if(EZ_WidgetBackground(widget))
    {
      pixel = EZ_GetDarkColor(EZ_WidgetBackground(widget));
      XSetForeground(EZ_Display, EZ_BD_DARKGC, pixel);
      *gcptr = EZ_BD_DARKGC;
    }
  else if(EZ_WidgetBGPixmap(widget))
    {
      EZ_Bitmap *bitmap = EZ_WidgetBGPixmap(widget);
      *gcptr = EZ_DTILEGC1;
      XSetTile(EZ_Display,EZ_DTILEGC1,bitmap->dkpixmap);	        
      kind = 1;
    }  
  else if(EZ_WidgetParentBG(widget) && *(EZ_WidgetParentBG(widget)))
    {
      pixel = EZ_GetDarkColor(*(EZ_WidgetParentBG(widget)));
      XSetForeground(EZ_Display, EZ_BD_DARKGC, pixel);
      *gcptr = EZ_BD_DARKGC;
    }
  else if(EZ_WidgetParentBGPixmap(widget))
    {
      EZ_Bitmap *bitmap = EZ_WidgetParentBGPixmap(widget);
      *gcptr = EZ_DTILEGC1;
      XSetTile(EZ_Display,EZ_DTILEGC1,bitmap->dkpixmap);	        
      kind = 2;
    }  
  else *gcptr = EZ_DARKGC2;
  return(kind);
}

int EZ_GetBrightBDGC(widget, gcptr)
     EZ_Widget *widget;
     GC        *gcptr;
{
  unsigned long pixel;
  int kind = 0;
  if(EZ_WidgetBackground(widget))
    {
      pixel = EZ_GetBrightColor(EZ_WidgetBackground(widget));
      XSetForeground(EZ_Display, EZ_BD_BRIGHTGC, pixel);
      *gcptr = EZ_BD_BRIGHTGC;
    }
  else if(EZ_WidgetBGPixmap(widget))
    {
      EZ_Bitmap *bitmap = EZ_WidgetBGPixmap(widget);
      *gcptr = EZ_BTILEGC1;
      XSetTile(EZ_Display,EZ_DTILEGC1,bitmap->btpixmap);	        
      kind = 1;
    }  
  else if(EZ_WidgetParentBG(widget) && *(EZ_WidgetParentBG(widget)))
    {
      pixel = EZ_GetBrightColor(*(EZ_WidgetParentBG(widget)));
      XSetForeground(EZ_Display, EZ_BD_BRIGHTGC, pixel);
      *gcptr = EZ_BD_BRIGHTGC;
    }
  else if(EZ_WidgetParentBGPixmap(widget))
    {
      EZ_Bitmap *bitmap = EZ_WidgetParentBGPixmap(widget);
      *gcptr = EZ_BTILEGC1;
      XSetTile(EZ_Display,EZ_DTILEGC1,bitmap->btpixmap);	        
      kind = 2;
    }  
  else *gcptr = EZ_BRIGHTGC1;
  return(kind);
}

void EZ_GetDarkBDpv(widget, ptr)
     EZ_Widget *widget;
     unsigned long   *ptr;
{
  unsigned long pixel;
  if(EZ_WidgetBackground(widget))
    pixel = EZ_GetDarkColor(EZ_WidgetBackground(widget));
  else if(EZ_WidgetParentBG(widget) && *(EZ_WidgetParentBG(widget)))
    pixel = EZ_GetDarkColor(*(EZ_WidgetParentBG(widget)));
  else pixel = EZ_ColorArray[EZ_DARK_GRAY2];
  *ptr = pixel;
}

void EZ_GetBrightBDpv(widget, ptr)
     EZ_Widget *widget;
     unsigned long   *ptr;
{
  unsigned long pixel;
  if(EZ_WidgetBackground(widget))
    pixel = EZ_GetBrightColor(EZ_WidgetBackground(widget));
  else if(EZ_WidgetParentBG(widget) && *(EZ_WidgetParentBG(widget)))
    pixel = EZ_GetBrightColor(*(EZ_WidgetParentBG(widget)));
  else pixel = EZ_ColorArray[EZ_BRIGHT_GRAY2];
  *ptr = pixel;
}


void  EZ_GetBackgroundPV(wptr,pv, hlight, specialhlight)
     EZ_Widget *wptr;
     unsigned long *pv;
     int       hlight, specialhlight;
{
  if(wptr)
    {
      int set = 0;
      switch(EZ_WidgetType(wptr))
	{
	case EZ_WIDGET_ITREE:
	  if(EZ_ITreeWTextBG(wptr) != ~0)
	    {
	      set = 1;
	      *pv = EZ_ITreeWTextBG(wptr);
	    }
	  break;
	case EZ_WIDGET_ITEXT:
	  if(EZ_ITextTextBG(wptr) != ~0)
	    {
	      set = 1;
	      *pv = EZ_ITextTextBG(wptr);
	    }
	  break;
	case EZ_WIDGET_ILIST_BOX:
	  if(EZ_IListBoxTextBG(wptr) != ~0)
	    {
	      set = 1;
	      *pv= EZ_IListBoxTextBG(wptr);
	    }
	  break;
	case EZ_WIDGET_IWORK_AREA:
	  if(EZ_IWorkAreaTextBG(wptr) != ~0)
	    {
	      set = 1;
	      *pv= EZ_IWorkAreaTextBG(wptr);
	    }
	  break;
	case EZ_WIDGET_IFANCY_LIST_BOX:	  
	  if(EZ_IFListBoxTextBG(wptr) != ~0)
	    {
	      set = 1;
	      *pv = EZ_IFListBoxTextBG(wptr);
	    }
	  break;
	case EZ_WIDGET_ITERM:
	  if(EZ_ITermTextBG(wptr) != ~0)
	    {
	      set = 1;
	      *pv = EZ_ITermTextBG(wptr);
	    }
	  break;
	default:
	  break;
	}
      if(set == 0)
	{
	  if((hlight && EZ_GetWidgetHighlightFlag(wptr)) || specialhlight)
	    {
	      if(EZ_WidgetBackground(wptr) != 0)
		*pv = EZ_GetHighlightColor(EZ_WidgetBackground(wptr));
	      else if(EZ_WidgetParentBG(wptr) && *(EZ_WidgetParentBG(wptr)))
		*pv = EZ_GetHighlightColor(*(EZ_WidgetParentBG(wptr)));
	      else 
		*pv = EZ_ColorArray[EZ_NORMAL_GRAY2];
	    }
	  else if(EZ_WidgetBackground(wptr) != 0)
	    *pv = EZ_WidgetBackground(wptr);
	  else if(EZ_WidgetParentBG(wptr) && *(EZ_WidgetParentBG(wptr)))
	    *pv = *(EZ_WidgetParentBG(wptr));
	  else 
	    *pv = EZ_ColorArray[EZ_NORMAL_GRAY1];
	}
    }
  else
    {
      *pv = EZ_ColorArray[EZ_NORMAL_GRAY1];
    }
}
/**************************************************************************
 * 
 * Mark children widgets that their size may  not be current
 *
 **************************************************************************/
void EZ_MarkAllChildrenWidgetSizeChanged(widget, sibling_also)
     EZ_Widget *widget; int sibling_also;
{
  EZ_Widget *children, *sibling;
  
  if(EZ_LookupWidgetFromAllHT(widget) == (EZ_Widget *)NULL) return;

  /*-------------------------------------
   * recursively mark all children widgets
   * and sibling widgets.
   *------------------------------------*/
  EZ_SetWidgetSizeMayBeChangedFlag(widget);
  children = EZ_WidgetChildren(widget);
  EZ_MarkAllChildrenWidgetSizeChanged(children,1);
  if(sibling_also)
    {
      sibling = EZ_WidgetSibling(widget);
      EZ_MarkAllChildrenWidgetSizeChanged(sibling,1);
    }
  if( EZ_WidgetWindow(widget) != (Window )NULL)
    {
      /*--------------------------------------
       * if window is maped, mark that it need
       * to be MoveResized.
       *-------------------------------------*/
      if(EZ_WidgetMaped(widget) != 0)
	EZ_WidgetMaped(widget) = 2;
    }
}

/***************************************************************
 *
 *  Widget Configuration functions.
 *
 ***************************************************************/
void EZ_SetWidgetCallBack(widget,callback, data)
     EZ_Widget *widget;
     EZ_CallBack callback;
     void *data;
{
  /* set callback to be the only callback, for compatibility */
  EZ_RemoveAllWidgetCallBacks(widget, EZ_CALLBACK);
  EZ_AddWidgetCallBack(widget,EZ_CALLBACK, callback, data, 0);
}
/*****************************************************************************/
void EZ_AddCallBackWork(funcList,callback, data, where, flag)
     EZ_CallBackStruct **funcList;
     EZ_CallBack callback;
     void *data;
     int flag ;  /* flag=1 means this callback is internal */
     int where; /* 0: insert at the front, !=0: insert at the end */
{
  /* 081899, don't want to insert the same callback twice */
  EZ_RemoveCallBackWork(funcList,callback, data);
  if(*funcList == NULL)
    {
      *funcList =  (EZ_CallBackStruct *)my_malloc(sizeof(EZ_CallBackStruct), _CALL_BACK_);
      if(!(*funcList))  EZ_OutOfMemory("EZ_AddCallBackWork");
      (*funcList)->callback = callback;
      (*funcList)->data = data;
      (*funcList)->next = NULL;
      (*funcList)->internal = flag;
    }
  else
    {
      EZ_CallBackStruct *tmp = *funcList;
      while(tmp) {if(tmp->callback== callback && tmp->data== data) {return; break;} tmp= tmp->next;}
      
      tmp = (EZ_CallBackStruct *)my_malloc(sizeof(EZ_CallBackStruct), _CALL_BACK_);     
      tmp->callback = callback;
      tmp->data = data;
      tmp->internal = flag;
      tmp->next = NULL;
      if(where <= 0 || flag != 0) /* insert at the front */
	{
	  EZ_CallBackStruct *f = *funcList;
	  if(where < 0) /* add infront of all callbacks, including internal callbacks */
	    {
	      tmp->next = *funcList;
	      *funcList = tmp;
	      tmp->internal = where;
	    }
	  else if(flag != 0) /* internal callback */
	    {
	      EZ_CallBackStruct *chase = f;
	      while(f && f->internal < 0)
		{
		  chase = f;
		  f = f->next;
		}
	      f = chase->next;
	      chase->next = tmp;
	      tmp->next = f;
	    }
	  else /* insert at the end of internal callbacks */
	    {
	      EZ_CallBackStruct *chase = f;
	      while(f && f->internal != 0)
		{
		  chase = f;
		  f = f->next;
		}
	      f = chase->next;
	      chase->next = tmp;
	      tmp->next = f;
	    }
	}
      else
	{
	  EZ_CallBackStruct *chase = *funcList;	  
	  while(chase->next != NULL) chase = chase->next;
	  chase->next = tmp;
	  tmp->next = NULL;
	}
    }
}
/*****************************************************************************/
void EZ_RemoveCallBackWork(funcList,callback, data)
     EZ_CallBackStruct **funcList;
     EZ_CallBack callback;
     void *data;
{
  if(*funcList == NULL) return;
  else
    {
      int found = 0;
      EZ_CallBackStruct *chase, *tmp;
      chase = tmp = *funcList;
      while(tmp)
	{
	  if(tmp->callback == callback && tmp->data == data)
	    { found = 1; break;}
	  else
	    {
	      chase = tmp; 
	      tmp = tmp->next;
	    }
	}
      if(found)
	{
	  if(tmp == *funcList) /* the first one */
            *funcList = tmp->next; 
	  else
            chase->next = tmp->next;
	  (void)my_free((char *)tmp);
	}
    }
}
/*****************************************************************************/
void EZ_RemoveAllCallBacksWork(funcList)
     EZ_CallBackStruct **funcList;
{
  if(*funcList == NULL) return;
  else
    {
      EZ_CallBackStruct *next, *save = NULL, *tmp = *funcList, *newhead = NULL;

      while(tmp)
	{
	  next = tmp->next;
	  if(tmp->internal>0)
	    { if(newhead == NULL) newhead = tmp; save = tmp;}
	  else (void)my_free((char *)tmp);
	  tmp = next;
	}
      if(save) save->next = NULL;
      *funcList = newhead;
    }
}
/*****************************************************************************/
EZ_CallBackStruct *EZ_TheIthCallBack(funcList, nn)
     EZ_CallBackStruct **funcList;
     int nn;
{
  if(*funcList == NULL || nn < 0) return(NULL);
  else
    {
      EZ_CallBackStruct *tmp;
      int i = 0;
      tmp = *funcList;
      while(tmp)
	{
          if(i == nn) return(tmp);
          tmp = tmp->next;
          i++;
	}
    }
  return(NULL);
}
/*****************************************************************************/

void EZ_MoveWidgetWindow(widget, x,y)
     EZ_Widget *widget;
     int       x,y;
{
  if(EZ_LookupWidgetFromAllHT(widget))
    {
      EZ_SetWidgetXSetFlag(widget);
      EZ_SetWidgetYSetFlag(widget);
      EZ_WidgetOriginX(widget) = x;
      EZ_WidgetOriginY(widget) = y;
      EZ_ClearWidgetSizeComputedFlag(widget);
      EZ_SetWidgetSizeMayBeChangedFlag(widget);
      if(EZ_WidgetMapped(widget))
        XMoveWindow(EZ_Display, EZ_WidgetWindow(widget),x ,y);
    }
}

void EZ_ResizeWidgetWindow(widget, w,h)
     EZ_Widget *widget;
     int       w,h;
{
  if(EZ_LookupWidgetFromAllHT(widget))
    {
      EZ_WidgetWidthHint(widget) = w;
      EZ_WidgetHeightHint(widget) = h;
      EZ_SetWidgetSizeSetFlag(widget);
      EZ_ClearWidgetSizeComputedFlag(widget);
      EZ_SetWidgetSizeMayBeChangedFlag(widget);
      if(EZ_WidgetMapped(widget))
        {
          EZ_WidgetWidth(widget) = w;
          EZ_WidgetHeight(widget) = h;
          XResizeWindow(EZ_Display, EZ_WidgetWindow(widget), w ,h);
        }      
    }
}

void EZ_MoveResizeWidgetWindow(widget, x,y, w,h)
     EZ_Widget *widget;
     int       x,y, w,h;

{
  if(EZ_LookupWidgetFromAllHT(widget))
    {
      EZ_SetWidgetXSetFlag(widget);
      EZ_SetWidgetYSetFlag(widget);
      EZ_WidgetOriginX(widget) = x;
      EZ_WidgetOriginY(widget) = y;
      EZ_WidgetWidthHint(widget) = w;
      EZ_WidgetHeightHint(widget) = h;
      EZ_SetWidgetSizeSetFlag(widget);
      EZ_SetWidgetSizeMayBeChangedFlag(widget);
      EZ_ClearWidgetSizeComputedFlag(widget);
      if(EZ_WidgetMapped(widget))
        {
          EZ_WidgetWidth(widget) = w;
          EZ_WidgetHeight(widget) = h;
          XMoveResizeWindow(EZ_Display, EZ_WidgetWindow(widget), x, y, w ,h);
        }      
    }
}

void  EZ_SetWidgetPosition(widget, x,y)
     EZ_Widget *widget;
     int       x,y;
{
  if(EZ_LookupWidgetFromAllHT(widget))
    {
      EZ_SetWidgetXSetFlag(widget);
      EZ_SetWidgetYSetFlag(widget);
      EZ_WidgetOriginX(widget) = x;
      EZ_WidgetOriginY(widget) = y;
      EZ_ClearWidgetSizeComputedFlag(widget);
      if(EZ_WidgetMapped(widget)) EZ_ReDisplayWidget(widget);
    }
}

void  EZ_SetWidgetWidth(widget,width)
     EZ_Widget *widget;
     int             width;
{
  if(EZ_LookupWidgetFromAllHT(widget))
    {
      EZ_WidgetWidthHint(widget) = width;
      EZ_SetWidgetWidthSetFlag(widget);
      EZ_ClearWidgetSizeComputedFlag(widget);
      if(EZ_WidgetMapped(widget)) EZ_ReDisplayWidget(widget);
    }
}

void  EZ_SetWidgetHeight(widget,height)
     EZ_Widget *widget;
     int             height;
{
  if(EZ_LookupWidgetFromAllHT(widget))
    { 
      EZ_WidgetHeightHint(widget) = height;
      EZ_SetWidgetHeightSetFlag(widget);
      EZ_ClearWidgetSizeComputedFlag(widget);
      if(EZ_WidgetMapped(widget)) EZ_ReDisplayWidget(widget);
    }
}

int EZ_GetWidgetWidth(widget)
     EZ_Widget *widget;
{
  if(EZ_LookupWidgetFromAllHT(widget))
    return(EZ_WidgetWidth(widget));
  else return(0);
}
int EZ_GetWidgetMinWidth(widget)
     EZ_Widget *widget;
{
  if(EZ_LookupWidgetFromAllHT(widget))
    return(EZ_WidgetMinWidth(widget));
  else return(0);
}

int EZ_GetWidgetHeight(widget)
     EZ_Widget *widget;
{
  if(EZ_LookupWidgetFromAllHT(widget))
    return(EZ_WidgetHeight(widget));
  else return(0);
}

int EZ_GetWidgetMinHeight(widget)
     EZ_Widget *widget;
{
  if(EZ_LookupWidgetFromAllHT(widget))
    return(EZ_WidgetMinHeight(widget));
  else return(0);
}

int EZ_GetWidgetBorderWidth(widget)
     EZ_Widget *widget;
{
  if(EZ_LookupWidgetFromAllHT(widget))
    return(EZ_WidgetBorderWidth(widget));
  else return(0);
}

void EZ_GetWidgetSize(widget, w, h)
     EZ_Widget *widget;
     int       *w, *h;
{
  if(EZ_LookupWidgetFromAllHT(widget))
    {
      *w = EZ_WidgetWidth(widget);
      *h = EZ_WidgetHeight(widget);
    }
}

void EZ_GetWidgetDimension(widget, w, h)
     EZ_Widget *widget;
     int       *w, *h;
{
  if(EZ_LookupWidgetFromAllHT(widget))
    {
      *w = EZ_WidgetWidth(widget);
      *h = EZ_WidgetHeight(widget);
    }
}

void EZ_GetWidgetGeometry(widget, x,y,w, h)
     EZ_Widget *widget;
     int       *x,*y, *w, *h;
{
  if(EZ_LookupWidgetFromAllHT(widget))
    {
      *x = EZ_WidgetOriginX(widget);
      *y = EZ_WidgetOriginY(widget);
      *w = EZ_WidgetWidth(widget);
      *h = EZ_WidgetHeight(widget);
    }
}

void EZ_GetWidgetPosition(widget, x, y)
     EZ_Widget *widget;
     int       *x, *y;
{
  if(EZ_LookupWidgetFromAllHT(widget))
    {
      *x = EZ_WidgetOriginX(widget);
      *y = EZ_WidgetOriginY(widget);
    }
}

void EZ_GetWidgetMinDimension(widget, w, h)
     EZ_Widget *widget;
     int       *w, *h;
{
  if(EZ_LookupWidgetFromAllHT(widget))
    {
      *w = EZ_WidgetMinWidth(widget);
      *h = EZ_WidgetMinHeight(widget);
    }
}
/***************************************************************
 *
 *  Enable/disable a widget tree.
 *
 */
static void EZ_DisableWidgetTreeWork(widget, flag) EZ_Widget *widget; int flag;
{
  if(widget)
    {
      EZ_Widget *children = EZ_WidgetChildren(widget);
      EZ_DisableWidget(widget);
      EZ_DisableWidgetTreeWork(children, 1);
      if(flag)
	{
	  EZ_Widget *sibling = EZ_WidgetSibling(widget);
	  EZ_DisableWidgetTreeWork(sibling, 1);
	}
    }
}
static void EZ_EnableWidgetTreeWork(widget,flag) EZ_Widget *widget; int flag;
{
  if(widget)
    {
      EZ_Widget *children = EZ_WidgetChildren(widget);
      EZ_EnableWidget(widget);
      EZ_EnableWidgetTreeWork(children, 1);
      if(flag)
	{
	  EZ_Widget *sibling = EZ_WidgetSibling(widget);
	  EZ_EnableWidgetTreeWork(sibling, 1);
	}
    }
}
void EZ_DisableWidgetTree(widget) EZ_Widget *widget;
{ 
  if(widget)
    {
      EZ_FreezeWidget(widget);
      EZ_DisableWidgetTreeWork(widget, 0); 
      EZ_UnFreezeWidget(widget);
    }
}
void EZ_EnableWidgetTree(widget) EZ_Widget *widget;
{
  if(widget)
    {
      EZ_FreezeWidget(widget);
      EZ_EnableWidgetTreeWork(widget, 0); 
      EZ_UnFreezeWidget(widget);
    }
}
/***************************************************************
 *
 *   De-activate  and activate a widget. 
 *
 */
void EZ_DisableWidget(widget)
     EZ_Widget *widget;
{
  if(EZ_LookupWidgetFromAllHT(widget) &&
     EZ_GetWidgetDisabledFlag(widget) == 0 &&
     widget != EZ_GrabedWidget) /* can we disable the current grab? */
    {
      int type = EZ_WidgetType(widget);

      EZ_SetWidgetDisabledFlag(widget);
      EZ_ClearWidgetHighlightFlag(widget);
      { EZ_UninstallHighlightColor(widget); } /*4/29/99*/
      if(type >= EZ_WIDGET_MENU_SEPARATOR && type <= EZ_WIDGET_MENU_RADIO_BUTTON)
	{
	  EZ_Widget *clone = EZ_GetMenuItemCompaion(widget);	  
	  EZ_WidgetBorderStyle(widget) = EZ_BORDER_NONE;
	  if(clone) EZ_DisableWidget(clone);
	}
      else if(type == EZ_WIDGET_NB_PAGE)
	{
	  EZ_Widget *companion = EZ_NBPageCompanion(widget);
	  EZ_DisableWidget(companion);
	}
      if(EZ_WidgetMapped(widget)) EZ_ReDisplayWidget(widget);
    }
}

void  EZ_EnableWidget(widget)
     EZ_Widget *widget;
{
  if(EZ_LookupWidgetFromAllHT(widget) &&
     EZ_GetWidgetDisabledFlag(widget))
    {
      int type = EZ_WidgetType(widget);
      EZ_ClearWidgetDisabledFlag(widget);
      if(type == EZ_WIDGET_NB_PAGE)
	{
	  EZ_Widget *companion = EZ_NBPageCompanion(widget);
	  EZ_ClearWidgetDisabledFlag(companion);
	}
      else if(type >= EZ_WIDGET_MENU_SEPARATOR && type <= EZ_WIDGET_MENU_RADIO_BUTTON)
	{
	  EZ_Widget *clone = EZ_GetMenuItemCompaion(widget);
          if(clone) EZ_EnableWidget(clone);
	}
      if(EZ_WidgetMapped(widget)) EZ_ReDisplayWidget(widget);
    }
}
/***************************************************************
 *
 *   Change the label string for a  button-like widget.
 *
 */
char *EZ_GetWidgetLabelString(widget)
     EZ_Widget *widget;
{
  if(EZ_LookupWidgetFromAllHT(widget) == (EZ_Widget *)NULL) return(NULL);
  return(EZ_LabelString(widget));
}

int EZ_SetWidgetLabelString(widget,str)
     EZ_Widget *widget;
     char            *str;
{
  int  underline, rlen=0, len = 0, wtype;
  char tmp[4];

  if(EZ_LookupWidgetFromAllHT(widget) == (EZ_Widget *)NULL) return(0);

  if(str != (char *)NULL) rlen=len = strlen(str);
  if(len == 0)
    { 
      len = 1;
      tmp[0]='\0'; tmp[1] = '\0';
      str = tmp;
    }
  
  wtype = EZ_WidgetType(widget);
  switch(wtype)
    {
    case EZ_WIDGET_FRAME:
      if(EZ_FrameLabelLength(widget) < len)
	{
	  EZ_FrameLabel(widget) = (char *)my_realloc(EZ_FrameLabel(widget),len+1, _LABEL_STRING_);
	  if(!EZ_FrameLabel(widget)) EZ_OutOfMemory("EZ_SetWidgetLabelString");
	}
      EZ_FrameLabelLength(widget) = len;
      (void)strcpy(EZ_FrameLabel(widget),str); 
      break;
    case EZ_WIDGET_POPUP_MENU:
      {
	EZ_Widget *lab = EZ_WidgetChildren(widget);
	if(lab != NULL && EZ_WidgetType(lab) == EZ_WIDGET_LABEL)
	  {
	    if(str) {EZ_ClearWidgetNonActiveFlag(lab);}
	    else {  EZ_SetWidgetNonActiveFlag(lab); }
	    EZ_ClearWidgetSizeComputedFlag(EZ_WidgetParent(lab));
	    widget = lab;
	  }
	else break;
      }
    case EZ_WIDGET_ITOPLEVEL:
    case EZ_WIDGET_LABEL:
    case EZ_WIDGET_NW_LABEL:
      if(wtype == EZ_WIDGET_ITOPLEVEL &&
	 ( (len != EZ_LabelStringLength(widget) ||
	    strcmp(str, EZ_LabelString(widget))) ) )
	{ EZ_LabelTextMaskDirty(widget) = 1;  EZ_LabelMaskDirty(widget) = 1; }
      if(EZ_LabelStringLength(widget) < len)
	{
	  EZ_LabelString(widget) = (char *)my_realloc(EZ_LabelString(widget),len+1, _LABEL_STRING_);
	  if(!EZ_LabelString(widget)) EZ_OutOfMemory("EZ_SetWidgetLabelString");
	}
      EZ_LabelStringLength(widget) = len;
      (void)strcpy(EZ_LabelString(widget),str);
      underline =  EZ_LabelUnderLine(widget);
      if(underline >= 0)
	{
	  if(underline < len && isprint(str[underline]))
	    {
	      EZ_LabelModifiers(widget) = Mod1Mask;
	      EZ_LabelShortcut(widget)  = (EZ_LabelString(widget) + underline);
	      EZ_InsertGlobalKeyPressEvent(widget, Mod1Mask, (str+underline));
	    }
	  else
	    {
	      EZ_LabelUnderLine(widget) = -1;
	      EZ_LabelModifiers(widget) = 0;
	      EZ_LabelShortcut(widget)  = (char *)NULL;
	    }
	}
      break;
    case EZ_WIDGET_NORMAL_BUTTON:
    case EZ_WIDGET_ARROW_BUTTON:
    case EZ_WIDGET_MENU_NORMAL_BUTTON:
      if(EZ_ButtonLabelLength(widget) < len)
	{
	  EZ_ButtonLabel(widget) = (char *)my_realloc(EZ_ButtonLabel(widget),len+1, _LABEL_STRING_);
	  if(!EZ_ButtonLabel(widget)) EZ_OutOfMemory("EZ_SetWidgetLabelString");
	}
      EZ_CButtonLabelLength(widget) = len;
      (void)strcpy(EZ_ButtonLabel(widget),str); 
      underline =  EZ_ButtonUnderLine(widget);
      if(underline >= 0)
	{
	  if(underline < len && isprint(str[underline]))
	    {
	      EZ_ButtonModifiers(widget) = Mod1Mask;
	      EZ_ButtonShortcut(widget)  = (EZ_ButtonLabel(widget) + underline);
	      EZ_InsertGlobalKeyPressEvent(widget, Mod1Mask, (str+underline));
	    }
	  else
	    {
	      EZ_ButtonUnderLine(widget) = -1;
	      EZ_ButtonModifiers(widget) = 0;
	      EZ_ButtonShortcut(widget)  = (char *)NULL;
	    }
	}
      break;
    case EZ_WIDGET_CHECK_BUTTON:
    case EZ_WIDGET_MENU_CHECK_BUTTON:
      if(EZ_CButtonLabelLength(widget) < len)
	{
	  EZ_CButtonLabel(widget) = (char *)my_realloc(EZ_CButtonLabel(widget),len+1,_LABEL_STRING_);
	  if(!EZ_CButtonLabel(widget)) EZ_OutOfMemory("EZ_SetWidgetLabelString");
	}
      EZ_CButtonLabelLength(widget) = len;
      (void)strcpy(EZ_CButtonLabel(widget),str); 
      underline =  EZ_CButtonUnderLine(widget);
      if(underline >= 0)
	{
	  if(underline < len && isprint(str[underline]))
	    {
	      EZ_CButtonModifiers(widget) = Mod1Mask;
	      EZ_CButtonShortcut(widget)  = (EZ_CButtonLabel(widget) + underline);
	      EZ_InsertGlobalKeyPressEvent(widget, Mod1Mask, (str+underline));
	    }
	  else
	    {
	      EZ_CButtonUnderLine(widget) = -1;
	      EZ_CButtonModifiers(widget) = 0;
	      EZ_CButtonShortcut(widget)  = (char *)NULL;
	    }
	}
      break;
    case EZ_WIDGET_RADIO_BUTTON:
    case EZ_WIDGET_MENU_RADIO_BUTTON:
      if(EZ_RButtonLabelLength(widget) < len)
	{
	  EZ_RButtonLabel(widget) = (char *)my_realloc(EZ_RButtonLabel(widget),len+1,_LABEL_STRING_);
	  if(!EZ_RButtonLabel(widget)) EZ_OutOfMemory("EZ_SetWidgetLabelString");
	}
      EZ_RButtonLabelLength(widget) = len;
      (void)strcpy(EZ_RButtonLabel(widget),str); 
      underline =  EZ_RButtonUnderLine(widget);
      if(underline >= 0)
	{
	  if( underline < len && isprint(str[underline]))
	    {
	      EZ_RButtonModifiers(widget) = Mod1Mask;
	      EZ_RButtonShortcut(widget)  = (EZ_RButtonLabel(widget) + underline);
	      EZ_InsertGlobalKeyPressEvent(widget, Mod1Mask, (str+underline));
	    }
	  else 
	    {
	      EZ_RButtonUnderLine(widget) = -1;
	      EZ_RButtonModifiers(widget) = 0;
	      EZ_RButtonShortcut(widget)  = (char *)NULL;
	    }
	}
      break;
    case EZ_WIDGET_MENU_BUTTON:
      if(EZ_MButtonLabelLength(widget) < len)
	{
	  EZ_MButtonLabel(widget) = (char *)my_realloc(EZ_MButtonLabel(widget),len+1,_LABEL_STRING_);
	  if(!EZ_MButtonLabel(widget)) EZ_OutOfMemory("EZ_SetWidgetLabelString");
	}
      EZ_MButtonLabelLength(widget) = len;
      (void)strcpy(EZ_MButtonLabel(widget),str); 
      underline =  EZ_MButtonUnderLine(widget);
      if(underline >= 0)
	{
	  if(underline < len && isprint(str[underline]))
	    {
	      EZ_MButtonModifiers(widget) = Mod1Mask;
	      EZ_MButtonShortcut(widget)  = (EZ_MButtonLabel(widget) + underline);
	      EZ_InsertGlobalKeyPressEvent(widget, Mod1Mask, (str+underline));
	    }
	  else
	    {
	      EZ_MButtonUnderLine(widget) = -1;
	      EZ_MButtonModifiers(widget) = 0;
	      EZ_MButtonShortcut(widget)  = (char *)NULL;
	    }
	}
      break;
    case EZ_WIDGET_MENU_SUBMENU:
      if(EZ_SubMenuLabelLength(widget) < len)
	{
	  EZ_SubMenuLabel(widget) = (char *)my_realloc(EZ_SubMenuLabel(widget),len+1,_LABEL_STRING_);
	  if(!EZ_SubMenuLabel(widget)) EZ_OutOfMemory("EZ_SetWidgetLabelString");
	}
      EZ_SubMenuLabelLength(widget) = len;
      (void)strcpy(EZ_SubMenuLabel(widget),str); 
      underline =  EZ_SubMenuUnderLine(widget);
      if(underline >= 0)
	{
	  if(underline < len && isprint(str[underline]))
	    {

	      EZ_SubMenuModifiers(widget) = Mod1Mask;
	      EZ_SubMenuShortcut(widget)  = (EZ_SubMenuLabel(widget) + underline);
	      EZ_InsertGlobalKeyPressEvent(widget, Mod1Mask, (str+underline));
	    }
	  else
	    {
	      EZ_SubMenuUnderLine(widget) = -1;
	      EZ_SubMenuModifiers(widget) = 0;
	      EZ_SubMenuShortcut(widget)  = (char *)NULL;
	    }
	}
      break;
    case EZ_WIDGET_NB_PAGE:
      {
	EZ_Widget *companion = EZ_NBPageCompanion(widget);
	if(EZ_ButtonLabelLength(companion) < len)
	  {
	    EZ_ButtonLabel(companion) = 
	      (char *)my_realloc(EZ_ButtonLabel(companion),len+1,_LABEL_STRING_);
	    if(!EZ_ButtonLabel(companion)) EZ_OutOfMemory("EZ_SetWidgetLabelString");
	    EZ_ClearWidgetSizeComputedFlag(companion);
	    EZ_ClearWidgetSizeComputedFlag(EZ_WidgetParent(companion));
	  }
	EZ_ButtonLabelLength(companion) = len;
	(void)strcpy(EZ_ButtonLabel(companion),str); 
	underline =  EZ_ButtonUnderLine(companion);
	if(underline >= 0)
	  {
	    if(underline < len && isprint(str[underline]))
	      {
		EZ_ButtonModifiers(companion) = Mod1Mask;
		EZ_ButtonShortcut(companion)  = (EZ_ButtonLabel(widget) + underline);
		EZ_InsertGlobalKeyPressEvent(companion, Mod1Mask, (str+underline));
	      }
	    else
	      {
		EZ_ButtonUnderLine(companion) = -1;
		EZ_ButtonModifiers(companion) = 0;
		EZ_ButtonShortcut(companion)  = (char *)NULL;
	      }
	  }
      }
      break;
    case EZ_WIDGET_VERTICAL_SLIDER:
    case EZ_WIDGET_HORIZONTAL_SLIDER:
      if(EZ_SliderLabelLength(widget) < len)
	{
	  EZ_SliderLabel(widget) = (char *)my_realloc(EZ_SliderLabel(widget),len+1,_LABEL_STRING_);
	  if(!EZ_SliderLabel(widget)) EZ_OutOfMemory("EZ_SetWidgetLabelString");
	}
      EZ_SliderLabelLength(widget) = rlen;
      (void)strcpy(EZ_SliderLabel(widget),str); 
      break;
    case EZ_WIDGET_LCD:
      if(EZ_LCDStringLength(widget)<len)
	{
	  EZ_LCDString(widget) = (char *)my_realloc(EZ_LCDString(widget),len+1,_LABEL_STRING_);
	  if(!EZ_LCDString(widget)) EZ_OutOfMemory("EZ_SetWidgetLabelString");	  
	}
      EZ_LCDStringLength(widget) = len;
      (void)strcpy(EZ_LCDString(widget),str); 
      break;
    case EZ_WIDGET_LED:
      if(EZ_LEDStringLength(widget)<len)
	{
	  EZ_LEDString(widget) = (char *)my_realloc(EZ_LEDString(widget),len+1,_LABEL_STRING_);
	  if(!EZ_LEDString(widget)) EZ_OutOfMemory("EZ_SetWidgetLabelString");	  
	}
      EZ_LEDStringLength(widget) = rlen;
      (void)strcpy(EZ_LEDString(widget),str); 
      /* mark led mask/pixmap dirty, see EZ_WidgetLed.c */
      EZ_LEDDirty(widget) |= (LED_MASK_DIRTY | LED_PIXMAP_DIRTY);
      EZ_LEDStatus(widget) |= LED_BLOCKED;
      break;
    case EZ_WIDGET_DIAL:
      if(EZ_DialLabelLength(widget)<len)
	{
	  EZ_DialLabel(widget) = (char *)my_realloc(EZ_DialLabel(widget),len+1,_LABEL_STRING_);
	  if(!EZ_DialLabel(widget)) EZ_OutOfMemory("EZ_SetWidgetLabelString");	  
	}
      EZ_DialLabelLength(widget) = rlen;
      (void)strcpy(EZ_DialLabel(widget),str); 
      break;
    default:
      {
	extern int silenceOnConfigError;
	if(silenceOnConfigError == 0)
	  (void)fprintf(stderr,"EZ_SetWidgetLabelString(): invalid widget type %s.\n",
			EZ_WidgetDefaultClassNames[wtype]);
	return(0);
      }
      break;
    }
  return(1);
}


/***************************************************************/
void EZ_SetWidgetLabelPixmap(widget, bitmap)
     EZ_Widget *widget; EZ_Bitmap *bitmap;
{
  if(EZ_LookupWidgetFromAllHT(widget))
    {
      int shape;
      if(bitmap) bitmap->ref += 1;
      shape = EZ_SetWidgetPixmap(widget, bitmap, 0);
      if(shape & (1 << 20)) EZ_SetWindowShape(widget, 1); /* remove shape */
      if(shape & (1 << 21)) EZ_SetWindowShape(widget, 0); /* set shape */
    }
}
/***************************************************************
 *
 *   Return the integer stored in widget->common.IntData.
 */

int EZ_GetWidgetIntData(widget)
     EZ_Widget *widget;
{
  if(EZ_LookupWidgetFromAllHT(widget))
    return(EZ_WidgetIntData(widget));
  return(0);
}

void *EZ_GetWidgetPtrData(widget)
     EZ_Widget *widget;
{
  if(EZ_LookupWidgetFromAllHT(widget))
    return(EZ_WidgetPtrData(widget));
  return(0);
}
void EZ_GetWidgetClientData(widget, i, p)
     EZ_Widget *widget; int *i; void **p;
{
  if(EZ_LookupWidgetFromAllHT(widget))
    {
      *i = EZ_WidgetIntData(widget);
      *p = EZ_WidgetPtrData(widget);
    }
}
int EZ_GetWidgetReturnedData(widget)
     EZ_Widget *widget;
{
  if(EZ_LookupWidgetFromAllHT(widget))
    return(EZ_WidgetRetData(widget));
  return(0);
}

int EZ_GetWidgetReturnData(widget)
     EZ_Widget *widget;
{
  if(EZ_LookupWidgetFromAllHT(widget))
    return(EZ_WidgetRetData(widget));
  return(0);
}

/***************************************************************
 *
 *   Set  widget->common.IntData and  widget->common.PtrData.
 */
void EZ_SetWidgetIntData(widget,data)
     EZ_Widget *widget;
     int             data;
{
  if(EZ_LookupWidgetFromAllHT(widget))
    {
      int type = EZ_WidgetType(widget);
      EZ_Widget *twidget = NULL;
      switch(type)
	{
	case EZ_WIDGET_TEXT:
	  twidget = EZ_TextIText(widget);		  
	  break;
	case EZ_WIDGET_TREE:
	  twidget = EZ_TreeWITree(widget);		  
	  break;
	case EZ_WIDGET_LIST_BOX:
	  twidget = EZ_ListBoxIListBox(widget);
	  break;
	case EZ_WIDGET_FANCY_LIST_BOX:
	  twidget = EZ_FListBoxIFListBox(widget);
	  break;
	case EZ_WIDGET_WORK_AREA:
	  twidget = EZ_WorkAreaIWorkArea(widget);
	  break;
        case EZ_WIDGET_TERM:
          twidget = EZ_TermITerm(widget); 
          break;
	default:
	  break;
	}
      EZ_WidgetIntData(widget) = data;
      if(twidget) EZ_WidgetIntData(twidget) = data;
    }
}


void EZ_SetWidgetPtrData(widget,data)
     EZ_Widget *widget;
     void            *data;
{
  if(EZ_LookupWidgetFromAllHT(widget))
    {
      int type = EZ_WidgetType(widget);
      EZ_Widget *twidget = NULL;
      switch(type)
	{
	case EZ_WIDGET_TREE:
	  twidget = EZ_TreeWITree(widget);		  
	  break;
	case EZ_WIDGET_TEXT:
	  twidget = EZ_TextIText(widget);		  
	  break;
	case EZ_WIDGET_LIST_BOX:
	  twidget = EZ_ListBoxIListBox(widget);
	  break;
	case EZ_WIDGET_FANCY_LIST_BOX:
	  twidget = EZ_FListBoxIFListBox(widget);
	  break;
	case EZ_WIDGET_WORK_AREA:
	  twidget = EZ_WorkAreaIWorkArea(widget);
	  break;
        case EZ_WIDGET_TERM:
          twidget = EZ_TermITerm(widget); 
          break;
	default:
	  break;
	}
      EZ_WidgetPtrData(widget) = data;
      if(twidget) EZ_WidgetPtrData(twidget) = data;
    }
}

void EZ_SetWidgetClientData(widget, i, p)
     EZ_Widget *widget;
     int i; void *p;
{
  EZ_SetWidgetIntData(widget, i);
  EZ_SetWidgetPtrData(widget, p);
}

/***************************************************************
 *
 *  Hide a widget. This is primarily for toplevel widgets like
 *  a dialogue. For non-toplevel widgets, this it will only 
 *  hide the widget temporary (till the next redisplay of
 *  parent widget).  Use EZ_DeActivateWidget() for non-toplevel
 *  widgets.
 */
void EZ_HideWidgetWork(widget, recursion)
     EZ_Widget *widget; int recursion;
{
  if(EZ_LookupWidgetFromAllHT(widget) && EZ_WidgetWindow(widget))
    {
      if( EZ_WidgetMapped(widget) != 0)
	{
	  EZ_Widget *parent = EZ_WidgetParent(widget);
	  if(EZ_WidgetWindow(widget)) 
            {
              XUnmapWindow(EZ_Display, EZ_WidgetWindow(widget));
              XSync(EZ_Display,False);
            }
	  EZ_WidgetMapped(widget) = 0;
	  if(parent) { EZ_ClearWidgetSizeComputedFlag(parent);}
	  if(EZ_GetWidgetEmbededFlag(widget))
	    {
	      Window commWin = EZ_WidgetDnDApplName(widget);
	      Window pWin = EZ_WidgetDnDParent(widget);
	      if(EZ_VerifyExistence(commWin) && EZ_WindowExist(pWin))
		EZ_SendEmbedingMessage(EZ_EMBEDING_UNMAPPED_S,
				       EZ_DummyWindow, EZ_WidgetWindow(widget), (unsigned long)widget,
				       commWin, pWin, 0,
				       EZ_ApplicationSetupTime,
				       0,0,0,0,0,0);
	    }
	  if(widget == EZ_GrabedWidget) EZ_GrabedWidget = NULL;
	  if(recursion)
	    {
	      EZ_Widget *sto[1024];
	      int       idx = 0;
	      EZ_GetDetachedInferior(widget, 0, &idx, sto);
	      while(--idx >= 0) EZ_HideWidgetWork(sto[idx], 0);
	    }
	}
    }
}

void  EZ_HideWidget(widget)
     EZ_Widget *widget;
{
  EZ_HideWidgetWork(widget,1);
  XSync(EZ_Display, False);
}
/***************************************************************
 *
 *   De-activate a widget. It non-destructively remove widget
 *   from display. The geometry of parent widget will be 
 *   re-computed.  
 *   De-activated widget can be Activated again by calling
 *   EZ_ActivateWidget().
 */
static void EZ_DeActivateWidgetWork(widget, recursion)
     EZ_Widget *widget; int recursion;
{
  if(EZ_LookupWidgetFromAllHT(widget))
    {
      if(EZ_GetWidgetNonActiveFlag(widget) == 0)
	{
	  EZ_Widget *parent = EZ_WidgetParent(widget);
	  /*--------------------------------------------------------
	   *  If it is being displayed now, unmap it.
	   *-------------------------------------------------------*/
	  EZ_SetWidgetNonActiveFlag(widget);
	  if(parent) { EZ_ClearWidgetSizeComputedFlag(parent);}
	  if(EZ_WidgetMapped(widget) != 0)
	    {
	      if(EZ_WidgetWindow(widget))
		XUnmapWindow(EZ_DisplayForWidgets, EZ_WidgetWindow(widget));
	      EZ_WidgetMapped(widget) = 0;
	      if(EZ_GetWidgetEmbededFlag(widget))
		{
		  Window commWin = EZ_WidgetDnDApplName(widget);
		  Window pWin = EZ_WidgetDnDParent(widget);
		  if(EZ_VerifyExistence(commWin) && EZ_WindowExist(pWin))
		    EZ_SendEmbedingMessage(EZ_EMBEDING_UNMAPPED_S,
					   EZ_DummyWindow, EZ_WidgetWindow(widget),
					   (unsigned long)widget,
					   commWin, pWin, 0,
					   EZ_ApplicationSetupTime,
					   0,0,0,0,0,0);
		}
	      if(widget == EZ_GrabedWidget) EZ_GrabedWidget = NULL;
	      if(recursion)
		{
		  EZ_Widget *stor[1024];
		  int       idx = 0;
		  EZ_GetDetachedInferior(widget, 0, &idx, stor);
		  while(--idx >= 0) EZ_DeActivateWidgetWork(stor[idx], 0);
		  if(EZ_WidgetType(widget) != EZ_WIDGET_NW_LABEL)
		    if(parent && EZ_WidgetIsViewable(parent))
		      EZ_ReDisplayWidget(parent);
		}
	    }
	  /* 12-28-97: hacked for tear off menu items */
	  {
	    int type = EZ_WidgetType(widget);
	    if(type >= EZ_WIDGET_MENU_SEPARATOR && type <= EZ_WIDGET_MENU_RADIO_BUTTON)
	      {
		EZ_Widget *clone = EZ_GetMenuItemCompaion(widget);
		if(clone) EZ_DeActivateWidget(clone);
	      }
	  }
	}
    }
}
void EZ_DeActivateWidget(widget)
     EZ_Widget *widget;
{
  EZ_DeActivateWidgetWork(widget, 1);
  XSync(EZ_Display, False);	
}
/******************************************************************/
static void EZ_ActivateWidgetWork(widget, recursion)
     EZ_Widget *widget; int recursion;
{
  if(EZ_LookupWidgetFromAllHT(widget))
    {
      if(EZ_GetWidgetNonActiveFlag(widget) != 0)
	{
	  int type = EZ_WidgetType(widget);
	  EZ_Widget *parent = EZ_WidgetParent(widget);
	  EZ_ClearWidgetNonActiveFlag(widget);
	  EZ_ClearWidgetSizeComputedFlag(widget);
	  if(recursion)
	    {
	      EZ_Widget *sto[1024];
	      int       idx = 0;
	      EZ_GetDetachedInferior(widget, 0, &idx, sto);
	      while(--idx >= 0) EZ_ActivateWidgetWork(sto[idx], 0);
	      if(type != EZ_WIDGET_NW_LABEL)
		{
		  if(parent)
		    {
		      EZ_ClearWidgetSizeComputedFlag(parent);
		      if(EZ_WidgetIsViewable(parent))
			EZ_ReDisplayWidget(widget);
		    }
		  else EZ_DisplayWidget(widget);
		}
	    }
	  /* 12-28-97: hacked for tear off menu items */
	  {
	    if(type >= EZ_WIDGET_MENU_SEPARATOR && type <= EZ_WIDGET_MENU_RADIO_BUTTON)
	      {
		EZ_Widget *clone = EZ_GetMenuItemCompaion(widget);
		if(clone) EZ_ActivateWidget(clone);
	      }
	  }
	}
    }
}
void EZ_ActivateWidget(widget)
     EZ_Widget *widget; 
{
  EZ_ActivateWidgetWork(widget, 1);
  XSync(EZ_Display, False);	
}
/********************************************************************/
void EZ_DeActivateChildren(widget)
     EZ_Widget *widget;
{
  if(EZ_LookupWidgetFromAllHT(widget))
    {
      EZ_Widget *children = EZ_WidgetChildren(widget);
      if(children)
	{
	  int freeze = EZ_GetWidgetFreezedFlag(widget);
	  EZ_SetWidgetFreezedFlag(widget);
	  while(children)
	    {
	      EZ_DeActivateWidgetWork(children, 1);
	      children = EZ_WidgetSibling(children);
	    }
	  if(freeze == 0) EZ_UnFreezeWidget(widget);
	}
      XSync(EZ_Display, False);	
    }
}

void EZ_ActivateChildren(widget)
     EZ_Widget *widget;
{
  if(EZ_LookupWidgetFromAllHT(widget))
    {
      EZ_Widget *children = EZ_WidgetChildren(widget);
      if(children)
	{
	  int freeze = EZ_GetWidgetFreezedFlag(widget);
	  EZ_SetWidgetFreezedFlag(widget);
	  while(children)
	    {
	      EZ_ActivateWidgetWork(children, 1);
	      children = EZ_WidgetSibling(children);
	    }
	  if(freeze == 0) EZ_UnFreezeWidget(widget);
	}
      XSync(EZ_Display, False);	
    }
}

void EZ_DeActivateSibling(widget)
     EZ_Widget *widget;
{
  if(EZ_LookupWidgetFromAllHT(widget))
    {
      EZ_Widget *parent = EZ_WidgetParent(widget);
      if(parent)
	{
	  EZ_Widget *children = EZ_WidgetChildren(parent);
	  int freeze = EZ_GetWidgetFreezedFlag(parent);
	  EZ_SetWidgetFreezedFlag(widget);
	  while(children)
	    {
	      if(children != widget) EZ_DeActivateWidgetWork(children, 1);
	      children = EZ_WidgetSibling(children);
	    }
	  if(freeze == 0) EZ_UnFreezeWidget(parent);
	}
      XSync(EZ_Display, False);	
    }
}

void EZ_ActivateSibling(widget)
     EZ_Widget *widget;
{
  if(EZ_LookupWidgetFromAllHT(widget))
    {
      EZ_Widget *parent = EZ_WidgetParent(widget);
      if(parent)
	{
	  EZ_Widget *children = EZ_WidgetChildren(parent);
	  int freeze = EZ_GetWidgetFreezedFlag(parent);
	  EZ_SetWidgetFreezedFlag(widget);
	  while(children)
	    {
	      if(children != widget) EZ_ActivateWidgetWork(children,1);
	      children = EZ_WidgetSibling(children);
	    }
	  if(freeze == 0) EZ_UnFreezeWidget(parent);
	}
    }
}
/***************************************************************/
static void EZ_FreezeWidgetWork(widget, level)
     EZ_Widget *widget;
     int       level;
{
  if(widget != (EZ_Widget *)NULL)
    {
      EZ_SetWidgetFreezedFlag(widget);

      EZ_FreezeWidgetWork(EZ_WidgetChildren(widget), 1);
      if(level) EZ_FreezeWidgetWork(EZ_WidgetSibling(widget), 1);
    }
}

static void EZ_UnFreezeWidgetWork(widget, level)
     EZ_Widget *widget;
     int       level;
{
  if(widget != (EZ_Widget *)NULL)
    {
      EZ_ClearWidgetFreezedFlag(widget);

      EZ_UnFreezeWidgetWork(EZ_WidgetChildren(widget), 1);
      if(level) EZ_UnFreezeWidgetWork(EZ_WidgetSibling(widget), 1);
    }
}

void EZ_FreezeWidget(widget)
     EZ_Widget *widget;
{
  if(EZ_LookupWidgetFromAllHT(widget))
    { 
      if( EZ_GetWidgetFreezedFlag(widget) == 0)
	EZ_FreezeWidgetWork(widget, 0);
    }
}

void EZ_UnFreezeWidget(widget)
     EZ_Widget *widget;
{
  if(EZ_LookupWidgetFromAllHT(widget))
    {
      if( EZ_GetWidgetFreezedFlag(widget) != 0)
	{
	  EZ_UnFreezeWidgetWork(widget, 0);
	  if(EZ_WidgetMapped(widget) != 0) EZ_ReDisplayWidget(widget);
	  XSync(EZ_Display, False);
	}
    }
}
/***************************************************************/
      
int EZ_GetWidgetOrientation(widget)
     EZ_Widget *widget;
{
  if(EZ_LookupWidgetFromAllHT(widget))
    return((int)EZ_WidgetStacking(widget));
  return(0);
}
/***************************************************************/
/*
 * return the geometry of widget in screen coordinates.
 * position is relative to the upper-left corner of the display
 */
void EZ_GetWidgetAbsoluteGeometry(widget, x_ret,y_ret, w_ret, h_ret)
     EZ_Widget *widget;
     int *x_ret,*y_ret, *w_ret, *h_ret;
{
  if(EZ_LookupWidgetFromAllHT(widget) && EZ_WidgetMapped(widget) != 0)  
    {
      Window junkwin, win = EZ_WidgetWindow(widget);
      XTranslateCoordinates(EZ_Display, win,
                            RootWindow(EZ_Display, EZ_ScreenNum),
                            0,0, x_ret,y_ret, &junkwin); 
      *w_ret = EZ_WidgetWidth(widget);
      *h_ret = EZ_WidgetHeight(widget);
    }
}
/****************************************************************
 *
 *   Manipulating callbacks: For internal use only
 */
void  EZ_AddWidgetCallBackInternal(widget, which, callback, data, where)
     EZ_Widget *widget;
     EZ_CallBack callback;
     void *data;
     int where;
     int which;
{
  if(EZ_LookupWidgetFromAllHT(widget))
    { 
      EZ_CallBackStruct **funcList = EZ_GetCallbackType(widget, which);
      if(funcList) EZ_AddCallBackWork(funcList,callback,data, where, 1);
      else { fprintf(stderr,"EZ_AddWidgetCallBack(), unknown callback type\n");}
    }
}

void  EZ_AddWidgetCallBack(widget, which, callback, data, where)
     EZ_Widget *widget;
     EZ_CallBack callback;
     void *data;
     int where;
     int which;
{
  if(EZ_LookupWidgetFromAllHT(widget))
    { 
      EZ_CallBackStruct **funcList = EZ_GetCallbackType(widget, which);
      if(funcList) EZ_AddCallBackWork(funcList,callback,data, where, 0);
      else { fprintf(stderr,"EZ_AddWidgetCallBack(), unknown callback type\n");}
    }
}
/* add callback to widget, do not decend to subwidgets */
void  EZ_AddWidgetCallBack00(widget, which, callback, data, where)
     EZ_Widget *widget;
     EZ_CallBack callback;
     void *data;
     int where;
     int which;
{
  if(EZ_LookupWidgetFromAllHT(widget))
    { 
      EZ_CallBackStruct **funcList = NULL;
      switch(which)
        {
        case EZ_CALLBACK:
          funcList = &(EZ_WidgetCallback(widget)); 
          break;
        case EZ_DESTROY_CALLBACK:
          funcList = &(EZ_WidgetDestroyCallBack(widget)); 
          break;
        case EZ_MOTION_CALLBACK:
          funcList = &(EZ_WidgetMotionCallBack(widget)); 
          break;
        }
      if(funcList) EZ_AddCallBackWork(funcList,callback,data, where, 0);
      else { fprintf(stderr,"EZ_AddWidgetCallBack(), unknown callback type\n");}
    }
}

/*****************************************************************************/
void EZ_RemoveWidgetCallBack(widget, which, callback, data)
     EZ_Widget *widget;
     EZ_CallBack callback;
     void *data;
     int which;
{
  if(EZ_LookupWidgetFromAllHT(widget))
    { 
      EZ_CallBackStruct **funcList = EZ_GetCallbackType(widget, which);
      if(funcList) EZ_RemoveCallBackWork(funcList,callback,data);
      else {fprintf(stderr, "EZ_RemoveWidgetCallBack(), unknown callback type\n");}
    }
}
void EZ_RemoveWidgetCallBack00(widget, which, callback, data)
     EZ_Widget *widget;
     EZ_CallBack callback;
     void *data;
     int which;
{
  if(EZ_LookupWidgetFromAllHT(widget))
    { 
      EZ_CallBackStruct **funcList = NULL;
      switch(which)
        {
        case EZ_CALLBACK:
          funcList = &(EZ_WidgetCallback(widget)); 
          break;
        case EZ_DESTROY_CALLBACK:
          funcList = &(EZ_WidgetDestroyCallBack(widget)); 
          break;
        case EZ_MOTION_CALLBACK:
          funcList = &(EZ_WidgetMotionCallBack(widget)); 
          break;
        }
      if(funcList) EZ_RemoveCallBackWork(funcList,callback,data);
      else {fprintf(stderr, "EZ_RemoveWidgetCallBack(), unknown callback type\n");}
    }
}

/*****************************************************************************/
void EZ_RemoveAllWidgetCallBacks(widget, which)
     EZ_Widget *widget;
     int which;
{
  if(EZ_LookupWidgetFromAllHT(widget))
    { 
      EZ_CallBackStruct **funcList = EZ_GetCallbackType(widget, which);
      if(funcList) EZ_RemoveAllCallBacksWork(funcList);
      else {fprintf(stderr, "EZ_RemoveAllWidgetCallBack(), unknown callback type\n");}
    }
}
void EZ_RemoveAllWidgetCallBacks00(widget, which)
     EZ_Widget *widget;
     int which;
{
  if(EZ_LookupWidgetFromAllHT(widget))
    { 
      EZ_CallBackStruct **funcList = NULL;
      switch(which)
        {
        case EZ_CALLBACK:
          funcList = &(EZ_WidgetCallback(widget)); 
          break;
        case EZ_DESTROY_CALLBACK:
          funcList = &(EZ_WidgetDestroyCallBack(widget)); 
          break;
        case EZ_MOTION_CALLBACK:
          funcList = &(EZ_WidgetMotionCallBack(widget)); 
          break;
        }
      if(funcList) EZ_RemoveAllCallBacksWork(funcList);
      else {fprintf(stderr, "EZ_RemoveAllWidgetCallBack(), unknown callback type\n");}
    }
}

/*****************************************************************************/
EZ_CallBackStruct  **EZ_GetCallbackType(widget, which)
     EZ_Widget *widget;
     int which;
{
  EZ_CallBackStruct **funcList = NULL;

  EZ_Widget *twidget;
  
  switch(EZ_WidgetType(widget))
    {
    case EZ_WIDGET_LIST_BOX:
      twidget = EZ_ListBoxIListBox(widget);
      break;
    case EZ_WIDGET_TEXT:
      twidget = EZ_TextIText(widget); 
      break;
    case EZ_WIDGET_TREE:
      twidget = EZ_TreeWITree(widget);
      break;
    case EZ_WIDGET_FANCY_LIST_BOX:
      twidget = EZ_FListBoxIFListBox(widget);
      break;
    case EZ_WIDGET_WORK_AREA:
      twidget = EZ_WorkAreaIWorkArea(widget);
      break;
    case EZ_WIDGET_TERM:
      twidget = EZ_TermITerm(widget); 
      break;
    case EZ_WIDGET_SPIN_BUTTON:
      twidget = EZ_SpinButtonEntry(widget);
      break;
    case EZ_WIDGET_OPTIONAL_ENTRY:
      twidget = EZ_OptEntryEntry(widget);
      break;
    default:
      twidget = widget;
      break;
    }  
  
  switch(which)
    {
    case EZ_CALLBACK:
      funcList = &(EZ_WidgetCallback(twidget)); 
      break;
    case EZ_DESTROY_CALLBACK:
      funcList = &(EZ_WidgetDestroyCallBack(twidget)); 
      break;
    case EZ_MOTION_CALLBACK:
      funcList = &(EZ_WidgetMotionCallBack(twidget)); 
      break;
    default:
      break;
    }
  return(funcList);
}
/*****************************************************************************/
EZ_EventHandlerStruct **GetWidgetEventHandlers(widget)
     EZ_Widget *widget;
{
  EZ_EventHandlerStruct **handlers = NULL;

  if(widget)
    {
      EZ_Widget *twidget = widget;

      switch(EZ_WidgetType(widget))
	{
	case EZ_WIDGET_LIST_BOX:
	  twidget = EZ_ListBoxIListBox(widget);
	  break;
	case EZ_WIDGET_TEXT:
	  twidget = EZ_TextIText(widget); 
	  break;
	case EZ_WIDGET_TREE:
	  twidget = EZ_TreeWITree(widget);
	  break;
	case EZ_WIDGET_FANCY_LIST_BOX:
	  twidget = EZ_FListBoxIFListBox(widget);
	  break;
	case EZ_WIDGET_WORK_AREA:
	  twidget = EZ_WorkAreaIWorkArea(widget);
	  break;
        case EZ_WIDGET_TERM:
          twidget = EZ_TermITerm(widget); 
          break;
        case EZ_WIDGET_SPIN_BUTTON:
          twidget = EZ_SpinButtonEntry(widget);
          break;
        case EZ_WIDGET_OPTIONAL_ENTRY:
          twidget = EZ_OptEntryEntry(widget);
          break;
        case EZ_WIDGET_NOTE_BOOK:
          twidget = EZ_NoteBookBFrame(widget);
          break;
	default:
	  break;
	}
      handlers = &(EZ_WidgetEventHandler(twidget)); 
    }
  return(handlers);
}


void  EZ_AddEventHandler(widget, handler, data, where)
     EZ_Widget *widget;
     EZ_EventHandler handler;
     void *data;
     int where;
{
  EZ_CallBackStruct **funcList = NULL;
  if(widget == NULL)  funcList = (EZ_CallBackStruct **)&EZ_NoWidgetEventHandlers;
  else if(EZ_LookupWidgetFromAllHT(widget))
    funcList = (EZ_CallBackStruct **) GetWidgetEventHandlers(widget);
  if(funcList) EZ_AddCallBackWork(funcList,(EZ_CallBack)handler, data, where, 0);
}

/* add event handler to widget, do not decend to subwidgets */
void  EZ_AddEventHandler00(widget, handler, data, where)
     EZ_Widget *widget;
     EZ_EventHandler handler;
     void *data;
     int where;
{
  EZ_CallBackStruct **funcList = NULL;
  if(widget == NULL)  funcList = (EZ_CallBackStruct **)&EZ_NoWidgetEventHandlers;
  else if(EZ_LookupWidgetFromAllHT(widget))
    {
      funcList = (EZ_CallBackStruct **) &EZ_WidgetEventHandler(widget); 
      if(funcList) EZ_AddCallBackWork(funcList,(EZ_CallBack)handler, data, where, 0);
    }
}

/* add event hander to widget and all its inferiors */
static void  EZ_AddEventHandlerAllWork(widget, handler, data, where, depth)
     EZ_Widget *widget;
     EZ_EventHandler handler;
     void *data;
     int where, depth;
{
  if(widget)
    {
      EZ_Widget *child, *sibling;
      EZ_CallBackStruct **funcList =NULL;
      if(EZ_LookupWidgetFromAllHT(widget))
        funcList = (EZ_CallBackStruct **) &(EZ_WidgetEventHandler(widget)); 
      if(funcList) EZ_AddCallBackWork(funcList,(EZ_CallBack)handler, data, where, 0);
      child = EZ_WidgetChildren(widget);
      if(child) EZ_AddEventHandlerAllWork(child, handler, data, where, 1);
      if(depth)
        {
          sibling = EZ_WidgetSibling(widget);
          if(sibling) EZ_AddEventHandlerAllWork(sibling, handler, data, where, 1);          
        }
    }
}
void  EZ_AddEventHandlerAll(widget, handler, data, where)
     EZ_Widget *widget;
     EZ_EventHandler handler;
     void *data;
     int where;
{
  EZ_AddEventHandlerAllWork(widget, handler, data, where, 0);
}


void EZ_RemoveEventHandler(widget, handler, data)
     EZ_Widget *widget;
     EZ_EventHandler handler;
     void *data;
{
  EZ_CallBackStruct **funcList = NULL;
  if(widget == NULL) funcList = (EZ_CallBackStruct **)&EZ_NoWidgetEventHandlers;
  else if(EZ_LookupWidgetFromAllHT(widget))
    funcList = (EZ_CallBackStruct **) GetWidgetEventHandlers(widget);
  if(funcList) EZ_RemoveCallBackWork(funcList,(EZ_CallBack)handler, data);
} 

void EZ_RemoveEventHandler00(widget, handler, data)
     EZ_Widget *widget;
     EZ_EventHandler handler;
     void *data;
{
  EZ_CallBackStruct **funcList = NULL;
  if(widget == NULL) funcList = (EZ_CallBackStruct **)&EZ_NoWidgetEventHandlers;
  else if(EZ_LookupWidgetFromAllHT(widget))
    {
      funcList = (EZ_CallBackStruct **) &EZ_WidgetEventHandlers(widget);
      if(funcList) EZ_RemoveCallBackWork(funcList,(EZ_CallBack)handler, data);
    }
} 

void EZ_RemoveAllEventHandlers(widget)
     EZ_Widget *widget;
{
  EZ_CallBackStruct **funcList = NULL;
  if(widget == NULL) funcList = (EZ_CallBackStruct **)&EZ_NoWidgetEventHandlers;
  else if(EZ_LookupWidgetFromAllHT(widget))
    funcList = (EZ_CallBackStruct **) GetWidgetEventHandlers(widget);
  if(funcList) EZ_RemoveAllCallBacksWork(funcList);
}

void EZ_RemoveAllEventHandlers00(widget)
     EZ_Widget *widget;
{
  EZ_CallBackStruct **funcList = NULL;
  if(widget == NULL) funcList = (EZ_CallBackStruct **)&EZ_NoWidgetEventHandlers;
  else if(EZ_LookupWidgetFromAllHT(widget))
    {
      funcList = (EZ_CallBackStruct **) &EZ_WidgetEventHandlers(widget);
      if(funcList) EZ_RemoveAllCallBacksWork(funcList);
    }
}
/************************************************************************************/
int EZ_GetWidgetUnknownData(widget, idx, ret)
     EZ_Widget *widget; int idx; EZ_UnknownDataType *ret;
{
  if(idx >= 0 && EZ_LookupWidgetFromAllHT(widget))  
    {
      int count = EZ_WidgetNPValues(widget);
      if(idx < count) { *ret = EZ_WidgetPValues(widget)[idx];  return(1);}
    }
  return( 0);
}
EZ_UnknownDataType   *EZ_GetWidgetUnknownDataPtr(widget)
     EZ_Widget *widget;
{
  if( EZ_LookupWidgetFromAllHT(widget))  
    return(EZ_WidgetPValues(widget));
  return(NULL);
}

void EZ_SetWidgetUnknownData(widget, idx, data)
     EZ_Widget *widget; EZ_UnknownDataType data; int idx;
{
  if(idx >= 0 && EZ_LookupWidgetFromAllHT(widget))  
    {
      int count = EZ_WidgetNPValues(widget);
      if(idx < count)  	  EZ_WidgetPValues(widget)[idx] = data;	  
      else
	{
	  EZ_UnknownDataType *ptr = EZ_WidgetPValues(widget);
	  count = idx + 1;
	  if(ptr) 
	    {
	      EZ_WidgetPValues(widget) = (EZ_UnknownDataType *)
		my_realloc(ptr, count * sizeof(EZ_UnknownDataType), _CLIENT_DATA_);
	    }
	  else
	    {
	      EZ_WidgetPValues(widget) = (EZ_UnknownDataType *)
		my_malloc(count * sizeof(EZ_UnknownDataType), _CLIENT_DATA_);
	    }
	  EZ_WidgetNPValues(widget) = count;
	  EZ_WidgetPValues(widget)[idx] = data;	  
	}
    }
}
/************************************************************************************/
void EZ_GetWidgetPaddings(widget, px, py, pb)
     EZ_Widget *widget; int *px; int *py; int *pb;
{
  if(widget)
    {
      if(px) *px = EZ_WidgetPadX(widget);
      if(py) *py = EZ_WidgetPadY(widget);
      if(pb) *pb = EZ_WidgetPadB(widget);
    }
}
void EZ_GetWidgetBorder(widget, bw, bs)
     EZ_Widget *widget; int *bw; int *bs;
{
  if(widget)
    {
      if(bw) *bw = EZ_WidgetBorderWidth(widget);
      if(bs) *bs = EZ_WidgetBorderStyle(widget);
    }
}
/************************************************************************************/
EZ_Widget  *EZ_GetParentWidget(widget) EZ_Widget *widget;
{
  if(EZ_LookupWidgetFromAllHT(widget)) return(EZ_WidgetParent(widget));
  return(NULL);
}
EZ_Widget  *EZ_GetChildrenWidget(widget) EZ_Widget *widget;
{
  if(EZ_LookupWidgetFromAllHT(widget)) return(EZ_WidgetChildren(widget));
  return(NULL);
}
EZ_Widget  *EZ_GetSiblingWidget(widget) EZ_Widget *widget;
{
  if(EZ_LookupWidgetFromAllHT(widget)) return(EZ_WidgetSibling(widget));
  return(NULL);
}
/************************************************************************************/
unsigned long EZ_GetForegroundPV(widget) EZ_Widget *widget;
{   if(EZ_LookupWidgetFromAllHT(widget)) return(EZ_WidgetForeground(widget)); return(0);}
EZ_Bitmap  *EZ_GetWidgetPixmap(widget)  EZ_Widget *widget;
{  if(EZ_LookupWidgetFromAllHT(widget)) return(EZ_WidgetPixmap(widget)); return(NULL);}
/************************************************************************************/
void EZ_GetWidgetTileOrigin(widget, x, y) EZ_Widget *widget; int *x, *y;
{
  if(widget)
    {
      EZ_Bitmap *bitmap;
      if(EZ_WidgetBGPixmap(widget))
	{
          if(EZ_WidgetTOSet(widget))
            {
              *x = EZ_WidgetTOX(widget);
              *y = EZ_WidgetTOY(widget); 
            }
          else
            {
              int type = EZ_WidgetType(widget);
              switch(type)
                {
                case EZ_WIDGET_IWORK_AREA:
                  *x = EZ_IWorkAreaXStart(widget);
                  *y = EZ_IWorkAreaYStart(widget);
                  break;
                case EZ_WIDGET_ITREE:
                  *x = EZ_ITreeWXStart(widget);
                  *y = EZ_ITreeWYStart(widget);
                  break;
                case EZ_WIDGET_IFANCY_LIST_BOX:
                  *x = EZ_IFListBoxXStart(widget);
                  *y = EZ_IFListBoxYStart(widget);
                  break;
		case EZ_WIDGET_SPREAD_SHEET:
		  *x = EZ_SSheetTx(widget);
		  *y = EZ_SSheetTy(widget);
		  break;
                default:
                  *x = *y = 0;
                  break;
                }
            }
	  return;
	}
      else if((bitmap = EZ_WidgetParentBGPixmap(widget)) != NULL)
	{
          EZ_Widget *parent = EZ_WidgetParent(widget);
          int xcorr = 0, ycorr = 0;
          if(parent)
            {
              int type = EZ_WidgetType(parent);
              if(EZ_WidgetBGPixmap(parent) == bitmap && EZ_WidgetTOSet(parent) == 0)
                {
                  switch(type)
                    {
                    case EZ_WIDGET_IWORK_AREA:
                      xcorr = EZ_IWorkAreaXStart(parent);
                      ycorr = EZ_IWorkAreaYStart(parent);
                      break;
                    case EZ_WIDGET_ITREE:
                      xcorr = EZ_ITreeWXStart(parent);
                      ycorr = EZ_ITreeWYStart(parent);
                      break;
                    case EZ_WIDGET_IFANCY_LIST_BOX:
                      xcorr = EZ_IFListBoxXStart(parent);
                      ycorr = EZ_IFListBoxYStart(parent);
                      break;
		    case EZ_WIDGET_SPREAD_SHEET:
		      xcorr = EZ_SSheetTx(parent);
		      ycorr = EZ_SSheetTy(parent);
		      break;
                    default:
                      break;
                    }
                }
              {
                int xx = EZ_WidgetOriginX(widget) -xcorr;
                int yy = EZ_WidgetOriginY(widget) -ycorr;

                if(EZ_GetWidgetDetachedFlag(widget))
                  {
                    Window junkwin, win = EZ_WidgetWindow(parent);
                    int rx, ry;
                    XTranslateCoordinates(EZ_Display, win, RootWindow(EZ_Display, EZ_ScreenNum),
                                          0,0, &rx, &ry, &junkwin); 	  
                    xx -= rx; yy -= ry;
                  }
                while(parent)
                  {
                    if(EZ_WidgetBGPixmap(parent) == bitmap)
                      {
                        if(EZ_WidgetTOSet(parent))
                          {
                            xx -= EZ_WidgetTOX(parent);
                            yy -= EZ_WidgetTOY(parent);              
                          }
                        break;
                      }
                    if(EZ_GetWidgetDetachedFlag(parent))
                      {
                        EZ_Widget *gparent = EZ_WidgetParent(parent);
                        if(gparent)
                          {
                            Window junkwin, win = EZ_WidgetWindow(gparent);
                            int rx, ry;
                            XTranslateCoordinates(EZ_Display, win, RootWindow(EZ_Display, EZ_ScreenNum),
                                                  0,0, &rx, &ry, &junkwin); 	  
                            xx -= rx; yy -= ry;
                          }
                      }
                    xx += EZ_WidgetOriginX(parent);
                    yy += EZ_WidgetOriginY(parent);
                    parent = EZ_WidgetParent(parent);
                  }
                if(parent == NULL) /* children of a popup menu or popup listbox */
                  {
                    EZ_Widget *pant = EZ_WidgetParent(widget);
                    int tx, ty;
                    while(EZ_WidgetParent(pant)) pant = EZ_WidgetParent(pant);
                    EZ_GetWidgetTileOrigin(pant, &tx, &ty);                    
                    xx -= tx + EZ_WidgetOriginX(pant);
                    yy -= ty + EZ_WidgetOriginY(pant);
                  }
                
                if(EZ_WidgetType(widget) == EZ_WIDGET_NW_LABEL)
                  {
                    int cx = 0, cy = 0;
                    while(widget != parent && EZ_WidgetType(widget) == EZ_WIDGET_NW_LABEL)
                      {
                        cx += EZ_WidgetOriginX(widget);
                        cy += EZ_WidgetOriginY(widget);
                        widget = EZ_WidgetParent(widget);
                      }
                    xx -= cx;  yy -= cy;
                  }
                *x = -xx; *y = -yy;
              }
            }
          else /* must be a popup menu or popup listbox */
            {
              int type = EZ_WidgetType(widget);
              if(type == EZ_WIDGET_ITOPLEVEL)
                {
                  EZ_Widget *itor = EZ_IToplevelInitiator(widget);
                  if(itor)
                    {
                      int tx, ty;
                      EZ_GetWidgetTileOrigin(itor, &tx, &ty);
                      *x = tx;
                      *y = ty - EZ_WidgetHeight(itor);
                      return;
                    }
                }
              else if(type == EZ_WIDGET_POPUP_MENU)
                {
                  EZ_Widget *itor = EZ_PopupInitiator(widget);
                  if(itor)
                    {
                      int tx, ty;
                      EZ_GetWidgetTileOrigin(itor, &tx, &ty);
                      *x = tx - EZ_PopupXoff(widget);
                      *y = ty - EZ_PopupYoff(widget);
                      return;
                    }
                }
              *x = -EZ_WidgetOriginX(widget);
              *y = -EZ_WidgetOriginY(widget);
            }
          return;
        }
    }
  *x = *y = 0;
}
/************************************************************************************/
void  EZ_GetDetachedInferior(widget, sib, idx, sto)
     EZ_Widget *widget; int sib, *idx;
     EZ_Widget **sto;
{
  if(widget)
    {
      EZ_Widget *sibling, *child;
      child = EZ_WidgetChildren(widget);
      if(child != NULL)
	{
	  if(EZ_GetWidgetDetachedFlag(child))
	    { sto[*idx] = child; *idx += 1;}
	  EZ_GetDetachedInferior(child, 1, idx, sto);
	}
      if(sib)
	{
	  sibling = EZ_WidgetSibling(widget);
	  if( sibling != NULL)
	    {
	      if(EZ_GetWidgetDetachedFlag(sibling)) 
		{sto[*idx] = sibling; *idx += 1;}
	      EZ_GetDetachedInferior(sibling, 1, idx, sto);
	    }
	}
    }
}
/************************************************************************************/
void EZ_HighlightWidget(widget, flag)  EZ_Widget *widget; int flag;
{
  if(widget)
    {
      int draw = 0;
      if(flag)
        {
          if( EZ_GetWidgetHighlightFlag(widget) == 0)
            {
              EZ_SetWidgetHighlightFlag(widget);
              { EZ_InstallHighlightColor(widget); } /*4/29/99*/
              draw = 1;
            }
        }
      else 
        {
          if( EZ_GetWidgetHighlightFlag(widget) != 0)
            {
              EZ_ClearWidgetHighlightFlag(widget);
              { EZ_UninstallHighlightColor(widget); } /*4/29/99*/
              draw = 1;
            }
        }
      if(draw && EZ_WidgetMapped(widget)) EZ_DrawWidget(widget);
    }
}
/************************************************************************************/
#undef _EZ_WIDGET_C_
