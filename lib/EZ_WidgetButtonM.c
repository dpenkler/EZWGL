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
 ***              Widget Menu Button                           ***
 ***                                                           ***
 *****************************************************************/
#define _EZ_WIDGET_BUTTONM_C_

#include "EZ_Widget.h"

/*********************************************************************
 * 
 *  Functions implemented in this file:
 */
EZ_Widget  *EZ_CreateMenuButton MY_ANSIARGS((EZ_Widget *parent, char *label, int undline));
void              EZ_DrawWidgetMenuButton MY_ANSIARGS((EZ_Widget *widget));
void              EZ_ComputeWidgetMButtonSize MY_ANSIARGS((EZ_Widget *widget, int *w, int *h));
void              EZ_FreeWidgetMButtonData MY_ANSIARGS((EZ_Widget *widget));
void              EZ_MButtonEventHandle MY_ANSIARGS((EZ_Widget *widget, XEvent *event));

void              EZ_SetMenuButtonMenu MY_ANSIARGS((EZ_Widget *widget, EZ_Widget *menu));
EZ_Widget         *EZ_GetMenuButtonMenu MY_ANSIARGS((EZ_Widget *widget));
int               EZ_LookForPopupShortCuts MY_ANSIARGS((EZ_Widget *widget, int modfiers, 
							char *str, int *x, int *y, 
							EZ_Widget **item));
int               EZ_MatchCharIgnoreCase MY_ANSIARGS((char c1, char c2));

int               EZ_MenuItemReachable MY_ANSIARGS((EZ_Widget *menu, EZ_Widget *item));
EZ_Widget         *EZ_FirstMenuItem MY_ANSIARGS((EZ_Widget *widget));

static EZ_Widget         *FindAdjacentMBtn MY_ANSIARGS((EZ_Widget *widget, int dir));
/*********************************************************************
 * 
 *  Local Variables.
 */
static EZ_WidgetHandle EZ_MButtonHandle =
{ 
  EZ_ComputeWidgetMButtonSize,
  EZ_DrawWidgetMenuButton,
  EZ_FreeWidgetMButtonData,
  EZ_MButtonEventHandle,
};


/*********************************************************************/

EZ_Widget  *EZ_CreateMenuButton(parent,label,underline)
     EZ_Widget  *parent;     /* parent widget    */
     char             *label;
     int              underline;
{
  EZ_Widget  *wptr;

  wptr = EZ_CreateNewWidget(parent);
  /*--------------------------------------------------
   * Register the handling functions for MButton.
   *  has to be done after  EZ_CreateNewWidget.
   *-------------------------------------------------*/
  EZ_WidgetHandlingFunctions[EZ_WIDGET_MENU_BUTTON] = &EZ_MButtonHandle;

  EZ_SetWidgetTypeAndNames(wptr,  EZ_WIDGET_MENU_BUTTON);
  EZ_WidgetSepX(wptr) = EZ_WIDGET_DEFAULT_PADX;
  EZ_WidgetSepY(wptr) = EZ_WIDGET_DEFAULT_PADY;
  EZ_WidgetBorderWidth(wptr) = 2;
  EZ_WidgetBorderStyle(wptr)  = EZ_BORDER_NONE;
  EZ_WidgetPadB(wptr) = 2;

  EZ_MButtonLabel(wptr) = EZ_AllocCopyString(label);
  EZ_MButtonLabelLength(wptr) = (label == (char *)NULL? 1 : strlen(label));
  EZ_MButtonPixmap(wptr) = (EZ_Bitmap *)NULL;
  EZ_MButtonFont(wptr) = EZ_GetFontFromId(EZ_BUTTON_FONT);
  EZ_MButtonTWidth(wptr)     = 0;
  EZ_MButtonTHeight(wptr)    = 0;
  EZ_MButtonFontAscent(wptr) = 0;
  EZ_MButtonLineLength(wptr) = EZ_DEFAULT_LABEL_LINE_LENGTH;
  EZ_MButtonNLines(wptr)  = 1;

  EZ_MButtonLabel2(wptr)  = (char *)NULL;
  EZ_MButtonLabel2Length(wptr) = 0;
  EZ_MButtonLabel2Width(wptr) = 0;

  EZ_MButtonCheckType(wptr) = 0;
  EZ_MButtonCheckColor(wptr) = 0L;
  EZ_MButtonCheckBorderWidth(wptr) = 2;
  EZ_MButtonCheckBorderStyle(wptr)  = EZ_BORDER_UP;
  EZ_MButtonCheckSizeAdjust(wptr) = 0;

  EZ_SetWidgetFocusableFlag(wptr);

  if(label && underline >= 0 && underline < EZ_MButtonLabelLength(wptr) &&
     isprint(label[underline]))
    {
      EZ_MButtonUnderLine(wptr) = underline;
      EZ_MButtonModifiers(wptr) = Mod1Mask;
      EZ_MButtonShortcut(wptr)  = (EZ_MButtonLabel(wptr) + underline);
      /*-------------------------------------------------------------
       *  Register this shortcut in the globale KeyPress event table
       *------------------------------------------------------------*/
      EZ_InsertGlobalKeyPressEvent(wptr, Mod1Mask, (label+underline));
    }
  else
    {
      EZ_MButtonUnderLine(wptr) = -1;
      EZ_MButtonModifiers(wptr) = 0;
      EZ_MButtonShortcut(wptr)  = (char *)NULL;
    }
  EZ_MButtonTheMenu(wptr) = (EZ_Widget *)NULL;
  EZ_MButtonLSItem(wptr)  = (EZ_Widget *)NULL;
  return(wptr);
}

/********************************************************************
 *
 *  Figure out the dimension of a menu button
 */
void EZ_ComputeWidgetMButtonSize(widget, w, h)
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
      EZ_Bitmap *bitmap = EZ_MButtonPixmap(widget);

      if(!bitmap || EZ_PTLabel)
        {
          EZ_FigureLabelExtent(EZ_MButtonFont(widget),
                               EZ_MButtonLabel(widget),
                               (int)EZ_MButtonLabelLength(widget),
                               (int)EZ_MButtonLineLength(widget),
                               &nlines, &twidth);
          EZ_MButtonTWidth(widget) = twidth;
          EZ_MButtonNLines(widget) = nlines;
	  
          EZ_MButtonTHeight(widget) = EZ_MButtonFont(widget)->ascent +
            EZ_MButtonFont(widget)->descent;
          EZ_MButtonFontAscent(widget) = EZ_MButtonFont(widget)->ascent;
	  
          if(EZ_MButtonLabel2Length(widget) != 0)
            {
              EZ_MButtonLabel2Width(widget) = EZ_LABEL_SEPARATION +
                XTextWidth(EZ_MButtonFont(widget), 
                           EZ_MButtonLabel2(widget),
                           EZ_MButtonLabel2Length(widget));
              EZ_WidgetAuxXRight(widget) = EZ_MButtonLabel2Width(widget);
            }
          else
            {
              EZ_MButtonLabel2Width(widget) = 0;
              EZ_WidgetAuxXRight(widget) = 0;
            }
        }
      else
        {
          EZ_MButtonTWidth(widget) =0;
          EZ_MButtonNLines(widget) =0;
          EZ_MButtonTHeight(widget) =0;
          EZ_MButtonFontAscent(widget) =0;
          EZ_MButtonLabel2Width(widget) = 0;
          EZ_WidgetAuxXRight(widget) = 0;
        }
      if(bitmap)
	{
          EZ_MButtonMWidth(widget) = bitmap ->width;
          EZ_MButtonMHeight(widget) = bitmap ->height;
        }
      else
        {
          EZ_MButtonMWidth(widget) = 0;
          EZ_MButtonMHeight(widget) = 0;
        }
      EZ_SetWidgetSizeComputedFlag(widget);
    }

  length = EZ_MButtonTWidth(widget) + EZ_MButtonLabel2Width(widget);
  height = EZ_MButtonTHeight(widget) * EZ_MButtonNLines(widget);
  pwidth = EZ_MButtonMWidth(widget);
  pheight = EZ_MButtonMHeight(widget);
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
  ch = ch + ch + hh;
  if(EZ_MButtonCheckType(widget) != EZ_EMPTY_INDICATOR)
    EZ_WidgetAuxXLeft(widget) = (ch << 1)/5  + 6 + EZ_MButtonCheckSizeAdjust(widget); 
  else EZ_WidgetAuxXLeft(widget) = 0;

  cw = cw + cw + ww +  EZ_WidgetAuxXLeft(widget);

  *w = cw;
  *h = ch;
}

/******************************************************************************
 *
 *  Free allocated data for a menu button.
 */

void EZ_FreeWidgetMButtonData(mbtn)
     EZ_Widget *mbtn;
{
  if(mbtn == (EZ_Widget *)NULL) return;
  if(EZ_MButtonLabel(mbtn) != (char *)NULL)
    (void)my_free((char *)EZ_MButtonLabel(mbtn));        /* free label string */
  if(EZ_MButtonLabel2(mbtn) != (char *)NULL)
    (void)my_free((char *)EZ_MButtonLabel2(mbtn));        /* free label string */
  if(EZ_MButtonPixmap(mbtn) != (EZ_Bitmap *)NULL)   /* free pixmap       */
    EZ_FreeBitmap(EZ_MButtonPixmap(mbtn));
}

/******************************************************************************
 *
 *  Draw a menu button
 */

void  EZ_DrawWidgetMenuButton(wptr)
     EZ_Widget *wptr;
{
  int             w, h,x,y, xx, yy, fillx, filly, padb, padb2, tile;
  Pixmap          pixmap;
  Window          win;
  GC              gc;
  int             offset, offseta =0, tileOx=0,tileOy=0;
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
   *  copy to the button window when finished.
   *----------------------------------------------------------*/
  pixmap = XCreatePixmap(EZ_DisplayForWidgets, win, w, h, EZ_DepthForWidgets);    
  EZ_FixFocusHighlightBorder(wptr, pixmap, w, h, padb);

  if(EZ_OffsetEnabled && EZ_WidgetBorderStyle(wptr) == EZ_BORDER_DOWN)  offseta = 1;
  tile = EZ_GetBackgroundGC(wptr, &gc, &bgpv, 1,0);  
  if(tile)
    {
      EZ_GetWidgetTileOrigin(wptr, &tileOx, &tileOy);      
      tileOx += offseta; tileOy += offseta;
      XSetTSOrigin(EZ_Display, gc, tileOx, tileOy);
    }
  XFillRectangle(EZ_DisplayForWidgets, pixmap, gc, padb,padb, w-padb2, h-padb2); 

  /*--------------------------------------------------------
   *  Draw the label.
   *-------------------------------------------------------*/
  bitmap = EZ_RButtonPixmap(wptr); 
  xx = EZ_WidgetXOffset(wptr) + fillx;
  yy = EZ_WidgetYOffset(wptr) + filly;
  orien = EZ_WidgetOrientation(wptr);
  pwidth = EZ_MButtonMWidth(wptr);
  pheight = EZ_MButtonMHeight(wptr);
  twidth = EZ_MButtonTWidth(wptr);
  theight = EZ_MButtonTHeight(wptr) * EZ_MButtonNLines(wptr);

  offset = offseta + padb;
  x = xx + EZ_WidgetPadX(wptr) + EZ_WidgetBorderWidth(wptr) + EZ_WidgetAuxXLeft(wptr)+ offset;
  y = yy + EZ_MButtonFontAscent(wptr) + EZ_WidgetBorderWidth(wptr) + EZ_WidgetPadY(wptr) + offset;
  
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
                               - (int) (EZ_MButtonFontAscent(wptr)));
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
                               - (int) (EZ_MButtonFontAscent(wptr)));
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
                                     (int)EZ_MButtonTHeight(wptr), EZ_MButtonLabel(wptr),
                                     (int)EZ_MButtonLabelLength(wptr), (int)EZ_MButtonLineLength(wptr),
                                     (int)EZ_MButtonNLines(wptr),
                                     (int)EZ_MButtonTWidth(wptr),
                                     (int)EZ_WidgetJustification(wptr),
                                     EZ_MButtonFont(wptr));
        }
      else
        {
          int shadow = EZ_WidgetLabelShadow(wptr);
          if(EZ_MButtonForeground(wptr) != EZ_DefaultForeground || 
             EZ_MButtonFont(wptr) != EZ_GetFontFromId(EZ_BUTTON_FONT))
            {
              XSetFont(EZ_DisplayForWidgets, EZ_WRITABLEGC, EZ_MButtonFont(wptr)->fid);
              XSetForeground(EZ_DisplayForWidgets, EZ_WRITABLEGC, EZ_MButtonForeground(wptr));	
              gc = EZ_WRITABLEGC;
            }
          else gc = EZ_BUTTON_TEXTGC;
	  
          EZ_RenderLabelText(pixmap, gc, x, y, (int)EZ_MButtonTHeight(wptr), EZ_MButtonLabel(wptr),
                             (int)EZ_MButtonLabelLength(wptr), (int)EZ_MButtonLineLength(wptr),
                             (int)EZ_MButtonNLines(wptr),
                             (int)EZ_MButtonTWidth(wptr),
                             (int)EZ_WidgetJustification(wptr),
                             EZ_MButtonFont(wptr), shadow);
          
          /*-------------------------------------------------------------------------
           *  Keyboard shortcuts. If explicitly set, it take over the implicit one.
           *  (set by the underline character)
           *------------------------------------------------------------------------*/
          if(EZ_MButtonLabel2Length(wptr) != 0)
            {
              int tmpx, tmpy = ((h+EZ_MButtonFontAscent(wptr))>>1) + offseta;
              tmpx = EZ_GetLabel2XPosition(wptr,
                                           EZ_MButtonLabel2Width(wptr) -
                                           EZ_LABEL_SEPARATION, offseta);
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
                              EZ_MButtonLabel2(wptr),
                              EZ_MButtonLabel2Length(wptr));
                }
              XDrawString(EZ_DisplayForWidgets, 
                          pixmap,  gc,
                          tmpx, tmpy,
                          EZ_MButtonLabel2(wptr),
                          EZ_MButtonLabel2Length(wptr));
            }
          else if(EZ_MButtonUnderLine(wptr) >= 0 && 
                  EZ_MButtonUnderLine(wptr) < ((int)EZ_MButtonLineLength(wptr)>>1))
            {
              int tlen = XTextWidth(EZ_MButtonFont(wptr),EZ_MButtonLabel(wptr),
                                    EZ_MButtonUnderLine(wptr));
              int tlen1 = tlen + XTextWidth(EZ_MButtonFont(wptr),
                                            EZ_MButtonLabel(wptr)+EZ_MButtonUnderLine(wptr), 1);
              int base = y + EZ_MButtonTHeight(wptr) - EZ_MButtonFontAscent(wptr)-1;
              int boldfont;

              if(EZ_MButtonFont(wptr) != EZ_GetFontFromId(EZ_BUTTON_FONT))
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
  /*---------------------
   * draw the indicator
   *--------------------*/
  EZ_DrawIndicator(wptr, pixmap, 0,
		   (int) EZ_MButtonCheckType(wptr),
		   (int) EZ_MButtonCheckBorderWidth(wptr),
		   (int) EZ_MButtonCheckSizeAdjust(wptr),
		   EZ_MButtonCheckColor(wptr));

  /*============================================================
   * for complex button with  NW labels 
   */
  {
    EZ_Widget *children = EZ_WidgetChildren(wptr);
    if(children && EZ_WidgetType(children) == EZ_WIDGET_NW_LABEL)
      {
	EZ_DisplayNWLabel(children, pixmap, EZ_WidgetForeground(wptr), bgpv,
			  EZ_GetWidgetDisabledFlag(wptr), EZ_GetWidgetHighlightFlag(wptr),
			  0, 0, offseta, 1);
      }
  }
  /*============================================================*/  


  /*------------------------------------
   *  Now the real border.
   *-----------------------------------*/
  EZ_DrawRectBorder(wptr, pixmap);
  XCopyArea(EZ_DisplayForWidgets,pixmap,win, EZ_WRITABLEGC,0,0,w,h,0,0); 
  XFreePixmap(EZ_DisplayForWidgets, pixmap); 
}

/*************************************************************************
 *
 *  Event handling, tricky!
 */

void  EZ_MButtonEventHandle(widget, event)
     EZ_Widget *widget;
     XEvent          *event;
{
  EZ_Widget *mmenu;

  if(widget == (EZ_Widget *)NULL) return;

  if(event->type == Expose)   EZ_DrawWidget(widget);      
  if( EZ_GetWidgetDisabledFlag(widget) ) return;

  switch(event->type)
    {
    case ButtonPress:
      if(event->xbutton.button == EZ_Btn1)      
	{
	  if(EZ_CurrentPopupMenu != (EZ_Widget *)NULL)
	    { return;}
	  /*-----------------------------------------------------
	   * Now we do a popup. Grab the pointer and wait for
	   * a selection.
	   *----------------------------------------------------*/
	startagain:
	  EZ_MButtonLSItem(widget) = (EZ_Widget *)NULL;
	  
	  mmenu = EZ_MButtonTheMenu(widget);
	  if( mmenu  && EZ_WidgetNumChildren(mmenu) == 0) mmenu = NULL;
	  if(mmenu == NULL) return;

	  if(1)
	    { 
	      int          srx,sry,rx,ry,x,y, itmp;
	      unsigned int mask;
	      Window       root,win;
	      
	      /* first set the background ptr */
	      if(mmenu) EZ_PropagateBgInfo2Widget(widget, mmenu);
              
	      /*-----------------------------------------------------------
	       * find out the position to popup the menu and do the popup.
	       *----------------------------------------------------------*/
	      if(mmenu)
		{
		  XQueryPointer(EZ_Display, EZ_WidgetWindow(widget),
				&root,                                    /* root return win */
				&win,                                     /* child ret win   */
				&rx, &ry,                                 /* x, y in root    */
				&x,&y,                                    /* x, y in win     */
				&mask
				);
		  itmp = (EZ_WidgetHeight(widget) -y);
		  srx = rx = rx - x;
		  sry = ry = ry + itmp;
		  {
		    int ww, hh;
		    if( !(EZ_GetWidgetSizeComputedFlag(mmenu)) )
		      {
			EZ_MarkAllChildrenWidgetSizeChanged(mmenu,0);
			EZ_ComputeWidgetWindowSize(mmenu, &ww, &hh, NULL);
		      }
		    else
		      {
			ww = EZ_WidgetWidth(mmenu);
			hh = EZ_WidgetHeight(mmenu);
		      }
		    if(rx < 0) rx = 0;
		    else if(rx + ww > EZ_XDisplayWidth)  rx = (int)EZ_XDisplayWidth - ww;
		    if(ry < 0) ry = 0;
		    else if(ry + hh > EZ_XDisplayHeight) ry = (int)EZ_XDisplayHeight - hh;
		  }
		  /*EZ_WidgetBorderStyle(widget) = EZ_BORDER_UP;*/
		  EZ_SetWidgetHighlightFlag(widget);
                  { EZ_InstallHighlightColor(widget); } /*4/29/99*/
		  EZ_DrawWidget(widget);
                  EZ_PopupInitiator(mmenu) = widget;
                  EZ_PopupXoff(mmenu) = rx - srx;
                  EZ_PopupYoff(mmenu) = EZ_WidgetHeight(widget) + ry -sry;
		  EZ_SetupPopup(mmenu);
		  EZ_DisplayPopup(mmenu, rx, ry);
		}
	      /*
	       * Now wait until a selection is made.
	       */
	      {
		XEvent          xevent;
		Window          event_window;
		EZ_Widget *tmp, *cmenu, *the_popup = EZ_CurrentPopupMenu;
		EZ_Widget *shortcutItem, *CurrentItem;
		EZ_Widget *out, *sitem;
		int             done = 0;
		

		XGrabPointer(EZ_Display,
			     EZ_WidgetWindow(the_popup),
			     True,
			     ButtonPressMask|ButtonReleaseMask|ButtonMotionMask|PointerMotionMask,
			     GrabModeAsync,
			     GrabModeAsync, 
			     None,
			     EZ_GetCursor(EZ_C_RIGHT_PTR),
			     CurrentTime);
	      
		/*
		 * fvwm eats some keystrokes, so...
		 */

		XGrabKeyboard(EZ_Display,
			      EZ_WidgetWindow(the_popup),
			      True,
			      GrabModeAsync,
			      GrabModeAsync, 
			      CurrentTime);
		
		cmenu = the_popup;
		/* if the menu is poped via key press', select the first item */
		if(event->xbutton.send_event)
		  {
		    CurrentItem = EZ_FirstMenuItem(cmenu);
		    event->xbutton.send_event = False;
		  }
		else CurrentItem = NULL;
		EZ_PopupLink2(cmenu) = CurrentItem;
		while(1)
		  {
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
			EZ_Widget         *tmpBB;
#undef TEMP_BUFFER_SIZE
			shortcutItem = NULL;
			sitem = NULL;
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
			      case XK_Up:  case XK_k:  case XK_p: case XK_K:  case XK_P:
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
				  EZ_Widget *tmpa = EZ_PopupLink1(cmenu);
				  if(tmpa)
				    {
				      cmenu = tmpa;
				      EZ_DeselectCurrentMenuItem(CurrentItem);
				      CurrentItem = EZ_MoveMenuSelection(cmenu, EZ_PopupLink2(cmenu), 0);
				    }
				  else  /* cycle to the next menu on the right */
				    {
				      tmpBB = FindAdjacentMBtn(widget, -1);
				      if(tmpBB && tmpBB != widget)
					{
                                          EZ_HideCurrentPopupMenuA();
                                          XUngrabPointer(EZ_Display, CurrentTime); 
                                          XUngrabKeyboard(EZ_Display,CurrentTime); 
					  EZ_ClearWidgetHighlightFlag(widget);
                                          { EZ_UninstallHighlightColor(widget); } /*4/29/99*/
					  /*EZ_WidgetBorderStyle(widget) = EZ_BORDER_NONE;*/
					  EZ_DrawWidget(widget);
					  event->type = ButtonPress;
					  event->xbutton.window = EZ_WidgetWindow(tmpBB);
					  event->xbutton.x = event->xbutton.y = 1;
					  widget = tmpBB;
					  goto startagain;
					}
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
				else /* cycle to the next menu on the right */
				  {
				    tmpBB = FindAdjacentMBtn(widget, 1);
				    if(tmpBB && tmpBB != widget)
				      {
                                        EZ_HideCurrentPopupMenuA();
                                        XUngrabPointer(EZ_Display, CurrentTime); 
                                        XUngrabKeyboard(EZ_Display,CurrentTime); 
					EZ_ClearWidgetHighlightFlag(widget);
                                        { EZ_UninstallHighlightColor(widget); } /*4/29/99*/
					/*EZ_WidgetBorderStyle(widget) = EZ_BORDER_NONE;*/
					EZ_DrawWidget(widget);
					event->type = ButtonPress;
					event->xbutton.window = EZ_WidgetWindow(tmpBB);
					event->xbutton.x = event->xbutton.y = 1;
					widget = tmpBB;
					goto startagain;
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
                              case XK_Escape:
                                sitem = NULL;
                                done = 1;    
                                break;
			      default:
				break;
			      }
			  }
			/*--------------------------------------------
			 * if a selection has been made, finish up
			 *-------------------------------------------*/
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
				    /*EZ_WidgetBorderStyle(sitem) = EZ_BORDER_UP;*/
				    EZ_DrawWidget(sitem);
				    
				    if(EZ_CurrentPopupMenu != (EZ_Widget *)NULL)
				      {  /* return value */
					EZ_WidgetRetData(EZ_CurrentPopupMenu) = EZ_WidgetRetData(sitem);
				 	/* this is the selected item     */
					EZ_PopupLSItem(EZ_CurrentPopupMenu) = sitem;  
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
			    xbevent.window = EZ_WidgetWindow(out);
			    xbevent.time = CurrentTime;
			    xbevent.x=0; xbevent.y=0;
			    XSendEvent(EZ_Display, EZ_WidgetWindow(out),
				       False, ButtonReleaseMask, (XEvent *)&xbevent);
			    EZ_HandleGlobalKeyEvent(out, 0, 0);
			    done = 1;
			  }
		      } /* KeyPress */
		    else
		      {
			/*---------------------------------------------------------
			 *  Non KeyPress Events.
			 *--------------------------------------------------------*/
			if(tmp == (EZ_Widget *)NULL) continue;
			/*---------------------------------------------------------
			 * Intercept all events on the current (active) MenuButton
			 *--------------------------------------------------------*/
			else if(tmp == widget)
			  {
			    if(xevent.type == EnterNotify)
			      {
				EZ_SetWidgetHighlightFlag(widget);
                                { EZ_InstallHighlightColor(widget); } /*4/29/99*/
				EZ_DrawWidget(widget);
			      }
			    else if(xevent.type == LeaveNotify && xevent.xcrossing.mode == NotifyNormal)
			      {
				EZ_ClearWidgetHighlightFlag(widget);
                                { EZ_UninstallHighlightColor(widget); } /*4/29/99*/
				EZ_DrawWidget(widget);
			      }
			    else if(xevent.type == ButtonRelease)   /* 'cause of the grab */
			      {
				int x = xevent.xbutton.x;
				int y = xevent.xbutton.y;
				if( x < 0 || x > EZ_WidgetWidth(tmp) ||
				    y < 0 || y > EZ_WidgetHeight(tmp)) done = 1;
			      }
			  }
			else  /* event happend on some other widgets */
			  {
			    if(xevent.type == Expose)
			      EZ_HandleWidgetWindowEvent(tmp, &xevent);
			    else if(xevent.type == EnterNotify)
			      {
				if(EZ_WidgetType(tmp) == EZ_WIDGET_MENU_BUTTON)
				  {
				    /*
				    EZ_SetWidgetHighlightFlag(tmp);
				    EZ_DrawWidget(tmp);
				    */
				    EZ_ClearWidgetHighlightFlag(widget);
                                    { EZ_UninstallHighlightColor(widget); } /*4/29/99*/
				    /*EZ_WidgetBorderStyle(widget) = EZ_BORDER_NONE;*/
				    EZ_DrawWidget(widget);

				    /* if we get here, we've entered into another
				     * menu-button. Just start over again.
				     */
				    if(xevent.xcrossing.mode != NotifyGrab  &&
				       xevent.xcrossing.mode != NotifyUngrab)
				      {
					EZ_HideCurrentPopupMenuA();
					XUngrabPointer(EZ_Display, CurrentTime); 
					XUngrabKeyboard(EZ_Display,CurrentTime); 
					widget = tmp;
					event->xbutton.window = EZ_WidgetWindow(tmp);
					goto startagain;
				      }
				  }
				else if(EZ_GetWidgetIsCloneFlag(tmp) == 0)
				  {
				    int type = EZ_WidgetType(tmp);
				    if(type == EZ_WIDGET_MENU_TEAR_OFF_BAR ||
				       (type >= EZ_WIDGET_MENU_SEPARATOR &&
					type <= EZ_WIDGET_MENU_RADIO_BUTTON))
				      {
					if(xevent.xcrossing.mode != NotifyGrab  &&
					   xevent.xcrossing.mode != NotifyUngrab)
					  {
					    EZ_DeselectCurrentMenuItem(CurrentItem);
					    EZ_HandleWidgetWindowEvent(tmp, &xevent);
					    CurrentItem = tmp;
					  }
				      }
				  }
			      }
			    else if(xevent.type == LeaveNotify)
			      {
				if(EZ_WidgetType(tmp) == EZ_WIDGET_MENU_BUTTON)
				  {
				    EZ_ClearWidgetHighlightFlag(tmp);
                                    { EZ_UninstallHighlightColor(tmp); } /*4/29/99*/
				    EZ_DrawWidget(tmp);
				  }
				else if(EZ_GetWidgetIsCloneFlag(tmp) == 0)
				  {
				    int type = EZ_WidgetType(tmp);
				    if(type == EZ_WIDGET_MENU_TEAR_OFF_BAR ||
				       (type >= EZ_WIDGET_MENU_SEPARATOR &&
					type <= EZ_WIDGET_MENU_RADIO_BUTTON))
				      {
					if(xevent.xcrossing.mode != NotifyGrab  &&
					   xevent.xcrossing.mode != NotifyUngrab)
					  {
					    EZ_DeselectCurrentMenuItem(CurrentItem);
					    CurrentItem = (EZ_Widget *)NULL;
					    EZ_HandleWidgetWindowEvent(tmp, &xevent);
					  }
				      }
				  }
			      }
			    else if(xevent.type == FocusIn || xevent.type == FocusOut)
			      EZ_HandleWidgetWindowEvent(tmp, &xevent);
			    else if(xevent.type == ButtonPress || xevent.type == ButtonRelease)
			      {
				if(xevent.type == ButtonRelease &&
				   (EZ_WidgetType(tmp) != EZ_WIDGET_MENU_SUBMENU)) done = 1;
				if(EZ_GetWidgetIsCloneFlag(tmp) == 0)
				  {
				    int type = EZ_WidgetType(tmp);
				    if(type == EZ_WIDGET_MENU_TEAR_OFF_BAR ||
				       (type >= EZ_WIDGET_MENU_SEPARATOR &&
					type <= EZ_WIDGET_MENU_RADIO_BUTTON))
				      EZ_HandleWidgetWindowEvent(tmp, &xevent);
				  }
			      }
			  }
		      } 
		    if(done) break;
		  }
		EZ_HideCurrentPopupMenu();
		XUngrabPointer(EZ_Display, CurrentTime); 
		XUngrabKeyboard(EZ_Display,CurrentTime); 
		/*------------------------------------------------------
		 * Callbacks. 
		 *  handle popup menu callback first, and
		 *  handle menu-buttoncallback the second.
		 *-----------------------------------------------------*/
		if(the_popup)
		  {
		    EZ_Widget *lsitem = EZ_PopupLSItem(the_popup);
		    if(lsitem)
		      {
			EZ_ClearWidgetHighlightFlag(lsitem);
                        { EZ_UninstallHighlightColor(lsitem); } /*4/29/99*/
			/*EZ_WidgetBorderStyle(lsitem) = EZ_BORDER_NONE;*/

			EZ_HandlePopupCallBack(the_popup); /* popup callback */
			if(EZ_WidgetCallBackFunc(widget) != NULL)
			  {
			    if(EZ_MenuItemReachable(the_popup, lsitem))
			      {
				/* return value from menu */
				EZ_WidgetRetData(widget) = EZ_WidgetRetData(the_popup); 
				/* the selection on menu  */
				EZ_MButtonLSItem(widget) = EZ_PopupLSItem(the_popup); 
				{ EZ_ExecuteWidgetCallBacks(widget);}
			      }
			  } 
		      }
		  }
	      }
	    }
	  EZ_ClearWidgetHighlightFlag(widget);
          { EZ_UninstallHighlightColor(widget); } /*4/29/99*/
	  /*EZ_WidgetBorderStyle(widget) = EZ_BORDER_NONE;*/
	  EZ_DrawWidget(widget);
	}
      else if(event->xbutton.button == EZ_Btn3)
	{
          /*
            #include "EZ_DnDHandler1.c"
            EZ_DnDEventHandler1(widget, event);
          */
          EZ_HandleDnDEvents(widget, event);
	}      
      break;
    case EnterNotify:
      EZ_SetWidgetHighlightFlag(widget);
      { EZ_InstallHighlightColor(widget); } /*4/29/99*/
      EZ_DrawWidget(widget);
      break;
    case LeaveNotify:
      EZ_ClearWidgetHighlightFlag(widget);
      { EZ_UninstallHighlightColor(widget); } /*4/29/99*/
      EZ_DrawWidget(widget);
      break;
    default:
      break;
    }
}

int EZ_MenuItemReachable(menu, wgt)
     EZ_Widget *menu, *wgt;
{
  if(menu && EZ_WidgetType(menu)== EZ_WIDGET_POPUP_MENU)
    {
      EZ_Widget *children = EZ_WidgetChildren(menu);
      while(children)
	{
	  if(children == wgt) return 1;
	  else if(EZ_WidgetType(children) == EZ_WIDGET_MENU_SUBMENU)
	    {
	      if(EZ_MenuItemReachable(EZ_SubMenuTheMenu(children), wgt))
		return(1);
	    }
	  children = EZ_WidgetSibling(children);
	}
    }
  return(0);
}
/************************************************************************************/

int EZ_LookForPopupShortCuts(popup,modifiers,str,x, y, item)
     EZ_Widget  *popup, **item;
     int              modifiers;
     char             *str;
     int              *x,*y;
{
  if(popup != (EZ_Widget *)NULL)
    {
      Window          tmpwin;
      EZ_Widget *tmp = EZ_WidgetChildren(popup);

      while(tmp != (EZ_Widget *)NULL)
	{
	  if(EZ_GetWidgetDisabledFlag(tmp) == 0 && EZ_GetWidgetNonActiveFlag(tmp) == 0)
	    {
	      switch(EZ_WidgetType(tmp))
		{
		case EZ_WIDGET_MENU_NORMAL_BUTTON:
		  if(EZ_ButtonModifiers(tmp) == modifiers &&
		     EZ_ButtonShortcut(tmp) != (char *)NULL &&
		     EZ_MatchCharIgnoreCase(*(EZ_ButtonShortcut(tmp)), *str))
		    {
		      XTranslateCoordinates(EZ_DisplayForWidgets, EZ_WidgetWindow(tmp),
					    RootWindow(EZ_DisplayForWidgets, EZ_ScreenNumForWidgets),
					    3,3,
					    x,y, &tmpwin);
		      *item = tmp;
		      return(1);
		    }
		  break;
		case EZ_WIDGET_MENU_CHECK_BUTTON:
		  if(EZ_CButtonModifiers(tmp) == modifiers &&
		     EZ_CButtonShortcut(tmp) != (char *)NULL &&
		     EZ_MatchCharIgnoreCase(*(EZ_CButtonShortcut(tmp)), *str))
		    {
		      XTranslateCoordinates(EZ_DisplayForWidgets, EZ_WidgetWindow(tmp),
					    RootWindow(EZ_DisplayForWidgets, EZ_ScreenNumForWidgets),
					    2,2,
					    x,y, &tmpwin);
		      *item = tmp;
		      return(1);
		    }
		  break;
		case EZ_WIDGET_MENU_RADIO_BUTTON:
		  if(EZ_RButtonModifiers(tmp) == modifiers &&
		     EZ_RButtonShortcut(tmp) != (char *)NULL &&
		     EZ_MatchCharIgnoreCase(*(EZ_RButtonShortcut(tmp)), *str))
		    {
		      XTranslateCoordinates(EZ_DisplayForWidgets, EZ_WidgetWindow(tmp),
					    RootWindow(EZ_DisplayForWidgets, EZ_ScreenNumForWidgets),
					    2,2,
					    x,y, &tmpwin);
		      *item = tmp;
		      return(1);
		    }
		  break;
		case EZ_WIDGET_MENU_SUBMENU:
		  {
		    int              ans;
		    EZ_Widget  *the_menu = EZ_SubMenuTheMenu(tmp);
		    if(the_menu != (EZ_Widget *)NULL && (EZ_WidgetMapped(the_menu) != 0))
		      {
			ans = EZ_LookForPopupShortCuts(the_menu,modifiers,str,x, y, item);
			if(ans != 0) return(1);
		      }
		    /* else */  /*  !!! */
		      {
			if(EZ_SubMenuModifiers(tmp) == modifiers &&
			   EZ_SubMenuShortcut(tmp) != (char *)NULL &&
			   EZ_MatchCharIgnoreCase(*(EZ_SubMenuShortcut(tmp)), *str))
			  {
			    XTranslateCoordinates(EZ_DisplayForWidgets, EZ_WidgetWindow(tmp),
						  RootWindow(EZ_DisplayForWidgets, EZ_ScreenNumForWidgets),
						  3,3,
						  x,y, &tmpwin);
			    *item = tmp;
			    return(1);
			  }
		      }
		  }
		  break;
		default:
		  break;
		}
	    }
	  tmp = EZ_WidgetSibling(tmp);
	}
    }
  return(0);
}

/************************************************************************************
 *
 *  Set the pull down menu for a menubutton.
 */

void  EZ_SetMenuButtonMenu(mbutton,menu)
     EZ_Widget *mbutton, *menu;
{
  if(mbutton == (EZ_Widget *)NULL || menu == (EZ_Widget *)NULL ||
     EZ_WidgetType(mbutton) != EZ_WIDGET_MENU_BUTTON ||
     EZ_WidgetType(menu) != EZ_WIDGET_POPUP_MENU)
    {
      (void)fprintf(stderr,"In EZ_SetMenuButtonMenu(), wrong type of widgets\n");
      return;
    }
  EZ_MButtonTheMenu(mbutton) = menu;
}

/************************************************************************************
 *
 *  return True if c1 == c2, ignore case
 */

int EZ_MatchCharIgnoreCase(c1,c2)
     char c1,c2;
{
  if(isupper(c1)) c1 = tolower(c1);
  if(isupper(c2)) c2 = tolower(c2);
  return( (c1 == c2));
}

/************************************************************************************/
EZ_Widget *EZ_FirstMenuItem(menu)
     EZ_Widget *menu;
{
  EZ_Widget *children;

  if(menu == NULL) return(NULL);
  children = EZ_WidgetChildren(menu);
  while(children)
    {
      int type = EZ_WidgetType(children);
      if(type == EZ_WIDGET_LABEL || type == EZ_WIDGET_MENU_TEAR_OFF_BAR ||
	 type == EZ_WIDGET_MENU_SEPARATOR || EZ_GetWidgetDisabledFlag(children) ||
	 EZ_GetWidgetNonActiveFlag(children))
	children = EZ_WidgetSibling(children);
      else break;
    }
  if(children && EZ_WidgetMapped(children))
    {
      EZ_WidgetBorderStyle(children) = EZ_BORDER_UP;
      EZ_SetWidgetHighlightFlag(children);
      { EZ_InstallHighlightColor(children); } /*4/29/99*/
      EZ_HideSubMenuOf(EZ_WidgetParent(children));
      EZ_DrawWidget(children);
    }
  return(children);
}
/************************************************************************************/
EZ_Widget *EZ_GetMenuButtonMenu(widget) EZ_Widget *widget;
{
  if(widget && EZ_WidgetType(widget) == EZ_WIDGET_MENU_BUTTON)
    return( EZ_MButtonTheMenu(widget));
  return(NULL);
}
/************************************************************************************/
static EZ_Widget *FindAdjacentMBtn(widget, dir)
     EZ_Widget *widget; int dir;
{
  EZ_Widget *parent;
  if(widget && (parent = EZ_WidgetParent(widget)) != NULL)
    {
      if(dir == 1) /* move right */
	{
	  EZ_Widget *next = EZ_WidgetSibling(widget);
	  while(next != widget)
	    {
	      if(next == NULL) next = EZ_WidgetChildren(parent);
	      if(EZ_WidgetType(next) == EZ_WIDGET_MENU_BUTTON &&
		 EZ_MButtonTheMenu(next) != NULL)
		return(next);
	      else next = EZ_WidgetSibling(next);
	    }
	  return(widget);
	}
      else
	{
	  EZ_Widget *widgets[64];
	  int idx=0, cnt = 0;
	  EZ_Widget *next = EZ_WidgetChildren(parent);
	  while(next)
	    {
	      if(EZ_WidgetType(next) == EZ_WIDGET_MENU_BUTTON &&
		 EZ_MButtonTheMenu(next) != NULL)	      
		{ 
		  if(next == widget) idx = cnt;
		  widgets[cnt++] = next;
		}
	      next = EZ_WidgetSibling(next);
	    }
	  if(idx > 0) return(widgets[idx-1]);
	  else if(cnt > 0) return(widgets[cnt-1]);
	  else return(widget);
	}
    }
  return(NULL);
}
/************************************************************************************/
#undef _EZ_WIDGET_BUTTONM_C_
