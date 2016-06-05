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
 ***              Widget Button                                ***
 ***                                                           ***
 *****************************************************************/
#define _EZ_WIDGET_BUTTONN_C_

#include "EZ_Widget.h"

/*********************************************************************
 * 
 *  Functions implemented in this file:
 */
EZ_Widget  *EZ_CreateButton MY_ANSIARGS((EZ_Widget *parent, char *label, int undline));
void             EZ_DrawWidgetButton MY_ANSIARGS((EZ_Widget *widget));
void             EZ_ComputeWidgetButtonSize MY_ANSIARGS((EZ_Widget *widget, int *w, int *h));
void             EZ_FreeWidgetNButtonData MY_ANSIARGS((EZ_Widget *widget));
void             EZ_NButtonEventHandle MY_ANSIARGS((EZ_Widget *widget, XEvent *event));
void             EZ_SetButtonBorderType MY_ANSIARGS((EZ_Widget *widget, int flag));
int              EZ_OppsiteBorder MY_ANSIARGS((int bd, int flag));
/*********************************************************************/

/*********************************************************************
 * 
 *  Local Variables.
 */
static EZ_WidgetHandle EZ_ButtonHandle =
{ 
  EZ_ComputeWidgetButtonSize,
  EZ_DrawWidgetButton,
  EZ_FreeWidgetNButtonData,
  EZ_NButtonEventHandle,
};

/*********************************************************************/

EZ_Widget  *EZ_CreateButton(parent,label, underline)
     EZ_Widget  *parent;     /* parent widget    */
     char       *label;
     int        underline;     
{
  EZ_Widget  *wptr;

  wptr = EZ_CreateNewWidget(parent);
  /*--------------------------------------------------
   * Register the handling functions for Button.
   *  has to be done after  EZ_CreateNewWiget.
   *-------------------------------------------------*/
  EZ_WidgetHandlingFunctions[EZ_WIDGET_NORMAL_BUTTON] = &EZ_ButtonHandle;

  EZ_SetWidgetTypeAndNames(wptr, EZ_WIDGET_NORMAL_BUTTON);
  EZ_ButtonLabel(wptr) = EZ_AllocCopyString(label);
  EZ_ButtonLabelLength(wptr) = (label == (char *)NULL? 1 : strlen(label));
  EZ_ButtonPixmap(wptr) = (EZ_Bitmap *)NULL;
  EZ_ButtonFont(wptr) = EZ_GetFontFromId(EZ_BUTTON_FONT);
  EZ_WidgetBorderStyle(wptr)  = EZ_BORDER_UP;
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
  EZ_WidgetPadB(wptr) = 2;

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


void   EZ_FreeWidgetNButtonData(widget)
     EZ_Widget *widget;
{
  if(EZ_ButtonLabel(widget) != (char *)NULL)
    (void) my_free((char *)EZ_ButtonLabel(widget));
  if(EZ_ButtonPixmap(widget) != (EZ_Bitmap *)NULL)
    EZ_FreeBitmap(EZ_ButtonPixmap(widget));
  if(EZ_ButtonLabel2(widget)  != (char *)NULL)
    (void) my_free((char *)EZ_ButtonLabel2(widget));
  {
    EZ_Widget *clone = EZ_ButtonCopy(widget);
    if(clone) {EZ_ButtonCopy(clone) = NULL; }
  }
}
 
/********************************************************************
 *
 *  Figure out the dimension of a normal button
 */
void EZ_ComputeWidgetButtonSize(widget, w, h)
    EZ_Widget *widget;
     int             *w, *h;
{
  int length, height, cw, ch, bw;
  int pwidth, pheight, ww, hh;
  int orien = EZ_WidgetOrientation(widget);
  int ipadx=0, ipady = 0;

  if(!(EZ_GetWidgetSizeComputedFlag(widget)))
    { 
      int nlines, twidth;
      EZ_Bitmap *bitmap = EZ_ButtonPixmap(widget);

      if(!bitmap || EZ_PTLabel)
        {
          EZ_FigureLabelExtent(EZ_ButtonFont(widget),
                               EZ_ButtonLabel(widget),
                               (int)EZ_ButtonLabelLength(widget),
                               (int)EZ_ButtonLineLength(widget),
                               &nlines, &twidth);
          EZ_ButtonTWidth(widget) = twidth;
          EZ_ButtonNLines(widget) = nlines;
	  
          EZ_ButtonTHeight(widget) = EZ_ButtonFont(widget)->ascent +
            EZ_ButtonFont(widget)->descent;
          EZ_ButtonFontAscent(widget) = EZ_ButtonFont(widget)->ascent;
          if(EZ_ButtonLabel2Length(widget) != 0)
            {
              EZ_ButtonLabel2Width(widget) = EZ_LABEL_SEPARATION +
                XTextWidth(EZ_ButtonFont(widget), 
                           EZ_ButtonLabel2(widget),
                           EZ_ButtonLabel2Length(widget));
              EZ_WidgetAuxXRight(widget) = EZ_ButtonLabel2Width(widget);
            }
          else
            {
              EZ_ButtonLabel2Width(widget) = 0;
              EZ_WidgetAuxXRight(widget) = 0;
            }
        }
      else
        {
          EZ_ButtonTWidth(widget) = 0;
          EZ_ButtonNLines(widget) = 0;
          EZ_ButtonTHeight(widget) = 0;
          EZ_ButtonFontAscent(widget)=0;
          EZ_ButtonLabel2Width(widget) = 0;
          EZ_WidgetAuxXRight(widget) = 0;
        }

      if(bitmap)
	{
          EZ_ButtonMWidth(widget) = bitmap ->width;
          EZ_ButtonMHeight(widget) = bitmap ->height;
        }
      else
        {
          EZ_ButtonMWidth(widget) = 0;
          EZ_ButtonMHeight(widget) = 0;
        }
      EZ_SetWidgetSizeComputedFlag(widget);
    }
  length = EZ_ButtonTWidth(widget) + EZ_ButtonLabel2Width(widget);
  height = EZ_ButtonTHeight(widget) * EZ_ButtonNLines(widget);
  pwidth = EZ_ButtonMWidth(widget);
  pheight = EZ_ButtonMHeight(widget);

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

  bw =  EZ_WidgetBorderWidth(widget) + EZ_WidgetPadB(widget);
  cw = EZ_WidgetPadX(widget) + bw;
  ch = EZ_WidgetPadY(widget) + bw;
  cw = cw + cw + ww + ipadx;
  ch = ch + ch + hh + ipady;

  *w = cw;
  *h = ch;
}

/****************************************************************************
 *
 *  Draw a normal button.
 */

void  EZ_DrawWidgetButton(wptr)
     EZ_Widget *wptr;
{
  int             w, h,x,y,xx,yy,fillx,filly, padb, padb2;
  Pixmap          pixmap;
  EZ_Bitmap       *bitmap;
  Window          win;
  GC              gc;
  int             offset, tile, offseta = 0, tileOx=0, tileOy=0;
  int             orien, tx0 = 0, ty0 = 0, pwidth, twidth, pheight, theight;
  unsigned long   bgpv;

  win = EZ_WidgetWindow(wptr);
  w   = EZ_WidgetWidth(wptr);
  h   = EZ_WidgetHeight(wptr);
  padb = EZ_WidgetPadB(wptr);
  padb2 = padb + padb;

  bitmap = EZ_ButtonPixmap(wptr);

  if(EZ_OffsetEnabled)
    {
      switch(EZ_WidgetBorderStyle(wptr))
	{
	case EZ_BORDER_DOWN:
	case EZ_BORDER_NB_DOWN: 
	case EZ_BORDER_NB_DOWN_B: 
	case EZ_BORDER_NB_DOWN_R: 
	case EZ_BORDER_NB_DOWN_L: 
	  offseta = 1;
	  break;
	}
    }
  EZ_GetWidgetLabelPosition(wptr, &fillx, &filly);
  /*-----------------------------------------------------------
   *  Create a pixmap, draw into this pixmap in background and
   *  copy to the button window when finished.
   *----------------------------------------------------------*/
  pixmap = XCreatePixmap(EZ_DisplayForWidgets, win, w, h, EZ_DepthForWidgets);    
  EZ_FixFocusHighlightBorder(wptr, pixmap, w, h, padb);
  if(EZ_ButtonNoHighlight(wptr)) EZ_ClearWidgetHighlightFlag(wptr);
  tile = EZ_GetBackgroundGC(wptr, &gc,&bgpv,1,0);  
  if(tile)
    {
      EZ_GetWidgetTileOrigin(wptr, &tileOx, &tileOy);      
      tileOx += offseta; tileOy += offseta;
      XSetTSOrigin(EZ_Display, gc, tileOx, tileOy);
    }

  /* fill the background */
    {
      int bdstyle = EZ_WidgetBorderStyle(wptr);
      switch(bdstyle)
	{
	case EZ_BORDER_NB_UP:
	case EZ_BORDER_NB_DOWN:
	  XFillRectangle(EZ_Display, pixmap, gc, padb,padb, w-padb2, h); 
	  break;
	case EZ_BORDER_NB_UP_B:
	case EZ_BORDER_NB_DOWN_B: 
	  XFillRectangle(EZ_Display, pixmap, gc, padb, 0, w-padb2, h -padb); 
	  break;
	case EZ_BORDER_NB_UP_L:
	case EZ_BORDER_NB_DOWN_L: 
	  XFillRectangle(EZ_Display, pixmap, gc, padb, padb, w-padb, h -padb2); 
	  break;
	case EZ_BORDER_NB_UP_R:
	case EZ_BORDER_NB_DOWN_R: 
	  XFillRectangle(EZ_Display, pixmap, gc, 0, padb, w-padb, h -padb2); 
	  break;
	default:
	  XFillRectangle(EZ_Display, pixmap, gc, padb,padb, w-padb2, h-padb2); 
	  break;
	}
    }
  /*--------------------------------------------------------
   *  Draw the label.
   *-------------------------------------------------------*/
  xx = fillx + EZ_WidgetXOffset(wptr);
  yy = filly + EZ_WidgetYOffset(wptr);
  orien = EZ_WidgetOrientation(wptr);
  pwidth = EZ_ButtonMWidth(wptr);
  pheight = EZ_ButtonMHeight(wptr);
  twidth = EZ_ButtonTWidth(wptr);
  theight = EZ_ButtonTHeight(wptr) * EZ_ButtonNLines(wptr);

  offset = offseta + padb;
  x = xx + EZ_WidgetPadX(wptr) + EZ_WidgetBorderWidth(wptr) + offset;
  y = yy + EZ_ButtonFontAscent(wptr) + EZ_WidgetBorderWidth(wptr) + EZ_WidgetPadY(wptr) + offset;

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
                               - (int) (EZ_ButtonFontAscent(wptr)));
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
                               - (int) (EZ_ButtonFontAscent(wptr)));
        }
    }
  x += tx0; y += ty0;
  if(!bitmap || EZ_PTLabel)
    {
      if( EZ_GetWidgetDisabledFlag(wptr) )
        {
          GC  bgc, lgc, ngc;
          if( EZ_GetDarkBrightNormalBDGC(wptr, &bgc, &lgc, &ngc))
            {
              XSetTSOrigin(EZ_Display, bgc, tileOx, tileOy);
              XSetTSOrigin(EZ_Display, lgc, tileOx, tileOy);
              XSetTSOrigin(EZ_Display, ngc, tileOx, tileOy);
            }
          XSetFont(EZ_DisplayForWidgets, bgc, EZ_WidgetFont(wptr)->fid);
          XSetFont(EZ_DisplayForWidgets, lgc, EZ_WidgetFont(wptr)->fid);
          XSetFont(EZ_DisplayForWidgets, ngc, EZ_WidgetFont(wptr)->fid);



          EZ_RenderLabelTextDisabled(pixmap, lgc, bgc, ngc, x, y, 
                                     (int)EZ_ButtonTHeight(wptr), EZ_ButtonLabel(wptr),
                                     (int)EZ_ButtonLabelLength(wptr), (int)EZ_ButtonLineLength(wptr),
                                     (int)EZ_ButtonNLines(wptr),
                                     (int)EZ_ButtonTWidth(wptr),
                                     (int)EZ_WidgetJustification(wptr),
                                     EZ_ButtonFont(wptr));

        }
      else
        {
          int shadow = EZ_WidgetLabelShadow(wptr);
          if(EZ_ButtonForeground(wptr) != EZ_DefaultForeground || 
             EZ_ButtonFont(wptr) != EZ_GetFontFromId(EZ_BUTTON_FONT))
            {
              XSetFont(EZ_DisplayForWidgets,  EZ_WRITABLEGC, EZ_ButtonFont(wptr)->fid);
              XSetForeground(EZ_DisplayForWidgets, EZ_WRITABLEGC, EZ_ButtonForeground(wptr));
              gc =  EZ_WRITABLEGC;
            }
          else 
            gc = EZ_BUTTON_TEXTGC;

          EZ_RenderLabelText(pixmap, gc, x, y, 
                             (int)EZ_ButtonTHeight(wptr), EZ_ButtonLabel(wptr),
                             (int)EZ_ButtonLabelLength(wptr), (int)EZ_ButtonLineLength(wptr),
                             (int)EZ_ButtonNLines(wptr),
                             (int)EZ_ButtonTWidth(wptr),
                             (int)EZ_WidgetJustification(wptr),
                             EZ_ButtonFont(wptr), shadow);
          if(EZ_ButtonLabel2Length(wptr) != 0)
            {
              int tmpx, tmpy=((h+EZ_ButtonFontAscent(wptr))>>1) + offseta;
              tmpx = EZ_GetLabel2XPosition(wptr,
                                           EZ_ButtonLabel2Width(wptr) -
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
                              EZ_ButtonLabel2(wptr),
                              EZ_ButtonLabel2Length(wptr));
                }
              XDrawString(EZ_DisplayForWidgets, 
                          pixmap,  gc,
                          tmpx, tmpy,
                          EZ_ButtonLabel2(wptr),
                          EZ_ButtonLabel2Length(wptr));
            }
          else if(EZ_ButtonUnderLine(wptr) >= 0 && 
                  EZ_ButtonUnderLine(wptr) < ((int)EZ_ButtonLineLength(wptr)>>1))
            {
              int tlen = XTextWidth(EZ_ButtonFont(wptr),EZ_ButtonLabel(wptr),
                                    EZ_ButtonUnderLine(wptr));
              int tlen1 = tlen + XTextWidth(EZ_ButtonFont(wptr),
                                            EZ_ButtonLabel(wptr)+EZ_ButtonUnderLine(wptr), 1);
              int base = y + EZ_ButtonTHeight(wptr) - EZ_ButtonFontAscent(wptr)-1;
              int boldfont;

              if(EZ_ButtonFont(wptr) != EZ_GetFontFromId(EZ_BUTTON_FONT))
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
  
  /*============================================================
   *for complex button with  NW labels 
   */
  {
    EZ_Widget *children = EZ_WidgetChildren(wptr);
    if(children && EZ_WidgetType(children) == EZ_WIDGET_NW_LABEL)
      {
	EZ_DisplayNWLabel(children, pixmap, EZ_WidgetForeground(wptr), bgpv,
			  EZ_GetWidgetDisabledFlag(wptr),EZ_GetWidgetHighlightFlag(wptr),
			  0, 0, offseta, 1);
      }
  }
  /*============================================================*/  
  
  EZ_DrawRectBorder(wptr, pixmap);
  XCopyArea(EZ_Display,pixmap,win, EZ_WRITABLEGC,0,0,w,h,0,0);  
  XFreePixmap(EZ_Display, pixmap);                             
}

/*************************************************************************
 *
 *  Event handling
 */

void  EZ_NButtonEventHandle(widget, event)
     EZ_Widget *widget;
     XEvent          *event;
{
  int x, y;
  if(widget == (EZ_Widget *)NULL)   return;

  if(event->type == Expose)   EZ_DrawWidget(widget);      
  if( EZ_GetWidgetDisabledFlag(widget) )  return;

  switch(event->type)
    {
    case ButtonPress:
      if(event->xbutton.button == EZ_Btn1)
	{
	  EZ_ButtonPressed(widget) = 1;
	  EZ_SetButtonBorderType(widget, 1);
	  EZ_DrawWidget(widget);
	}
      else if(event->xbutton.button == EZ_Btn3)
	{
          EZ_HandleDnDEvents(widget, event);
	}
      break;
    case ButtonRelease:
      if(event->xbutton.button == EZ_Btn1)
	{
	  if(EZ_ButtonPressed(widget))
	    {
	      EZ_ButtonPressed(widget) = 0;
	      EZ_SetButtonBorderType(widget, 0);
	      EZ_DrawWidget(widget);
	      x = event->xbutton.x;
	      y = event->xbutton.y;
	      if( x >=0 && x <= EZ_WidgetWidth(widget) && y >= 0 &&
		  y <= EZ_WidgetHeight(widget))
		{ EZ_ExecuteWidgetCallBacks(widget);}
	    }
	}
      break;
    case EnterNotify:
      if(EZ_ButtonPressed(widget) == 1)
	EZ_SetButtonBorderType(widget, 1);
      EZ_SetWidgetHighlightFlag(widget);
      { EZ_InstallHighlightColor(widget); } /*4/29/99*/
      EZ_DrawWidget(widget);
      break;
    case LeaveNotify:
      EZ_SetButtonBorderType(widget, 0);
      EZ_ClearWidgetHighlightFlag(widget);
      { EZ_UninstallHighlightColor(widget); } /*4/29/99*/
      EZ_DrawWidget(widget);
      break;
    default:
      break;
    }
}
/****************************************************************/
void  EZ_SetButtonBorderType(widget, flag)
     EZ_Widget *widget;
     int flag;
{
  int nbd;
  int obd = EZ_WidgetBorderStyle(widget);
  if(flag)
    {
      nbd = EZ_OppsiteBorder(obd, 1);
      if(nbd != obd)
        {
          EZ_ButtonSetBorder(widget) = 1;
          EZ_WidgetBorderStyle(widget) = nbd;
        }
      else  EZ_ButtonSetBorder(widget) = 0;
    }
  else
    {
      if(EZ_ButtonSetBorder(widget))
	{EZ_WidgetBorderStyle(widget) = EZ_OppsiteBorder(obd, 0);}
    }
}

/****************************************************************/
int  EZ_OppsiteBorder(bd, flag) int bd, flag;
{
  if(flag)  /*  get down border */
    {
      switch(bd)
        {
        case EZ_BORDER_UP:
          return( EZ_BORDER_DOWN );
          break;
        case EZ_BORDER_FRAMED_UP:
          return( EZ_BORDER_FRAMED_DOWN);
          break;
        case EZ_BORDER_NB_UP:
          return( EZ_BORDER_NB_DOWN);
          break;
        case EZ_BORDER_NB_UP_B:
          return( EZ_BORDER_NB_DOWN_B);
          break;
        case EZ_BORDER_NB_UP_L:
          return( EZ_BORDER_NB_DOWN_L);
          break;
        case EZ_BORDER_NB_UP_R:
          return( EZ_BORDER_NB_DOWN_R);
          break;
        case EZ_BORDER_FRAMED:
          return( EZ_BORDER_CARVED);
          break;
        default:
          return(bd);
        }
    }
  else  /* get up border */
    {
      switch(bd)
        {
        case EZ_BORDER_DOWN:
          return( EZ_BORDER_UP);
          break;
        case EZ_BORDER_FRAMED_DOWN:
          return( EZ_BORDER_FRAMED_UP);
          break;
        case EZ_BORDER_NB_DOWN:
          return( EZ_BORDER_NB_UP);
          break;
        case EZ_BORDER_NB_DOWN_B:
          return( EZ_BORDER_NB_UP_B);
          break;
        case EZ_BORDER_NB_DOWN_L:
          return( EZ_BORDER_NB_UP_L);
          break;
        case EZ_BORDER_NB_DOWN_R:
          return( EZ_BORDER_NB_UP_R);
          break;
        case EZ_BORDER_CARVED:
          return( EZ_BORDER_FRAMED);
          break;
        default:
          return(bd);
          break;
        }
    }
}
/********************************************************************************/
#undef _EZ_WIDGET_BUTTONN_C_
