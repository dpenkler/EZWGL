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
 ***              Widget Check Button                          ***
 ***                                                           ***
 *****************************************************************/
#define _EZ_WIDGET_BUTTONC_C_

#include "EZ_Widget.h"

/*********************************************************************
 * 
 *  Functions implemented in this file:
 */
EZ_Widget        *EZ_CreateCheckButton MY_ANSIARGS((EZ_Widget *parent,
						    char *label,
						    int underline,
						    int onvalue, int offvalue, int onff
						    ));
void             EZ_DrawWidgetCheckButton MY_ANSIARGS((EZ_Widget *widget));
void             EZ_ComputeWidgetCButtonSize MY_ANSIARGS((EZ_Widget *widget, int *w, int *h));
void             EZ_FreeWidgetCButtonData MY_ANSIARGS((EZ_Widget *widget));
void             EZ_CButtonEventHandle MY_ANSIARGS((EZ_Widget *widget, XEvent *event));
void             EZ_SetCheckButtonState MY_ANSIARGS((EZ_Widget *widget, int onoff));
int              EZ_GetCheckButtonState MY_ANSIARGS((EZ_Widget *widget, int *value));
/*********************************************************************
 * 
 *  Local Variables.
 */
static EZ_WidgetHandle EZ_CButtonHandle =
{
  EZ_ComputeWidgetCButtonSize,    
  EZ_DrawWidgetCheckButton,
  EZ_FreeWidgetCButtonData,
  EZ_CButtonEventHandle,
};

/*********************************************************************/

EZ_Widget  *EZ_CreateCheckButton(parent, label, underline, onvalue, offvalue, onoff)
     EZ_Widget  *parent;     /* parent widget    */
     char             *label;
     int              underline, onvalue, offvalue, onoff;
{
  EZ_Widget  *wptr;

  wptr = EZ_CreateNewWidget(parent);
  /*--------------------------------------------------
   * Register the handling functions for CButton.
   *  has to be done after  EZ_CreateNewWidget.
   *-------------------------------------------------*/
  EZ_WidgetHandlingFunctions[EZ_WIDGET_CHECK_BUTTON] = &EZ_CButtonHandle;

  EZ_SetWidgetTypeAndNames(wptr, EZ_WIDGET_CHECK_BUTTON);
  EZ_CButtonLabel(wptr) = EZ_AllocCopyString(label);
  EZ_CButtonLabelLength(wptr) = (label == (char *)NULL? 1 : strlen(label));
  EZ_CButtonPixmap(wptr) = (EZ_Bitmap *)NULL;
  EZ_CButtonFont(wptr) = EZ_GetFontFromId(EZ_BUTTON_FONT);
  EZ_WidgetBorderStyle(wptr)  = EZ_BORDER_NONE;
  EZ_CButtonTWidth(wptr)     = 0;
  EZ_CButtonTHeight(wptr)    = 0;
  EZ_CButtonFontAscent(wptr) = 0;
  EZ_CButtonLineLength(wptr) = EZ_DEFAULT_LABEL_LINE_LENGTH;
  EZ_CButtonNLines(wptr)  = 1;
  EZ_CButtonCheckBorderWidth(wptr) = 2;
  EZ_CButtonCheckBorderStyle(wptr) = EZ_BORDER_UP;
  EZ_CButtonCheckSizeAdjust(wptr) = 0;
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
  EZ_WidgetPadB(wptr) = 2;

  if(label && underline >= 0 && underline < EZ_MButtonLabelLength(wptr) && isprint(label[underline]))
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

void   EZ_FreeWidgetCButtonData(widget)
     EZ_Widget *widget;
{
  if(EZ_CButtonLabel(widget) != (char *)NULL)
    (void) my_free ((char *)EZ_CButtonLabel(widget));
  if(EZ_CButtonLabel2(widget) != (char *)NULL)
    (void) my_free ((char *)EZ_CButtonLabel2(widget));
  if(EZ_CButtonPixmap(widget) != (EZ_Bitmap *)NULL)
    EZ_FreeBitmap(EZ_CButtonPixmap(widget));
  (void)my_free( (char *)EZ_CButtonData(widget));   /* 6-15-97 */
  {
    EZ_Widget *clone = EZ_CButtonCopy(widget);
    if(clone) {EZ_CButtonCopy(clone) = NULL; }
  }
}

/********************************************************************
 *
 *  Figure out the dimension of a check button
 */
void EZ_ComputeWidgetCButtonSize(widget, w, h)
    EZ_Widget *widget;
     int             *w, *h;
{
  int length, height, cw, ch, padb;
  int pwidth, pheight, ww, hh;
  int orien = EZ_WidgetOrientation(widget);
  int ipadx=0, ipady = 0;

  if(!(EZ_GetWidgetSizeComputedFlag(widget)))
    { 
      int nlines, twidth;
      EZ_Bitmap *bitmap = EZ_CButtonPixmap(widget);

      if(!bitmap || EZ_PTLabel)
        {
          EZ_FigureLabelExtent(EZ_CButtonFont(widget),
                               EZ_CButtonLabel(widget),
                               (int)EZ_CButtonLabelLength(widget),
                               (int)EZ_CButtonLineLength(widget),
                               &nlines, &twidth);
          EZ_CButtonTWidth(widget) = twidth;
          EZ_CButtonNLines(widget) = nlines;

          EZ_CButtonTHeight(widget) = EZ_CButtonFont(widget)->ascent +
            EZ_CButtonFont(widget)->descent;
          EZ_CButtonFontAscent(widget) = EZ_CButtonFont(widget)->ascent;
          if(EZ_CButtonLabel2Length(widget) != 0)
            {
              EZ_CButtonLabel2Width(widget) = EZ_LABEL_SEPARATION +
                XTextWidth(EZ_CButtonFont(widget), 
                           EZ_CButtonLabel2(widget),
                           EZ_CButtonLabel2Length(widget));
              EZ_WidgetAuxXRight(widget) = EZ_CButtonLabel2Width(widget);
            }
          else
            {
              EZ_CButtonLabel2Width(widget) = 0;
              EZ_WidgetAuxXRight(widget) = 0;
            }
        }
      else
        {
          EZ_CButtonTWidth(widget) = 0;
          EZ_CButtonNLines(widget) = 0;
          EZ_CButtonTHeight(widget) = 0;
          EZ_CButtonFontAscent(widget) = 0;
          EZ_CButtonLabel2Width(widget) = 0;
          EZ_WidgetAuxXRight(widget) = 0;
        }

      if(bitmap)
        {
          EZ_CButtonMWidth(widget) = bitmap ->width;
          EZ_CButtonMHeight(widget) = bitmap ->height;
        }
      else
        {
          EZ_CButtonMWidth(widget) = 0;
          EZ_CButtonMHeight(widget) = 0;
        }
      EZ_SetWidgetSizeComputedFlag(widget);
    }
  length = EZ_CButtonTWidth(widget) + EZ_CButtonLabel2Width(widget);
  height = EZ_CButtonTHeight(widget) * EZ_CButtonNLines(widget);
  pwidth = EZ_CButtonMWidth(widget);
  pheight = EZ_CButtonMHeight(widget);
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
  
  cw = EZ_WidgetPadX(widget) + EZ_WidgetBorderWidth(widget);
  ch = EZ_WidgetPadY(widget) + EZ_WidgetBorderWidth(widget);
  ch = ch + ch + hh;
  if(EZ_CButtonCheckType(widget) != EZ_EMPTY_INDICATOR)
    EZ_WidgetAuxXLeft(widget) = (ch << 1)/5  + 6 + EZ_CButtonCheckSizeAdjust(widget); 
  else EZ_WidgetAuxXLeft(widget) = 0;

  cw = cw + cw + ww + EZ_WidgetAuxXLeft(widget);
  padb = (EZ_WidgetPadB(widget) << 1);

  *w = cw + padb;
  *h = ch + padb;
}

/******************************************************************************
 *
 *  Draw a check button
 */

void  EZ_DrawWidgetCheckButton(wptr)
     EZ_Widget *wptr;
{
  int             w, h,x,y, xx, yy, fillx, filly, padb, padb2;
  Pixmap          pixmap;
  Window          win;
  GC              gc;
  int             offset = 0, offseta=0,tileOx=0,tileOy=0,tile;
  int             orien, tx0 = 0, ty0 = 0, pwidth, twidth, pheight, theight;
  unsigned long   bgpv;
  EZ_Bitmap       *bitmap;
  
  /* => if there is no indicator, need a way to indicates the 
   * selection. so we ignore the user-configured border here and
   * switche between UP-DOWN borders. Anything else ?
   */
  if(EZ_CButtonCheckType(wptr) == EZ_EMPTY_INDICATOR)
    {
      if(EZ_CButtonCheck(wptr))
	{
	  EZ_WidgetBorderStyle(wptr) = EZ_BORDER_DOWN;
	}
      else
	{
	  EZ_WidgetBorderStyle(wptr) = EZ_BORDER_UP;
	}
    }
  /*<= */
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

  bitmap = EZ_CButtonPixmap(wptr); 
  xx = EZ_WidgetXOffset(wptr) + fillx;
  yy = EZ_WidgetYOffset(wptr) + filly;
  orien = EZ_WidgetOrientation(wptr);
  pwidth = EZ_CButtonMWidth(wptr);
  pheight = EZ_CButtonMHeight(wptr);
  twidth = EZ_CButtonTWidth(wptr);
  theight = EZ_CButtonTHeight(wptr) * EZ_CButtonNLines(wptr);

  offset = offseta + padb;
  x =  xx + EZ_WidgetPadX(wptr) + EZ_WidgetBorderWidth(wptr) + EZ_WidgetAuxXLeft(wptr) + offset;
  y = yy + EZ_CButtonFontAscent(wptr) + EZ_WidgetBorderWidth(wptr) + EZ_WidgetPadY(wptr) + offset;
  /*--------------------------------------------------------
   *  Draw the label.
   *-------------------------------------------------------*/
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
                               - (int) (EZ_CButtonFontAscent(wptr)));
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
                               - (int) (EZ_CButtonFontAscent(wptr)));
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
                                     (int)EZ_CButtonTHeight(wptr), EZ_CButtonLabel(wptr),
                                     (int)EZ_CButtonLabelLength(wptr), (int)EZ_CButtonLineLength(wptr),
                                     (int)EZ_CButtonNLines(wptr),
                                     (int)EZ_CButtonTWidth(wptr),
                                     (int)EZ_WidgetJustification(wptr),
                                     EZ_CButtonFont(wptr));
        }
      else
        {
          int shadow = EZ_WidgetLabelShadow(wptr);
          if(EZ_CButtonForeground(wptr) != EZ_DefaultForeground || 
             EZ_CButtonFont(wptr) != EZ_GetFontFromId(EZ_BUTTON_FONT))
            {
              XSetFont(EZ_DisplayForWidgets, EZ_WRITABLEGC, EZ_CButtonFont(wptr)->fid);
              XSetForeground(EZ_DisplayForWidgets, EZ_WRITABLEGC, EZ_CButtonForeground(wptr));	
              gc = EZ_WRITABLEGC;
            }
          else
            gc = EZ_BUTTON_TEXTGC;

          EZ_RenderLabelText(pixmap, gc, x, y, 
                             (int)EZ_CButtonTHeight(wptr), EZ_CButtonLabel(wptr),
                             (int)EZ_CButtonLabelLength(wptr), (int)EZ_CButtonLineLength(wptr),
                             (int)EZ_CButtonNLines(wptr),
                             (int)EZ_CButtonTWidth(wptr),
                             (int)EZ_WidgetJustification(wptr),
                             EZ_CButtonFont(wptr),
                             shadow);

          if(EZ_CButtonLabel2Length(wptr) > 0)
            {
              int tmpx, tmpy;
              tmpx = EZ_GetLabel2XPosition(wptr,
                                           EZ_CButtonLabel2Width(wptr) -
                                           EZ_LABEL_SEPARATION, offseta);
              tmpy = ((h+EZ_CButtonFontAscent(wptr))>>1) + offseta;
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
                              EZ_CButtonLabel2(wptr),
                              EZ_CButtonLabel2Length(wptr));
                }
              XDrawString(EZ_DisplayForWidgets, 
                          pixmap,  gc,
                          tmpx, tmpy,
                          EZ_CButtonLabel2(wptr),
                          EZ_CButtonLabel2Length(wptr));
            }
          else if(EZ_CButtonUnderLine(wptr) >= 0 && 
                  EZ_CButtonUnderLine(wptr) < ((int)EZ_CButtonLineLength(wptr)>>1))
            {
              int tlen = XTextWidth(EZ_CButtonFont(wptr),EZ_CButtonLabel(wptr),
                                    EZ_CButtonUnderLine(wptr));
              int tlen1 = tlen + XTextWidth(EZ_CButtonFont(wptr),
                                            EZ_CButtonLabel(wptr)+EZ_CButtonUnderLine(wptr), 1);
              int base = y + EZ_CButtonTHeight(wptr) - EZ_CButtonFontAscent(wptr)-1;
              int boldfont;

              if(EZ_CButtonFont(wptr) != EZ_GetFontFromId(EZ_BUTTON_FONT))
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
                              x+tlen+shadowx, base+shadowy+1, x+tlen1+shadowx, base+1+shadowy);
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
   * draw the check mark
   *--------------------*/
  EZ_DrawIndicator(wptr, pixmap, (int)EZ_CButtonOn(wptr),
		   (int) EZ_CButtonCheckType(wptr),
		   (int) EZ_CButtonCheckBorderWidth(wptr),
		   (int) EZ_CButtonCheckSizeAdjust(wptr),
		   EZ_CButtonCheckColor(wptr));

  /*============================================================
   *for complex button with  NW labels 
   */
  {
    EZ_Widget *children = EZ_WidgetChildren(wptr);
    if(children && EZ_WidgetType(children) == EZ_WIDGET_NW_LABEL)
      {
	EZ_DisplayNWLabel(children, pixmap, EZ_WidgetForeground(wptr), bgpv, 
			  EZ_GetWidgetDisabledFlag(wptr), EZ_GetWidgetHighlightFlag(wptr),
			  0,0, offseta,1);
      }
  }
  /*============================================================*/  
  /*------------------------------------
   *  Now draw the real border.
   *-----------------------------------*/
  if(EZ_CButtonCheckType(wptr) == EZ_EMPTY_INDICATOR)
    {
      /* no indicator, need a way to indicates the selection.
       * so we ignore the user-configured border here and
       * switche between UP-DOWN borders. Anything else ?
       */
      if(EZ_CButtonCheck(wptr))
	{
	  EZ_WidgetBorderStyle(wptr) = EZ_BORDER_DOWN;
	}
      else
	{
	  EZ_WidgetBorderStyle(wptr) = EZ_BORDER_UP;
	}
    }
  EZ_DrawRectBorder(wptr, pixmap);
  XCopyArea(EZ_DisplayForWidgets,pixmap,win, EZ_WRITABLEGC,0,0,w,h,0,0); 
  XFreePixmap(EZ_DisplayForWidgets, pixmap);                             
}


/*************************************************************************
 *
 *  Event handling
 */
void  EZ_CButtonEventHandle(widget, event)
     EZ_Widget *widget;
     XEvent          *event;
{
  if(widget == (EZ_Widget *)NULL)  return;

  if(event->type == Expose) EZ_DrawWidget(widget);      

  if( EZ_GetWidgetDisabledFlag(widget) )   return;
  /*
    if(event->type == ButtonPress && event->xbutton.button != EZ_Btn1)
    return;
    */

  switch(event->type)
    {
    case ButtonPress:
      if(event->xbutton.button == EZ_Btn1)
	{
	  if( EZ_CButtonOn(widget) )
	    {
	      EZ_CButtonSetOffValue(widget);
	    }
	  else
	    {
	      EZ_CButtonSetOnValue(widget);
	    }
	  EZ_DrawWidget(widget);
	  { EZ_ExecuteWidgetCallBacks(widget);}
	}
      else  if(event->xbutton.button == EZ_Btn3)
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

void EZ_SetCheckButtonState(widget, on_off)
     EZ_Widget *widget;
     int       on_off;
{
  if(widget != (EZ_Widget *)NULL)
    if(EZ_WidgetType(widget) == EZ_WIDGET_CHECK_BUTTON ||
       EZ_WidgetType(widget) == EZ_WIDGET_MENU_CHECK_BUTTON)
      {
	if(on_off == 0)   EZ_CButtonSetOffValue(widget);
	else  EZ_CButtonSetOnValue(widget);
	if(EZ_WidgetMapped(widget) !=  0)
	  EZ_DrawWidget(widget);
      }
}

int EZ_GetCheckButtonState(widget, value_ret)
     EZ_Widget *widget;
     int       *value_ret;
{
  if(widget != (EZ_Widget *)NULL)
    if(EZ_WidgetType(widget) == EZ_WIDGET_CHECK_BUTTON ||
       EZ_WidgetType(widget) == EZ_WIDGET_MENU_CHECK_BUTTON)
      {
	if(EZ_CButtonOn(widget)) 
	  {
	    *value_ret = EZ_CButtonVarOnValue(widget);
	    return(1);
	  }
	else
	  {
	    *value_ret = EZ_CButtonVarOffValue(widget);
	    return(0);
	  }
      }
  return(-1);
}
/****************************************************************************/
#undef _EZ_WIDGET_BUTTONC_C_
