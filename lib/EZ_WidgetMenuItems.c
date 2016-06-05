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
#define _EZ_WIDGET_MENU_ITEMS_C_

#include "EZ_Widget.h"

/*********************************************************************
 * 
 *  Functions implemented in this file:
 */
EZ_Widget        *EZ_CreateMenuSeparator MY_ANSIARGS((EZ_Widget *parent));
void             EZ_DrawWidgetMenuSeparator MY_ANSIARGS((EZ_Widget *widget));
void             EZ_ComputeWidgetMenuSeparatorSize MY_ANSIARGS((EZ_Widget *widget, int *w, int *h));

EZ_Widget        *EZ_CreateMenuSubMenu MY_ANSIARGS((EZ_Widget *parent, char *lab, int udl));
void             EZ_DrawWidgetMenuSubMenu MY_ANSIARGS((EZ_Widget *widget));
void             EZ_ComputeWidgetMenuSubMenuSize MY_ANSIARGS((EZ_Widget *widget, int *w, int *h));
void             EZ_FreeWidgetMenuSubMenuData MY_ANSIARGS((EZ_Widget *widget));
void             EZ_MenuSubMenuEventHandle MY_ANSIARGS((EZ_Widget *widget, XEvent *event));

EZ_Widget        *EZ_CreateMenuNormalButton MY_ANSIARGS((EZ_Widget *parent, char *lab, int udl, int ret));
void             EZ_MenuNormalButtonEventHandle MY_ANSIARGS((EZ_Widget *widget, XEvent *event));

EZ_Widget        *EZ_CreateMenuCheckButton MY_ANSIARGS((EZ_Widget *parent, char *lab, int udl, 
							int onv, int offv, int onoff, int ret));

void             EZ_MenuCheckButtonEventHandle MY_ANSIARGS((EZ_Widget *widget, XEvent *event));

EZ_Widget        *EZ_CreateMenuRadioButton MY_ANSIARGS((EZ_Widget *parent, char *lab, int udl,
							int gid, int val, int ret));
void             EZ_MenuRadioButtonEventHandle MY_ANSIARGS((EZ_Widget *widget, XEvent *event));

void             EZ_SetSubMenuMenu MY_ANSIARGS((EZ_Widget *widget, EZ_Widget *menu));

void             EZ_MenuSeparatorEventHandle MY_ANSIARGS((EZ_Widget *widget, XEvent *event));

EZ_Widget        *EZ_GetMenuItemCompaion MY_ANSIARGS((EZ_Widget *widget));
/*********************************************************************
 * 
 *  Local Variables.
 */
static EZ_WidgetHandle EZ_MenuSeparatorHandle =
{
  EZ_ComputeWidgetMenuSeparatorSize,
  EZ_DrawWidgetMenuSeparator,
  EZ_FreeUnknownData,
  EZ_MenuSeparatorEventHandle,    /* response only to Expose */
};

static EZ_WidgetHandle EZ_MenuSubmenuHandle =
{                  
  EZ_ComputeWidgetMenuSubMenuSize,
  EZ_DrawWidgetMenuSubMenu,
  EZ_FreeWidgetMenuSubMenuData,
  EZ_MenuSubMenuEventHandle,
};

static EZ_WidgetHandle EZ_MenuNormalButtonHandle =
{    
  EZ_ComputeWidgetButtonSize,
  EZ_DrawWidgetButton,
  EZ_FreeWidgetNButtonData,
  EZ_MenuNormalButtonEventHandle,
};

static EZ_WidgetHandle EZ_MenuCheckButtonHandle =
{  
  EZ_ComputeWidgetCButtonSize,    
  EZ_DrawWidgetCheckButton,
  EZ_FreeWidgetCButtonData,
  EZ_MenuCheckButtonEventHandle,
};

static EZ_WidgetHandle EZ_MenuRadioButtonHandle =
{
  EZ_ComputeWidgetRButtonSize,
  EZ_DrawWidgetRadioButton,
  EZ_FreeWidgetRButtonData,
  EZ_MenuRadioButtonEventHandle,
};


/*********************************************************************
 ***                                                               ***
 ***                    Menu item:  separator                      ***
 ***                                                               ***
 *********************************************************************/

EZ_Widget  *EZ_CreateMenuSeparator(parent)
     EZ_Widget  *parent;     /* parent widget    */
{
  EZ_Widget  *wptr;

  wptr = EZ_CreateNewWidget(parent);
  /*--------------------------------------------------
   * Register the handling functions for CButton.
   *  has to be done after  EZ_CreateNewWiget.
   *-------------------------------------------------*/
  EZ_WidgetHandlingFunctions[EZ_WIDGET_MENU_SEPARATOR] = &EZ_MenuSeparatorHandle;

  EZ_SetWidgetTypeAndNames(wptr,EZ_WIDGET_MENU_SEPARATOR);
  EZ_WidgetBorderStyle(wptr)  = EZ_BORDER_NONE;
  EZ_WidgetBorderWidth(wptr)  = 0;
  EZ_WidgetWidth(wptr)  = 10;    /* default size */
  EZ_WidgetHeight(wptr) = 3;

  EZ_MenuSeparatorCopy(wptr)   = NULL;

  return(wptr);
}

/*
 *  Figure out the dimension of a menu separator
 */
void EZ_ComputeWidgetMenuSeparatorSize(widget, w, h)
    EZ_Widget *widget;
     int             *w, *h;
{
  EZ_SetWidgetSizeComputedFlag(widget);

  *w = EZ_WidgetWidth(widget);
  *h = EZ_WidgetHeight(widget);
}

/*
 *  Draw a Menu separator
 */

void  EZ_DrawWidgetMenuSeparator(wptr)
     EZ_Widget *wptr;
{
  int             w, h,y, tile;
  Pixmap          pixmap;
  Window          win;
  GC              gc, gc1, junkgc;
  unsigned long   bgpv;
  int             ox=0, oy=0;

  win = EZ_WidgetWindow(wptr);
  w   = EZ_WidgetWidth(wptr);
  h   = EZ_WidgetHeight(wptr);

  /*-----------------------------------------------------------
   *  Create a pixmap, draw into this pixmap in background and
   *  copy to the button window when finished.
   *----------------------------------------------------------*/
  pixmap = XCreatePixmap(EZ_DisplayForWidgets, win, w, h, EZ_DepthForWidgets);   
  /*----------------------------------------------------------
   *       Fill the background 
   *---------------------------------------------------------*/
  if((tile =EZ_GetBackgroundGC(wptr, &gc, &bgpv, 0, 0)))
    {
      EZ_GetWidgetTileOrigin(wptr, &ox, &oy);      
      XSetTSOrigin(EZ_Display, gc, ox, oy);
    }
  XFillRectangle(EZ_DisplayForWidgets, pixmap, gc, 0,0, w, h); 

  if(EZ_GetDarkBrightNormalBDGC(wptr, &gc, &gc1, &junkgc))
    {
      if(tile== 0) EZ_GetWidgetTileOrigin(wptr, &ox, &oy);      
      XSetTSOrigin(EZ_Display, gc, ox, oy);      
      XSetTSOrigin(EZ_Display, gc1, ox, oy);      
    }
  y = (h>>1) - 1;
  XDrawLine(EZ_DisplayForWidgets,pixmap, gc,  0, y,   w, y);
  y = (h>>1) + 1;
  XDrawLine(EZ_DisplayForWidgets,pixmap, gc1,  0, y,   w, y);

  XCopyArea(EZ_DisplayForWidgets,pixmap,win, EZ_WRITABLEGC,0,0,w,h,0,0); 
  XFreePixmap(EZ_DisplayForWidgets, pixmap);                             
}

void EZ_MenuSeparatorEventHandle(widget, event)
     EZ_Widget *widget;
     XEvent          *event;
{
  if(widget == (EZ_Widget *)NULL)   return;

  if(event->type == Expose)   EZ_DrawWidget(widget);      
  else  if(event->type == KeyPress || (event->type == ButtonPress &&
				       event->xbutton.button == EZ_Btn1))
    EZ_HandleToplevelMenuEvents(widget, event); 
}



/*********************************************************************
 ***                                                               ***
 ***                   Menu item:  submenu                         ***
 ***                                                               ***
 *********************************************************************/

EZ_Widget  *EZ_CreateMenuSubMenu(parent,label, underline)
     EZ_Widget  *parent;     /* parent widget    */
     char       *label;
     int        underline;
{
  EZ_Widget  *wptr;

  wptr = EZ_CreateNewWidget(parent);
  /*--------------------------------------------------
   * Register the handling functions for ...
   *  has to be done after  EZ_CreateNewWiget.
   *-------------------------------------------------*/
  EZ_WidgetHandlingFunctions[EZ_WIDGET_MENU_SUBMENU] = &EZ_MenuSubmenuHandle;

  EZ_SetWidgetTypeAndNames(wptr,EZ_WIDGET_MENU_SUBMENU);
  EZ_WidgetBorderStyle(wptr)  = EZ_BORDER_NONE;
  EZ_WidgetLabelPosition(wptr) = EZ_LEFT;
  EZ_WidgetBorderWidth(wptr)  = 2;
  EZ_WidgetPadY(wptr) = 1;
  EZ_WidgetPadB(wptr) = 0;

  EZ_WidgetSepX(wptr) = EZ_WIDGET_DEFAULT_PADX;
  EZ_WidgetSepY(wptr) = EZ_WIDGET_DEFAULT_PADY;

  EZ_SubMenuLabel(wptr) = EZ_AllocCopyString(label);
  EZ_SubMenuLabelLength(wptr) = (label == (char *)NULL? 1 : strlen(label));
  EZ_SubMenuPixmap(wptr) = (EZ_Bitmap *)NULL;
  EZ_SubMenuFont(wptr)  = EZ_GetFontFromId(EZ_MENU_FONT);

  EZ_SubMenuTWidth(wptr) = 0;
  EZ_SubMenuTHeight(wptr) = 0;
  EZ_SubMenuFontAscent(wptr) = 0;
  EZ_SubMenuArrowSize(wptr) = 8;
  EZ_SubMenuArrowBWidth(wptr) = 1;

  EZ_SubMenuLineLength(wptr) = EZ_DEFAULT_LABEL_LINE_LENGTH;
  EZ_SubMenuNLines(wptr)  = 1;

  EZ_SubMenuLabel2(wptr) = (char *)NULL;
  EZ_SubMenuLabel2Length(wptr) = 0;
  EZ_SubMenuLabel2Width(wptr) = 0;

  if(label && underline >= 0 && underline < EZ_SubMenuLabelLength(wptr) && isprint(label[underline]))
    {
      EZ_SubMenuUnderLine(wptr) = underline;
      EZ_SubMenuModifiers(wptr) = Mod1Mask;
      EZ_SubMenuShortcut(wptr)  = (EZ_SubMenuLabel(wptr) + underline);
      EZ_InsertGlobalKeyPressEvent(wptr, Mod1Mask, (label+underline));
    }
  else
    {
      EZ_SubMenuUnderLine(wptr) = -1;
      EZ_SubMenuModifiers(wptr) = 0;
      EZ_SubMenuShortcut(wptr)  = (char *)NULL;
    }

  EZ_SubMenuTheMenu(wptr) = (EZ_Widget *)NULL;
  EZ_SubMenuCopy(wptr) = (EZ_Widget *)NULL;

  EZ_SetWidgetFocusableFlag(wptr);

  return(wptr);
}

/*
 *  Free private data for a submenu
 */
void  EZ_FreeWidgetMenuSubMenuData(widget)
     EZ_Widget *widget;
{
  if(EZ_SubMenuLabel(widget) != (char *)NULL)
    (void) my_free ((char *)EZ_SubMenuLabel(widget));
  if(EZ_SubMenuLabel2(widget) != (char *)NULL)
    (void) my_free ((char *)EZ_SubMenuLabel2(widget));
  if(EZ_SubMenuPixmap(widget) != (EZ_Bitmap *)NULL)
    EZ_FreeBitmap(EZ_SubMenuPixmap(widget));
}

/*
 *  Figure out the dimension of a submenu
 */
void EZ_ComputeWidgetMenuSubMenuSize(widget, w, h)
    EZ_Widget *widget;
     int             *w, *h;
{
  int length, height, cw, ch;
  int pwidth, pheight, ww, hh;
  int orien = EZ_WidgetOrientation(widget);
  int ipadx=0, ipady = 0;

  if(!(EZ_GetWidgetSizeComputedFlag(widget)))
    { 
      int nlines, twidth;
      EZ_Bitmap *bitmap = EZ_SubMenuPixmap(widget);
      
      if(!bitmap || EZ_PTLabel)
        {
          EZ_FigureLabelExtent(EZ_SubMenuFont(widget),
                               EZ_SubMenuLabel(widget),
                               (int)EZ_SubMenuLabelLength(widget),
                               (int)EZ_SubMenuLineLength(widget),
                               &nlines, &twidth);
          EZ_SubMenuTWidth(widget) = twidth;
          EZ_SubMenuNLines(widget) = nlines;

          EZ_SubMenuTHeight(widget) = EZ_SubMenuFont(widget)->ascent +
            EZ_SubMenuFont(widget)->descent;
          EZ_SubMenuFontAscent(widget) = EZ_SubMenuFont(widget)->ascent;

          EZ_SubMenuArrowSize(widget) = 
            ((EZ_SubMenuTHeight(widget) *  EZ_SubMenuNLines(widget))<<1)/3;

          if(EZ_SubMenuLabel2Length(widget) != 0)
            {
              EZ_SubMenuLabel2Width(widget) = EZ_LABEL_SEPARATION +
                XTextWidth(EZ_SubMenuFont(widget), 
                           EZ_SubMenuLabel2(widget),
                           EZ_SubMenuLabel2Length(widget));
            }
          else
            {
              EZ_SubMenuLabel2Width(widget) = 0;
            }
        }
      else
        {
          EZ_SubMenuTWidth(widget) = 0;
          EZ_SubMenuNLines(widget) = 0;

          EZ_SubMenuTHeight(widget) = 0;
          EZ_SubMenuFontAscent(widget) = 0;
          EZ_SubMenuArrowSize(widget) =  12;
          EZ_SubMenuLabel2Width(widget) = 0;
        }
      if(bitmap)
	{
          EZ_SubMenuMWidth(widget) = bitmap ->width;
          EZ_SubMenuMHeight(widget) = bitmap ->height;
        }
      else
        {
          EZ_SubMenuMWidth(widget) = 0;
          EZ_SubMenuMHeight(widget) = 0;
        }
      EZ_SetWidgetSizeComputedFlag(widget);
    }

  EZ_WidgetAuxXRight(widget) = EZ_SubMenuLabel2Width(widget) +
    EZ_SubMenuArrowSize(widget) + (EZ_SubMenuArrowBWidth(widget)<<1) + 8;
  length = EZ_SubMenuTWidth(widget) + EZ_WidgetAuxXRight(widget);
  height = MAXV(EZ_SubMenuTHeight(widget)* EZ_SubMenuNLines(widget),
		EZ_SubMenuArrowSize(widget) + (EZ_SubMenuArrowBWidth(widget)<<1));
  pwidth = EZ_SubMenuMWidth(widget);
  pheight = EZ_SubMenuMHeight(widget);
  if(orien < EZ_VERTICAL) /* horizontal */
    {
      if(pwidth && length) ipadx = EZ_WidgetSepX(widget);
      ww = ipadx + length + pwidth;
      hh = pheight > height? pheight: height;
    }
  else
    {
      if(pheight && height) ipady = EZ_WidgetSepY(widget);
      ww = pwidth > length? pwidth: length;
      hh = ipady + height + pheight;
    }
  cw = EZ_WidgetPadX(widget) + EZ_WidgetBorderWidth(widget) + EZ_WidgetPadB(widget);
  ch = EZ_WidgetPadY(widget) + EZ_WidgetBorderWidth(widget) + EZ_WidgetPadB(widget);

  cw = cw + cw + ww;  /* extra space between label and arrow */
  ch = ch + ch + hh;

  *w = cw;
  *h = ch;
}

/*
 *
 *  Draw a SubMenu item. (not the menu !)
 */

void  EZ_DrawWidgetMenuSubMenu(wptr)
     EZ_Widget *wptr;
{
  int             w, h,x,y, xx, yy, fillx, filly, padb, padb2, offset, offseta = 0;
  Pixmap          pixmap;
  Window          win;
  GC              gc;
  int             arrow_style, tileOx = 0, tileOy = 0;
  int             orien, tx0 = 0, ty0 = 0, pwidth, twidth, pheight, theight;
  unsigned long   bgpv;
  EZ_Bitmap       *bitmap;

  win = EZ_WidgetWindow(wptr);
  w   = EZ_WidgetWidth(wptr);
  h   = EZ_WidgetHeight(wptr);
  padb = EZ_WidgetPadB(wptr);
  padb2 = padb + padb;

  EZ_GetWidgetLabelPosition(wptr, &fillx, &filly);

  /*-----------------------------------------------------------
   *  Create a pixmap, draw into this pixmap in background and
   *  copy to the SubMenu window when finished.
   *----------------------------------------------------------*/
  pixmap = XCreatePixmap(EZ_DisplayForWidgets, win, w, h, EZ_DepthForWidgets);    
  EZ_FixFocusHighlightBorder(wptr, pixmap, w, h, padb);

  if(EZ_OffsetEnabled && EZ_WidgetBorderStyle(wptr) == EZ_BORDER_DOWN)  offseta = 1;

  if(EZ_GetBackgroundGC(wptr, &gc, &bgpv, 1,0))
    {
      EZ_GetWidgetTileOrigin(wptr, &tileOx, &tileOy);      
      tileOx += offseta; tileOy += offseta;
      XSetTSOrigin(EZ_Display, gc, tileOx, tileOy);
    }
  XFillRectangle(EZ_DisplayForWidgets, pixmap, gc, padb,padb, w-padb2, h-padb2); 

  /*--------------------------------------------------------
   *  Draw the label.
   *-------------------------------------------------------*/
  bitmap = EZ_SubMenuPixmap(wptr); 
  xx = EZ_WidgetXOffset(wptr) + fillx;
  yy = EZ_WidgetYOffset(wptr) + filly;
  
  offset = padb + offseta;
  x = xx + EZ_WidgetPadX(wptr) + EZ_WidgetBorderWidth(wptr) + offset;
  y = yy + EZ_SubMenuFontAscent(wptr) + EZ_WidgetBorderWidth(wptr) + EZ_WidgetPadY(wptr) + offset;
  orien = EZ_WidgetOrientation(wptr);
  pwidth = EZ_SubMenuMWidth(wptr);
  pheight = EZ_SubMenuMHeight(wptr);
  twidth = EZ_SubMenuTWidth(wptr);
  theight = EZ_SubMenuTHeight(wptr) * EZ_SubMenuNLines(wptr);

  if(orien >= EZ_VERTICAL)
    {
      if(bitmap)
        {
          int px0, py0;
          if(twidth > pwidth)
            {
              px0=(twidth - pwidth)>>1; py0 = 0;
              tx0 = 0; ty0 = pheight + EZ_WidgetSepY(wptr);
            }
          else
            {
              px0 = 0; py0=0;
              tx0 = (pwidth-twidth)>>1; 
              ty0 = bitmap->height + EZ_WidgetSepY(wptr);                  
            }
          EZ_RenderPixmapLabel(wptr, bitmap, pixmap, x+px0, y+py0
                               - (int) (EZ_SubMenuFontAscent(wptr)));
        }
    }
  else /* horizontal */
    {
      if(bitmap)
        {
          int px0, py0;
          if(theight > pheight) 
            {
              px0 = 0; py0 =  (theight-pheight)>>1;
              tx0 = bitmap->width + EZ_WidgetSepX(wptr); ty0 = 0; 
            }
          else
            {
              px0 = 0; py0 = 0;
              tx0 = bitmap->width + EZ_WidgetSepX(wptr);
              ty0 = (pheight-theight)>>1;
            }
          EZ_RenderPixmapLabel(wptr, bitmap, pixmap, x+px0, y+py0
                               - (int) (EZ_SubMenuFontAscent(wptr)));
        }
    }
  x += tx0; y += ty0;
  if(!bitmap || EZ_PTLabel)
    {
      if( EZ_GetWidgetDisabledFlag(wptr) )
        {
          GC  bgc, lgc, ngc;
          if(EZ_GetDarkBrightNormalBDGC(wptr, &bgc, &lgc, &ngc))
            {
              XSetTSOrigin(EZ_Display, bgc, tileOx, tileOy);
              XSetTSOrigin(EZ_Display, lgc, tileOx, tileOy);
              XSetTSOrigin(EZ_Display, ngc, tileOx, tileOy);
            }
          XSetFont(EZ_DisplayForWidgets, bgc, EZ_WidgetFont(wptr)->fid);
          XSetFont(EZ_DisplayForWidgets, lgc, EZ_WidgetFont(wptr)->fid);
          XSetFont(EZ_DisplayForWidgets, ngc, EZ_WidgetFont(wptr)->fid);

          EZ_RenderLabelTextDisabled(pixmap, lgc, bgc, ngc, x, y, 
                                     (int)EZ_SubMenuTHeight(wptr), EZ_SubMenuLabel(wptr),
                                     (int)EZ_SubMenuLabelLength(wptr), (int)EZ_SubMenuLineLength(wptr),
                                     (int)EZ_SubMenuNLines(wptr),
                                     (int)EZ_SubMenuTWidth(wptr),
                                     (int)EZ_WidgetJustification(wptr),
                                     EZ_SubMenuFont(wptr));
        }
      else
        {
          int shadow = EZ_WidgetLabelShadow(wptr);
          if(EZ_SubMenuForeground(wptr) != EZ_DefaultForeground || 
             EZ_SubMenuFont(wptr) != EZ_GetFontFromId(EZ_MENU_FONT))
            {
              XSetFont(EZ_DisplayForWidgets, EZ_WRITABLEGC, EZ_SubMenuFont(wptr)->fid);
              XSetForeground(EZ_DisplayForWidgets, EZ_WRITABLEGC, EZ_SubMenuForeground(wptr));	
              gc = EZ_WRITABLEGC;
            }
          else  gc = EZ_MENU_TEXTGC;
	  
          EZ_RenderLabelText(pixmap, gc, x, y, (int)EZ_SubMenuTHeight(wptr), EZ_SubMenuLabel(wptr),
                             (int)EZ_SubMenuLabelLength(wptr), (int)EZ_SubMenuLineLength(wptr),
                             (int)EZ_SubMenuNLines(wptr),
                             (int)EZ_SubMenuTWidth(wptr),
                             (int)EZ_WidgetJustification(wptr),
                             EZ_SubMenuFont(wptr), shadow);

          if(EZ_SubMenuLabel2Length(wptr) != 0)
            {
              int tmpx =  w - (EZ_WidgetPadX(wptr) + EZ_WidgetBorderWidth(wptr) + padb +
                               EZ_WidgetAuxXRight(wptr)) + offseta + EZ_LABEL_SEPARATION;
              int tmpy = ((h+EZ_SubMenuFontAscent(wptr))>>1) + offseta;
              if(shadow)
                {
                  int shadowx, shadowy;
                  if(shadow &0xf0)
                    {
                      shadowx = (shadow & 0xf0) >> 4;
                      shadowy = (shadow & 0x0f);
                    }
                  else shadowx = shadowy = shadow;
                  if(shadowx & 8) shadowx= (8-shadowx);
                  if(shadowy & 8) shadowy= (8-shadowy);
                  XDrawString(EZ_DisplayForWidgets, 
                              pixmap,  EZ_SHADOWGC,
                              tmpx+shadowx, tmpy+shadowy,
                              EZ_SubMenuLabel2(wptr),
                              EZ_SubMenuLabel2Length(wptr));
                }
              XDrawString(EZ_DisplayForWidgets, 
                          pixmap,  gc,
                          tmpx, tmpy,
                          EZ_SubMenuLabel2(wptr),
                          EZ_SubMenuLabel2Length(wptr));
            }
          else if(EZ_SubMenuUnderLine(wptr) >= 0 && 
                  EZ_SubMenuUnderLine(wptr) < ((int)EZ_SubMenuLineLength(wptr)>>1))
            {
              int tlen = XTextWidth(EZ_SubMenuFont(wptr),EZ_SubMenuLabel(wptr),
                                    EZ_SubMenuUnderLine(wptr));
              int tlen1 = tlen + XTextWidth(EZ_SubMenuFont(wptr),
                                            EZ_SubMenuLabel(wptr)+EZ_SubMenuUnderLine(wptr), 1);
              int base = y + EZ_SubMenuTHeight(wptr) - EZ_SubMenuFontAscent(wptr)-1;
              int boldfont;

              if(EZ_SubMenuFont(wptr) != EZ_GetFontFromId(EZ_MENU_FONT))
                boldfont = (EZ_GetWidgetThinFontFlag(wptr) == 0);
              else boldfont = EZ_LabelFontIsBold; 	  

              if(shadow)
                {
                  int shadowx, shadowy;
                  if(shadow &0xf0)
                    {
                      shadowx = (shadow & 0xf0) >> 4;
                      shadowy = (shadow & 0x0f);
                    }
                  else shadowx = shadowy = shadow;
                  if(shadowx & 8) shadowx= (8-shadowx);
                  if(shadowy & 8) shadowy= (8-shadowy);
                  XDrawLine(EZ_DisplayForWidgets, pixmap, EZ_SHADOWGC,
                            x+tlen+shadowx, base+shadowy, x+tlen1+shadowx, base+shadowy);
                  if(boldfont)
                    XDrawLine(EZ_DisplayForWidgets, pixmap, EZ_SHADOWGC,
                              x+tlen+shadowx, base+1+shadowy, x+tlen1+shadowx, base+1+shadowy);
                }	      
              XDrawLine(EZ_DisplayForWidgets, pixmap, gc,
                        x+tlen, base, x+tlen1, base);
              if(boldfont)
                XDrawLine(EZ_DisplayForWidgets, pixmap, gc,
                          x+tlen, base+1, x+tlen1, base+1);
            }

        }
    }

  arrow_style = EZ_BORDER_UP;
  if(EZ_WidgetBorderStyle(wptr) == EZ_BORDER_UP) arrow_style = EZ_BORDER_DOWN;

  /*--------------------------------------------------------
   *  Draw the little arrow.
   *-------------------------------------------------------*/  
  x = w - (EZ_WidgetPadX(wptr) + EZ_WidgetBorderWidth(wptr) + padb +
	   EZ_SubMenuArrowSize(wptr) + ( EZ_SubMenuArrowBWidth(wptr)<<1) );
  y =  ((h - EZ_SubMenuArrowSize(wptr))>>1) -  EZ_SubMenuArrowBWidth(wptr);
    
  EZ_DrawHRArrow(wptr,pixmap,x,y,
		 EZ_SubMenuArrowSize(wptr) + (EZ_SubMenuArrowBWidth(wptr)<<1),
		 arrow_style, 
		 EZ_SubMenuArrowBWidth(wptr),
		 EZ_GetWidgetHighlightFlag(wptr));

  /*============================================================
   *for complex button with  NW labels 
   */
  {
    EZ_Widget *children = EZ_WidgetChildren(wptr);
    if(children && EZ_WidgetType(children) == EZ_WIDGET_NW_LABEL)
      {
	EZ_DisplayNWLabel(children, pixmap, EZ_WidgetForeground(wptr), bgpv,
			  EZ_GetWidgetDisabledFlag(wptr), EZ_GetWidgetHighlightFlag(wptr),
			  0, 0, offset,1);
      }
  }
  /*============================================================*/  

  
  EZ_DrawRectBorder(wptr, pixmap);
  XCopyArea(EZ_DisplayForWidgets,pixmap,win, EZ_WRITABLEGC,0,0,w,h,0,0); 
  XFreePixmap(EZ_DisplayForWidgets, pixmap);                             
}


/*
 * Submenu Event handle.
 */

void EZ_MenuSubMenuEventHandle(widget, event)
     EZ_Widget *widget;
     XEvent          *event;
{
  if(widget == (EZ_Widget *)NULL)   return;

  if(EZ_GetWidgetDisabledFlag(widget))
    {
      if(event->type == Expose)	EZ_DrawWidget(widget);      
      else if(event->type == EnterNotify)
	EZ_HideSubMenuOf(EZ_WidgetParent(widget));
      else  if(event->type == KeyPress || (event->type == ButtonPress &&
					   event->xbutton.button == EZ_Btn1))
	EZ_HandleToplevelMenuEvents(widget, event); 
      return;
    }

  switch(event->type)
    {
    case Expose:
      EZ_DrawWidget(widget);      
      break;
    case ButtonPress:
      if(!event->xbutton.send_event)
	{
	  EZ_WidgetBorderStyle(widget) = EZ_BORDER_DOWN;
	  EZ_DrawWidget(widget);
	}
      if(EZ_CurrentPopupMenu == (EZ_Widget *)NULL)
	{
	  EZ_DisplayPopupSubMenuOf(widget);
	  EZ_HandleToplevelMenuEvents(widget, event); 
	}
      break;
    case ButtonRelease:
      EZ_WidgetBorderStyle(widget) = EZ_BORDER_UP;
      EZ_DrawWidget(widget);
      break;
    case EnterNotify:
    case FocusIn:
      EZ_SetWidgetHighlightFlag(widget);
      { EZ_InstallHighlightColor(widget); } /*4/29/99*/
      EZ_WidgetBorderStyle(widget) = EZ_BORDER_UP;
      EZ_DrawWidget(widget);
      if(EZ_CurrentPopupMenu != (EZ_Widget *)NULL)  /* 1-7-97 */
	EZ_DisplayPopupSubMenuOf(widget);
      break;
    case LeaveNotify:
      if(event->xcrossing.mode == NotifyGrab  ||
	 event->xcrossing.mode == NotifyUngrab)
	break;
    case FocusOut:
      EZ_ClearWidgetHighlightFlag(widget);
      { EZ_UninstallHighlightColor(widget); } /*4/29/99*/
      EZ_WidgetBorderStyle(widget) = EZ_BORDER_NONE;
      EZ_DrawWidget(widget);      
      break;
    case KeyPress:
      if(EZ_CurrentPopupMenu == (EZ_Widget *)NULL)
	EZ_HandleToplevelMenuEvents(widget, event); 
      break;
    default:
      break;
    }
}


/*********************************************************************
 *
 *  Check whether the menu hierarchy contains a loop.
 *  return 1 if a loop is found.
 */
static int check_loop_in_popup(m1, m)
     EZ_Widget *m1,*m;
{
  EZ_Widget *children;
  EZ_Widget *tmp;

  tmp = m1;
  if(tmp != (EZ_Widget *)NULL)
    {
      children = EZ_WidgetChildren(tmp);
      while(children != (EZ_Widget *)NULL)
	{
	  if(EZ_WidgetType(children) == EZ_WIDGET_MENU_SUBMENU)
	    {
	      if(EZ_SubMenuTheMenu(children) == m) return(1);
	      else if(check_loop_in_popup(EZ_SubMenuTheMenu(children), m)) return(1);
	    }
	  children = EZ_WidgetSibling(children);
	}
    }
  return(0);
}
/*********************************************************************
 *
 *   Set the menu for a submenu.
 */
void EZ_SetSubMenuMenu(subm, m)
     EZ_Widget *subm, *m;
{
  if(subm != (EZ_Widget *)NULL &&
     EZ_WidgetType(subm) == EZ_WIDGET_MENU_SUBMENU)
    {
      if(m != (EZ_Widget *)NULL)
	{
	  if(EZ_WidgetType(m) == EZ_WIDGET_POPUP_MENU)
	    {
	      EZ_SubMenuTheMenu(subm) = m;
	      if(check_loop_in_popup(m,m) == 1)
		{
		  (void)fprintf(stderr, "Error: Loop detected in a menu hierarchy!\n\
       Operation 'EZ_SetSubMenuMenu' is ignored.\n\
       SubMenu item=%lx, Menu=%lx\n",(unsigned long)subm,(unsigned long)m);
		  EZ_SubMenuTheMenu(subm) = (EZ_Widget *)NULL;
		}
	    }
	}
      else
	EZ_SubMenuTheMenu(subm) = m;
    }
}

/*********************************************************************
 ***                                                               ***
 ***                    Menu item:  normal button                  ***
 ***                                                               ***
 *********************************************************************/

EZ_Widget  *EZ_CreateMenuNormalButton(parent, label, underline, ret_value)
     EZ_Widget *parent;
     char            *label;
     int             ret_value, underline;
{
  EZ_Widget *wptr;

  wptr = EZ_CreateNewWidget(parent);
  EZ_WidgetHandlingFunctions[EZ_WIDGET_MENU_NORMAL_BUTTON] = &EZ_MenuNormalButtonHandle;
  EZ_SetWidgetTypeAndNames(wptr,EZ_WIDGET_MENU_NORMAL_BUTTON);

  EZ_ButtonLabel(wptr) = EZ_AllocCopyString(label);
  EZ_ButtonLabelLength(wptr) = (label == (char *)NULL? 1 : strlen(label));
  EZ_ButtonPixmap(wptr) = (EZ_Bitmap *)NULL;
  EZ_ButtonFont(wptr) = EZ_GetFontFromId(EZ_BUTTON_FONT);
  EZ_WidgetBorderStyle(wptr)  = EZ_BORDER_NONE;
  EZ_WidgetLabelPosition(wptr) = EZ_LEFT_2_RIGHT;
  EZ_WidgetPadY(wptr) = 1;
  EZ_WidgetPadB(wptr) = 0;
  EZ_WidgetRetData(wptr) = ret_value;

  EZ_ButtonTWidth(wptr)     = 0;
  EZ_ButtonTHeight(wptr)    = 0;
  EZ_ButtonFontAscent(wptr) = 0;
  EZ_ButtonLabel2(wptr)     = (char *)NULL;
  EZ_ButtonLabel2Length(wptr) = 0;
  EZ_ButtonLabel2Width(wptr)  = 0;
  EZ_ButtonLineLength(wptr) = EZ_DEFAULT_LABEL_LINE_LENGTH;
  EZ_ButtonNLines(wptr)  = 1;
  EZ_ButtonPressed(wptr) = 0;
  EZ_ButtonNoHighlight(wptr) = 0;
  EZ_ButtonSetBorder(wptr)  = 0;
  EZ_SetWidgetFocusableFlag(wptr);
  /* special fields */
  EZ_ButtonCompanion(wptr) = NULL;
  EZ_ButtonFSB(wptr)       = NULL;

  EZ_WidgetSepX(wptr) = EZ_WIDGET_DEFAULT_PADX;
  EZ_WidgetSepY(wptr) = EZ_WIDGET_DEFAULT_PADY;

  if(label && underline >= 0 && underline < EZ_ButtonLabelLength(wptr) && isprint(label[underline]))
    {
      EZ_ButtonUnderLine(wptr) = underline;
      EZ_ButtonModifiers(wptr) = Mod1Mask;
      EZ_ButtonShortcut(wptr)  = (EZ_ButtonLabel(wptr) + underline);
      /*-------------------------------------------------------------
       *  Register this shortcut in the globale KeyPress event table
       *------------------------------------------------------------*/
      EZ_InsertGlobalKeyPressEvent(wptr, Mod1Mask, (label+underline));
    }
  else
    {
      EZ_ButtonUnderLine(wptr) = -1;
      EZ_ButtonModifiers(wptr) = 0;
      EZ_ButtonShortcut(wptr)  = (char *)NULL;
    }
  EZ_ButtonCopy(wptr) = NULL;

  return(wptr);
}
/*
 *   event handle
 */
void EZ_MenuNormalButtonEventHandle(widget, event)
     EZ_Widget *widget;
     XEvent          *event;
{
  if(widget == (EZ_Widget *)NULL) return;

  if(EZ_GetWidgetDisabledFlag(widget))
    {
      if(event->type == Expose)	EZ_DrawWidget(widget);      
      else if(event->type == EnterNotify)
	EZ_HideSubMenuOf(EZ_WidgetParent(widget));
      else  if(event->type == KeyPress || (event->type == ButtonPress &&
				       event->xbutton.button == EZ_Btn1))
	EZ_HandleToplevelMenuEvents(widget, event); 
      return;
    }

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
      EZ_WidgetBorderStyle(widget) = EZ_BORDER_UP;
      EZ_DrawWidget(widget);
      if(EZ_CurrentPopupMenu != (EZ_Widget *)NULL)
	{
	  EZ_WidgetRetData(EZ_CurrentPopupMenu) = EZ_WidgetRetData(widget); /* return value */
	  EZ_PopupLSItem(EZ_CurrentPopupMenu) = widget;   /* this is the selected item     */
	}
      break;
    case EnterNotify:
    case FocusIn:
      EZ_WidgetBorderStyle(widget) = EZ_BORDER_UP;
      EZ_SetWidgetHighlightFlag(widget);
      { EZ_InstallHighlightColor(widget); } /*4/29/99*/
      EZ_HideSubMenuOf(EZ_WidgetParent(widget));
      EZ_DrawWidget(widget);
      break;
    case LeaveNotify:
      if(event->xcrossing.mode == NotifyGrab  ||
	 event->xcrossing.mode == NotifyUngrab)
	break;
    case FocusOut:
      EZ_ClearWidgetHighlightFlag(widget);
      { EZ_UninstallHighlightColor(widget); } /*4/29/99*/
      EZ_WidgetBorderStyle(widget) = EZ_BORDER_NONE;
      EZ_DrawWidget(widget);      
      break;
    case KeyPress:
      if(EZ_CurrentPopupMenu == (EZ_Widget *)NULL)
	EZ_HandleToplevelMenuEvents(widget, event); 
      break;
    default:
      break;
    }
}

/*********************************************************************
 ***                                                               ***
 ***                    Menu item:  check button                   ***
 ***                                                               ***
 *********************************************************************/

EZ_Widget  *EZ_CreateMenuCheckButton(parent, label, underline, onvalue, offvalue, onoff,ret_value)
     EZ_Widget  *parent;     /* parent widget    */
     char             *label;
     int              onvalue, offvalue, onoff;
     int              ret_value, underline;
{

  EZ_Widget *wptr = EZ_CreateNewWidget(parent);
  EZ_WidgetHandlingFunctions[EZ_WIDGET_MENU_CHECK_BUTTON] = &EZ_MenuCheckButtonHandle;
  EZ_SetWidgetTypeAndNames(wptr,EZ_WIDGET_MENU_CHECK_BUTTON);

  EZ_CButtonLabel(wptr) = EZ_AllocCopyString(label);
  EZ_CButtonLabelLength(wptr) = (label == (char *)NULL? 1 : strlen(label));
  EZ_CButtonPixmap(wptr) = (EZ_Bitmap *)NULL;
  EZ_CButtonFont(wptr) = EZ_GetFontFromId(EZ_BUTTON_FONT);
  EZ_WidgetBorderStyle(wptr)  = EZ_BORDER_NONE;
  EZ_WidgetLabelPosition(wptr) = EZ_LEFT_2_RIGHT;
  EZ_CButtonTWidth(wptr)     = 0;
  EZ_CButtonTHeight(wptr)    = 0;
  EZ_CButtonFontAscent(wptr) = 0;
  EZ_CButtonLineLength(wptr) = EZ_DEFAULT_LABEL_LINE_LENGTH;
  EZ_CButtonNLines(wptr)  = 1;
  EZ_CButtonCheckBorderWidth(wptr) = 2;
  EZ_CButtonCheckBorderStyle(wptr) = EZ_BORDER_UP;
  EZ_WidgetPadY(wptr) = 0;
  EZ_WidgetPadB(wptr) = 0;
  EZ_WidgetRetData(wptr) = ret_value;
  EZ_CButtonCheckSizeAdjust(wptr) = 3;
  /* 6-15-97 */
  EZ_CButtonData(wptr) = (int *)my_malloc(3 * sizeof(int), _CBTN_DATA_);
  EZ_CButtonVarValue(wptr) = (onoff == 0)? offvalue : onvalue;
  EZ_CButtonVarOnValue(wptr) = onvalue;
  EZ_CButtonVarOffValue(wptr) = offvalue;
  EZ_CButtonCheckType(wptr) = EZ_SQUARE_INDICATOR;
  EZ_CButtonCheckColor(wptr) = 0;

  EZ_CButtonLabel2(wptr)  = (char *)NULL;
  EZ_CButtonLabel2Length(wptr) = 0;
  EZ_CButtonLabel2Width(wptr) = 0;

  EZ_SetWidgetFocusableFlag(wptr);

  EZ_WidgetSepX(wptr) = EZ_WIDGET_DEFAULT_PADX;
  EZ_WidgetSepY(wptr) = EZ_WIDGET_DEFAULT_PADY;

  if(label && underline >= 0 && underline < EZ_SubMenuLabelLength(wptr) && isprint(label[underline]))
    {
      EZ_CButtonUnderLine(wptr) = underline;
      EZ_CButtonModifiers(wptr) = Mod1Mask;
      EZ_CButtonShortcut(wptr)  = (EZ_CButtonLabel(wptr) + underline);
      /*-------------------------------------------------------------
       *  Register this shortcut in the globale KeyPress event table
       *------------------------------------------------------------*/
      EZ_InsertGlobalKeyPressEvent(wptr, Mod1Mask, (label+underline));
    }
  else
    {
      EZ_CButtonUnderLine(wptr) = -1;
      EZ_CButtonModifiers(wptr) = 0;
      EZ_CButtonShortcut(wptr)  = (char *)NULL;
    }
  EZ_CButtonCopy(wptr) = NULL;
  return(wptr);
}

/*
 *   event handle
 */
void EZ_MenuCheckButtonEventHandle(widget, event)
     EZ_Widget *widget;
     XEvent          *event;
{
  if(widget == (EZ_Widget *)NULL) return;
  
  if(EZ_GetWidgetDisabledFlag(widget))
    {
      if(event->type == Expose)
	EZ_DrawWidget(widget);      
      else if(event->type == EnterNotify)
	EZ_HideSubMenuOf(EZ_WidgetParent(widget));
      else  if(event->type == KeyPress || (event->type == ButtonPress &&
					   event->xbutton.button == EZ_Btn1))
	EZ_HandleToplevelMenuEvents(widget, event); 
      return;
    }

  switch(event->type)
    {
    case Expose:
      EZ_DrawWidget(widget);      
      break;
    case ButtonPress:
      if(EZ_CurrentPopupMenu == (EZ_Widget *)NULL)
	EZ_HandleToplevelMenuEvents(widget, event); 
      break;
    case ButtonRelease:
      if( EZ_CButtonOn(widget) )
	{
	  EZ_CButtonSetOffValue(widget);
	}
      else
	{
	  EZ_CButtonSetOnValue(widget);
	}
      EZ_DrawWidget(widget);
      if(EZ_CurrentPopupMenu != (EZ_Widget *)NULL)
	{
	  EZ_WidgetRetData(EZ_CurrentPopupMenu) = EZ_WidgetRetData(widget);
	  EZ_PopupLSItem(EZ_CurrentPopupMenu) = widget;  /* this is the selected item     */
	}
      break;

    case EnterNotify:
    case FocusIn:
      EZ_WidgetBorderStyle(widget) = EZ_BORDER_UP;
      EZ_SetWidgetHighlightFlag(widget);
      { EZ_InstallHighlightColor(widget); } /*4/29/99*/
      EZ_HideSubMenuOf(EZ_WidgetParent(widget));
      EZ_DrawWidget(widget);
      break;
    case LeaveNotify:
      if(event->xcrossing.mode == NotifyGrab  ||
	 event->xcrossing.mode == NotifyUngrab)
	break;
    case FocusOut:
      EZ_ClearWidgetHighlightFlag(widget);
      { EZ_UninstallHighlightColor(widget); } /*4/29/99*/
      EZ_WidgetBorderStyle(widget) = EZ_BORDER_NONE;
      EZ_DrawWidget(widget);      
      break;
    case KeyPress:
      if(EZ_CurrentPopupMenu == (EZ_Widget *)NULL)
	EZ_HandleToplevelMenuEvents(widget, event); 
      break;
    default:
      EZ_WidgetBorderStyle(widget) = EZ_BORDER_NONE;
      break;
    }
}


/*********************************************************************
 ***                                                               ***
 ***                    Menu item:  radio button                   ***
 ***                                                               ***
 *********************************************************************/

EZ_Widget  *EZ_CreateMenuRadioButton(parent, label, underline, gid, value, ret_value)
     EZ_Widget  *parent;     /* parent widget    */
     char             *label;
     int              value, gid;
     int              ret_value, underline;
{
  EZ_Widget *wptr = EZ_CreateNewWidget(parent);
  EZ_WidgetHandlingFunctions[EZ_WIDGET_MENU_RADIO_BUTTON] = &EZ_MenuRadioButtonHandle;
  EZ_SetWidgetTypeAndNames(wptr,EZ_WIDGET_MENU_RADIO_BUTTON);

  EZ_RButtonLabel(wptr) = EZ_AllocCopyString(label);
  EZ_RButtonLabelLength(wptr) = (label == (char *)NULL? 1 : strlen(label));
  EZ_RButtonPixmap(wptr) = (EZ_Bitmap *)NULL;
  EZ_RButtonFont(wptr) = EZ_GetFontFromId(EZ_BUTTON_FONT);
  EZ_WidgetBorderStyle(wptr) = EZ_BORDER_NONE;
  EZ_RButtonTWidth(wptr)     = 0;
  EZ_RButtonTHeight(wptr)    = 0;
  EZ_RButtonFontAscent(wptr) = 0;
  EZ_WidgetLabelPosition(wptr) = EZ_LEFT_2_RIGHT;
  EZ_WidgetPadY(wptr) = 0;
  EZ_WidgetPadB(wptr) = 0;
  EZ_WidgetRetData(wptr) = ret_value;
  EZ_RButtonCheckSizeAdjust(wptr) = 3; 

  EZ_RButtonLineLength(wptr) = EZ_DEFAULT_LABEL_LINE_LENGTH;
  EZ_RButtonNLines(wptr)  = 1;
  EZ_RButtonCheckBorderWidth(wptr) = 2;
  EZ_RButtonCheckBorderStyle(wptr) = EZ_BORDER_UP;
  EZ_RButtonCheckType(wptr) = EZ_DIAMOND_INDICATOR;
  EZ_RButtonCheckColor(wptr)= 0;

  EZ_RButtonLabel2(wptr)  = (char *)NULL;
  EZ_RButtonLabel2Length(wptr) = 0;
  EZ_RButtonLabel2Width(wptr) = 0;

  EZ_WidgetSepX(wptr) = EZ_WIDGET_DEFAULT_PADX;
  EZ_WidgetSepY(wptr) = EZ_WIDGET_DEFAULT_PADY;


  EZ_SetWidgetFocusableFlag(wptr);

  EZ_RButtonGroup(wptr) = NULL;
  EZ_RButtonVarPtr(wptr) = NULL;

  EZ_RButtonVarValue(wptr) = value;
  EZ_FixRadioButtonList(wptr,gid); 

  if(label && underline >= 0 && underline < EZ_ButtonLabelLength(wptr) && isprint(label[underline]))
    {
      EZ_RButtonUnderLine(wptr) = underline;
      EZ_RButtonModifiers(wptr) = Mod1Mask;
      EZ_RButtonShortcut(wptr)  = (EZ_RButtonLabel(wptr) + underline);
      /*-------------------------------------------------------------
       *  Register this shortcut in the globale KeyPress event table
       *------------------------------------------------------------*/
      EZ_InsertGlobalKeyPressEvent(wptr, Mod1Mask, (label+underline));
    }
  else
    {
      EZ_RButtonUnderLine(wptr) = -1;
      EZ_RButtonModifiers(wptr) = 0;
      EZ_RButtonShortcut(wptr)  = (char *)NULL;
    }
  EZ_RButtonCopy(wptr) = NULL;
  return(wptr);
}

/*
 *   event handle
 */
void EZ_MenuRadioButtonEventHandle(widget, event)
     EZ_Widget *widget;
     XEvent          *event;
{
  if(widget == (EZ_Widget *)NULL) return;
  
  if(EZ_GetWidgetDisabledFlag(widget))
    {
      if(event->type == Expose)	EZ_DrawWidget(widget);      
      else if(event->type == EnterNotify)
	EZ_HideSubMenuOf(EZ_WidgetParent(widget));
      else  if(event->type == KeyPress || (event->type == ButtonPress &&
					   event->xbutton.button == EZ_Btn1))
	EZ_HandleToplevelMenuEvents(widget, event); 
      return;
    }

  switch(event->type)
    {
    case Expose:
	EZ_DrawWidget(widget);      
      break;
    case ButtonPress:
      if(EZ_CurrentPopupMenu == (EZ_Widget *)NULL)
	EZ_HandleToplevelMenuEvents(widget, event); 
      break;
    case ButtonRelease:      
      if( !( EZ_RButtonOn(widget)) )
	{
	  EZ_RButtonList  *friends;
	  EZ_Widget *old = (EZ_Widget *)NULL;

	  friends = EZ_RButtonGroup(widget)->list;
	  while(friends)
	    {
	      if( EZ_RButtonOn( friends->rbutton) )
		{
		  old = friends->rbutton;  /* currently checked  */
		  break;
		}
	      friends = friends->next;
	    }
	  EZ_RButtonSetValue(widget);
	  EZ_DrawWidget(widget);
	  if(old && EZ_WidgetMapped(old)) EZ_DrawWidget(old);
	}
      if(EZ_CurrentPopupMenu != (EZ_Widget *)NULL)
	{
	  EZ_WidgetRetData(EZ_CurrentPopupMenu) = EZ_WidgetRetData(widget);
	  EZ_PopupLSItem(EZ_CurrentPopupMenu) = widget;  /* this is the selected item     */
	}
      break;
    case EnterNotify:
    case FocusIn:
      EZ_WidgetBorderStyle(widget) = EZ_BORDER_UP;
      EZ_SetWidgetHighlightFlag(widget);
      { EZ_InstallHighlightColor(widget); } /*4/29/99*/
      EZ_HideSubMenuOf(EZ_WidgetParent(widget));
      EZ_DrawWidget(widget);
      break;
    case LeaveNotify:
      if(event->xcrossing.mode == NotifyGrab  ||
	 event->xcrossing.mode == NotifyUngrab)
	break;
    case FocusOut:
      EZ_ClearWidgetHighlightFlag(widget);
      { EZ_UninstallHighlightColor(widget); } /*4/29/99*/
      EZ_WidgetBorderStyle(widget) = EZ_BORDER_NONE;
      EZ_DrawWidget(widget);      
      break;
    case KeyPress:
      if(EZ_CurrentPopupMenu == (EZ_Widget *)NULL)
	EZ_HandleToplevelMenuEvents(widget, event); 
      break;
    default:
      EZ_WidgetBorderStyle(widget) = EZ_BORDER_NONE;
      break;
    }
}
/**************************************************************/
EZ_Widget *EZ_GetMenuItemCompaion(widget)
     EZ_Widget *widget;
{
  if(widget)
    {
      int WType = EZ_WidgetType(widget);
      EZ_Widget *clone;
      
      switch(WType)
	{
	case EZ_WIDGET_MENU_SEPARATOR:
	    clone = EZ_MenuSeparatorCopy(widget);
	      break;
	case EZ_WIDGET_MENU_SUBMENU:
	    clone = EZ_SubMenuCopy(widget);
	      break;
	case EZ_WIDGET_MENU_NORMAL_BUTTON:
	    clone = EZ_ButtonCopy(widget);
	      break;
	case EZ_WIDGET_MENU_CHECK_BUTTON:
	    clone = EZ_CButtonCopy(widget);
	      break;
	case EZ_WIDGET_MENU_RADIO_BUTTON:
	    clone = EZ_RButtonCopy(widget);
	      break;
	default:
	    clone = NULL;
	      break;
	}
      return(clone);
    }
  return(NULL);
}
/**************************************************************************************/
#undef _EZ_WIDGET_MENU_ITEMS_C_










