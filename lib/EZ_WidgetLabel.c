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
 ***              Widget Label.                                ***
 ***                                                           ***
 *****************************************************************/
#define _EZ_WIDGET_LABEL_C_
#include "EZ_Widget.h"

/*********************************************************************
 * 
 *  Functions implemented in this file:
 */
EZ_Widget  *EZ_CreateLabel MY_ANSIARGS((EZ_Widget *parent, char *label));
void             EZ_DrawWidgetLabel MY_ANSIARGS((EZ_Widget *widget));
void             EZ_ComputeWidgetLabelSize MY_ANSIARGS((EZ_Widget *widget, int *w, int *h));
void             EZ_FreeWidgetLabelData MY_ANSIARGS((EZ_Widget *widget));
void             EZ_LabelEventHandle MY_ANSIARGS((EZ_Widget *widget, XEvent *event));
/*********************************************************************
 * 
 *  Local Variables.
 */
static EZ_WidgetHandle EZ_LabelHandle =
{ 
  EZ_ComputeWidgetLabelSize,
  EZ_DrawWidgetLabel,
  EZ_FreeWidgetLabelData,
  EZ_LabelEventHandle,    /* response only to Expose */
};


/*********************************************************************/


EZ_Widget  *EZ_CreateLabel(parent,label)
     EZ_Widget  *parent;     /* parent widget    */
     char             *label;      /* label string     */
{
  EZ_Widget  *tmp;
  
  tmp = EZ_CreateNewWidget(parent);
  /*--------------------------------------------------
   * Register the handling functions for label.
   *  has to be done after  EZ_CreateNewWiget.
   *-------------------------------------------------*/
  EZ_WidgetHandlingFunctions[EZ_WIDGET_LABEL] = &EZ_LabelHandle;

  EZ_SetWidgetTypeAndNames(tmp,EZ_WIDGET_LABEL);
  EZ_WidgetBorderStyle(tmp)  = EZ_BORDER_NONE;
  EZ_WidgetBorderWidth(tmp)  = 0;
  EZ_WidgetPadB(tmp)  = 0;
  EZ_WidgetSepX(tmp)  = 8;
  EZ_WidgetSepY(tmp)  = 8;
  EZ_LabelPixmap(tmp) = (EZ_Bitmap *)NULL;
  EZ_LabelString(tmp) = EZ_AllocCopyString(label);
  EZ_LabelStringLength(tmp) = (label == (char *)NULL ? 1: strlen(label));
  EZ_LabelLineLength(tmp) = EZ_DEFAULT_LABEL_LINE_LENGTH;
  EZ_LabelNLines(tmp) = 1;
  EZ_LabelFont(tmp) = EZ_GetFontFromId(EZ_LABEL_FONT);
  EZ_LabelTWidth(tmp)  = 0;
  EZ_LabelTHeight(tmp) = 0;
  EZ_LabelFontAscent(tmp) = 0;
  EZ_LabelAlwaysHighlight(tmp) = 0;
  /* for free label: serve for special popups */
  EZ_LabelMovable(tmp) = 1; 
  EZ_LabelDontTakeFocus(tmp) = 0;
  EZ_LabelIsMenuTitle(tmp) = 0;
  EZ_LabelIsLabeledIcon(tmp) = 0;
  EZ_LabelTextMask(tmp) = (Pixmap)None;
  EZ_LabelUnderline(tmp) = -1;
  EZ_LabelModifiers(tmp) = 0;
  EZ_LabelShortcut(tmp) = NULL;
  EZ_LabelMenu(tmp) = NULL;
  EZ_LabelCopy(tmp) = NULL;
  return(tmp);
}


void   EZ_FreeWidgetLabelData(widget)
     EZ_Widget *widget;
{
  if(EZ_LabelString(widget) != (char *)NULL)
    (void) my_free((char *)EZ_LabelString(widget));
  if(EZ_LabelPixmap(widget) != (EZ_Bitmap *)NULL)
    EZ_FreeBitmap(EZ_LabelPixmap(widget));
  if(EZ_LabelTextMask(widget) != (Pixmap)None)
    XFreePixmap(EZ_Display, EZ_LabelTextMask(widget));
  {
    EZ_Widget *clone = EZ_LabelCopy(widget);
    if(clone) {EZ_LabelCopy(clone) = NULL; }
  }
}

 
/********************************************************************
 *
 *  Figure out the dimension of a label widget.
 */

void EZ_ComputeWidgetLabelSize(widget, w, h)
    EZ_Widget *widget;
     int             *w, *h;
{
  int      length, height, cw, ch, ww, hh;
  int      pwidth, pheight;
  int      orien = EZ_WidgetOrientation(widget);
  int      ipadx=0, ipady = 0;

  if(!(EZ_GetWidgetSizeComputedFlag(widget)))
    { 
      int nlines, twidth;
      EZ_Bitmap *bitmap = EZ_LabelPixmap(widget);

      if(!bitmap || EZ_PTLabel)
        {
          EZ_FigureLabelExtent(EZ_LabelFont(widget),
                               EZ_LabelString(widget),
                               (int)EZ_LabelStringLength(widget),
                               (int)EZ_LabelLineLength(widget),
                               &nlines, &twidth);
          EZ_LabelTWidth(widget) = twidth;
          EZ_LabelNLines(widget) = nlines;
          EZ_LabelTHeight(widget) = (EZ_LabelFont(widget)->ascent +
                                     EZ_LabelFont(widget)->descent);
          EZ_LabelFontAscent(widget) = EZ_LabelFont(widget)->ascent;
        }
      else
        {
          EZ_LabelTWidth(widget) = 0;
          EZ_LabelNLines(widget) = 0;
          EZ_LabelFontAscent(widget) = 0;
          EZ_LabelTHeight(widget) = 0;
        }
      
      if(bitmap)
        {
          EZ_LabelMWidth(widget) = bitmap->width;
          EZ_LabelMHeight(widget) = bitmap->height;
        }
      else
        {
          EZ_LabelMWidth(widget) = 0;
          EZ_LabelMHeight(widget) = 0;
        }
      EZ_SetWidgetSizeComputedFlag(widget);
    }
  length = EZ_LabelTWidth(widget);
  height = EZ_LabelTHeight(widget) * EZ_LabelNLines(widget);
  pwidth = EZ_LabelMWidth(widget);
  pheight = EZ_LabelMHeight(widget);

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

  if(EZ_LabelArrowType(widget)) { ww = hh = 12;}
  cw = EZ_WidgetPadX(widget) + EZ_WidgetBorderWidth(widget) + EZ_WidgetPadB(widget);
  ch = EZ_WidgetPadY(widget) + EZ_WidgetBorderWidth(widget) + EZ_WidgetPadB(widget);
  cw = cw + cw + ww + ipadx;
  ch = ch + ch + hh + ipady;  
  
  *w = cw;
  *h = ch;
}

/*******************************************************************
 *
 *  Draw A Label Widget.
 */
void  EZ_DrawWidgetLabel(wptr)
     EZ_Widget *wptr;
{
  int       w, h,x,y,xx,yy,fillx,filly,padb, padb2, tile, arrowType;
  EZ_Bitmap *bitmap;
  Pixmap    pixmap;
  Window    win;
  GC        gc;
  unsigned long   bgpv;
  int      pwidth, pheight;

  win = EZ_WidgetWindow(wptr);
  w   = EZ_WidgetWidth(wptr);
  h   = EZ_WidgetHeight(wptr);
  padb = EZ_WidgetPadB(wptr);
  padb2 = padb + padb;

  bitmap = EZ_LabelPixmap(wptr);
  EZ_GetWidgetLabelPosition(wptr, &fillx, &filly);
  pixmap = XCreatePixmap(EZ_Display, win, w, h, EZ_Depth); 
  EZ_FixFocusHighlightBorder(wptr, pixmap, w, h, padb);
  tile = EZ_GetBackgroundGC(wptr, &gc, &bgpv, 1, (int)EZ_LabelAlwaysHighlight(wptr));
  if(tile)
    {
      int ox, oy;
      EZ_GetWidgetTileOrigin(wptr, &ox, &oy);      
      XSetTSOrigin(EZ_Display, gc, ox, oy);
    }
  XFillRectangle(EZ_Display, pixmap, gc, padb,padb, w-padb2, h-padb2); 

  /*--------------------------------------
   * render the label 
   *-------------------------------------*/
  xx = fillx + EZ_WidgetXOffset(wptr);
  yy = filly + EZ_WidgetYOffset(wptr);
  
  x = xx + EZ_WidgetPadX(wptr) + EZ_WidgetBorderWidth(wptr) + padb;
  y = yy + EZ_LabelFontAscent(wptr) + EZ_WidgetBorderWidth(wptr) + EZ_WidgetPadY(wptr) + padb;

  if((arrowType = EZ_LabelArrowType(wptr)) != 0)
    {
      int itmp =  EZ_WidgetBorderWidth(wptr) + padb;
      int x0 = EZ_WidgetPadX(wptr) + itmp;
      int y0 = EZ_WidgetPadY(wptr) + itmp;
      gc = EZ_WRITABLEGC;
      EZ_DrawSpecial(wptr,pixmap, None, arrowType, x0, y0, w-(x0+x0), h-(y0+y0));
    }
  else 
    {
      int orien = EZ_WidgetOrientation(wptr);
      int tx0=0, ty0=0;
      pwidth = EZ_LabelMWidth(wptr);
      pheight = EZ_LabelMHeight(wptr);

      if(orien >= EZ_VERTICAL)
        {
          if(bitmap)
            {
              int twidth = EZ_LabelTWidth(wptr);
              int px0, py0;
              if(twidth > pwidth) 
                {
                  px0 = (twidth-pwidth)>>1; py0 = 0;
                  tx0 = 0; ty0 = bitmap->height + EZ_WidgetSepY(wptr);
                }
              else
                {
                  px0 = 0; py0=0;
                  tx0 = (pwidth-twidth)>>1; 
                  ty0 = bitmap->height + EZ_WidgetSepY(wptr);                  
                }
              EZ_RenderPixmapLabel(wptr, bitmap, pixmap,
                                   x+px0, y+py0 - (int)( EZ_LabelFontAscent(wptr)));
            }
        }
      else /* horizontal */
        {
          if(bitmap)
            {
              int theight = EZ_LabelTHeight(wptr) * EZ_LabelNLines(wptr);
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
              EZ_RenderPixmapLabel(wptr, bitmap, pixmap,
                                   x+px0, y+py0 - (int)( EZ_LabelFontAscent(wptr)));
            }
        }
      x += tx0; y += ty0;
      if(!bitmap || EZ_PTLabel)
        {
          short *txtpos = EZ_IToplevelTextGeom(wptr);
          int shadow = EZ_WidgetLabelShadow(wptr);

          if(EZ_LabelForeground(wptr) != EZ_DefaultForeground ||
             EZ_LabelFont(wptr) != EZ_GetFontFromId(EZ_LABEL_FONT))
            {
              XSetFont(EZ_Display, EZ_WRITABLEGC, EZ_LabelFont(wptr)->fid);
              XSetForeground(EZ_Display, EZ_WRITABLEGC, EZ_LabelForeground(wptr));
              gc = EZ_WRITABLEGC;
            }
          else  gc = EZ_LABEL_TEXTGC;

          EZ_RenderLabelText(pixmap, gc, x, y, (int)EZ_LabelTHeight(wptr), EZ_LabelString(wptr),
                             (int)EZ_LabelStringLength(wptr), (int)EZ_LabelLineLength(wptr),
                             (int)EZ_LabelNLines(wptr),
                             (int)EZ_LabelTWidth(wptr),
                             (int)EZ_WidgetJustification(wptr),
                             EZ_LabelFont(wptr), shadow);
          txtpos[0] = (short)x;
          txtpos[1] = (short)y - EZ_LabelFontAscent(wptr);
          txtpos[2] = (short)EZ_LabelTWidth(wptr);
          txtpos[3] = (short)EZ_LabelTHeight(wptr) * EZ_LabelNLines(wptr);
        }
    }
  /*============================================================
   *for complex Label with NW labels 
   */
  {
    EZ_Widget *children = EZ_WidgetChildren(wptr);
    if(children && EZ_WidgetType(children) == EZ_WIDGET_NW_LABEL)
      {
	EZ_DisplayNWLabel(children, pixmap, EZ_WidgetForeground(wptr), bgpv,
			  EZ_GetWidgetDisabledFlag(wptr),EZ_GetWidgetHighlightFlag(wptr),
			  0, 0, 0, 1);
      }
  }
  /*============================================================*/  

  EZ_DrawRectBorder(wptr, pixmap);     
  XCopyArea(EZ_Display,pixmap,win, EZ_WRITABLEGC,0,0,w,h,0,0); 
  XFreePixmap(EZ_Display, pixmap); 
}

void EZ_LabelEventHandle(widget, event)
     EZ_Widget *widget;
     XEvent *event;
{
  if(widget)
    {
      if(event->type == Expose)	EZ_DrawWidget(widget);      
      else if(event->type == ButtonPress)
	{
	  if(event->xbutton.button == EZ_Btn1)
	    { EZ_ExecuteWidgetCallBacks(widget);}
	  else if(event->xbutton.button == EZ_Btn3)
	    {
              /*
                #include "EZ_DnDHandler1.c"
                EZ_DnDEventHandler1(widget, event);
                */
              EZ_HandleDnDEvents(widget, event);
	    }
	}
      else if(event->type == KeyPress)
	{
	  if(EZ_LabelIsMenuTitle(widget)) 
	    EZ_HandleToplevelMenuEvents(widget, event);
	}
    }
}

/****************************************************************************/
#undef _EZ_WIDGET_LABEL_C_
