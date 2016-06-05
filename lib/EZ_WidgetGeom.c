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
/******************************************************************
 ***                                                            ***
 ***     Compute the size and position of a widget window.      ***
 ***                                                            ***
 ******************************************************************/
#define _EZ_WIDGET_GEOM_C_
#include "EZ_Widget.h"

/*********************************************************************
 * 
 *  Functions implemented in this file:
 */
void EZ_ComputeWidgetWindowSize MY_ANSIARGS((EZ_Widget *widget,
                                             int *w, int *h,
                                             EZ_Widget *dirtyAns));
void EZ_SetChildrenPositionAndSize MY_ANSIARGS((EZ_Widget *widget, int w, int h));

int EZ_CountWidthScalableChildren MY_ANSIARGS((EZ_Widget *widget));
int EZ_CountHeightScalableChildren MY_ANSIARGS((EZ_Widget *widget));
int EZ_GetNumberOfActiveChildren MY_ANSIARGS((EZ_Widget *widget));
void EZ_FixCheckMarkOffsets MY_ANSIARGS((EZ_Widget *widget, int h1, int h2));
void EZ_NullComputeGeometry  MY_ANSIARGS((EZ_Widget *widget, int *w, int *h, EZ_Widget *td));
void EZ_NullSetChildrenPositionAndSize MY_ANSIARGS((EZ_Widget *widget, int w, int h));
void EZ_ComputeGeometryDefault  MY_ANSIARGS((EZ_Widget *widget, int *w, int *h, EZ_Widget *td));
void EZ_SetChildrenPositionAndSizeDefault MY_ANSIARGS((EZ_Widget *widget, int w, int h));
/*********************************************************************
 *
 * Compute the minimal dimension of widget. It is the smallest size
 * to let its children fit in nicely.
 */
void EZ_ComputeWidgetWindowSize(widget, w, h, EZ_TopmostGeomDirtyWidget)
     EZ_Widget *widget,*EZ_TopmostGeomDirtyWidget;
     int             *w, *h;
{
  int               cw, ch, tw, th, ww,wh, type;
  EZ_GManager       *gmanager; 
  EZ_Bitmap         *shape;
  int               gtype = EZ_DEFAULT_GEOM_MANAGER;
  /*------------------------------------------------------------
   *  A NULL widget must be of size 0
   *-----------------------------------------------------------*/
  if(widget == (EZ_Widget *)NULL || EZ_GetWidgetNonActiveFlag(widget) != 0)
    {
      *w = 0; *h = 0;
      return;
    }
  /*-----------------------------------------------------------
   *  If widget is a terminal widget. ww and wh holds its
   *  minimal size.
   *---------------------------------------------------------*/
  gmanager =  EZ_WidgetGManager(widget);
  shape = EZ_WidgetShapeMask(widget);
  type = EZ_WidgetType(widget); 
  (EZ_WidgetHandlingFunctions[type]->ComputeSize)(widget, &ww, &wh);
      
  if(gmanager) gtype = EZ_GMType(gmanager);
  switch(gtype)
    {
    case EZ_GB_GEOM_MANAGER:
      EZ_RowColumnComputeGeometry(widget, &cw, &ch, EZ_TopmostGeomDirtyWidget);
      break;
    case EZ_NULL_GEOM_MANAGER:
      EZ_NullComputeGeometry(widget, &cw, &ch,  EZ_TopmostGeomDirtyWidget);
      break;
    case EZ_ROW_GEOM_MANAGER:
      EZ_RowComputeGeometry(widget, &cw, &ch,  EZ_TopmostGeomDirtyWidget);
      break;          
    case EZ_COLUMN_GEOM_MANAGER:
      EZ_ColComputeGeometry(widget, &cw, &ch,  EZ_TopmostGeomDirtyWidget);
      break;          
    default:
      EZ_ComputeGeometryDefault(widget, &cw, &ch,  EZ_TopmostGeomDirtyWidget);
      break;
    }

  /*-----------------------------------------------------
   * we need to remember the difference between
   * the internal min size and the min size needed to
   * fit all children in.
   *----------------------------------------------------*/
  {
    int tmp;
    
    tmp = ww - cw;
    EZ_WidgetCW(widget) = tmp > 0? tmp: 0;
    tmp = wh - ch;
    EZ_WidgetCH(widget) = tmp > 0? tmp: 0;
  }
  /*------------------------------------------------------
   *  This is the smallest possible size of widget
   *-----------------------------------------------------*/
  tw = MAXV(cw,ww);  tw = (tw > 0? tw : 1);
  th = MAXV(ch,wh);  th = (th > 0? th : 1);

  /*----------------------------------------------------------
   * set the minimal size of widget.
   *---------------------------------------------------------*/
  EZ_WidgetMinWidth(widget) = tw;
  EZ_WidgetMinHeight(widget) = th;
  EZ_WidgetAMinWidth(widget) = tw;
  EZ_WidgetAMinHeight(widget) = th;
  
  /* for notebook widget, record the max mini dim of pages */
  if(EZ_WidgetType(widget) == EZ_WIDGET_NB_PAGE)
    {
      EZ_Widget *notebook = EZ_NBPageNoteBook(widget);
      int tmpwidth = EZ_NoteBookMaxMinWidth(notebook);
      int tmpheight = EZ_NoteBookMaxMinHeight(notebook);

      tmpwidth =  EZ_NoteBookMaxMinWidth(notebook) = MAXV(tmpwidth, tw);
      tmpheight = EZ_NoteBookMaxMinHeight(notebook) = MAXV(tmpheight, th);
      EZ_WidgetWidthHint(widget) = tmpwidth;
      EZ_WidgetHeightHint(widget) = tmpheight;
    }

  /*-----------------------------------------------------------
   * Ttricky toplevel windows ...
   *----------------------------------------------------------*/
  {
    int wset = 0, hset = 0;

    if((EZ_WidgetParent(widget) == (EZ_Widget *)NULL && 
        EZ_WidgetOverrideRedirect(widget) == 0 &&
        EZ_GetWidgetToplevelFlag(widget) == 0 &&
        /* EZ_GetWidgetEmbededFlag(widget) == 0 && */
	type < EZ_WIDGET_POPUP_MENU ) ||
       EZ_GetWidgetDetachedFlag(widget))
      {
	if(EZ_WidgetWidth(widget) > EZ_WidgetMinWidth(widget)) wset = 1;
	if(EZ_WidgetHeight(widget)> EZ_WidgetMinHeight(widget)) hset = 1;
      }
    /*-----------------------------------------------------------
     * Honor configured size only if explicitly set
     *----------------------------------------------------------*/
    if(wset == 0)
      {
	if((EZ_GetWidgetWidthSetFlag(widget) && EZ_WidgetWidthHint(widget) > 0) ||
	   (EZ_GetWidgetWidthHintFlag(widget) && EZ_WidgetWidthHint(widget) >=tw))
	  EZ_WidgetWidth(widget) = EZ_WidgetWidthHint(widget);
	else EZ_WidgetWidth(widget) = tw;
      }
    if(hset == 0)
      {
	if((EZ_GetWidgetHeightSetFlag(widget) && EZ_WidgetHeightHint(widget) > 0) ||
	   (EZ_GetWidgetHeightHintFlag(widget) && EZ_WidgetHeightHint(widget) >= th))
	  EZ_WidgetHeight(widget) = EZ_WidgetHeightHint(widget);
	else EZ_WidgetHeight(widget) = th;
      }
  }
  /*----------------------------------------------------------
   * 3D canvas is special
   *---------------------------------------------------------*/
  if(EZ_WidgetType(widget) == EZ_WIDGET_3D_CANVAS)

    {
      int oldwidth = EZ_WidgetWidth(widget);
      int oldheight = EZ_WidgetHeight(widget);
      int xsize = oldwidth;
      int ysize = oldheight;

      xsize  = (xsize > 1024? 1024: ((xsize < 8)? 8: xsize));
      ysize = (ysize > 1024? 1024:((ysize < 8)? 8: ysize));
      /*
       * make sure the size is a multiple of 4
       */
      xsize = xsize & ~3;
      ysize = ysize & ~3;
      /*-----------------------------------
       * The set size of 3D Canvas.
       *----------------------------------*/
      EZ_WidgetWidth(widget) = xsize;
      EZ_WidgetHeight(widget) = ysize;
      if(EZ_WidgetWindow(widget) != None)
	{
	  /*---------------------------------------------
	   * The widget window has already been created.
	   * If the size has been changed, we'll have to
	   * reconfigure the GL stuff.
	   *--------------------------------------------*/
          if(EZ_Context != NULL)
            {
              EZ_Widget *currentCanvas = EZ_CurrentGWindow;
              if(EZ_WindowWidth != xsize || EZ_WindowHeight != ysize)
                EZ_Configure3DCanvas(widget);
              if(currentCanvas != widget) EZ_Set3DCanvas(currentCanvas);
            }
	}
    }
  /*---------------------------------------------------
   *  If the DoNotPropagateFlag is set on widget and
   *  if widget is the starting widget whose geom is being
   *  computed and its size has been previously assigned
   *  by its parent. Using the assigned geom instead.
   *  
   *  Its the FILL_MODE and EXPAND make life hard here.
   *--------------------------------------------------*/
  if(widget == EZ_TopmostGeomDirtyWidget   &&
     EZ_GetDoNotPropagateFlag(widget) != 0 &&
     EZ_WidgetAWidth(widget) > 0)
    {
      EZ_WidgetWidth(widget) = (int) EZ_WidgetAWidth(widget);
      EZ_WidgetHeight(widget) = (int) EZ_WidgetAHeight(widget);
    }
  /*---------------------------------------------------
   *  Mark that the geometry of widget is current.
   *--------------------------------------------------*/
  EZ_SetWidgetSizeComputedFlag(widget);

  /* if a shaped mask is set, honor the shape mask. */
  if(shape)
    {
      *w = EZ_WidgetWidth(widget) =  shape->width;
      *h = EZ_WidgetHeight(widget)=  shape->height;      
    }
  else
    {
      *w = EZ_WidgetWidth(widget);
      *h = EZ_WidgetHeight(widget);
    }
  
  /*--------------------------------------------------------
   *  Compute the position of children widgets.
   *-------------------------------------------------------*/
  if(EZ_GetNumberOfActiveChildren(widget) != 0 )
    EZ_SetChildrenPositionAndSize(widget,  *w, *h);
}

/*******************************************************************
 *
 *  Set the internal position and size of a widget window. If
 *  necessary, adjust the position of its direct children.
 *
 *  When this function is called, the dimension of widget and
 *  all its descendants must have been computed.
 */
void  EZ_SetChildrenPositionAndSize(widget, w, h)
     EZ_Widget   *widget;
     int         w,h;         /* new dimension of widget */
{
  EZ_GManager     *gmanager; 
  int             type, gtype = EZ_DEFAULT_GEOM_MANAGER;

  if(widget == (EZ_Widget *)NULL || EZ_GetWidgetNonActiveFlag(widget) != 0)
    return;
  type = EZ_WidgetType(widget);
  /*-----------------------------------------------------------
   *  Because of the design, 3D_Canvas has to be treated 
   *  differently. 
   *----------------------------------------------------------*/
  if(type == EZ_WIDGET_3D_CANVAS)
    {
      int xsize = w;
      int ysize = h;

      if(!(EZ_GetWidgetWidthSetFlag(widget)))
	{ 
	  xsize  = (xsize > 1024? 1024: ((xsize < 8)? 8: xsize)); 
	  xsize = xsize & ~3;
	  EZ_WidgetWidth(widget) = xsize;
	}
      if(!(EZ_GetWidgetHeightSetFlag(widget)))
	{
	  ysize = (ysize > 1024? 1024:((ysize < 8)? 8: ysize));
	  ysize = ysize & ~3;
	  EZ_WidgetHeight(widget)  = ysize;
	}
      if(EZ_WidgetWindow(widget) != (Window )NULL)
	{
	  /*--------------------------------------
	   * The canvas has already been created.
	   * The possible change of size comes from
	   * a widget reconfiguration or a resizing
	   * of the top level widget. If the size
	   * has really been changed, we have to
	   * reconfigure the canvas.
	   *-------------------------------------*/
          if(EZ_Context)
            {
              EZ_Widget *currentCanvas = EZ_CurrentGWindow;  /* current canvas */
              if(EZ_WindowWidth != xsize || EZ_WindowHeight != ysize)
                EZ_Configure3DCanvas(widget);
              if(currentCanvas != widget) EZ_Set3DCanvas(currentCanvas);	  
            }
	}
      if(EZ_WidgetAWidth(widget) > 0) 
	{
	  EZ_WidgetAWidth(widget) = EZ_WidgetWidth(widget);
	  EZ_WidgetAHeight(widget) = EZ_WidgetHeight(widget);
	}
    }
  else /* other kinds of widgets */
    {
      /*-----------------------------------------------------------
       * If the W or H of widget has been specified explicitly,
       * honor that.
       *----------------------------------------------------------*/
      if(!(EZ_GetWidgetWidthSetFlag(widget))) EZ_WidgetWidth(widget)  = w;
      if(!(EZ_GetWidgetHeightSetFlag(widget))) EZ_WidgetHeight(widget)  = h;
    }

  gmanager = EZ_WidgetGManager(widget);
  if(gmanager) gtype = EZ_GMType(gmanager);
  switch(gtype)
    {
    case EZ_GB_GEOM_MANAGER:
      EZ_RowColSetChildrenPositionAndSize(widget,w,h);
      break;
    case EZ_NULL_GEOM_MANAGER:
      EZ_NullSetChildrenPositionAndSize(widget,w,h);
      break;
    case EZ_ROW_GEOM_MANAGER:
      EZ_RowSetChildrenPositionAndSize(widget,w,h);
      break;          
    case EZ_COLUMN_GEOM_MANAGER:
      EZ_ColSetChildrenPositionAndSize(widget,w,h);
      break;          
    default:
      EZ_SetChildrenPositionAndSizeDefault(widget, w, h);
      break;
    }
}
/****************************************************************************
 *
 *  Count the number of children of widget whose width/height may be streched.
 */
int EZ_CountWidthScalableChildren(widget)
     EZ_Widget *widget;
{
  EZ_Widget *child;
  int              count = 0;
  
  if(widget == (EZ_Widget *)NULL) return(0);
  
  child = EZ_WidgetChildren(widget);
  
  while(child != (EZ_Widget *)NULL)
    {
      if(EZ_GetWidgetNoGeomFlag(child) == 0 && 
	 EZ_WidgetType(child) < EZ_WIDGET_ITOPLEVEL &&
	 EZ_GetWidgetWidthSetFlag(child) == 0 ) count++;
      child = EZ_WidgetSibling(child);
    }
  return(count);
}

int EZ_CountHeightScalableChildren(widget)
     EZ_Widget *widget;
{
  EZ_Widget *child;
  int              count = 0;
  
  if(widget == (EZ_Widget *)NULL) return(0);
  
  child = EZ_WidgetChildren(widget);
  
  while(child != (EZ_Widget *)NULL)
    {
      if( EZ_GetWidgetNoGeomFlag(child) == 0 &&
	 EZ_WidgetType(child) < EZ_WIDGET_ITOPLEVEL &&
	 EZ_GetWidgetHeightSetFlag(child) == 0 ) count++;
      child = EZ_WidgetSibling(child);
    }
  return(count);
}
/*************************************************************************************
 *
 *  Return the number of active children of widget
 */
int EZ_GetNumberOfActiveChildren(widget)
     EZ_Widget *widget;     
{
  EZ_Widget *child;
  int             count = 0;
  
  if(widget == (EZ_Widget *)NULL) return(0);
  
  child = EZ_WidgetChildren(widget);

  while(child != (EZ_Widget *)NULL)
    {
      if(EZ_GetWidgetNoGeomFlag(child) == 0 &&
	 EZ_WidgetType(child) < EZ_WIDGET_ITOPLEVEL)
	count++;
      child = EZ_WidgetSibling(child);
    }  
  return(count);
}
/*************************************************************************************/
void   EZ_FixCheckMarkOffsets(widget,ch, ch2)
     EZ_Widget *widget; int ch, ch2;
{
  /*==========================================================
   *
   * The size of the check mark depends on the size of the
   * check_button/radio_button. Too bad!
   */
  switch(EZ_WidgetType(widget))
    {
    case  EZ_WIDGET_CHECK_BUTTON:
    case  EZ_WIDGET_MENU_CHECK_BUTTON:
      if(EZ_CButtonCheckType(widget) != EZ_EMPTY_INDICATOR)	  
        EZ_WidgetAuxXLeft(widget) = (ch << 1)/5  + 6 + EZ_CButtonCheckSizeAdjust(widget);    
      else  EZ_WidgetAuxXLeft(widget) = 0;
      break;
    case  EZ_WIDGET_RADIO_BUTTON:
    case  EZ_WIDGET_MENU_RADIO_BUTTON:
      if(EZ_RButtonCheckType(widget) != EZ_EMPTY_INDICATOR)	  
        EZ_WidgetAuxXLeft(widget) =  (ch << 1)/5  + 6 + EZ_RButtonCheckSizeAdjust(widget);    
      else  EZ_WidgetAuxXLeft(widget) = 0;
      break;
    case  EZ_WIDGET_MENU_BUTTON:
      if(EZ_MButtonCheckType(widget) != EZ_EMPTY_INDICATOR)	  
        EZ_WidgetAuxXLeft(widget) = (ch << 1)/5  + 6 + EZ_MButtonCheckSizeAdjust(widget);    
      else  EZ_WidgetAuxXLeft(widget) = 0;
      break;
    case EZ_WIDGET_MENU_SUBMENU:
      EZ_SubMenuArrowSize(widget) = (ch2 >>1);
      EZ_WidgetAuxXRight(widget) = EZ_SubMenuLabel2Width(widget) +
        EZ_SubMenuArrowSize(widget) + (EZ_SubMenuArrowBWidth(widget)<<1) + 8;
      break;
    default:
      break;
    }
}
/*************************************************************************************/
void EZ_NullComputeGeometry(widget, w, h, topdirtywgt)
     EZ_Widget *widget; int *w; int *h; EZ_Widget *topdirtywgt;
{
  EZ_Widget *children;
  int cw = 0, ch = 0;
  int tw, th, itmp;
  if(widget == (EZ_Widget *)NULL || EZ_GetWidgetNonActiveFlag(widget) != 0)
    {
      *w = 0; *h = 0;
      return;
    }
  /*-----------------------------------------------------------
   *  If widget is a terminal widget. ww and wh holds its
   *  minimal size.
   *---------------------------------------------------------*/
  children = EZ_WidgetChildren(widget);
  ch = 0; cw = 0;
  while(children)
    {
      if(EZ_GetWidgetNonActiveFlag(children) == 0)
        {
          EZ_ComputeWidgetWindowSize(children, &tw, &th, topdirtywgt);      
          if(EZ_GetWidgetXSetFlag(children))
            {
              itmp = EZ_WidgetOriginX(children) + tw;
              cw = MAXV(cw, itmp);
            }
          else if(tw > cw) cw = tw;

          if(EZ_GetWidgetYSetFlag(children))
            {
              itmp = EZ_WidgetOriginY(children) + th;
              ch = MAXV(ch, itmp);          
            }
          else  if(th > ch) ch = th;
        }
      children = EZ_WidgetSibling(children);
    }
  *w =cw; *h = ch;
}
/*************************************************************************************/
void EZ_NullSetChildrenPositionAndSize(widget, w, h)
     EZ_Widget *widget; int w; int h;
{
}
/*************************************************************************************/
void EZ_ComputeGeometryDefault(widget, w, h, EZ_TopmostGeomDirtyWidget)
     EZ_Widget *widget,*EZ_TopmostGeomDirtyWidget;
     int             *w, *h;
{
  int               cw, ch, tw, th;
  int               stacking, numchildren=0, itmp, jtmp, fillmode;
  EZ_Widget         *children;
  /*------------------------------------------------------------
   *  A NULL widget must be of size 0
   *-----------------------------------------------------------*/
  if(widget == (EZ_Widget *)NULL || EZ_GetWidgetNonActiveFlag(widget) != 0)
    {
      *w = 0; *h = 0;
      return;
    }
  /*-----------------------------------------------------------
   *  If widget is a terminal widget. ww and wh holds its
   *  minimal size.
   *---------------------------------------------------------*/
      
  cw = 0;
  ch = 0;
  stacking = EZ_WidgetStacking(widget);
  fillmode = (EZ_WidgetFillMode(widget) == EZ_FILL_NONE ? 1 : 0);

  if((children = EZ_WidgetChildren(widget)) != (EZ_Widget *)NULL)
    {
      /*------------------------------------------------------
       *  figure out the dimensions of all descendants
       *-----------------------------------------------------*/
      while(children)
        {
          if(EZ_GetWidgetNonActiveFlag(children) == 0)
            {
              EZ_ComputeWidgetWindowSize(children, &tw, &th, EZ_TopmostGeomDirtyWidget);
              if(EZ_WidgetType(children) < EZ_WIDGET_ITOPLEVEL && 
                 EZ_GetWidgetDetachedFlag(children) == 0)
                {
                  if(stacking >= EZ_HORIZONTAL && stacking <= EZ_HORIZONTAL_CENTER)
                    {
                      if(fillmode && EZ_GetWidgetXSetFlag(children) && 
                         EZ_GetWidgetExpandFlag(children)== 0)
                        {
                          itmp = EZ_WidgetOriginX(children) + tw;
                          cw = MAXV(cw, itmp);
                        }
                      else
                        {
                          EZ_ClearWidgetXSetFlag(children);
                          cw += tw;
                        }
                      if(fillmode && EZ_GetWidgetYSetFlag(children) && 
                         EZ_GetWidgetExpandFlag(children) == 0)
                        {
                          itmp = EZ_WidgetOriginY(children) + th;
                          ch = MAXV(ch, itmp);
                        }		  
                      else 
                        {
                          EZ_ClearWidgetYSetFlag(children);
                          ch = MAXV(ch, th);
                        }
                    }
                  else /* stacking >= EZ_VERTICAL && stacking <= EZ_VERTICAL_CENTER */
                    {
                      if(fillmode && EZ_GetWidgetXSetFlag(children) &&
                         EZ_GetWidgetExpandFlag(children) == 0)
                        {
                          itmp = EZ_WidgetOriginX(children) + tw;
                          cw = MAXV(cw, itmp);
                        }
                      else
                        {
                          EZ_ClearWidgetXSetFlag(children);
                          cw = MAXV(cw,tw);
                        }
                      if(fillmode && EZ_GetWidgetYSetFlag(children) && 
                         EZ_GetWidgetExpandFlag(children) == 0)
                        {
                          itmp = EZ_WidgetOriginY(children) + th;
                          ch = MAXV(ch, itmp);
                        }		  
                      else 
                        {
                          EZ_ClearWidgetYSetFlag(children);
                          ch += th ;
                        }
                    }
                }
            }
          children = EZ_WidgetSibling(children);
        }
      /*----------------------------------------------------
       *  Now include the borderwidth and padding,
       *---------------------------------------------------*/
      itmp = EZ_WidgetBorderWidth(widget) + EZ_WidgetPadB(widget);
      jtmp = ch;
      ch += ((itmp + EZ_WidgetPadY(widget) + EZ_WidgetAuxY(widget)) << 1) +
        EZ_WidgetHAdjust(widget);

      EZ_FixCheckMarkOffsets(widget,ch, jtmp);
      /*==========================================================*/
      cw += ((itmp + EZ_WidgetPadX(widget))<<1) + EZ_WidgetAuxXLeft(widget) + EZ_WidgetAuxXRight(widget);
      numchildren = EZ_GetNumberOfActiveChildren(widget);
      /*----------------------------------------------------------------
       * Finally, include the internal spacing between children widgets
       *----------------------------------------------------------------*/
      if(numchildren > 0)
        {
          if(stacking >= EZ_HORIZONTAL && stacking <= EZ_HORIZONTAL_CENTER)
            cw += (EZ_WidgetSepX(widget) * (numchildren - 1));
          else
            ch += (EZ_WidgetSepY(widget) * (numchildren - 1));
        }
    }
  *w = cw;
  *h = ch;
}
/*************************************************************************************/
void  EZ_SetChildrenPositionAndSizeDefault(widget, w, h)
     EZ_Widget   *widget;
     int         w,h;         /* new dimension of widget */
{
  int             cw, ch, tw, th, wtmp, htmp, type;
  int             bwpadx, bwpady, sepx,sepy,auxxL,auxxR, auxxT, auxy, auxy2;
  int             stacking, alignment, fillmode, hadjust;
  int             numchildren, ncWS, ncHS;
  float           dx, dy, xx, yy, x0, y0;
  EZ_Widget       *children;

  if(widget == (EZ_Widget *)NULL || EZ_GetWidgetNonActiveFlag(widget) != 0)
    return;

  type = EZ_WidgetType(widget);
  /*-----------------------------------------------------------
   *  Because of the design, 3D_Canvas has to be treated 
   *  differently. 
   *----------------------------------------------------------*/
  cw = EZ_WidgetMinWidth(widget);
  ch = EZ_WidgetMinHeight(widget);  

  /*----------------------------------------------------------
   * widget has no children, don't need to do anything
   *---------------------------------------------------------*/
  numchildren = EZ_GetNumberOfActiveChildren(widget);
  if(numchildren == 0)  return; 

  bwpadx = EZ_WidgetBorderWidth(widget) + EZ_WidgetPadX(widget) + EZ_WidgetPadB(widget);
  bwpady = EZ_WidgetBorderWidth(widget) + EZ_WidgetPadY(widget) + EZ_WidgetPadB(widget);
  auxxL = EZ_WidgetAuxXLeft(widget);
  auxxR = EZ_WidgetAuxXRight(widget);
  auxxT = auxxL + auxxR;
  auxy = EZ_WidgetAuxY(widget);
  auxy2 = auxy << 1;
  hadjust = EZ_WidgetHAdjust(widget);

  stacking = EZ_WidgetStacking(widget);
  alignment = EZ_WidgetAlignment(widget);
  fillmode = EZ_WidgetFillMode(widget);

  /*------------------------------------------------
   * Reset the position of all descendants
   *-----------------------------------------------*/ 
  tw = EZ_WidgetWidth(widget);      /* the new width  */
  th = EZ_WidgetHeight(widget);     /* the new height */


  /* like "NW gravity":  This has effects only when the set dimension is smaller than
   * the minimal dimension of a parent widget.
   * 
   * With the following 2 lines, the position of children are computed normally, using
   * parent's minimal dimension. (toplevel widgets)
   *
   * Without them, children are squeezed together, may overlaping ... (internal widgets )
   *
   */
  if(tw < cw )
    {
      if(EZ_WidgetParent(widget) == (EZ_Widget *)NULL && EZ_GetWidgetEmbededFlag(widget) == 0)
	tw = cw;
    }
  if(th < ch )
    {
      if(EZ_WidgetParent(widget) == (EZ_Widget *)NULL && EZ_GetWidgetEmbededFlag(widget) == 0)
	th = ch; 
    }

  /*---------------------------------------------
   *  First check to see if any children widget 
   *  need to be stretched.
   *------------------------------------------*/
  if(stacking >= EZ_VERTICAL && stacking <= EZ_VERTICAL_CENTER)
    {
      /* children are stacked vertically, stretch in the horizontal direction */
      children = EZ_WidgetChildren(widget);
      while(children)
	{
	  EZ_WidgetAWidth(children) = 0;
	  if(EZ_GetWidgetExpandFlag(children) != 0 &&
	     EZ_WidgetType(children) < EZ_WIDGET_ITOPLEVEL &&
	     EZ_GetWidgetWidthSetFlag(children) == 0 && 
	     EZ_GetWidgetNoGeomFlag(children) == 0)
	    {
	      int ttt;
	      if(EZ_GetWidgetXSetFlag(children)) ttt = tw - EZ_WidgetOriginX(children) - auxxR;
	      else ttt = tw - auxxT;
	      EZ_WidgetAWidth(children) = ttt;
	      EZ_WidgetAHeight(children) = EZ_WidgetHeight(children);
	      EZ_SetChildrenPositionAndSize(children, ttt, EZ_WidgetHeight(children));
	    }
	  children = EZ_WidgetSibling(children);
	}
    }
  else
    {
      /* children are stacked horizontally, stretch in the vertical direction */
      children = EZ_WidgetChildren(widget);
      while(children)
	{
	  EZ_WidgetAWidth(children) = 0;
	  if(EZ_GetWidgetExpandFlag(children) != 0  &&
	     EZ_WidgetType(children) < EZ_WIDGET_ITOPLEVEL &&
	     EZ_GetWidgetHeightSetFlag(children) == 0 &&
	     EZ_GetWidgetNoGeomFlag(children) == 0)
	    {
	      int ttt;
	      if(EZ_GetWidgetYSetFlag(children)) ttt = th - EZ_WidgetOriginY(children) - auxy;
	      else ttt = th - auxy2 - hadjust;
	      EZ_WidgetAWidth(children) =EZ_WidgetWidth(children);
	      EZ_WidgetAHeight(children) =  ttt;
	      EZ_SetChildrenPositionAndSize(children, EZ_WidgetWidth(children), ttt);
	    }
	  children = EZ_WidgetSibling(children);
	}
    }

  /*---------------------------------------------
   *  If a fill mode is set on this widget, fill 
   *  its scalable children and recompute the
   *  geometry of all children widgets.
   *------------------------------------------*/
  ncWS = EZ_CountWidthScalableChildren(widget);
  ncHS = EZ_CountHeightScalableChildren(widget);

  cw -= EZ_WidgetCW(widget);
  ch -= EZ_WidgetCH(widget);

  switch(fillmode)
    {
    case  EZ_FILL_VERTICALLY:
      if(stacking >= EZ_VERTICAL && stacking <= EZ_VERTICAL_CENTER)
	{
	  if(ncHS > 0)
	    {
	      htmp = (th - ch) / ncHS;
	      if(htmp > 0)
		{
		  int herror = th - ch - ncHS * htmp;
		  EZ_WidgetMinHeight(widget) = th; 
		  EZ_WidgetCH(widget) = 0;
		  children = EZ_WidgetChildren(widget);
		  while(children)
		    {
		      if(EZ_GetWidgetHeightSetFlag(children) == 0 && 
			 EZ_WidgetType(children) < EZ_WIDGET_ITOPLEVEL &&
			 EZ_GetWidgetNoGeomFlag(children) == 0)
			{
			  int tmpwidth = EZ_WidgetWidth(children);
                          int epsilon = (herror <= 0? 0: 1);
			  int tmpheight = EZ_WidgetHeight(children) + htmp + epsilon;
			  EZ_WidgetAWidth(children) =tmpwidth;
			  EZ_WidgetAHeight(children) = tmpheight;
			  EZ_SetChildrenPositionAndSize(children, tmpwidth,tmpheight);
			  herror--;
			}
		      children = EZ_WidgetSibling(children);
		    }
		}
	    }
	}
      else  /* children are stacked horizontally */
	{
	  htmp = th - (bwpady << 1) - auxy2 - hadjust;
	  children = EZ_WidgetChildren(widget);
	  while(children)
	    {
	      if(EZ_GetWidgetNoGeomFlag(children) == 0 &&
		 EZ_WidgetType(children) < EZ_WIDGET_ITOPLEVEL &&
		 EZ_WidgetHeight(children) < htmp  && 
		 EZ_GetWidgetHeightSetFlag(children) == 0)
		{
		  int tmpwidth = EZ_WidgetWidth(children);
		  int tmpheight = htmp;
		  EZ_WidgetAWidth(children) =tmpwidth;
		  EZ_WidgetAHeight(children) = tmpheight;
		  EZ_SetChildrenPositionAndSize(children, tmpwidth, tmpheight);
		}
	      children = EZ_WidgetSibling(children);
	    }
	}
      break;
    case EZ_FILL_HORIZONTALLY:
      if(stacking >= EZ_HORIZONTAL && stacking <= EZ_HORIZONTAL_CENTER)
	{
	  if(ncWS > 0)
	    {
	      wtmp = (tw - cw) / ncWS;
	      if(wtmp > 0)
		{
		  int werror = tw - cw - ncWS * wtmp;
		  EZ_WidgetMinWidth(widget) = tw; 
		  EZ_WidgetCW(widget) = 0;
		  children = EZ_WidgetChildren(widget);
		  while(children)
		    {
		      if(EZ_GetWidgetWidthSetFlag(children) == 0 && 
			 EZ_WidgetType(children) < EZ_WIDGET_ITOPLEVEL &&
			 EZ_GetWidgetNoGeomFlag(children) == 0 )
			{
                          int epsilon = (werror <= 0? 0: 1);
			  int tmpwidth = EZ_WidgetWidth(children) + wtmp + epsilon;
			  int tmpheight = EZ_WidgetHeight(children);
			  EZ_WidgetAWidth(children) =tmpwidth;
			  EZ_WidgetAHeight(children) = tmpheight;
			  EZ_SetChildrenPositionAndSize(children, tmpwidth, tmpheight);
			  werror--;
			}
		      children = EZ_WidgetSibling(children);
		    }
		}
	    }	      
	}
      else 
	{
	  wtmp = tw - (bwpadx << 1) - auxxT;
	  children = EZ_WidgetChildren(widget);
	  while(children)
	    {
	      if((EZ_GetWidgetNoGeomFlag(children) == 0) &&
		 EZ_WidgetType(children) < EZ_WIDGET_ITOPLEVEL &&
		 EZ_GetWidgetWidthSetFlag(children) == 0 &&
		 EZ_WidgetWidth(children) < wtmp )
		{
		  int tmpwidth = wtmp;
		  int tmpheight = EZ_WidgetHeight(children);
		  EZ_WidgetAWidth(children) =tmpwidth;
		  EZ_WidgetAHeight(children) = tmpheight;
		  EZ_SetChildrenPositionAndSize(children, tmpwidth, tmpheight);
		}
	      children = EZ_WidgetSibling(children);
	    }	      
	}
      break;
    case EZ_FILL_BOTH:
      if(stacking >= EZ_HORIZONTAL && stacking <= EZ_HORIZONTAL_CENTER)
	{
	  int new_width, new_height, werror=0; 
	  
	  htmp = th - (bwpady << 1) - auxy2 - hadjust;
	  if(ncWS > 0)
	    {
	      wtmp = (tw - cw) /ncWS;
	      werror = tw - cw - wtmp * ncWS;
	      EZ_WidgetMinWidth(widget) = tw; 
	      EZ_WidgetCW(widget) = 0;
	    }
	  else wtmp = 0;
	  
	  children = EZ_WidgetChildren(widget);
	  while(children)
	    {
	      if(EZ_GetWidgetNoGeomFlag(children) == 0 &&
		 EZ_WidgetType(children) < EZ_WIDGET_ITOPLEVEL)
		{
		  if(wtmp > 0 || EZ_WidgetHeight(children) < htmp)
		    {
		      new_width = EZ_WidgetWidth(children);
		      new_height = EZ_WidgetHeight(children);
		      
		      if( wtmp > 0 && EZ_GetWidgetWidthSetFlag(children) == 0)
			{
                          int epsilon = (werror <= 0? 0: 1);
                          new_width += wtmp + epsilon;
                          werror--; 
                        }
		      if(EZ_GetWidgetHeightSetFlag(children) == 0 &&
			 EZ_WidgetHeight(children) < htmp)  new_height = htmp;
		      EZ_WidgetAWidth(children) =new_width;
		      EZ_WidgetAHeight(children) = new_height;		  
		      EZ_SetChildrenPositionAndSize(children,new_width, new_height);
		    }
		}
	      children = EZ_WidgetSibling(children);
	    } 
	}
      else /* if(stacking >= EZ_VERTICAL && stacking <= EZ_VERTICAL_CENTER) */
	{
	  int new_width, new_height, herror;
	  
	  wtmp = tw - (bwpadx << 1) - auxxT;
	  if(ncHS > 0)
	    {
	      htmp = (th - ch) / ncHS;
	      herror = th - ch - ncHS * htmp;
	      EZ_WidgetMinHeight(widget) = th; 
	      EZ_WidgetCH(widget) = 0;
	    }
	  else { htmp = 0; herror = 0; }

	  children = EZ_WidgetChildren(widget);
	  while(children)
	    {
	      if(EZ_GetWidgetNoGeomFlag(children) == 0 &&
		 EZ_WidgetType(children) < EZ_WIDGET_ITOPLEVEL )
		{
		  new_width = EZ_WidgetWidth(children);
		  new_height = EZ_WidgetHeight(children);
		  
		  if(htmp > 0 || EZ_WidgetWidth(children) < wtmp)
		    {
		      if( EZ_GetWidgetWidthSetFlag(children) == 0 &&
			 EZ_WidgetWidth(children) < wtmp) new_width = wtmp;
		      if( htmp > 0 && EZ_GetWidgetHeightSetFlag(children) == 0)
			{
                          int epsilon = (herror <= 0? 0: 1);
                          new_height += htmp + epsilon;
                          herror--;
                        }
		      EZ_WidgetAWidth(children) =new_width;
		      EZ_WidgetAHeight(children) = new_height; 
		      EZ_SetChildrenPositionAndSize(children, new_width, new_height);
		    }
		}
	      children = EZ_WidgetSibling(children);
	    } 
	}
      break;
    default:
      break;
    }
      
  /*---------------------------------------------
   * Now compute the position of children widgets
   *---------------------------------------------*/
  cw = EZ_WidgetMinWidth(widget) - EZ_WidgetCW(widget);
  ch = EZ_WidgetMinHeight(widget) - EZ_WidgetCH(widget); 

  sepx = EZ_WidgetSepX(widget);       /* horizontal space between neighboring children */
  sepy = EZ_WidgetSepY(widget);       /* vertical space between neighboring children   */

      
  if(stacking >= EZ_HORIZONTAL && stacking <= EZ_HORIZONTAL_CENTER)
    {                           /* Children are stacked horizontally */
      dx = 0.0;
      if(stacking == EZ_HORIZONTAL_LEFT)         x0 = 0.0;
      else if(stacking == EZ_HORIZONTAL_RIGHT)   x0 = (float)(tw - cw);
      else if(stacking == EZ_HORIZONTAL_CENTER)  x0 = 0.5* (tw - cw);
      else
	{
	  dx = (float)(tw - cw)/((float)numchildren + 1.0);
	  x0 = dx;
	}
      x0 += (float)(bwpadx + auxxL);    /* starting  X  */
      dx += (float)sepx;  
      xx = 0.0;
      children = EZ_WidgetChildren(widget);
      while(children)
	{
	  if(EZ_GetWidgetNoGeomFlag(children) == 0 &&
	     EZ_WidgetType(children) < EZ_WIDGET_ITOPLEVEL)
	    {
	      if(alignment == EZ_TOP_ALIGNED) 
		yy = (float)(bwpady + auxy + hadjust);
	      else if(alignment == EZ_BOTTOM_ALIGNED)  
		yy = (float)( th - EZ_WidgetHeight(children) - bwpady - auxy);
	      else   
		yy = 0.5*(float)(th + hadjust - EZ_WidgetHeight(children));
	      
	      if(EZ_GetWidgetXSetFlag(children) == 0)
		EZ_WidgetOriginX(children) =  (int) (xx + x0);
	      if(EZ_GetWidgetYSetFlag(children) == 0)	      
		{
		  if(EZ_GetWidgetExpandFlag(children) != 0  &&
		     EZ_GetWidgetHeightSetFlag(children) == 0)
		    EZ_WidgetOriginY(children) = auxy + hadjust;
		  else 	EZ_WidgetOriginY(children) =  (int) (yy);
		}
	      xx += (dx + EZ_WidgetWidth(children)); 
	    }
	  children = EZ_WidgetSibling(children);
	}
    }
  /* Children are stacked vertically */
  else /* stacking >= EZ_VERTICAL && stacking <= EZ_VERTICAL_CENTER */
    {
      dy = 0.0;
      if(stacking == EZ_VERTICAL_TOP)           y0 = 0.0;
      else if(stacking == EZ_VERTICAL_BOTTOM)   y0 = (float)(th - ch);
      else if(stacking == EZ_VERTICAL_CENTER)   y0 = 0.5 * (th - ch);
      else
	{
	  dy = (float)(th - ch)/((float)numchildren + 1.0);
	  y0 = dy;
	}
      y0 += (float)(bwpady + hadjust + auxy);       /* Starting Y */
      dy += (float) sepy; 
      yy = 0.0;
      children = EZ_WidgetChildren(widget);
      while(children)
	{
	  if(EZ_GetWidgetNoGeomFlag(children) == 0 &&
	     EZ_WidgetType(children) < EZ_WIDGET_ITOPLEVEL )
	    {
	      if(alignment == EZ_LEFT_ALIGNED)
  		xx = (float)(bwpadx + auxxL);
	      else if(alignment == EZ_RIGHT_ALIGNED)  
		xx = (float)( tw -  EZ_WidgetWidth(children) - bwpadx - auxxR);
	      else     
		xx = 0.5*(tw + auxxL - auxxR - EZ_WidgetWidth(children));

	      if(EZ_GetWidgetXSetFlag(children) == 0)	      
		{
		  if(EZ_GetWidgetExpandFlag(children) != 0  &&
		     EZ_GetWidgetWidthSetFlag(children) == 0)
		    EZ_WidgetOriginX(children) = auxxL;
		  else 	EZ_WidgetOriginX(children) = (int) (xx);
		}
	      if(EZ_GetWidgetYSetFlag(children) == 0)
		EZ_WidgetOriginY(children) = (int) (yy + y0);
	      yy += (dy + EZ_WidgetHeight(children)); 
	    }
	  children = EZ_WidgetSibling(children);
	}
    }
}
/*************************************************************************************/
#undef _EZ_WIDGET_GEOM_C_
